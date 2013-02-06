

#include <QtGui>
#include "pullplan.h"
#include "niceviewwidget.h"
#include "geometry.h"
#include "mesh.h"
#include "purecolorlibrarywidget.h"
#include "glasscolor.h"
#include "colorreader.h"
#include "coloreditorwidget.h"

ColorEditorWidget :: ColorEditorWidget(QWidget* parent) : QWidget(parent)
{
	resetGlassColor();

	geometry = new Geometry();

	niceViewWidget = new NiceViewWidget(NiceViewWidget::PULLPLAN_CAMERA_MODE, this);
	generateColorMesh(glassColor, geometry);
	niceViewWidget->setGeometry(geometry);

	setupLayout();
	setupConnections();

	// fake selecting the first list for initialization
	collectionComboBox->setCurrentIndex(0);
}

void ColorEditorWidget :: resetGlassColor()
{
	glassColor = new GlassColor();
}

GlassColor* ColorEditorWidget :: getGlassColor() {

	return glassColor;
}

void ColorEditorWidget :: setGlassColor(GlassColor* _gc) {
	glassColor = _gc;
}


void ColorEditorWidget :: collectionComboBoxChanged(int _index)
{
	if (_index < collectionComboBox->count() - 1) // if it's not the ``Add collection...'' one
	{
		prevCollection = _index;
		collectionStack->setCurrentIndex(_index);
	}
	else
	{
		// immediately reset back to previously selected collection, 
		// faking the fact that ``Add collection...'' should not be 
		// a selectable option like the real collections.
		collectionComboBox->setCurrentIndex(prevCollection);
		QString fileName = QFileDialog::getOpenFileName(this, "Load Color Library", "", 
			"VirtualGlass Color Libraries (*.vgc)");

		if (!fileName.isNull())
		{
			loadCollection(fileName);
			collectionComboBox->setCurrentIndex(collectionComboBox->count()-2);
		}
	}
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
	collectionComboBox = new QComboBox(this);
	collectionComboBox->setDuplicatesEnabled(true);
	sourceLayout->addWidget(collectionComboBox, 1);

	collectionComboBox->addItem("Add collection...");

	collectionStack = new QStackedWidget(this);
	editorLayout->addWidget(collectionStack);

	loadCollection(":/reichenbach-opaque-colors.vgc");
	loadCollection(":/reichenbach-transparent-colors.vgc");
	loadCollection(":/kugler-opaque-colors.vgc");
	loadCollection(":/kugler-transparent-colors.vgc");
	loadCollection(":/gaffer-opaque-colors.vgc");
	loadCollection(":/gaffer-transparent-colors.vgc");

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
	connect(collectionComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(collectionComboBoxChanged(int)));
	connect(alphaSlider, SIGNAL(valueChanged(int)), this, SLOT(alphaSliderPositionChanged(int)));
}

void ColorEditorWidget :: loadCollection(QString fileName)
{
	QFile file(fileName);
	file.open(QIODevice::ReadOnly | QIODevice::Text);
	// Add the list name to the dropdown menu of lists
	if (file.atEnd())
	{
		file.close();
		return;
	}
	else
	{
		// First line of file is collection name
		collectionComboBox->insertItem(collectionComboBox->count()-1, file.readLine().trimmed());
	}

	// This part sets up the necessary GUI parts 
	// related to the color list.
	QScrollArea* listScrollArea = new QScrollArea(collectionStack);
	colorLibraryScrollAreas.push_back(listScrollArea); // push the new scroll area
	collectionStack->addWidget(listScrollArea); // add this to the layout
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

	// Here we now actually read the list from the file and
	// create a list of GUI labels for the colors
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
			// if there's no second line for RGB values, quit
			if (file.atEnd())
			{
				file.close();
				return;
			}
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
	generateColorMesh(glassColor, geometry);
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



