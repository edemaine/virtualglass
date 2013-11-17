
#ifndef PIECEEDITORWIDGET_H
#define PIECEEDITORWIDGET_H

#include <QWidget>
#include <QMutex>
#include <QWaitCondition>

#include "pickuptemplate.h"
#include "piecetemplate.h"
#include "geometry.h"
#include "constants.h"

class QGridLayout;
class QVBoxLayout;
class QHBoxLayout;
class QTabWidget;
class QStackedWidget;
class QSpinBox;
class QSlider;
class QPushButton;
class QScrollArea;
class QImage;

class PieceGeometryThread;
class TwistWidget;	
class PieceCustomizeViewWidget;
class PieceLibraryWidget;
class PickupEditorViewWidget;
class NiceViewWidget;
class Piece;
	
class PieceEditorWidget : public QWidget
{
	Q_OBJECT

	friend class PieceGeometryThread;

	public:
		PieceEditorWidget(QWidget* parent=0);
		void resetPiece();
		Piece* piece();
		void updateLibraryWidgetPixmaps(PieceLibraryWidget* w);
		void seedTemplates();
		void setPiece(Piece* p);
		void setPieceTemplateType(enum PieceTemplate::Type t);
		void setPickupTemplateType(enum PickupTemplate::Type t);
		void setPickupParameter(int param, int value);
		void writePieceToPLYFile(QString& filename);
		void writePieceToOBJFile(QString& filename);
		void reset3DCamera();
		QImage pieceImage();
		void undo();
		void redo();
		bool canUndo();
		bool canRedo();

	protected:
		bool eventFilter(QObject* obj, QEvent* event);
		void mousePressEvent(QMouseEvent*);
		void mouseMoveEvent(QMouseEvent*);
		void mouseReleaseEvent(QMouseEvent*);

	signals:
		void someDataChanged();
		void showMessage(const QString& message, unsigned int timeout);

	public slots:
		void updateEverything();

	private slots:
		void pieceControlsTabChanged(int);
		void childWidgetDataChanged();
		void twistEnded();
		void pickupCountSpinChanged(int);
		void geometryThreadFinishedMesh(bool completed, unsigned int quality);
		void addControlPointButtonClicked();
		void removeControlPointButtonClicked();

	private:
		QMutex tempPieceMutex;
		Piece* tempPiece;
		bool tempPieceDirty;

		QWaitCondition wakeWait;
		QMutex wakeMutex;

		PieceGeometryThread* geometryThread;

		QMutex geometryDirtyMutex;
		bool geometryDirty;
		QMutex tempGeometryMutex;
		Geometry tempPieceGeometry;
		Geometry tempPickupGeometry;

		Geometry geometry;

		Piece* _piece;

		PickupEditorViewWidget* pickupViewWidget;	
		PieceCustomizeViewWidget* pieceCustomizeViewWidget;
		NiceViewWidget* pieceNiceViewWidget;
		QStackedWidget* pieceViewStack;

		QPushButton* addControlPointButton;
		QPushButton* removeControlPointButton;

		QSpinBox* pickupCountSpin;

		QTabWidget* pickupControlsTab;
		QTabWidget* pieceControlsTab;
		TwistWidget* twistWidget;
		QScrollArea* pickupTemplateLibraryScrollArea;
		QScrollArea* pieceTemplateLibraryScrollArea;
	
		QHBoxLayout* pickupTemplateLibraryLayout;
		QHBoxLayout* pieceTemplateLibraryLayout;

		bool isDragging;
		bool dragIsPickup;
		QPoint dragStartPosition;
		QPoint lastDragPosition;
		int maxDragDistance;

		void setupLayout();
		void setupThreading();
		void setupConnections();
};

#endif




