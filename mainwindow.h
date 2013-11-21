

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <vector> 
#include <stack>

#include <QMainWindow>

#include "undoredo.h"

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

class UndoRedo;
class GlassColor;
class Cane;
class Pickup;
class Piece;
class Email;
class NiceViewWidget;
class CaneEditorWidget;
class ColorEditorWidget;
class PieceEditorWidget;
class GlassColorLibraryWidget;
class CaneLibraryWidget;
class PieceLibraryWidget;
class GlassLibraryWidget;

using std::vector;
using std::stack;

class MainWindow : public QMainWindow
{
	Q_OBJECT

	friend class UndoRedo;

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
			GLASSCOLOR_VIEW_MODE,
			CANE_VIEW_MODE,
			PIECE_VIEW_MODE
		};

		// Methods
		void setViewMode(enum ViewMode m);
		void setupUndoRedo();
		void setupLibrary();
		void setupEditors();
		void setupColorEditor();
		void setupCaneEditor();
		void setupPieceEditor();
		void setupConnections();
		void setupMenus();
		void setupStatusBar();
		void setupSaveFile();
		void deleteCurrentEditingObject();
		void moveCurrentEditingObject(int d);
		void clearLibrary();
		bool glassColorIsDependancy(GlassColor* color);
		bool caneIsDependancy(Cane* cane);
		void updateWindowTitle();
		void setDirtyBit(bool v);
		void setSaveFilename(QString filename);
		void getLibraryContents(vector<GlassColor*>& colors, 
			vector<Cane*>& canes, vector<Piece*>& pieces);
		void getDependantLibraryContents(GlassColor* color, 
			vector<GlassColor*>& colors, vector<Cane*>& canes);
		void getDependantLibraryContents(Cane* cane, 
			vector<GlassColor*>& colors, vector<Cane*>& canes);
		void getDependantLibraryContents(Piece* piece, 
			vector<GlassColor*>& colors, vector<Cane*>& canes, vector<Piece*>& pieces);
		bool findLibraryWidgetData(GlassLibraryWidget* lw, int* type, QVBoxLayout** layout, int* index);

		// Variables
		QScrollArea* libraryScrollArea;
		QVBoxLayout* glassColorLibraryLayout;
		QVBoxLayout* caneLibraryLayout;
		QVBoxLayout* pieceLibraryLayout;
		QStackedWidget* editorStack; //editorStack.currentIndex() gives which mode
		QWidget* centralWidget;
		QHBoxLayout* centralLayout;
		ColorEditorWidget* glassColorEditorWidget;
		CaneEditorWidget* caneEditorWidget;
		PieceEditorWidget* pieceEditorWidget;

		QPushButton* newFileButton;
		QPushButton* openFileButton;
		QPushButton* saveFileButton;
		QPushButton* shareFileButton;

		QPushButton* newGlassColorButton;
		QPushButton* newCaneButton;
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

		bool dirtyBit;
		QString saveFilename;
		bool clickDown;

		Email* email;
		UndoRedo* undoRedo;

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
		void windowedViewActionTriggered();
		void fullscreenViewActionTriggered();
		void randomSimpleCaneExampleActionTriggered();
		void randomSimplePieceExampleActionTriggered();
		void randomComplexCaneExampleActionTriggered();
		void randomComplexPieceExampleActionTriggered();
		void depthPeelActionTriggered();

		// library slots
		void newGlassColorButtonClicked();
		void newCaneButtonClicked();
		void newPieceButtonClicked();
		void editorChangedData();
		void updateLibrary();
		void updateLibrary(GlassColor* gc);
		void updateLibrary(Cane* c);
		void updateLibrary(Piece* p);
		void updateLibraryHighlighting();

		// Status bar slots
		void showStatusMessage(const QString& message, unsigned int timeout);
		void emailSuccess(QString to);
		void emailFailure(QString error);
};

#endif

