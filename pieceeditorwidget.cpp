
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
#include <QTimer>
#include <QApplication>
#include <QScrollBar>

#include "pickupeditorviewwidget.h"
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
#include "globalgraphicssetting.h"
#include "constants.h"
#include "globalundoredo.h"

PieceEditorWidget :: PieceEditorWidget(QWidget* parent) : QWidget(parent)
{
	this->piece_ = new Piece(PieceTemplate::TUMBLER, PickupTemplate::VERTICAL);
	this->tempPiece_ = NULL;	

	this->pickupViewWidget = new PickupEditorViewWidget(this->piece_, this);	
	this->pieceNiceViewWidget = new NiceViewWidget(NiceViewWidget::PIECE_CAMERA_MODE, this);
	pieceNiceViewWidget->setGeometry(&geometry);
	this->pieceCustomizeViewWidget = new PieceCustomizeViewWidget(this->piece_, this);

	setupLayout();
	setupThreading();
	setupConnections();

	seedTemplates();
}

bool PieceEditorWidget :: eventFilter(QObject* obj, QEvent* event)
{
	// Goal is to stop the spinboxes from eating undo/redo commands
	// for their own text editing purpose. These events should instead
	// go up the chain to our own undo/redo implementation.
	if (obj == pickupCountSpin && event->type() == QEvent::ShortcutOverride)
	{
		event->ignore();
		return true;
	}
	return false;
}

QImage PieceEditorWidget :: pieceImage()
{
	return pieceNiceViewWidget->grabFrameBuffer();
}

void PieceEditorWidget :: reset3DCamera()
{
	pieceNiceViewWidget->resetCamera();
}	

void PieceEditorWidget :: resetPiece()
{
	setPiece(new Piece(PieceTemplate::TUMBLER, PickupTemplate::VERTICAL));
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
		pktlw->setHighlighted(pktlw->type == this->piece_->pickupTemplateType());
	}
	pickupViewWidget->updateEverything();
	TemplateParameter tp;
	this->piece_->pickupParameter(0, &tp);
	pickupCountSpin->blockSignals(true);
	pickupCountSpin->setRange(tp.lowerLimit, tp.upperLimit);
	pickupCountSpin->setValue(tp.value);
	pickupCountSpin->blockSignals(false);

	// update piece stuff
	pieceCustomizeViewWidget->updateEverything();	
	PieceTemplateLibraryWidget* ptlw;
	for (int i = 0; i < pieceTemplateLibraryLayout->count(); ++i)
	{
		ptlw = dynamic_cast<PieceTemplateLibraryWidget*>(
			dynamic_cast<QWidgetItem *>(pieceTemplateLibraryLayout->itemAt(i))->widget());
		ptlw->setHighlighted(ptlw->type == this->piece_->pieceTemplateType());
	}
	twistWidget->updateEverything();
	if (this->piece_->pieceTemplateType() != PieceTemplate::CUSTOM)
		pieceControlsTab->setCurrentIndex(0);
	
	// Make a copy of the current state of the piece
	tempPieceMutex.lock();
	deep_delete(tempPiece_);
	tempPiece_ = deep_copy(this->piece_);
	tempPieceDirty = true;
	tempPieceMutex.unlock();

	// Start up the rendering thread, clearing out geometry first
	// so old piece/pickup don't stay visible and confuse the user
	QString message("Rendering piece...");
	emit showMessage(message, 0); // show until next message 
	wakeWait.wakeOne();
}

void PieceEditorWidget :: geometryThreadFinishedMesh(bool completed, unsigned int quality)
{
	geometryDirtyMutex.lock();
	bool dirty = geometryDirty;
	geometryDirtyMutex.unlock();
	if (!dirty)
		return;

	if (tempGeometryMutex.tryLock())
	{
		geometryDirtyMutex.lock();
		geometryDirty = false;
		geometryDirtyMutex.unlock();
		
		geometry.vertices = tempPieceGeometry.vertices;
		geometry.triangles = tempPieceGeometry.triangles;
		geometry.groups = tempPieceGeometry.groups;
		pickupViewWidget->geometry.vertices = tempPickupGeometry.vertices;
		pickupViewWidget->geometry.triangles = tempPickupGeometry.triangles;
		pickupViewWidget->geometry.groups = tempPickupGeometry.groups;
		tempGeometryMutex.unlock();

		pickupViewWidget->updateEverything();
		pieceNiceViewWidget->repaint();
	}
	else
		// geometry being recomputed; wait for next finished message
		return;

	// report what happened if it's the high quality mesh
	if (quality != GlobalGraphicsSetting::HIGH)
		return;
	if (completed)
		emit showMessage("Piece rendered successfully.", 3);
	else
		emit showMessage("Piece is too complex to render completely.", 3);
}

void PieceEditorWidget :: pickupCountSpinChanged(int)
{
	TemplateParameter tp;
	this->piece_->pickupParameter(0, &tp);
	if (tp.value != pickupCountSpin->value())
	{
		this->piece_->setPickupParameter(0, pickupCountSpin->value());
		GlobalUndoRedo::modifiedPiece(this->piece_);
	}
}

void PieceEditorWidget :: mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton && pickupTemplateLibraryScrollArea->geometry().contains(event->pos()))
	{
		isDragging = true;
		lastDragPosition = dragStartPosition = event->pos();
		dragIsPickup = true;
		maxDragDistance = 0;
	}
	else if (event->button() == Qt::LeftButton && pieceTemplateLibraryScrollArea->geometry().contains(event->pos()))
	{
		isDragging = true;
		lastDragPosition = dragStartPosition = event->pos();
		dragIsPickup = false;
		maxDragDistance = 0;
	}
	else
		isDragging = false;
}

void PieceEditorWidget :: mouseMoveEvent(QMouseEvent* event)
{
	// If the left mouse button isn't down
	if ((event->buttons() & Qt::LeftButton) == 0)
	{
		isDragging = false;
		return;
	}

	maxDragDistance = MAX(maxDragDistance, abs(event->pos().x() - dragStartPosition.x()));
	if (!isDragging || maxDragDistance < QApplication::startDragDistance())
		return;

	int movement = event->pos().x() - lastDragPosition.x();
	QScrollArea* draggedScrollArea;
	if (dragIsPickup)
		draggedScrollArea = pickupTemplateLibraryScrollArea;
	else
		draggedScrollArea = pieceTemplateLibraryScrollArea;
	draggedScrollArea->horizontalScrollBar()->setValue(draggedScrollArea->horizontalScrollBar()->value() - movement);
	lastDragPosition = event->pos();
}

void PieceEditorWidget :: mouseReleaseEvent(QMouseEvent* event)
{
	// If not dragging or dragging cause a scroll
	if (!isDragging || (isDragging && maxDragDistance >= QApplication::startDragDistance()))
		return;

	PickupTemplateLibraryWidget* pktlw = dynamic_cast<PickupTemplateLibraryWidget*>(childAt(event->pos()));
	PieceTemplateLibraryWidget* ptlw = dynamic_cast<PieceTemplateLibraryWidget*>(childAt(event->pos()));

	if (pktlw != NULL)
	{
		if (pktlw->type != this->piece_->pickupTemplateType())
		{
			this->piece_->setPickupTemplateType(pktlw->type);
			GlobalUndoRedo::modifiedPiece(this->piece_);
		}
	}
	else if (ptlw != NULL)
	{
		if (ptlw->type == PieceTemplate::CUSTOM)
			pieceControlsTab->setCurrentIndex(1);
		else
		{
			pieceControlsTab->setCurrentIndex(0);
			this->piece_->setPieceTemplateType(ptlw->type);
			GlobalUndoRedo::modifiedPiece(this->piece_);
		}
	}
}

void PieceEditorWidget :: setupLayout()
{
	QGridLayout* editorLayout = new QGridLayout(this);
	this->setLayout(editorLayout);
	editorLayout->setContentsMargins(0, 0, 0, 0);
	editorLayout->setSpacing(10);

	// two 3D views in the first row (and stretched to take up all the slack space 
	editorLayout->addWidget(pickupViewWidget, 0, 0); 
	pieceViewStack = new QStackedWidget(this);
	pieceViewStack->addWidget(pieceNiceViewWidget);
	pieceViewStack->addWidget(pieceCustomizeViewWidget);
	editorLayout->addWidget(pieceViewStack, 0, 1);

	// pickup template and piece template selectors in the second row
	QWidget* pickupTemplateLibraryWidget = new QWidget(this);
	pickupTemplateLibraryLayout = new QHBoxLayout(pickupTemplateLibraryWidget);
	pickupTemplateLibraryLayout->setSpacing(10);
	pickupTemplateLibraryLayout->setContentsMargins(10, 10, 10, 10);
	pickupTemplateLibraryWidget->setLayout(pickupTemplateLibraryLayout);

	pickupTemplateLibraryScrollArea = new QScrollArea(this);
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

	pieceTemplateLibraryScrollArea = new QScrollArea(this);
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
	QGridLayout* pickupParamLayout = new QGridLayout(pickupParamWidget);
	pickupParamWidget->setLayout(pickupParamLayout);
	
	pickupParamLayout->addWidget(new QLabel("Count:", pickupParamWidget), 0, 0);
	
	pickupCountSpin = new QSpinBox(pickupParamWidget);
	pickupCountSpin->installEventFilter(this);
	pickupParamLayout->addWidget(pickupCountSpin, 0, 2);

	pickupParamLayout->setColumnStretch(4, 1);

	pickupControlsTab->addTab(pickupParamWidget, "Fill and Case");

	
	// Piece parameter layout 
	pieceControlsTab = new QTabWidget(this);
	editorLayout->addWidget(pieceControlsTab, 2, 1);


	// Regular controls tab: twist
	QWidget* tab1Widget = new QWidget(pieceControlsTab);
	QVBoxLayout* tab1Layout = new QVBoxLayout(tab1Widget);
	tab1Widget->setLayout(tab1Layout); 
	pieceControlsTab->addTab(tab1Widget, "Twist");

	twistWidget = new TwistWidget(NULL, this->piece_, 3.0, pieceControlsTab);
	tab1Layout->addWidget(twistWidget, 0);
	tab1Layout->addStretch(10);


	// Custom controls tab
	QWidget* tab2Widget = new QWidget(pieceControlsTab);
	QVBoxLayout* tab2Layout = new QVBoxLayout(tab2Widget);
	tab2Widget->setLayout(tab2Layout);
	pieceControlsTab->addTab(tab2Widget, "Customize");

	QWidget* customPieceControlsWidget = new QWidget(tab2Widget);
	QHBoxLayout* customPieceControlsLayout = new QHBoxLayout(tab2Widget);
	customPieceControlsWidget->setLayout(customPieceControlsLayout); 

	addControlPointButton = new QPushButton("Add control point", customPieceControlsWidget);
	deleteControlPointButton = new QPushButton("Delete control point", customPieceControlsWidget);
	customPieceControlsLayout->addStretch(1);
	customPieceControlsLayout->addWidget(addControlPointButton);
	customPieceControlsLayout->addWidget(deleteControlPointButton);
	customPieceControlsLayout->addStretch(1);

	tab2Layout->addWidget(customPieceControlsWidget);
	tab2Layout->addStretch(1);	

	editorLayout->setRowStretch(0, 10);
}


void PieceEditorWidget :: setupThreading()
{
	geometryDirty = false;
	tempPiece_ = deep_copy(this->piece_);
	tempPieceDirty = true;
	geometryThread = new PieceGeometryThread(this);
	geometryThread->start();	
}

void PieceEditorWidget :: setupConnections()
{
	// pickup controls
	connect(pickupCountSpin, SIGNAL(valueChanged(int)), this, SLOT(pickupCountSpinChanged(int)));

	// custom piece controls
	connect(addControlPointButton, SIGNAL(clicked()), this, SLOT(addControlPointButtonClicked()));
	connect(deleteControlPointButton, SIGNAL(clicked()), this, SLOT(deleteControlPointButtonClicked()));
	
	// threaded rendering
	connect(geometryThread, SIGNAL(finishedMesh(bool, unsigned int)), 
		this, SLOT(geometryThreadFinishedMesh(bool, unsigned int)));

	// subwidget communication
	connect(pieceControlsTab, SIGNAL(currentChanged(int)), this, SLOT(pieceControlsTabChanged(int)));

	connect(this->piece_, SIGNAL(modified()), this, SLOT(updateEverything()));
}

void PieceEditorWidget :: twistEnded()
{
	GlobalUndoRedo::modifiedPiece(this->piece_);
}

void PieceEditorWidget :: addControlPointButtonClicked()
{
	Spline spline = this->piece_->spline();
	spline.addPoint(Point2D(make_vector(0.0f, 0.0f)));
	this->piece_->setSpline(spline);
	GlobalUndoRedo::modifiedPiece(this->piece_);
}

void PieceEditorWidget :: deleteControlPointButtonClicked()
{
	Spline spline = this->piece_->spline();
	spline.removePoint();
	this->piece_->setSpline(spline);
	GlobalUndoRedo::modifiedPiece(this->piece_);
}

void PieceEditorWidget :: pieceControlsTabChanged(int tab)
{
	// change the view to match the tab
	pieceViewStack->setCurrentIndex(tab);

	if (tab != 0) // customize mode
	{
		this->piece_->setPieceTemplateType(PieceTemplate::CUSTOM);
		GlobalUndoRedo::modifiedPiece(this->piece_);
	}
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
		Piece dummyPiece(t, PickupTemplate::VERTICAL);

		QPixmap templatePixmap(100, 100);
		templatePixmap.fill(GlobalBackgroundColor::qcolor);

		QPainter painter(&templatePixmap);
		PieceCrossSectionRender::render(&painter, 100, &dummyPiece);
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
	this->piece_->setPickupParameter(param, value);
	GlobalUndoRedo::modifiedPiece(this->piece_);
}

void PieceEditorWidget :: setPieceTemplateType(enum PieceTemplate::Type _type)
{
	this->piece_->setPieceTemplateType(_type);
	GlobalUndoRedo::modifiedPiece(this->piece_);
}

void PieceEditorWidget :: setPickupTemplateType(enum PickupTemplate::Type _type)
{
	this->piece_->setPickupTemplateType(_type);
	GlobalUndoRedo::modifiedPiece(this->piece_);
}

void PieceEditorWidget :: setPiece(Piece* piece)
{
	disconnect(this->piece_, SIGNAL(modified()), this, SLOT(updateEverything()));
	this->piece_ = piece;
	connect(this->piece_, SIGNAL(modified()), this, SLOT(updateEverything()));
	pieceControlsTab->setCurrentIndex(0);
	updateEverything();
	pickupViewWidget->setPiece(this->piece_);
	twistWidget->setPiece(this->piece_);
	pieceCustomizeViewWidget->setPiece(this->piece_);
}

Piece* PieceEditorWidget :: piece() const
{
	return this->piece_;
}




