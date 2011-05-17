#include "workwindow.h"

WorkWindow::WorkWindow()
{
        centralWidget = new QWidget(this);
        this->setCentralWidget(centralWidget);
        librarySize = 0;

        window_layout = new QVBoxLayout(centralWidget);
        setupWorkArea();
        setupLibraryArea();

        setWindowTitle(tr("Chicane"));
        resize(1000, 1000);
}

void WorkWindow::saveCaneToLibrary()
{
        LibraryCaneWidget* lc = new LibraryCaneWidget((OpenGLWidget*) this->glassgl, 
                this->glassgl->getCane()->deep_copy(), 0);
        stock_layout->addWidget(lc);
}

void WorkWindow::setupLibraryArea()
{
        QWidget* stock_widget = new QWidget;
        stock_layout = new QHBoxLayout(stock_widget);
        stock_layout->setSpacing(10);

        libraryScrollArea = new QScrollArea;
        libraryScrollArea->setBackgroundRole(QPalette::Dark);
        libraryScrollArea->setWidget(stock_widget);
        libraryScrollArea->setWidgetResizable(true);
        libraryScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        libraryScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        libraryScrollArea->setFixedHeight(130);
        
        window_layout->addWidget(libraryScrollArea);
}

void WorkWindow::seedLibrary()
{
        // Now seed the library with some basic canes
        Cane* c = new Cane();
        Cane* stch = new Cane();
        stch->num_subcanes = 1;
        stch->subcanes[0] = c;
        stch->stretch = 12.0;

        c->color.r = 0.8;
        c->color.g = 0.8;
        c->color.b = 0.8;
        glassgl->setFocusCane(stch);
        saveCaneToLibrary();
        c->color.r = 1.0;
        c->color.g = 0.5;
        c->color.b = 0.5;
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

        //glassgl->setFocusCane(make_wiki_cane());        
        //saveCaneToLibrary();
}

void WorkWindow::zoomInButtonPressed()
{
        glassgl->zoomIn();
}

void WorkWindow::zoomOutButtonPressed()
{
        glassgl->zoomOut();
}

void WorkWindow::lookButtonPressed()
{
        glassgl->setMode(LOOK_MODE);
}

void WorkWindow::twistButtonPressed()
{
        glassgl->setMode(TWIST_MODE);
}

void WorkWindow::stretchButtonPressed()
{
        glassgl->setMode(STRETCH_MODE);
}

void WorkWindow::bundleButtonPressed()
{
        glassgl->setMode(MOVE_MODE);
}

void WorkWindow::nextButtonPressed()
{
        glassgl->advanceActiveSubcane();
}

void WorkWindow::saveButtonPressed()
{
        saveCaneToLibrary();
}

void WorkWindow::clearButtonPressed()
{
        glassgl->zeroCanes();
}

void WorkWindow::setupWorkArea()
{
        glassgl = new OpenGLWidget(this);

        QPushButton* look_button = new QPushButton("Look");
        connect(look_button, SIGNAL(clicked()), this, SLOT(lookButtonPressed()));

        QPushButton* zoom_in_button = new QPushButton("Zoom In");
        connect(zoom_in_button, SIGNAL(pressed()), this, SLOT(zoomInButtonPressed()));

        QPushButton* zoom_out_button = new QPushButton("Zoom Out");
        connect(zoom_out_button, SIGNAL(pressed()), this, SLOT(zoomOutButtonPressed()));

        QPushButton* twist_button = new QPushButton("Twist");
        connect(twist_button, SIGNAL(pressed()), this, SLOT(twistButtonPressed()));

        QPushButton* stretch_button = new QPushButton("Stretch");
        connect(stretch_button, SIGNAL(pressed()), this, SLOT(stretchButtonPressed()));

        QPushButton* bundle_button = new QPushButton("Bundle");
        connect(bundle_button, SIGNAL(pressed()), this, SLOT(bundleButtonPressed())); 

        QPushButton* next_button = new QPushButton("Next");
        connect(next_button, SIGNAL(pressed()), this, SLOT(nextButtonPressed())); 

        QPushButton* save_button = new QPushButton("Save");
        connect(save_button, SIGNAL(pressed()), this, SLOT(saveButtonPressed())); 

        QPushButton* clear_button = new QPushButton("Clear");
        connect(clear_button, SIGNAL(pressed()), this, SLOT(clearButtonPressed())); 

        QVBoxLayout* button_layout = new QVBoxLayout();
        button_layout->addWidget(look_button);
        button_layout->addWidget(zoom_in_button);
        button_layout->addWidget(zoom_out_button);
        button_layout->addWidget(twist_button);
        button_layout->addWidget(stretch_button);
        button_layout->addWidget(bundle_button);
        button_layout->addWidget(next_button);
        button_layout->addWidget(save_button);
        button_layout->addWidget(clear_button);

        QHBoxLayout* work_layout = new QHBoxLayout();
        work_layout->addLayout(button_layout);
        work_layout->addWidget(glassgl, 1);
        window_layout->addLayout(work_layout, 5);
}

void WorkWindow::keyPressEvent(QKeyEvent* e)
{
        if (e->key() == 0x58) // X 
                exit(0);
        if (e->key() == 0x41) // A 
                glassgl->zoomOut();
        if (e->key() == 0x53) // S 
                glassgl->zoomIn();
        if (e->key() == 0x31) // 1 
                glassgl->setMode(1);
        if (e->key() == 0x32) // 2 
                glassgl->setMode(2);
        if (e->key() == 0x33) // 3 
                glassgl->setMode(3);
        if (e->key() == 0x34) // 4 
                glassgl->setMode(4);
        if (e->key() == 0x59) // Y 
                saveCaneToLibrary();
        if (e->key() == 0x4e) // N 
                glassgl->advanceActiveSubcane();
        if (e->key() == 0x01000007) // Delete 
                glassgl->zeroCanes();
}


