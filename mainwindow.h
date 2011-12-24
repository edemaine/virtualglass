

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include <QObject>

#include "constants.h"
#include "niceviewwidget.h"
#include "piecelibrarywidget.h"
#include "pullplanlibrarywidget.h"
#include "colorbarlibrarywidget.h"
#include "pulltemplate.h"
#include "pullplan.h"
#include "pickupplan.h"
#include "piece.h"
#include "pullplaneditorwidget.h"
#include "coloreditorwidget.h"
#include "pieceeditorwidget.h"
#include "qgraphicshighlighteffect.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT

	public:
		MainWindow();
		void mousePressEvent(QMouseEvent* event);
		void mouseReleaseEvent(QMouseEvent* event);
		void mouseMoveEvent(QMouseEvent* event);
		void dragMoveEvent(QDragMoveEvent* event);
		void seedEverything();
		void keyPressEvent(QKeyEvent* e);

	signals:
		void someDataChanged();

	private:
		// Methods
		void setupLibrary();
		void setupStatusBar();
		void setupEditors();
		void setupEmptyPaneEditor();
		void setupColorEditor();
		void setupPullPlanEditor();
		void setupPieceEditor();
		void setupConnections();
		void updateLibrary();
		void initializeRandomPiece();
		void deleteCurrentEditingObject();

		void unhighlightLibraryWidget(PieceTemplateLibraryWidget* w);
		void unhighlightLibraryWidget(PickupTemplateLibraryWidget* w);
		void unhighlightLibraryWidget(ColorBarLibraryWidget* w);
		void unhighlightLibraryWidget(PullPlanLibraryWidget* w);
		void unhighlightLibraryWidget(PieceLibraryWidget* w);
		void highlightLibraryWidget(PieceTemplateLibraryWidget* w);
		void highlightLibraryWidget(PickupTemplateLibraryWidget* w);
		void highlightLibraryWidget(ColorBarLibraryWidget* w, int dependancy);
		void highlightLibraryWidget(PullPlanLibraryWidget* w, int dependancy);
		void highlightLibraryWidget(PieceLibraryWidget* w, int dependancy);
		void unhighlightAllLibraryWidgets();

		// Variables
		QLabel* whatToDoLabel;
		QStatusBar* statusBar;
		NiceViewWidget* colorBarNiceViewWidget;
		QVBoxLayout* colorBarLibraryLayout;
		QVBoxLayout* pullPlanLibraryLayout;
		QVBoxLayout* pieceLibraryLayout;
		QPoint dragStartPosition;
		QStackedWidget* editorStack; //editorStack.currentIndex() gives with mode
		QWidget* emptyEditorPage;
		ColorBarLibraryWidget* colorEditorBarLibraryWidget;
		PullPlanLibraryWidget* pullPlanEditorPlanLibraryWidget;
		PieceLibraryWidget* pieceEditorPieceLibraryWidget;
		QWidget* centralWidget;
		QHBoxLayout* centralLayout;
		ColorEditorWidget* colorEditorWidget;
		PullPlanEditorWidget* pullPlanEditorWidget;
		PieceEditorWidget* pieceEditorWidget;
		QPushButton* newColorBarButton;
		QPushButton* newPullPlanButton;
		QPushButton* newPieceButton;

	private slots:
		void updateEverything();
		void newColorBar();
		void newPullPlan();
		void newPullPlan(PullPlan* p);
		void newPiece();
};


#endif

