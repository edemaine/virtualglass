
#include "coloreditorwidget.h"

ColorEditorWidget :: ColorEditorWidget(QWidget* parent) : QWidget(parent)
{
	Color* color = new Color;
	color->r = color->g = color->b = 1.0;
	color->a = 0.0;
        this->colorBar = new PullPlan(CIRCLE_BASE_PULL_TEMPLATE, color);
	this->niceViewWidget = new NiceViewWidget(PULLPLAN_MODE, this);
	mesher.generateMesh(colorBar, &geometry);
	niceViewWidget->setGeometry(&geometry);

	setupLayout();
	setupConnections();
}

void ColorEditorWidget :: setupLayout()
{
        QHBoxLayout* pageLayout = new QHBoxLayout(this);
        this->setLayout(pageLayout);
        QVBoxLayout* editorLayout = new QVBoxLayout(this);
        pageLayout->addLayout(editorLayout);

        QTabWidget* tabs = new QTabWidget(this);
        editorLayout->addWidget(tabs);
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
        pageLayout->addWidget(niceViewWidget, 10);

        // Little description for the editor
        QLabel* descriptionLabel = new QLabel("Color editor", this);
        descriptionLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        editorLayout->addWidget(descriptionLabel, 0);
}

void ColorEditorWidget :: setupConnections()
{
	connect(this, SIGNAL(someDataChanged()), this, SLOT(updateEverything()));
}

void ColorEditorWidget :: updateLibraryWidgetPixmaps(ColorBarLibraryWidget* w)
{
	QPixmap editorPixmap(100, 100);
	editorPixmap.fill(QColor(255*colorBar->color->r,
		255*colorBar->color->g,
		255*colorBar->color->b,
		MAX(255*colorBar->color->a, 255*0.05)));
	w->updatePixmaps(
		QPixmap::fromImage(niceViewWidget->renderImage()).scaled(100, 100),
		editorPixmap);
}

void ColorEditorWidget :: setColor(float r, float g, float b, float a)
{
	colorBar->color->r = r;	
	colorBar->color->g = g;	
	colorBar->color->b = b;	
	colorBar->color->a = a;	
	emit someDataChanged();	
}

Color* ColorEditorWidget :: getColor()
{
	return colorBar->color;
}

PullPlan* ColorEditorWidget :: getColorBar()
{
	return colorBar;
}


void ColorEditorWidget :: seedColors()
{ 
	Color caneColor;
	QString caneName;

        QFile file(":/Colors1.txt");
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

void ColorEditorWidget :: alphaSliderPositionChanged(int)
{
	if (alphaSlider->sliderPosition() != (int) (colorBar->color->a * 255))
	{
		colorBar->color->a = alphaSlider->sliderPosition() / 255.0;
		emit someDataChanged();
	} 
}

void ColorEditorWidget :: mousePressEvent(QMouseEvent* event)
{
        PureColorLibraryWidget* pclw = dynamic_cast<PureColorLibraryWidget*>(childAt(event->pos()));
	if (pclw != NULL)
	{
		*(colorBar->color) = pclw->getColor();
		this->alphaSlider->setSliderPosition((int) (colorBar->color->a * 255));
		emit someDataChanged();	
	}
}

void ColorEditorWidget :: unhighlightLibraryWidget(PureColorLibraryWidget* w)
{
        w->graphicsEffect()->setEnabled(false);
}

void ColorEditorWidget :: highlightLibraryWidget(PureColorLibraryWidget* w)
{
        w->graphicsEffect()->setEnabled(false);
        ((QGraphicsHighlightEffect*) w->graphicsEffect())->setHighlightType(IS_DEPENDANCY);
        w->graphicsEffect()->setEnabled(true);
}

void ColorEditorWidget :: setColorBar(PullPlan* b)
{
	this->colorBar = b;
	emit someDataChanged();
}

void ColorEditorWidget :: updateEverything()
{
        geometry.clear();
        mesher.generateMesh(colorBar, &geometry);
        niceViewWidget->repaint();

	this->alphaSlider->setSliderPosition((int) (colorBar->color->a * 255));

        QLayoutItem* w;
	PureColorLibraryWidget* pclw;
	Color* pColor;
	for (int j = 0; j < colorLibrary1Layout->count(); ++j)
        {
                w = colorLibrary1Layout->itemAt(j);
                if(dynamic_cast<QWidgetItem *>(w))
		{
			pclw = dynamic_cast<PureColorLibraryWidget*>(w->widget());
			pColor = colorBar->color;
			if (pclw->getColor().r == pColor->r &&
				pclw->getColor().g == pColor->g &&
				pclw->getColor().b == pColor->b) 
				highlightLibraryWidget(pclw);
			else
				unhighlightLibraryWidget(pclw);
		}
	}
	for (int j = 0; j < colorLibrary2Layout->count(); ++j)
        {
                w = colorLibrary2Layout->itemAt(j);
                if(dynamic_cast<QWidgetItem *>(w))
		{
			pclw = dynamic_cast<PureColorLibraryWidget*>(w->widget());
			pColor = colorBar->color;
			if (pclw->getColor().r == pColor->r &&
				pclw->getColor().g == pColor->g &&
				pclw->getColor().b == pColor->b) 
				highlightLibraryWidget(pclw);
			else
				unhighlightLibraryWidget(pclw);
		}
        }
}



