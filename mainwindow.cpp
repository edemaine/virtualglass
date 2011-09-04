#include "mainwindow.h"
#include "SVG.hpp"
#include "PlanarMap.hpp"

#include <set>
#include <map>
#include <deque>
#include <limits>

using std::set;
using std::map;
using std::deque;
using std::numeric_limits;

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
	setupChangeDialog();
	setupArrangementDialog();
	updateModeButtonsEnabled();

	setWindowTitle(tr("Virtual Glass"));
	resize(1000, 750);
	move(75,25);
}

void MainWindow::setupMenuBar()
{
	fileMenu = menuBar()->addMenu(tr("&File"));

	QAction* importLibrary = new QAction(tr("&Load canes"), this);
	importLibrary->setStatusTip(tr("Load a library of saved canes"));
	connect(importLibrary, SIGNAL(triggered()), this, SLOT(importLibraryDialog()));
	QList<QKeySequence> importShortcuts;
	importShortcuts << QKeySequence("CTRL+O") << QKeySequence("SHIFT+CTRL+O");
	importLibrary->setShortcuts(importShortcuts);
	fileMenu->addAction(importLibrary);

	QAction* exportLibrary = new QAction(tr("&Save all canes"), this);
	exportLibrary->setStatusTip(tr("Save the current library of canes"));
	connect(exportLibrary, SIGNAL(triggered()), this, SLOT(exportLibraryDialog()));
	exportLibrary->setShortcut(QKeySequence("SHIFT+CTRL+S"));
	fileMenu->addAction(exportLibrary);

	QAction* exportCane = new QAction(tr("&Save current cane"), this);
	exportCane->setStatusTip(tr("Save the current cane to a file"));
	connect(exportCane, SIGNAL(triggered()), this, SLOT(exportCaneDialog()));
	exportCane->setShortcut(QKeySequence("CTRL+S"));
	fileMenu->addAction(exportCane);

	fileMenu->addSeparator();

	QAction* loadSVG = new QAction(tr("Cane from SVG"), this);
	loadSVG->setStatusTip(tr("Create a cane based on an SVG file"));
	connect(loadSVG, SIGNAL(triggered()), this, SLOT(loadSVGFileDialog()));
	fileMenu->addAction(loadSVG);

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
	togglePeel->setCheckable(openglWidget->peelEnabled());
	togglePeel->setChecked(true);
	connect(togglePeel, SIGNAL(triggered()), openglWidget, SLOT(togglePeel()));
	connect(openglWidget, SIGNAL(updatePeelButton(bool)), togglePeel, SLOT(setChecked(bool)));
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
	QList<QKeySequence> zoomInShortcuts;
	zoomInShortcuts << QKeySequence("CTRL+=") << QKeySequence("CTRL++");
	zoomIn->setShortcuts(zoomInShortcuts);
	viewMenu->addAction(zoomIn);

	QAction* zoomOut = new QAction(tr("&Zoom Out"), this);
	zoomOut->setStatusTip(tr("Zoom in the camera."));
	connect(zoomOut, SIGNAL(triggered()), openglWidget, SLOT(zoomOut()));
	QList<QKeySequence> zoomOutShortcuts;
	zoomOutShortcuts << QKeySequence("CTRL+-") << QKeySequence("CTRL+_");
	zoomOut->setShortcuts(zoomOutShortcuts);
	viewMenu->addAction(zoomOut);


	caneMenu = menuBar()->addMenu(tr("&New Cane"));

	QAction* addCane = new QAction(tr("&Add Cane"), this);
	addCane->setStatusTip(tr("Add a new cane to the piece."));
	connect(addCane, SIGNAL(triggered()), this, SLOT(addNewDefaultCane()));
	addCane->setShortcut(QKeySequence("CTRL+N"));
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
	QString shapeText = caneShapeBox->currentText();
        float diameter = caneSizeEditBox->text().toFloat()/10.0;//caneSizeSlider->sliderPosition() / 60.0;
	CaneShape shape;
	int resolution = LOW_ANGULAR_RESOLUTION;

	if (shapeText == "Circle")
		shape.setByTypeAndDiameter(CIRCLE_SHAPE, diameter, resolution);
	else if (shapeText == "Half Circle")
		shape.setByTypeAndDiameter(HALF_CIRCLE_SHAPE, diameter, resolution);
	else if (shapeText == "Third Circle")
		shape.setByTypeAndDiameter(THIRD_CIRCLE_SHAPE, diameter, resolution);
	else if (shapeText == "Square")
		shape.setByTypeAndDiameter(SQUARE_SHAPE, diameter, resolution);
	else if (shapeText == "Rectangle")
		shape.setByTypeAndDiameter(RECTANGLE_SHAPE, diameter, resolution);
	else if (shapeText == "Triangle")
		shape.setByTypeAndDiameter(TRIANGLE_SHAPE, diameter, resolution);

	model->setSubcaneShape(caneChangeSubcane, &shape);
}

void MainWindow::caneChangeRequest(int subcane)
{
	caneChangeSubcane = subcane;
	saveCaneColorAndShape();
	if (model->subcaneHasColorAndShape(subcane))
	{
		CaneShape* subcaneShape = model->getSubcaneShape(subcane);
		Color* subcaneColor = model->getSubcaneColor(subcane);
		if (subcaneShape->getType() != UNDEFINED_SHAPE)
		{
			caneShapeBox->setCurrentIndex(subcaneShape->getType()-1);
			caneSizeSlider->setSliderPosition(((int) (subcaneShape->getDiameter() * 60)));
		}
		caneAlphaSlider->setSliderPosition(((int) (subcaneColor->a * 255)));
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
	case NO_MODE:
		modeLabel->setText("NO MODE");
		break;
	default:
		modeLabel->setText("UNKNOWN MODE");
		displayTextMessage("Unknown mode not specified",0);
		break;
	}
	checkButton(mode);
}

void MainWindow::checkButton(int mode)
{
	pull_button->setDown(false);
	bundle_button->setDown(false);
	flatten_button->setDown(false);
	switch(mode)
	{
	case PULL_MODE:
		pull_button->setDown(true);
		break;
	case BUNDLE_MODE:
		bundle_button->setDown(true);
		break;
	case FLATTEN_MODE:
		flatten_button->setDown(true);
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
	// still working on arrangement dialog feature
	//connect(lc, SIGNAL(arrangementRequest(Cane*)), this, SLOT(arrangementRequest(Cane*)));
	connect(stockLayout, SIGNAL(destroyed(QObject*)), this, SLOT(libraryCaneDestroyed(QObject*)));
	connect(lc, SIGNAL(requestDelete(Cane*)), openglWidget, SLOT(processLibraryDelete(Cane*)));
	connect(openglWidget, SIGNAL(acceptLibraryDelete(Cane*)), lc, SLOT(deleteRequestAccepted(Cane*)));
}

void MainWindow::arrangementRequest(Cane* c)
{
	model->addCane(c);
	arrangementDialog->show();
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
	base->shape.setByTypeAndDiameter(CIRCLE_SHAPE, 0.2, LOW_ANGULAR_RESOLUTION);
	model->setCane(base);
	saveCaneToLibrary();

	base->color.r = 1.0;
	base->color.g = 1.0;
	base->color.b = 1.0;
	base->color.a = 0.2;
	base->shape.setByTypeAndDiameter(CIRCLE_SHAPE, 0.2, LOW_ANGULAR_RESOLUTION);
	model->setCane(base);
	saveCaneToLibrary();

	base->color.r = 0.5;
	base->color.g = 0.7;
	base->color.b = 1.0;
	base->color.a = 0.2;
	base->shape.setByTypeAndDiameter(CIRCLE_SHAPE, 0.2, LOW_ANGULAR_RESOLUTION);
	model->setCane(base);
	saveCaneToLibrary();


	//--------------------------------------------


	base->color.r = 1.0;
	base->color.g = 0.5;
	base->color.b = 0.5;
	base->color.a = 0.8;
	base->shape.setByTypeAndDiameter(CIRCLE_SHAPE, 0.2, LOW_ANGULAR_RESOLUTION);
	model->setCane(base);
	saveCaneToLibrary();
	base->shape.setByTypeAndDiameter(TRIANGLE_SHAPE, 0.2, LOW_ANGULAR_RESOLUTION);
	model->setCane(base);
	saveCaneToLibrary();

	base->color.r = 0.5;
	base->color.g = 1.0;
	base->color.b = 0.5;
	base->color.a = 0.8;
	base->shape.setByTypeAndDiameter(SQUARE_SHAPE, 0.2, LOW_ANGULAR_RESOLUTION);
	model->setCane(base);
	saveCaneToLibrary();
	base->shape.setByTypeAndDiameter(RECTANGLE_SHAPE, 0.2, LOW_ANGULAR_RESOLUTION);
	model->setCane(base);
	saveCaneToLibrary();

	base->color.r = 0.5;
	base->color.g = 0.5;
	base->color.b = 1.0;
	base->color.a = 0.8;
	base->shape.setByTypeAndDiameter(HALF_CIRCLE_SHAPE, 0.2, LOW_ANGULAR_RESOLUTION);
	model->setCane(base);
	saveCaneToLibrary();
	base->shape.setByTypeAndDiameter(THIRD_CIRCLE_SHAPE, 0.2, LOW_ANGULAR_RESOLUTION);
	model->setCane(base);
	saveCaneToLibrary();

	model->setCane(NULL);
	displayTextMessage("Default library loaded");
}

void MainWindow :: exportCaneDialog(){

	QString fileName =  QFileDialog::getSaveFileName(this, tr("Export Single Cane"),
		"mycane.glass", tr("Glass (*.glass)"));

	if (fileName==NULL)
		return;
	if (fileName=="")
		return;
	Cane* cane = this->openglWidget->getModel()->getCane();
	if (cane == NULL)
		return;

	vector<Cane*> canes;
	canes.push_back(cane);
	saveCanesToFile(fileName, canes);

	displayTextMessage("Cane saved to: " + fileName);
}

void MainWindow::exportLibraryDialog()
{
	QString fileName =  QFileDialog::getSaveFileName(this, tr("Export Cane Library"), "mycanes.glass", tr("Glass (*.glass)"));

	if (fileName==NULL)
		return;
	if (fileName=="")
		return;
	vector<Cane*> canes;
	QList<LibraryCaneWidget*> libraryList = libraryScrollArea->findChildren<LibraryCaneWidget*>();
	for (int i = 0; i < libraryList.size(); ++i)
	{
		Cane* cane = libraryList.at(i)->getCane();
		canes.push_back(cane);
	}

	saveCanesToFile(fileName, canes);

	displayTextMessage("Library saved to: " + fileName);
}

void MainWindow::importLibraryDialog()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Import Cane Library"), "", tr("Glass (*.glass)"));

	if (fileName==NULL)
		return;
	if (fileName=="")
		return;
	vector<Cane*> canes = loadCanesFromFile(fileName);

	for(unsigned i = 0; i < canes.size(); ++i) {
		model->setCane(canes[i]);
		saveCaneToLibrary();
	}

	model->setCane(NULL);
	displayTextMessage("Library loaded from: " + fileName);
}

void MainWindow::setupArrangementDialog()
{
	arrangementDialog = new QDialog(NULL);
	QFormLayout* layout = new QFormLayout(arrangementDialog->window());
	layout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
	arrangementDialog->setLayout(layout);

	arrangementRadiusSlider = new QSlider(Qt::Horizontal, layout->widget());
	arrangementRadiusSlider->setRange(1, 60);
	QBoxLayout* radiusLayout = new QBoxLayout(QBoxLayout::LeftToRight, layout->widget());
	QLabel* lsLabel = new QLabel("0.1 in.", radiusLayout->widget());
	QLabel* rsLabel = new QLabel("6 in.", radiusLayout->widget());
	radiusLayout->insertWidget(0, lsLabel);
	radiusLayout->insertWidget(1, arrangementRadiusSlider);
	radiusLayout->insertWidget(2, rsLabel);

	layout->addRow("Arrangement radius", radiusLayout);

	arrangementCountSlider = new QSlider(Qt::Horizontal, layout->widget());
	arrangementCountSlider->setRange(2, 20);
	QBoxLayout* countLayout = new QBoxLayout(QBoxLayout::LeftToRight, layout->widget());
	lsLabel = new QLabel("2 ", countLayout->widget());
	rsLabel = new QLabel("20", countLayout->widget());
	countLayout->insertWidget(0, lsLabel);
	countLayout->insertWidget(1, arrangementCountSlider);
	countLayout->insertWidget(2, rsLabel);

	layout->addRow("Number of canes", countLayout);

}

void MainWindow::setupChangeDialog()
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

	// Alpha slider
	casLayoutWidget = new QWidget(caneSplitter);
	QVBoxLayout* alphaSliderLayout = new QVBoxLayout();
	casLayoutWidget->setLayout(alphaSliderLayout);
	caneAlphaSlider = new QSlider(Qt::Vertical, casLayoutWidget);
		caneAlphaSlider->setRange(0, 254);
		caneAlphaSlider->setValue(254);
	QLabel* alphaNameLabel = new QLabel("Opacity", casLayoutWidget);
	QLabel* alphaLsLabel = new QLabel("0%", casLayoutWidget);
	QLabel* alphaRsLabel = new QLabel("100%", casLayoutWidget);
	alphaSliderLayout->addWidget(alphaNameLabel);
	alphaSliderLayout->addWidget(alphaRsLabel);
	alphaSliderLayout->addWidget(caneAlphaSlider);
	alphaSliderLayout->addWidget(alphaLsLabel);
	caneSplitter->addWidget(casLayoutWidget);
	layout->addRow(caneSplitter);

	connect(caneTypeListBox, SIGNAL(clicked(QModelIndex)), this,
		SLOT(updateBrandColorPickerSublist(QModelIndex)));
	connect(caneColorListBox, SIGNAL(clicked(QModelIndex)), this,
		SLOT(updateBrandColorPickerColor(QModelIndex)));
		connect(caneAlphaSlider, SIGNAL(valueChanged(int)),
						this, SLOT(changeAlphaEvent(int)));

	// Shape drop-down menu
	// It is assumed that the constant values corresponding
	// to CIRCLE_SHAPE, SQUARE_SHAPE, etc. start at 1 and increase
	// and are contiguous. The shapes corresponding to these values
	// must be added to the caneShapeBox in this ascending order.
	caneShapeBox = new QComboBox(layout->widget());
	caneShapeBox->addItem("Circle");
	caneShapeBox->addItem("Half Circle");
	caneShapeBox->addItem("Third Circle");
	caneShapeBox->addItem("Square");
	caneShapeBox->addItem("Rectangle");
	caneShapeBox->addItem("Triangle");
	layout->addRow("Shape:", caneShapeBox);

	// Size slider
	caneSizeSlider = new QSlider(Qt::Horizontal, layout->widget());
        caneSizeSlider->setRange(1, 60);
//        caneSizeSlider->setSizePolicy(2);
        QBoxLayout* sliderLayout1 = new QBoxLayout(QBoxLayout::LeftToRight, layout->widget());
        QBoxLayout* sliderLayout2 = new QBoxLayout(QBoxLayout::LeftToRight, layout->widget());
        QLabel* lsLabel = new QLabel("0.1 in.", sliderLayout2->widget());
        QLabel* rsLabel = new QLabel("6 in.", sliderLayout2->widget());
        QLabel* boxLabel = new QLabel("inches", sliderLayout1->widget());
        caneSizeEditBox = new QLineEdit(sliderLayout1->widget());
        caneSizeEditBox->setValidator(new QDoubleValidator());
        caneSizeEditBox->setText("1.0");
        sliderLayout2->insertWidget(0, lsLabel);
        sliderLayout2->insertWidget(1, caneSizeSlider);
        sliderLayout2->insertWidget(2, rsLabel);
        sliderLayout1->insertWidget(0, caneSizeEditBox);
        sliderLayout1->insertWidget(1, boxLabel);

        layout->addRow("Diameter:", sliderLayout1);
        layout->addRow(sliderLayout2);

        connect(caneSizeEditBox, SIGNAL(editingFinished()),
                        this,SLOT(size_changeEditSliderFromText()));
        connect(caneSizeSlider, SIGNAL(valueChanged(int)),
                        this,SLOT(size_changeEditTextFromSlider(int)));
	connect(caneShapeBox, SIGNAL(currentIndexChanged(int)),
			this, SLOT(shapeTypeEvent(int)));
	connect(caneSizeSlider, SIGNAL(valueChanged(int)),
                        this, SLOT(shapeSizeEvent(int)));

		// Location slider
        xySliderResolution = 6;
        radiusSliderResolution = 6;
        thetaSliderResolution = 60;
        radiusMaxByDefault = 2;

		QGridLayout* xy_editorLayout = new QGridLayout(layout->widget());
        QGridLayout* rtheta_editorLayout = new QGridLayout(layout->widget());

		x_editlabel = new QLabel("X position:", xy_editorLayout->widget());
		x_editbox = new QLineEdit(xy_editorLayout->widget());
		x_editbox->setValidator(new QDoubleValidator());
		x_editbox->setText("0.0");
		x_editslider = new QSlider(Qt::Horizontal, xy_editorLayout->widget());
		x_editslider->setRange(-xySliderResolution,xySliderResolution);
		x_editslider->setValue(0);
		x_editslider->setTickPosition(QSlider::TicksBothSides);
		x_editslider->setTickInterval(1);
		y_editlabel = new QLabel("Y position:", xy_editorLayout->widget());
		y_editbox = new QLineEdit(xy_editorLayout->widget());
		y_editbox->setValidator(new QDoubleValidator());
		y_editbox->setText("0.0");
		y_editslider = new QSlider(Qt::Horizontal, xy_editorLayout->widget());
		y_editslider->setRange(-xySliderResolution,xySliderResolution);
		y_editslider->setValue(0);
		y_editslider->setTickPosition(QSlider::TicksBothSides);
		y_editslider->setTickInterval(1);

        radius_editlabel = new QLabel("Radius:", rtheta_editorLayout->widget());
        radius_editbox = new QLineEdit(rtheta_editorLayout->widget());
        radius_editbox->setValidator(new QDoubleValidator());
        radius_editbox->setText("0.0");
        radius_editslider = new QSlider(Qt::Horizontal, rtheta_editorLayout->widget());
        radius_editslider->setRange(0,radiusSliderResolution*radiusMaxByDefault);
        radius_editslider->setValue(0);
        radius_editslider->setTickPosition(QSlider::TicksBothSides);
        radius_editslider->setTickInterval(1);
        theta_editlabel = new QLabel("Angle:", rtheta_editorLayout->widget());
        theta_editbox = new QLineEdit(rtheta_editorLayout->widget());
        theta_editbox->setValidator(new QDoubleValidator());
        theta_editbox->setText("0.0");
        theta_editslider = new QSlider(Qt::Horizontal, rtheta_editorLayout->widget());
        theta_editslider->setRange(0,thetaSliderResolution-1);
        theta_editslider->setValue(0);
        theta_editslider->setTickPosition(QSlider::TicksBothSides);
        theta_editslider->setTickInterval(1);

        xy_editorLayout->addWidget(x_editlabel,0,0,1,1);
        xy_editorLayout->addWidget(x_editbox,0,2,1,1);
        xy_editorLayout->addWidget(x_editslider,1,0,1,3);
        xy_editorLayout->addWidget(y_editlabel,0,4,1,1);
        xy_editorLayout->addWidget(y_editbox,0,6,1,1);
        xy_editorLayout->addWidget(y_editslider,1,4,1,3);
        layout->addRow(xy_editorLayout);
        rtheta_editorLayout->addWidget(radius_editlabel,0,0,1,1);
        rtheta_editorLayout->addWidget(radius_editbox,0,2,1,1);
        rtheta_editorLayout->addWidget(radius_editslider,1,0,1,3);
        rtheta_editorLayout->addWidget(theta_editlabel,0,4,1,1);
        rtheta_editorLayout->addWidget(theta_editbox,0,6,1,1);
        rtheta_editorLayout->addWidget(theta_editslider,1,4,1,3);
        layout->addRow(rtheta_editorLayout);

		connect(x_editbox, SIGNAL(editingFinished()),this,SLOT(x_changeEditSliderFromText()));
		connect(x_editslider, SIGNAL(valueChanged(int)),this,SLOT(x_changeEditTextFromSlider(int)));
		connect(y_editbox, SIGNAL(editingFinished()),this,SLOT(y_changeEditSliderFromText()));
		connect(y_editslider, SIGNAL(valueChanged(int)),this,SLOT(y_changeEditTextFromSlider(int)));
        connect(radius_editbox, SIGNAL(editingFinished()),this,SLOT(radius_changeEditSliderFromText()));
        connect(radius_editslider, SIGNAL(valueChanged(int)),this,SLOT(radius_changeEditTextFromSlider(int)));
        connect(theta_editbox, SIGNAL(editingFinished()),this,SLOT(theta_changeEditSliderFromText()));
        connect(theta_editslider, SIGNAL(valueChanged(int)),this,SLOT(theta_changeEditTextFromSlider(int)));

	// Ok, cancel buttons
	QHBoxLayout* buttonLayout = new QHBoxLayout(layout->widget());
	QPushButton* okButton = new QPushButton("Ok", layout->widget());
	QPushButton* cancelButton = new QPushButton("Cancel", layout->widget());
	buttonLayout->addStretch();
	buttonLayout->addWidget(okButton);
	buttonLayout->addWidget(cancelButton);
	buttonLayout->addStretch();
	layout->addRow(buttonLayout);

	connect(cancelButton, SIGNAL(clicked()),
			this, SLOT(cancelCaneChangeDialog()));
	connect(okButton, SIGNAL(clicked()),
			changeDialog, SLOT(hide()));
	connect(changeDialog, SIGNAL(rejected()),
			this, SLOT(cancelCaneChangeDialog()));
}

void MainWindow::size_changeEditSliderFromText()
{
        QString s = caneSizeEditBox->text();
        float n = caneSizeEditBox->text().toFloat();
        if (n > 60)
        {
                caneSizeSlider->setValue(60);
        }
        else if (n < 1)
        {
                caneSizeSlider->setValue(1);
                return;
        }
        else
        {
                caneSizeSlider->setValue(round(n*10));
        }
        caneSizeEditBox->setText(s);
        emit shapeSizeEvent(n);
}

void MainWindow::size_changeEditTextFromSlider(int i)
{
        QString s;
        s.sprintf("%.1f", double(i)/double(10));
        caneSizeEditBox->setText(s);
        emit shapeSizeEvent(i);
}

void MainWindow::disconnectLocationSignals()
{
    disconnect(x_editbox, SIGNAL(editingFinished()),this,SLOT(x_changeEditSliderFromText()));
    disconnect(x_editslider, SIGNAL(valueChanged(int)),this,SLOT(x_changeEditTextFromSlider(int)));
    disconnect(y_editbox, SIGNAL(editingFinished()),this,SLOT(y_changeEditSliderFromText()));
    disconnect(y_editslider, SIGNAL(valueChanged(int)),this,SLOT(y_changeEditTextFromSlider(int)));
    disconnect(radius_editbox, SIGNAL(editingFinished()),this,SLOT(radius_changeEditSliderFromText()));
    disconnect(radius_editslider, SIGNAL(valueChanged(int)),this,SLOT(radius_changeEditTextFromSlider(int)));
    disconnect(theta_editbox, SIGNAL(editingFinished()),this,SLOT(theta_changeEditSliderFromText()));
    disconnect(theta_editslider, SIGNAL(valueChanged(int)),this,SLOT(theta_changeEditTextFromSlider(int)));
}

void MainWindow::reconnectLocationSignals()
{
    connect(x_editbox, SIGNAL(editingFinished()),this,SLOT(x_changeEditSliderFromText()));
    connect(x_editslider, SIGNAL(valueChanged(int)),this,SLOT(x_changeEditTextFromSlider(int)));
    connect(y_editbox, SIGNAL(editingFinished()),this,SLOT(y_changeEditSliderFromText()));
    connect(y_editslider, SIGNAL(valueChanged(int)),this,SLOT(y_changeEditTextFromSlider(int)));
    connect(radius_editbox, SIGNAL(editingFinished()),this,SLOT(radius_changeEditSliderFromText()));
    connect(radius_editslider, SIGNAL(valueChanged(int)),this,SLOT(radius_changeEditTextFromSlider(int)));
    connect(theta_editbox, SIGNAL(editingFinished()),this,SLOT(theta_changeEditSliderFromText()));
    connect(theta_editslider, SIGNAL(valueChanged(int)),this,SLOT(theta_changeEditTextFromSlider(int)));
}

void MainWindow::x_changeEditSliderFromText()
{
        disconnectLocationSignals();
        QString s = x_editbox->text();
        float n = x_editbox->text().toFloat();
        if (n > xySliderResolution)
        {
                x_editslider->setValue(xySliderResolution);
        }
        else if (n < -xySliderResolution)
        {
                x_editslider->setValue(-xySliderResolution);
        }
        else
        {
            x_editslider->setValue(round(n * xySliderResolution));
        }
        x_editbox->setText(s);
        changeRThetaFromXY();
}

void MainWindow::x_changeEditTextFromSlider(int i)
{
        disconnectLocationSignals();
        QString s;
        s.sprintf("%.2f", double(i)/double(xySliderResolution));
        x_editbox->setText(s);
        changeRThetaFromXY();
}

void MainWindow::y_changeEditSliderFromText()
{
        disconnectLocationSignals();
        QString s = y_editbox->text();
        float n = y_editbox->text().toFloat();
        if (n > xySliderResolution)
        {
                y_editslider->setValue(xySliderResolution);
        }
        else if (n < -xySliderResolution)
        {
                y_editslider->setValue(-xySliderResolution);
        }
        else
        {
            y_editslider->setValue(round(n * xySliderResolution));
        }
        y_editbox->setText(s);
        changeRThetaFromXY();
}

void MainWindow::y_changeEditTextFromSlider(int i)
{
        disconnectLocationSignals();
        QString s;
        s.sprintf("%.2f", double(i)/double(xySliderResolution));
        y_editbox->setText(s);
        changeRThetaFromXY();
}

void MainWindow::radius_changeEditSliderFromText()
{
        disconnectLocationSignals();
        QString s = radius_editbox->text();
        float n = radius_editbox->text().toFloat();
        if (n > radiusMaxByDefault)
        {
                radius_editslider->setValue(radiusSliderResolution*radiusMaxByDefault);
        }
        else if (n < 0)
        {
                s.sprintf("%.2f", -n);
                if (-n > radiusMaxByDefault)
                {
                        radius_editslider->setValue(radiusSliderResolution*radiusMaxByDefault);
                }
                else
                {
                        radius_editslider->setValue(round(-n*radiusSliderResolution));
                }
                float r = theta_editbox->text().toFloat();
                QString* r_string = new QString();
                if (r >= 180)
                        r_string->sprintf("%.2f", r - 180);
                else
                        r_string->sprintf("%.2f", r + 180);
                theta_editslider->setValue(round(r / thetaSliderResolution));
                theta_editbox->setText(*r_string);
        }
        else
        {
                radius_editslider->setValue(round(n * radiusSliderResolution));
        }
        radius_editbox->setText(s);
        changeXYFromRTheta();
}

void MainWindow::radius_changeEditTextFromSlider(int i)
{
        disconnectLocationSignals();
        QString s;
        s.sprintf("%.2f", double(i)/double(radiusSliderResolution));
        radius_editbox->setText(s);
        changeXYFromRTheta();
}

void MainWindow::theta_changeEditSliderFromText()
{
        disconnectLocationSignals();
        QString s = theta_editbox->text();
        float n = theta_editbox->text().toFloat();
        while (n >= 360)
                n = n - 360;
        while (n < 0)
                n = n + 360;
        s.sprintf("%.2f", n);
        theta_editslider->setValue(round(n * thetaSliderResolution / 360.0));
        theta_editbox->setText(s);
        changeXYFromRTheta();
}

void MainWindow::theta_changeEditTextFromSlider(int i)
{
        disconnectLocationSignals();
        QString s;
        s.sprintf("%.2f", i * 360 / double(thetaSliderResolution));
        theta_editbox->setText(s);
        changeXYFromRTheta();
}

void MainWindow::changeRThetaFromXY()
{
        float new_r = sqrt(pow(x_editbox->text().toFloat(),2) + pow(y_editbox->text().toFloat(),2));
        float new_theta = atan(y_editbox->text().toFloat() / x_editbox->text().toFloat())*180.0/3.14159265;
        if (x_editbox->text().toFloat() < 0)
                new_theta = new_theta + 180;
        if (new_theta < 0)
                new_theta = new_theta + 360;
        QString* r = new QString();
        QString* theta = new QString();
        r->sprintf("%.2f", new_r);
        theta->sprintf("%.2f", new_theta);
        if (new_r > radiusMaxByDefault)
        {
                radius_editslider->setValue(radiusSliderResolution*radiusMaxByDefault);
        }
        else
        {
                radius_editslider->setValue(round(new_r * radiusSliderResolution));
        }
        radius_editbox->setText(*r);
        theta_editslider->setValue(round(new_theta * thetaSliderResolution / 360.0));
        theta_editbox->setText(*theta);
        model->setSubcaneLocation(caneChangeSubcane,x_editbox->text().toFloat(),y_editbox->text().toFloat(),0);
        reconnectLocationSignals();
}

void MainWindow::changeXYFromRTheta()
{
        float new_x = radius_editbox->text().toFloat() * cos(theta_editbox->text().toFloat()*3.14159265/180.0);
        float new_y = radius_editbox->text().toFloat() * sin(theta_editbox->text().toFloat()*3.14159265/180.0);
        QString* x = new QString();
        QString* y = new QString();
        x->sprintf("%.2f", new_x);
        y->sprintf("%.2f", new_y);
        if (new_x > xySliderResolution)
        {
                x_editslider->setValue(xySliderResolution);
        }
        else if (new_x < -xySliderResolution)
        {
                x_editslider->setValue(-xySliderResolution);
        }
        else
        {
                x_editslider->setValue(round(new_x * xySliderResolution));
        }
        x_editbox->setText(*x);
        if (new_y > xySliderResolution)
        {
                y_editslider->setValue(xySliderResolution);
        }
        else if (new_y < -xySliderResolution)
        {
                y_editslider->setValue(-xySliderResolution);
        }
        else
        {
                y_editslider->setValue(round(new_y * xySliderResolution));
        }
        y_editbox->setText(*y);
        model->setSubcaneLocation(caneChangeSubcane,x_editbox->text().toFloat(),y_editbox->text().toFloat(),0);
        reconnectLocationSignals();
}

void MainWindow::updateCaneAlphaSlider(int i)
{
		if (i > 255 || i < 0)
				return;
		if (i == 255)
		{
				caneAlphaSlider->setValue(254);
				caneAlphaSlider->setEnabled(false);
		}
		else
		{
				caneAlphaSlider->setEnabled(true);
				caneAlphaSlider->setValue(i);
		}
}

void MainWindow::changeAlphaEvent(int i)
{
	model->setSubcaneAlpha(caneChangeSubcane, i);
}

void MainWindow::cancelCaneChangeDialog()
{
	revertCaneColorAndShape();
	changeDialog->hide();
}

void MainWindow :: saveCaneColorAndShape()
{
	Color* c = model->getSubcaneColor(caneChangeSubcane);
	savedColor.r = c->r;
	savedColor.g = c->g;
	savedColor.b = c->b;
	savedColor.a = c->a;

	model->getSubcaneShape(caneChangeSubcane)->copy(&savedShape);

        savedLocation = *(model->getSubcaneLocation(caneChangeSubcane));
}

void MainWindow :: revertCaneColorAndShape()
{
	model->setSubcaneColor(caneChangeSubcane, &savedColor);
	model->setSubcaneShape(caneChangeSubcane, &savedShape);
        model->setSubcaneLocation(caneChangeSubcane, savedLocation.x, savedLocation.y, savedLocation.z);
}

void MainWindow::updateBrandColorPickerColor(QModelIndex i)
{
	selectedColor = i.row();
	if (selectedBrand == -1 || selectedColor == -1 || selectedBrand >= caneColorListList->size() ||
			selectedColor >= caneColorListList->at(selectedBrand).size())
		return;
	QColor color = caneColorListList->at(selectedBrand).at(selectedColor);
	Color c;
		c.r = color.redF();
		c.g = color.greenF();
		c.b = color.blueF();
		c.a = color.alphaF();
		updateCaneAlphaSlider(c.a*255);
		model->setSubcaneColor(caneChangeSubcane, &c);
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

#define NCOLORPATHS 2
QString colorPaths[NCOLORPATHS] = {"Colors1.txt", "../src/Colors1.txt"};

void MainWindow::loadOfficialCanes()
{
	caneTypeList = new QStringList;
	caneNameListList = new QList<QStringList>;
	caneColorListList = new QList<QList<QColor> >;
	QStringList* caneNameList = new QStringList();
	QList<QColor>* caneColorList = new QList<QColor>();
	int currentCane = -1;
	bool onColor = false;

	int i;
	for (i = 0; i < NCOLORPATHS; i++)
		if (QFile::exists (colorPaths[i]))
			break;
	if (i >= NCOLORPATHS)
	{
		caneTypeList->append("Could not find "+colorPaths[0]);
		return;
	}
	QFile file(colorPaths[i]);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		caneTypeList->append("Could not read "+colorPaths[0]);
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

class SVGPath {
public:
	SVGPath() : color(make_vector(0.5f, 0.5f, 0.5f, 1.0f)) {
	}
	Vector4f color;
	vector< Vector2d > points;
	vector< Vector3ui > tris;
};

void remove_triangle(Vector3ui const &tri, map< Vector2ui, Vector3ui > &edge_to_tri, set< Vector3ui > &unclaimed_tris) {
	for (unsigned int i = 0; i < 3; ++i) {
		Vector2ui edge = make_vector(tri.c[i], tri.c[(i+1)%3]);
		map< Vector2ui, Vector3ui >::iterator f = edge_to_tri.find(edge);
		assert(f != edge_to_tri.end());
		edge_to_tri.erase(f);
	}
	set< Vector3ui >::iterator tf = unclaimed_tris.find(tri);
	assert(tf != unclaimed_tris.end());
	unclaimed_tris.erase(tf);
}

//get bounds, without actually going to all the trouble of triangulating:
void update_bounds(double tol, SVG::Matrix const &xform, SVG::Node const &node, Box2d &bounds) {
	vector< vector< Vector2d > > node_paths;
	node.execute(xform, tol, node_paths);
	for (vector< vector< Vector2d > >::iterator path = node_paths.begin(); path != node_paths.end(); ++path) {
		for (vector< Vector2d >::const_iterator v = path->begin(); v != path->end(); ++v) {
			bounds.min = min(bounds.min, *v);
			bounds.max = max(bounds.max, *v);
		}
	}

	for (std::list< SVG::Node >::const_iterator n = node.children.begin(); n != node.children.end(); ++n) {
		SVG::Matrix next_xform = xform * make_matrix(n->transform, transpose(make_matrix(make_vector(0.0, 0.0, 1.0))));
		update_bounds(tol, next_xform, *n, bounds);
	}
}

void run_nodes(double tol, SVG::Matrix const &prev_xform, SVG::Node const &node, Vector2d &center, double &inch, vector< SVGPath > &paths) {
	SVG::Matrix xform = prev_xform * make_matrix(node.transform, transpose(make_matrix(make_vector(0.0, 0.0, 1.0))));
	if (node.tag == "inch" || node.tag == "center") {
		//need to run children to get center or size:
		double diag_scale = length(xform * make_vector(1.0, 1.0, 0.0));
		double sub_tol = tol;
		if (diag_scale > 0.001) {
			sub_tol /= diag_scale;
		}
		Box2d bounds;
		bounds.min = make_vector< double, 2 >(numeric_limits< double >::infinity());
		bounds.max = make_vector< double, 2 >(-numeric_limits< double >::infinity());
		SVG::Matrix identity = identity_matrix< double, 2, 3 >();

		update_bounds(sub_tol, identity, node, bounds);
		if (bounds.min.x <= bounds.max.x) {
			if (node.tag == "center") {
				center = xform * make_vector(bounds.center(), 1.0);
			} else if (node.tag == "inch") {
				inch = length(xform * make_vector(bounds.size().x, 0.0, 0.0));
			} else {
				assert(0);
			}
		}
		return;
	} else if (node.tag != "") {
		std::cerr << "WARNING: ignoring tag '" << node.tag << "' in SVG." << std::endl;
		return;
	}
	vector< vector< Vector2d > > node_paths;
	node.execute(xform, tol, node_paths);
	Box2d bounds;
	bounds.min = make_vector< double, 2 >(numeric_limits< double >::infinity());
	bounds.max = make_vector< double, 2 >(-numeric_limits< double >::infinity());
	for (vector< vector< Vector2d > >::iterator path = node_paths.begin(); path != node_paths.end(); ++path) {
		for (vector< Vector2d >::const_iterator v = path->begin(); v != path->end(); ++v) {
			bounds.min = min(bounds.min, *v);
			bounds.max = max(bounds.max, *v);
		}
	}

	if (bounds.min.x < bounds.max.x) {
		vector< Vector2d > points;
		vector< Vector3ui > tris;
		PlanarMap::fill(node.fill_rule, node_paths, bounds, points, tris);

		set< Vector3ui > unclaimed_tris(tris.begin(), tris.end()); //should be an unordered_set but I don't recall how to get that to compile on OSX.
		map< Vector2ui, Vector3ui > edge_to_tri;
		for (vector< Vector3ui >::const_iterator tri = tris.begin(); tri != tris.end(); ++tri) {
			assert(!edge_to_tri.count(make_vector(tri->c[0], tri->c[1])));
			assert(!edge_to_tri.count(make_vector(tri->c[1], tri->c[2])));
			assert(!edge_to_tri.count(make_vector(tri->c[2], tri->c[0])));
			edge_to_tri.insert(make_pair(make_vector(tri->c[0], tri->c[1]), *tri));
			edge_to_tri.insert(make_pair(make_vector(tri->c[1], tri->c[2]), *tri));
			edge_to_tri.insert(make_pair(make_vector(tri->c[2], tri->c[0]), *tri));
		}

		while (!unclaimed_tris.empty()) {
			Vector3ui seed = *unclaimed_tris.begin();
			remove_triangle(seed, edge_to_tri, unclaimed_tris);
			vector< Vector3ui > used_triangles(1, seed);
			vector< Vector2ui > edges_to_expand;
			edges_to_expand.push_back(make_vector(seed[0],seed[1]));
			edges_to_expand.push_back(make_vector(seed[1],seed[2]));
			edges_to_expand.push_back(make_vector(seed[2],seed[0]));
			while (!edges_to_expand.empty()) {
				Vector2ui last_edge = edges_to_expand.back();
				edges_to_expand.pop_back();
				//see if we can expand this edge by finding a triangle on the other side of it:
				map< Vector2ui, Vector3ui >::iterator f = edge_to_tri.find(make_vector(last_edge[1], last_edge[0]));
				if (f == edge_to_tri.end()) {
					//No triangle. This edge is done!
					continue;
				}
				//There was a triangle.
				Vector3ui tri = f->second;
				//remove free triangle data structures, add to claimed list:
				remove_triangle(tri, edge_to_tri, unclaimed_tris);
				used_triangles.push_back(tri);
				//Figure out which new edges to add:
				bool found = false;
				for (unsigned int i = 0; i < 3; ++i) {
					unsigned int a = tri[i];
					unsigned int b = tri[(i+1)%3];
					unsigned int c = tri[(i+2)%3];
					if (a == last_edge[1] && b == last_edge[0]) {
						assert(!found);
						found = true;
						edges_to_expand.push_back(make_vector(b,c));
						edges_to_expand.push_back(make_vector(c,a));
					}
				}
				assert(found);
			} //while (edges to expand)

			assert(!used_triangles.empty());

			SVGPath path;
			path.color = node.fill_paint.color;
			path.color.w *= node.fill_paint.opacity;

			vector< unsigned int > new_idx(points.size(), -1U);
			for (vector< Vector3ui >::const_iterator tri = used_triangles.begin(); tri != used_triangles.end(); ++tri) {
				Vector3ui out = *tri;
				for (unsigned int i = 0; i < 3; ++i) {
					unsigned int &v = out[i];
					assert(v < new_idx.size());
					if (new_idx[v] >= path.points.size()) {
						new_idx[v] = path.points.size();
						path.points.push_back(points[v]);
					}
					v = new_idx[v];
				}
				path.tris.push_back(out);
			}

			paths.push_back(path);
		} //while (triangles exist to claim)

	} //if (nonempty node)

	for (std::list< SVG::Node >::const_iterator n = node.children.begin(); n != node.children.end(); ++n) {
		run_nodes(tol, xform, *n, center, inch, paths);
	}

}

void MainWindow::loadSVGFileDialog()
{
	QString file = QFileDialog::getOpenFileName(this, tr("Load SVG file"), "", tr("Scalable Vector Graphics files (*.svg);;All files (*)"));
	if (!file.isNull())
	{
		SVG::SVG svg;
		if (!SVG::load_svg(qPrintable(file), svg))
		{
			QMessageBox::warning(this, tr("Loading SVG file failed."), tr("Loading a cane from SVG failed. There may be words about this on the console."));
		}
		else
		{
			double page_rad = length(svg.page);
			if (page_rad == 0.0) {
				std::cerr << "WARNING: page is very small." << std::endl;
				page_rad = 1.0;
			}
			//map SVG page to unit square -- will actually remap later
			//based on contained scale data, but this is a starting point.
			SVG::Matrix page_to_unit = identity_matrix< double, 2, 3 >();
			page_to_unit(0,0) =  2.0 / page_rad;
			page_to_unit(1,1) = -2.0 / page_rad; //flip SVG coords to reasonable coords.
			page_to_unit(0,2) = -0.5 * svg.page.x * page_to_unit(0,0);
			page_to_unit(1,2) = -0.5 * svg.page.y * page_to_unit(1,1);

			Vector2d center = make_vector(0.0, 0.0);
			double inch = 1.0;
			vector< SVGPath > paths;
			run_nodes(0.01, page_to_unit, svg.root, center, inch, paths);
			std::cerr << "Inch is " << inch << " units." << std::endl;
			std::cerr << "Center is at " << center << "." << std::endl;
			double to_inches = 1.0;
			if (inch > 0.001) {
				to_inches = 1.0 / inch;
			}
			//Center on 'center' and convert to inches:
			Box2d test_bounds;
			test_bounds.min = make_vector< double, 2 >(numeric_limits< double >::infinity());
			test_bounds.max = -test_bounds.min;
			for (vector< SVGPath >::iterator path = paths.begin(); path != paths.end(); ++path) {
				for (vector< Vector2d >::iterator p = path->points.begin(); p != path->points.end(); ++p) {
					*p = (*p - center) * to_inches;
					test_bounds.min = min(test_bounds.min, *p);
					test_bounds.max = max(test_bounds.max, *p);
				}
			}
			std::cerr << "After transformation, cane fits in " << test_bounds.min << " to " << test_bounds.max << " box." << std::endl;

			//build cane:
			Cane* base = new Cane(BUNDLE_CANETYPE);
			for (vector< SVGPath >::iterator path = paths.begin(); path != paths.end(); ++path) {
				assert(!path->tris.empty());
				Box2d bounds;
				bounds.min = make_vector< double, 2 >(numeric_limits< double >::infinity());
				bounds.max = -bounds.min;
				for (vector< Vector2d >::const_iterator p = path->points.begin(); p != path->points.end(); ++p) {
					bounds.min = min(bounds.min, *p);
					bounds.max = max(bounds.max, *p);
				}
				assert(bounds.min.x <= bounds.max.x);
				assert(bounds.min.y <= bounds.max.y);

				Vector2d local_center = bounds.center();

				Cane* child = new Cane(BASE_POLYGONAL_CANETYPE);
				child->color = path->color;
				child->shape.type = UNDEFINED_SHAPE;
				child->shape.vertices.resize(path->points.size());
				for (unsigned int i = 0; i < path->points.size(); ++i) {
					child->shape.vertices[i] = make_vector< float >(path->points[i] - local_center);
				}
				child->shape.tris = path->tris;
				child->shape.diameter = 1.0f;
				model->setCane(child);
				saveCaneToLibrary();
				if (base->subcaneCount < MAX_SUBCANE_COUNT) {
					base->subcanes[base->subcaneCount] = child;
					base->subcaneLocations[base->subcaneCount].xy = make_vector< float >(local_center);
					base->subcaneLocations[base->subcaneCount].z = 0.0f;
					base->subcaneCount += 1;
				} else {
					std::cout << "Ran out of subcanes." << std::endl;
				}

				//TODO: figure out how not to leak 'base'
			}
			model->setCane(base);
			saveCaneToLibrary();

		}
	}
}

void MainWindow::saveObjFileDialog()
{
	QString file = QFileDialog::getSaveFileName(this, tr("Save obj file"), "", tr("Wavefront obj files (*.obj);;All files (*)"));
	if (!file.isNull())
	{
		model->saveObjFile(qPrintable(file));
	}
}

void MainWindow::saveRawFile()
{
	model->saveRawFile("cane.raw");
}

void MainWindow::toggleFlat()
{
	toggle2D_button->setDown(openglWidget->is2D());
}

void MainWindow::setupButtonBar()
{
	int iconSize = this->size().height()/15;
		cane_button = new QPushButton("New Cane");
		cane_button->setToolTip("Add a New Cane to the Piece.");
		cane_button->setShortcut(QKeySequence("CTRL+N"));
		QImage cane_icon ("../src/icon_newcane.png");
		cane_button->setIcon(QPixmap::fromImage(cane_icon));
		cane_button->setIconSize(QSize(iconSize,iconSize));
	pull_button = new QPushButton("Pull");
	pull_button->setToolTip("Drag Mouse Horizontally to Twist, Vertically to Stretch. Use Shift to twist and stretch independently.");
	pull_button->setShortcut(QKeySequence("CTRL+P"));
	QImage pull_icon ("../src/icon_pull.png");
	pull_button->setIcon(QPixmap::fromImage(pull_icon));
	pull_button->setIconSize(QSize(iconSize,iconSize));
	bundle_button = new QPushButton("Bundle");
	bundle_button->setShortcut(QKeySequence("CTRL+B"));
	QImage bundle_icon ("../src/icon_bundle.png");
	bundle_button->setIcon(QPixmap::fromImage(bundle_icon));
	bundle_button->setIconSize(QSize(iconSize,iconSize));
	flatten_button = new QPushButton("Flatten");
	flatten_button->setToolTip("Drag Mouse Horizontally to Squish, Vertically to Flatten");
	flatten_button->setShortcut(QKeySequence("CTRL+F"));
	QImage flatten_icon ("../src/icon_flatten.png");
	flatten_button->setIcon(QPixmap::fromImage(flatten_icon));
	flatten_button->setIconSize(QSize(iconSize,iconSize));
	toggle2D_button = new QPushButton("2D View");
	toggle2D_button->setToolTip(tr("Switch between 2D and 3D view."));
	connect(toggle2D_button, SIGNAL(clicked()), this, SLOT(toggleFlat()));
	QImage toggle2D_icon ("../src/icon_2dview.png");
	toggle2D_button->setIcon(QPixmap::fromImage(toggle2D_icon));
	toggle2D_button->setIconSize(QSize(iconSize,iconSize));
	undo_button = new QPushButton("Undo");
	undo_button->setShortcut(QKeySequence("CTRL+Z"));
	undo_button->setToolTip("Undo the last operation.");
	QImage undo_icon ("../src/icon_undo.png");
	undo_button->setIcon(QPixmap::fromImage(undo_icon));
	undo_button->setIconSize(QSize(iconSize,iconSize));
	redo_button = new QPushButton("Redo");
	redo_button->setShortcut(QKeySequence("CTRL+Y"));
	redo_button->setToolTip("Redo the last operation.");
	QImage redo_icon ("../src/icon_redo.png");
	redo_button->setIcon(QPixmap::fromImage(redo_icon));
	redo_button->setIconSize(QSize(iconSize,iconSize));
	save_button = new QPushButton("Save");
	save_button->setToolTip("Save Current Model to Library");
	save_button->setShortcut(QKeySequence("CTRL+S"));
	QImage save_icon ("../src/icon_save.png");
	save_button->setIcon(QPixmap::fromImage(save_icon));
	save_button->setIconSize(QSize(iconSize,iconSize));
	clear_button = new QPushButton("Clear");
	clear_button->setToolTip("Clear Current Model");
	clear_button->setShortcut(QKeySequence("CTRL+R"));
	QImage clear_icon ("../src/icon_clear.png");
	clear_button->setIcon(QPixmap::fromImage(clear_icon));
	clear_button->setIconSize(QSize(iconSize,iconSize));

	previewLabel = new QLabel();
	previewLabel->setFixedSize(100,100);
	previewLabel->setScaledContents(true);

	hiddenLabel = new QLabel();
	hiddenLabel->setFixedSize(100,100);
	hiddenLabel->setScaledContents(true);

	operButton_layout = new QVBoxLayout();
		operButton_layout->addWidget(cane_button);
	operButton_layout->addWidget(pull_button);
	operButton_layout->addWidget(bundle_button);
	operButton_layout->addWidget(flatten_button);

	operButton_layout->addWidget(toggle2D_button);
	operButton_layout->addWidget(redo_button);
	operButton_layout->addWidget(undo_button);
	operButton_layout->addWidget(save_button);
	operButton_layout->addWidget(clear_button);
	operButton_layout->addWidget(previewLabel,1, Qt::AlignHCenter);
	operButton_layout->addWidget(hiddenLabel,1, Qt::AlignHCenter);
	previewLabel->setHidden(true);
	hiddenLabel->setHidden(false);
}

void MainWindow::setupOGLArea()
{
	oglLayoutWidget = new QWidget(stackLayout->widget());
	QHBoxLayout* oglLayout = new QHBoxLayout();
	oglLayoutWidget->setLayout(oglLayout);

	// Setup opengl 3D view
	openglWidget = new OpenGLWidget(oglLayoutWidget, model);
	oglLayout->addWidget(openglWidget, 1);

	// Setup slider for how much length of cane is shown
	oglGeometryHeightSlider = new QSlider(Qt::Vertical, oglLayoutWidget);
	oglGeometryHeightSlider->setRange(1, 24);
	oglGeometryHeightSlider->setSliderPosition(6);
	oglGeometryHeightSlider->setTickInterval(1);
	oglGeometryHeightSlider->setTickPosition(QSlider::TicksBothSides);
	connect(oglGeometryHeightSlider, SIGNAL(valueChanged(int)),
		this, SLOT(geometryHeightEvent(int)));

	// Setup slider with its labels
	QVBoxLayout* oglSliderLayout = new QVBoxLayout();
	QLabel* bsLabel = new QLabel("1 in.", oglLayoutWidget);
	QLabel* tsLabel = new QLabel("24 in.", oglLayoutWidget);
	oglSliderLayout->addWidget(tsLabel);
	oglSliderLayout->addWidget(oglGeometryHeightSlider);
	oglSliderLayout->addWidget(bsLabel);

	oglLayout->addLayout(oglSliderLayout);
}

void MainWindow::geometryHeightEvent(int)
{
	float newHeight = oglGeometryHeightSlider->sliderPosition() / 6.0;

	model->setGeometryHeight(newHeight);
	openglWidget->setGeometryHeight(newHeight);
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
		hiddenLabel->setHidden(true);
	} else
	{
		openglWidget->setVisible(true);
		stackLayout->setCurrentWidget(oglLayoutWidget);
		previewLabel->setHidden(true);
		hiddenLabel->setHidden(false);
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
	//previewLabel->setPixmap(QPixmap::grabWidget(openglWidget));
}
