#include <QtGui>
#include <map>
#include <json/json.h>
#include <string>
#include <sstream>
#include <fstream>
#include <QTextStream>
#include <iostream.h>

#include "constants.h"
#include "dependancy.h"
#include "niceviewwidget.h"
#include "asyncpiecelibrarywidget.h"
#include "asyncpullplanlibrarywidget.h"
#include "asynccolorbarlibrarywidget.h"
#include "pullplan.h"
#include "pickupplan.h"
#include "piece.h"
#include "pullplaneditorwidget.h"
#include "coloreditorwidget.h"
#include "pieceeditorwidget.h"
#include "randomglass.h"
#include "glassmime.h"
#include "exampleglass.h"
#include "pulltemplate.h"
#include "mainwindow.h"
#include "globalglass.h"

MainWindow :: MainWindow()
{
    setWindowTitle(windowTitle());
    centralWidget = new QWidget(this);
    this->setCentralWidget(centralWidget);

    centralLayout = new QHBoxLayout(centralWidget);
    setupLibrary();
    setupEditors();
    setupMenus();
    setupConnections();

    seedEverything();
    setViewMode(EMPTY_VIEW_MODE);
    show();

    emit someDataChanged();
    whatToDoLabel->setText("Click a library item at left to edit/view.");

    createMenus();
}

void MainWindow :: setViewMode(enum ViewMode _mode)
{
    editorStack->setCurrentIndex(_mode);
    copyColorBarButton->setEnabled(false);
    copyPullPlanButton->setEnabled(false);
    copyPieceButton->setEnabled(false);
    switch (_mode)
    {
        case EMPTY_VIEW_MODE:
            // leave all copy buttons disabled
            break;
        case COLORBAR_VIEW_MODE:
            copyColorBarButton->setEnabled(true);
            break;
        case PULLPLAN_VIEW_MODE:
            copyPullPlanButton->setEnabled(true);
            break;
        case PIECE_VIEW_MODE:
            copyPieceButton->setEnabled(true);
            break;
    }
    emit someDataChanged();
}

QString MainWindow :: windowTitle()
{
    QString title = tr("VirtualGlass");
    QFile inFile(":/version.txt");
    if (inFile.open(QIODevice::ReadOnly)) {
        QTextStream in(&inFile);
        QString revision = in.readLine();
        QString date = in.readLine();
        title += " - r" + revision + " built on " + date;
    }
    return title;
}

void MainWindow :: seedEverything()
{
    // Load color stuff
    setViewMode(COLORBAR_VIEW_MODE);
    emit someDataChanged();

    // Load pull template types
    setViewMode(PULLPLAN_VIEW_MODE);
    pullPlanEditorWidget->seedTemplates();

    // Load pickup and piece template types
    setViewMode(PIECE_VIEW_MODE);
    pieceEditorWidget->seedTemplates();
}

void MainWindow :: unhighlightAllLibraryWidgets()
{
    QLayoutItem* w;
    for (int j = 0; j < colorBarLibraryLayout->count(); ++j)
    {
        w = colorBarLibraryLayout->itemAt(j);
        static_cast<AsyncColorBarLibraryWidget*>(w->widget())->setDependancy(false);
    }
    for (int j = 0; j < pullPlanLibraryLayout->count(); ++j)
    {
        w = pullPlanLibraryLayout->itemAt(j);
        static_cast<AsyncPullPlanLibraryWidget*>(w->widget())->setDependancy(false);
    }
    for (int j = 0; j < pieceLibraryLayout->count(); ++j)
    {
        w = pieceLibraryLayout->itemAt(j);
        static_cast<AsyncPieceLibraryWidget*>(w->widget())->setDependancy(false);
    }
}

void MainWindow :: keyPressEvent(QKeyEvent* e)
{
    if (e->key() == Qt::Key_Backspace || e->key() == Qt::Key_Delete)
        deleteCurrentEditingObject();
}

void MainWindow :: deleteCurrentEditingObject()
{
    QLayoutItem* w;

    switch (editorStack->currentIndex())
    {
        case COLORBAR_VIEW_MODE:
        {
            if (colorBarLibraryLayout->count() == 1)
                return;

            int i;
            for (i = 0; i < colorBarLibraryLayout->count(); ++i)
            {
                w = colorBarLibraryLayout->itemAt(i);
                GlassColor* gc = dynamic_cast<AsyncColorBarLibraryWidget*>(w->widget())->getGlassColor();
                if (gc == colorEditorWidget->getGlassColor())
                {
                    if (glassColorIsDependancy(gc))
                    {
                        QMessageBox msgBox;
                        msgBox.setText("This color cannot be deleted: other objects use it.");
                        msgBox.exec();
                    }
                    else
                    {
                        // this may be a memory leak, the library widget is never explicitly deleted
                        w = colorBarLibraryLayout->takeAt(i);
                        delete w->widget();
                        delete w;
                    }
                    break;
                }
            }

            colorEditorWidget->setGlassColor(dynamic_cast<AsyncColorBarLibraryWidget*>(colorBarLibraryLayout->itemAt(
                    MIN(colorBarLibraryLayout->count()-1, i))->widget())->getGlassColor());
            emit someDataChanged();
            break;
        }
        case PULLPLAN_VIEW_MODE:
        {
            if (pullPlanLibraryLayout->count() == 1)
                return;

            int i;
            for (i = 0; i < pullPlanLibraryLayout->count(); ++i)
            {
                w = pullPlanLibraryLayout->itemAt(i);
                PullPlan* p = dynamic_cast<AsyncPullPlanLibraryWidget*>(w->widget())->getPullPlan();
                if (p == pullPlanEditorWidget->getPlan())
                {
                    if (pullPlanIsDependancy(p))
                    {
                        QMessageBox msgBox;
                        msgBox.setText("This cane cannot be deleted: other objects use it.");
                        msgBox.exec();
                    }
                    else
                    {
                        // this may be a memory leak, the library widget is never explicitly deleted
                        w = pullPlanLibraryLayout->takeAt(i);
                        delete w->widget();
                        delete w;
                    }
                    break;
                }
            }

            pullPlanEditorWidget->setPlan(dynamic_cast<AsyncPullPlanLibraryWidget*>(pullPlanLibraryLayout->itemAt(
                    MIN(pullPlanLibraryLayout->count()-1, i))->widget())->getPullPlan());
            emit someDataChanged();
            break;
        }
        case PIECE_VIEW_MODE:
        {
            if (pieceLibraryLayout->count() == 1)
                return;

            int i;
            for (i = 0; i < pieceLibraryLayout->count(); ++i)
            {
                w = pieceLibraryLayout->itemAt(i);
                Piece* p = dynamic_cast<AsyncPieceLibraryWidget*>(w->widget())->getPiece();
                if (p == pieceEditorWidget->getPiece())
                {
                    // this may be a memory leak, the library widget is never explicitly deleted
                    w = pieceLibraryLayout->takeAt(i);
                    delete w->widget();
                    delete w;
                    break;
                }
            }

            pieceEditorWidget->setPiece(dynamic_cast<AsyncPieceLibraryWidget*>(pieceLibraryLayout->itemAt(
                    MIN(pieceLibraryLayout->count()-1, i))->widget())->getPiece());
            emit someDataChanged();
            break;
        }
    }

}

void MainWindow :: mouseReleaseEvent(QMouseEvent* event)
{
    // If this is a drag and not the end of a click, don't process (dropEvent will do it instead)
    if (isDragging && (event->pos() - dragStartPosition).manhattanLength() > QApplication::startDragDistance()) return;

    AsyncColorBarLibraryWidget* cblw = dynamic_cast<AsyncColorBarLibraryWidget*>(childAt(event->pos()));
    AsyncPullPlanLibraryWidget* plplw = dynamic_cast<AsyncPullPlanLibraryWidget*>(childAt(event->pos()));
    AsyncPieceLibraryWidget* plw = dynamic_cast<AsyncPieceLibraryWidget*>(childAt(event->pos()));

    if (cblw != NULL)
    {
        colorEditorWidget->setGlassColor(cblw->getGlassColor());
        setViewMode(COLORBAR_VIEW_MODE);
    }
    else if (plplw != NULL)
    {
        pullPlanEditorWidget->setPlan(plplw->getPullPlan());
        setViewMode(PULLPLAN_VIEW_MODE);
    }
    else if (plw != NULL)
    {
        pieceEditorWidget->setPiece(plw->getPiece());
        setViewMode(PIECE_VIEW_MODE);
    }
}


void MainWindow :: mousePressEvent(QMouseEvent* event)
{
    AsyncColorBarLibraryWidget* cblw = dynamic_cast<AsyncColorBarLibraryWidget*>(childAt(event->pos()));
    AsyncPullPlanLibraryWidget* plplw = dynamic_cast<AsyncPullPlanLibraryWidget*>(childAt(event->pos()));

    if (event->button() == Qt::LeftButton && (cblw != NULL || plplw != NULL))
    {
        isDragging = true;
        this->dragStartPosition = event->pos();
    }
    else
        isDragging = false;
}

void MainWindow :: mouseMoveEvent(QMouseEvent* event)
{
    // If the left mouse button isn't down
    if ((event->buttons() & Qt::LeftButton) == 0)
    {
        isDragging = false;
        return;
    }

    if (!isDragging || (event->pos() - dragStartPosition).manhattanLength() < QApplication::startDragDistance())
        return;

    AsyncColorBarLibraryWidget* cblw = dynamic_cast<AsyncColorBarLibraryWidget*>(childAt(event->pos()));
    AsyncPullPlanLibraryWidget* plplw = dynamic_cast<AsyncPullPlanLibraryWidget*>(childAt(event->pos()));

    char buf[500];
    QPixmap pixmap;

    if (cblw != NULL)
    {
        GlassMime::encode(buf, cblw->getGlassColor()->getColor(), GlassMime::colorbar);
        pixmap = *cblw->getDragPixmap();
    }
    else if (plplw != NULL)
    {
        GlassMime::encode(buf, plplw->getPullPlan(), GlassMime::pullplan);
        pixmap = *plplw->getDragPixmap();
    }
    else
        return;

    QByteArray pointerData(buf);
    QMimeData* mimeData = new QMimeData;
    mimeData->setText(pointerData);

    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);
    drag->setPixmap(pixmap);

    drag->exec(Qt::CopyAction);
}

void MainWindow :: setupConnections()
{
    connect(this, SIGNAL(someDataChanged()), this, SLOT(updateEverything()));

    connect(newColorBarButton, SIGNAL(pressed()), this, SLOT(newColorBar()));
    connect(copyColorBarButton, SIGNAL(pressed()), this, SLOT(copyColorBar()));
    connect(colorEditorWidget, SIGNAL(someDataChanged()), this, SLOT(updateEverything()));

    connect(newPullPlanButton, SIGNAL(pressed()), this, SLOT(newPullPlan()));
    connect(copyPullPlanButton, SIGNAL(pressed()), this, SLOT(copyPullPlan()));
    connect(pullPlanEditorWidget, SIGNAL(someDataChanged()), this, SLOT(updateEverything()));

    connect(newPieceButton, SIGNAL(pressed()), this, SLOT(newPiece()));
    connect(copyPieceButton, SIGNAL(pressed()), this, SLOT(copyPiece()));
    connect(pieceEditorWidget, SIGNAL(someDataChanged()), this, SLOT(updateEverything()));

    //connect(openAction, SIGNAL(triggered()), this, SLOT(open()));
    //connect(saveAction, SIGNAL(triggered()), this, SLOT(save()));
    //connect(saveAsAction, SIGNAL(triggered()), this, SLOT(saveAs()));

    connect(randomSimpleCaneAction, SIGNAL(triggered()), this, SLOT(randomSimpleCaneExampleActionTriggered()));
    connect(randomSimplePieceAction, SIGNAL(triggered()), this, SLOT(randomSimplePieceExampleActionTriggered()));

    connect(randomComplexCaneAction, SIGNAL(triggered()), this, SLOT(randomComplexCaneExampleActionTriggered()));
    connect(randomComplexPieceAction, SIGNAL(triggered()), this, SLOT(randomComplexPieceExampleActionTriggered()));

    connect(web1PieceAction, SIGNAL(triggered()), this, SLOT(web1PieceExampleActionTriggered()));
    connect(web2PieceAction, SIGNAL(triggered()), this, SLOT(web2PieceExampleActionTriggered()));

    connect(depthPeelAction, SIGNAL(triggered()), this, SLOT(depthPeelActionTriggered()));
}

void MainWindow :: depthPeelActionTriggered()
{
    NiceViewWidget::peelEnable = !(NiceViewWidget::peelEnable);
    depthPeelAction->setChecked(NiceViewWidget::peelEnable);
    emit someDataChanged();
}

void MainWindow :: web1PieceExampleActionTriggered()
{
    GlassColor* gc;
    PullPlan *pp;
    Piece* p;

    web1Piece(&gc, &pp, &p);

    colorBarLibraryLayout->addWidget(new AsyncColorBarLibraryWidget(gc));
    pullPlanLibraryLayout->addWidget(new AsyncPullPlanLibraryWidget(pp));
    pieceLibraryLayout->addWidget(new AsyncPieceLibraryWidget(p));

    pieceEditorWidget->setPiece(p);
    setViewMode(PIECE_VIEW_MODE);
}

void MainWindow :: web2PieceExampleActionTriggered()
{
    GlassColor* gc;
    PullPlan *pp1, *pp2;
    Piece* p;

    web2Piece(&gc, &pp1, &pp2, &p);

    colorBarLibraryLayout->addWidget(new AsyncColorBarLibraryWidget(gc));
    pullPlanLibraryLayout->addWidget(new AsyncPullPlanLibraryWidget(pp1));
    pullPlanLibraryLayout->addWidget(new AsyncPullPlanLibraryWidget(pp2));
    pieceLibraryLayout->addWidget(new AsyncPieceLibraryWidget(p));

    pieceEditorWidget->setPiece(p);
    setViewMode(PIECE_VIEW_MODE);
}

void MainWindow :: randomSimpleCaneExampleActionTriggered()
{
    GlassColor* randomGC = randomGlassColor();
    PullPlan* randomPP = randomSimplePullPlan(CIRCLE_SHAPE, randomGC);

    colorBarLibraryLayout->addWidget(new AsyncColorBarLibraryWidget(randomGC));

    AsyncPullPlanLibraryWidget* pplw = new AsyncPullPlanLibraryWidget(randomPP);
    pullPlanLibraryLayout->addWidget(pplw);

    pullPlanEditorWidget->setPlan(randomPP);
    setViewMode(PULLPLAN_VIEW_MODE);
}

void MainWindow :: randomComplexCaneExampleActionTriggered()
{
    GlassColor* randomGC = randomGlassColor();
    PullPlan* randomCPP = randomSimplePullPlan(CIRCLE_SHAPE, randomGC);
    PullPlan* randomSPP = randomSimplePullPlan(SQUARE_SHAPE, randomGC);
    PullPlan* randomComplexPP = randomComplexPullPlan(randomCPP, randomSPP);

    colorBarLibraryLayout->addWidget(new AsyncColorBarLibraryWidget(randomGC));
    pullPlanLibraryLayout->addWidget(new AsyncPullPlanLibraryWidget(randomComplexPP));
    // add simple plans only if they are used
    // memory leak! as unused ones never appear in library
    if (randomComplexPP->hasDependencyOn(randomCPP))
        pullPlanLibraryLayout->addWidget(new AsyncPullPlanLibraryWidget(randomCPP));
    if (randomComplexPP->hasDependencyOn(randomSPP))
        pullPlanLibraryLayout->addWidget(new AsyncPullPlanLibraryWidget(randomSPP));

    pullPlanEditorWidget->setPlan(randomComplexPP);
    setViewMode(PULLPLAN_VIEW_MODE);
}

void MainWindow :: randomSimplePieceExampleActionTriggered()
{
    GlassColor* randomGC = randomGlassColor();
    PullPlan* randomSPP = randomSimplePullPlan(SQUARE_SHAPE, randomGC);
    Piece* randomP = randomPiece(randomPickup(randomSPP));

    colorBarLibraryLayout->addWidget(new AsyncColorBarLibraryWidget(randomGC));
    pullPlanLibraryLayout->addWidget(new AsyncPullPlanLibraryWidget(randomSPP));

    AsyncPieceLibraryWidget* plw = new AsyncPieceLibraryWidget(randomP);
    pieceLibraryLayout->addWidget(plw);

    pieceEditorWidget->setPiece(randomP);
    setViewMode(PIECE_VIEW_MODE);
}

void MainWindow :: randomComplexPieceExampleActionTriggered()
{
    GlassColor* randomGC1 = randomGlassColor();
    GlassColor* randomGC2 = randomGlassColor();
        PullPlan* randomCPP = randomSimplePullPlan(CIRCLE_SHAPE, randomGC1);
        PullPlan* randomSPP = randomSimplePullPlan(SQUARE_SHAPE, randomGC2);
        PullPlan* randomComplexPP1 = randomComplexPullPlan(randomCPP, randomSPP);
        PullPlan* randomComplexPP2 = randomComplexPullPlan(randomCPP, randomSPP);
    Piece* randomP = randomPiece(randomPickup(randomComplexPP1, randomComplexPP2));

    if (randomP->hasDependencyOn(randomGC1)) // memory leak if returns no
        colorBarLibraryLayout->addWidget(new AsyncColorBarLibraryWidget(randomGC1));
    if (randomP->hasDependencyOn(randomGC2)) // memory leak if returns no
        colorBarLibraryLayout->addWidget(new AsyncColorBarLibraryWidget(randomGC2));
    if (randomP->hasDependencyOn(randomCPP)) // memory leak if returns no
        pullPlanLibraryLayout->addWidget(new AsyncPullPlanLibraryWidget(randomCPP));
    if (randomP->hasDependencyOn(randomSPP)) // memory leak if returns no
        pullPlanLibraryLayout->addWidget(new AsyncPullPlanLibraryWidget(randomSPP));
    pullPlanLibraryLayout->addWidget(new AsyncPullPlanLibraryWidget(randomComplexPP1));
    pullPlanLibraryLayout->addWidget(new AsyncPullPlanLibraryWidget(randomComplexPP2));
    pieceLibraryLayout->addWidget(new AsyncPieceLibraryWidget(randomP));

    pieceEditorWidget->setPiece(randomP);
    setViewMode(PIECE_VIEW_MODE);
}

void MainWindow :: setupLibrary()
{
    QWidget* bigOleLibraryWidget = new QWidget(centralWidget);
    centralLayout->addWidget(bigOleLibraryWidget);

    QVBoxLayout* libraryAreaLayout = new QVBoxLayout(bigOleLibraryWidget);
    bigOleLibraryWidget->setLayout(libraryAreaLayout);

    QScrollArea* libraryScrollArea = new QScrollArea(bigOleLibraryWidget);
    libraryAreaLayout->addWidget(libraryScrollArea, 1);
    libraryScrollArea->setBackgroundRole(QPalette::Dark);
    libraryScrollArea->setWidgetResizable(true);
    libraryScrollArea->setFixedWidth(370);
    libraryScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    libraryScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    QWidget* libraryWidget = new QWidget(libraryScrollArea);
    libraryScrollArea->setWidget(libraryWidget);

    QGridLayout* superlibraryLayout = new QGridLayout(libraryWidget);
    libraryWidget->setLayout(superlibraryLayout);

    newColorBarButton = new QPushButton("New Color", libraryWidget);
    newPullPlanButton = new QPushButton("New Cane", libraryWidget);
    newPieceButton = new QPushButton("New Piece", libraryWidget);
    superlibraryLayout->addWidget(newColorBarButton, 0, 0);
    superlibraryLayout->addWidget(newPullPlanButton, 0, 1);
    superlibraryLayout->addWidget(newPieceButton, 0, 2);

    copyColorBarButton = new QPushButton("Copy Color", libraryWidget);
    copyPullPlanButton = new QPushButton("Copy Cane", libraryWidget);
    copyPieceButton = new QPushButton("Copy Piece", libraryWidget);
    superlibraryLayout->addWidget(copyColorBarButton, 1, 0);
    superlibraryLayout->addWidget(copyPullPlanButton, 1, 1);
    superlibraryLayout->addWidget(copyPieceButton, 1, 2);

    colorBarLibraryLayout = new QVBoxLayout(libraryWidget);
    colorBarLibraryLayout->setDirection(QBoxLayout::BottomToTop);
    pullPlanLibraryLayout = new QVBoxLayout(libraryWidget);
    pullPlanLibraryLayout->setDirection(QBoxLayout::BottomToTop);
    pieceLibraryLayout = new QVBoxLayout(libraryWidget);
    pieceLibraryLayout->setDirection(QBoxLayout::BottomToTop);
    superlibraryLayout->addLayout(colorBarLibraryLayout, 2, 0, Qt::AlignTop);
    superlibraryLayout->addLayout(pullPlanLibraryLayout, 2, 1, Qt::AlignTop);
    superlibraryLayout->addLayout(pieceLibraryLayout, 2, 2, Qt::AlignTop);

    // make three qlabels for a legend
    QGridLayout* legendLayout = new QGridLayout(libraryWidget);
    QLabel* l1 = new QLabel("Used By Selected");
    l1->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    l1->setStyleSheet("border: 2px dashed " + QColor(0, 139, 69, 255).name() + ";");
    QLabel* l2 = new QLabel("Selected");
    l2->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    l2->setStyleSheet("border: 2px solid " + QColor(0, 0, 255, 255).name() + ";");
    QLabel* l3 = new QLabel("Uses Selected");
    l3->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    l3->setStyleSheet("border: 2px dotted " + QColor(200, 100, 0, 255).name() + ";");
    legendLayout->addWidget(l1,0,1);
    legendLayout->addWidget(l2,0,2);
    legendLayout->addWidget(l3,0,3);
    libraryAreaLayout->addLayout(legendLayout, 0);

    QLabel* descriptionLabel = new QLabel("Library - click to edit or drag to add.",
        libraryWidget);
    descriptionLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    libraryAreaLayout->addWidget(descriptionLabel, 0);
}

void MainWindow :: setupEditors()
{
    editorStack = new QStackedWidget(centralWidget);
    centralLayout->addWidget(editorStack, 2);

    // The order that the editors are added to the stacked widget
    // must match their order values in the enum EditorGUI::Mode
    // in mainwindow.h
    setupEmptyPaneEditor();
    editorStack->addWidget(emptyEditorPage);

    setupColorEditor();
    editorStack->addWidget(colorEditorWidget);

    setupPullPlanEditor();
    editorStack->addWidget(pullPlanEditorWidget);

    setupPieceEditor();
    editorStack->addWidget(pieceEditorWidget);
}

void MainWindow :: setupPieceEditor()
{
    // Setup data objects - the current plan and library widget for this plan
    pieceEditorWidget = new PieceEditorWidget(editorStack);
    pieceLibraryLayout->addWidget(new AsyncPieceLibraryWidget(pieceEditorWidget->getPiece()));
}

void MainWindow :: setupEmptyPaneEditor()
{
    emptyEditorPage = new QWidget(editorStack);
    QHBoxLayout* editorLayout = new QHBoxLayout(emptyEditorPage);
    emptyEditorPage->setLayout(editorLayout);
    whatToDoLabel = new QLabel("Click a library item at left to edit/view.", emptyEditorPage);
    whatToDoLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    editorLayout->addWidget(whatToDoLabel, 0);
}

void MainWindow :: setupColorEditor()
{
    // Setup data objects - the current plan and library widget for this plan
    AsyncColorBarLibraryWidget* starterLibraryWidget =
        new AsyncColorBarLibraryWidget(new GlassColor(), this);
    colorEditorWidget = new ColorEditorWidget(starterLibraryWidget->getGlassColor(), editorStack);
    colorBarLibraryLayout->addWidget(starterLibraryWidget);
}

void MainWindow :: setupPullPlanEditor()
{
    // Setup data objects - the current plan and library widget for this plan
    pullPlanEditorWidget = new PullPlanEditorWidget(editorStack);
    pullPlanLibraryLayout->addWidget(new AsyncPullPlanLibraryWidget(pullPlanEditorWidget->getPlan()));
}

void MainWindow :: newPiece()
{
    // Create the new piece
    Piece* newEditorPiece = new Piece(PieceTemplate::TUMBLER);

    // Create the new library entry
    pieceLibraryLayout->addWidget(new AsyncPieceLibraryWidget(newEditorPiece));
    pieceEditorWidget->setPiece(newEditorPiece);

    // Load up the right editor
    setViewMode(PIECE_VIEW_MODE);
}

void MainWindow :: copyPiece()
{
    if (editorStack->currentIndex() != PIECE_VIEW_MODE)
        return;

    // Create the new piece
    Piece* newEditorPiece = pieceEditorWidget->getPiece()->copy();
    pieceLibraryLayout->addWidget(new AsyncPieceLibraryWidget(newEditorPiece));
    pieceEditorWidget->setPiece(newEditorPiece);

    emit someDataChanged();
}

void MainWindow :: newColorBar()
{
    GlassColor* newGlassColor = new GlassColor();

    // Create the new library entry
    colorBarLibraryLayout->addWidget(new AsyncColorBarLibraryWidget(newGlassColor, this));
    colorEditorWidget->setGlassColor(newGlassColor);

    // Load up the right editor
    setViewMode(COLORBAR_VIEW_MODE);
}

void MainWindow :: copyColorBar()
{
    if (editorStack->currentIndex() != COLORBAR_VIEW_MODE)
        return;

    GlassColor* newEditorGlassColor = colorEditorWidget->getGlassColor()->copy();

    // Create the new library entry
    colorBarLibraryLayout->addWidget(new AsyncColorBarLibraryWidget(newEditorGlassColor, this));
    colorEditorWidget->setGlassColor(newEditorGlassColor);

    // Trigger GUI updates
    emit someDataChanged();
}

void MainWindow :: newPullPlan()
{
    PullPlan *newEditorPlan = new PullPlan(PullTemplate::BASE_CIRCLE);
    emit newPullPlan(newEditorPlan);
}

void MainWindow :: copyPullPlan()
{
    if (editorStack->currentIndex() != PULLPLAN_VIEW_MODE)
        return;

    PullPlan *newEditorPlan = pullPlanEditorWidget->getPlan()->copy();
    emit newPullPlan(newEditorPlan);
}

void MainWindow :: newPullPlan(PullPlan* newPlan)
{
    pullPlanLibraryLayout->addWidget(new AsyncPullPlanLibraryWidget(newPlan));

    // Give the new plan to the editor
    pullPlanEditorWidget->setPlan(newPlan);

    // Load up the right editor
    setViewMode(PULLPLAN_VIEW_MODE);
}

void MainWindow :: updateEverything()
{
    switch (editorStack->currentIndex())
    {
        case COLORBAR_VIEW_MODE:
            colorEditorWidget->updateEverything();
            break;
        case PULLPLAN_VIEW_MODE:
            pullPlanEditorWidget->updateEverything();
            break;
        case PIECE_VIEW_MODE:
            pieceEditorWidget->updateEverything();
            break;
    }
        updateLibrary();
}

// returns whether the pull plan is a dependancy of something in the library
// (either a pull plan or a piece)
bool MainWindow :: glassColorIsDependancy(GlassColor* color)
{
    AsyncPullPlanLibraryWidget* pplw;
    for (int i = 0; i < pullPlanLibraryLayout->count(); ++i)
    {
        pplw = dynamic_cast<AsyncPullPlanLibraryWidget*>(
            dynamic_cast<QWidgetItem *>(pullPlanLibraryLayout->itemAt(i))->widget());
        if (pplw->getPullPlan()->hasDependencyOn(color))
        {
            return true;
        }
    }

    AsyncPieceLibraryWidget* plw;
    for (int i = 0; i < pieceLibraryLayout->count(); ++i)
    {
        plw = dynamic_cast<AsyncPieceLibraryWidget*>(
            dynamic_cast<QWidgetItem *>(pieceLibraryLayout->itemAt(i))->widget());
        if (plw->getPiece()->hasDependencyOn(color))
        {
            return true;
        }
    }

    return false;
}

// returns whether the pull plan is a dependancy of something in the library
// (either another pull plan or a piece)
bool MainWindow :: pullPlanIsDependancy(PullPlan* plan)
{
    AsyncPullPlanLibraryWidget* pplw;
    for (int i = 0; i < pullPlanLibraryLayout->count(); ++i)
    {
        pplw = dynamic_cast<AsyncPullPlanLibraryWidget*>(
                dynamic_cast<QWidgetItem *>(pullPlanLibraryLayout->itemAt(i))->widget());
        // Check whether the pull plan in the library is:
        // 1. the parameter plan
        // 2. a plan with the plan currently being edited as a subplan
        if (pplw->getPullPlan() == plan)
        {
            continue;
        }
        else if (pplw->getPullPlan()->hasDependencyOn(plan))
        {
            return true;
        }
    }

    AsyncPieceLibraryWidget* plw;
    for (int i = 0; i < pieceLibraryLayout->count(); ++i)
    {
        plw = dynamic_cast<AsyncPieceLibraryWidget*>(
            dynamic_cast<QWidgetItem *>(pieceLibraryLayout->itemAt(i))->widget());
        if (plw->getPiece()->hasDependencyOn(plan))
        {
            return true;
        }
    }

    return false;
}

void MainWindow :: updateLibrary()
{
    unhighlightAllLibraryWidgets();

    switch (editorStack->currentIndex())
    {
        case COLORBAR_VIEW_MODE:
        {
                        AsyncColorBarLibraryWidget* cblw;
                        for (int i = 0; i < colorBarLibraryLayout->count(); ++i)
                        {
                                cblw = dynamic_cast<AsyncColorBarLibraryWidget*>(
                                        dynamic_cast<QWidgetItem *>(colorBarLibraryLayout->itemAt(i))->widget());
                                if (colorEditorWidget->getGlassColor() == cblw->getGlassColor())
                {
                    cblw->updatePixmaps();
                    cblw->setDependancy(true, IS_DEPENDANCY);
                }
                        }


            AsyncPullPlanLibraryWidget* pplw;
            for (int i = 0; i < pullPlanLibraryLayout->count(); ++i)
            {
                pplw = dynamic_cast<AsyncPullPlanLibraryWidget*>(
                    dynamic_cast<QWidgetItem *>(pullPlanLibraryLayout->itemAt(i))->widget());
                if (pplw->getPullPlan()->hasDependencyOn(colorEditorWidget->getGlassColor()))
                {
                    pplw->updatePixmaps();
                    pplw->setDependancy(true, USES_DEPENDANCY);
                }
            }

            AsyncPieceLibraryWidget* plw;
            for (int i = 0; i < pieceLibraryLayout->count(); ++i)
            {
                plw = dynamic_cast<AsyncPieceLibraryWidget*>(
                    dynamic_cast<QWidgetItem *>(pieceLibraryLayout->itemAt(i))->widget());
                if (plw->getPiece()->hasDependencyOn(colorEditorWidget->getGlassColor()))
                {
                    plw->updatePixmap();
                    plw->setDependancy(true, USES_DEPENDANCY);
                }
            }

            break;
        }
        case PULLPLAN_VIEW_MODE:
        {
            AsyncColorBarLibraryWidget* cblw;
            for (int i = 0; i < colorBarLibraryLayout->count(); ++i)
            {
                cblw = dynamic_cast<AsyncColorBarLibraryWidget*>(
                    dynamic_cast<QWidgetItem *>(colorBarLibraryLayout->itemAt(i))->widget());
                if (pullPlanEditorWidget->getPlan()->hasDependencyOn(cblw->getGlassColor()))
                    cblw->setDependancy(true, USEDBY_DEPENDANCY);
            }

            AsyncPullPlanLibraryWidget* pplw;
            for (int i = 0; i < pullPlanLibraryLayout->count(); ++i)
            {
                pplw = dynamic_cast<AsyncPullPlanLibraryWidget*>(
                        dynamic_cast<QWidgetItem *>(pullPlanLibraryLayout->itemAt(i))->widget());
                // Check whether the pull plan in the library is:
                // 1. the plan currently being edited
                // 2. a subplan of the plan current being edited
                // 3. a plan with the plan currently being edited as a subplan
                if (pullPlanEditorWidget->getPlan() == pplw->getPullPlan())
                {
                    pplw->updatePixmaps();
                    pplw->setDependancy(true, IS_DEPENDANCY);
                }
                else if (pullPlanEditorWidget->getPlan()->hasDependencyOn(pplw->getPullPlan()))
                {
                    pplw->setDependancy(true, USEDBY_DEPENDANCY);
                }
                else if (pplw->getPullPlan()->hasDependencyOn(pullPlanEditorWidget->getPlan()))
                {
                    pplw->updatePixmaps();
                    pplw->setDependancy(true, USES_DEPENDANCY);
                }
            }

            AsyncPieceLibraryWidget* plw;
            for (int i = 0; i < pieceLibraryLayout->count(); ++i)
            {
                plw = dynamic_cast<AsyncPieceLibraryWidget*>(
                    dynamic_cast<QWidgetItem *>(pieceLibraryLayout->itemAt(i))->widget());
                if (plw->getPiece()->hasDependencyOn(pullPlanEditorWidget->getPlan()))
                {
                    plw->updatePixmap();
                    plw->setDependancy(true, USES_DEPENDANCY);
                }
            }

            break;
        }
        case PIECE_VIEW_MODE:
        {
            AsyncColorBarLibraryWidget* cblw;
            for (int i = 0; i < colorBarLibraryLayout->count(); ++i)
            {
                cblw = dynamic_cast<AsyncColorBarLibraryWidget*>(
                    dynamic_cast<QWidgetItem *>(colorBarLibraryLayout->itemAt(i))->widget());
                if (pieceEditorWidget->getPiece()->hasDependencyOn(cblw->getGlassColor()))
                    cblw->setDependancy(true, USEDBY_DEPENDANCY);
            }

            AsyncPullPlanLibraryWidget* pplw;
            for (int i = 0; i < pullPlanLibraryLayout->count(); ++i)
            {
                pplw = dynamic_cast<AsyncPullPlanLibraryWidget*>(
                    dynamic_cast<QWidgetItem*>(pullPlanLibraryLayout->itemAt(i))->widget());
                if (pieceEditorWidget->getPiece()->hasDependencyOn(pplw->getPullPlan()))
                    pplw->setDependancy(true, USEDBY_DEPENDANCY);
            }

                        AsyncPieceLibraryWidget* plw;
                        for (int i = 0; i < pieceLibraryLayout->count(); ++i)
                        {
                                plw = dynamic_cast<AsyncPieceLibraryWidget*>(
                                        dynamic_cast<QWidgetItem *>(pieceLibraryLayout->itemAt(i))->widget());
                                if (plw->getPiece() == pieceEditorWidget->getPiece())
                                {
                                        plw->updatePixmap();
                    plw->setDependancy(true, IS_DEPENDANCY);
                                }
                        }

            break;
        }
    }
}


void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);
    menu.exec(event->globalPos());
}

void MainWindow::buildCaneTree(PullPlan* plan, vector<PullPlan*>* caneVec, vector<GlassColor*>* colorVec){
    if(std::find((*caneVec).begin(), (*caneVec).end(),plan) != (*caneVec).end()){
    }
    else{
        if(int (plan->subs.size())==0){
            for(int i =0; i<int (plan->getCasingCount());i++){
                if (std::find(colorVec->begin(), colorVec->end(), plan->getCasingColor(i)) != colorVec->end());
                else colorVec->push_back(plan->getCasingColor(i));
            }
            if (std::find(caneVec->begin(), caneVec->end(), plan) != caneVec->end());
            else(*caneVec).push_back(plan);
        }
        else{
            for(int i =0; i<int (plan->getCasingCount());i++){
                if (std::find(colorVec->begin(), colorVec->end(), plan->getCasingColor(i)) != colorVec->end());
                else colorVec->push_back(plan->getCasingColor(i));
            }
            for(int i=0; i<int(plan->subs.size()); i++){
                SubpullTemplate subplan = (*plan).subs.at(i);
                //go down and build tree
                buildCaneTree(subplan.plan, caneVec, colorVec);
                if (std::find(caneVec->begin(), caneVec->end(), subplan.plan) != caneVec->end());
                else(*caneVec).push_back(subplan.plan);
            }
        }
    }
}

void MainWindow::buildCaneMap(vector<PullPlan*>* caneVec, vector<GlassColor*>* colorVec){
    AsyncPieceLibraryWidget *plw;
    for (int j = 0; j < pieceLibraryLayout->count(); ++j){
        plw = dynamic_cast<AsyncPieceLibraryWidget*>(dynamic_cast<QWidgetItem *>(pieceLibraryLayout->itemAt(j))->widget());

        Piece* piece = plw->getPiece();

        for(int i =0; i<(int(((*piece).pickup->subs.size()))); i++){
            SubpickupTemplate pick = (*piece).pickup->subs.at(i);
            PullPlan* plan = pick.plan;

            if (std::find(caneVec->begin(), caneVec->end(), plan) != caneVec->end()){
                ;
            }
            else{
                if (std::find(colorVec->begin(), colorVec->end(), ((*piece).pickup->overlayGlassColor)) != colorVec->end());
                else{
                    colorVec->push_back(((*piece).pickup->overlayGlassColor));
                }
                if (std::find(colorVec->begin(), colorVec->end(), (*piece).pickup->casingGlassColor) != colorVec->end());
                else colorVec->push_back((*piece).pickup->casingGlassColor);

                if (std::find(colorVec->begin(), colorVec->end(), (*piece).pickup->underlayGlassColor) != colorVec->end());
                else colorVec->push_back((*piece).pickup->underlayGlassColor);

                buildCaneTree(plan, caneVec, colorVec);
                if (std::find(caneVec->begin(), caneVec->end(), plan) != caneVec->end());
                else(*caneVec).push_back(plan);
            }
        }
    }
}

QString MainWindow::writeJson(Json::Value root){
    Json::StyledWriter writer;
    std::string outputConfig = writer.write( root );
    QString output = QString::fromStdString(outputConfig);
    return output;
}

void MainWindow::writeCane(Json::Value *root, map<PullPlan*, int>* caneMap, map<GlassColor*, int> colorMap, vector<PullPlan*> caneVec){
    AsyncPullPlanLibraryWidget* pplw;
    for (int i = 0; i < pullPlanLibraryLayout->count(); ++i){
        pplw = dynamic_cast<AsyncPullPlanLibraryWidget*>(
            dynamic_cast<QWidgetItem *>(pullPlanLibraryLayout->itemAt(i))->widget());
        PullPlan* plan=pplw->getPullPlan();

        if((std::find(caneVec.begin(), caneVec.end(), plan)) != caneVec.end()){
            (*caneMap)[plan] = i;
        }

    }

    Json::Value *pullplan_nested = new Json::Value;
    map<PullPlan*, int>::iterator position;
    for(position=(*caneMap).begin(); position != (*caneMap).end();++position){
        PullPlan* plan = position->first;
        int i = position->second;
        //check dependency from piece to plan to save only dependent canes
        Json::Value *value5 = new Json::Value;
        *value5 = (plan->getTemplateType());
        Json::Value *value6 = new Json::Value;
        *value6 = (plan->hasSquareCasing());
        Json::Value *value7 = new Json::Value;
        *value7 = (plan->getCasingCount());
        Json::Value *value8 = new Json::Value;
         *value8 = (plan->getTwist());

        Json::Value *nested_value = new Json::Value;

        std::stringstream *templateTypeSstr = new std::stringstream;
        *templateTypeSstr << "templatetype";
        string templateType = (*templateTypeSstr).str();

        std::stringstream *hasSquareCasingSstr = new std::stringstream;
        *hasSquareCasingSstr << "hasSquareCasing";
        string hasSquareCasing = (*hasSquareCasingSstr).str();

        std::stringstream *casingcountSstr = new std::stringstream;
        *casingcountSstr << "casingcount";
        string casingCount = (*casingcountSstr).str();


        for (int k=0;k<int (plan->getCasingCount());k++){
            Json::Value *nested_value2 = new Json::Value;
            (*nested_value2)["CasingColor"] = colorMap.find(plan->getCasingColor(k))->second;
            (*nested_value2)["CasingShape"] = plan->getCasingShape(k);
            (*nested_value2)["CasingThickness"] = plan->getCasingThickness(k);
            std::stringstream *casingSstr = new std::stringstream;
            *casingSstr << k<<"Casing";
            string casing = (*casingSstr).str();
            (*nested_value)[casing]=(*nested_value2);
        }

        std::stringstream *twistsSstr = new std::stringstream;
        *twistsSstr << "twists";
        string twists = (*twistsSstr).str();

        (*nested_value)[templateType] = *value5;
        (*nested_value)[hasSquareCasing] = *value6;
        (*nested_value)[casingCount] = *value7;
        (*nested_value)[twists] = *value8;

        std::stringstream *pullplannrSstr = new std::stringstream;
        *pullplannrSstr  <<i<< "canes";
        string pullPlanNr = (*pullplannrSstr).str();

        if(plan->subs.size()==0);
        else{
            Json::Value *nested_value3 = new Json::Value;
            for(int j=0; j<(int (plan->subs.size())); j++){
                Json::Value *nested_value2 = new Json::Value;
                SubpullTemplate templ = plan->subs.at(j);

                //(*nested_value2)["group"] = templ.group;
                (*nested_value2)["shape"] = templ.shape;
                (*nested_value2)["diameter"] = templ.diameter;
                (*nested_value2)["cane"] = (*caneMap).find(templ.plan)->second;

                (*nested_value2)["x"] = templ.location.operator [](0);
                (*nested_value2)["y"] = templ.location.operator [](1);
                (*nested_value2)["z"] = templ.location.operator [](2);

                std::stringstream *Sstr = new std::stringstream;
                if(j<10)
                    *Sstr <<"0" <<j << "cane";
                else
                    *Sstr <<j << "cane";
                string name = (*Sstr).str();

                (*nested_value3)[name] = (*nested_value2);

            }
            (*nested_value)["SubpullplanTemplate"] = (*nested_value3);
        }
        (*pullplan_nested)[pullPlanNr] = *nested_value;
        i++;
    }
    (*root)["canes"] = (*pullplan_nested);
}

void MainWindow::writeColor(Json::Value* root, map<GlassColor*, int>* colorMap, vector<GlassColor*> colorVec){
    AsyncColorBarLibraryWidget* cblw;
    Json::Value *color_nested = new Json::Value;
    for (int i = 0; i < colorBarLibraryLayout->count(); ++i){
        cblw = dynamic_cast<AsyncColorBarLibraryWidget*>(
            dynamic_cast<QWidgetItem *>(colorBarLibraryLayout->itemAt(i))->widget());
        GlassColor* color = (cblw->getGlassColor());

        if((std::find(colorVec.begin(), colorVec.end(), color)) != colorVec.end()){

            (*colorMap)[color] = i+1; //global_clear ==0

            Json::Value *nested_value = new Json::Value;

            (*nested_value)["R"] = cblw->getGlassColor()->getColor()->operator [](0);
            (*nested_value)["G"] = cblw->getGlassColor()->getColor()->operator [](1);
            (*nested_value)["B"] = cblw->getGlassColor()->getColor()->operator [](2);
            (*nested_value)["alpha"] = cblw->getGlassColor()->getColor()->operator [](3);

            std::stringstream *colorSstr = new std::stringstream;
            *colorSstr  <<i<<"_" << (cblw->getGlassColor()->getName())->toStdString();
            string colorName = (*colorSstr).str();

            (*color_nested)[colorName] = (*nested_value);
        }
    }
    (*root)["colors"] = (*color_nested);
}

void MainWindow::writePiece(Json::Value* root, map<Piece*, int>* pieceMap, map<PullPlan*, int>* caneMap, map<GlassColor*, int> colorMap){
    AsyncPieceLibraryWidget *plw;
    Json::Value *piece_nested = new Json::Value;

    for (int i = 0; i < pieceLibraryLayout->count(); ++i){
        plw = dynamic_cast<AsyncPieceLibraryWidget*>(
                    dynamic_cast<QWidgetItem *>(pieceLibraryLayout->itemAt(i))->widget());

        Piece* piece = plw->getPiece();
        (*pieceMap)[piece] = i;
    }

    map<Piece*, int>::iterator position;
    for(position = (*pieceMap).begin(); position != (*pieceMap).end(); ++position){
        Json::Value *nested_value = new Json::Value;
        Piece* piece = position->first;
        int i = position->second;

        (*nested_value)["templateType"] =((*piece).pickup->getTemplateType());
        (*nested_value)["overlayGlassColor"] = colorMap.find((*piece).pickup->overlayGlassColor)->second;
        (*nested_value)["underlayGlassColor"] = colorMap.find((*piece).pickup->underlayGlassColor)->second;
        (*nested_value)["casingGlassColor"] = colorMap.find((*piece).pickup->casingGlassColor)->second;

        Json::Value *nested_value3 = new Json::Value;

        for(int i = 0;i<int ((*piece).pickup->getParameterCount());i++){
            TemplateParameter pickTemplPara;
            (*piece).pickup->getParameter(i, &pickTemplPara);
            (*nested_value3)[(pickTemplPara.name)] = pickTemplPara.value;
        }


        for(int i = 0;i<(int((*piece).getParameterCount()));i++){
            TemplateParameter pieceTemplPara;
            (*piece).getParameter(i,&pieceTemplPara);
            (*nested_value)[pieceTemplPara.name] = pieceTemplPara.value;
        }



        for(int j=0; j<(int ((*piece).pickup->subs.size())); j++){
            Json::Value *nested_value2 = new Json::Value;
            SubpickupTemplate templ = (*piece).pickup->subs.at(j);

            (*nested_value2)["length"] = templ.length;
            (*nested_value2)["orientation"] = templ.orientation;
            (*nested_value2)["shape"] = templ.shape;
            (*nested_value2)["width"] = templ.width;
            (*nested_value2)["cane"] = (*caneMap).find(templ.plan)->second;

            (*nested_value2)["x"] = templ.location.operator [](0);
            (*nested_value2)["y"] = templ.location.operator [](1);
            (*nested_value2)["z"] = templ.location.operator [](2);

            std::stringstream *Sstr = new std::stringstream;
            *Sstr <<j << "cane";
            string name = (*Sstr).str();

            (*nested_value3)[name] = (*nested_value2);
        }
        (*nested_value)[PieceTemplate::enumToString(piece->getTemplateType())] = (*nested_value3);
        std::stringstream *Sstr = new std::stringstream;
        *Sstr << i<<"_piece";
        string name = (*Sstr).str();
        (*piece_nested)[name]=(*nested_value);
    }
    (*root)["pieces"] = (*piece_nested);
}

void MainWindow::openPiece(Json::Value* root, int index){
    std::vector<std::string> rootObjectList = (*root).getMemberNames();
    enum piece{
        PickupParameterCount,
        PickupTemplateType,
        PickupoverlayGlassColor,
        PickupunderlayGlassColor
    };
    static std::map<std::string, int> mapEnum;
    mapEnum["PickupParameterCount"] = 100;
    mapEnum["PickupTemplateType"] = 101;
    mapEnum["PickupoverlayGlassColor"] = 102;
    mapEnum["PickupunderlayGlassColor"] = 103;

    switch(mapEnum[rootObjectList.operator [](index)]){
        case 100 : cout << (*root)["PickupParameterCount"]; break;
        case 101 : cout << (*root)["PickupTemplateType"]; break;
        case 102 : cout << (*root)["PickupoverlayGlassColor"]; break;
        case 103 : cout << (*root)["PickupunderlayGlassColor"]; break;
        default: cout << "error " << rootObjectList.operator [](index) << endl;
        //im default nach cane, samecane teilen
    }
    Json::Value *root1 = new Json::Value;
    *root1 = (rootObjectList.operator [](index));
    for(int i=0; i<(int (rootObjectList.operator [](index).size()));i++){
        cout << (rootObjectList.operator [](index));//getmembers wie oben!! ["cane1"]["subcane_depth1_"]["CasingThickness"];
        cout << endl;
    }
}

void MainWindow::openColors(Json::Value rootColor, map<GlassColor*, int>* colorMap){
    //rootXXX variables are Json::Values; getMemberNames available
    //vecXXX variables are vectors; operator [] available
    vector<std::string> vecColorMembers = rootColor.getMemberNames(); //vector for colornames
    (*colorMap).end();
    enum colors{
        R,
        G,
        B,
        alpha
    };

    static std::map<std::string, int> colorMapEnum;
    colorMapEnum["R"] = R;
    colorMapEnum["G"] = G;
    colorMapEnum["B"] = B;
    colorMapEnum["alpha"] = alpha;


    for(int j=0;j<(int (vecColorMembers.size()));j++){
        Json::Value rootColorValue = rootColor[vecColorMembers.at(j)];
        vector<std::string> vecColorValueMembers = rootColorValue.getMemberNames(); //vector for RGBalpha

        Color *color = new Color;
        GlassColor *glasscolor = new GlassColor;

        if(vecColorValueMembers.size()==4){
            for(int k=0; k<4; k++){
                switch(colorMapEnum[vecColorValueMembers.at(k)]){
                case R : ((*color).c)[0] = rootColorValue["R"].asFloat(); break;
                case G : ((*color).c)[1] = rootColorValue["G"].asFloat(); break;
                case B : ((*color).c)[2] = rootColorValue["B"].asFloat(); break;
                case alpha : ((*color).c)[3] = rootColorValue["alpha"].asFloat(); break;
                }
                (*glasscolor).setColor(*color);
            }
            string colorNumberSt = (vecColorMembers.at(j));
            colorNumberSt.resize((vecColorMembers.at(j)).find("_")); //extracts number from string
            int colorNumberInt = atoi(colorNumberSt.c_str());
            (*colorMap)[glasscolor] = colorNumberInt;

            string colorName = (vecColorMembers.at(j)).substr ((vecColorMembers.at(j)).find("_")+1);
            (*glasscolor).setName(QString::fromStdString(colorName));
            unhighlightAllLibraryWidgets();
            AsyncColorBarLibraryWidget *w =new AsyncColorBarLibraryWidget(glasscolor, this);
            colorBarLibraryLayout->addWidget(w);
            colorEditorWidget->setGlassColor(glasscolor);
            colorBarLibraryLayout->update();
            // Trigger GUI updates
            show();
            w->updatePixmaps();
            emit someDataChanged();
        }
        else
            cout << "error in color " << vecColorValueMembers.operator [](j);
    }
}

//void MainWindow::openCanes(Json::Value root, map<PullPlan*, int>* caneMap, map<GlassColor*, int>* colorMap){
//    ;
//}

//void MainWindow::openPieces(Json::Value root, map<Piece*, int>* pieceMap,map<PullPlan*, int>* caneMap, map<GlassColor*, int>* colorMap){
//    ;
//}

void MainWindow::open(){
    //open file dialog
        QString filename = QFileDialog::getOpenFileName(this, tr("Open Document"), QDir::currentPath(), tr("VirtualGlass file (*.glass);;All files (*.*)") );
    QFile openFile(filename);
    openFile.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream fileInput(&openFile);
    QString QStr = fileInput.readAll();
    std::string str = QStr.toStdString();
    Json::Value root;
    Json::Reader reader;
    ifstream readHdl;
    char versionNo[16]; //date has always 12 characters

    readHdl.open(filename.toAscii());
    readHdl.getline(versionNo,16,'\n');
    readHdl.close();

    //check version No
    if((int(versionNo[2]))>55);//ascii: char 7 == int 55
    else{
        //lower version than 7xx
    }
    if((int(versionNo[3])<53)&&((int(versionNo[2]))>55)){
            //lower version than (firstIf)5x
    }
    if((int(versionNo[4])<56)&&(int(versionNo[3])>53)&&((int(versionNo[2]))>55)){
        //lower version than (firstIf)(secondIf)8
    }

    bool parsedSuccess = reader.parse(str,root,false);
    map<GlassColor*, int> colorMap;
    map<PullPlan*, int> caneMap;
    map<Piece*, int> pieceMap;

    if(!parsedSuccess){
        cout<<"Failed to parse JSON"<<endl<<reader.getFormatedErrorMessages()<<endl; //debugging
    }

    if(int(root.size()!=3)){
        cout << "error in file";
    }
    else{
        vector<std::string> rootMembers = root.getMemberNames();
        for(int i=0;i<3;i++){
            if(rootMembers.operator [](i)=="colors") openColors(root["colors"], &colorMap);
            //if(rootMembers.operator [](i)=="canes") openCanes(root["canes"], &caneMap, &colorMap);
            //if(rootMembers.operator [](i)=="pieces") openPieces(root["pieces"], &pieceMap, &caneMap, &colorMap);
        }
    }
}

void MainWindow::saveAll(){
    ;//open
}

void MainWindow::saveSelected(){
    ;
}

void MainWindow::saveSelectedAs(){
    ;
}

void MainWindow::saveAllAs(){
    //save file dialog
    QString filename = QFileDialog::getSaveFileName(this, tr("Save your glass piece"), QDir::currentPath(), tr("VirtualGlass (*.glass)") );
    //improve: prevent character set error in filename
    //improve: empty file name -> "no savefile choosen"

    QFile saveFile(filename);
    saveFile.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream fileOutput(&saveFile);

    //read version and date from version.txt; write this into json file (root0)
    //first line; versionNo and date
    ifstream readHdl;
    //date has always 12 characters
    char date[11];
    char versionNo[4];

    readHdl.open(":/version.txt");
    readHdl.getline(versionNo,4,'\n');
    string versionStr;
    versionStr.assign(versionNo, 4);
    readHdl.getline(date,11,'\n');
    string dateStr = date;
    readHdl.close();

    QString versionQstr(versionStr.c_str());
    QString dateQstr(dateStr.c_str());
    fileOutput << "//"<<versionQstr <<"_"<< dateQstr << "\n";

    Json::Value root;
    map<Piece*,int> pieceMap;
    map<PullPlan*,int> caneMap;
    map<GlassColor*, int> colorMap;
    vector<PullPlan*> caneVec;
    vector<GlassColor*> colorVec;

    colorMap[GlobalGlass::color()] = NULL;

    buildCaneMap(&caneVec, &colorVec);
    writeColor(&root, &colorMap, colorVec);
    writeCane(&root, &caneMap, colorMap, caneVec);
    writePiece(&root, &pieceMap, &caneMap, colorMap);

    fileOutput << writeJson(root);
    saveFile.close();
}

void MainWindow::setupMenus()
{
    //open
    openAct = new QAction(tr("&Open"), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    //save
    saveAllAct = new QAction(tr("&SaveAll"), this);
    saveAllAct->setShortcuts(QKeySequence::Save);
    saveAllAct->setStatusTip(tr("Save all glass to disk"));
    connect(saveAllAct, SIGNAL(triggered()), this, SLOT(saveAll()));

    //saveSelected; no shortcut
    saveSelectedAct = new QAction(tr("&SaveSelected"), this);
    saveSelectedAct->setStatusTip(tr("Save selected glass to disk"));
    connect(saveSelectedAct, SIGNAL(triggered()), this, SLOT(saveSelected()));

    //saveAllAs
    saveAllAsAct = new QAction(tr("&SaveAllAs"), this);
    saveAllAsAct->setShortcuts(QKeySequence::SaveAs);
    saveAllAsAct->setStatusTip(tr("Save all glass to disk"));
    connect(saveAllAsAct, SIGNAL(triggered()), this, SLOT(saveAllAs()));

    //saveSelectedAs
    saveSelectedAsAct = new QAction(tr("&SaveSelectedAs"), this);
    saveSelectedAsAct->setStatusTip(tr("Save selected glass to disk"));
    connect(saveSelectedAsAct, SIGNAL(triggered()), this, SLOT(saveSelectedAs()));


    //examples:webtutorial1
    web1PieceAction = new QAction("Tutorial 1", this);
    web1PieceAction->setStatusTip("The first tutorial found on virtualglass.org");

    //examples:webtutorial2
    web2PieceAction = new QAction("Tutorial 2", this);
    web2PieceAction->setStatusTip("The second tutorial found on virtualglass.org");

    //examples:random:simple cane
    randomSimpleCaneAction = new QAction("&Simple Cane", this);
    randomSimpleCaneAction->setStatusTip("Randomly generate a simple example cane.");

    //examples:random:simple piece
    randomSimplePieceAction = new QAction("&Simple Piece", this);
    randomSimplePieceAction->setStatusTip("Randomly generate a simple example piece.");

    //examples:random:complex cane
    randomComplexCaneAction = new QAction("&Complex Cane", this);
    randomComplexCaneAction->setStatusTip("Ranomly generate a complex example cane.");

    //examples:random:complex piece
    randomComplexPieceAction = new QAction("&Complex Piece", this);
    randomComplexPieceAction->setStatusTip("Randomly generate a complex example piece.");

    // Examples menu and Examples:Random menu
    examplesMenu = menuBar()->addMenu("&Examples"); //create menu for cane/piece examples
    webExamplesMenu = examplesMenu->addMenu("&Web");
    webExamplesMenu->addAction(web1PieceAction);
    webExamplesMenu->addAction(web2PieceAction);
    randomExamplesMenu = examplesMenu->addMenu("&Random");
    randomExamplesMenu->addAction(randomSimpleCaneAction);
    randomExamplesMenu->addAction(randomComplexCaneAction);
    randomExamplesMenu->addSeparator();
    randomExamplesMenu->addAction(randomSimplePieceAction);
    randomExamplesMenu->addAction(randomComplexPieceAction);

    // toggle depth peeling
    depthPeelAction = new QAction(tr("&Depth peeling"), this);
    depthPeelAction->setCheckable(true);
    depthPeelAction->setChecked(NiceViewWidget::peelEnable);
    depthPeelAction->setStatusTip(tr("Toggle high-quality transparency rendering in 3D views"));

    // Performance menu
    perfMenu = menuBar()->addMenu(tr("Performance"));
    perfMenu->addAction(depthPeelAction);

}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File")); //create File menu
    fileMenu->addAction(openAct); //add openButton
    fileMenu->addAction(saveAllAct); //add saveButton
    fileMenu->addAction(saveSelectedAct); //add saveButton
    fileMenu->addAction(saveAllAsAct); //add saveButton
    fileMenu->addAction(saveSelectedAsAct); //add saveAsButton
}
