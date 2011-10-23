

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include <QObject>
#include "constants.h"
#include "niceviewwidget.h"
#include "piecelibrarywidget.h"
#include "pickupplanlibrarywidget.h"
#include "pullplanlibrarywidget.h"
#include "colorbarlibrarywidget.h"
#include "pulltemplate.h"
#include "model.h"
#include "pullplan.h"
#include "pullplaneditorviewwidget.h"
#include "pickupplan.h"
#include "pickupplaneditorviewwidget.h"
#include "piece.h"
#include "pieceeditorviewwidget.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT

	public:
		MainWindow(Model* model);
        	void mousePressEvent(QMouseEvent* event);
        	void mouseMoveEvent(QMouseEvent* event);
		void mouseDoubleClickEvent(QMouseEvent* event);
        	void dragMoveEvent(QDragMoveEvent* event);
		void seedTable();
	
	signals:
		void someDataChanged();

	private:
		// Methods
		void setupTable();
		void setupNiceView();	
		void setupEditors();
		void setupPullPlanEditor();
		void setupPickupPlanEditor();
		void setupPieceEditor();
		void setupConnections();
		void updatePullPlanEditor();
		void updatePickupPlanEditor();
		void updatePieceEditor();
		void updateNiceView();
		void updateLibrary();

		// Variables
		QWidget* drawingWidget;
		QPoint dragStartPosition;
		QStackedWidget* editorStack;
		QWidget* pullPlanEditorPage;
		QWidget* pickupPlanEditorPage;
		QWidget* pieceEditorPage;
		PullPlan* pullPlanEditorPlan;
		PickupPlan* pickupPlanEditorPlan;
		Piece* pieceEditorPlan;
		Color defaultColor;
		PullPlanLibraryWidget* pullPlanEditorPlanLibraryWidget;
		PickupPlanLibraryWidget* pickupPlanEditorPlanLibraryWidget;
		PieceLibraryWidget* pieceEditorPlanLibraryWidget;
		Model* model;
		QWidget* centralWidget;
		QHBoxLayout* centralLayout;
		QComboBox* pullTemplateComboBox;
		QComboBox* pickupTemplateComboBox;
		QComboBox* pieceTemplateComboBox;
		PullPlanEditorViewWidget* pullPlanEditorViewWidget;
		PickupPlanEditorViewWidget* pickupPlanEditorViewWidget;
		PieceEditorViewWidget* pieceEditorViewWidget;
		QLabel* niceViewLabel;
		QVBoxLayout* niceViewLayout;
		NiceViewWidget* niceViewWidget;
		QScrollArea* pullPlanLibraryScrollArea;
		QScrollArea* colorBarLibraryScrollArea;
		QScrollArea* pickupPlanLibraryScrollArea;
		QScrollArea* pieceLibraryScrollArea;
		QHBoxLayout* pullPlanLibraryLayout;
		QHBoxLayout* colorBarLibraryLayout;
		QHBoxLayout* pickupPlanLibraryLayout;
		QHBoxLayout* pieceLibraryLayout;
		QSlider* pullPlanTwistSlider;
		QSpinBox* pullPlanTwistSpin;
		QPushButton* newPullPlanButton;
		QPushButton* newPickupPlanButton;
		QPushButton* newPieceButton;

	private slots:
		void updateEverything();
		void pullTemplateComboBoxChanged(int index);
		void pickupTemplateComboBoxChanged(int index);
		void pieceTemplateComboBoxChanged(int index);
		void newPullPlan();	
		void newPickupPlan();	
		void newPiece();	
		void pullPlanTwistSliderChanged(int p);
		void pullPlanTwistSpinChanged(int p);
};


#endif

