
#include <QSpinBox>
#include <QHBoxLayout>
#include <QTabWidget>
#include <QPushButton>
#include <QStackedWidget>
#include <QScrollArea>
#include <QTimer>
#include <QApplication>
#include <QScrollBar>

#include "cane.h"
#include "geometry.h"
#include "caneeditorwidget.h"
#include "caneeditorviewwidget.h"
#include "canecustomizeviewwidget.h"
#include "niceviewwidget.h"
#include "canetemplatelibrarywidget.h"
#include "canelibrarywidget.h"
#include "mesh.h"
#include "dependency.h"
#include "templateparameter.h"
#include "canegeometrythread.h"
#include "twistwidget.h"
#include "canecrosssectionrender.h"
#include "globalbackgroundcolor.h"
#include "globalgraphicssetting.h"
#include "constants.h"
#include "undoredo.h"

CaneEditorWidget :: CaneEditorWidget(UndoRedo* undoRedo, QWidget* parent) : QWidget(parent)
{
	this->cane_ = new Cane(CaneTemplate::HORIZONTAL_LINE_CIRCLE);

	viewWidget = new CaneEditorViewWidget(cane_, undoRedo, this);	
	customizeViewWidget = new CaneCustomizeViewWidget(cane_, undoRedo, this);
	niceViewWidget = new NiceViewWidget(NiceViewWidget::PULLPLAN_CAMERA_MODE, this);
	niceViewWidget->setGeometry(&geometry);

	setupLayout();
	setupThreading();
	setupConnections();

	this->undoRedo = undoRedo;

	seedTemplates();
}

bool CaneEditorWidget :: eventFilter(QObject* obj, QEvent* event)
{
	// Goal is to stop the count spin from eating undo/redo commands
	// for its own text editing purpose. These events should instead
	// go up the chain to our own undo/redo implementation.
	if (obj == countSpin && event->type() == QEvent::ShortcutOverride)
	{
		event->ignore();
		return true;
	}
	return false;
}

QImage CaneEditorWidget :: caneImage()
{
	return niceViewWidget->grabFrameBuffer();
}

void CaneEditorWidget :: reset3DCamera()
{
	niceViewWidget->resetCamera();
}

void CaneEditorWidget :: resetCane()
{
	this->setCane(new Cane(CaneTemplate::HORIZONTAL_LINE_CIRCLE));
}

void CaneEditorWidget :: writeCaneToOBJFile(QString& filename)
{
	geometry.save_obj_file(filename.toStdString());
}

void CaneEditorWidget :: writeCaneToPLYFile(QString& filename)
{
	geometry.save_ply_file(filename.toStdString());
}

void CaneEditorWidget :: setupThreading()
{
	geometryDirty = false;
	tempCane = deep_copy(cane_);
	tempCaneDirty = true;
	geometryThread = new CaneGeometryThread(this);
	geometryThread->start();
}

void CaneEditorWidget :: updateEverything()
{
	// set casing buttons
	switch (cane_->outermostCasingShape())
	{
		case CIRCLE_SHAPE:
			circleCasingPushButton->setDown(true);
			squareCasingPushButton->setDown(false);
			break;
		case SQUARE_SHAPE:
			circleCasingPushButton->setDown(false);
			squareCasingPushButton->setDown(true);
			break;
	}
	removeCasingButton->setEnabled(!cane_->hasMinimumCasingCount());
	countSpin->setValue(cane_->count());
	countLabel->setEnabled(cane_->templateType() != CaneTemplate::CUSTOM);
	countSpin->setEnabled(cane_->templateType() != CaneTemplate::CUSTOM);
	twistWidget->updateEverything();
	twistWidget->setEnabled(cane_->outermostCasingShape() == CIRCLE_SHAPE);
	if (cane_->templateType() != CaneTemplate::CUSTOM)
		controlsTab->setCurrentIndex(0);
	
	tempCaneMutex.lock();
	deep_delete(tempCane);
	tempCane = deep_copy(cane_);
	tempCaneDirty = true;
	tempCaneMutex.unlock();

	QString message("Rendering cane...");
	emit showMessage(message, 0); // show until next message 
	wakeWait.wakeOne(); // wake up the thread if it's sleeping

	// Highlight correct pull template
	CaneTemplateLibraryWidget* ptlw; 
	for (int i = 0; i < templateLibraryLayout->count(); ++i)
	{
		ptlw = dynamic_cast<CaneTemplateLibraryWidget*>(
				dynamic_cast<QWidgetItem *>(templateLibraryLayout->itemAt(i))->widget());
		ptlw->setHighlighted(ptlw->type == cane_->templateType());
	}

	viewWidget->updateEverything();
	customizeViewWidget->updateEverything();
}

void CaneEditorWidget :: geometryThreadFinishedMesh(bool completed, unsigned int quality)
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
		geometry.vertices = tempGeometry.vertices;
		geometry.triangles = tempGeometry.triangles;
		geometry.groups = tempGeometry.groups;
		tempGeometryMutex.unlock();

		niceViewWidget->repaint();
	}
	else
	{
		// try to get the lock again in 250 ms
		QTimer::singleShot(250, this, SLOT(geometryThreadFinishedMesh(completed, quality))); 
		return;
	}

	// report what happened if it's the high quality mesh
	if (quality != GlobalGraphicsSetting::VERY_HIGH)
		return;
	QString result;
	if (completed)
		emit showMessage("Cane rendered successfully.", 3);
	else
		emit showMessage("Cane is too complex to render completely.", 3);
}

void CaneEditorWidget :: setupLayout()
{
	// we use a grid layout, with the edit-y parts in the left column 
	// and 3D view in the right column 
	QGridLayout* editorLayout = new QGridLayout(this);
	this->setLayout(editorLayout);
	editorLayout->setContentsMargins(0, 0, 0, 0);
	editorLayout->setSpacing(10);

	// build pair of editor views: regular and custom
	viewEditorStack = new QStackedWidget(this);
	viewEditorStack->addWidget(viewWidget);
	viewEditorStack->addWidget(customizeViewWidget);
	editorLayout->addWidget(viewEditorStack, 0, 0);

	// next is scrollable library of templates
	QWidget* templateLibraryWidget = new QWidget(this);
	templateLibraryLayout = new QHBoxLayout(templateLibraryWidget);
	templateLibraryLayout->setSpacing(10);
	templateLibraryLayout->setContentsMargins(10, 10, 10, 10);
	templateLibraryWidget->setLayout(templateLibraryLayout);

	pullTemplateLibraryScrollArea = new QScrollArea(this);
	pullTemplateLibraryScrollArea->setBackgroundRole(QPalette::Dark);
	pullTemplateLibraryScrollArea->setWidget(templateLibraryWidget);
	pullTemplateLibraryScrollArea->setWidgetResizable(true);
	pullTemplateLibraryScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	pullTemplateLibraryScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

	pullTemplateLibraryScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	pullTemplateLibraryScrollArea->setFixedHeight(140);
	editorLayout->addWidget(pullTemplateLibraryScrollArea, 1, 0);



	// now the two tabs of controls, one for each GUI mode
	controlsTab = new QTabWidget(this);
	editorLayout->addWidget(controlsTab, 2, 0);


	// regular controls: casing, count, twist
	QWidget* tab1Widget = new QWidget(controlsTab);
	QVBoxLayout* tab1Layout = new QVBoxLayout(tab1Widget);
	tab1Widget->setLayout(tab1Layout);
	controlsTab->addTab(tab1Widget, "Fill and Case");

	QWidget* casingWidget = new QWidget(tab1Widget);
	QHBoxLayout* casingLayout = new QHBoxLayout(casingWidget);
	QLabel* casingLabel = new QLabel("Casing:", casingWidget);
	circleCasingPushButton = new QPushButton(casingWidget); 
	QIcon circleIcon(QPixmap::fromImage(QImage(":/images/circle_icon.png")));
	circleCasingPushButton->setIcon(circleIcon);
	squareCasingPushButton = new QPushButton(casingWidget); 
	QIcon squareIcon(QPixmap::fromImage(QImage(":/images/square_icon.png")));
	squareCasingPushButton->setIcon(squareIcon);
	addCasingButton = new QPushButton("+", casingWidget);
	removeCasingButton = new QPushButton("-", casingWidget);
	casingLayout->addWidget(casingLabel);
	casingLayout->addWidget(circleCasingPushButton);
	casingLayout->addWidget(squareCasingPushButton);
	casingLayout->addWidget(addCasingButton);
	casingLayout->addWidget(removeCasingButton);

	countLabel = new QLabel("Count:", casingWidget);
	countSpin = new QSpinBox(casingWidget);
	countSpin->setRange(MIN_CANE_COUNT_PARAMETER_VALUE, MAX_CANE_COUNT_PARAMETER_VALUE);
	countSpin->installEventFilter(this);
	casingLayout->addStretch(1);
	casingLayout->addWidget(countLabel);
	casingLayout->addWidget(countSpin);

	twistWidget = new TwistWidget(this->cane_, NULL, 10, tab1Widget);

	tab1Layout->addWidget(casingWidget);
	tab1Layout->addWidget(twistWidget);
	tab1Layout->addStretch(1);


	// custom controls: add circle, square, duplicate selection, delete selection
	QWidget* tab2Widget = new QWidget(controlsTab);
	QVBoxLayout* tab2Layout = new QVBoxLayout(tab2Widget);
	tab2Widget->setLayout(tab2Layout);
	controlsTab->addTab(tab2Widget, "Customize");

	QWidget* customControlsWidget = new QWidget(tab2Widget);
	QHBoxLayout* customControlsLayout = new QHBoxLayout(customControlsWidget);
	customControlsWidget->setLayout(customControlsLayout);
	addCircleButton = new QPushButton("Add Circle", customControlsWidget);
	addSquareButton = new QPushButton("Add Square", customControlsWidget);
	copySelectedButton = new QPushButton("Duplicate", customControlsWidget);
	deleteSelectedButton = new QPushButton("Delete", customControlsWidget);
	customControlsLayout->addStretch(1);
	customControlsLayout->addWidget(addCircleButton);
	customControlsLayout->addWidget(addSquareButton);
	customControlsLayout->addWidget(copySelectedButton);
	customControlsLayout->addWidget(deleteSelectedButton);
	customControlsLayout->addStretch(1);
	
	tab2Layout->addWidget(customControlsWidget);
	tab2Layout->addStretch(1);


	// now add the 3D view 
	editorLayout->addWidget(niceViewWidget, 0, 1, 3, 1);

	// at this point the editor GUI elements are done


	// set proportions of the various parts: 
	// horizontally, 60% is the editor, 40% is the 3D view
	editorLayout->setColumnStretch(0, 3);
	editorLayout->setColumnStretch(1, 2);

	// vertically, the editor view that the user manipulates/interacts with 
	// takes up all extra space
	// controls are comclicked as much as possible 
	// (they are ugly and don't require much precision)
	editorLayout->setRowStretch(0, 10);
}


void CaneEditorWidget :: controlsTabChanged(int tab)
{
	// change the blueprint view to match the tab
	viewEditorStack->setCurrentIndex(tab); 

	if (tab != 0) // customize mode	
	{
		CaneTemplate::Type oldType = cane_->templateType();
		cane_->setTemplateType(CaneTemplate::CUSTOM);				
		if (oldType != CaneTemplate::CUSTOM)
			undoRedo->modifiedCane(cane_);
	}
}

void CaneEditorWidget :: circleCasingButtonClicked()
{
	cane_->setOutermostCasingShape(CIRCLE_SHAPE);
	undoRedo->modifiedCane(cane_);
}

void CaneEditorWidget :: squareCasingButtonClicked()
{
	cane_->setOutermostCasingShape(SQUARE_SHAPE);
	undoRedo->modifiedCane(cane_);
}

void CaneEditorWidget :: removeCasingButtonClicked()
{
	cane_->removeCasing();
	undoRedo->modifiedCane(cane_);
}

void CaneEditorWidget :: addCasingButtonClicked()
{
	cane_->addCasing(cane_->outermostCasingShape());
	undoRedo->modifiedCane(cane_);
}

void CaneEditorWidget :: copySelectedButtonClicked()
{
	customizeViewWidget->copySelectionClicked();
}

void CaneEditorWidget :: deleteSelectedButtonClicked()
{
	customizeViewWidget->deleteSelectionClicked();
}

void CaneEditorWidget :: addCircleButtonClicked()
{
	customizeViewWidget->addCircleClicked();
}

void CaneEditorWidget :: addSquareButtonClicked()
{
	customizeViewWidget->addSquareClicked();
}
	
void CaneEditorWidget :: mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton && pullTemplateLibraryScrollArea->geometry().contains(event->pos()))
	{
		isDragging = true;
		lastDragPosition = dragStartPosition = event->pos();
		maxDragDistance = 0;
	}
	else
		isDragging = false;
}

void CaneEditorWidget :: mouseMoveEvent(QMouseEvent* event)
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
	pullTemplateLibraryScrollArea->horizontalScrollBar()->setValue(
		pullTemplateLibraryScrollArea->horizontalScrollBar()->value() - movement);
	lastDragPosition = event->pos();
}

void CaneEditorWidget :: mouseReleaseEvent(QMouseEvent* event)
{
	// If not dragging or dragging caused a scroll
	if (!isDragging || (isDragging && maxDragDistance >= QApplication::startDragDistance()))
		return;

	CaneTemplateLibraryWidget* ptlw = dynamic_cast<CaneTemplateLibraryWidget*>(childAt(event->pos()));

	if (ptlw == NULL)
		return;

	if (ptlw->type == CaneTemplate::CUSTOM)
	{
		// simulate user pressing customize tab
		controlsTab->setCurrentIndex(1);
	}
	else
	{
		// put the user back in fill and case mode,
		// as they're no longer working on a custom template
		controlsTab->setCurrentIndex(0);
		cane_->setTemplateType(ptlw->type);	
		undoRedo->modifiedCane(cane_);
	}
}

void CaneEditorWidget :: setupConnections()
{
	// editor controls
	connect(circleCasingPushButton, SIGNAL(clicked()), this, SLOT(circleCasingButtonClicked()));
	connect(squareCasingPushButton, SIGNAL(clicked()), this, SLOT(squareCasingButtonClicked()));
	connect(addCasingButton, SIGNAL(clicked()), this, SLOT(addCasingButtonClicked()));
	connect(removeCasingButton, SIGNAL(clicked()), this, SLOT(removeCasingButtonClicked()));
	connect(copySelectedButton, SIGNAL(clicked()), this, SLOT(copySelectedButtonClicked()));
	connect(deleteSelectedButton, SIGNAL(clicked()), this, SLOT(deleteSelectedButtonClicked()));
	connect(addCircleButton, SIGNAL(clicked()), this, SLOT(addCircleButtonClicked()));
	connect(addSquareButton, SIGNAL(clicked()), this, SLOT(addSquareButtonClicked()));
	connect(twistWidget, SIGNAL(valueChanged()), this, SLOT(childWidgetDataChanged()));
	connect(twistWidget, SIGNAL(valueChangeEnded()), this, SLOT(twistEnded()));
	connect(countSpin, SIGNAL(valueChanged(int)), this, SLOT(countSpinChanged(int)));
	connect(controlsTab, SIGNAL(currentChanged(int)), this, SLOT(controlsTabChanged(int)));

	// render thread	
	connect(geometryThread, SIGNAL(finishedMesh(bool, unsigned int)), 
		this, SLOT(geometryThreadFinishedMesh(bool, unsigned int)));

	// modified() events
	connect(this->cane_, SIGNAL(modified()), this, SLOT(updateEverything()));
}
	
void CaneEditorWidget :: twistEnded()
{
	undoRedo->modifiedCane(cane_);
}

void CaneEditorWidget :: countSpinChanged(int)
{
	// update template
	unsigned int count = cane_->count();
	if (count != static_cast<unsigned int>(countSpin->value()))
	{
		cane_->setCount(countSpin->value());
		undoRedo->modifiedCane(cane_);
	}
}

void CaneEditorWidget :: seedTemplates()
{
	for (int i = CaneTemplate::firstSeedTemplate(); i <= CaneTemplate::lastSeedTemplate(); ++i)
	{
		CaneTemplate::Type t = static_cast<CaneTemplate::Type>(i);
		Cane cane(t);

		QPixmap templatePixmap(100, 100);
		templatePixmap.fill(GlobalBackgroundColor::qcolor);

		QPainter painter(&templatePixmap);
		CaneCrossSectionRender::render(&painter, 100, &cane);
		painter.end();

		CaneTemplateLibraryWidget *ptlw = new CaneTemplateLibraryWidget(templatePixmap, t);
		templateLibraryLayout->addWidget(ptlw);
	}

	// add the custom template last
	CaneTemplateLibraryWidget *ptlw = new CaneTemplateLibraryWidget(
		QPixmap::fromImage(QImage(":/images/pulltemplate_custom.png")), CaneTemplate::CUSTOM);
	templateLibraryLayout->addWidget(ptlw);
}

void CaneEditorWidget :: setCane(Cane* cane_)
{
	disconnect(this->cane_, SIGNAL(modified()), this, SLOT(updateEverything()));
	this->cane_ = cane_;
	connect(this->cane_, SIGNAL(modified()), this, SLOT(updateEverything()));
	controlsTab->setCurrentIndex(0);
	updateEverything();
	twistWidget->setCane(cane_);
	viewWidget->setCane(cane_);
	customizeViewWidget->setCane(cane_);
}

Cane* CaneEditorWidget :: cane()
{
	return this->cane_;
}


