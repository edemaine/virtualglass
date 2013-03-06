
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTabWidget>
#include <QStackedWidget>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QSlider>
#include <QScrollArea>
#include <QMouseEvent>
#include <QPushButton>

#include "pickupplaneditorviewwidget.h"
#include "piece.h"
#include "pieceeditorwidget.h"
#include "piecegeometrythread.h"
#include "twistwidget.h"
#include "piececustomizeviewwidget.h"
#include "piecetemplatelibrarywidget.h"
#include "pickuptemplatelibrarywidget.h"
#include "piecelibrarywidget.h"
#include "niceviewwidget.h"
#include "piececrosssectionrender.h"
#include "globalbackgroundcolor.h"

PieceEditorWidget :: PieceEditorWidget(QWidget* parent) : QWidget(parent)
{
	resetPiece();

	this->pickupViewWidget = new PickupPlanEditorViewWidget(piece->pickup, this);	
	this->pieceNiceViewWidget = new NiceViewWidget(NiceViewWidget::PIECE_CAMERA_MODE, this);
	pieceNiceViewWidget->setGeometry(&geometry);
	this->pieceCustomizeViewWidget = new PieceCustomizeViewWidget(piece, this);

	setupLayout();
	setupThreading();
	setupConnections();
}

void PieceEditorWidget :: resetPiece()
{
	piece = new Piece(PieceTemplate::TUMBLER);
}

void PieceEditorWidget :: writePieceToOBJFile(QString& filename)
{
	geometry.save_obj_file(filename.toStdString());
}

void PieceEditorWidget :: writePieceToPLYFile(QString& filename)
{
	geometry.save_ply_file(filename.toStdString());
}

void PieceEditorWidget :: updateEverything()
{
	// update pickup stuff
	PickupTemplateLibraryWidget* pktlw;
	for (int i = 0; i < pickupTemplateLibraryLayout->count(); ++i)
	{
		pktlw = dynamic_cast<PickupTemplateLibraryWidget*>(
				dynamic_cast<QWidgetItem *>(pickupTemplateLibraryLayout->itemAt(i))->widget());
		pktlw->setHighlighted(pktlw->type == piece->pickup->getTemplateType());
	}

	pickupViewWidget->updateEverything();

	unsigned int i = 0;
	for (; i < piece->pickup->getParameterCount(); ++i)
	{
		// get parameter info
		TemplateParameter tp;
		piece->pickup->getParameter(i, &tp);
		pickupParamLabels[i]->setText(tp.name.c_str());

		pickupParamSpinBoxes[i]->blockSignals(true);
		pickupParamSliders[i]->blockSignals(true);
		pickupParamSpinBoxes[i]->setRange(tp.lowerLimit, tp.upperLimit);
		pickupParamSpinBoxes[i]->setValue(tp.value);
		pickupParamSliders[i]->setRange(tp.lowerLimit, tp.upperLimit);
		pickupParamSliders[i]->setValue(tp.value);
		pickupParamSpinBoxes[i]->blockSignals(false);
		pickupParamSliders[i]->blockSignals(false);

		pickupParamStacks[i]->setCurrentIndex(0); // show
	}
	for (; i < pickupParamStacks.size(); ++i)
	{
		pickupParamStacks[i]->setCurrentIndex(1); // hide
	}

	// update piece stuff
	pieceCustomizeViewWidget->updateEverything();	

	PieceTemplateLibraryWidget* ptlw;
	for (int i = 0; i < pieceTemplateLibraryLayout->count(); ++i)
	{
		ptlw = dynamic_cast<PieceTemplateLibraryWidget*>(
			dynamic_cast<QWidgetItem *>(pieceTemplateLibraryLayout->itemAt(i))->widget());
		ptlw->setHighlighted(ptlw->type == piece->getTemplateType());
	}
	
	tempPieceMutex.lock();
	deep_delete(tempPiece);
	tempPiece = deep_copy(piece);
	tempPieceDirty = true;
	tempPieceMutex.unlock();
	wakeWait.wakeOne();
}

void PieceEditorWidget :: geometryThreadFinishedMesh()
{
	geometryDirtyMutex.lock();
	bool dirty = geometryDirty;
	geometryDirtyMutex.unlock();
	if (!dirty)
		return;

	if (tempGeometry1Mutex.tryLock())
	{
		geometryDirtyMutex.lock();
		geometryDirty = false;
		geometryDirtyMutex.unlock();
		geometry.vertices = tempPieceGeometry1.vertices;
		geometry.triangles = tempPieceGeometry1.triangles;
		geometry.groups = tempPieceGeometry1.groups;
		pickupViewWidget->geometry.vertices = tempPickupGeometry1.vertices;
		pickupViewWidget->geometry.triangles = tempPickupGeometry1.triangles;
		pickupViewWidget->geometry.groups = tempPickupGeometry1.groups;
		tempGeometry1Mutex.unlock();
	}
	else if (tempGeometry2Mutex.tryLock())
	{
		geometryDirtyMutex.lock();
		geometryDirty = false;
		geometryDirtyMutex.unlock();
		geometry.vertices = tempPieceGeometry2.vertices;
		geometry.triangles = tempPieceGeometry2.triangles;
		geometry.groups = tempPieceGeometry2.groups;
		pickupViewWidget->geometry.vertices = tempPickupGeometry2.vertices;
		pickupViewWidget->geometry.triangles = tempPickupGeometry2.triangles;
		pickupViewWidget->geometry.groups = tempPickupGeometry2.groups;
		tempGeometry2Mutex.unlock();
	}
	// else: this might happen if we get incredibly unlucky:
	// We try lock 1 while the geometry thread has it, 
	// and before we can try lock 2, the geometry thread unlocks 1
	// and takes lock 2.
	// 
	// Because the goal is to have a non-blocking GUI thread *and* geometry
	// thread that continously writes new geometry regardless of whether the
	// GUI thread read the last geometry, we can never *ensure* that we can read. 

	pickupViewWidget->updateEverything();
	pieceNiceViewWidget->repaint();
}

void PieceEditorWidget :: pickupParameterSpinBoxChanged(int)
{
	bool pieceChanged = false;
	for (unsigned int i = 0; i < piece->pickup->getParameterCount(); ++i)
	{
		TemplateParameter tp;
		piece->pickup->getParameter(i, &tp);
		if (tp.value != pickupParamSpinBoxes[i]->value())
		{
			piece->pickup->setParameter(i, pickupParamSpinBoxes[i]->value());
			pieceChanged = true;
		}
	}
	if (pieceChanged)
		emit someDataChanged();
}

void PieceEditorWidget :: pickupParameterSliderChanged(int)
{
	bool pieceChanged = false;
	for (unsigned int i = 0; i < piece->pickup->getParameterCount(); ++i)
	{
		TemplateParameter tp;
		piece->pickup->getParameter(i, &tp);
		if (tp.value != pickupParamSliders[i]->value())
		{
			piece->pickup->setParameter(i, pickupParamSliders[i]->value());
			pieceChanged = true;
		}
	}
	if (pieceChanged)
		emit someDataChanged();
}

void PieceEditorWidget :: addPickupParam(QVBoxLayout* pickupParamLayout)
{
	pickupParamStacks.push_back(new QStackedWidget(this));
	QWidget* pickupParameterWidget = new QWidget(pickupParamStacks.back());
	QLabel* pickupParameterLabel = new QLabel("???");
	pickupParamLabels.push_back(pickupParameterLabel);
	QSpinBox* pickupParameterSpinBox = new QSpinBox(pickupParameterWidget);
	pickupParamSpinBoxes.push_back(pickupParameterSpinBox);
	QSlider* pickupParameterSlider = new QSlider(Qt::Horizontal, pickupParameterWidget);
	pickupParamSliders.push_back(pickupParameterSlider);
	QHBoxLayout* pickupParameterLayout = new QHBoxLayout(pickupParameterWidget);
	pickupParameterWidget->setLayout(pickupParameterLayout);
	pickupParameterLayout->addWidget(pickupParameterLabel);
	pickupParameterLayout->addWidget(pickupParameterSpinBox);
	pickupParameterLayout->addWidget(pickupParameterSlider);
	pickupParamStacks.back()->addWidget(pickupParameterWidget);
	pickupParamStacks.back()->addWidget(new QWidget(pickupParamStacks.back()));
	pickupParamLayout->addWidget(pickupParamStacks.back());
}

void PieceEditorWidget :: setupLayout()
{
	QGridLayout* editorLayout = new QGridLayout(this);
	this->setLayout(editorLayout);

	// two 3D views in the first row (and stretched to take up all the slack space 
	editorLayout->addWidget(pickupViewWidget, 0, 0); 
	pieceViewStack = new QStackedWidget(this);
	pieceViewStack->addWidget(pieceNiceViewWidget);
	pieceViewStack->addWidget(pieceCustomizeViewWidget);
	editorLayout->addWidget(pieceViewStack, 0, 1);
	editorLayout->setRowStretch(0, 10);

	// pickup template and piece template selectors in the second row
	QWidget* pickupTemplateLibraryWidget = new QWidget(this);
	pickupTemplateLibraryLayout = new QHBoxLayout(pickupTemplateLibraryWidget);
	pickupTemplateLibraryLayout->setSpacing(10);
	pickupTemplateLibraryLayout->setContentsMargins(10, 10, 10, 10);
	pickupTemplateLibraryWidget->setLayout(pickupTemplateLibraryLayout);

	QScrollArea* pickupTemplateLibraryScrollArea = new QScrollArea(this);
	pickupTemplateLibraryScrollArea->setBackgroundRole(QPalette::Dark);
	pickupTemplateLibraryScrollArea->setWidget(pickupTemplateLibraryWidget);
	pickupTemplateLibraryScrollArea->setWidgetResizable(true);
	pickupTemplateLibraryScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	pickupTemplateLibraryScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	pickupTemplateLibraryScrollArea->setFixedHeight(140);
	editorLayout->addWidget(pickupTemplateLibraryScrollArea, 1, 0);

	QWidget* pieceTemplateLibraryWidget = new QWidget(this);
	pieceTemplateLibraryLayout = new QHBoxLayout(pieceTemplateLibraryWidget);
	pieceTemplateLibraryLayout->setSpacing(10);
	pieceTemplateLibraryLayout->setContentsMargins(10, 10, 10, 10);
	pieceTemplateLibraryWidget->setLayout(pieceTemplateLibraryLayout);

	QScrollArea* pieceTemplateLibraryScrollArea = new QScrollArea(this);
	pieceTemplateLibraryScrollArea->setBackgroundRole(QPalette::Dark);
	pieceTemplateLibraryScrollArea->setWidget(pieceTemplateLibraryWidget);
	pieceTemplateLibraryScrollArea->setWidgetResizable(true);
	pieceTemplateLibraryScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	pieceTemplateLibraryScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	pieceTemplateLibraryScrollArea->setFixedHeight(140);
	editorLayout->addWidget(pieceTemplateLibraryScrollArea, 1, 1);

	// layouts containing pickup and piece template parameters in the third row

	// Pickup parameter layout
	pickupControlsTab = new QTabWidget(this);
	editorLayout->addWidget(pickupControlsTab, 2, 0);

	QWidget* pickupParamWidget = new QWidget(pickupControlsTab);
	QVBoxLayout* pickupParamLayout = new QVBoxLayout(pickupParamWidget);
	pickupParamWidget->setLayout(pickupParamLayout);
	addPickupParam(pickupParamLayout);
	addPickupParam(pickupParamLayout);
	pickupControlsTab->addTab(pickupParamWidget, "Fill and Case");

	QLabel* pickupEditorDescriptionLabel = new QLabel("Pickup editor - drag canes in.", this);
	pickupEditorDescriptionLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	editorLayout->addWidget(pickupEditorDescriptionLabel, 3, 0);
	
	// Piece parameter layout 
	pieceControlsTab = new QTabWidget(this);
	editorLayout->addWidget(pieceControlsTab, 2, 1);

	QWidget* tab1Widget = new QWidget(pieceControlsTab);
	QVBoxLayout* tab1Layout = new QVBoxLayout(tab1Widget);
	tab1Widget->setLayout(tab1Layout); 
	twistWidget = new TwistWidget(&(piece->twist), 3.0, pieceControlsTab);
	tab1Layout->addWidget(twistWidget, 0);
	tab1Layout->addStretch(10);
	pieceControlsTab->addTab(tab1Widget, "Twist");

	QWidget* tab2Widget = new QWidget(pieceControlsTab);
	QGridLayout* splineParamLayout = new QGridLayout(tab2Widget);
	tab2Widget->setLayout(splineParamLayout);
	addControlPointButton = new QPushButton("Add control point", tab2Widget);
	removeControlPointButton = new QPushButton("Remove control point", tab2Widget);
	splineParamLayout->addWidget(addControlPointButton, 0, 0);
	splineParamLayout->addWidget(removeControlPointButton, 0, 1);
	splineParamLayout->setRowStretch(1, 1);
	splineParamLayout->setColumnStretch(2, 1);
	pieceControlsTab->addTab(tab2Widget, "Customize");

	pieceEditorDescriptionLabel = new QLabel("Piece editor.", this);
	pieceEditorDescriptionLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	editorLayout->addWidget(pieceEditorDescriptionLabel, 3, 1);
}

void PieceEditorWidget :: mousePressEvent(QMouseEvent* event)
{
	PickupTemplateLibraryWidget* pktlw = dynamic_cast<PickupTemplateLibraryWidget*>(childAt(event->pos()));
	PieceTemplateLibraryWidget* ptlw = dynamic_cast<PieceTemplateLibraryWidget*>(childAt(event->pos()));

	if (pktlw != NULL)
	{
		if (pktlw->type != piece->pickup->getTemplateType())
		{
			piece->pickup->setTemplateType(pktlw->type);
			emit someDataChanged();
		}
	}
	else if (ptlw != NULL)
	{
		if (ptlw->type == PieceTemplate::CUSTOM)
			pieceControlsTab->setCurrentIndex(1);
		else
		{
			pieceControlsTab->setCurrentIndex(0);
			piece->setTemplateType(ptlw->type);
			emit someDataChanged();
		}
	}
}


void PieceEditorWidget :: setupThreading()
{
	geometryDirty = false;
	tempPiece = deep_copy(piece);
	tempPieceDirty = true;
	geometryThread = new PieceGeometryThread(this);
	geometryThread->start();	
}

void PieceEditorWidget :: setupConnections()
{
	for (unsigned int i = 0; i < pickupParamSpinBoxes.size(); ++i)
	{
		connect(pickupParamSpinBoxes[i], SIGNAL(valueChanged(int)),
			this, SLOT(pickupParameterSpinBoxChanged(int)));
	}
	for (unsigned int i = 0; i < pickupParamSliders.size(); ++i)
	{
		connect(pickupParamSliders[i], SIGNAL(valueChanged(int)),
			this, SLOT(pickupParameterSliderChanged(int)));
	}
	connect(addControlPointButton, SIGNAL(clicked()), this, SLOT(addControlPointButtonClicked()));
	connect(removeControlPointButton, SIGNAL(clicked()), this, SLOT(removeControlPointButtonClicked()));
	
	connect(geometryThread, SIGNAL(finishedMesh()), this, SLOT(geometryThreadFinishedMesh()));
	connect(pickupViewWidget, SIGNAL(someDataChanged()), this, SLOT(childWidgetDataChanged()));
	connect(twistWidget, SIGNAL(valueChanged()), this, SLOT(childWidgetDataChanged()));
	connect(pieceCustomizeViewWidget, SIGNAL(someDataChanged()), this, SLOT(childWidgetDataChanged()));
	connect(pieceControlsTab, SIGNAL(currentChanged(int)), this, SLOT(pieceControlsTabChanged(int)));
	connect(this, SIGNAL(someDataChanged()), this, SLOT(updateEverything()));
}

void PieceEditorWidget :: addControlPointButtonClicked()
{
	piece->spline.addPoint(Point2D(make_vector(0.0f, 0.0f)));
	emit someDataChanged();
}

void PieceEditorWidget :: removeControlPointButtonClicked()
{
	piece->spline.removePoint();
	emit someDataChanged();
}

void PieceEditorWidget :: pieceControlsTabChanged(int tab)
{
	// change the view to match the tab
        pieceViewStack->setCurrentIndex(tab);

	if (tab == 0) // Twist mode
		pieceEditorDescriptionLabel->setText("Piece editor.");
	else // customize mode
	{
		piece->setTemplateType(PieceTemplate::CUSTOM);
		pieceEditorDescriptionLabel->setText("Piece customizer.");
		emit someDataChanged();
	}
}

void PieceEditorWidget :: childWidgetDataChanged()
{
	emit someDataChanged();
}

void PieceEditorWidget :: seedTemplates()
{
	char filename[100];

	for (int i = PickupTemplate::firstSeedTemplate(); i <= PickupTemplate::lastSeedTemplate(); ++i)
	{
		sprintf(filename, ":/images/pickuptemplate%d.png", i);
		PickupTemplateLibraryWidget *pktlw = new PickupTemplateLibraryWidget(
			QPixmap::fromImage(QImage(filename)), static_cast<PickupTemplate::Type>(i));
		pickupTemplateLibraryLayout->addWidget(pktlw);
	}

	for (int i = PieceTemplate::firstSeedTemplate(); i <= PieceTemplate::lastSeedTemplate(); ++i)
	{
		PieceTemplate::Type t = static_cast<PieceTemplate::Type>(i);
		Piece piece(t);

		QPixmap templatePixmap(100, 100);
		templatePixmap.fill(GlobalBackgroundColor::qcolor);

		QPainter painter(&templatePixmap);
		PieceCrossSectionRender::render(&painter, 100, &piece);
		painter.end();

		PieceTemplateLibraryWidget *ptlw = new PieceTemplateLibraryWidget(templatePixmap, t);
		pieceTemplateLibraryLayout->addWidget(ptlw);
	}
	PieceTemplateLibraryWidget *ptlw = new PieceTemplateLibraryWidget(
		QPixmap::fromImage(QImage(":/images/piecetemplate_custom.png")), 
		static_cast<PieceTemplate::Type>(PieceTemplate::CUSTOM));
	pieceTemplateLibraryLayout->addWidget(ptlw);
	
}

void PieceEditorWidget :: updateLibraryWidgetPixmaps(PieceLibraryWidget* w)
{
	w->updatePixmap();
}

void PieceEditorWidget :: setPickupParameter(int param, int value)
{
	piece->pickup->setParameter(param, value);
	emit someDataChanged();
}

void PieceEditorWidget :: setPieceTemplateType(enum PieceTemplate::Type _type)
{
	piece->setTemplateType(_type);
	emit someDataChanged();
}

void PieceEditorWidget :: setPickupTemplateType(enum PickupTemplate::Type _type)
{
	piece->pickup->setTemplateType(_type);
	emit someDataChanged();
}

void PieceEditorWidget :: setPiece(Piece* p)
{
	piece = p;
	pickupViewWidget->setPickup(p->pickup);
	twistWidget->setTwist(&(p->twist));
	pieceCustomizeViewWidget->setPiece(p);
	emit someDataChanged();	
}

Piece* PieceEditorWidget :: getPiece()
{
	return piece;
}




