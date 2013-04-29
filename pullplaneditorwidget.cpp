
#include <QSpinBox>
#include <QHBoxLayout>
#include <QTabWidget>
#include <QPushButton>
#include <QStackedWidget>
#include <QScrollArea>
#include <QTimer>

#include "pullplan.h"
#include "geometry.h"
#include "pullplaneditorwidget.h"
#include "pullplaneditorviewwidget.h"
#include "pullplancustomizeviewwidget.h"
#include "niceviewwidget.h"
#include "pulltemplatelibrarywidget.h"
#include "pullplanlibrarywidget.h"
#include "mesh.h"
#include "dependancy.h"
#include "templateparameter.h"
#include "pullplangeometrythread.h"
#include "twistwidget.h"
#include "pullplancrosssectionrender.h"
#include "globalbackgroundcolor.h"
#include "globalgraphicssetting.h"

PullPlanEditorWidget :: PullPlanEditorWidget(QWidget* parent) : QWidget(parent)
{
	resetPullPlan();

	viewWidget = new PullPlanEditorViewWidget(plan, this);	
	customizeViewWidget = new PullPlanCustomizeViewWidget(plan, this);
	niceViewWidget = new NiceViewWidget(NiceViewWidget::PULLPLAN_CAMERA_MODE, this);
	niceViewWidget->setGeometry(&geometry);

	setupLayout();
	setupThreading();
	setupConnections();

	seedTemplates();
}

void PullPlanEditorWidget :: resetPullPlan()
{
	plan = new PullPlan(PullTemplate::HORIZONTAL_LINE_CIRCLE);
}

void PullPlanEditorWidget :: writePlanToOBJFile(QString& filename)
{
	geometry.save_obj_file(filename.toStdString());
}

void PullPlanEditorWidget :: writePlanToPLYFile(QString& filename)
{
	geometry.save_ply_file(filename.toStdString());
}

void PullPlanEditorWidget :: setupThreading()
{
	geometryDirty = false;
	tempPullPlan = deep_copy(plan);
	tempPullPlanDirty = true;
	geometryThread = new PullPlanGeometryThread(this);
	geometryThread->start();
}

void PullPlanEditorWidget :: updateEverything()
{
	// set casing buttons
	switch (plan->getOutermostCasingShape())
	{
		case CIRCLE_SHAPE:
			circleCasingPushButton->setDown(true);
			squareCasingPushButton->setDown(false);
			break;
		case SQUARE_SHAPE:
			circleCasingPushButton->setDown(false);
			squareCasingPushButton->setDown(true);
			break;
	}
	removeCasingButton->setEnabled(!plan->hasMinimumCasingCount());

	countSpin->setValue(plan->getCount());
	countLabel->setEnabled(plan->getTemplateType() != PullTemplate::CUSTOM);
	countSpin->setEnabled(plan->getTemplateType() != PullTemplate::CUSTOM);

	twistWidget->updateEverything();
	twistWidget->setEnabled(plan->getOutermostCasingShape() == CIRCLE_SHAPE);
	
	tempPullPlanMutex.lock();
	deep_delete(tempPullPlan);
	tempPullPlan = deep_copy(plan);
	tempPullPlanDirty = true;
	tempPullPlanMutex.unlock();

	QString message("Rendering cane...");
	emit showMessage(message, 0); // show until next message 
	wakeWait.wakeOne(); // wake up the thread if it's sleeping

	// Highlight correct pull template
	PullTemplateLibraryWidget* ptlw; 
	for (int i = 0; i < templateLibraryLayout->count(); ++i)
	{
		ptlw = dynamic_cast<PullTemplateLibraryWidget*>(
				dynamic_cast<QWidgetItem *>(templateLibraryLayout->itemAt(i))->widget());
		ptlw->setHighlighted(ptlw->type == plan->getTemplateType());
	}

	viewWidget->updateEverything();
	customizeViewWidget->updateEverything();
}

void PullPlanEditorWidget :: geometryThreadFinishedMesh(bool completed, unsigned int quality)
{
	geometryDirtyMutex.lock();
	bool dirty = geometryDirty;
	geometryDirtyMutex.unlock();
	if (!dirty)
		return;

	if (tempGeometryMutex.tryLock())
	{
		geometryDirtyMutex.lock();
		geometryDirty = false;
		geometryDirtyMutex.unlock();
		geometry.vertices = tempGeometry.vertices;
		geometry.triangles = tempGeometry.triangles;
		geometry.groups = tempGeometry.groups;
		tempGeometryMutex.unlock();

		niceViewWidget->repaint();
	}
	else
	{
		// try to get the lock again in 250 ms
		QTimer::singleShot(250, this, SLOT(geometryThreadFinishedMesh(completed, quality))); 
		return;
	}

	// report what happened if it's the high quality mesh
	if (quality != GlobalGraphicsSetting::VERY_HIGH)
		return;
	QString result;
	if (completed)
		emit showMessage("Cane rendered successfully.", 5);
	else
		emit showMessage("Cane is too complex to render completely.", 5);
}

void PullPlanEditorWidget :: setupLayout()
{
	// we use a grid layout, with the edit-y parts in the left column 
	// and 3D view in the right column 
	QGridLayout* editorLayout = new QGridLayout(this);
	this->setLayout(editorLayout);

	// build pair of editor views: regular and custom
	viewEditorStack = new QStackedWidget(this);
	viewEditorStack->addWidget(viewWidget);
	viewEditorStack->addWidget(customizeViewWidget);
	editorLayout->addWidget(viewEditorStack, 0, 0);

	// next is scrollable library of templates
	QWidget* templateLibraryWidget = new QWidget(this);
	templateLibraryLayout = new QHBoxLayout(templateLibraryWidget);
	templateLibraryLayout->setSpacing(10);
	templateLibraryLayout->setContentsMargins(10, 10, 10, 10);
	templateLibraryWidget->setLayout(templateLibraryLayout);

	QScrollArea* pullTemplateLibraryScrollArea = new QScrollArea(this);
	pullTemplateLibraryScrollArea->setBackgroundRole(QPalette::Dark);
	pullTemplateLibraryScrollArea->setWidget(templateLibraryWidget);
	pullTemplateLibraryScrollArea->setWidgetResizable(true);
	pullTemplateLibraryScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	pullTemplateLibraryScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	pullTemplateLibraryScrollArea->setFixedHeight(140);
	editorLayout->addWidget(pullTemplateLibraryScrollArea, 1, 0);



	// now the two tabs of controls, one for each GUI mode
	controlsTab = new QTabWidget(this);
	editorLayout->addWidget(controlsTab, 2, 0);


	// regular controls: casing, count, twist
	QWidget* tab1Widget = new QWidget(controlsTab);
	QVBoxLayout* tab1Layout = new QVBoxLayout(tab1Widget);
	tab1Widget->setLayout(tab1Layout);
	controlsTab->addTab(tab1Widget, "Fill and Case");

	QWidget* casingWidget = new QWidget(tab1Widget);
	QHBoxLayout* casingLayout = new QHBoxLayout(casingWidget);
	QLabel* casingLabel = new QLabel("Casing:", casingWidget);
	circleCasingPushButton = new QPushButton(casingWidget); 
	QIcon circleIcon(QPixmap::fromImage(QImage(":/images/circle_icon.png")));
	circleCasingPushButton->setIcon(circleIcon);
	squareCasingPushButton = new QPushButton(casingWidget); 
	QIcon squareIcon(QPixmap::fromImage(QImage(":/images/square_icon.png")));
	squareCasingPushButton->setIcon(squareIcon);
	addCasingButton = new QPushButton("+", casingWidget);
	removeCasingButton = new QPushButton("-", casingWidget);
	casingLayout->addWidget(casingLabel);
	casingLayout->addWidget(circleCasingPushButton);
	casingLayout->addWidget(squareCasingPushButton);
	casingLayout->addWidget(addCasingButton);
	casingLayout->addWidget(removeCasingButton);

	countLabel = new QLabel("Count:", casingWidget);
	countSpin = new QSpinBox(casingWidget);
	countSpin->setRange(0, 30);
	casingLayout->addStretch(1);
	casingLayout->addWidget(countLabel);
	casingLayout->addWidget(countSpin);

	twistWidget = new TwistWidget(&(plan->twist), 10, tab1Widget);

	tab1Layout->addWidget(casingWidget);
	tab1Layout->addWidget(twistWidget);
	tab1Layout->addStretch(1);


	// custom controls: add circle, square, duplicate selection, delete selection
	QWidget* tab2Widget = new QWidget(controlsTab);
	QVBoxLayout* tab2Layout = new QVBoxLayout(tab2Widget);
	tab2Widget->setLayout(tab2Layout);
	controlsTab->addTab(tab2Widget, "Customize Layout");

	QWidget* customControlsWidget = new QWidget(tab2Widget);
	QHBoxLayout* customControlsLayout = new QHBoxLayout(customControlsWidget);
	customControlsWidget->setLayout(customControlsLayout);
	addCircleButton = new QPushButton("Add Circle", customControlsWidget);
	addSquareButton = new QPushButton("Add Square", customControlsWidget);
	copySelectedButton = new QPushButton("Duplicate", customControlsWidget);
	deleteSelectedButton = new QPushButton("Delete", customControlsWidget);
	customControlsLayout->addStretch(1);
	customControlsLayout->addWidget(addCircleButton);
	customControlsLayout->addWidget(addSquareButton);
	customControlsLayout->addWidget(copySelectedButton);
	customControlsLayout->addWidget(deleteSelectedButton);
	customControlsLayout->addStretch(1);
	
	tab2Layout->addWidget(customControlsWidget);
	tab2Layout->addStretch(1);


	// below the tabs goes a labeled descriptor (changes depending on view)
	descriptionLabel = new QLabel("Cane editor - drag color or other canes in.", this);
	descriptionLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	editorLayout->addWidget(descriptionLabel, 3, 0);

	// at this point the editor GUI elements are done



	// now add the 3D view and its label
	editorLayout->addWidget(niceViewWidget, 0, 1, 3, 1);
	QLabel* niceViewDescriptionLabel = new QLabel("3D view of cane.", this);
	niceViewDescriptionLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	editorLayout->addWidget(niceViewDescriptionLabel, 3, 1);

	// set proportions of the various parts: 
	// horizontally, 60% is the editor, 40% is the 3D view
	editorLayout->setColumnStretch(0, 3);
	editorLayout->setColumnStretch(1, 2);

	// vertically, the editor view that the user manipulates/interacts with 
	// takes up all extra space
	// controls are compressed as much as possible 
	// (they are ugly and don't require much precision)
	editorLayout->setRowStretch(0, 10);
}

void PullPlanEditorWidget :: mousePressEvent(QMouseEvent* event)
{
	PullTemplateLibraryWidget* ptlw = dynamic_cast<PullTemplateLibraryWidget*>(childAt(event->pos()));

	if (ptlw != NULL)
	{
		if (ptlw->type == PullTemplate::CUSTOM)
		{
			// simulate user pressing customize tab
			controlsTab->setCurrentIndex(1);
		}
		else
		{
			// put the user back in fill and case mode,
			// as they're no longer working on a custom template
			controlsTab->setCurrentIndex(0);
			plan->setTemplateType(ptlw->type);	
			updateEverything();
			emit someDataChanged();
		}
	}
}

void PullPlanEditorWidget :: controlsTabChanged(int tab)
{
	// change the blueprint view to match the tab
	viewEditorStack->setCurrentIndex(tab); 

	if (tab == 0) // Fill and case mode
		descriptionLabel->setText("Cane editor - drag color or other canes in.");
	else // customize mode
	{
		plan->setTemplateType(PullTemplate::CUSTOM);				
		descriptionLabel->setText("Cane customizer - select and drag shapes around to customize cane layout.");
		updateEverything();
		emit someDataChanged();
	}
}

void PullPlanEditorWidget :: circleCasingButtonPressed()
{
	plan->setOutermostCasingShape(CIRCLE_SHAPE);
	updateEverything();
	emit someDataChanged();
}

void PullPlanEditorWidget :: squareCasingButtonPressed()
{
	plan->setOutermostCasingShape(SQUARE_SHAPE);
	updateEverything();
	emit someDataChanged();
}

void PullPlanEditorWidget :: removeCasingButtonPressed()
{
	plan->removeCasing();
	updateEverything();
	emit someDataChanged();
}

void PullPlanEditorWidget :: addCasingButtonPressed()
{
	plan->addCasing(plan->getOutermostCasingShape());
	updateEverything();
	emit someDataChanged();
}

void PullPlanEditorWidget :: copySelectedButtonPressed()
{
	customizeViewWidget->copySelectionPressed();
}

void PullPlanEditorWidget :: deleteSelectedButtonPressed()
{
	customizeViewWidget->deleteSelectionPressed();
}

void PullPlanEditorWidget :: addCircleButtonPressed()
{
	customizeViewWidget->addCirclePressed();
}

void PullPlanEditorWidget :: addSquareButtonPressed()
{
	customizeViewWidget->addSquarePressed();
}

void PullPlanEditorWidget :: setupConnections()
{
	// editor controls
	connect(circleCasingPushButton, SIGNAL(clicked()), this, SLOT(circleCasingButtonPressed()));
	connect(squareCasingPushButton, SIGNAL(clicked()), this, SLOT(squareCasingButtonPressed()));
	connect(addCasingButton, SIGNAL(clicked()), this, SLOT(addCasingButtonPressed()));
	connect(removeCasingButton, SIGNAL(clicked()), this, SLOT(removeCasingButtonPressed()));
	connect(copySelectedButton, SIGNAL(pressed()), this, SLOT(copySelectedButtonPressed()));
	connect(deleteSelectedButton, SIGNAL(pressed()), this, SLOT(deleteSelectedButtonPressed()));
	connect(addCircleButton, SIGNAL(pressed()), this, SLOT(addCircleButtonPressed()));
	connect(addSquareButton, SIGNAL(pressed()), this, SLOT(addSquareButtonPressed()));
	connect(twistWidget, SIGNAL(valueChanged()), this, SLOT(childWidgetDataChanged()));
	connect(countSpin, SIGNAL(valueChanged(int)), this, SLOT(countSpinChanged(int)));
	connect(controlsTab, SIGNAL(currentChanged(int)), this, SLOT(controlsTabChanged(int)));

	// subeditors
	connect(viewWidget, SIGNAL(someDataChanged()), this, SLOT(childWidgetDataChanged()));
	connect(customizeViewWidget, SIGNAL(someDataChanged()), this, SLOT(childWidgetDataChanged()));
	
	// render thread	
	connect(geometryThread, SIGNAL(finishedMesh(bool, unsigned int)), 
		this, SLOT(geometryThreadFinishedMesh(bool, unsigned int)));
}

void PullPlanEditorWidget :: childWidgetDataChanged()
{
	updateEverything();
	emit someDataChanged();
}

void PullPlanEditorWidget :: countSpinChanged(int)
{
	// update template
	unsigned int count = plan->getCount();
	if (count != static_cast<unsigned int>(countSpin->value()))
	{
		plan->setCount(countSpin->value());
		updateEverything();
		emit someDataChanged();
	}
}

void PullPlanEditorWidget :: seedTemplates()
{
	for (int i = PullTemplate::firstSeedTemplate(); i <= PullTemplate::lastSeedTemplate(); ++i)
	{
		PullTemplate::Type t = static_cast<PullTemplate::Type>(i);
		PullPlan plan(t);

		QPixmap templatePixmap(100, 100);
		templatePixmap.fill(GlobalBackgroundColor::qcolor);

		QPainter painter(&templatePixmap);
		PullPlanCrossSectionRender::render(&painter, 100, &plan);
		painter.end();

		PullTemplateLibraryWidget *ptlw = new PullTemplateLibraryWidget(templatePixmap, t);
		templateLibraryLayout->addWidget(ptlw);
	}

	// add the custom template last
	PullTemplateLibraryWidget *ptlw = new PullTemplateLibraryWidget(
		QPixmap::fromImage(QImage(":/images/pulltemplate_custom.png")), PullTemplate::CUSTOM);
	templateLibraryLayout->addWidget(ptlw);
}

void PullPlanEditorWidget :: setPullPlan(PullPlan* _plan)
{
	plan = _plan;
	controlsTab->setCurrentIndex(0);
	updateEverything();
	twistWidget->setTwist(&(plan->twist));
	viewWidget->setPullPlan(plan);
	customizeViewWidget->setPullPlan(plan);
}

PullPlan* PullPlanEditorWidget :: getPullPlan()
{
	return plan;
}


