
#include "coloreditorviewwidget.h"

ColorEditorViewWidget :: ColorEditorViewWidget(PullPlan* plan, QWidget* parent) : QWidget(parent)
{
	QVBoxLayout* editorLayout = new QVBoxLayout(this);
	this->setLayout(editorLayout);

	QTabWidget* tabs = new QTabWidget(this);
	editorLayout->addWidget(tabs);

        QWidget* colorLibrary1Widget = new QWidget(tabs);
        colorLibrary1Layout = new QVBoxLayout(colorLibrary1Widget);
        colorLibrary1Layout->setSpacing(10);
        colorLibrary1Widget->setLayout(colorLibrary1Layout);
        QScrollArea* colorLibrary1ScrollArea = new QScrollArea;
        colorLibrary1ScrollArea->setBackgroundRole(QPalette::Dark);
        colorLibrary1ScrollArea->setWidget(colorLibrary1Widget);
        colorLibrary1ScrollArea->setWidgetResizable(true);
	colorLibrary1ScrollArea->setMinimumWidth(320);
        colorLibrary1ScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        colorLibrary1ScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	tabs->addTab(colorLibrary1ScrollArea, "Reichenbach Transparents");

        QWidget* colorLibrary2Widget = new QWidget(tabs);
        colorLibrary2Layout = new QVBoxLayout(colorLibrary2Widget);
        colorLibrary2Layout->setSpacing(10);
        colorLibrary2Widget->setLayout(colorLibrary2Layout);
        QScrollArea* colorLibrary2ScrollArea = new QScrollArea;
        colorLibrary2ScrollArea->setBackgroundRole(QPalette::Dark);
        colorLibrary2ScrollArea->setWidget(colorLibrary2Widget);
        colorLibrary2ScrollArea->setWidgetResizable(true);
	colorLibrary2ScrollArea->setMinimumWidth(320);
        colorLibrary2ScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        colorLibrary2ScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	tabs->addTab(colorLibrary2ScrollArea, "Reichenbach Opaques");

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

	this->plan = plan;
}

void ColorEditorViewWidget :: seedMartyColors()
{
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
		QString name(names[i]);
                PureColorLibraryWidget* pclw = new PureColorLibraryWidget(color, name, this);
		if (rgba[i][3] != 255)
			colorLibrary1Layout->addWidget(pclw);
		else
			colorLibrary2Layout->addWidget(pclw);
        }

}

void ColorEditorViewWidget :: seedBrandColors()
{ 
	Color caneColor;
	QString caneName;

        QFile file("../src/Colors1.txt");
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        while (!file.atEnd())
        {
                QByteArray line = file.readLine();
                line = line.trimmed();
                if (line.isEmpty())
			continue;

                if (line.at(0) == '[')
                {
                        line.remove(0,1);
                        line.remove(line.lastIndexOf(']'),1);
                        line = line.trimmed();
			caneName = line;
			if (caneName[0] != 'R') // Only load reichenbach colors for now, bug if list is too large
				break;
                }
                else if (line.at(0) == '-')
                {
                        line.remove(0,1);
                        line = line.trimmed();
                        QList<QByteArray> colorData = line.split(',');
			caneColor.r = colorData[0].toInt() / 255.0;
			caneColor.g = colorData[1].toInt() / 255.0;
			caneColor.b = colorData[2].toInt() / 255.0;
			caneColor.a = colorData[3].toInt() / 255.0;

	                PureColorLibraryWidget* pclw = new PureColorLibraryWidget(caneColor, caneName, this);
			if (colorData[3].toInt() != 255)
				colorLibrary1Layout->addWidget(pclw);
			else
				colorLibrary2Layout->addWidget(pclw);
                }
        }
	file.close();      
}

void ColorEditorViewWidget :: alphaSliderPositionChanged(int)
{
	if (alphaSlider->sliderPosition() != (int) (this->plan->color->a * 255))
	{
		this->plan->color->a = alphaSlider->sliderPosition() / 255.0;
		emit someDataChanged();
	} 
}

void ColorEditorViewWidget :: mouseReleaseEvent(QMouseEvent* event)
{
        PureColorLibraryWidget* pclw = dynamic_cast<PureColorLibraryWidget*>(childAt(event->pos()));
	if (pclw != NULL)
	{
		*(this->plan->color) = pclw->getColor();
		this->alphaSlider->setSliderPosition((int) (plan->color->a * 255));
		emit someDataChanged();	
	}
}

void ColorEditorViewWidget :: setPullPlan(PullPlan* plan)
{
	this->plan = plan;
	this->alphaSlider->setSliderPosition((int) (plan->color->a * 255));
	emit someDataChanged();	
}



