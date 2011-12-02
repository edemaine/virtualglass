

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
#include "pullplaneditorviewwidget.h"
#include "pickupplan.h"
#include "pickupplaneditorviewwidget.h"
#include "piece.h"
#include "pulltemplatelibrarywidget.h"
#include "pickuptemplatelibrarywidget.h"

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
		void setupTable();
		void setupEditors();
		void setupEmptyPaneEditor();
		void setupColorEditor();
		void setupPullPlanEditor();
		void setupPieceEditor();
		void setupPieceSubeditor1(QVBoxLayout* layout);
		void setupPieceSubeditor2(QVBoxLayout* layout);
		void setupConnections();
		void updateColorEditor();
		void updatePullPlanEditor();
		void updatePickupPlanEditor();
		void updatePieceEditor();
		void updateLibrary();
		void initializeRandomPiece();

		void highlightPlanLibraryWidgets(ColorBarLibraryWidget* cblw,bool highlight,bool setupDone);
		void highlightPlanLibraryWidgets(PullPlanLibraryWidget* plplw,bool highlight,bool setupDone);
		void highlightPlanLibraryWidgets(PieceLibraryWidget* plw,bool highlight,bool setupDone);
		void unhighlightAllPlanLibraryWidgets(bool setupDone);

		// Variables
		NiceViewWidget* colorBarNiceViewWidget;
		NiceViewWidget* pullPlanNiceViewWidget;
		NiceViewWidget* pieceNiceViewWidget;
		QLabel* pieceTemplateParameter1Label;
		QLabel* pieceTemplateParameter2Label;
		QLabel* pickupTemplateParameter1Label;
		QSpinBox* pickupTemplateParameter1SpinBox;
		QSlider* pieceTemplateParameter1Slider;
		QSlider* pieceTemplateParameter2Slider;
		QCheckBox* writeRawCheckBox;
		QGridLayout* tableGridLayout;
		int pullPlanCount;
		int pieceCount;
		int colorBarCount;
		bool setupDone;
		QButtonGroup* pullTemplateShapeButtonGroup;
		QPoint dragStartPosition;
		QStackedWidget* editorStack; //editorStack.currentIndex() gives with mode
		QWidget* emptyEditorPage;
		QWidget* colorEditorPage;
		QWidget* pullPlanEditorPage;
		QWidget* pieceEditorPage;
		PullPlan* colorEditorPlan;
		PullPlan* pullPlanEditorPlan;
		Piece* pieceEditorPiece;
		ColorBarLibraryWidget* colorEditorPlanLibraryWidget;
		PullPlanLibraryWidget* pullPlanEditorPlanLibraryWidget;
		PieceLibraryWidget* pieceEditorPieceLibraryWidget;
		Model* model;
		QWidget* centralWidget;
		QHBoxLayout* centralLayout;
		QComboBox* pieceTemplateComboBox;
		ColorEditorViewWidget* colorEditorViewWidget;
		PullPlanEditorViewWidget* pullPlanEditorViewWidget;
		PickupPlanEditorViewWidget* pickupPlanEditorViewWidget;
		QHBoxLayout* pullTemplateLibraryLayout;
		QHBoxLayout* pickupTemplateLibraryLayout;
		QSlider* pullPlanTwistSlider;
		QSlider* pullTemplateCasingThicknessSlider;
		QSpinBox* pullPlanTwistSpin;
		QPushButton* newColorBarButton;
		QPushButton* newPullPlanButton;
		QPushButton* newPieceButton;

	private slots:
		void updateEverything();
		void pieceTemplateComboBoxChanged(int index);
		void newColorBar();
		void newPullPlan();
		void newPiece();
		void pullPlanTwistSliderChanged(int p);
		void pullTemplateCasingThicknessSliderChanged(int p);
		void pullPlanTwistSpinChanged(int p);
		void pullTemplateShapeButtonGroupChanged(int);
		void pieceTemplateParameterSlider1Changed(int);
		void pieceTemplateParameterSlider2Changed(int);
		void pickupTemplateParameter1SpinBoxChanged(int);
		void writeRawCheckBoxChanged(int);
};


#endif

