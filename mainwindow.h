

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
// <json/json.h>

class GlassColor;
class PullPlan;
class PickupPlan;
class Piece;
class NiceViewWidget;
class AsyncPieceLibraryWidget;
class AsyncPullPlanLibraryWidget;
class AsyncColorBarLibraryWidget;
class PullPlanEditorWidget;
class ColorEditorWidget;
class PieceEditorWidget;

class QMouseEvent;
class QKeyEvent;
class QLabel;
class QVBoxLayout; 
class QHBoxLayout; 
class QPoint;
class QStackedWidget;
class QWidget;
class QPushButton;
class QMenu;
class QAction;

class MainWindow : public QMainWindow
{
	Q_OBJECT

	public:
		MainWindow();
		void mousePressEvent(QMouseEvent* event);
		void mouseReleaseEvent(QMouseEvent* event);
		void mouseMoveEvent(QMouseEvent* event);
		void seedEverything();
		void keyPressEvent(QKeyEvent* e);
		QString windowTitle();

	//protected:
		//void contextMenuEvent(QContextMenuEvent *event);

	signals:
		void someDataChanged();

	private:
		// enums
		enum ViewMode
		{
			EMPTY_VIEW_MODE=0, // must start at 0 to match usage as QStackedWidget index
			COLORBAR_VIEW_MODE,
			PULLPLAN_VIEW_MODE,
			PIECE_VIEW_MODE
		};

		// Methods
		void setupLibrary();
		void setupEditors();
		void setupEmptyPaneEditor();
		void setupColorEditor();
		void setupPullPlanEditor();
		void setupPieceEditor();
		void setupConnections();
		void setupMenus();
		void updateLibrary();
		void initializeRandomPiece();
		void deleteCurrentEditingObject();
		void setViewMode(enum ViewMode m);
		//QString writeJson(Json::Value);
		//void prepareJson(PullPlan*, Json::Value*, string nestedValue);
		//void buildCaneTree(PullPlan* , PullPlan* , map<PullPlan*,int>, Json::Value*);
		//void buildCaneMap(PullPlan*, Json::Value*, map<PullPlan*,int>);
		//int getM();
		//void setM(int m);

		void unhighlightAllLibraryWidgets();
		bool glassColorIsDependancy(GlassColor* color);
		bool pullPlanIsDependancy(PullPlan* plan);

		// Variables
		//int m;
		QLabel* whatToDoLabel;
		NiceViewWidget* colorBarNiceViewWidget;
		QVBoxLayout* colorBarLibraryLayout;
		QVBoxLayout* pullPlanLibraryLayout;
		QVBoxLayout* pieceLibraryLayout;
		bool isDragging;
		QPoint dragStartPosition;
		QStackedWidget* editorStack; //editorStack.currentIndex() gives with mode
		QWidget* emptyEditorPage;
		QWidget* centralWidget;
		QHBoxLayout* centralLayout;
		ColorEditorWidget* colorEditorWidget;
		PullPlanEditorWidget* pullPlanEditorWidget;
		PieceEditorWidget* pieceEditorWidget;
		QPushButton* newColorBarButton;
		QPushButton* newPullPlanButton;
		QPushButton* newPieceButton;
		QPushButton* copyColorBarButton;
		QPushButton* copyPullPlanButton;
		QPushButton* copyPieceButton;
		QMenu *webExamplesMenu;
		QMenu *randomExamplesMenu;
		QMenu *examplesMenu;
		QAction *randomSimpleCaneAction;
		QAction *randomSimplePieceAction;
		QAction *randomComplexCaneAction;
		QAction *randomComplexPieceAction;
		QAction *web1PieceAction;
		QAction *web2PieceAction;
		QMenu *fileMenu;
		QAction *openAction;
		QAction *saveAction;
		QAction *saveAsAction;
		QMenu *perfMenu;
		QAction *depthPeelAction;

	private slots:
		void updateEverything();
		void newColorBar();
		void newPullPlan();
		void newPullPlan(PullPlan* p);
		void newPiece();
		void copyColorBar();
		void copyPullPlan();
		void copyPiece();
		void open();
		void save();
		void saveAs();
		void randomSimpleCaneExampleActionTriggered();
		void randomSimplePieceExampleActionTriggered();
		void randomComplexCaneExampleActionTriggered();
		void randomComplexPieceExampleActionTriggered();
		void web1PieceExampleActionTriggered();
		void web2PieceExampleActionTriggered();
		void depthPeelActionTriggered();
};


#endif

