#include "mainwindow.h"
#include <fstream>

MainWindow::MainWindow()
{
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

void MainWindow::libraryCaneDestroyed(QObject* obj)
{
    stockLayout->removeWidget((QWidget*) obj);

    statusBar->showMessage("Deleted Cane From Library", 2000);
}

void MainWindow::saveCaneToLibrary()
{
    LibraryCaneWidget* lc = new LibraryCaneWidget((OpenGLWidget*) this->glassgl,
               this->glassgl->getCane()->deepCopy(), 0);
    stockLayout->addWidget(lc);
    connect(stockLayout,SIGNAL(destroyed(QObject*)),this,SLOT(libraryCaneDestroyed(QObject*)));

    statusBar->showMessage("Saved Cane to Library", 2000);
}

void MainWindow::userModeChanged(int mode) {
    statusBar->showMessage("Switching User Mode", 2000);
    switch(mode)
    {
    case LOOK_MODE:
       modeLabel->setText("LOOK MODE");
       break;
    case PULL_MODE:
       modeLabel->setText("PULL MODE");
       break;
    case BUNDLE_MODE:
       modeLabel->setText("BUNDLE MODE");
       break;
    case FLATTEN_MODE:
       modeLabel->setText("FLATTEN MODE");
       break;
    default:
       modeLabel->setText("NO MODE");
    }
}

void MainWindow::setupStatusBar()
{
    statusBar = new QStatusBar(this);
    modeLabel = new QLabel("NO MODE",statusBar);
    statusBar->addPermanentWidget(modeLabel);
    setStatusBar(statusBar);
    connect(glassgl,SIGNAL(modeChanged(int)),this,SLOT(userModeChanged(int)));
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
    Cane* stch = new Cane(STRETCH_CANETYPE);
    Cane* stretch = new Cane(STRETCH_CANETYPE);
    Cane* twist = new Cane(TWIST_CANETYPE);
    Cane* bundle = new Cane(BUNDLE_CANETYPE);

    stch->subcaneCount = 1;
    stch->subcanes[0] = c;
    stch->amts[0] = 100.0;

    c->color.r = 0.8;
    c->color.g = 0.8;
    c->color.b = 0.8;
    c->color.a = 0.3;

    glassgl->setFocusCane(stch);
    saveCaneToLibrary();
    c->color.r = 1.0;
    c->color.g = 0.5;
    c->color.b = 0.5;
    c->color.a = 0.7;
    glassgl->setFocusCane(stch);
    saveCaneToLibrary();
    c->color.r = 0.5;
    c->color.g = 1.0;
    c->color.b = 0.5;
    glassgl->setFocusCane(stch);
    saveCaneToLibrary();
    c->color.r = 0.5;
    c->color.g = 0.5;
    c->color.b = 1.0;
    glassgl->setFocusCane(stch);
    saveCaneToLibrary();

    // tmp code to test whether stretch and twist are commutative
    twist->amts[0] = 10.0;
    stretch->amts[0] = 10.0;

    bundle->subcaneCount = 1;
    twist->subcaneCount = 1;
    stretch->subcaneCount = 1;

    bundle->subcanes[0] = stch;
    bundle->subcaneLocations[0].x = 0.2;
    bundle->subcaneLocations[0].y = 0.2;

    stretch->subcanes[0] = twist;
    twist->subcanes[0] = bundle;

    glassgl->setFocusCane(stretch);
    saveCaneToLibrary();

    twist->subcanes[0] = stretch;
    stretch->subcanes[0] = bundle;

    glassgl->setFocusCane(twist);
    saveCaneToLibrary();
    // end of tmp code


    glassgl->zeroCanes();

    statusBar->showMessage("Default Library Loaded", 2000);
}

void MainWindow::zoomInButtonPressed()
{
    glassgl->zoomIn();

    statusBar->showMessage("Zoomed In", 2000);
}

void MainWindow::zoomOutButtonPressed()
{
    glassgl->zoomOut();

    statusBar->showMessage("Zoomed Out", 2000);
}

void MainWindow::toggleAxesButtonPressed()
{
    glassgl->toggleAxes();

    statusBar->showMessage("Axes Toggled", 2000);
}

void MainWindow::lookButtonPressed()
{
    glassgl->setMode(LOOK_MODE);

    statusBar->showMessage("Entered Look Mode", 2000);
}

void MainWindow::topViewButtonPressed()
{
    glassgl->setMode(LOOK_MODE);
    glassgl->setCamera(0.0,0.01);
    statusBar->showMessage("Switched to Top View", 2000);
}

void MainWindow::sideViewButtonPressed()
{
    glassgl->setMode(LOOK_MODE);
    glassgl->setCamera(PI/2,PI/2);
    statusBar->showMessage("Switched to Side View", 2000);
}

void MainWindow::pullButtonPressed()
{
    glassgl->setMode(PULL_MODE);
    statusBar->showMessage("Entered Pull Mode", 2000);
}

void MainWindow::bundleButtonPressed()
{
    glassgl->setMode(BUNDLE_MODE);

    statusBar->showMessage("Entered Bundle Mode", 2000);
}

void MainWindow::nextButtonPressed()
{
    glassgl->setMode(BUNDLE_MODE);
    glassgl->advanceActiveSubcane();

}

void MainWindow::flattenButtonPressed()
{
    glassgl->setMode(FLATTEN_MODE);

    statusBar->showMessage("Entered Flatten Mode", 2000);
}

void MainWindow::wrapButtonPressed()
{
    glassgl->setMode(WRAP_MODE);

    statusBar->showMessage("Entered Wrap Mode", 2000);
}

void MainWindow::selectButtonPressed()
{
    glassgl->setMode(SELECT_MODE);

    statusBar->showMessage("Entered Select Mode", 2000);
}

void MainWindow::saveButtonPressed()
{
    if (glassgl->hasCanes())
        saveCaneToLibrary();
}

void MainWindow::clearButtonPressed()
{
    glassgl->zeroCanes();

    statusBar->showMessage("Cleared", 2000);
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

    statusBar->showMessage("Library Saved to: "+fileName, 2000);
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
        glassgl->setFocusCane(&loadCane);
        saveCaneToLibrary();
    }

    glassgl->zeroCanes();

    statusBar->showMessage("Library Loaded from: "+fileName, 2000);
}

void MainWindow::newColorPickerCaneButtonPressed()
{
    colorPickerSelected(QColorDialog::getColor());
}

void MainWindow::saveObjButtonPressed()
{
    QString file = QFileDialog::getSaveFileName(this, tr("Save obj file"), "", tr("Wavefront obj files (*.obj);;All files (*)"));
    if (!file.isNull())
    {
        glassgl->saveObjFile(qPrintable(file));
    }
}

void MainWindow::newStatus(QString message)
{
    statusBar->showMessage(message,2000);
}

void MainWindow::colorPickerSelected(QColor color)
{
    saveButtonPressed();
    clearButtonPressed();

    Cane* c = new Cane(BASE_CIRCLE_CANETYPE);
    Cane* stch = new Cane(STRETCH_CANETYPE);

    stch->subcaneCount = 1;
    stch->subcanes[0] = c;
    stch->amts[0] = 100.0;

    c->color.r = color.redF();
    c->color.g = color.greenF();
    c->color.b = color.blueF();
    c->color.a = color.alphaF();

    glassgl->setFocusCane(stch);
}

void MainWindow::setupWorkArea()
{
    glassgl = new OpenGLWidget(this);

    QPushButton* look_button = new QPushButton("Look");
    connect(look_button, SIGNAL(clicked()), this, SLOT(lookButtonPressed()));

    QPushButton* topView_button = new QPushButton("Top View");
    connect(topView_button, SIGNAL(clicked()), this, SLOT(topViewButtonPressed()));

    QPushButton* sideView_button = new QPushButton("Side View");
    connect(sideView_button, SIGNAL(clicked()), this, SLOT(sideViewButtonPressed()));

    QPushButton* zoom_in_button = new QPushButton("Zoom In");
    connect(zoom_in_button, SIGNAL(pressed()), this, SLOT(zoomInButtonPressed()));

    QPushButton* zoom_out_button = new QPushButton("Zoom Out");
    connect(zoom_out_button, SIGNAL(pressed()), this, SLOT(zoomOutButtonPressed()));

    QPushButton* toggle_axes_button = new QPushButton("Toggle Axes");
    connect(toggle_axes_button, SIGNAL(pressed()), this, SLOT(toggleAxesButtonPressed()));

    QVBoxLayout* viewButton_layout = new QVBoxLayout();
    //viewButton_layout->addWidget(look_button);
    viewButton_layout->addWidget(topView_button);
    viewButton_layout->addWidget(sideView_button);
    viewButton_layout->addWidget(zoom_in_button);
    viewButton_layout->addWidget(zoom_out_button);
    viewButton_layout->addWidget(toggle_axes_button);
    QWidget* viewButtonWidget = new QWidget();
    viewButtonWidget->setLayout(viewButton_layout);

    QPushButton* pull_button = new QPushButton("Pull");
    connect(pull_button, SIGNAL(pressed()), this, SLOT(pullButtonPressed()));
    pull_button->setToolTip("Drag Mouse Horizontally to Twist, Vertically to Stretch. Use Shift to twist and stretch independently.");

    QPushButton* bundle_button = new QPushButton("Bundle");
    connect(bundle_button, SIGNAL(pressed()), this, SLOT(bundleButtonPressed()));

    QPushButton* flatten_button = new QPushButton("Flatten");
    connect(flatten_button, SIGNAL(pressed()), this, SLOT(flattenButtonPressed()));
    flatten_button->setToolTip("Drag Mouse Horizontally to Squish, Vertically to Flatten");

    QPushButton* wrap_button = new QPushButton("Wrap");
    connect(wrap_button, SIGNAL(pressed()), this, SLOT(wrapButtonPressed()));
    wrap_button->setToolTip("Not Implemented - Select must be functional first");

    QPushButton* select_button = new QPushButton("Select");
    connect(select_button, SIGNAL(pressed()), this, SLOT(selectButtonPressed()));
    select_button->setToolTip("Click on a cane to select it.");

    QVBoxLayout* operButton_layout = new QVBoxLayout();
    operButton_layout->addWidget(pull_button);
    operButton_layout->addWidget(bundle_button);
    operButton_layout->addWidget(flatten_button);
    operButton_layout->addWidget(wrap_button);
    operButton_layout->addWidget(select_button);

    QWidget* operButtonWidget = new QWidget();
    operButtonWidget->setLayout(operButton_layout);

    QPushButton* next_button = new QPushButton("Next");
    connect(next_button, SIGNAL(pressed()), this, SLOT(nextButtonPressed()));
    next_button->setToolTip("Next Cane in Current Model");

    QPushButton* save_button = new QPushButton("Save");
    connect(save_button, SIGNAL(pressed()), this, SLOT(saveButtonPressed()));
    save_button->setToolTip("Save Current Model to Library");

    QPushButton* clear_button = new QPushButton("Clear");
    connect(clear_button, SIGNAL(pressed()), this, SLOT(clearButtonPressed()));
    clear_button->setToolTip("Clear Current Model");

    QPushButton* exportLibrary_button = new QPushButton("Export Library");
    connect(exportLibrary_button, SIGNAL(pressed()), this, SLOT(exportLibraryButtonPressed()));
    exportLibrary_button->setToolTip("Save Library to File");

    QPushButton* importLibrary_button = new QPushButton("Import Library");
    connect(importLibrary_button, SIGNAL(pressed()), this, SLOT(importLibraryButtonPressed()));
    importLibrary_button->setToolTip("Load Library from File");

    QPushButton* colorPicker_button = new QPushButton("New Cane Color");
    connect(colorPicker_button, SIGNAL(pressed()), this, SLOT(newColorPickerCaneButtonPressed()));
    colorPicker_button->setToolTip("Create a New Cane with specified color");

    QPushButton* saveObj_button = new QPushButton("Save .obj file");
    connect(saveObj_button, SIGNAL(pressed()), this, SLOT(saveObjButtonPressed()));
    saveObj_button->setToolTip("Save an .obj file with the current cane geometry for rendering in an external program.");

    QVBoxLayout* utilButton_layout = new QVBoxLayout();
    utilButton_layout->addWidget(next_button);
    utilButton_layout->addWidget(save_button);
    utilButton_layout->addWidget(clear_button);
    utilButton_layout->addWidget(exportLibrary_button);
    utilButton_layout->addWidget(importLibrary_button);
    utilButton_layout->addWidget(colorPicker_button);
    utilButton_layout->addWidget(saveObj_button);
    QWidget* utilButtonWidget = new QWidget();
    utilButtonWidget->setLayout(utilButton_layout);

    tabWidget = new QTabWidget();
    tabWidget->setTabPosition(QTabWidget::North);
    tabWidget->addTab(viewButtonWidget,"View");
    tabWidget->addTab(operButtonWidget,"Operations");
    tabWidget->addTab(utilButtonWidget,"Util");
    connect(tabWidget,SIGNAL(currentChanged(int)),this,SLOT(modeSelect(int)));

    QVBoxLayout* button_layout = new QVBoxLayout();
    button_layout->addWidget(tabWidget);

    QHBoxLayout* workLayout = new QHBoxLayout();
    workLayout->addLayout(button_layout);
    workLayout->addWidget(glassgl, 1);
    windowLayout->addLayout(workLayout, 5);
}

void MainWindow::modeSelect(int index)
{
    glassgl->setMode(LOOK_MODE);
    statusBar->showMessage("Entered Look Mode", 2000);
}


void MainWindow::keyPressEvent(QKeyEvent* e)
{
    switch (e->key())
    {
    case 0x58: // X
       exit(0);
    case 0x31: // 1
       this->lookButtonPressed();
       break;
    case 0x32: // 2
       this->pullButtonPressed();
       break;
    case 0x34: // 4
       this->bundleButtonPressed();
       break;
    case 0x35: // 5
       this->flattenButtonPressed();
       break;
    case 0x01000020: // Shift
       glassgl->setShiftButtonDown(true);
       break;
    case 0x01000007: // Delete
       this->clearButtonPressed();
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
       glassgl->setShiftButtonDown(false);
       break;
    default:
       break;
    }
}

