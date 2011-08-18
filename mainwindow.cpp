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
	setupMenuBar();
	setupNewColorPickerCaneDialog();
	setupNewBrandCaneDialog();
	setWindowTitle(tr("Virtual Glass"));

	resize(1000, 750);
	move(75,25);
}

void MainWindow::setupMenuBar()
{
	fileMenu = menuBar()->addMenu(tr("&File"));

	QAction* importLibrary = new QAction(tr("&Import Library"), this);
	importLibrary->setStatusTip(tr("Load a saved library of canes"));
	connect(importLibrary, SIGNAL(triggered()), this, SLOT(importLibraryDialog()));
	fileMenu->addAction(importLibrary);

	QAction* exportLibrary = new QAction(tr("&Export Library"), this);
	exportLibrary->setStatusTip(tr("Save the current library of canes to a file"));
	connect(exportLibrary, SIGNAL(triggered()), this, SLOT(exportLibraryDialog()));
	fileMenu->addAction(exportLibrary);

	fileMenu->addSeparator();

	QAction* importCane = new QAction(tr("&Import Cane"), this);
	importCane->setStatusTip(tr("Load a saved cane"));
	connect(importCane, SIGNAL(triggered()), this, SLOT(importCaneDialog()));
	fileMenu->addAction(importCane);

	QAction* exportCane = new QAction(tr("&Export Cane"), this);
	exportCane->setStatusTip(tr("Save the current cane to a file"));
	connect(exportCane, SIGNAL(triggered()), this, SLOT(exportCaneDialog()));
	fileMenu->addAction(exportCane);

	fileMenu->addSeparator();

	QAction* exportObj = new QAction(tr("&Export to .obj"), this);
	exportObj->setStatusTip(tr("Save the geometry of the current cane as a .obj file"));
	connect(exportObj, SIGNAL(triggered()), this, SLOT(saveObjFileDialog()));
	fileMenu->addAction(exportObj);

	QAction* exportRaw = new QAction(tr("&Export raw"), this);
	exportRaw->setStatusTip(tr("Save the geometry of the current cane as a .raw file for the visualizer"));
	connect(exportRaw, SIGNAL(triggered()), this, SLOT(saveRawFile()));
	fileMenu->addAction(exportRaw);


	viewMenu = menuBar()->addMenu(tr("&View"));

	QAction* toggleRecipe = new QAction(tr("&Recipe View"), this);
	toggleRecipe->setStatusTip(tr("Switch between 3D view and recipe view"));
	toggleRecipe->setCheckable(true);
	toggleRecipe->setChecked(false);
	//connect(toggleRecipe, SIGNAL(triggered()), openglWidget, SLOT(toggleClickable()));
	connect(toggleRecipe, SIGNAL(triggered()), this, SLOT(toggleRecipe()));
	isRecipe = false;
	viewMenu->addAction(toggleRecipe);

	viewMenu->addSeparator();

	QAction* toggleAxes = new QAction(tr("&Show Axes"), this);
	toggleAxes->setStatusTip(tr("Show the reference lines on the X, Y, and Z axes."));
	toggleAxes->setCheckable(true);
	toggleAxes->setChecked(true);
	openglWidget->setAxes(true);
	connect(toggleAxes, SIGNAL(triggered()), openglWidget, SLOT(toggleAxes()));
	viewMenu->addAction(toggleAxes);

	QAction* toggleGrid = new QAction(tr("&Show Grid"), this);
	toggleGrid->setStatusTip(tr("Show the reference grid."));
	toggleGrid->setCheckable(true);
	toggleGrid->setChecked(false);
	openglWidget->setGrid(false);
	connect(toggleGrid, SIGNAL(triggered()), openglWidget, SLOT(toggleGrid()));
	viewMenu->addAction(toggleGrid);

	QAction* toggleSnaps = new QAction(tr("&Show Snaps"), this);
	toggleSnaps->setStatusTip(tr("Show and activate the snaps."));
	toggleSnaps->setCheckable(true);
	toggleSnaps->setChecked(true);
	openglWidget->setSnaps(true);
	connect(toggleSnaps, SIGNAL(triggered()), openglWidget, SLOT(toggleSnaps()));
	viewMenu->addAction(toggleSnaps);

	QAction* toggleRefSnaps = new QAction(tr("&Show Auxiliary Snaps"), this);
	toggleRefSnaps->setStatusTip(tr("Show auxiliary snaps."));
	toggleRefSnaps->setCheckable(true);
	toggleRefSnaps->setChecked(true);
	openglWidget->setRefSnaps(true);
	connect(toggleRefSnaps, SIGNAL(triggered()), openglWidget, SLOT(toggleRefSnaps()));
	viewMenu->addAction(toggleRefSnaps);

	viewMenu->addSeparator();

	QMenu* specialViewMenu = viewMenu->addMenu(tr("&Axis View"));

	QAction* topView = new QAction(tr("&Top View"), this);
	topView->setStatusTip(tr("View the cane from above."));
	connect(topView, SIGNAL(triggered()), openglWidget, SLOT(setTopView()));
	specialViewMenu->addAction(topView);

	QAction* sideView = new QAction(tr("&Side View"), this);
	sideView->setStatusTip(tr("View the cane from the side."));
	connect(sideView, SIGNAL(triggered()), openglWidget, SLOT(setSideView()));
	specialViewMenu->addAction(sideView);

	QAction* frontView = new QAction(tr("&Front View"), this);
	frontView->setStatusTip(tr("View the cane from the front."));
	connect(frontView, SIGNAL(triggered()), openglWidget, SLOT(setFrontView()));
	specialViewMenu->addAction(frontView);

	viewMenu->addSeparator();

	QAction* switchProjection = new QAction(tr("&Switch Projection"), this);
	switchProjection->setStatusTip(tr("Switch the projection between perspective to orthographic."));
	connect(switchProjection, SIGNAL(triggered()), openglWidget, SLOT(switchProjection()));
	switchProjection->setToolTip("Not Implemented");
	viewMenu->addAction(switchProjection);

	QAction* toggle2D = new QAction(tr("&Toggle 2D View"), this);
	toggle2D->setStatusTip(tr("Switch between 2D and 3D view."));
	toggle2D->setCheckable(true);
	toggle2D->setChecked(false);
	connect(toggle2D, SIGNAL(triggered()), openglWidget, SLOT(toggle2D()));
	toggle2D->setToolTip("Not Implemented");
	viewMenu->addAction(toggle2D);

	QAction* backgroundColor = new QAction(tr("&Change Background Color"), this);
	backgroundColor->setStatusTip(tr("Change the background color of the cane."));
	connect(backgroundColor, SIGNAL(triggered()), this, SLOT(changeBgColorDialog()));
	viewMenu->addAction(backgroundColor);

	viewMenu->addSeparator();

	QAction* zoomIn = new QAction(tr("&Zoom In"), this);
	zoomIn->setStatusTip(tr("Zoom in the camera."));
	connect(zoomIn, SIGNAL(triggered()), openglWidget, SLOT(zoomIn()));
	viewMenu->addAction(zoomIn);

	QAction* zoomOut = new QAction(tr("&Zoom Out"), this);
	zoomOut->setStatusTip(tr("Zoom in the camera."));
	connect(zoomOut, SIGNAL(triggered()), openglWidget, SLOT(zoomOut()));
	viewMenu->addAction(zoomOut);

	caneMenu = menuBar()->addMenu(tr("&Cane"));

	QAction* newCaneColor = new QAction(tr("&New Cane Color"), this);
	zoomOut->setStatusTip(tr("Create a new cane of desired color."));
	connect(newCaneColor, SIGNAL(triggered()), this, SLOT(newColorPickerCaneDialog()));
	caneMenu->addAction(newCaneColor);

	QAction* newBrandColor = new QAction(tr("&New Cane Color From Brand"), this);
	zoomOut->setStatusTip(tr("Create a new cane using standard colors."));
	connect(newBrandColor, SIGNAL(triggered()), this, SLOT(newBrandCaneDialog()));
	caneMenu->addAction(newBrandColor);
}

void MainWindow::modeChanged(int mode)
{
	switch (mode)
	{
	case PULL_MODE:
		modeLabel->setText("PULL MODE");
		displayTextMessage("Click and drag mouse up and down to stretch cane, left and right to twist cane",0);
		break;
	case BUNDLE_MODE:
		modeLabel->setText("BUNDLE MODE");
		displayTextMessage("Click and drag individual canes to move them",0);
		break;
	case CASING_MODE:
		modeLabel->setText("CASING MODE");
		break;
	case FLATTEN_MODE:
		modeLabel->setText("FLATTEN MODE");
		displayTextMessage("Click and drag mouse up and down to square off cane, left and right to flatten cane",0);
		break;
	case LOOK_MODE:
		modeLabel->setText("LOOK MODE");
		displayTextMessage("Click and drag mouse to re-orient the camera position",0);
		break;
	case WRAP_MODE:
		modeLabel->setText("WRAP MODE");
		displayTextMessage("Click on first cane as the base, click and drag second cane to be the wrap",0);
		break;
	case SNAP_MODE:
		modeLabel->setText("SNAP MODE");
		displayTextMessage("Click to set individual snap points",0);
		break;
	case SNAP_LINE_MODE:
		modeLabel->setText("SNAP LINE MODE");
		displayTextMessage("Click to set individual snap lines",0);
		break;
	case SNAP_CIRCLE_MODE:
		modeLabel->setText("SNAP CIRCLE MODE");
		displayTextMessage("Click to set individual snap circles",0);
		break;
	default:
		modeLabel->setText("UNKNOWN MODE");
		displayTextMessage("Unknown mode not specified",0);
		break;
	}

}

void MainWindow::libraryCaneDestroyed(QObject* obj)
{
	stockLayout->removeWidget((QWidget*) obj);
	updateLibraryToolTip((LibraryCaneWidget*) obj);
}

void MainWindow::displayTextMessage(QString msg)
{
	statusBar->showMessage(msg, 2000);
}
void MainWindow::displayTextMessage(QString msg, int msec)
{
	statusBar->showMessage(msg, msec);
}

void MainWindow::saveCaneToLibrary()
{
	if (model->getCane() == NULL)
		return;

	LibraryCaneWidget* lc = new LibraryCaneWidget((OpenGLWidget*) this->openglWidget, this->model, 0);
	//connect(lc, SIGNAL(newIndex(int)), model->getCane(), SLOT(changeLibraryIndex(int)));
	//lc->setMouseTracking(true);
	stockLayout->addWidget(lc);
	updateLibraryToolTip(lc);
	connect(lc, SIGNAL(mouseOver(LibraryCaneWidget*)), this, SLOT(updateLibraryToolTip(LibraryCaneWidget*)));
	connect(stockLayout, SIGNAL(destroyed(QObject*)), this, SLOT(libraryCaneDestroyed(QObject*)));
}

void MainWindow::updateLibraryToolTip(LibraryCaneWidget *lc)
{
	int result=stockLayout->indexOf(lc);
	lc->setToolTip(QString("Cane #%1").arg(result));
	lc->setAccessibleName(QString("Cane #%1").arg(result));
	lc->newLibraryIndex(result);
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

	//model->setCane(stretch);
	emit setCaneSig(stretch);
	saveCaneToLibrary();
	c->color.r = 1.0;
	c->color.g = 0.5;
	c->color.b = 0.5;
	c->color.a = 0.8;
	//model->setCane(stretch);
	emit setCaneSig(stretch);
	saveCaneToLibrary();
	c->color.r = 0.5;
	c->color.g = 1.0;
	c->color.b = 0.5;
	c->color.a = 0.8;
	//model->setCane(stretch);
	emit setCaneSig(stretch);
	saveCaneToLibrary();
	c->color.r = 0.5;
	c->color.g = 0.5;
	c->color.b = 1.0;
	c->color.a = 0.8;
	//model->setCane(stretch);
	emit setCaneSig(stretch);
	saveCaneToLibrary();

	//model->setCane(NULL);
	emit setCaneSig(NULL);
	displayTextMessage("Default library loaded");
}

void MainWindow :: exportCaneDialog(){
	QString fileName =  QFileDialog::getSaveFileName();

	YAML::Emitter out;
	out << 1;
	out << YAML::BeginSeq;

	Cane* cane = this->openglWidget->getModel()->getCane();
	out << YAML::Literal << cane->yamlRepresentation();
	out << YAML::EndSeq;

	QFile file(fileName);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
		return;

	file.reset();

	QTextStream outStream(&file);
	outStream << out.c_str() << "\n";
	outStream.flush();
	file.close();

	displayTextMessage("Cane Saved to: " + fileName);
}

void MainWindow :: importCaneDialog(){
	QString fileName = QFileDialog::getOpenFileName();

	std::ifstream fin(fileName.toStdString().c_str());
	YAML::Parser parser(fin);

	YAML::Node doc;
	parser.GetNextDocument(doc);
	parser.GetNextDocument(doc);

	Cane loadCane = Cane(UNASSIGNED_CANETYPE);
	loadLibraryCane(doc[0],&loadCane);

	//model->setCane(&loadCane);
	emit setCaneSig(&loadCane);

	saveCaneToLibrary();

	//model->clearCurrentCane();
	emit setCaneSig(NULL);
	displayTextMessage("Cane loaded from: " + fileName);
}

void MainWindow::exportLibraryDialog()
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

	displayTextMessage("Library Saved to: " + fileName);
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

	const YAML::Node& caneAmts = newNode["Defined Amounts"];

	int amtsCount=0;

	for(YAML::Iterator it2=caneAmts.begin();it2!=caneAmts.end();++it2)
	{
		std::string temp;
		*it2 >> temp;

		++it2;

		*it2 >> cane->amts[amtsCount];
		amtsCount++;
	}

	newNode["Number of Subcanes"] >> cane->subcaneCount;

	const YAML::Node& subLocations = newNode["Subcane Locations"];
	int subLocationCount=0;
	for(YAML::Iterator it3=subLocations.begin();it3!=subLocations.end();++it3) {
		const YAML::Node& subCaneLocation = *it3;
		subCaneLocation[0] >> cane->subcaneLocations[subLocationCount].x;
		subCaneLocation[1] >> cane->subcaneLocations[subLocationCount].y;
		subCaneLocation[2] >> cane->subcaneLocations[subLocationCount].z;
		subLocationCount++;
	}

	newNode["RGBA Color"][0] >> cane->color.r;
	newNode["RGBA Color"][1] >> cane->color.g;
	newNode["RGBA Color"][2] >> cane->color.b;
	newNode["RGBA Color"][3] >> cane->color.a;

	const YAML::Node& subCanes = newNode["Subcanes"];
	int subCaneCount = 0;
	for(YAML::Iterator it4=subCanes.begin();it4!=subCanes.end();++it4) {
		const YAML::Node& subCane = *it4;
		Cane* loadCane = new Cane(UNASSIGNED_CANETYPE);
		loadLibraryCane(subCane,loadCane);
		cane->subcanes[subCaneCount]=loadCane;
		subCaneCount++;
	}
}

void MainWindow::importLibraryDialog()
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

		//model->setCane(&loadCane);
		emit setCaneSig(&loadCane);

		saveCaneToLibrary();
	}

	//model->clearCurrentCane();
	emit setCaneSig(NULL);
	displayTextMessage("Library loaded from: " + fileName);
}

void MainWindow::setupNewColorPickerCaneDialog()
{
	caneDialog = new QDialog(NULL);
	caneForm = new QFormLayout(caneDialog->window());

	colorDialog = new QColorDialog(Qt::white, caneForm->widget());
	colorDialog->setOptions(QColorDialog::ShowAlphaChannel | QColorDialog::NoButtons);
	caneForm->addRow(colorDialog);

	caneTypeBox = new QComboBox(caneForm->widget());
	caneTypeBox->addItem("Circle Base", QVariant(BASE_CIRCLE_CANETYPE));
	caneTypeBox->addItem("Square Base", QVariant(FLATTEN_CANETYPE));

	caneForm->addRow("Base Type",caneTypeBox);

	QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	connect(buttons,SIGNAL(accepted()),caneDialog,SLOT(accept()));
	connect(buttons,SIGNAL(accepted()),this,SLOT(colorPickerSelected()));
	connect(buttons,SIGNAL(rejected()),caneDialog,SLOT(reject()));

	caneForm->addRow(buttons);

	caneDialog->setLayout(caneForm);
}

void MainWindow::setupNewBrandCaneDialog()
{
	brandDialog = new QDialog(NULL);

	caneForm = new QFormLayout(brandDialog->window());

	loadOfficialCanes();

	caneSplitter = new QSplitter(caneForm->widget());

	caneTypeListModel = new QStringListModel();
	caneTypeListModel->setStringList(*caneTypeList);
	caneTypeListBox = new QListView();
	caneTypeListBox->setModel(caneTypeListModel);
	caneTypeListBox->setEditTriggers(QAbstractItemView::NoEditTriggers);

	dummyList = new QStringList("Please select a cane type.");
	dummyModel = new QStringListModel();
	dummyModel->setStringList(*dummyList);
	caneColorListBox = new QListView();//new QTreeView();
	caneColorListBox->setModel(dummyModel);
	dummyInUse = true;
	selectedBrand = -1;
	selectedColor = -1;
	caneColorListBox->setEditTriggers(QAbstractItemView::NoEditTriggers);

	caneSplitter->addWidget(caneTypeListBox);
	caneSplitter->addWidget(caneColorListBox);

	caneForm->addRow(caneSplitter);

	connect(caneTypeListBox, SIGNAL(clicked(QModelIndex)),this,SLOT(updateSublist(QModelIndex)));
	connect(caneColorListBox, SIGNAL(clicked(QModelIndex)),this,SLOT(updateColor(QModelIndex)));

	caneTypeBox = new QComboBox(caneForm->widget());
	caneTypeBox->addItem("Circle Base",QVariant(BASE_CIRCLE_CANETYPE));
	caneTypeBox->addItem("Square Base",QVariant(FLATTEN_CANETYPE));

	caneForm->addRow("Base Type",caneTypeBox);

	QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	connect(buttons,SIGNAL(accepted()),brandDialog,SLOT(accept()));
	connect(buttons,SIGNAL(accepted()),this,SLOT(colorBrandPickerSelected()));
	connect(buttons,SIGNAL(rejected()),brandDialog,SLOT(reject()));

	caneForm->addRow(buttons);

	brandDialog->setLayout(caneForm);
}

void MainWindow::updateColor(QModelIndex i)
{
	selectedColor = i.row();
}

void MainWindow::updateSublist(QModelIndex i)
{
	int index = i.row();
	if (index < 0 || index >= caneColorListList->size())
	{
		if (!dummyInUse)
		{
			QItemSelectionModel *m = caneColorListBox->selectionModel();
			caneColorListBox->setModel(dummyModel);
			delete m;
			dummyInUse = true;
			selectedBrand = -1;
			selectedColor = -1;
		}
	}
	else
	{
		if (!dummyInUse)
		{
			QItemSelectionModel *m = caneColorListBox->selectionModel();
			caneColorListBox->setModel(new QStringListModel(caneNameListList->at(index)));
			/*                for (int i = 0; i < caneNameListList[index].size(); i++)
 {
   caneColorListBox->drawRow(&makePainter(index,i), *(new QStyleOptionViewItem()), i);
 }*/
			delete m;
			selectedBrand = index;
			selectedColor = -1;
		}
		else
		{
			caneColorListBox->setModel(new QStringListModel(caneNameListList->at(index)));
			/*                for (int i = 0; i < caneNameListList[index].size(); i++)
{
caneColorListBox->drawRow(&makePainter(index,i), *(new QStyleOptionViewItem()), i);
}*/
		}
		dummyInUse = false;
		selectedBrand = index;
		selectedColor = -1;
	}
}

QPainter* MainWindow::makePainter(int caneType, int caneIndex)
{
	QPainter* painter = new QPainter();
	if (caneColorListList->size() <= caneType || caneColorListList->at(caneType).size() <= caneIndex)
	{
		painter->setPen(Qt::black);
		return painter;
	}
	painter->setBackgroundMode(Qt::OpaqueMode);
	painter->setBackground(*(new QBrush(caneColorListList->at(caneType).at(caneIndex))));
	painter->setPen(Qt::black);
	if (caneColorListList->at(caneType).at(caneIndex).red() +
			caneColorListList->at(caneType).at(caneIndex).green() +
			caneColorListList->at(caneType).at(caneIndex).blue() < 381)
		painter->setPen(Qt::white);
	return painter;
}

void MainWindow::loadOfficialCanes()
{
	caneTypeList = new QStringList;
	caneNameListList = new QList<QStringList>;
	caneColorListList = new QList<QList<QColor> >;
	QStringList* caneNameList = new QStringList();
	QList<QColor>* caneColorList = new QList<QColor>();
	int currentCane = -1;
	bool onColor = false;
	QFile file("../src/Colors1.txt");
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		caneTypeList->append("File Misread");
		return;
	}
	while (!file.atEnd()) {
		QByteArray line = file.readLine();
		//                process_line(line);
		line = line.trimmed();
		if (line.isEmpty())
		{
			if (onColor)
			{
				caneTypeList->append("Empty on color");
				return; //throw error? Not sure on this one
			}
		}
		else if (line.at(0) == '[')
		{
			if (onColor)
			{
				caneTypeList->append("Color mismatch A");
				return; //throw error
			}
			if (currentCane == -1)
			{
				caneTypeList->append("Cane mismatch A");
				return; //throw error
			}
			line.remove(0,1);
			if (line.lastIndexOf(']') == -1)
			{
				caneTypeList->append("Bracket mismatch");
				return; //throw error
			}
			line.remove(line.lastIndexOf(']'),1);
			line = line.trimmed();
			caneNameList->append(line);
			onColor = true;
		}
		else if (line.at(0) == '-')
		{
			if (!onColor)
			{
				caneTypeList->append("Color mismatch B");
				return; //throw error
			}
			if (currentCane == -1)
			{
				caneTypeList->append("Cane mismatch B");
				return; //throw error
			}
			line.remove(0,1);
			line = line.trimmed();
			QList<QByteArray> color = line.split(',');
			if (color.length() > 3)
			{
				caneColorList->append(*(new QColor(color[0].toInt(),color[1].toInt(),
												   color[2].toInt(),color[3].toInt())));
			}
			else
			{
				caneTypeList->append("Not enough color fields");
				return; //throw error
			}
			onColor = false;
		}
		else
		{
			if (onColor)
			{
				caneTypeList->append("Colot mismatch C");
				return; //throw error
			}
			currentCane++;
			caneTypeList->append(line);
			if (currentCane > 0)
			{
				caneNameListList->append(*caneNameList);
				caneColorListList->append(*caneColorList);
			}
			caneNameList->clear();
			caneColorList->clear();
		}
	}
	caneNameListList->append(*caneNameList);
	caneColorListList->append(*caneColorList);
	delete caneNameList;
	delete caneColorList;
	if (onColor)
	{
		caneTypeList->append("Color mismatch D");
		return; //throw error
	}
}

void MainWindow::newBrandCaneDialog()
{
	if (!dummyInUse)
	{
		QItemSelectionModel *m = caneColorListBox->selectionModel();
		caneColorListBox->setModel(dummyModel);
		delete m;
		dummyInUse = true;
		selectedBrand = -1;
		selectedColor = -1;
	}
	brandDialog->exec();
}

void MainWindow::newColorPickerCaneDialog()
{
	caneDialog->exec();
}

void MainWindow::changeBgColorDialog()
{
	openglWidget->setBgColor(QColorDialog::getColor());
}

void MainWindow::saveObjFileDialog()
{
	QString file = QFileDialog::getSaveFileName(this, tr("Save obj file"), "", tr("Wavefront obj files (*.obj);;All files (*)"));
	if (!file.isNull())
	{
		openglWidget->saveObjFile(qPrintable(file));
	}
}

void MainWindow::saveRawFile()
{
	openglWidget->saveRawFile("cane.raw");
}

void MainWindow::colorBrandPickerSelected()
{
	if (selectedBrand == -1 || selectedColor == -1 || selectedBrand >= caneColorListList->size() ||
			selectedColor >= caneColorListList->at(selectedBrand).size())
		return;
	QColor color = caneColorListList->at(selectedBrand).at(selectedColor);
	QVariant data = caneTypeBox->itemData(caneTypeBox->currentIndex());
	bool isOk = false;
	int caneType = data.toInt(&isOk);
	if (!isOk)
		return;

	saveCaneToLibrary();
	//model->clearCurrentCane();
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
	//model->setCane(stch);
	//saveCaneToLibrary();

	if (caneType == FLATTEN_CANETYPE)
	{
		stch->flatten(0.0,0.0,1.0);
	}

	emit setCaneSig(stch);
}

void MainWindow::colorPickerSelected()
{
	QColor color = colorDialog->currentColor();
	QVariant data = caneTypeBox->itemData(caneTypeBox->currentIndex());
	bool isOk = false;
	int caneType = data.toInt(&isOk);
	if (!isOk)
		return;

	saveCaneToLibrary();
	//model->clearCurrentCane();
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
	//model->setCane(stch);
	//saveCaneToLibrary();

	if (caneType == FLATTEN_CANETYPE)
	{
		stch->flatten(0.0,0.0,1.0);
	}

	emit setCaneSig(stch);
}

void MainWindow::setupWorkArea()
{

	pull_button = new QPushButton("Pull");
	pull_button->setToolTip("Drag Mouse Horizontally to Twist, Vertically to Stretch. Use Shift to twist and stretch independently.");
	bundle_button = new QPushButton("Bundle");
	case_button = new QPushButton("Case");
	case_button->setToolTip("Create a clear casing around the glass.");
	flatten_button = new QPushButton("Flatten");
	flatten_button->setToolTip("Drag Mouse Horizontally to Squish, Vertically to Flatten");
	snap_button = new QPushButton("Alternate Snap Modes");
	snap_button->setToolTip("TODO");
	undo_button = new QPushButton("Undo");
	undo_button->setToolTip("Undo the last operation.");
	redo_button = new QPushButton("Redo");
	redo_button->setToolTip("Redo the last operation.");
	save_button = new QPushButton("Save");
	save_button->setToolTip("Save Current Model to Library");
	clear_button = new QPushButton("Clear");
	clear_button->setToolTip("Clear Current Model");

	previewLabel = new QLabel();
	previewLabel->setFixedSize(100,100);
	previewLabel->setScaledContents(true);

	QVBoxLayout* operButton_layout = new QVBoxLayout();
	operButton_layout->addWidget(pull_button);
	operButton_layout->addWidget(bundle_button);
	operButton_layout->addWidget(case_button);
	operButton_layout->addWidget(flatten_button);
	operButton_layout->addWidget(snap_button);
	operButton_layout->addWidget(undo_button);
	operButton_layout->addWidget(redo_button);
	operButton_layout->addWidget(save_button);
	operButton_layout->addWidget(clear_button);
	operButton_layout->addWidget(previewLabel,1, Qt::AlignHCenter);
	previewLabel->setHidden(true);

	QHBoxLayout* workLayout = new QHBoxLayout(windowLayout->widget());
	workLayout->addLayout(operButton_layout);

	stackLayout = new QStackedLayout(workLayout->widget());

	openglWidget = new OpenGLWidget(stackLayout->widget(), model);
	setupRecipeArea();

	stackLayout->addWidget(openglWidget);
	stackLayout->addWidget(recipeWidget);
	stackLayout->setCurrentWidget(openglWidget);

	workLayout->addLayout(stackLayout,1);
	//workLayout->addWidget(recipeWidget,1);
	windowLayout->addLayout(workLayout, 5);
}

void MainWindow::keyPressEvent(QKeyEvent* e)
{
	switch (e->key())
	{
	case 0x01000020: // Shift
		openglWidget->setShiftButtonDown(true);
		break;
	case 0x01000021: // Control
		openglWidget->setControlButtonDown(true);
		break;
	case 0x01000007: // Delete
		openglWidget->setDeleteButtonDown(true);
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
	case 0x01000021: // Control
		openglWidget->setControlButtonDown(false);
		break;
	case 0x01000007: // Delete
		openglWidget->setDeleteButtonDown(false);
		break;
	default:
		break;
	}
}

void MainWindow::setupRecipeArea()
{
	recipeWidget = new RecipeWidget(stackLayout->widget(),openglWidget);
}

void MainWindow::toggleRecipe()
{
	isRecipe = !isRecipe;
	if (isRecipe)
	{
		openglWidget->setClickable(false);
		recipeWidget->updateRecipe(true);
		stackLayout->setCurrentWidget(recipeWidget);
		previewLabel->setHidden(false);
	} else
	{
		openglWidget->setClickable(true);
		stackLayout->setCurrentWidget(openglWidget);
		previewLabel->setHidden(true);
	}
}

void MainWindow::updatePreview()
{
	previewLabel->setPixmap(QPixmap::grabWidget(openglWidget));
}
