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

void MainWindow::userModeChanged(int mode){
    statusBar->showMessage("Switching User Mode", 2000);
    switch(mode)
    {
    case LOOK_MODE:
        modeLabel->setText("LOOK MODE");
        break;
    case TWIST_MODE:
        modeLabel->setText("TWIST MODE");
        break;
    case STRETCH_MODE:
        modeLabel->setText("STRETCH MODE");
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

        stch->subcaneCount = 1;
        stch->subcanes[0] = c;
        stch->amts[0] = 12.0;

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

void MainWindow::twistButtonPressed()
{
        glassgl->setMode(TWIST_MODE);

        statusBar->showMessage("Entered Twist Mode", 2000);
}

void MainWindow::stretchButtonPressed()
{
        glassgl->setMode(STRETCH_MODE);

        statusBar->showMessage("Entered Stretch Mode", 2000);
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
        stch->amts[0] = 12.0;

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

        QPushButton* twist_button = new QPushButton("Twist");
        connect(twist_button, SIGNAL(pressed()), this, SLOT(twistButtonPressed()));
        twist_button->setToolTip("Drag Mouse Horizontally");

        QPushButton* stretch_button = new QPushButton("Stretch");
        connect(stretch_button, SIGNAL(pressed()), this, SLOT(stretchButtonPressed()));
        stretch_button->setToolTip("Drag Mouse Vertically");

        QPushButton* bundle_button = new QPushButton("Bundle");
        connect(bundle_button, SIGNAL(pressed()), this, SLOT(bundleButtonPressed())); 

        QPushButton* flatten_button = new QPushButton("Flatten");
        connect(flatten_button, SIGNAL(pressed()), this, SLOT(flattenButtonPressed()));
        flatten_button->setToolTip("Drag Mouse Horizontally to Squish, Vertically to Flatten");

        QVBoxLayout* operButton_layout = new QVBoxLayout();
        operButton_layout->addWidget(twist_button);
        operButton_layout->addWidget(stretch_button);
        operButton_layout->addWidget(bundle_button);
        operButton_layout->addWidget(flatten_button);
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

        QVBoxLayout* utilButton_layout = new QVBoxLayout();
        utilButton_layout->addWidget(next_button);
        utilButton_layout->addWidget(save_button);
        utilButton_layout->addWidget(clear_button);
        utilButton_layout->addWidget(exportLibrary_button);
        utilButton_layout->addWidget(importLibrary_button);
        utilButton_layout->addWidget(colorPicker_button);
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
        if (e->key() == 0x58) // X 
                exit(0);
        if (e->key() == 0x41) // A 
                this->zoomOutButtonPressed();
        if (e->key() == 0x53) // S 
                this->zoomInButtonPressed();
        if (e->key() == 0x31) // 1 
                this->lookButtonPressed();
        if (e->key() == 0x32) // 2 
                this->twistButtonPressed();
        if (e->key() == 0x33) // 3 
                this->stretchButtonPressed();
        if (e->key() == 0x34) // 4 
                this->bundleButtonPressed();
        if (e->key() == 0x35) // 5
                this->flattenButtonPressed();
        if (e->key() == 0x59) // Y 
                saveButtonPressed();
        if (e->key() == 0x4e) // N 
                nextButtonPressed();
                //glassgl->advanceActiveSubcane();
        if (e->key() == 0x01000007) // Delete 
                this->clearButtonPressed();
}


