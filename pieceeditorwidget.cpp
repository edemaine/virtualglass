
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
#include "globalgraphicssetting.h"

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

	seedTemplates();
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
	{
		// try to get the lock again in 250 ms
		QTimer::singleShot(250, this, SLOT(geometryThreadFinishedMesh(completed, quality)));
		return;
	}

	// report what happened if it's the high quality mesh
	if (quality != GlobalGraphicsSetting::HIGH)
		return;
	QString result;
	if (completed)
		emit showMessage("Piece rendered successfully.", 5);
	else
		emit showMessage("Piece is too complex to render completely.", 5);
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
	{
		updateEverything();
		emit someDataChanged();
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

	maxDragDistance = MAX(maxDragDistance, fabs(event->pos().x() - dragStartPosition.x()));
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
		if (pktlw->type != piece->pickup->getTemplateType())
		{
			piece->pickup->setTemplateType(pktlw->type);
			updateEverything();
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
			updateEverything();
			emit someDataChanged();
		}
	}
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
	{
		updateEverything();
		emit someDataChanged();
	}
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
	editorLayout->setContentsMargins(0, 0, 0, 0);

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
	QVBoxLayout* pickupParamLayout = new QVBoxLayout(pickupParamWidget);
	pickupParamWidget->setLayout(pickupParamLayout);
	addPickupParam(pickupParamLayout);
	addPickupParam(pickupParamLayout);
	pickupControlsTab->addTab(pickupParamWidget, "Fill and Case");

	// below the tabs goes a labeled descriptor (changes depending on view)
	QLabel* pickupEditorDescriptionLabel = new QLabel("Pickup editor - drag color or canes in.", this);
	pickupEditorDescriptionLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	editorLayout->addWidget(pickupEditorDescriptionLabel, 3, 0);


	
	// Piece parameter layout 
	pieceControlsTab = new QTabWidget(this);
	editorLayout->addWidget(pieceControlsTab, 2, 1);


	// Regular controls tab: twist
	QWidget* tab1Widget = new QWidget(pieceControlsTab);
	QVBoxLayout* tab1Layout = new QVBoxLayout(tab1Widget);
	tab1Widget->setLayout(tab1Layout); 
	pieceControlsTab->addTab(tab1Widget, "Twist");

	twistWidget = new TwistWidget(&(piece->twist), 3.0, pieceControlsTab);
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
	removeControlPointButton = new QPushButton("Remove control point", customPieceControlsWidget);
	customPieceControlsLayout->addStretch(1);
	customPieceControlsLayout->addWidget(addControlPointButton);
	customPieceControlsLayout->addWidget(removeControlPointButton);
	customPieceControlsLayout->addStretch(1);

	tab2Layout->addWidget(customPieceControlsWidget);
	tab2Layout->addStretch(1);	


	// below the tabs goes a labeled descriptor (changes depending on view)
	pieceEditorDescriptionLabel = new QLabel("Piece editor.", this);
	pieceEditorDescriptionLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	editorLayout->addWidget(pieceEditorDescriptionLabel, 3, 1);

	editorLayout->setRowStretch(0, 10);
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
	
	connect(geometryThread, SIGNAL(finishedMesh(bool, unsigned int)), 
		this, SLOT(geometryThreadFinishedMesh(bool, unsigned int)));
	connect(pickupViewWidget, SIGNAL(someDataChanged()), this, SLOT(childWidgetDataChanged()));
	connect(twistWidget, SIGNAL(valueChanged()), this, SLOT(childWidgetDataChanged()));
	connect(pieceCustomizeViewWidget, SIGNAL(someDataChanged()), this, SLOT(childWidgetDataChanged()));
	connect(pieceControlsTab, SIGNAL(currentChanged(int)), this, SLOT(pieceControlsTabChanged(int)));
}

void PieceEditorWidget :: addControlPointButtonClicked()
{
	piece->spline.addPoint(Point2D(make_vector(0.0f, 0.0f)));
	updateEverything();
	emit someDataChanged();
}

void PieceEditorWidget :: removeControlPointButtonClicked()
{
	piece->spline.removePoint();
	updateEverything();
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
		updateEverything();
		emit someDataChanged();
	}
}

void PieceEditorWidget :: childWidgetDataChanged()
{
	updateEverything();
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
	updateEverything();
	emit someDataChanged();
}

void PieceEditorWidget :: setPieceTemplateType(enum PieceTemplate::Type _type)
{
	piece->setTemplateType(_type);
	updateEverything();
	emit someDataChanged();
}

void PieceEditorWidget :: setPickupTemplateType(enum PickupTemplate::Type _type)
{
	piece->pickup->setTemplateType(_type);
	updateEverything();
	emit someDataChanged();
}

void PieceEditorWidget :: setPiece(Piece* _piece)
{
	piece = _piece;
	pieceControlsTab->setCurrentIndex(0);
	updateEverything();
	pickupViewWidget->setPickup(piece->pickup);
	twistWidget->setTwist(&(piece->twist));
	pieceCustomizeViewWidget->setPiece(piece);
}

Piece* PieceEditorWidget :: getPiece()
{
	return piece;
}




