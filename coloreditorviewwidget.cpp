
#include "coloreditorviewwidget.h"

ColorEditorViewWidget :: ColorEditorViewWidget(PullPlan* plan, QWidget* parent) : QWidget(parent)
{
	QVBoxLayout* editorLayout = new QVBoxLayout(this);
	this->setLayout(editorLayout);

        QWidget* tableGridLibraryWidget = new QWidget(this);
        QGridLayout* tableGridLayout = new QGridLayout(tableGridLibraryWidget);
        tableGridLayout->setSpacing(10);
        tableGridLibraryWidget->setLayout(tableGridLayout);

        // Setup pickup template scrolling library
        QScrollArea* tableGridLibraryScrollArea = new QScrollArea;
        tableGridLibraryScrollArea->setBackgroundRole(QPalette::Dark);
        tableGridLibraryScrollArea->setWidget(tableGridLibraryWidget);
        tableGridLibraryScrollArea->setWidgetResizable(true);
        tableGridLibraryScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tableGridLibraryScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        editorLayout->addWidget(tableGridLibraryScrollArea);

        /* MARTY'S HAND SELECTION OF COLORS:
         * Clear, K141A Cherry Red, K210 Sari Blue, K212A Brilliant Gold, K228 Dark Heliotrope, K215A Gold Brown, K213 Brilliant Green;
         * White, Black, K070 Opal Green, K078A Canary Yellow, K086 Turquoise
         */

	char* names[] = {
		(char*) "Cherry Red", 
		(char*) "Sari Blue", 
		(char*) "Brilliant Gold", 
		(char*) "Dark Heliotrope", 
		(char*) "Gold Brown",
		(char*) "Clear", 
		(char*) "Brilliant Green", 
		(char*) "White", 
		(char*) "Black", 
		(char*) "Opal Green", 
		(char*) "Canary Yellow", 
		(char*) "Turqouise"};
        int rgba[][4] = {
              	{204,0,0,126}, {0,112,179,126}, {254,220,29,126},
                {164,116,184,126}, {163,118,58,126}, {255, 255, 255, 0}, {153,204,51,126}, 
		{255,255,255,255}, {0,0,0,255}, {0,140,0,255}, {249,219,6,255}, 
		{121,190,196,255},
                {-1,-1,-1,-1}};
        Color color;
        for (int i = 0; rgba[i][0] >= 0; ++i) 
	{
                color.r = rgba[i][0] / 255.0;
                color.g = rgba[i][1] / 255.0;
                color.b = rgba[i][2] / 255.0;
                color.a = rgba[i][3] / 255.0;
		char* name = new char[100];
		strcpy(name, names[i]);
		PureColorLibraryWidget* pclw = new PureColorLibraryWidget(color, name, this);
		tableGridLayout->addWidget(pclw, i, 0);
	}

	QHBoxLayout* alphaLayout = new QHBoxLayout(this);
        editorLayout->addLayout(alphaLayout);

        QLabel* alphaLabel1 = new QLabel("Opacity:", this);
        alphaLayout->addWidget(alphaLabel1, 0);
        QLabel* alphaLabel2 = new QLabel("0%", this);
        alphaLayout->addWidget(alphaLabel2, 0);
	alphaSlider = new QSlider(Qt::Horizontal, this);
	alphaSlider->setRange(0, 255);
	alphaLayout->addWidget(alphaSlider);
        QLabel* alphaLabel3 = new QLabel("100%", this);
        alphaLayout->addWidget(alphaLabel3, 0);
	connect(alphaSlider, SIGNAL(valueChanged(int)), this, SLOT(alphaSliderPositionChanged(int)));	

        QLabel* casingLabel = new QLabel("Casing:");
        QCheckBox* circleCheckBox = new QCheckBox("Circle");
        QCheckBox* squareCheckBox = new QCheckBox("Square");
        colorBarTemplateShapeButtonGroup = new QButtonGroup();
        colorBarTemplateShapeButtonGroup->addButton(circleCheckBox, 1);
        colorBarTemplateShapeButtonGroup->addButton(squareCheckBox, 2);
        QHBoxLayout* colorBarShapeLayout = new QHBoxLayout(this);
        colorBarShapeLayout->addWidget(casingLabel);
        colorBarShapeLayout->addWidget(circleCheckBox);
        colorBarShapeLayout->addWidget(squareCheckBox);
        editorLayout->addLayout(colorBarShapeLayout, 0);
        connect(colorBarTemplateShapeButtonGroup, SIGNAL(buttonClicked(int)), 
		this, SLOT(colorBarTemplateShapeButtonGroupChanged(int)));

	this->plan = plan;
}

void ColorEditorViewWidget :: colorBarTemplateShapeButtonGroupChanged(int)
{
        switch (colorBarTemplateShapeButtonGroup->checkedId())
        {
                case 1:
                        if (plan->getTemplate()->shape == CIRCLE_SHAPE)
                                return;
                        plan->getTemplate()->shape = CIRCLE_SHAPE;
                        emit someDataChanged();
                        break;
                case 2:
                        if (plan->getTemplate()->shape == SQUARE_SHAPE)
                                return;
                        plan->getTemplate()->shape = SQUARE_SHAPE;
                        emit someDataChanged();
                        break;
        }
}

void ColorEditorViewWidget :: alphaSliderPositionChanged(int)
{
	if (alphaSlider->sliderPosition() != (int) (this->plan->color.a * 255))
	{
		this->plan->color.a = alphaSlider->sliderPosition() / 255.0;
		emit someDataChanged();
	} 
}

void ColorEditorViewWidget :: mouseReleaseEvent(QMouseEvent* event)
{
        PureColorLibraryWidget* pclw = dynamic_cast<PureColorLibraryWidget*>(childAt(event->pos()));
	if (pclw != NULL)
	{
		this->plan->color = pclw->getColor();
		this->alphaSlider->setSliderPosition((int) (plan->color.a * 255));
		emit someDataChanged();	
	}
}

void ColorEditorViewWidget :: setPullPlan(PullPlan* plan)
{
	this->plan = plan;
	this->alphaSlider->setSliderPosition((int) (plan->color.a * 255));
        static_cast<QCheckBox*>(colorBarTemplateShapeButtonGroup->button(
                plan->getTemplate()->shape))->setCheckState(Qt::Checked);
	emit someDataChanged();	
}



