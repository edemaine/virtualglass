
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
		void underlayCheckBoxChanged(int);
		void fillComboBoxChanged(int);
                void pieceTemplateParameterSlider1Changed(int);
                void pieceTemplateParameterSlider2Changed(int);
                void pieceTemplateParameterSlider3Changed(int);
                void pickupParameter1SpinBoxChanged(int);
                void pickupParameter1SliderChanged(int);

	private:
		Geometry geometry;
		Mesher mesher;
		Piece* piece;
		PickupPlanEditorViewWidget* pickupViewWidget;	
		NiceViewWidget* niceViewWidget;

		QCheckBox* underlayCheckBox;
		QComboBox* fillComboBox;
                QLabel* pickupTemplateParameter1Label;
                QSpinBox* pickupParameter1SpinBox;
                QSlider* pickupParameter1Slider;
                QSlider* pickupTemplateParameter1Slider;

                QLabel* pieceTemplateParameter1Label;
                QLabel* pieceTemplateParameter2Label;
                QLabel* pieceTemplateParameter3Label;
                QSlider* pieceTemplateParameter1Slider;
                QSlider* pieceTemplateParameter2Slider;
                QSlider* pieceTemplateParameter3Slider;
		vector<QLabel*> pieceParamLabels;
		vector<QSlider*> pieceParamSliders;

                QHBoxLayout* pickupTemplateLibraryLayout;
                QHBoxLayout* pieceTemplateLibraryLayout;
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

