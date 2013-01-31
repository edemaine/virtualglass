
#ifndef PIECEEDITORWIDGET_H
#define PIECEEDITORWIDGET_H

#include <QtGui>
#include "mainwindow.h"
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
		Piece* getPiece();
		void updateLibraryWidgetPixmaps(AsyncPieceLibraryWidget* w);
		void seedTemplates();
		void setPiece(Piece* p);
		void setPieceTemplateType(enum PieceTemplate::Type t);
		void setPickupTemplateType(enum PickupTemplate::Type t);
		void setPickupParameter(int param, int value);
		void mousePressEvent(QMouseEvent*);
		void setMainWindow(MainWindow*);

	signals:
		void someDataChanged();

	public slots:
		void updateEverything();

	private slots:
		void pickupViewWidgetDataChanged();
		void pieceParameterSliderChanged(int);
		void pickupParameterSpinBoxChanged(int);
		void pickupParameterSliderChanged(int);
		void addLayer();
		void removeLayer();
		void changeLayer();
		void combineLayers();
		void viewAllPieces();
		//	int calculatePullVertices(PullPlan*, int);
		//void recurseLayers(vector<PickupPlan*>, vector<int>, int);

	private:
		Geometry geometry;
		Mesher mesher;
		Piece* piece;
		Piece* allLayersPiece;
		PickupPlanEditorViewWidget* pickupViewWidget;	
		NiceViewWidget* niceViewWidget;
		MainWindow* MainWin;

		vector<QLabel*> pickupParamLabels;
		vector<QSpinBox*> pickupParamSpinboxes;
		vector<QSlider*> pickupParamSliders;
		vector<QStackedWidget*> pickupParamStacks;
		vector<QLabel*> pieceParamLabels;
		vector<QSlider*> pieceParamSliders;
		vector<QStackedWidget*> pieceParamStacks;
		vector<PickupPlan> vecLayerComboBox;

		QHBoxLayout* pickupTemplateLibraryLayout;
		QHBoxLayout* pieceTemplateLibraryLayout;
		QHBoxLayout* pickupLayerViewLayout;
		QHBoxLayout* templateLibraryLayout;
		QButtonGroup* shapeButtonGroup;
		QSlider* casingThicknessSlider;
		QPushButton* addCasingButton;
		QPushButton* addLayerButton;
		QPushButton* removeLayerButton;
		QComboBox* layerComboBox;
		QCheckBox* viewCheckBox;
		QLabel* layerLabel;

		void setupLayout();
		void setupConnections();
};


#endif

