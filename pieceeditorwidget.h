
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

class PieceEditorWidget : public QWidget
{
	Q_OBJECT

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

	private:
		Geometry geometry;
		Mesher mesher;
		Piece* piece;
		PickupPlanEditorViewWidget* pickupViewWidget;	
		NiceViewWidget* niceViewWidget;

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
		void setupConnections();
};


#endif

