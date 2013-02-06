
#ifndef PIECEEDITORWIDGET_H
#define PIECEEDITORWIDGET_H

#include <QtGui>
#include "piece.h"
#include "pullplan.h"
#include "pickupplaneditorviewwidget.h"
#include "asyncpiecelibrarywidget.h"
#include "pickuptemplatelibrarywidget.h"
#include "piecetemplatelibrarywidget.h"
#include "niceviewwidget.h"
#include "geometry.h"
#include "mesh.h"

class PieceGeometryThread;
		
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


	signals:
		void someDataChanged();

	public slots:
		void updateEverything();

	private slots:
		void pickupViewWidgetDataChanged();
		void pieceParameterSliderChanged(int);
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

		QMutex tempGeometry1Mutex;
		QMutex tempGeometry2Mutex;
		Geometry tempGeometry1;
		Geometry tempGeometry2;

		Geometry geometry;

		Piece* piece;
		NiceViewWidget* niceViewWidget;
		PickupPlanEditorViewWidget* pickupViewWidget;	

		vector<QLabel*> pickupParamLabels;
		vector<QSpinBox*> pickupParamSpinboxes;
		vector<QSlider*> pickupParamSliders;
		vector<QStackedWidget*> pickupParamStacks;
		vector<QLabel*> pieceParamLabels;
		vector<QSlider*> pieceParamSliders;
		vector<QStackedWidget*> pieceParamStacks;

		QHBoxLayout* pickupTemplateLibraryLayout;
		QHBoxLayout* pieceTemplateLibraryLayout;
		QButtonGroup* shapeButtonGroup;
		QSlider* casingThicknessSlider;
		QHBoxLayout* templateLibraryLayout;
		QPushButton* addCasingButton;

		void setupLayout();
		void setupThreading();
		void setupConnections();

};

class PieceEditorWidget;

class PieceGeometryThread : public QThread
{
	Q_OBJECT

	public:
		PieceGeometryThread(PieceEditorWidget* pew);
		void run();
		PieceEditorWidget* pew;
		PieceEditorWidget* ppew;

	signals:
		void finishedMesh();
};

#endif




