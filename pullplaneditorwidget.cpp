
#include "pullplaneditorwidget.h"

PullPlanEditorWidget :: PullPlanEditorWidget(QWidget* parent) : QWidget(parent)
{
	Color* color = new Color;
	*color = make_vector(1.0f, 1.0f, 1.0f, 0.0f); //clear
	this->plan = new PullPlan(CIRCLE_BASE_PULL_TEMPLATE, color);
	this->plan->setTemplateType(CASED_CIRCLE_PULL_TEMPLATE);

	this->viewWidget = new PullPlanEditorViewWidget(plan, this);	
	this->niceViewWidget = new NiceViewWidget(PULLPLAN_MODE, this);
	niceViewWidget->setGeometry(&geometry);

	setupLayout();
	setupConnections();
}

void PullPlanEditorWidget :: updateEverything()
{
        static_cast<QCheckBox*>(shapeButtonGroup->button(
                plan->getShape()))->setCheckState(Qt::Checked);

        int thickness = (int) (plan->getCasingThickness() * 100);
        casingThicknessSlider->setSliderPosition(thickness);
        casingThicknessSpin->setValue(thickness);

        int twist = plan->getTwist();
        twistSlider->setSliderPosition(twist);
        twistSpin->setValue(twist);

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
        mesher.generateMesh(plan, &geometry);
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

        QLabel* casingLabel = new QLabel("Casing:");
        QCheckBox* circleCheckBox = new QCheckBox("Circle");
        QCheckBox* squareCheckBox = new QCheckBox("Square");
	addCasingButton = new QPushButton("Add Casing");
        shapeButtonGroup = new QButtonGroup();
        shapeButtonGroup->addButton(circleCheckBox, 1);
        shapeButtonGroup->addButton(squareCheckBox, 2);
        QHBoxLayout* pullTemplateShapeLayout = new QHBoxLayout(this);
        pullTemplateShapeLayout->addWidget(casingLabel);
        pullTemplateShapeLayout->addWidget(circleCheckBox);
        pullTemplateShapeLayout->addWidget(squareCheckBox);
        pullTemplateShapeLayout->addWidget(addCasingButton);
        editorLayout->addLayout(pullTemplateShapeLayout, 0);

        // Casing thickness slider stuff
        QHBoxLayout* casingThicknessLayout = new QHBoxLayout(this);
        editorLayout->addLayout(casingThicknessLayout);

        QLabel* casingLabel1 = new QLabel("Casing Thickness:", this);
        casingThicknessLayout->addWidget(casingLabel1, 0);

        casingThicknessSpin = new QSpinBox(this);
        casingThicknessSpin->setRange(1, 99);
        casingThicknessSpin->setSingleStep(1);
        casingThicknessLayout->addWidget(casingThicknessSpin, 1);

        QLabel* casingLabel2 = new QLabel("0%", this);
        casingThicknessLayout->addWidget(casingLabel2, 0);

        casingThicknessSlider = new QSlider(Qt::Horizontal, this);
        casingThicknessSlider->setRange(1, 99);
        casingThicknessSlider->setTickPosition(QSlider::TicksBothSides);
        casingThicknessSlider->setSliderPosition(0);
        casingThicknessLayout->addWidget(casingThicknessSlider, 10);

        QLabel* casingLabel3 = new QLabel("100%", this);
        casingThicknessLayout->addWidget(casingLabel3, 0);

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
        twistSlider->setTickInterval(5);
        twistSlider->setTickPosition(QSlider::TicksBothSides);
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

void PullPlanEditorWidget :: addCasingButtonPressed()
{
	PullPlan* superplan;

	switch (plan->getShape())
	{
		case CIRCLE_SHAPE:
			superplan =  new PullPlan(CASED_CIRCLE_PULL_TEMPLATE, plan->getColor());
			break;
		case SQUARE_SHAPE:
			superplan =  new PullPlan(CASED_SQUARE_PULL_TEMPLATE, plan->getColor());
			break;
		default:
			exit(0);
	}
	
	superplan->subplans.clear();
	superplan->subplans.push_back(plan);
	emit newPullPlan(superplan);
}

void PullPlanEditorWidget :: setupConnections()
{
	connect(addCasingButton, SIGNAL(pressed()), this, SLOT(addCasingButtonPressed()));
        connect(shapeButtonGroup, SIGNAL(buttonClicked(int)), this, SLOT(shapeButtonGroupChanged(int)));
        connect(casingThicknessSlider, SIGNAL(valueChanged(int)),
                this, SLOT(casingThicknessSliderChanged(int)));
        connect(casingThicknessSpin, SIGNAL(valueChanged(int)),
                this, SLOT(casingThicknessSpinChanged(int)));
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

void PullPlanEditorWidget :: casingThicknessSliderChanged(int)
{
        float thickness = casingThicknessSlider->sliderPosition() / 100.0;

	if (thickness == plan->getCasingThickness())
		return;
        plan->setCasingThickness(thickness);
        emit someDataChanged();
}

void PullPlanEditorWidget :: casingThicknessSpinChanged(int)
{
        float thickness = casingThicknessSpin->value() / 100.0;

	if (thickness == plan->getCasingThickness())
		return;
        plan->setCasingThickness(thickness);
        emit someDataChanged();
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
                        if (plan->getShape() == CIRCLE_SHAPE)
                                return;
                        plan->setShape(CIRCLE_SHAPE);
                       	emit someDataChanged(); 
                        break;
                case 2:
                        if (plan->getShape() == SQUARE_SHAPE)
                                return;
                        plan->setShape(SQUARE_SHAPE);
			plan->setTwist(0.0); // reset twist to zero because square casing can't be twisted
                        emit someDataChanged();
                        break;
        }
}

void PullPlanEditorWidget :: setPlanTwist(int twist)
{
	plan->setTwist(twist);
	emit someDataChanged();	
}

void PullPlanEditorWidget :: setPlanTemplate(int templateType)
{
	plan->setTemplateType(templateType);
	emit someDataChanged();	
}

void PullPlanEditorWidget :: setPlanColor(Color* c)
{
	plan->setColor(c);
	emit someDataChanged();	
}

void PullPlanEditorWidget :: setPlanTemplateCasingThickness(float t)
{
	plan->setCasingThickness(t);
	emit someDataChanged();	
}

void PullPlanEditorWidget :: setPlanSubplans(PullPlan* sp)
{
	plan->subplans.clear();
        for (unsigned int i = 0; i < plan->subtemps.size(); ++i)
        {
                if (plan->subtemps[i].shape == sp->getShape()
			|| sp->getTemplateType() == AMORPHOUS_BASE_PULL_TEMPLATE)
                {
			plan->subplans.push_back(sp);
                }
		else
		{
			switch(plan->subtemps[i].shape)
			{
				case CIRCLE_SHAPE:
					plan->subplans.push_back(
						new PullPlan(CIRCLE_BASE_PULL_TEMPLATE, sp->getColor()));
					break;
				case SQUARE_SHAPE:
					plan->subplans.push_back(
						new PullPlan(SQUARE_BASE_PULL_TEMPLATE, sp->getColor()));
					break;
			}
		}
        }
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

void PullPlanEditorWidget :: updateLibraryWidgetPixmaps(PullPlanLibraryWidget* w)
{
	w->updatePixmaps(
		QPixmap::fromImage(niceViewWidget->renderImage()).scaled(100, 100),
		QPixmap::grabWidget(viewWidget).scaled(100, 100));
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




