
#ifndef PIECEEDITORWIDGET_H
#define PIECEEDITORWIDGET_H

#include <QtGui>
#include "piece.h"
#include "pullplan.h"
#include "pickupplaneditorviewwidget.h"
#include "piecelibrarywidget.h"
#include "pickuptemplatelibrarywidget.h"
#include "piecetemplatelibrarywidget.h"
#include "qgraphicshighlighteffect.h"
#include "niceviewwidget.h"
#include "geometry.h"
#include "mesh.h"

class PieceEditorWidget : public QWidget
{
	Q_OBJECT

	public:
		PieceEditorWidget(QWidget* parent=0);
		Piece* getPiece();
		void updateLibraryWidgetPixmaps(PieceLibraryWidget* w);
		void seedTemplates();
		void setPiece(Piece* p);
		void setPieceTemplate(PieceTemplate* t);
		void setPickupSubplans(PullPlan* s);
		void setPickupTemplateType(int templateType);
		void setPickupParameter(int param, int value);
		void mousePressEvent(QMouseEvent*);

	signals:
		void someDataChanged();

	public slots:
		void updateEverything();

	private slots:
                void pieceTemplateParameterSlider1Changed(int);
                void pieceTemplateParameterSlider2Changed(int);
                void pickupTemplateParameter1SpinBoxChanged(int);

	private:
		Geometry geometry;
		Mesher mesher;
		Piece* piece;
		PickupPlanEditorViewWidget* pickupViewWidget;	
		NiceViewWidget* niceViewWidget;
                QLabel* pieceTemplateParameter1Label;
                QLabel* pieceTemplateParameter2Label;
                QLabel* pickupTemplateParameter1Label;
                QSpinBox* pickupTemplateParameter1SpinBox;
                QSlider* pieceTemplateParameter1Slider;
                QSlider* pieceTemplateParameter2Slider;
                QHBoxLayout* pickupTemplateLibraryLayout;
                QHBoxLayout* pieceTemplateLibraryLayout;
		vector<QLabel*> paramLabels;
		vector<QSpinBox*> paramSpins;
                QButtonGroup* shapeButtonGroup;
		QSlider* casingThicknessSlider;
        	QHBoxLayout* templateLibraryLayout;
		QPushButton* addCasingButton;

		void setupLayout();
		void setupConnections();
		void highlightLibraryWidget(PickupTemplateLibraryWidget* w);
		void unhighlightLibraryWidget(PickupTemplateLibraryWidget* w);
		void highlightLibraryWidget(PieceTemplateLibraryWidget* w);
		void unhighlightLibraryWidget(PieceTemplateLibraryWidget* w);
};


#endif

