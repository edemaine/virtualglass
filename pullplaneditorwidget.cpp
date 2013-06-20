
#include <QSpinBox>
#include <QHBoxLayout>
#include <QTabWidget>
#include <QPushButton>
#include <QStackedWidget>
#include <QScrollArea>
#include <QTimer>
#include <QApplication>
#include <QScrollBar>

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
#include "constants.h"

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

QImage PullPlanEditorWidget :: pullPlanImage()
{
	return niceViewWidget->grabFrameBuffer();
}

void PullPlanEditorWidget :: reset3DCamera()
{
	niceViewWidget->resetCamera();
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
	switch (plan->outermostCasingShape())
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

	countSpin->setValue(plan->count());
	countLabel->setEnabled(plan->templateType() != PullTemplate::CUSTOM);
	countSpin->setEnabled(plan->templateType() != PullTemplate::CUSTOM);

	twistWidget->updateEverything();
	twistWidget->setEnabled(plan->outermostCasingShape() == CIRCLE_SHAPE);
	
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
		ptlw->setHighlighted(ptlw->type == plan->templateType());
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
		emit showMessage("Cane rendered successfully.", 3);
	else
		emit showMessage("Cane is too complex to render completely.", 3);
}

void PullPlanEditorWidget :: setupLayout()
{
	// we use a grid layout, with the edit-y parts in the left column 
	// and 3D view in the right column 
	QGridLayout* editorLayout = new QGridLayout(this);
	this->setLayout(editorLayout);
	editorLayout->setContentsMargins(0, 0, 0, 0);
	editorLayout->setSpacing(10);

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

	pullTemplateLibraryScrollArea = new QScrollArea(this);
	pullTemplateLibraryScrollArea->setBackgroundRole(QPalette::Dark);
	pullTemplateLibraryScrollArea->setWidget(templateLibraryWidget);
	pullTemplateLibraryScrollArea->setWidgetResizable(true);
	pullTemplateLibraryScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
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
	countSpin->setRange(MIN_PULLPLAN_COUNT_PARAMETER_VALUE, MAX_PULLPLAN_COUNT_PARAMETER_VALUE);
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
	controlsTab->addTab(tab2Widget, "Customize");

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


	// now add the 3D view 
	editorLayout->addWidget(niceViewWidget, 0, 1, 3, 1);

	// at this point the editor GUI elements are done


	// set proportions of the various parts: 
	// horizontally, 60% is the editor, 40% is the 3D view
	editorLayout->setColumnStretch(0, 3);
	editorLayout->setColumnStretch(1, 2);

	// vertically, the editor view that the user manipulates/interacts with 
	// takes up all extra space
	// controls are comclicked as much as possible 
	// (they are ugly and don't require much precision)
	editorLayout->setRowStretch(0, 10);
}


void PullPlanEditorWidget :: controlsTabChanged(int tab)
{
	// change the blueprint view to match the tab
	viewEditorStack->setCurrentIndex(tab); 

	if (tab != 0) // customize mode	
	{
		plan->setTemplateType(PullTemplate::CUSTOM);				
		updateEverything();
		emit someDataChanged();
	}
}

void PullPlanEditorWidget :: circleCasingButtonClicked()
{
	plan->setOutermostCasingShape(CIRCLE_SHAPE);
	updateEverything();
	emit someDataChanged();
}

void PullPlanEditorWidget :: squareCasingButtonClicked()
{
	plan->setOutermostCasingShape(SQUARE_SHAPE);
	updateEverything();
	emit someDataChanged();
}

void PullPlanEditorWidget :: removeCasingButtonClicked()
{
	plan->removeCasing();
	updateEverything();
	emit someDataChanged();
}

void PullPlanEditorWidget :: addCasingButtonClicked()
{
	plan->addCasing(plan->outermostCasingShape());
	updateEverything();
	emit someDataChanged();
}

void PullPlanEditorWidget :: copySelectedButtonClicked()
{
	customizeViewWidget->copySelectionClicked();
}

void PullPlanEditorWidget :: deleteSelectedButtonClicked()
{
	customizeViewWidget->deleteSelectionClicked();
}

void PullPlanEditorWidget :: addCircleButtonClicked()
{
	customizeViewWidget->addCircleClicked();
}

void PullPlanEditorWidget :: addSquareButtonClicked()
{
	customizeViewWidget->addSquareClicked();
}
	
void PullPlanEditorWidget :: mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton && pullTemplateLibraryScrollArea->geometry().contains(event->pos()))
	{
		isDragging = true;
		lastDragPosition = dragStartPosition = event->pos();
		maxDragDistance = 0;
	}
	else
		isDragging = false;
}

void PullPlanEditorWidget :: mouseMoveEvent(QMouseEvent* event)
{
	// If the left mouse button isn't down
	if ((event->buttons() & Qt::LeftButton) == 0)
	{
		isDragging = false;
		return;
	}

	maxDragDistance = MAX(maxDragDistance, fabs(event->pos().x() - dragStartPosition.x()));
	if (!isDragging || maxDragDistance < QApplication::startDragDistance())
		return;

	int movement = event->pos().x() - lastDragPosition.x();
	pullTemplateLibraryScrollArea->horizontalScrollBar()->setValue(
		pullTemplateLibraryScrollArea->horizontalScrollBar()->value() - movement);
	lastDragPosition = event->pos();
}

void PullPlanEditorWidget :: mouseReleaseEvent(QMouseEvent* event)
{
	// If not dragging or dragging caused a scroll
	if (!isDragging || (isDragging && maxDragDistance >= QApplication::startDragDistance()))
		return;

	PullTemplateLibraryWidget* ptlw = dynamic_cast<PullTemplateLibraryWidget*>(childAt(event->pos()));

	if (ptlw == NULL)
		return;

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

void PullPlanEditorWidget :: setupConnections()
{
	// editor controls
	connect(circleCasingPushButton, SIGNAL(clicked()), this, SLOT(circleCasingButtonClicked()));
	connect(squareCasingPushButton, SIGNAL(clicked()), this, SLOT(squareCasingButtonClicked()));
	connect(addCasingButton, SIGNAL(clicked()), this, SLOT(addCasingButtonClicked()));
	connect(removeCasingButton, SIGNAL(clicked()), this, SLOT(removeCasingButtonClicked()));
	connect(copySelectedButton, SIGNAL(clicked()), this, SLOT(copySelectedButtonClicked()));
	connect(deleteSelectedButton, SIGNAL(clicked()), this, SLOT(deleteSelectedButtonClicked()));
	connect(addCircleButton, SIGNAL(clicked()), this, SLOT(addCircleButtonClicked()));
	connect(addSquareButton, SIGNAL(clicked()), this, SLOT(addSquareButtonClicked()));
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
	unsigned int count = plan->count();
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

PullPlan* PullPlanEditorWidget :: pullPlan()
{
	return plan;
}


