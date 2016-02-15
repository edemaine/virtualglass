
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
		Piece* piece() const;
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
	
	public slots:
		void updateEverything();

	protected:
		bool eventFilter(QObject* obj, QEvent* event);
		void mousePressEvent(QMouseEvent*);
		void mouseMoveEvent(QMouseEvent*);
		void mouseReleaseEvent(QMouseEvent*);

	signals:
		void showMessage(const QString& message, unsigned int timeout);

	private slots:
		void pieceControlsTabChanged(int);
		void twistEnded();
		void pickupCountSpinChanged(int);
		void geometryThreadFinishedMesh(bool completed, unsigned int quality);
		void addControlPointButtonClicked();
		void deleteControlPointButtonClicked();

	private:
		Piece* piece_;

		Piece* tempPiece_;
		QMutex tempPieceMutex;
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


		PickupEditorViewWidget* pickupViewWidget;	
		PieceCustomizeViewWidget* pieceCustomizeViewWidget;
		NiceViewWidget* pieceNiceViewWidget;
		QStackedWidget* pieceViewStack;

		QPushButton* addControlPointButton;
		QPushButton* deleteControlPointButton;

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




