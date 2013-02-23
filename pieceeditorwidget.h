
#ifndef PIECEEDITORWIDGET_H
#define PIECEEDITORWIDGET_H

#include <QWidget>
#include <QMutex>
#include <QWaitCondition>
#include "pickuptemplate.h"
#include "piecetemplate.h"
#include "geometry.h"

class QLabel;
class QGridLayout;
class QVBoxLayout;
class QHBoxLayout;
class QTabWidget;
class QStackedWidget;
class QSpinBox;
class QDoubleSpinBox;
class QSlider;

class PieceGeometryThread;
class TwistWidget;	
class PieceCustomizeViewWidget;
class AsyncPieceLibraryWidget;
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
		void updateLibraryWidgetPixmaps(AsyncPieceLibraryWidget* w);
		void seedTemplates();
		void setPiece(Piece* p);
		void setPieceTemplateType(enum PieceTemplate::Type t);
		void setPickupTemplateType(enum PickupTemplate::Type t);
		void setPickupParameter(int param, int value);
		void mousePressEvent(QMouseEvent*);
		void writePieceToPLYFile(QString& filename);
		void writePieceToOBJFile(QString& filename);

	signals:
		void someDataChanged();

	public slots:
		void updateEverything();

	private slots:
		void pieceControlsTabChanged(int);
		void childWidgetDataChanged();
		void pieceSplineSpinBoxChanged(double);
		void pickupParameterSpinBoxChanged(int);
		void pickupParameterSliderChanged(int);
		void geometryThreadFinishedMesh();

	private:
		QMutex tempPieceMutex;
		Piece* tempPiece;
		bool tempPieceDirty;

		QWaitCondition wakeWait;
		QMutex wakeMutex;

		PieceGeometryThread* geometryThread;

		QMutex geometryDirtyMutex;
		bool geometryDirty;
		QMutex tempGeometry1Mutex;
		QMutex tempGeometry2Mutex;
		Geometry tempGeometry1;
		Geometry tempGeometry2;

		Geometry geometry;

		Piece* piece;

		PickupPlanEditorViewWidget* pickupViewWidget;	
		NiceViewWidget* pieceNiceViewWidget;
		PieceCustomizeViewWidget* pieceCustomizeViewWidget;
                QStackedWidget* pieceViewStack;

		vector<QStackedWidget*> pickupParamStacks;
		vector<QLabel*> pickupParamLabels;
		vector<QSpinBox*> pickupParamSpinBoxes;
		vector<QSlider*> pickupParamSliders;
		vector<QDoubleSpinBox*> pieceSplineSpins;
		QTabWidget* pickupControlsTab;
		QTabWidget* pieceControlsTab;
		TwistWidget* twistWidget;
		QLabel* pieceEditorDescriptionLabel;
	
		QHBoxLayout* pickupTemplateLibraryLayout;
		QHBoxLayout* pieceTemplateLibraryLayout;

		void setupLayout();
		void setupThreading();
		void setupConnections();
		void addPickupParam(QVBoxLayout* pickupParamLayout);
		void addPieceSpline(QGridLayout* pieceParamLayout);
};

#endif




