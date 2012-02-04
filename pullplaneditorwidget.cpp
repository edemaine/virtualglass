
#include "pullplaneditorwidget.h"

PullPlanEditorWidget :: PullPlanEditorWidget(QWidget* parent) : QWidget(parent)
{
	this->plan = new PullPlan(CIRCLE_BASE_PULL_TEMPLATE);
	this->plan->setTemplateType(CASED_CIRCLE_PULL_TEMPLATE);

	this->viewWidget = new PullPlanEditorViewWidget(plan, this);	
	this->niceViewWidget = new NiceViewWidget(PULLPLAN_MODE, this);
	niceViewWidget->setGeometry(&geometry);
	this->pullPlanCustomizeWidget = new PullPlanCustomizeWidget(this->getPlan());
	this->pullPlanCustomizeWidget->hide();

	setupLayout();
	setupConnections();
}

void PullPlanEditorWidget :: updateEverything()
{
	static_cast<QCheckBox*>(fillRuleButtonGroup->button(
		viewWidget->getFillRule()))->setCheckState(Qt::Checked);

	static_cast<QCheckBox*>(shapeButtonGroup->button(
		plan->getOutermostCasingShape()))->setCheckState(Qt::Checked);

	int twist = plan->getTwist();
	twistSlider->setSliderPosition(twist);
	twistSpin->setValue(twist);
	twistSlider->setEnabled(!plan->hasSquareCasing());
	twistSpin->setEnabled(!plan->hasSquareCasing());

	viewWidget->setPullPlan(plan);
	viewWidget->repaint();

	unsigned int i = 0;
	for (; i < plan->getParameterCount(); ++i)
	{
		paramLabels[i]->setText(plan->getParameterName(i));
		paramLabels[i]->show();
		paramSpins[i]->setValue(plan->getParameter(i));
		paramSpins[i]->show();
	}
	for (; i < paramLabels.size(); ++i)
	{
		paramLabels[i]->hide();
		paramSpins[i]->hide();
	}

	geometry.clear();
	mesher.generatePullMesh(plan, &geometry);
	niceViewWidget->repaint();
	//geometry.save_raw_file("./cane.raw");

	// Highlight correct pull template
	for (int i = 0; i < templateLibraryLayout->count(); ++i)
	{
		if (i + FIRST_PULL_TEMPLATE == plan->getTemplateType())
			highlightLibraryWidget(dynamic_cast<PullTemplateLibraryWidget*>(
				dynamic_cast<QWidgetItem *>(templateLibraryLayout->itemAt(i))->widget()));
		else
			unhighlightLibraryWidget(dynamic_cast<PullTemplateLibraryWidget*>(
				dynamic_cast<QWidgetItem *>(templateLibraryLayout->itemAt(i))->widget()));
	}
}

void PullPlanEditorWidget :: unhighlightLibraryWidget(PullTemplateLibraryWidget* w)
{
	w->graphicsEffect()->setEnabled(false);
}

void PullPlanEditorWidget :: highlightLibraryWidget(PullTemplateLibraryWidget* w)
{
	w->graphicsEffect()->setEnabled(false);
	((QGraphicsHighlightEffect*) w->graphicsEffect())->setHighlightType(IS_DEPENDANCY);
	w->graphicsEffect()->setEnabled(true);
}

void PullPlanEditorWidget :: setupLayout()
{
	// Setup the editor layout
	QHBoxLayout* pageLayout = new QHBoxLayout(this);
	this->setLayout(pageLayout);
	QVBoxLayout* editorLayout = new QVBoxLayout(this);
	pageLayout->addLayout(editorLayout);

	editorLayout->addWidget(viewWidget, 0);

	QHBoxLayout* fillRuleLayout = new QHBoxLayout(this);
	fillRuleLayout->addWidget(new QLabel("Fill rule:", this), 1);
	QCheckBox* singleCheckBox = new QCheckBox("Single");
	QCheckBox* eoCheckBox = new QCheckBox("Every other");
	QCheckBox* etCheckBox = new QCheckBox("Every third");
	QCheckBox* gCheckBox = new QCheckBox("Group");
	QCheckBox* aCheckBox = new QCheckBox("All");
	fillRuleButtonGroup = new QButtonGroup();
	fillRuleButtonGroup->addButton(singleCheckBox, 1);
	fillRuleButtonGroup->addButton(eoCheckBox, 2);
	fillRuleButtonGroup->addButton(etCheckBox, 3);
	fillRuleButtonGroup->addButton(gCheckBox, 4);
	fillRuleButtonGroup->addButton(aCheckBox, 5);
	fillRuleLayout->addWidget(singleCheckBox, 1);
	fillRuleLayout->addWidget(eoCheckBox, 1);
	fillRuleLayout->addWidget(etCheckBox, 1);
	fillRuleLayout->addWidget(gCheckBox, 1);
	fillRuleLayout->addWidget(aCheckBox, 1);
	editorLayout->addLayout(fillRuleLayout, 1);
	
	// Setup pull template scrolling library
	QWidget* templateLibraryWidget = new QWidget(this);
	templateLibraryLayout = new QHBoxLayout(templateLibraryWidget);
	templateLibraryLayout->setSpacing(10);
	templateLibraryWidget->setLayout(templateLibraryLayout);

	QScrollArea* pullTemplateLibraryScrollArea = new QScrollArea(this);
	pullTemplateLibraryScrollArea->setBackgroundRole(QPalette::Dark);
	pullTemplateLibraryScrollArea->setWidget(templateLibraryWidget);
	pullTemplateLibraryScrollArea->setWidgetResizable(true);
	pullTemplateLibraryScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	pullTemplateLibraryScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	pullTemplateLibraryScrollArea->setFixedHeight(130);
	editorLayout->addWidget(pullTemplateLibraryScrollArea, 0);

	QHBoxLayout* pullTemplateShapeLayout = new QHBoxLayout(this);
	QLabel* casingLabel = new QLabel("Casing:");
	QCheckBox* circleCheckBox = new QCheckBox("Circle");
	QCheckBox* squareCheckBox = new QCheckBox("Square");
	addCasingButton = new QPushButton("Add Casing");
	removeCasingButton = new QPushButton("Remove Casing");
	shapeButtonGroup = new QButtonGroup();
	shapeButtonGroup->addButton(circleCheckBox, 1);
	shapeButtonGroup->addButton(squareCheckBox, 2);
	pullTemplateShapeLayout->addWidget(casingLabel);
	pullTemplateShapeLayout->addWidget(circleCheckBox);
	pullTemplateShapeLayout->addWidget(squareCheckBox);
	pullTemplateShapeLayout->addWidget(addCasingButton);
	pullTemplateShapeLayout->addWidget(removeCasingButton);
	editorLayout->addLayout(pullTemplateShapeLayout, 0);

	// Twist slider stuff
	QHBoxLayout* twistLayout = new QHBoxLayout(this);
	editorLayout->addLayout(twistLayout, 0);

	QLabel* twistLabel1 = new QLabel("Twist:", this);
	twistLayout->addWidget(twistLabel1);

	twistSpin = new QSpinBox(this);
	twistSpin->setRange(0, 100);
	twistSpin->setSingleStep(1);
	twistLayout->addWidget(twistSpin, 1);

	QLabel* twistLabel2 = new QLabel("0", this);
	twistLayout->addWidget(twistLabel2);

	twistSlider = new QSlider(Qt::Horizontal, this);
	twistSlider->setRange(0, 100);
	twistSlider->setSliderPosition(0);
	twistLayout->addWidget(twistSlider, 10);

	QLabel* twistLabel3 = new QLabel("100", this);
	twistLayout->addWidget(twistLabel3);

	// Parameter spin stuff
	QHBoxLayout* paramLayout = new QHBoxLayout(this);
	paramLabels.push_back(new QLabel("Param 1:", this));
	paramLabels.push_back(new QLabel("Param 2:", this));
	paramLabels.push_back(new QLabel("Param 3:", this));
	paramSpins.push_back(new QSpinBox(this));
	paramSpins.push_back(new QSpinBox(this));
	paramSpins.push_back(new QSpinBox(this));
	for (unsigned int i = 0; i < paramLabels.size(); ++i)
	{
		paramLayout->addWidget(paramLabels[i], 0);
		paramLayout->addWidget(paramSpins[i], 0);
		paramSpins[i]->setRange(2, 30);
		paramSpins[i]->setSingleStep(1);
		paramLabels[i]->hide();
		paramSpins[i]->hide();
	}
	editorLayout->addLayout(paramLayout, 0);	

	QHBoxLayout* customizePullTemplateLayout = new QHBoxLayout(this);
	customizePlanButton = new QPushButton("Manually Customize");
	customizePullTemplateLayout->addWidget(customizePlanButton);
	editorLayout->addLayout(customizePullTemplateLayout, 0);

	// Little description for the editor
	editorLayout->addStretch(1);
	QLabel* descriptionLabel = new QLabel("Cane editor - drag color or other canes in.", this);
	descriptionLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	editorLayout->addWidget(descriptionLabel, 0);

	QVBoxLayout* niceViewLayout = new QVBoxLayout(this);
	pageLayout->addLayout(niceViewLayout, 1);
	niceViewLayout->addWidget(niceViewWidget, 10);

	// Little description for the editor
	QLabel* niceViewDescriptionLabel = new QLabel("3D view of cane.", this);
	niceViewDescriptionLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	niceViewLayout->addWidget(niceViewDescriptionLabel, 0);
}

void PullPlanEditorWidget :: mousePressEvent(QMouseEvent* event)
{
	PullTemplateLibraryWidget* ptlw = dynamic_cast<PullTemplateLibraryWidget*>(childAt(event->pos()));

	if (ptlw != NULL)
	{
		plan->setTemplateType(ptlw->getPullTemplateType());
		emit someDataChanged();
	}
}

void PullPlanEditorWidget :: removeCasingButtonPressed()
{
	plan->removeCasing();
	emit someDataChanged();
}

void PullPlanEditorWidget :: addCasingButtonPressed()
{
	plan->addCasing(shapeButtonGroup->checkedId());
	emit someDataChanged();
}

void PullPlanEditorWidget :: fillRuleButtonGroupChanged(int)
{
	viewWidget->setFillRule(fillRuleButtonGroup->checkedId());
}

void PullPlanEditorWidget :: setupConnections()
{
	connect(fillRuleButtonGroup, SIGNAL(buttonClicked(int)), this, SLOT(fillRuleButtonGroupChanged(int)));
	connect(addCasingButton, SIGNAL(pressed()), this, SLOT(addCasingButtonPressed()));
	connect(removeCasingButton, SIGNAL(pressed()), this, SLOT(removeCasingButtonPressed()));
	connect(shapeButtonGroup, SIGNAL(buttonClicked(int)), this, SLOT(shapeButtonGroupChanged(int)));
	connect(twistSlider, SIGNAL(valueChanged(int)), this, SLOT(twistSliderChanged(int)));
	connect(twistSpin, SIGNAL(valueChanged(int)), this, SLOT(twistSpinChanged(int)));

	for (unsigned int i = 0; i < paramSpins.size(); ++i)
	{
		connect(paramSpins[i], SIGNAL(valueChanged(int)), this, SLOT(paramSpinChanged(int)));
	}
	connect(customizePlanButton, SIGNAL(pressed()),this,SLOT(openCustomizeWidget()));

	connect(this, SIGNAL(someDataChanged()), this, SLOT(updateEverything()));
	connect(viewWidget, SIGNAL(someDataChanged()), this, SLOT(viewWidgetDataChanged()));
}

void PullPlanEditorWidget :: viewWidgetDataChanged()
{
	emit someDataChanged();
}

void PullPlanEditorWidget :: paramSpinChanged(int)
{
	// update template
	for (unsigned int i = 0; i < plan->getParameterCount(); ++i)
	{
		if (plan->getParameter(i) != paramSpins[i]->value())
		{
			plan->setParameter(i, paramSpins[i]->value());
			emit someDataChanged();
		}
	}
}

void PullPlanEditorWidget :: twistSliderChanged(int)
{
	float twist = twistSlider->sliderPosition();

	if (twist == plan->getTwist())
		return;
	plan->setTwist(twist);
	emit someDataChanged();
}

void PullPlanEditorWidget :: twistSpinChanged(int)
{
	float twist = twistSpin->value();

	if (twist == plan->getTwist())
		return;
	plan->setTwist(twist);
	emit someDataChanged();
}


void PullPlanEditorWidget :: shapeButtonGroupChanged(int)
{
	switch (shapeButtonGroup->checkedId())
	{
		case 1:
			if (plan->getOutermostCasingShape() == CIRCLE_SHAPE)
				return;
			plan->setOutermostCasingShape(CIRCLE_SHAPE);
			emit someDataChanged(); 
			break;
		case 2:
			if (plan->getOutermostCasingShape() == SQUARE_SHAPE)
				return;
			plan->setOutermostCasingShape(SQUARE_SHAPE);
			emit someDataChanged();
			break;
	}
}

void PullPlanEditorWidget :: seedTemplates()
{
	char filename[100];
	for (int i = FIRST_PULL_TEMPLATE; i <= LAST_PULL_TEMPLATE; ++i)
	{
		sprintf(filename, ":/images/pulltemplate%d.png", i - FIRST_PULL_TEMPLATE + 1);
		PullTemplateLibraryWidget *ptlw = new PullTemplateLibraryWidget(
			QPixmap::fromImage(QImage(filename)), i);
		templateLibraryLayout->addWidget(ptlw);
	}
}

void PullPlanEditorWidget :: updateLibraryWidgetPixmaps(AsyncPullPlanLibraryWidget* w)
{
	w->updatePixmaps(QPixmap::grabWidget(viewWidget).scaled(100, 100));
}

void PullPlanEditorWidget :: openCustomizeWidget()
{
	pullPlanCustomizeWidget->openWindow(this->getPlan());
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




