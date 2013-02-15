
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

	double twist = plan->twist;
	twistSpin->setValue(twist);
	twistSlider->setSliderPosition(twist*10);
	twistSpin->setEnabled(plan->getOutermostCasingShape() == CIRCLE_SHAPE);
	twistSlider->setEnabled(plan->getOutermostCasingShape() == CIRCLE_SHAPE);

	viewWidget->setPullPlan(plan);
	customizeViewWidget->setPullPlan(plan);

	// we always keep the first parameter shown so that the stack widget
	// occupies the correct region, even if the template has no parameters
	// parameters 2..k are hidden, though
	if (plan->getParameterCount() > 0)
		paramStack->setCurrentIndex(0);
	else 
		paramStack->setCurrentIndex(1);
	
	unsigned int i = 0;
	for (; i < plan->getParameterCount(); ++i)
	{
		TemplateParameter tp;
		plan->getParameter(i, &tp);
		paramLabels[i]->setText(tp.name.c_str());
		disconnect(paramSpins[i], SIGNAL(valueChanged(int)), this, SLOT(paramSpinChanged(int)));
		paramSpins[i]->setRange(tp.lowerLimit, tp.upperLimit);
		connect(paramSpins[i], SIGNAL(valueChanged(int)), this, SLOT(paramSpinChanged(int)));
		paramSpins[i]->setValue(tp.value);
		paramSliders[i]->setRange(tp.lowerLimit, tp.upperLimit);
		paramSliders[i]->setValue(tp.value);
		if (i != 0)
		{
			paramLabels[i]->show();
			paramSpins[i]->show();
			paramSliders[i]->show();
		}
	}
	for (; i < paramLabels.size(); ++i)
	{
		if (i != 0)
		{
			paramLabels[i]->hide();
			paramSpins[i]->hide();
			paramSliders[i]->hide();
		}
	}

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
		if (ptlw->type == plan->getTemplateType())
			ptlw->setDependancy(true, IS_DEPENDANCY);
		else
		        ptlw->setDependancy(false);
	}
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
	// Editor layout
	QWidget* editorWidget = new QWidget(this);
	QVBoxLayout* editorLayout = new QVBoxLayout(editorWidget);
	editorWidget->setLayout(editorLayout);

	// Editor layout: interactive top-down visualization
	editorLayout->addWidget(viewWidget, 1);

	// Editor layout: pull template scrolling library
	QWidget* templateLibraryWidget = new QWidget(editorWidget);
	templateLibraryLayout = new QHBoxLayout(templateLibraryWidget);
	templateLibraryLayout->setSpacing(10);
	templateLibraryLayout->setContentsMargins(10, 10, 10, 10);
	templateLibraryWidget->setLayout(templateLibraryLayout);

	QScrollArea* pullTemplateLibraryScrollArea = new QScrollArea(editorWidget);
	pullTemplateLibraryScrollArea->setBackgroundRole(QPalette::Dark);
	pullTemplateLibraryScrollArea->setWidget(templateLibraryWidget);
	pullTemplateLibraryScrollArea->setWidgetResizable(true);
	pullTemplateLibraryScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	pullTemplateLibraryScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	pullTemplateLibraryScrollArea->setFixedHeight(140);
	editorLayout->addWidget(pullTemplateLibraryScrollArea, 0);

	// Editor layout: casing buttons layout
	QHBoxLayout* pullTemplateShapeLayout = new QHBoxLayout(editorWidget);
	QLabel* casingLabel = new QLabel("Casing:");
	circleCasingPushButton = new QPushButton(QString(QChar(9673))); 
	squareCasingPushButton = new QPushButton(QString(QChar(9635)));
	addCasingButton = new QPushButton("+");
	removeCasingButton = new QPushButton("-");
	pullTemplateShapeLayout->addWidget(casingLabel);
	pullTemplateShapeLayout->addWidget(circleCasingPushButton);
	pullTemplateShapeLayout->addWidget(squareCasingPushButton);
	pullTemplateShapeLayout->addWidget(addCasingButton);
	pullTemplateShapeLayout->addWidget(removeCasingButton);
	pullTemplateShapeLayout->addStretch(1);
	editorLayout->addLayout(pullTemplateShapeLayout, 0);

	// Editor layout: twist layout
	QHBoxLayout* twistLayout = new QHBoxLayout(editorWidget);
	editorLayout->addLayout(twistLayout, 0);

	QLabel* twistLabel1 = new QLabel("Twist:", editorWidget);
	twistLayout->addWidget(twistLabel1);

	twistSpin = new QDoubleSpinBox(editorWidget);
	twistSpin->setRange(-10.0, 10.0);
	twistSpin->setSingleStep(0.1);
	twistSpin->setDecimals(1);
	twistLayout->addWidget(twistSpin, 1);

	QLabel* twistLabel2 = new QLabel("-10.0", editorWidget);
	twistLayout->addWidget(twistLabel2);

	twistSlider = new QSlider(Qt::Horizontal, editorWidget);
	twistSlider->setRange(-100, 100);
	twistSlider->setSingleStep(1);
	twistSlider->setTickInterval(100);	
	twistSlider->setTickPosition(QSlider::TicksBothSides);
	twistLayout->addWidget(twistSlider, 10);

	QLabel* twistLabel3 = new QLabel("10.0", editorWidget);
	twistLayout->addWidget(twistLabel3);

	// Editor layout: parameter spin stuff
	paramStack = new QStackedWidget(editorWidget); 
	QWidget* paramWidget = new QWidget(paramStack);
	paramStack->addWidget(paramWidget);
	QHBoxLayout* paramLayout = new QHBoxLayout(paramWidget);
	paramWidget->setLayout(paramLayout);
	paramLayout->setContentsMargins(0, 0, 0, 0);
	paramLabels.push_back(new QLabel("Param 1:", paramWidget));
	paramLabels.push_back(new QLabel("Param 2:", paramWidget));
	paramLabels.push_back(new QLabel("Param 3:", paramWidget));
	paramSpins.push_back(new QSpinBox(paramWidget));
	paramSpins.push_back(new QSpinBox(paramWidget));
	paramSpins.push_back(new QSpinBox(paramWidget));
	paramSliders.push_back(new QSlider(Qt::Horizontal, paramWidget));
	paramSliders.push_back(new QSlider(Qt::Horizontal, paramWidget));
	paramSliders.push_back(new QSlider(Qt::Horizontal, paramWidget));
	for (unsigned int i = 0; i < paramLabels.size(); ++i)
	{
		paramLayout->addWidget(paramLabels[i]);
		paramLayout->addWidget(paramSpins[i]);
		paramLayout->addWidget(paramSliders[i]);
	}
	paramStack->addWidget(new QWidget(paramStack));
	editorLayout->addWidget(paramStack);	

	// Customize layout
	QWidget* customizeWidget = new QWidget(this);
	QVBoxLayout* customizeLayout = new QVBoxLayout(customizeWidget);
	customizeWidget->setLayout(customizeLayout);

	// Customize layout: interactive editor
	customizeLayout->addWidget(customizeViewWidget, 1);

	// Customize layout: add cane layout
	QHBoxLayout* addCaneLayout = new QHBoxLayout(customizeWidget);
	addCircleButton = new QPushButton("Add New Circle");
	addSquareButton = new QPushButton("Add New Square");
	addCaneLayout->addWidget(addCircleButton);
	addCaneLayout->addWidget(addSquareButton);
	addCaneLayout->addStretch();
	customizeLayout->addLayout(addCaneLayout, 0);

	// Customize layout: duplicate/delete cane layout
	QHBoxLayout* windowControlsLayout = new QHBoxLayout(customizeWidget);
	copySelectedButton = new QPushButton("Duplicate Selection");
	deleteSelectedButton = new QPushButton("Delete Selection");
	confirmChangesButton = new QPushButton("Confirm changes");
	cancelChangesButton = new QPushButton("Cancel");
	windowControlsLayout->addWidget(copySelectedButton);
	windowControlsLayout->addWidget(deleteSelectedButton);
	windowControlsLayout->addStretch();
	customizeLayout->addLayout(windowControlsLayout, 0);
	
	// Combine editor and customize layouts into a pair of tabs with 
	// descriptive text in the left layout
	QWidget* leftWidget = new QWidget(this);
	QVBoxLayout* leftLayout = new QVBoxLayout(leftWidget);
	leftWidget->setLayout(leftLayout);
	tabs = new QTabWidget(leftWidget);
	tabs->addTab(editorWidget, QString("Fill Arrangement"));
	tabs->addTab(customizeWidget, QString("Rearrange Current"));
	leftLayout->addWidget(tabs, 1);
	descriptionLabel = new QLabel("Cane editor - drag color or other canes in.", leftWidget);
	descriptionLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	leftLayout->addWidget(descriptionLabel, 0);

	// Combine 3D viewer and descriptive text into the right layout
	QWidget* rightWidget = new QWidget(this);
	QVBoxLayout* rightLayout = new QVBoxLayout(rightWidget);
	rightWidget->setLayout(rightLayout);
	rightLayout->addWidget(niceViewWidget, 1);
	QLabel* niceViewDescriptionLabel = new QLabel("3D view of cane.", rightWidget);
	niceViewDescriptionLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	rightLayout->addWidget(niceViewDescriptionLabel, 0);

	// Combine left and right layouts
	QHBoxLayout* pageLayout = new QHBoxLayout(this);
	this->setLayout(pageLayout);
	pageLayout->addWidget(leftWidget, 1);
	pageLayout->addWidget(rightWidget, 1);
}

void PullPlanEditorWidget :: mousePressEvent(QMouseEvent* event)
{
	PullTemplateLibraryWidget* ptlw = dynamic_cast<PullTemplateLibraryWidget*>(childAt(event->pos()));

	if (ptlw != NULL)
	{
		plan->setTemplateType(ptlw->type);
		emit someDataChanged();
	}
}

void PullPlanEditorWidget :: tabChanged(int i)
{
	if (i == 0)
		descriptionLabel->setText("Cane editor - drag color or other canes in.");
	else
		descriptionLabel->setText("Cane customizer - select and drag shapes around to customize cane layout.");
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
	connect(twistSlider, SIGNAL(valueChanged(int)), this, SLOT(twistSliderChanged(int)));
	connect(twistSpin, SIGNAL(valueChanged(double)), this, SLOT(twistSpinChanged(double)));
	for (unsigned int i = 0; i < paramSpins.size(); ++i)
	{
		connect(paramSpins[i], SIGNAL(valueChanged(int)), this, SLOT(paramSpinChanged(int)));
		connect(paramSliders[i], SIGNAL(valueChanged(int)), this, SLOT(paramSliderChanged(int)));
	}

	connect(geometryThread, SIGNAL(finishedMesh()), this, SLOT(geometryThreadFinishedMesh()));
	connect(this, SIGNAL(someDataChanged()), this, SLOT(updateEverything()));
	connect(viewWidget, SIGNAL(someDataChanged()), this, SLOT(viewWidgetDataChanged()));
	connect(customizeViewWidget, SIGNAL(someDataChanged()), this, SLOT(customizeViewWidgetDataChanged()));
	connect(tabs, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)));
}

void PullPlanEditorWidget :: viewWidgetDataChanged()
{
	emit someDataChanged();
}

void PullPlanEditorWidget :: customizeViewWidgetDataChanged()
{
	emit someDataChanged();
}

void PullPlanEditorWidget :: paramSpinChanged(int)
{
	// update template
	for (unsigned int i = 0; i < plan->getParameterCount(); ++i)
	{
		TemplateParameter tp;
		plan->getParameter(i, &tp);
		if (tp.value != paramSpins[i]->value())
		{
			plan->setParameter(i, paramSpins[i]->value());
			emit someDataChanged();
		}
	}
}

void PullPlanEditorWidget :: paramSliderChanged(int)
{
	// update template
	for (unsigned int i = 0; i < plan->getParameterCount(); ++i)
	{
		TemplateParameter tp;
		plan->getParameter(i, &tp);
		if (tp.value != paramSliders[i]->value())
		{
			plan->setParameter(i, paramSliders[i]->value());
			emit someDataChanged();
		}
	}
}

void PullPlanEditorWidget :: twistSliderChanged(int)
{
	float twist = twistSlider->value() / 10.0;

	if (twist == plan->twist)
		return;
	plan->twist = twist;
	emit someDataChanged();
}

void PullPlanEditorWidget :: twistSpinChanged(double)
{
	double twist = twistSpin->value();

	if (twist == plan->twist)
		return;
	plan->twist = twist;
	emit someDataChanged();
}

void PullPlanEditorWidget :: seedTemplates()
{
	char filename[100];

	for (int i = PullTemplate::firstSeedTemplate(); i <= PullTemplate::lastSeedTemplate(); ++i)
	{
		sprintf(filename, ":/images/pulltemplate%d.png", i);
		PullTemplateLibraryWidget *ptlw = new PullTemplateLibraryWidget(
			QPixmap::fromImage(QImage(filename)), static_cast<PullTemplate::Type>(i));
		templateLibraryLayout->addWidget(ptlw);
	}
}

void PullPlanEditorWidget :: setPlan(PullPlan* p)
{
	plan = p;
	emit someDataChanged();	
}

PullPlan* PullPlanEditorWidget :: getPlan()
{
	return plan;
}
