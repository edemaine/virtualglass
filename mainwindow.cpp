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
	setupCaneChangeDialog();
	updateModeButtonsEnabled();

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

	QAction* importCane = new QAction(tr("&Import Single Cane"), this);
	importCane->setStatusTip(tr("Load a saved cane"));
	connect(importCane, SIGNAL(triggered()), this, SLOT(importCaneDialog()));
	fileMenu->addAction(importCane);

	QAction* exportCane = new QAction(tr("&Export Current Cane"), this);
	exportCane->setStatusTip(tr("Save the current cane to a file"));
	connect(exportCane, SIGNAL(triggered()), this, SLOT(exportCaneDialog()));
	fileMenu->addAction(exportCane);

	fileMenu->addSeparator();

	QAction* exportObj = new QAction(tr("&Export to .obj"), this);
	exportObj->setStatusTip(tr("Save the geometry of the current cane as a .obj file"));
	connect(exportObj, SIGNAL(triggered()), this, SLOT(saveObjFileDialog()));
	fileMenu->addAction(exportObj);

	QAction* exportRaw = new QAction(tr("&Export to ray tracer"), this);
	exportRaw->setStatusTip(tr("Save the geometry of the current cane as a .raw file for the ray tracer"));
	connect(exportRaw, SIGNAL(triggered()), this, SLOT(saveRawFile()));
	fileMenu->addAction(exportRaw);


	viewMenu = menuBar()->addMenu(tr("&View"));

	QAction* toggleRecipe = new QAction(tr("&Recipe View"), this);
	toggleRecipe->setStatusTip(tr("Switch between 3D view and recipe view"));
	toggleRecipe->setCheckable(true);
	toggleRecipe->setChecked(false);
	connect(toggleRecipe, SIGNAL(triggered()), this, SLOT(toggleRecipe()));
	isRecipe = false;
	viewMenu->addAction(toggleRecipe);

	viewMenu->addSeparator();

	QAction* togglePeel = new QAction(tr("&Depth Peelin'"), this);
	togglePeel->setStatusTip(tr("Show better transparency."));
	togglePeel->setCheckable(true);
	togglePeel->setChecked(true);
	connect(togglePeel, SIGNAL(triggered()), openglWidget, SLOT(togglePeel()));
	viewMenu->addAction(togglePeel);


	QAction* toggleAxes = new QAction(tr("&Show Axes"), this);
	toggleAxes->setStatusTip(tr("Show the reference lines on the X, Y, and Z axes."));
	toggleAxes->setCheckable(true);
	toggleAxes->setChecked(true);
	openglWidget->setAxes(true);
	connect(toggleAxes, SIGNAL(triggered()), openglWidget, SLOT(toggleAxes()));
	viewMenu->addAction(toggleAxes);

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

	QMenu* projectionMenu = viewMenu->addMenu(tr("&Projection"));

	orthographicProjection = new QAction(tr("&Orthographic Projection"), this);
	orthographicProjection->setCheckable(true);
	orthographicProjection->setStatusTip(tr("Set the camera projection to orthographic (parallelism preserving)."));
	connect(orthographicProjection, SIGNAL(triggered()), model, SLOT(setOrthographicProjection()));
	projectionMenu->addAction(orthographicProjection);

	perspectiveProjection = new QAction(tr("&Perspective Projection"), this);
	perspectiveProjection->setCheckable(true);
	perspectiveProjection->setStatusTip(tr("Set the camera projection to perspective (realistic)."));
	connect(perspectiveProjection, SIGNAL(triggered()), model, SLOT(setPerspectiveProjection()));
	projectionMenu->addAction(perspectiveProjection);

	projectionChanged(); // Initialize projection menu

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


	caneMenu = menuBar()->addMenu(tr("&New Cane"));

	QAction* addCane = new QAction(tr("&Add Cane"), this);
	addCane->setStatusTip(tr("Add a new cane to the piece."));
	connect(addCane, SIGNAL(triggered()), this, SLOT(addNewDefaultCane()));
	caneMenu->addAction(addCane);
}

void MainWindow :: addNewDefaultCane()
{
	caneChangeRequest(model->addNewDefaultCane());	
}

void MainWindow :: shapeTypeEvent(int)
{
	shapePickerEvent();
} 

void MainWindow :: shapeSizeEvent(int)
{
	shapePickerEvent();
}

void MainWindow :: shapePickerEvent()
{
	QString shape = caneShapeBox->currentText();
	float size = caneSizeSlider->sliderPosition() / 100.0;	

	if (shape == "Circle")
		model->setSubcaneShape(caneChangeSubcane, CIRCLE, size);
	else if (shape == "Square")
		model->setSubcaneShape(caneChangeSubcane, SQUARE, size);
	else if (shape == "Rectangle")
		model->setSubcaneShape(caneChangeSubcane, RECTANGLE, size);
	else if (shape == "Triangle")
		model->setSubcaneShape(caneChangeSubcane, TRIANGLE, size);
	else if (shape == "Half Circle")
		model->setSubcaneShape(caneChangeSubcane, HALF_CIRCLE, size);
	else if (shape == "Third Circle")
		model->setSubcaneShape(caneChangeSubcane, THIRD_CIRCLE, size);
}

void MainWindow::caneChangeRequest(int subcane)
{
	caneChangeSubcane = subcane;
	if (model->subcaneHasColorAndShape(subcane))
	{
		changeDialog->show();
	}
}

void MainWindow::projectionChanged()
{
	if (model->getProjection() == ORTHOGRAPHIC_PROJECTION)
	{
		this->orthographicProjection->setChecked(true);
		this->perspectiveProjection->setChecked(false);
	}
	else if (model->getProjection() == PERSPECTIVE_PROJECTION)
	{
		this->orthographicProjection->setChecked(false);
		this->perspectiveProjection->setChecked(true);
	}
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
	case FLATTEN_MODE:
		modeLabel->setText("FLATTEN MODE");
		displayTextMessage("Click and drag mouse up and down to square off cane, left and right to flatten cane",0);
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
	stockLayout->addWidget(lc);
	updateLibraryToolTip(lc);
	connect(lc, SIGNAL(mouseOver(LibraryCaneWidget*)), this, SLOT(updateLibraryToolTip(LibraryCaneWidget*)));
	connect(stockLayout, SIGNAL(destroyed(QObject*)), this, SLOT(libraryCaneDestroyed(QObject*)));
	connect(lc,SIGNAL(addCane(Cane*)),this,SLOT(insertLibraryCane(Cane*)));
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
	modeLabel = new QLabel("", statusBar);
	statusBar->addPermanentWidget(modeLabel);
	setStatusBar(statusBar);
}

Point polygonalBasePoint(int total,int index)
{
	Point p;
	p.x = cos(2*PI*index/total);
	p.y = sin(2*PI*index/total);
	return p;
}

void MainWindow::setupLibraryArea()
{
	QWidget* stockWidget = new QWidget;
	stockLayout = new QBoxLayout(QBoxLayout::RightToLeft, stockWidget);
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
	Cane* base = new Cane(BASE_POLYGONAL_CANETYPE);

	//---------- just for alpha stuff ---------
	base->color.r = 1.0;
	base->color.g = 0.5;
	base->color.b = 0.5;
	base->color.a = 0.2;
	base->setShape(CIRCLE, LOW_ANGULAR_RESOLUTION, 0.2);
	model->setCane(base);
	saveCaneToLibrary();

	base->color.r = 1.0;
	base->color.g = 1.0;
	base->color.b = 1.0;
	base->color.a = 0.2;
	base->setShape(CIRCLE, LOW_ANGULAR_RESOLUTION, 0.2);
	model->setCane(base);
	saveCaneToLibrary();

	base->color.r = 0.5;
	base->color.g = 0.7;
	base->color.b = 1.0;
	base->color.a = 0.2;
	base->setShape(CIRCLE, LOW_ANGULAR_RESOLUTION, 0.2);
	model->setCane(base);
	saveCaneToLibrary();


	//--------------------------------------------


	base->color.r = 1.0;
	base->color.g = 0.5;
	base->color.b = 0.5;
	base->color.a = 0.8;
	base->setShape(CIRCLE, LOW_ANGULAR_RESOLUTION, 0.2);
	model->setCane(base);
	saveCaneToLibrary();
	base->setShape(TRIANGLE, LOW_ANGULAR_RESOLUTION, 0.2);
	model->setCane(base);
	saveCaneToLibrary();

	base->color.r = 0.5;
	base->color.g = 1.0;
	base->color.b = 0.5;
	base->color.a = 0.8;
	base->setShape(SQUARE, LOW_ANGULAR_RESOLUTION, 0.2);
	model->setCane(base);
	saveCaneToLibrary();
	base->setShape(RECTANGLE, LOW_ANGULAR_RESOLUTION, 0.2);
	model->setCane(base);
	saveCaneToLibrary();

	base->color.r = 0.5;
	base->color.g = 0.5;
	base->color.b = 1.0;
	base->color.a = 0.8;
	base->setShape(HALF_CIRCLE, LOW_ANGULAR_RESOLUTION, 0.2);
	model->setCane(base);
	saveCaneToLibrary();
	base->setShape(THIRD_CIRCLE, LOW_ANGULAR_RESOLUTION, 0.2);
	model->setCane(base);
	saveCaneToLibrary();

	model->setCane(NULL);
	displayTextMessage("Default library loaded");
}

void MainWindow :: exportCaneDialog(){


	QString fileName =  QFileDialog::getSaveFileName(this, tr("Export Single Cane"), "mycane.glass", tr("Glass (*.glass)"));
	if (!fileName.endsWith(".glass") || fileName.length() < 6)
		return;

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
	QString fileName = QFileDialog::getOpenFileName(this, tr("Import Single Cane"), "", tr("Glass (*.glass)"));

	std::ifstream fin(fileName.toStdString().c_str());
	YAML::Parser parser(fin);

	YAML::Node doc;
	parser.GetNextDocument(doc);
	parser.GetNextDocument(doc);

	Cane loadCane = Cane(UNASSIGNED_CANETYPE);
	loadLibraryCane(doc[0],&loadCane);

	model->setCane(&loadCane);

	saveCaneToLibrary();

	model->setCane(NULL);
	displayTextMessage("Cane loaded from: " + fileName);
}

void MainWindow::exportLibraryDialog()
{
	QString fileName =  QFileDialog::getSaveFileName(this, tr("Export Cane Library"), "mycanes.glass", tr("Glass (*.glass)"));
	if (!fileName.endsWith(".glass") || fileName.length() < 6)
		return;

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

	const YAML::Node& caneVertices = newNode["Vertices"];
	int verticesCount=0;

	for(YAML::Iterator it2=caneVertices.begin();it2!=caneVertices.end();++it2)
	{
		*it2 >> cane->vertices[verticesCount].x;

		++it2;
		*it2 >> cane->vertices[verticesCount].y;
		verticesCount++;
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
	QString fileName = QFileDialog::getOpenFileName(this, tr("Import Cane Library"), "", tr("Glass (*.glass)"));

	std::ifstream fin(fileName.toStdString().c_str());
	YAML::Parser parser(fin);

	YAML::Node doc;
	parser.GetNextDocument(doc);
	parser.GetNextDocument(doc);

	for(unsigned i=0;i<doc.size();i++) {
		Cane loadCane = Cane(UNASSIGNED_CANETYPE);
		loadLibraryCane(doc[i],&loadCane);

		model->setCane(&loadCane);

		saveCaneToLibrary();
	}

	model->setCane(NULL);
	displayTextMessage("Library loaded from: " + fileName);
}

void MainWindow::setupCaneChangeDialog()
{
	changeDialog = new QDialog(NULL);
	QFormLayout* layout = new QFormLayout(changeDialog->window());
	layout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);

	loadOfficialCanes();

	QSplitter* caneSplitter = new QSplitter(layout->widget());

	QStringListModel* caneTypeListModel = new QStringListModel();
	caneTypeListModel->setStringList(*caneTypeList);
	caneTypeListBox = new KeyQListView();
	caneTypeListBox->setModel(caneTypeListModel);
	caneTypeListBox->setEditTriggers(QAbstractItemView::NoEditTriggers);

	QStringList* dummyList = new QStringList("Please select a cane type.");
	dummyModel = new QStringListModel();
	dummyModel->setStringList(*dummyList);
	caneColorListBox = new KeyQListView();//QListView();
	caneColorListBox->setModel(dummyModel);
	dummyInUse = true;
	selectedBrand = -1;
	selectedColor = -1;
	caneColorListBox->setEditTriggers(QAbstractItemView::NoEditTriggers);

	caneSplitter->addWidget(caneTypeListBox);
	caneSplitter->addWidget(caneColorListBox);

	layout->addRow(caneSplitter);

	connect(caneTypeListBox, SIGNAL(clicked(QModelIndex)), this,
		SLOT(updateBrandColorPickerSublist(QModelIndex)));
	connect(caneColorListBox, SIGNAL(clicked(QModelIndex)), this,
		SLOT(updateBrandColorPickerColor(QModelIndex)));

        caneShapeBox = new QComboBox(layout->widget());
        caneShapeBox->addItem("Circle");
        caneShapeBox->addItem("Square");
        caneShapeBox->addItem("Rectangle");
        caneShapeBox->addItem("Triangle");
        caneShapeBox->addItem("Half Circle");
        caneShapeBox->addItem("Third Circle");
        layout->addRow("Shape:", caneShapeBox);

	
        caneSizeSlider = new QSlider(Qt::Horizontal, layout->widget());
        caneSizeSlider->setRange(1, 100);
	QBoxLayout* sliderLayout = new QBoxLayout(QBoxLayout::LeftToRight, layout->widget());
	QLabel* lsLabel = new QLabel("0 in.", sliderLayout->widget());
	QLabel* rsLabel = new QLabel("6 in.", sliderLayout->widget());
	sliderLayout->insertWidget(0, lsLabel); 
	sliderLayout->insertWidget(1, caneSizeSlider);
	sliderLayout->insertWidget(2, rsLabel); 

       	layout->addRow("Diameter:", sliderLayout);

        connect(caneShapeBox, SIGNAL(currentIndexChanged(int)),
                this, SLOT(shapeTypeEvent(int)));
        connect(caneSizeSlider, SIGNAL(sliderMoved(int)),
                this, SLOT(shapeSizeEvent(int)));
}

void MainWindow::updateBrandColorPickerColor(QModelIndex i)
{
	selectedColor = i.row();
	if (selectedBrand == -1 || selectedColor == -1 || selectedBrand >= caneColorListList->size() ||
			selectedColor >= caneColorListList->at(selectedBrand).size())
		return;
	QColor color = caneColorListList->at(selectedBrand).at(selectedColor);
	model->setSubcaneColor(caneChangeSubcane, color.redF(), color.greenF(), color.blueF(), color.alphaF());
}

void MainWindow::updateBrandColorPickerSublist(QModelIndex i)
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
			caneColorListBox->setModel(new QStandardItemModel(caneNameListList->at(index).size(),1));
			for (int i = 0; i < caneNameListList->at(index).size(); i++)
			{
				caneColorListBox->model()->setData(caneColorListBox->model()->index(i,0),caneNameListList->at(index).at(i),Qt::DisplayRole);
				QColor c = caneColorListList->at(index).at(i);
				c.setAlpha(255);
				caneColorListBox->model()->setData(caneColorListBox->model()->index(i,0),c,Qt::DecorationRole);
			}

			delete m;
			selectedBrand = index;
			selectedColor = -1;
		}
		else
		{
			caneColorListBox->setModel(new QStandardItemModel(caneNameListList->at(index).size(),1));
			for (int i = 0; i < caneNameListList->at(index).size(); i++)
			{
				caneColorListBox->model()->setData(caneColorListBox->model()->index(i,0),caneNameListList->at(index).at(i),Qt::DisplayRole);
				QColor c = caneColorListList->at(index).at(i);
				c.setAlpha(255);
				caneColorListBox->model()->setData(caneColorListBox->model()->index(i,0),c,Qt::DecorationRole);
			}
		}
		dummyInUse = false;
		selectedBrand = index;
		selectedColor = -1;
	}
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
				caneTypeList->append("Color mismatch C");
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
	changeDialog->exec();
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

void MainWindow::setupButtonBar()
{
	pull_button = new QPushButton("Pull");
	pull_button->setToolTip("Drag Mouse Horizontally to Twist, Vertically to Stretch. Use Shift to twist and stretch independently.");
	bundle_button = new QPushButton("Bundle");
	flatten_button = new QPushButton("Flatten");
	flatten_button->setToolTip("Drag Mouse Horizontally to Squish, Vertically to Flatten");
	toggle2D_button = new QPushButton("2D View");
	toggle2D_button->setToolTip(tr("Switch between 2D and 3D view."));
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

	operButton_layout = new QVBoxLayout();
	operButton_layout->addWidget(pull_button);
	operButton_layout->addWidget(bundle_button);
	operButton_layout->addWidget(flatten_button);
	operButton_layout->addWidget(toggle2D_button);
	operButton_layout->addWidget(redo_button);
	operButton_layout->addWidget(undo_button);
	operButton_layout->addWidget(save_button);
	operButton_layout->addWidget(clear_button);
	operButton_layout->addWidget(previewLabel,1, Qt::AlignHCenter);
	previewLabel->setHidden(true);
}

void MainWindow::setupOGLArea()
{
	oglLayoutWidget = new QWidget(stackLayout->widget());
	QHBoxLayout* oglLayout = new QHBoxLayout();
	oglLayoutWidget->setLayout(oglLayout);

	// Setup opengl 3D view
	openglWidget = new OpenGLWidget(oglLayoutWidget, model);
	oglLayout->addWidget(openglWidget);

	// Setup slider
	oglGeometryHeightSlider = new QSlider(Qt::Vertical, oglLayoutWidget);
	oglGeometryHeightSlider->setRange(1, 100);
	oglGeometryHeightSlider->setSliderPosition(25);
        connect(oglGeometryHeightSlider, SIGNAL(sliderMoved(int)),
                this, SLOT(geometryHeightEvent(int)));
	oglLayout->addWidget(oglGeometryHeightSlider);
}

void MainWindow::geometryHeightEvent(int)
{
	float newHeight = oglGeometryHeightSlider->sliderPosition() / 100.0 * 4.0;

	model->setGeometryHeight(newHeight);
}

void MainWindow::setupWorkArea()
{
	QHBoxLayout* workLayout = new QHBoxLayout(windowLayout->widget());
	setupButtonBar();
	workLayout->addLayout(operButton_layout);

	stackLayout = new QStackedLayout(workLayout->widget());

	setupOGLArea();
	setupRecipeArea();

	stackLayout->addWidget(oglLayoutWidget);
	stackLayout->addWidget(recipeWidget);
	stackLayout->setCurrentWidget(oglLayoutWidget);

	workLayout->addLayout(stackLayout,1);
	windowLayout->addLayout(workLayout, 5);
}

void MainWindow::keyPressEvent(QKeyEvent* e)
{
	switch (e->key())
	{
	case 0x01000020: // Shift
		openglWidget->setShiftButtonDown(true);
		break;
	case 0x01000003: // Backspace
	case 0x01000007: // Delete
		openglWidget->setDeleteButtonDown(true);
		break;
	case 0x2b: // +
	case 0x3d:
		openglWidget->zoomIn();
		break;
	case 0x2d: // -
	case 0x5f:
		openglWidget->zoomOut();
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
	case 0x01000003: // Backspace
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
		openglWidget->setVisible(false);
		stackLayout->setCurrentWidget(recipeWidget);
		recipeWidget->updateRecipe(true);
		previewLabel->setHidden(false);
	} else
	{
		openglWidget->setVisible(true);
		stackLayout->setCurrentWidget(openglWidget);
		previewLabel->setHidden(true);
	}
}

void MainWindow :: newMode(int i)
{
	if (recipeWidget->selectedItems().isEmpty())
		emit setNewMode(i,isRecipe, model->getCane());
	else
	{
		emit setNewMode(i,isRecipe, recipeWidget->getCane(recipeWidget->selectedItems().at(0)));
	}
}

void MainWindow :: insertLibraryCane(Cane* c)
{
	if (recipeWidget->selectedItems().isEmpty() || !recipeWidget->isVisible())
		model->addCane(c);
	else
	{
		model->addCane(recipeWidget->getCane(recipeWidget->selectedItems().at(0)),c);
	}
}

void MainWindow::caneChanged()
{
	updatePreview();
	updateModeButtonsEnabled();
}

void MainWindow :: updateModeButtonsEnabled()
{
	if (model->getCane() == NULL)
	{
		pull_button->setEnabled(false);
		flatten_button->setEnabled(false);
		bundle_button->setEnabled(false);
	}
	else
	{
		pull_button->setEnabled(true);
		flatten_button->setEnabled(true);
		bundle_button->setEnabled(true);
	}
}

void MainWindow::updatePreview()
{
	previewLabel->setPixmap(QPixmap::fromImage(openglWidget->grabFrameBuffer()));
}
