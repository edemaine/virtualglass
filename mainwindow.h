

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <json/json.h>

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
class QActionGroup;

class GlassColor;
class PullPlan;
class PickupPlan;
class Piece;
class NiceViewWidget;
class PullPlanEditorWidget;
class ColorEditorWidget;
class PieceEditorWidget;

using std::vector;

class MainWindow : public QMainWindow
{
	Q_OBJECT

	public:
		MainWindow();
		void mousePressEvent(QMouseEvent* event);
		void mouseReleaseEvent(QMouseEvent* event);
		void mouseMoveEvent(QMouseEvent* event);
		void setupViews();
		void keyPressEvent(QKeyEvent* e);
		QString windowTitle();
		void openFile(QString filename, bool add);

	protected:
		void closeEvent(QCloseEvent *event);
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
		void setupSaveFile();
		void updateLibrary();
		void initializeRandomPiece();
		void deleteCurrentEditingObject();
		void setViewMode(enum ViewMode m);
		void clearLibrary();
		void unhighlightAllLibraryWidgets();
		bool glassColorIsDependancy(GlassColor* color);
		bool pullPlanIsDependancy(PullPlan* plan);
		void updateWindowTitle();
		void setDirtyBit(bool v);
		void setSaveFilename(QString filename);
		void getLibraryContents(vector<GlassColor*>& colors, vector<PullPlan*>& plans, vector<Piece*>& pieces);
		void getDependantLibraryContents(GlassColor* color, vector<GlassColor*>& colors, vector<PullPlan*>& plans);
		void getDependantLibraryContents(PullPlan* plan, vector<GlassColor*>& colors, vector<PullPlan*>& plans);
		void getDependantLibraryContents(Piece* piece, vector<GlassColor*>& colors, vector<PullPlan*>& plans,
			vector<Piece*>& pieces);
		void addToLibrary(vector<GlassColor*>& colors, vector<PullPlan*>& plans, vector<Piece*>& pieces);

		// Variables
		bool dirtyBit;
		QString saveFilename;
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
		QPushButton* newGlassColorButton;
		QPushButton* newPullPlanButton;
		QPushButton* newPieceButton;
		QPushButton* copyGlassColorButton;
		QPushButton* copyPullPlanButton;
		QPushButton* copyPieceButton;
		QPushButton* mergeButton;
		QMenu *examplesMenu;
		QAction *randomSimpleCaneAction;
		QAction *randomSimplePieceAction;
		QAction *randomComplexCaneAction;
		QAction *randomComplexPieceAction;
		QMenu *fileMenu;
		QAction *newFileAction;
		QAction *openFileAction;
		QAction *addFileAction;
		QAction *importSVGFileAction;
		QAction *exportPLYFileAction;
		QAction *exportOBJFileAction;
		QAction *saveAllFileAction;
		QAction *saveAllAsFileAction;
		QAction *saveSelectedAsFileAction;
		QAction *exitAction;
		QMenu *perfMenu;
		QAction *depthPeelAction;

	private slots:
		void newFileActionTriggered();
		void openFileActionTriggered();
		void addFileActionTriggered();
		void saveAllFileActionTriggered();
		void saveAllAsFileActionTriggered();
		void saveSelectedAsFileActionTriggered();
		void importSVGActionTriggered();
		void exportPLYActionTriggered();
		void exportOBJActionTriggered();
		void updateEverything();
		void newGlassColor();
		void newPullPlan();
		void newPullPlan(PullPlan* p);
		void newPiece();
		void copyGlassColor();
		void copyPullPlan();
		void copyPiece();
		void randomSimpleCaneExampleActionTriggered();
		void randomSimplePieceExampleActionTriggered();
		void randomComplexCaneExampleActionTriggered();
		void randomComplexPieceExampleActionTriggered();
		void depthPeelActionTriggered();
		void attemptToQuit();
};

#endif

