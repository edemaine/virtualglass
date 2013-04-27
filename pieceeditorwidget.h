
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
class QSlider;
class QPushButton;

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
		void mousePressEvent(QMouseEvent*);
		void writePieceToPLYFile(QString& filename);
		void writePieceToOBJFile(QString& filename);

	signals:
		void someDataChanged();
		void showMessage(const QString& message);

	public slots:
		void updateEverything();

	private slots:
		void pieceControlsTabChanged(int);
		void childWidgetDataChanged();
		void pickupParameterSpinBoxChanged(int);
		void pickupParameterSliderChanged(int);
		void geometryThreadFinishedMesh(bool completed);
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
		vector<QStackedWidget*> pickupParamStacks;
		vector<QLabel*> pickupParamLabels;
		vector<QSpinBox*> pickupParamSpinBoxes;
		vector<QSlider*> pickupParamSliders;
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
};

#endif




