
#include <QSpinBox>
#include <QHBoxLayout>
#include <QTabWidget>
#include <QPushButton>
#include <QStackedWidget>
#include <QScrollArea>

#include "pullplan.h"
#include "geometry.h"
#include "pullplaneditorwidget.h"
#include "pullplaneditorviewwidget.h"
#include "pullplancustomizeviewwidget.h"
#include "niceviewwidget.h"
#include "pulltemplatelibrarywidget.h"
#include "asyncpullplanlibrarywidget.h"
#include "mesh.h"
#include "dependancy.h"
#include "templateparameter.h"
#include "pullplangeometrythread.h"
#include "twistwidget.h"

PullPlanEditorWidget :: PullPlanEditorWidget(QWidget* parent) : QWidget(parent)
{
	resetPlan();

	viewWidget = new PullPlanEditorViewWidget(plan, this);	
	customizeViewWidget = new PullPlanCustomizeViewWidget(plan, this);
	niceViewWidget = new NiceViewWidget(NiceViewWidget::PULLPLAN_CAMERA_MODE, this);
	niceViewWidget->setGeometry(&geometry);

	setupLayout();
	setupThreading();
	setupConnections();
}

void PullPlanEditorWidget :: resetPlan()
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

	twistWidget->setEnabled(plan->getOutermostCasingShape() == CIRCLE_SHAPE);

	tempPullPlanMutex.lock();
	deep_delete(tempPullPlan);
	tempPullPlan = deep_copy(plan);
	tempPullPlanDirty = true;
	tempPullPlanMutex.unlock();
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

void PullPlanEditorWidget :: geometryThreadFinishedMesh()
{
	geometryDirtyMutex.lock();
	bool dirty = geometryDirty;
	geometryDirtyMutex.unlock();
	if (!dirty)
		return;

	if (tempGeometry1Mutex.tryLock())
	{
		geometryDirtyMutex.lock();
		geometryDirty = false;
		geometryDirtyMutex.unlock();
		geometry.vertices = tempGeometry1.vertices;
		geometry.triangles = tempGeometry1.triangles;
		geometry.groups = tempGeometry1.groups;
		tempGeometry1Mutex.unlock();
	}
	else if (tempGeometry2Mutex.tryLock())
	{
		geometryDirtyMutex.lock();
		geometryDirty = false;
		geometryDirtyMutex.unlock();
		geometry.vertices = tempGeometry2.vertices;
		geometry.triangles = tempGeometry2.triangles;
		geometry.groups = tempGeometry2.groups;
		tempGeometry2Mutex.unlock();

	}

	niceViewWidget->repaint();
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

	// this is the regular view one, with casing, count, and twist controls

	// casing controls
	QWidget* casingWidget = new QWidget(this);
	QHBoxLayout* casingLayout = new QHBoxLayout(casingWidget);
	QLabel* casingLabel = new QLabel("Casing:");
	circleCasingPushButton = new QPushButton(QString(QChar(9673))); 
	squareCasingPushButton = new QPushButton(QString(QChar(9635)));
	addCasingButton = new QPushButton("+");
	removeCasingButton = new QPushButton("-");
	casingLayout->addWidget(casingLabel);
	casingLayout->addWidget(circleCasingPushButton);
	casingLayout->addWidget(squareCasingPushButton);
	casingLayout->addWidget(addCasingButton);
	casingLayout->addWidget(removeCasingButton);

	// count controls
	countLabel = new QLabel("Count:", casingWidget);
	countSpin = new QSpinBox(casingWidget);
	casingLayout->addStretch(1);
	casingLayout->addWidget(countLabel);
	casingLayout->addWidget(countSpin);

	// prepackaged twist controls
	twistWidget = new TwistWidget(&(plan->twist), 10, this);

	// combine controls into a widget for the first tab
	QWidget* tab1Widget = new QWidget(this);
	QVBoxLayout* tab1Layout = new QVBoxLayout(tab1Widget);
	tab1Widget->setLayout(tab1Layout);
	tab1Layout->addWidget(casingWidget);
	tab1Layout->addWidget(twistWidget);
	tab1Layout->addStretch(1);
	controlsTab->addTab(tab1Widget, "Fill and Case");

	// next build the controls for the custom view
	QWidget* customControlsWidget = new QWidget(this);
	QHBoxLayout* customControlsLayout = new QHBoxLayout(customControlsWidget);
	customControlsWidget->setLayout(customControlsLayout);
	addCircleButton = new QPushButton("Add Circle");
	addSquareButton = new QPushButton("Add Square");
	copySelectedButton = new QPushButton("Duplicate");
	deleteSelectedButton = new QPushButton("Delete");
	customControlsLayout->addStretch(1);
	customControlsLayout->addWidget(addCircleButton);
	customControlsLayout->addWidget(addSquareButton);
	customControlsLayout->addWidget(copySelectedButton);
	customControlsLayout->addWidget(deleteSelectedButton);
	customControlsLayout->addStretch(1);
	
	// combine controls into a widget for the second tab
	QWidget* tab2Widget = new QWidget(this);
	QVBoxLayout* tab2Layout = new QVBoxLayout(tab2Widget);
	tab2Widget->setLayout(tab2Layout);
	tab2Layout->addWidget(customControlsWidget);
	tab2Layout->addStretch(1);
	controlsTab->addTab(tab2Widget, "Customize Layout");
	editorLayout->addWidget(controlsTab, 2, 0);

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
		emit someDataChanged();
	}
}

void PullPlanEditorWidget :: circleCasingButtonPressed()
{
	plan->setOutermostCasingShape(CIRCLE_SHAPE);
	emit someDataChanged(); 
}

void PullPlanEditorWidget :: squareCasingButtonPressed()
{
	plan->setOutermostCasingShape(SQUARE_SHAPE);
	emit someDataChanged();
}

void PullPlanEditorWidget :: removeCasingButtonPressed()
{
	plan->removeCasing();
	emit someDataChanged();
}

void PullPlanEditorWidget :: addCasingButtonPressed()
{
	plan->addCasing(plan->getOutermostCasingShape());
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
	connect(geometryThread, SIGNAL(finishedMesh()), this, SLOT(geometryThreadFinishedMesh()));
	connect(this, SIGNAL(someDataChanged()), this, SLOT(updateEverything()));
	connect(viewWidget, SIGNAL(someDataChanged()), this, SLOT(childWidgetDataChanged()));
	connect(customizeViewWidget, SIGNAL(someDataChanged()), this, SLOT(childWidgetDataChanged()));
	connect(controlsTab, SIGNAL(currentChanged(int)), this, SLOT(controlsTabChanged(int)));
}

void PullPlanEditorWidget :: childWidgetDataChanged()
{
	emit someDataChanged();
}

void PullPlanEditorWidget :: countSpinChanged(int)
{
	// update template
	unsigned int count = plan->getCount();
	if (count != static_cast<unsigned int>(countSpin->value()))
	{
		plan->setCount(countSpin->value());
		emit someDataChanged();
	}
}

void PullPlanEditorWidget :: seedTemplates()
{
	char filename[100];
	
	// read image filenames according to some correspondence:
	// e.g. the 7th pull template's image filename is "pulltemplate7.png" 
	for (int i = PullTemplate::firstSeedTemplate(); i <= PullTemplate::lastSeedTemplate(); ++i)
	{
		sprintf(filename, ":/images/pulltemplate%d.png", i);
		PullTemplateLibraryWidget *ptlw = new PullTemplateLibraryWidget(
			QPixmap::fromImage(QImage(filename)), static_cast<PullTemplate::Type>(i));
		templateLibraryLayout->addWidget(ptlw);
	}

	// add the custom template last
	PullTemplateLibraryWidget *ptlw = new PullTemplateLibraryWidget(
		QPixmap::fromImage(QImage(":/images/pulltemplate_custom.png")), PullTemplate::CUSTOM);
	templateLibraryLayout->addWidget(ptlw);
}

void PullPlanEditorWidget :: setPlan(PullPlan* p)
{
	plan = p;
	twistWidget->setTwist(&(plan->twist));
	viewWidget->setPullPlan(plan);
	customizeViewWidget->setPullPlan(plan);
	emit someDataChanged();	
}

PullPlan* PullPlanEditorWidget :: getPlan()
{
	return plan;
}
