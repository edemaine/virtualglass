

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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
class QScrollArea;
class QToolButton;

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
		void keyPressEvent(QKeyEvent* e);
		QString windowTitle();
		void openFile(QString filename, bool add);

	protected:
		void mousePressEvent(QMouseEvent* event);
		void mouseReleaseEvent(QMouseEvent* event);
		void mouseMoveEvent(QMouseEvent* event);
		void closeEvent(QCloseEvent *event);
		void contextMenuEvent(QContextMenuEvent *event);
		bool eventFilter(QObject* obj, QEvent* event);

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
		void setupToolbar();
		void setupLibrary();
		void setupEditors();
		void setupColorEditor();
		void setupPullPlanEditor();
		void setupPieceEditor();
		void setupConnections();
		void setupMenus();
		void setupStatusBar();
		void setupSaveFile();
		void initializeRandomPiece();
		void deleteCurrentEditingObject();
		void moveCurrentEditingObject(int d);
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
		void newGlassColor();
		void newPullPlan();
		void newPiece();
		void copyGlassColor();
		void copyPullPlan();
		void copyPiece();

		// Variables
		QScrollArea* libraryScrollArea;
		QVBoxLayout* colorBarLibraryLayout;
		QVBoxLayout* pullPlanLibraryLayout;
		QVBoxLayout* pieceLibraryLayout;
		QStackedWidget* editorStack; //editorStack.currentIndex() gives which mode
		QWidget* centralWidget;
		QHBoxLayout* centralLayout;
		ColorEditorWidget* colorEditorWidget;
		PullPlanEditorWidget* pullPlanEditorWidget;
		PieceEditorWidget* pieceEditorWidget;

		QToolButton* newFileButton;
		QToolButton* openFileButton;
		QToolButton* saveFileButton;
		QToolButton* shareFileButton;
		QToolButton* exampleCaneButton;
		QToolButton* examplePieceButton;

		QPushButton* newObjectButton;
		QPushButton* copyObjectButton;
		QPushButton* deleteObjectButton;

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

		bool isDragging;
		QPoint dragStartPosition;
		QPoint lastDragPosition;
		int maxDragDistance;

		bool dirtyBit;
		QString saveFilename;

	private slots:
		void setDirtyBitTrue();
		void newFileActionTriggered();
		void openFileActionTriggered();
		void addFileActionTriggered();
		void saveAllFileActionTriggered();
		void saveAllAsFileActionTriggered();
		void saveSelectedAsFileActionTriggered();
		void shareFileActionTriggered();
		void importSVGActionTriggered();
		void exportPLYActionTriggered();
		void exportOBJActionTriggered();
		void updateLibrary();
		void newObject();
		void copyObject();
		void deleteObject();
		void randomSimpleCaneExampleActionTriggered();
		void randomSimplePieceExampleActionTriggered();
		void randomComplexCaneExampleActionTriggered();
		void randomComplexPieceExampleActionTriggered();
		void depthPeelActionTriggered();
		void attemptToQuit();
		void showStatusMessage(const QString& message, unsigned int timeout);
};

#endif

