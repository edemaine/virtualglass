#include "mainwindow.h"

MainWindow::MainWindow()
{
        centralWidget = new QWidget(this);
        this->setCentralWidget(centralWidget);
        librarySize = 0;

        windowLayout = new QVBoxLayout(centralWidget);
        setupWorkArea();
        setupLibraryArea();

        setWindowTitle(tr("Virtual Glass"));
        resize(1000, 1000);
}

void MainWindow::saveCaneToLibrary()
{
        LibraryCaneWidget* lc = new LibraryCaneWidget((OpenGLWidget*) this->glassgl,
                this->glassgl->getCane()->deepCopy(), 0);

        stockLayout->addWidget(lc);
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
}

void MainWindow::zoomInButtonPressed()
{
        glassgl->zoomIn();
}

void MainWindow::zoomOutButtonPressed()
{
        glassgl->zoomOut();
}

void MainWindow::toggleAxesButtonPressed()
{
        glassgl->toggleAxes();
}

void MainWindow::lookButtonPressed()
{
        glassgl->setMode(LOOK_MODE);
}

void MainWindow::topViewButtonPressed()
{
        glassgl->setMode(LOOK_MODE);
        glassgl->setCamera(0.0,0.01);
}

void MainWindow::sideViewButtonPressed()
{
        glassgl->setMode(LOOK_MODE);
        glassgl->setCamera(PI/2,PI/2);
}

void MainWindow::twistButtonPressed()
{
        glassgl->setMode(TWIST_MODE);
}

void MainWindow::stretchButtonPressed()
{
        glassgl->setMode(STRETCH_MODE);
}

void MainWindow::bundleButtonPressed()
{
        glassgl->setMode(BUNDLE_MODE);
}

void MainWindow::nextButtonPressed()
{
        glassgl->advanceActiveSubcane();
}

void MainWindow::flattenButtonPressed()
{
        glassgl->setMode(FLATTEN_MODE);
}

void MainWindow::saveButtonPressed()
{
        saveCaneToLibrary();
}

void MainWindow::clearButtonPressed()
{
        glassgl->zeroCanes();
}

void MainWindow::exportLibraryButtonPressed()
{

}

void MainWindow::importLibraryButtonPressed()
{

}

void MainWindow::newColorPickerCaneButtonPressed()
{
    colorPickerSelected(QColorDialog::getColor());
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

        QPushButton* twist_button = new QPushButton("Twist");
        connect(twist_button, SIGNAL(pressed()), this, SLOT(twistButtonPressed()));

        QPushButton* stretch_button = new QPushButton("Stretch");
        connect(stretch_button, SIGNAL(pressed()), this, SLOT(stretchButtonPressed()));

        QPushButton* bundle_button = new QPushButton("Bundle");
        connect(bundle_button, SIGNAL(pressed()), this, SLOT(bundleButtonPressed())); 

        QPushButton* next_button = new QPushButton("Next");
        connect(next_button, SIGNAL(pressed()), this, SLOT(nextButtonPressed())); 

        QPushButton* flatten_button = new QPushButton("Flatten");
        connect(flatten_button, SIGNAL(pressed()), this, SLOT(flattenButtonPressed())); 

        QPushButton* save_button = new QPushButton("Save");
        connect(save_button, SIGNAL(pressed()), this, SLOT(saveButtonPressed())); 

        QPushButton* clear_button = new QPushButton("Clear");
        connect(clear_button, SIGNAL(pressed()), this, SLOT(clearButtonPressed())); 

        QPushButton* exportLibrary_button = new QPushButton("Export Library");
        connect(exportLibrary_button, SIGNAL(pressed()), this, SLOT(exportLibraryButtonPressed()));

        QPushButton* importLibrary_button = new QPushButton("Import Library");
        connect(importLibrary_button, SIGNAL(pressed()), this, SLOT(importLibraryButtonPressed()));

        QPushButton* colorPicker_button = new QPushButton("New Cane Color");
        connect(colorPicker_button, SIGNAL(pressed()), this, SLOT(newColorPickerCaneButtonPressed()));

        QColorDialog* colorPickerDialog = new QColorDialog();

        QVBoxLayout* button_layout = new QVBoxLayout();
        button_layout->addWidget(look_button);
        button_layout->addWidget(topView_button);
        button_layout->addWidget(sideView_button);
        button_layout->addWidget(zoom_in_button);
        button_layout->addWidget(zoom_out_button);
        button_layout->addWidget(toggle_axes_button);
        button_layout->addWidget(twist_button);
        button_layout->addWidget(stretch_button);
        button_layout->addWidget(bundle_button);
        button_layout->addWidget(next_button);
        button_layout->addWidget(flatten_button);
        button_layout->addWidget(save_button);
        button_layout->addWidget(clear_button);
        button_layout->addWidget(exportLibrary_button);
        button_layout->addWidget(importLibrary_button);
        button_layout->addWidget(colorPicker_button);

        QHBoxLayout* workLayout = new QHBoxLayout();
        workLayout->addLayout(button_layout);
        workLayout->addWidget(glassgl, 1);
        windowLayout->addLayout(workLayout, 5);
}

void MainWindow::keyPressEvent(QKeyEvent* e)
{
        if (e->key() == 0x58) // X 
                exit(0);
        if (e->key() == 0x41) // A 
                glassgl->zoomOut();
        if (e->key() == 0x53) // S 
                glassgl->zoomIn();
        if (e->key() == 0x31) // 1 
                glassgl->setMode(LOOK_MODE);
        if (e->key() == 0x32) // 2 
                glassgl->setMode(TWIST_MODE);
        if (e->key() == 0x33) // 3 
                glassgl->setMode(STRETCH_MODE);
        if (e->key() == 0x34) // 4 
                glassgl->setMode(BUNDLE_MODE);
        if (e->key() == 0x35) // 5
                glassgl->setMode(FLATTEN_MODE);
        if (e->key() == 0x59) // Y 
                saveCaneToLibrary();
        if (e->key() == 0x4e) // N 
                glassgl->advanceActiveSubcane();
        if (e->key() == 0x01000007) // Delete 
                glassgl->zeroCanes();
}


