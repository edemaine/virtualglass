
#include "coloreditorwidget.h"

ColorEditorWidget :: ColorEditorWidget(GlassColor* _glassColor, QWidget* parent) : QWidget(parent)
{
	niceViewWidget = new NiceViewWidget(NiceViewWidget::PULLPLAN_CAMERA_MODE, this);
	glassColor = _glassColor;
	mesher.generateColorMesh(glassColor, &geometry);
	niceViewWidget->setGeometry(&geometry);

	setupLayout();
	setupConnections();

	colorLibraryScrollAreas[0]->show(); 
}


GlassColor* ColorEditorWidget :: getGlassColor() {

	return glassColor;
}

void ColorEditorWidget :: setGlassColor(GlassColor* _gc) {

	glassColor = _gc;
}


void ColorEditorWidget :: sourceComboBoxChanged(int)
{
	for (unsigned int i = 0; i < colorLibraryScrollAreas.size(); ++i)
	{
		colorLibraryScrollAreas[i]->hide();
	}
	colorLibraryScrollAreas[sourceComboBox->currentIndex()]->show();
}

void ColorEditorWidget :: setupLayout()
{
	QHBoxLayout* pageLayout = new QHBoxLayout(this);
	this->setLayout(pageLayout);
	QVBoxLayout* editorLayout = new QVBoxLayout(this);
	pageLayout->addLayout(editorLayout);

	QHBoxLayout* sourceLayout = new QHBoxLayout(this);
	editorLayout->addLayout(sourceLayout);
	sourceLayout->addWidget(new QLabel("Collection:", this), 0);
	sourceComboBox = new QComboBox(this);
	sourceLayout->addWidget(sourceComboBox, 1);
	sourceComboBox->addItem("Reichenbach - Transparents");
	sourceComboBox->addItem("Reichenbach - Opaques");
	sourceComboBox->addItem("Gaffer - Transparents");
	sourceComboBox->addItem("Gaffer - Opals");
	sourceComboBox->addItem("Kugler - Transparents");
	sourceComboBox->addItem("Kugler - Opaques");

	for (int i = 0; i < 6; ++i)
	{
		colorLibraryScrollAreas.push_back(new QScrollArea(this));
		editorLayout->addWidget(colorLibraryScrollAreas[i]);
		colorLibraryScrollAreas[i]->setBackgroundRole(QPalette::Dark);
		colorLibraryScrollAreas[i]->setWidgetResizable(true);
		colorLibraryScrollAreas[i]->setMinimumWidth(320);
		colorLibraryScrollAreas[i]->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		colorLibraryScrollAreas[i]->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
		QWidget* colorLibraryWidget = new QWidget(colorLibraryScrollAreas[i]);
		colorLibraryScrollAreas[i]->setWidget(colorLibraryWidget);
		colorLibraryLayouts.push_back(new QVBoxLayout(colorLibraryWidget));
		colorLibraryLayouts[i]->setSpacing(10);
		colorLibraryWidget->setLayout(colorLibraryLayouts[i]);
		colorLibraryScrollAreas[i]->hide();
	}

	QHBoxLayout* alphaLayout = new QHBoxLayout(this);
	editorLayout->addLayout(alphaLayout);
	QLabel* alphaLabel1 = new QLabel("Transparency:", this);
	alphaLayout->addWidget(alphaLabel1, 0);
	QLabel* alphaLabel2 = new QLabel("0%", this);
	alphaLayout->addWidget(alphaLabel2, 0);
	alphaSlider = new QSlider(Qt::Horizontal, this);
	alphaSlider->setRange(0, 255);
	alphaLayout->addWidget(alphaSlider);
	QLabel* alphaLabel3 = new QLabel("100%", this);
	alphaLayout->addWidget(alphaLabel3, 0);
	pageLayout->addWidget(niceViewWidget, 10);

	// Little description for the editor
	QLabel* descriptionLabel = new QLabel("Color editor.", this);
	descriptionLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	editorLayout->addWidget(descriptionLabel, 0);
}

void ColorEditorWidget :: setupConnections()
{
	connect(this, SIGNAL(someDataChanged()), this, SLOT(updateEverything()));
	connect(sourceComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(sourceComboBoxChanged(int)));
	connect(alphaSlider, SIGNAL(valueChanged(int)), this, SLOT(alphaSliderPositionChanged(int)));
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
			if (colorData[3].toInt() != 255) // if a transparent
			{
				if (caneName[0] ==  'R')
					colorLibraryLayouts[0]->addWidget(pclw);
				else if (caneName[0] == 'G')
					colorLibraryLayouts[2]->addWidget(pclw);
				else if (caneName[0] == 'K')
					colorLibraryLayouts[4]->addWidget(pclw);
			}
			else
			{
				if (caneName[0] ==  'R')
					colorLibraryLayouts[1]->addWidget(pclw);
				else if (caneName[0] == 'G')
					colorLibraryLayouts[3]->addWidget(pclw);
				else if (caneName[0] == 'K')
					colorLibraryLayouts[5]->addWidget(pclw);
			}
		}
	}
	file.close();
}

void ColorEditorWidget :: alphaSliderPositionChanged(int)
{
	if (alphaSlider->sliderPosition() != (int) (glassColor->getColor()->a * 255))
	{
		glassColor->getColor()->a = (255 - alphaSlider->sliderPosition()) / 255.0;
		emit someDataChanged();
	} 
}

void ColorEditorWidget :: mousePressEvent(QMouseEvent* event)
{
	PureColorLibraryWidget* pclw = dynamic_cast<PureColorLibraryWidget*>(childAt(event->pos()));
	if (pclw != NULL)
	{
		glassColor->setColor(pclw->getColor());
		glassColor->setName(pclw->getColorName().split(' ')[0]);
		this->alphaSlider->setSliderPosition(255 - int(glassColor->getColor()->a * 255));
		emit someDataChanged();	
	}
}

void ColorEditorWidget :: updateEverything()
{
	geometry.clear();
	mesher.generateColorMesh(glassColor, &geometry);
	niceViewWidget->repaint();

	this->alphaSlider->setSliderPosition(255 - (int) (glassColor->getColor()->a * 255));

	QLayoutItem* w;
	PureColorLibraryWidget* pclw;
	for (unsigned int i = colorLibraryLayouts.size() - 1; i <= colorLibraryLayouts.size(); --i)
	{
		for (int j = 0; j < colorLibraryLayouts[i]->count(); ++j)
		{
			// this generically checks that the RGBa values are the same, 
			// but we could do something fancier, checking that the names match
			w = colorLibraryLayouts[i]->itemAt(j);
			pclw = dynamic_cast<PureColorLibraryWidget*>(w->widget());
			if (glassColor->getName() == pclw->getColorName().split(' ')[0])
				pclw->setSelected(true);
			else
				pclw->setSelected(false);
		}
	}
}



