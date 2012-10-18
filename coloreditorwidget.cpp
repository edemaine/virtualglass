

#include <QtGui>
#include "pullplan.h"
#include "niceviewwidget.h"
#include "geometry.h"
#include "mesh.h"
#include "purecolorlibrarywidget.h"
#include "glasscolor.h"
#include "colorreader.h"
#include "coloreditorwidget.h"

ColorEditorWidget :: ColorEditorWidget(GlassColor* _glassColor, QWidget* parent) : QWidget(parent)
{
	geometry = new Geometry();
	mesher = new Mesher();

	niceViewWidget = new NiceViewWidget(NiceViewWidget::PULLPLAN_CAMERA_MODE, this);
	glassColor = _glassColor;
	mesher->generateColorMesh(glassColor, geometry);
	niceViewWidget->setGeometry(geometry);

	setupLayout();
	setupConnections();

	// fake selecting the first list just to show *some* list 
	// (rather than seeing the default hidden state of them all)
	sourceComboBoxChanged(0);
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
	editorLayout = new QVBoxLayout(this);
	pageLayout->addLayout(editorLayout);

	QHBoxLayout* sourceLayout = new QHBoxLayout(this);
	editorLayout->addLayout(sourceLayout);
	sourceLayout->addWidget(new QLabel("Collection:", this), 0);
	sourceComboBox = new QComboBox(this);
	sourceLayout->addWidget(sourceComboBox, 1);

	// for each file containing colors
	// read it, make [scroll area, widget, qvboxlayout], add title to combo box	
	loadColors(":/reichenbach-opaque-colors.txt");
	loadColors(":/reichenbach-transparent-colors.txt");
	loadColors(":/kugler-opaque-colors.txt");
	loadColors(":/kugler-transparent-colors.txt");
	loadColors(":/gaffer-opaque-colors.txt");
	loadColors(":/gaffer-transparent-colors.txt");

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

void ColorEditorWidget :: loadColors(QString fileName)
{
	// This part sets up the necessary GUI parts 
	// related to the color list.
	QScrollArea* listScrollArea = new QScrollArea(this);
	colorLibraryScrollAreas.push_back(listScrollArea); // push the new scroll area
	editorLayout->addWidget(listScrollArea); // add this to the layout
	listScrollArea->setBackgroundRole(QPalette::Dark);
	listScrollArea->setWidgetResizable(true);
	listScrollArea->setMinimumWidth(320);
	listScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	listScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

	QWidget* colorLibraryWidget = new QWidget(listScrollArea);
	listScrollArea->setWidget(colorLibraryWidget);

	QVBoxLayout* listLayout = new QVBoxLayout(colorLibraryWidget);
	colorLibraryLayouts.push_back(listLayout);
	listLayout->setSpacing(10);
	colorLibraryWidget->setLayout(listLayout);
        listScrollArea->hide();

	// Here we now actually read the list from the file and
	// create a list of GUI labels for the colors
	QFile file(fileName);
	file.open(QIODevice::ReadOnly | QIODevice::Text);
	// Add the list name to the dropdown menu of lists
	sourceComboBox->addItem(file.readLine().trimmed());

	// Create each library item and add it to the listLayout
	Color colorRGB;
	QString colorName;
	while (!file.atEnd())
	{
		QString line = file.readLine();
		line = line.trimmed();
		if (line.isEmpty())
			continue;

		if (line.at(0) == '[')
		{
			colorName = lineToColorName(line);
			colorRGB = lineToColorRGB(file.readLine()); 
			PureColorLibraryWidget* pclw = new PureColorLibraryWidget(colorRGB, colorName, this);
			listLayout->addWidget(pclw);
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
		glassColor->setName(shortColorName(pclw->getColorName()));
		this->alphaSlider->setSliderPosition(255 - int(glassColor->getColor()->a * 255));
		emit someDataChanged();	
	}
}

void ColorEditorWidget :: updateEverything()
{
	geometry->clear();
	mesher->generateColorMesh(glassColor, geometry);
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



