

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
#include "model.h"
#include "coloreditorviewwidget.h"
#include "pullplan.h"
#include "pullplaneditorwidget.h"
#include "pickupplan.h"
#include "pickupplaneditorviewwidget.h"
#include "piece.h"
#include "pulltemplatelibrarywidget.h"
#include "pickuptemplatelibrarywidget.h"
#include "piecetemplatelibrarywidget.h"
#include "qgraphicshighlighteffect.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT

	public:
		MainWindow(Model* model);
		void mousePressEvent(QMouseEvent* event);
		void mouseReleaseEvent(QMouseEvent* event);
		void mouseMoveEvent(QMouseEvent* event);
		void dragMoveEvent(QDragMoveEvent* event);
		void seedEverything();

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
		void setupPieceSubeditor1(QVBoxLayout* layout);
		void setupPieceSubeditor2(QVBoxLayout* layout);
		void setupConnections();
		void updateColorEditor();
		void updatePickupPlanEditor();
		void updatePieceEditor();
		void updateLibrary();
		void initializeRandomPiece();

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
		NiceViewWidget* pieceNiceViewWidget;
		QLabel* pieceTemplateParameter1Label;
		QLabel* pieceTemplateParameter2Label;
		QLabel* pickupTemplateParameter1Label;
		QSpinBox* pickupTemplateParameter1SpinBox;
		QSlider* pieceTemplateParameter1Slider;
		QSlider* pieceTemplateParameter2Slider;
		QGridLayout* tableGridLayout;
		int pullPlanCount;
		int pieceCount;
		int colorBarCount;
		QPoint dragStartPosition;
		QStackedWidget* editorStack; //editorStack.currentIndex() gives with mode
		QWidget* emptyEditorPage;
		QWidget* colorEditorPage;
		QWidget* pieceEditorPage;
		PullPlan* colorEditorPlan;
		Piece* pieceEditorPiece;
		ColorBarLibraryWidget* colorEditorPlanLibraryWidget;
		PullPlanLibraryWidget* pullPlanEditorPlanLibraryWidget;
		PieceLibraryWidget* pieceEditorPieceLibraryWidget;
		Model* model;
		QWidget* centralWidget;
		QHBoxLayout* centralLayout;
		ColorEditorViewWidget* colorEditorViewWidget;
		PullPlanEditorWidget* pullPlanEditorWidget;
		PickupPlanEditorViewWidget* pickupPlanEditorViewWidget;
		QHBoxLayout* pullTemplateLibraryLayout;
		QHBoxLayout* pickupTemplateLibraryLayout;
		QHBoxLayout* pieceTemplateLibraryLayout;
		QPushButton* newColorBarButton;
		QPushButton* newPullPlanButton;
		QPushButton* newPieceButton;

	private slots:
		void updateEverything();
		void newColorBar();
		void newPullPlan();
		void newPullPlan(PullPlan* p);
		void newPiece();
		void pieceTemplateParameterSlider1Changed(int);
		void pieceTemplateParameterSlider2Changed(int);
		void pickupTemplateParameter1SpinBoxChanged(int);
};


#endif

