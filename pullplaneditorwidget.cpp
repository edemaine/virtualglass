
#include "pullplaneditorwidget.h"

PullPlanEditorWidget :: PullPlanEditorWidget(QWidget* parent) : QWidget(parent)
{
	plan = new PullPlan(CIRCLE_BASE_PULL_TEMPLATE);
	plan->setTemplateType(CASED_CIRCLE_PULL_TEMPLATE);

	viewWidget = new PullPlanEditorViewWidget(plan, this);	
	customizeViewWidget = new PullPlanCustomizeViewWidget(plan, this);
	niceViewWidget = new NiceViewWidget(PULLPLAN_MODE, this);
	niceViewWidget->setGeometry(&geometry);

	setupLayout();
	setupConnections();
}

void PullPlanEditorWidget :: updateEverything()
{
	fillRuleComboBox->setCurrentIndex(viewWidget->getFillRule()-1);

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
	emit geometryChanged(geometry);
	emit pullPlanChanged(plan);

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
	templateLibraryWidget->setLayout(templateLibraryLayout);

	QScrollArea* pullTemplateLibraryScrollArea = new QScrollArea(editorWidget);
	pullTemplateLibraryScrollArea->setBackgroundRole(QPalette::Dark);
	pullTemplateLibraryScrollArea->setWidget(templateLibraryWidget);
	pullTemplateLibraryScrollArea->setWidgetResizable(true);
	pullTemplateLibraryScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	pullTemplateLibraryScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	pullTemplateLibraryScrollArea->setFixedHeight(130);
	editorLayout->addWidget(pullTemplateLibraryScrollArea, 0);

	// Editor layout: fill rule layout
	QHBoxLayout* fillRuleLayout = new QHBoxLayout(editorWidget);
	fillRuleComboBox = new QComboBox(editorWidget);
	fillRuleComboBox->addItem("Single");
	fillRuleComboBox->addItem("Every second");
	fillRuleComboBox->addItem("Every third");
	fillRuleComboBox->addItem("Group");
	fillRuleComboBox->addItem("All");
	fillRuleLayout->addWidget(new QLabel("Fill rule:", editorWidget), 0);
	fillRuleLayout->addWidget(fillRuleComboBox, 0);
	fillRuleLayout->addStretch(1);
	editorLayout->addLayout(fillRuleLayout, 0);

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

	twistSpin = new QSpinBox(editorWidget);
	twistSpin->setRange(-50, 50);
	twistSpin->setSingleStep(1);
	twistLayout->addWidget(twistSpin, 1);

	QLabel* twistLabel2 = new QLabel("-50", editorWidget);
	twistLayout->addWidget(twistLabel2);

	twistSlider = new QSlider(Qt::Horizontal, editorWidget);
	twistSlider->setRange(-50, 50);
	twistSlider->setSliderPosition(0);
	twistSlider->setTickInterval(50);	
	twistSlider->setTickPosition(QSlider::TicksBothSides);
	twistLayout->addWidget(twistSlider, 10);

	QLabel* twistLabel3 = new QLabel("50", editorWidget);
	twistLayout->addWidget(twistLabel3);

	// Editor layout: parameter spin stuff
	QHBoxLayout* paramLayout = new QHBoxLayout(editorWidget);
	paramLabels.push_back(new QLabel("Param 1:", editorWidget));
	paramLabels.push_back(new QLabel("Param 2:", editorWidget));
	paramLabels.push_back(new QLabel("Param 3:", editorWidget));
	paramSpins.push_back(new QSpinBox(editorWidget));
	paramSpins.push_back(new QSpinBox(editorWidget));
	paramSpins.push_back(new QSpinBox(editorWidget));
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
	editorLayout->addStretch(0);

	// Customize layout
	QWidget* customizeWidget = new QWidget(this);
	QVBoxLayout* customizeLayout = new QVBoxLayout(customizeWidget);
	customizeWidget->setLayout(customizeLayout);

	// Customize layout: interactive editor
	customizeLayout->addWidget(customizeViewWidget, 1);

	// Customize layout: button layout
        QHBoxLayout* windowControlsLayout = new QHBoxLayout(customizeWidget);
        confirmChangesButton = new QPushButton("Confirm changes", customizeWidget);
        cancelChangesButton = new QPushButton("Cancel", customizeWidget);
        windowControlsLayout->addWidget(confirmChangesButton);
        windowControlsLayout->addWidget(cancelChangesButton);
        customizeLayout->addLayout(windowControlsLayout, 0);
	
	// Combine editor and customize layouts into a pair of tabs with 
	// descriptive text in the left layout
	QWidget* leftWidget = new QWidget(this);
	QVBoxLayout* leftLayout = new QVBoxLayout(leftWidget);
	leftWidget->setLayout(leftLayout);
	QTabWidget* tabs = new QTabWidget(leftWidget);
	tabs->addTab(editorWidget, QString("Standard"));
	tabs->addTab(customizeWidget, QString("Customize"));
	leftLayout->addWidget(tabs, 1);
	QLabel* descriptionLabel = new QLabel("Cane editor - drag color or other canes in.", leftWidget);
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
		plan->setTemplateType(ptlw->getPullTemplateType());
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

void PullPlanEditorWidget :: fillRuleComboBoxChanged(int)
{
	viewWidget->setFillRule(fillRuleComboBox->currentIndex()+1);
}

void PullPlanEditorWidget :: setupConnections()
{
	connect(fillRuleComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(fillRuleComboBoxChanged(int)));
	connect(circleCasingPushButton, SIGNAL(clicked()), this, SLOT(circleCasingButtonPressed()));
	connect(squareCasingPushButton, SIGNAL(clicked()), this, SLOT(squareCasingButtonPressed()));
	connect(addCasingButton, SIGNAL(clicked()), this, SLOT(addCasingButtonPressed()));
	connect(removeCasingButton, SIGNAL(clicked()), this, SLOT(removeCasingButtonPressed()));
	connect(twistSlider, SIGNAL(valueChanged(int)), this, SLOT(twistSliderChanged(int)));
	connect(twistSpin, SIGNAL(valueChanged(int)), this, SLOT(twistSpinChanged(int)));

	for (unsigned int i = 0; i < paramSpins.size(); ++i)
	{
		connect(paramSpins[i], SIGNAL(valueChanged(int)), this, SLOT(paramSpinChanged(int)));
	}

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
	w->updatePixmaps(QPixmap::grabWidget(viewWidget, viewWidget->usedRect()).scaled(100, 100));
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




