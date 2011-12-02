
#include "pullplaneditorwidget.h"

PullPlanEditorWidget :: PullPlanEditorWidget(QWidget* parent) : QWidget(parent)
{
	Color* color = new Color;
	color->r = color->g = color->b = color->a = 1.0;
	this->plan = new PullPlan(CIRCLE_BASE_PULL_TEMPLATE, color);

	this->viewWidget = new PullPlanEditorViewWidget(plan, this);	
	this->niceViewWidget = new NiceViewWidget(this);
        niceViewWidget->setCameraMode(PULLPLAN_MODE);
	niceViewWidget->setGeometry(&geometry);

	setupLayout();
	setupConnections();
}

void PullPlanEditorWidget :: updateEverything()
{
        // Only attempt to set the shape if it's defined; it's undefined during loading
        static_cast<QCheckBox*>(shapeButtonGroup->button(
                plan->getTemplate()->getShape()))->setCheckState(Qt::Checked);

        int thickness = (int) (plan->getTemplate()->getCasingThickness() * 100);
        casingThicknessSlider->setSliderPosition(thickness);

        int twist = plan->twist;
        twistSlider->setSliderPosition(twist);
        twistSpin->setValue(twist);
        viewWidget->repaint();

        geometry.clear();
        mesher.generateMesh(plan, &geometry);
        niceViewWidget->repaint();
}

void PullPlanEditorWidget :: setupLayout()
{
        // Setup the editor layout
        QHBoxLayout* pageLayout = new QHBoxLayout(this);
        this->setLayout(pageLayout);
        QVBoxLayout* editorLayout = new QVBoxLayout(this);
        pageLayout->addLayout(editorLayout);

        editorLayout->addWidget(viewWidget, 10);

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
        shapeButtonGroup = new QButtonGroup();
        shapeButtonGroup->addButton(circleCheckBox, 1);
        shapeButtonGroup->addButton(squareCheckBox, 2);
        QHBoxLayout* pullTemplateShapeLayout = new QHBoxLayout(this);
        pullTemplateShapeLayout->addWidget(casingLabel);
        pullTemplateShapeLayout->addWidget(circleCheckBox);
        pullTemplateShapeLayout->addWidget(squareCheckBox);
        editorLayout->addLayout(pullTemplateShapeLayout, 0);

        // Casing thickness slider stuff
        QHBoxLayout* casingThicknessLayout = new QHBoxLayout(this);
        editorLayout->addLayout(casingThicknessLayout);

        QLabel* casingLabel1 = new QLabel("Casing Thickness:", this);
        casingThicknessLayout->addWidget(casingLabel1, 0);

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
        editorLayout->addLayout(twistLayout);

        QLabel* twistLabel1 = new QLabel("Twist:", this);
        twistLayout->addWidget(twistLabel1);

        twistSpin = new QSpinBox(this);
        twistSpin->setRange(-50, 50);
        twistSpin->setSingleStep(1);
        twistLayout->addWidget(twistSpin, 1);

        QLabel* twistLabel2 = new QLabel("-50", this);
        twistLayout->addWidget(twistLabel2);

        twistSlider = new QSlider(Qt::Horizontal, this);
        twistSlider->setRange(-50, 50);
        twistSlider->setTickInterval(5);
        twistSlider->setTickPosition(QSlider::TicksBothSides);
        twistSlider->setSliderPosition(0);
        twistLayout->addWidget(twistSlider, 10);

        QLabel* twistLabel3 = new QLabel("50", this);
        twistLayout->addWidget(twistLabel3);

        // Little description for the editor
        QLabel* descriptionLabel = new QLabel("Cane editor - drag color or other canes into the cane to edit.", this);
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

void PullPlanEditorWidget :: setupConnections()
{
        connect(shapeButtonGroup, SIGNAL(buttonClicked(int)), this, SLOT(shapeButtonGroupChanged(int)));
        connect(casingThicknessSlider, SIGNAL(valueChanged(int)),
                this, SLOT(casingThicknessSliderChanged(int)));
        connect(twistSlider, SIGNAL(valueChanged(int)), this, SLOT(twistSliderChanged(int)));
        connect(twistSpin, SIGNAL(valueChanged(int)), this, SLOT(twistSpinChanged(int)));
	connect(this, SIGNAL(someDataChanged()), this, SLOT(updateEverything()));
	connect(viewWidget, SIGNAL(someDataChanged()), this, SLOT(updateEverything()));
}

void PullPlanEditorWidget :: casingThicknessSliderChanged(int)
{
        float thickness = casingThicknessSlider->sliderPosition() / 100.0;

	if (thickness == plan->getTemplate()->getCasingThickness())
		return;
        plan->getTemplate()->setCasingThickness(thickness);
        emit someDataChanged();
}

void PullPlanEditorWidget :: twistSliderChanged(int)
{
        float twist = twistSlider->sliderPosition();

	if (twist == plan->twist)
		return;
        plan->twist = twist;
        emit someDataChanged();
}

void PullPlanEditorWidget :: twistSpinChanged(int)
{
        int twist = twistSpin->value();

	if (twist == plan->twist)
		return;
        twistSlider->setSliderPosition(twist);
        emit someDataChanged();
}

void PullPlanEditorWidget :: shapeButtonGroupChanged(int)
{
        switch (shapeButtonGroup->checkedId())
        {
                case 1:
                        if (plan->getTemplate()->getShape() == CIRCLE_SHAPE)
                                return;
                        plan->getTemplate()->setShape(CIRCLE_SHAPE);
                       	emit someDataChanged(); 
                        break;
                case 2:
                        if (plan->getTemplate()->getShape() == SQUARE_SHAPE)
                                return;
                        plan->getTemplate()->setShape(SQUARE_SHAPE);
                        emit someDataChanged();
                        break;
        }
}

void PullPlanEditorWidget :: setPlanTwist(int twist)
{
	plan->twist = twist;
	emit someDataChanged();	
}

void PullPlanEditorWidget :: setPlanTemplate(PullTemplate* t)
{
	plan->setTemplate(t);
	emit someDataChanged();	
}

void PullPlanEditorWidget :: setPlanColor(Color* c)
{
	plan->color = c;
	emit someDataChanged();	
}

void PullPlanEditorWidget :: setPlanTemplateCasingThickness(float t)
{
	plan->getTemplate()->setCasingThickness(t);
	emit someDataChanged();	
}

void PullPlanEditorWidget :: setPlanSubplans(Color* c)
{
	plan->subplans.clear();
        for (unsigned int i = 0; i < plan->getTemplate()->subtemps.size(); ++i)
        {
                switch (plan->getTemplate()->subtemps[i].shape)
                {
                        case CIRCLE_SHAPE:
                                plan->subplans.push_back(
                                        new PullPlan(CIRCLE_BASE_PULL_TEMPLATE, c));
                                break;
                        case SQUARE_SHAPE:
                                plan->subplans.push_back(
                                        new PullPlan(SQUARE_BASE_PULL_TEMPLATE, c));
                                break;
                }
        }
	emit someDataChanged();	
}

void PullPlanEditorWidget :: seedTemplates()
{
        for (int i = FIRST_PULL_TEMPLATE; i <= LAST_PULL_TEMPLATE; ++i)
        {
                plan->setTemplate(new PullTemplate(i));
		emit someDataChanged();	
                PullTemplateLibraryWidget *ptlw = getTemplateLibraryWidget();
                templateLibraryLayout->addWidget(ptlw);
        }
}

PullTemplateLibraryWidget* PullPlanEditorWidget :: getTemplateLibraryWidget()
{
	return new PullTemplateLibraryWidget(QPixmap::grabWidget(this->viewWidget).scaled(100, 100), plan->getTemplate()->type);
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




