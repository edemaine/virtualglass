
#ifndef PIECEEDITORWIDGET_H
#define PIECEEDITORWIDGET_H

#include <QWidget>
#include <QMutex>
#include <QWaitCondition>

#include "pickuptemplate.h"
#include "piecetemplate.h"
#include "geometry.h"
#include "constants.h"

class QLabel;
class QGridLayout;
class QVBoxLayout;
class QHBoxLayout;
class QTabWidget;
class QStackedWidget;
class QSpinBox;
class QSlider;
class QPushButton;
class QScrollArea;

class PieceGeometryThread;
class TwistWidget;	
class PieceCustomizeViewWidget;
class PieceLibraryWidget;
class PickupPlanEditorViewWidget;
class NiceViewWidget;
class Piece;
	
class PieceEditorWidget : public QWidget
{
	Q_OBJECT

	friend class PieceGeometryThread;

	public:
		PieceEditorWidget(QWidget* parent=0);
		void resetPiece();
		Piece* getPiece();
		void updateLibraryWidgetPixmaps(PieceLibraryWidget* w);
		void seedTemplates();
		void setPiece(Piece* p);
		void setPieceTemplateType(enum PieceTemplate::Type t);
		void setPickupTemplateType(enum PickupTemplate::Type t);
		void setPickupParameter(int param, int value);
		void writePieceToPLYFile(QString& filename);
		void writePieceToOBJFile(QString& filename);

	protected:
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
		void pickupCountMinusButtonClicked();
		void pickupCountSpinBoxChanged(int);
		void pickupCountPlusButtonClicked();
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

		Piece* piece;

		PickupPlanEditorViewWidget* pickupViewWidget;	
		NiceViewWidget* pieceNiceViewWidget;
		PieceCustomizeViewWidget* pieceCustomizeViewWidget;
		QStackedWidget* pieceViewStack;

		QPushButton* addControlPointButton;
		QPushButton* removeControlPointButton;

		QPushButton* pickupCountMinusButton;
		QSpinBox* pickupCountSpinBox;
		QPushButton* pickupCountPlusButton;

		QTabWidget* pickupControlsTab;
		QTabWidget* pieceControlsTab;
		TwistWidget* twistWidget;
		QLabel* pieceEditorDescriptionLabel;
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




