
#include "coloreditorviewwidget.h"

ColorEditorViewWidget :: ColorEditorViewWidget(PullPlan* plan, QWidget* parent) : QWidget(parent)
{
	QVBoxLayout* editorLayout = new QVBoxLayout(this);
	this->setLayout(editorLayout);

        QWidget* colorLibraryWidget = new QWidget(this);
        colorLibraryLayout = new QVBoxLayout(colorLibraryWidget);
        colorLibraryLayout->setSpacing(10);
        colorLibraryWidget->setLayout(colorLibraryLayout);

        // Setup pickup template scrolling library
        QScrollArea* colorLibraryScrollArea = new QScrollArea;
        colorLibraryScrollArea->setBackgroundRole(QPalette::Dark);
        colorLibraryScrollArea->setWidget(colorLibraryWidget);
        colorLibraryScrollArea->setWidgetResizable(true);
	colorLibraryScrollArea->setMinimumWidth(320);
        colorLibraryScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        colorLibraryScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        editorLayout->addWidget(colorLibraryScrollArea);

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
                colorLibraryLayout->addWidget(pclw);
        }

}

void ColorEditorViewWidget :: seedBrandColors()
{ 
        QList<Color> caneColorList;
        QStringList caneNameList;
	QStringList caneBrandList;
	QString currentBrand = "";

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
                        caneNameList.append(line);
                        caneBrandList.append(currentBrand);
                }
                else if (line.at(0) == '-')
                {
                        line.remove(0,1);
                        line = line.trimmed();
                        QList<QByteArray> colorData = line.split(',');
			Color c;
			c.r = colorData[0].toInt() / 255.0;
			c.g = colorData[1].toInt() / 255.0;
			c.b = colorData[2].toInt() / 255.0;
			c.a = colorData[3].toInt() / 255.0;
			caneColorList.append(c);
                }
                else
                {
			currentBrand = line;
                }
        }
	file.close();      
 
        for (int i = 0; i < caneColorList.size(); ++i)
        {
		if (caneNameList[i][0] != 'R') // Only load reichenbach colors for now, bug if list is too large
			break;
		QString name(caneNameList[i]);
                PureColorLibraryWidget* pclw = new PureColorLibraryWidget(caneColorList[i], name, this);
                colorLibraryLayout->addWidget(pclw, 1);
        }

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



