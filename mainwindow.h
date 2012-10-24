

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtGui>
#include <QObject>
#include <json/json.h>

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

        void setM(int);
        char* getMchar();
        int getM();

    protected:
        void contextMenuEvent(QContextMenuEvent *event);

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
        QString writeJson(Json::Value);
        void prepareJson(PullPlan*, Json::Value*, std::string nestedValue);
        void buildCaneTree(PullPlan* , PullPlan* , std::map<PullPlan*,int>, Json::Value*);
        void buildCaneMap(PullPlan*, Json::Value*, std::map<PullPlan*,int>);
        void createActions();
        void createMenus();

		void unhighlightAllLibraryWidgets();
		bool glassColorIsDependancy(GlassColor* color);
		bool pullPlanIsDependancy(PullPlan* plan);

		// Variables
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
        QAction *openAct;
        QAction *saveAllAct;
        QAction *saveSelectedAct;
        QAction *saveAllAsAct;
        QAction *saveSelectedAsAct;
		QMenu *perfMenu;
		QAction *depthPeelAction;
        int m;

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
        void saveAll();
        void saveSelected();
        void saveAllAs();
        void saveSelectedAs();
		void randomSimpleCaneExampleActionTriggered();
		void randomSimplePieceExampleActionTriggered();
		void randomComplexCaneExampleActionTriggered();
		void randomComplexPieceExampleActionTriggered();
		void web1PieceExampleActionTriggered();
		void web2PieceExampleActionTriggered();
		void depthPeelActionTriggered();
        void buildCaneTree(PullPlan* , std::vector<PullPlan*>*, std::vector<GlassColor*>*);
        void buildCaneMap(std::vector<PullPlan *>*, std::vector<GlassColor*>*);
        void writeCane(Json::Value*, std::map<PullPlan*, int>*, std::map<GlassColor*, int>, std::vector<PullPlan*>);
        void writeColor(Json::Value*, std::map<GlassColor*, int>*, std::vector<GlassColor*>);
        void writePiece(Json::Value*, std::map<Piece*, int>*, std::map<PullPlan*, int>*, std::map<GlassColor*, int>);
        void openColors(Json::Value, std::map<GlassColor*, int>*);
        //void openCanes(Json::Value, std::map<PullPlan*, int>*, std::map<GlassColor*, int>* colorMap);
        void openPiece(Json::Value*, int index);
        //void openPieces(Json::Value, std::map<Piece*, int>*, std::map<PullPlan*, int>*, std::map<GlassColor*, int>*);
};

#endif

