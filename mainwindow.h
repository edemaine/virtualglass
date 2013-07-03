

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

class GlassColor;
class PullPlan;
class PickupPlan;
class Piece;
class Email;
class NiceViewWidget;
class PullPlanEditorWidget;
class ColorEditorWidget;
class PieceEditorWidget;
class GlassColorLibraryWidget;
class PullPlanLibraryWidget;
class PieceLibraryWidget;
class GlassLibraryWidget;

using std::vector;

class MainWindow : public QMainWindow
{
	Q_OBJECT

	public:
		MainWindow();
		void keyPressEvent(QKeyEvent* e);
		QString windowTitle();
		void openFile(QString filename, bool add);
		void copyLibraryWidget(GlassLibraryWidget* w);
		void deleteLibraryWidget(GlassLibraryWidget* w);
		void setEditorLibraryWidget(GlassLibraryWidget* w);
		bool hasNoDependancies(GlassLibraryWidget* w);

	protected:
		void closeEvent(QCloseEvent *event);
		void contextMenuEvent(QContextMenuEvent *event);
		bool eventFilter(QObject* obj, QEvent* event);

	signals:
		void someDataChanged();

	private:
		enum ViewMode
		{
			EMPTY_VIEW_MODE=0, // must start at 0 to match usage as QStackedWidget index
			COLORBAR_VIEW_MODE,
			PULLPLAN_VIEW_MODE,
			PIECE_VIEW_MODE
		};

		// Methods
		void setViewMode(enum ViewMode m);
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
		void clearLibrary();
		bool glassColorIsDependancy(GlassColor* color);
		bool pullPlanIsDependancy(PullPlan* plan);
		void updateWindowTitle();
		void setDirtyBit(bool v);
		void setSaveFilename(QString filename);
		void getLibraryContents(vector<GlassColor*>& colors, 
			vector<PullPlan*>& plans, vector<Piece*>& pieces);
		void getDependantLibraryContents(GlassColor* color, 
			vector<GlassColor*>& colors, vector<PullPlan*>& plans);
		void getDependantLibraryContents(PullPlan* plan, 
			vector<GlassColor*>& colors, vector<PullPlan*>& plans);
		void getDependantLibraryContents(Piece* piece, 
			vector<GlassColor*>& colors, vector<PullPlan*>& plans, vector<Piece*>& pieces);
		void addToLibrary(vector<GlassColor*>& colors, vector<PullPlan*>& plans, vector<Piece*>& pieces);
		bool findLibraryWidgetData(GlassLibraryWidget* lw, int* type, QVBoxLayout** layout, int* index);

		// Variables
		QScrollArea* libraryScrollArea;
		QVBoxLayout* glassColorLibraryLayout;
		QVBoxLayout* pullPlanLibraryLayout;
		QVBoxLayout* pieceLibraryLayout;
		QStackedWidget* editorStack; //editorStack.currentIndex() gives which mode
		QWidget* centralWidget;
		QHBoxLayout* centralLayout;
		ColorEditorWidget* glassColorEditorWidget;
		PullPlanEditorWidget* pullPlanEditorWidget;
		PieceEditorWidget* pieceEditorWidget;

		QPushButton* newFileButton;
		QPushButton* openFileButton;
		QPushButton* saveFileButton;
		QPushButton* shareFileButton;

		QPushButton* newGlassColorButton;
		QPushButton* newPullPlanButton;
		QPushButton* newPieceButton;

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

		QMenu* editMenu;
		QAction* undoAction;
		QAction* redoAction;

		QMenu* viewMenu;
		QAction *fullscreenViewAction;
		QAction *windowedViewAction;

		QMenu *examplesMenu;
		QAction *randomSimpleCaneAction;
		QAction *randomSimplePieceAction;
		QAction *randomComplexCaneAction;
		QAction *randomComplexPieceAction;

		QMenu *perfMenu;
		QAction *depthPeelAction;

		bool isDragging;
		QPoint dragStartPosition;
		int maxDragDistance;

		bool dirtyBit;
		QString saveFilename;

		Email* email;

	private slots:

		void setDirtyBitTrue();
		void attemptToQuit();

		// menu slots
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
		void undoActionTriggered();
		void redoActionTriggered();
		void windowedViewActionTriggered();
		void fullscreenViewActionTriggered();
		void randomSimpleCaneExampleActionTriggered();
		void randomSimplePieceExampleActionTriggered();
		void randomComplexCaneExampleActionTriggered();
		void randomComplexPieceExampleActionTriggered();
		void depthPeelActionTriggered();

		// library slots
		void newGlassColorButtonClicked();
		void newPullPlanButtonClicked();
		void newPieceButtonClicked();
		void updateLibrary();

		// Status bar slots
		void showStatusMessage(const QString& message, unsigned int timeout);
		void emailSuccess(QString to);
		void emailFailure(QString error);
};

#endif

