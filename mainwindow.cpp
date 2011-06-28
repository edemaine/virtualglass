#include "mainwindow.h"
#include <fstream>

MainWindow::MainWindow(Model* model)
{
	this->model = model;
	centralWidget = new QWidget(this);
	this->setCentralWidget(centralWidget);
	librarySize = 0;

	windowLayout = new QVBoxLayout(centralWidget);
	setupWorkArea();
	setupLibraryArea();
	setupStatusBar();
	setWindowTitle(tr("Virtual Glass"));

	resize(1000, 750);
	move(75,25);
}

void MainWindow::modeChangedSlot(int mode)
{
	switch (mode)
	{
	case PULL_MODE:
		modeLabel->setText("PULL MODE");
		break;
	case BUNDLE_MODE:
		modeLabel->setText("BUNDLE MODE");
		break;
	case FLATTEN_MODE:
		modeLabel->setText("FLATTEN MODE");
		break;
	case LOOK_MODE:
		modeLabel->setText("LOOK MODE");
		break;
	default:
		modeLabel->setText("UNKNOWN MODE");
		break;
	}

}

void MainWindow::libraryCaneDestroyed(QObject* obj)
{
	stockLayout->removeWidget((QWidget*) obj);
}

void MainWindow::textMessageSlot(QString msg)
{
	statusBar->showMessage(msg, 2000);
}

void MainWindow::saveCaneToLibrarySlot()
{
	if (model->getCane() == NULL)
		return;

	LibraryCaneWidget* lc = new LibraryCaneWidget((OpenGLWidget*) this->openglWidget, this->model, 0);
	stockLayout->addWidget(lc);
	connect(stockLayout, SIGNAL(destroyed(QObject*)), this, SLOT(libraryCaneDestroyed(QObject*)));
}

void MainWindow::setupStatusBar()
{
	statusBar = new QStatusBar(this);
	modeLabel = new QLabel("????",statusBar);
	statusBar->addPermanentWidget(modeLabel);
	setStatusBar(statusBar);
}

void MainWindow::setupLibraryArea()
{
	QWidget* stockWidget = new QWidget;
	stockLayout = new QHBoxLayout(stockWidget);
	stockLayout->setSpacing(10);

	libraryScrollArea = new QScrollArea;
	libraryScrollArea->setBackgroundRole(QPalette::Dark);
	libraryScrollArea->setWidget(stockWidget);
	libraryScrollArea->setWidgetResizable(true);
	libraryScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	libraryScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	libraryScrollArea->setFixedHeight(130);

	windowLayout->addWidget(libraryScrollArea);
}

void MainWindow::seedLibrary()
{
	// Now seed the library with some basic canes
	Cane* c = new Cane(BASE_CIRCLE_CANETYPE);
	Cane* stretch = new Cane(PULL_CANETYPE);

	stretch->subcaneCount = 1;
	stretch->subcanes[0] = c;
	stretch->amts[0] = 0.0;
	stretch->amts[1] = 100.0; // amts[0] = twist, amts[1] = stretch

	c->color.r = 0.8;
	c->color.g = 0.8;
	c->color.b = 0.8;
	c->color.a = 0.3;

	model->setCane(stretch);
	emit saveCaneToLibrarySig();
	c->color.r = 1.0;
	c->color.g = 0.5;
	c->color.b = 0.5;
	c->color.a = 0.7;
	model->setCane(stretch);
	emit saveCaneToLibrarySig();
	c->color.r = 0.5;
	c->color.g = 1.0;
	c->color.b = 0.5;
	model->setCane(stretch);
	emit saveCaneToLibrarySig();
	c->color.r = 0.5;
	c->color.g = 0.5;
	c->color.b = 1.0;
	model->setCane(stretch);
	emit saveCaneToLibrarySig();

	model->setCane(NULL);
	emit textMessageSig("Default library loaded");
}

void MainWindow::exportLibraryButtonPressed()
{
	QString fileName =  QFileDialog::getSaveFileName();
	QList<LibraryCaneWidget*> libraryList = libraryScrollArea->findChildren<LibraryCaneWidget*>();

	YAML::Emitter out;
	out << libraryList.size();
	out << YAML::BeginSeq;

	for (int i=0;i<libraryList.size();i++)
	{
		Cane* cane = libraryList.at(i)->getCane();
		out << YAML::Literal << cane->yamlRepresentation();
	}

	out << YAML::EndSeq;

	QFile file(fileName);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
		return;

	file.reset();

	QTextStream outStream(&file);
	outStream << out.c_str() << "\n";
	outStream.flush();
	file.close();

	emit textMessageSig("Library Saved to: " + fileName);
}

void MainWindow::loadLibraryCane(const YAML::Node& node, Cane* cane)
{

	std::string caneLiteral;
	node.GetScalar(caneLiteral);

	istringstream sstream ( caneLiteral );
	YAML::Parser newParser (sstream);
	YAML::Node newNode;
	newParser.GetNextDocument(newNode);

	newNode["Type"] >> cane->type;

	const YAML::Node& caneAmts = newNode["Amounts"];

	int amtsCount=0;
	for(YAML::Iterator it2=caneAmts.begin();it2!=caneAmts.end();++it2) {
		*it2 >> cane->amts[amtsCount];
		amtsCount++;
	}

	newNode["SubCaneCount"] >> cane->subcaneCount;

	const YAML::Node& subLocations = newNode["SubCaneLocations"];
	int subLocationCount=0;
	for(YAML::Iterator it3=subLocations.begin();it3!=subLocations.end();++it3) {
		const YAML::Node& subCaneLocation = *it3;
		subCaneLocation[0] >> cane->subcaneLocations[subLocationCount].x;
		subCaneLocation[1] >> cane->subcaneLocations[subLocationCount].y;
		subCaneLocation[2] >> cane->subcaneLocations[subLocationCount].z;
		subLocationCount++;
	}

	newNode["Color"][0] >> cane->color.r;
	newNode["Color"][1] >> cane->color.g;
	newNode["Color"][2] >> cane->color.b;
	newNode["Color"][3] >> cane->color.a;

	const YAML::Node& subCanes = newNode["SubCanes"];
	int subCaneCount = 0;
	for(YAML::Iterator it4=subCanes.begin();it4!=subCanes.end();++it4) {
		const YAML::Node& subCane = *it4;
		Cane* loadCane = new Cane(UNASSIGNED_CANETYPE);
		loadLibraryCane(subCane,loadCane);
		cane->subcanes[subCaneCount]=loadCane;
		subCaneCount++;
	}
}

void MainWindow::importLibraryButtonPressed()
{
	QString fileName = QFileDialog::getOpenFileName();

	std::ifstream fin(fileName.toStdString().c_str());
	YAML::Parser parser(fin);

	YAML::Node doc;
	parser.GetNextDocument(doc);
	parser.GetNextDocument(doc);

	for(unsigned i=0;i<doc.size();i++) {
		Cane loadCane = Cane(UNASSIGNED_CANETYPE);
		loadLibraryCane(doc[i],&loadCane);
		emit setCaneSig(&loadCane);
		emit saveCaneToLibrarySig();
	}

	emit setCaneSig(NULL);
	emit textMessageSig("Library loaded from: " + fileName);
}

void MainWindow::newColorPickerCaneButtonPressed()
{
	colorPickerSelected(QColorDialog::getColor());
}

void MainWindow::changeBgColorButtonPressed()
{
	openglWidget->setBgColor(QColorDialog::getColor());
}

void MainWindow::saveObjButtonPressed()
{
	QString file = QFileDialog::getSaveFileName(this, tr("Save obj file"), "", tr("Wavefront obj files (*.obj);;All files (*)"));
	if (!file.isNull())
	{
		openglWidget->saveObjFile(qPrintable(file));
	}
}

void MainWindow::colorPickerSelected(QColor color)
{
	emit saveCaneToLibrarySig();
	emit setCaneSig(NULL);

	Cane* c = new Cane(BASE_CIRCLE_CANETYPE);
	Cane* stch = new Cane(PULL_CANETYPE);

	stch->subcaneCount = 1;
	stch->subcanes[0] = c;
	stch->amts[1] = 100.0;

	c->color.r = color.redF();
	c->color.g = color.greenF();
	c->color.b = color.blueF();
	c->color.a = color.alphaF();

	emit setCaneSig(stch);
}

void MainWindow::setupWorkArea()
{
	openglWidget = new OpenGLWidget(this, model);

	look_button = new QPushButton("Look");
	zoom_in_button = new QPushButton("Zoom In");
	zoom_out_button = new QPushButton("Zoom Out");
	frontView_button = new QPushButton("Front View");
	topView_button = new QPushButton("Top View");
	sideView_button = new QPushButton("Side View");
	switchView_button = new QPushButton("Switch View");
	switchView_button->setToolTip("Switch between perspective and orthographic views");
	toggle_axes_button = new QPushButton("Toggle Axes");
	toggle_grid_button = new QPushButton("Toggle Grid");

	QVBoxLayout* viewButton_layout = new QVBoxLayout();
	viewButton_layout->addWidget(frontView_button);
	viewButton_layout->addWidget(topView_button);
	viewButton_layout->addWidget(sideView_button);
	viewButton_layout->addWidget(switchView_button);
	viewButton_layout->addWidget(zoom_in_button);
	viewButton_layout->addWidget(zoom_out_button);
	viewButton_layout->addWidget(toggle_axes_button);
	viewButton_layout->addWidget(toggle_grid_button);
	QWidget* viewButtonWidget = new QWidget();
	viewButtonWidget->setLayout(viewButton_layout);

	pull_button = new QPushButton("Pull");
	pull_button->setToolTip("Drag Mouse Horizontally to Twist, Vertically to Stretch. Use Shift to twist and stretch independently.");
	bundle_button = new QPushButton("Bundle");
	flatten_button = new QPushButton("Flatten");
	flatten_button->setToolTip("Drag Mouse Horizontally to Squish, Vertically to Flatten");
	wrap_button = new QPushButton("Wrap");
	wrap_button->setToolTip("Not Implemented - Select must be functional first");
	undo_button = new QPushButton("Undo");
	undo_button->setToolTip("Undo the last operation.");

	QVBoxLayout* operButton_layout = new QVBoxLayout();
	operButton_layout->addWidget(pull_button);
	operButton_layout->addWidget(bundle_button);
	operButton_layout->addWidget(flatten_button);
	operButton_layout->addWidget(wrap_button);
	operButton_layout->addWidget(undo_button);

	QWidget* operButtonWidget = new QWidget();
	operButtonWidget->setLayout(operButton_layout);
	next_button = new QPushButton("Next");
	next_button->setToolTip("Next Cane in Current Model");
	save_button = new QPushButton("Save");
	save_button->setToolTip("Save Current Model to Library");
	clear_button = new QPushButton("Clear");
	clear_button->setToolTip("Clear Current Model");
	exportLibrary_button = new QPushButton("Export Library");
	exportLibrary_button->setToolTip("Save Library to File");
	importLibrary_button = new QPushButton("Import Library");
	importLibrary_button->setToolTip("Load Library from File");
	colorPicker_button = new QPushButton("New Cane Color");
	colorPicker_button->setToolTip("Create a New Cane with specified color");
	bgColorPicker_button = new QPushButton("Change Background Color");
	saveObj_button = new QPushButton("Save .obj file");
	saveObj_button->setToolTip("Save an .obj file with the current cane geometry for rendering in an external program.");

	QVBoxLayout* utilButton_layout = new QVBoxLayout();
	utilButton_layout->addWidget(next_button);
	utilButton_layout->addWidget(save_button);
	utilButton_layout->addWidget(clear_button);
	utilButton_layout->addWidget(exportLibrary_button);
	utilButton_layout->addWidget(importLibrary_button);
	utilButton_layout->addWidget(colorPicker_button);
	utilButton_layout->addWidget(bgColorPicker_button);
	utilButton_layout->addWidget(saveObj_button);
	QWidget* utilButtonWidget = new QWidget();
	utilButtonWidget->setLayout(utilButton_layout);

	tabWidget = new QTabWidget();
	tabWidget->setTabPosition(QTabWidget::North);
	tabWidget->addTab(viewButtonWidget,"View");
	tabWidget->addTab(operButtonWidget,"Operations");
	tabWidget->addTab(utilButtonWidget,"Util");
	//connect(tabWidget, SIGNAL(currentChanged(int)), this, SLOT(modeSelect(int)));

	QVBoxLayout* button_layout = new QVBoxLayout();
	button_layout->addWidget(tabWidget);

	QHBoxLayout* workLayout = new QHBoxLayout();
	workLayout->addLayout(button_layout);
	workLayout->addWidget(openglWidget, 1);
	windowLayout->addLayout(workLayout, 5);
}

void MainWindow::keyPressEvent(QKeyEvent* e)
{
	switch (e->key())
	{
	case 0x58: // X
		exit(0);
	case 0x01000020: // Shift
		openglWidget->setShiftButtonDown(true);
		break;
	default:
		break;
	}
}

void MainWindow::keyReleaseEvent(QKeyEvent* e)
{
	switch(e->key())
	{
	case 0x01000020: // Shift
		openglWidget->setShiftButtonDown(false);
		break;
	default:
		break;
	}
}

