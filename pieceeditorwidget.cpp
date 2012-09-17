
#include "pieceeditorwidget.h"

PieceEditorWidget :: PieceEditorWidget(QWidget* parent) : QWidget(parent)
{
	this->piece = new Piece(TUMBLER_PIECE_TEMPLATE);

	this->pickupViewWidget = new PickupPlanEditorViewWidget(piece->pickup, this);	
	this->niceViewWidget = new NiceViewWidget(PIECE_MODE, this);
	niceViewWidget->setGeometry(&geometry);

	setupLayout();
	setupConnections();
}

void PieceEditorWidget :: updateEverything()
{
	// update pickup stuff
	for (int i = 0; i < pickupTemplateLibraryLayout->count(); ++i)
	{
		if (i + PickupTemplate::firstPickupTemplate() == piece->pickup->getTemplateType())
			highlightLibraryWidget(dynamic_cast<PickupTemplateLibraryWidget*>(
				dynamic_cast<QWidgetItem *>(pickupTemplateLibraryLayout->itemAt(i))->widget()));
		else
			unhighlightLibraryWidget(dynamic_cast<PickupTemplateLibraryWidget*>(
				dynamic_cast<QWidgetItem *>(pickupTemplateLibraryLayout->itemAt(i))->widget()));
	}

	pickupViewWidget->setPickup(piece->pickup);

	unsigned int i = 0;
	for (; i < piece->pickup->getParameterCount(); ++i)
	{
		pickupParamLabels[i]->setText(piece->pickup->getParameterName(i));
		pickupParamSpinboxes[i]->setValue(piece->pickup->getParameter(i));
		pickupParamSliders[i]->setValue(piece->pickup->getParameter(i));
		pickupParamWidgets[i]->show();
	}
	for (; i < 2; ++i)
	{
		pickupParamWidgets[i]->hide();
	}


	// update piece stuff
	for (int i = 0; i < pieceTemplateLibraryLayout->count(); ++i)
	{
		if (i + FIRST_PIECE_TEMPLATE == piece->getTemplate()->type)
			highlightLibraryWidget(dynamic_cast<PieceTemplateLibraryWidget*>(
				dynamic_cast<QWidgetItem *>(pieceTemplateLibraryLayout->itemAt(i))->widget()));
		else
			unhighlightLibraryWidget(dynamic_cast<PieceTemplateLibraryWidget*>(
				dynamic_cast<QWidgetItem *>(pieceTemplateLibraryLayout->itemAt(i))->widget()));
	}

	geometry.clear();
	mesher.generateMesh(piece, &geometry);
	niceViewWidget->repaint();
	
	i = 0;
	for (; i < piece->getTemplate()->parameterNames.size(); ++i)
	{
		pieceParamLabels[i]->setText(piece->getTemplate()->parameterNames[i]);
		pieceParamSliders[i]->setValue(piece->getTemplate()->parameterValues[i]);
		pieceParamWidgets[i]->show();
	}
	for (; i < 3; ++i)
	{
		pieceParamWidgets[i]->hide();
	}
}

void PieceEditorWidget :: unhighlightLibraryWidget(PieceTemplateLibraryWidget* w)
{
	w->graphicsEffect()->setEnabled(false);
}

void PieceEditorWidget :: highlightLibraryWidget(PieceTemplateLibraryWidget* w)
{
	w->graphicsEffect()->setEnabled(false);
	((QGraphicsHighlightEffect*) w->graphicsEffect())->setHighlightType(IS_DEPENDANCY);
	w->graphicsEffect()->setEnabled(true);
}

void PieceEditorWidget :: unhighlightLibraryWidget(PickupTemplateLibraryWidget* w)
{
	w->graphicsEffect()->setEnabled(false);
}

void PieceEditorWidget :: highlightLibraryWidget(PickupTemplateLibraryWidget* w)
{
	w->graphicsEffect()->setEnabled(false);
	((QGraphicsHighlightEffect*) w->graphicsEffect())->setHighlightType(IS_DEPENDANCY);
	w->graphicsEffect()->setEnabled(true);
}

void PieceEditorWidget :: pieceTemplateParameterSlider3Changed(int)
{
	int value = pieceParamSliders[2]->sliderPosition();

	if (value == piece->getTemplate()->parameterValues[2])
		return;
	piece->getTemplate()->parameterValues[2] = value;
	emit someDataChanged();
}

void PieceEditorWidget :: pieceTemplateParameterSlider2Changed(int)
{
	int value = pieceParamSliders[1]->sliderPosition();

	if (value == piece->getTemplate()->parameterValues[1])
		return;
	piece->getTemplate()->parameterValues[1] = value;
	emit someDataChanged();
}

void PieceEditorWidget :: pieceTemplateParameterSlider1Changed(int)
{
	int value = pieceParamSliders[0]->sliderPosition();

	if (value == piece->getTemplate()->parameterValues[0])
		return;
	piece->getTemplate()->parameterValues[0] = value;
	emit someDataChanged();
}

void PieceEditorWidget :: pickupParameter1SpinBoxChanged(int)
{
	int value = pickupParamSpinboxes[0]->value();

	if (value == piece->pickup->getParameter(0))
		return;

	piece->pickup->setParameter(0, value);
	emit someDataChanged();
}

void PieceEditorWidget :: pickupParameter2SpinBoxChanged(int)
{
	int value = pickupParamSpinboxes[1]->value();

	if (value == piece->pickup->getParameter(1))
		return;

	piece->pickup->setParameter(1, value);
	emit someDataChanged();
}

void PieceEditorWidget :: pickupParameter1SliderChanged(int)
{
	int value = pickupParamSliders[0]->sliderPosition();

	if (value == piece->pickup->getParameter(0))
		return;

	piece->pickup->setParameter(0, value);
	emit someDataChanged();
}

void PieceEditorWidget :: pickupParameter2SliderChanged(int)
{
	int value = pickupParamSliders[1]->sliderPosition();

	if (value == piece->pickup->getParameter(1))
		return;

	piece->pickup->setParameter(1, value);
	emit someDataChanged();
}

void PieceEditorWidget :: setupLayout()
{
	QHBoxLayout* piecePageLayout = new QHBoxLayout(this);
	this->setLayout(piecePageLayout);

	// Left (pickup) layout
	QVBoxLayout* leftLayout = new QVBoxLayout(this);
	piecePageLayout->addLayout(leftLayout, 1);
	leftLayout->addWidget(pickupViewWidget, 1);

	QWidget* pickupTemplateLibraryWidget = new QWidget(this);
	pickupTemplateLibraryLayout = new QHBoxLayout(pickupTemplateLibraryWidget);
	pickupTemplateLibraryLayout->setSpacing(10);
	pickupTemplateLibraryWidget->setLayout(pickupTemplateLibraryLayout);

	QScrollArea* pickupTemplateLibraryScrollArea = new QScrollArea(this);
	pickupTemplateLibraryScrollArea->setBackgroundRole(QPalette::Dark);
	pickupTemplateLibraryScrollArea->setWidget(pickupTemplateLibraryWidget);
	pickupTemplateLibraryScrollArea->setWidgetResizable(true);
	pickupTemplateLibraryScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	pickupTemplateLibraryScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	pickupTemplateLibraryScrollArea->setFixedHeight(130);
	leftLayout->addWidget(pickupTemplateLibraryScrollArea, 0);

	QWidget* pickupParameter1Widget = new QWidget(this);
	QLabel* pickupParameter1Label = new QLabel("Parameter 1");
	pickupParamLabels.push_back(pickupParameter1Label);
	QSpinBox* pickupParameter1SpinBox = new QSpinBox(pickupParameter1Widget);
	pickupParameter1SpinBox->setRange(6, 40);
	pickupParameter1SpinBox->setSingleStep(1);
	pickupParamSpinboxes.push_back(pickupParameter1SpinBox);
	QSlider* pickupParameter1Slider = new QSlider(Qt::Horizontal, pickupParameter1Widget);
	pickupParameter1Slider->setRange(6, 40);
	pickupParameter1Slider->setValue(1);
	pickupParamSliders.push_back(pickupParameter1Slider);
	QHBoxLayout* pickupParameter1Layout = new QHBoxLayout(pickupParameter1Widget);
	pickupParameter1Widget->setLayout(pickupParameter1Layout);
	pickupParameter1Layout->setContentsMargins(0, 0, 0, 0);
	pickupParameter1Layout->addWidget(pickupParameter1Label, 0);
	pickupParameter1Layout->addWidget(pickupParameter1SpinBox, 0);
	pickupParameter1Layout->addWidget(pickupParameter1Slider, 1);
	leftLayout->addWidget(pickupParameter1Widget, 0);
	pickupParamWidgets.push_back(pickupParameter1Widget);

	QWidget* pickupParameter2Widget = new QWidget(this);
	QLabel* pickupParameter2Label = new QLabel("Parameter 2");
	pickupParamLabels.push_back(pickupParameter2Label);
	QSpinBox* pickupParameter2SpinBox = new QSpinBox(pickupParameter2Widget);
	pickupParameter2SpinBox->setRange(6, 40);
	pickupParameter2SpinBox->setSingleStep(1);
	pickupParamSpinboxes.push_back(pickupParameter2SpinBox);
	QSlider* pickupParameter2Slider = new QSlider(Qt::Horizontal, pickupParameter2Widget);
	pickupParameter2Slider->setRange(6, 40);
	pickupParamSliders.push_back(pickupParameter2Slider);
	QHBoxLayout* pickupParameter2Layout = new QHBoxLayout(pickupParameter2Widget);
	pickupParameter2Widget->setLayout(pickupParameter2Layout);
	pickupParameter2Layout->setContentsMargins(0, 0, 0, 0);
	pickupParameter2Layout->addWidget(pickupParameter2Label, 0);
	pickupParameter2Layout->addWidget(pickupParameter2SpinBox, 0);
	pickupParameter2Layout->addWidget(pickupParameter2Slider, 1);
	leftLayout->addWidget(pickupParameter2Widget, 0);
	pickupParamWidgets.push_back(pickupParameter2Widget);

	leftLayout->addStretch(0);
	QLabel* pieceEditorDescriptionLabel = new QLabel("Pickup editor - drag canes in.", this);
	pieceEditorDescriptionLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	leftLayout->addWidget(pieceEditorDescriptionLabel, 0);

	// Right (piece) layout
	QVBoxLayout* rightLayout = new QVBoxLayout(this);
	piecePageLayout->addLayout(rightLayout, 1);
	rightLayout->addWidget(niceViewWidget, 10);

	QWidget* pieceTemplateLibraryWidget = new QWidget(this);
	pieceTemplateLibraryLayout = new QHBoxLayout(pieceTemplateLibraryWidget);
	pieceTemplateLibraryLayout->setSpacing(10);
	pieceTemplateLibraryWidget->setLayout(pieceTemplateLibraryLayout);

	QScrollArea* pieceTemplateLibraryScrollArea = new QScrollArea(this);
	pieceTemplateLibraryScrollArea->setBackgroundRole(QPalette::Dark);
	pieceTemplateLibraryScrollArea->setWidget(pieceTemplateLibraryWidget);
	pieceTemplateLibraryScrollArea->setWidgetResizable(true);
	pieceTemplateLibraryScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	pieceTemplateLibraryScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	pieceTemplateLibraryScrollArea->setFixedHeight(130);
	rightLayout->addWidget(pieceTemplateLibraryScrollArea, 0);

	QWidget* pieceParameter1Widget = new QWidget(this);
	QLabel* pieceTemplateParameter1Label = new QLabel("Parameter 1");
	pieceParamLabels.push_back(pieceTemplateParameter1Label);
	QSlider* pieceTemplateParameter1Slider = new QSlider(Qt::Horizontal, pieceParameter1Widget);
	pieceParamSliders.push_back(pieceTemplateParameter1Slider);
	pieceTemplateParameter1Slider->setRange(0, 100);
	pieceTemplateParameter1Slider->setSliderPosition(0);
	QHBoxLayout* pieceParameter1Layout = new QHBoxLayout(pieceParameter1Widget);
	pieceParameter1Widget->setLayout(pieceParameter1Layout);
	pieceParameter1Layout->setContentsMargins(0, 0, 0, 0);
	pieceParameter1Layout->addWidget(pieceTemplateParameter1Label);
	pieceParameter1Layout->addWidget(pieceTemplateParameter1Slider);
	pieceParamWidgets.push_back(pieceParameter1Widget);
	rightLayout->addWidget(pieceParameter1Widget);

	QWidget* pieceParameter2Widget = new QWidget(this);
	QLabel* pieceTemplateParameter2Label = new QLabel("Parameter 2");
	pieceParamLabels.push_back(pieceTemplateParameter2Label);
	QSlider* pieceTemplateParameter2Slider = new QSlider(Qt::Horizontal, pieceParameter2Widget);
	pieceParamSliders.push_back(pieceTemplateParameter2Slider);
	pieceTemplateParameter2Slider->setRange(0, 100);
	pieceTemplateParameter2Slider->setSliderPosition(0);
	QHBoxLayout* pieceParameter2Layout = new QHBoxLayout(pieceParameter2Widget);
	pieceParameter2Widget->setLayout(pieceParameter2Layout);
	pieceParameter2Layout->setContentsMargins(0, 0, 0, 0);
	pieceParameter2Layout->addWidget(pieceTemplateParameter2Label);
	pieceParameter2Layout->addWidget(pieceTemplateParameter2Slider);
	pieceParamWidgets.push_back(pieceParameter2Widget);
	rightLayout->addWidget(pieceParameter2Widget);

	QWidget* pieceParameter3Widget = new QWidget(this);
	QLabel* pieceTemplateParameter3Label = new QLabel("Parameter 3");
	pieceParamLabels.push_back(pieceTemplateParameter3Label);
	QSlider* pieceTemplateParameter3Slider = new QSlider(Qt::Horizontal, pieceParameter3Widget);
	pieceParamSliders.push_back(pieceTemplateParameter3Slider);
	pieceTemplateParameter3Slider->setRange(0, 100);
	pieceTemplateParameter3Slider->setSliderPosition(0);
	QHBoxLayout* pieceParameter3Layout = new QHBoxLayout(pieceParameter3Widget);
	pieceParameter3Widget->setLayout(pieceParameter3Layout);
	pieceParameter3Layout->setContentsMargins(0, 0, 0, 0);
	pieceParameter3Layout->addWidget(pieceTemplateParameter3Label);
	pieceParameter3Layout->addWidget(pieceTemplateParameter3Slider);
	pieceParamWidgets.push_back(pieceParameter3Widget);
	rightLayout->addWidget(pieceParameter3Widget);
	
	QLabel* niceViewDescriptionLabel = new QLabel("Piece editor.", this);
	niceViewDescriptionLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	rightLayout->addWidget(niceViewDescriptionLabel, 0);
}


void PieceEditorWidget :: mousePressEvent(QMouseEvent* event)
{
	PickupTemplateLibraryWidget* pktlw = dynamic_cast<PickupTemplateLibraryWidget*>(childAt(event->pos()));
	PieceTemplateLibraryWidget* ptlw = dynamic_cast<PieceTemplateLibraryWidget*>(childAt(event->pos()));

	if (pktlw != NULL)
	{
		if (pktlw->getPickupTemplateType() != piece->pickup->getTemplateType())
		{
			piece->pickup->setTemplateType(pktlw->getPickupTemplateType());
			emit someDataChanged();
		}
	}
	else if (ptlw != NULL)
	{
		if (ptlw->getPieceTemplateType() != piece->getTemplate()->type)
		{
			piece->setTemplate(new PieceTemplate(ptlw->getPieceTemplateType()));
			emit someDataChanged();
		}
	}
}

void PieceEditorWidget :: setupConnections()
{
	connect(pickupParamSpinboxes[0], SIGNAL(valueChanged(int)),
		this, SLOT(pickupParameter1SpinBoxChanged(int)));
	connect(pickupParamSliders[0], SIGNAL(valueChanged(int)),
		this, SLOT(pickupParameter1SliderChanged(int)));
	connect(pickupParamSpinboxes[1], SIGNAL(valueChanged(int)),
		this, SLOT(pickupParameter2SpinBoxChanged(int)));
	connect(pickupParamSliders[1], SIGNAL(valueChanged(int)),
		this, SLOT(pickupParameter2SliderChanged(int)));
	connect(pieceParamSliders[0], SIGNAL(valueChanged(int)),
		this, SLOT(pieceTemplateParameterSlider1Changed(int)));
	connect(pieceParamSliders[1], SIGNAL(valueChanged(int)),
		this, SLOT(pieceTemplateParameterSlider2Changed(int)));
	connect(pieceParamSliders[2], SIGNAL(valueChanged(int)),
		this, SLOT(pieceTemplateParameterSlider3Changed(int)));

	connect(pickupViewWidget, SIGNAL(someDataChanged()), this, SLOT(pickupViewWidgetDataChanged()));
	connect(this, SIGNAL(someDataChanged()), this, SLOT(updateEverything()));
}

void PieceEditorWidget :: pickupViewWidgetDataChanged()
{
	emit someDataChanged();
}

void PieceEditorWidget :: seedTemplates()
{
	char filename[100];

	for (int i = PickupTemplate::firstPickupTemplate(); i <= PickupTemplate::lastPickupTemplate(); ++i)
	{
		sprintf(filename, ":/images/pickuptemplate%d.png", i);
		PickupTemplateLibraryWidget *pktlw = new PickupTemplateLibraryWidget(
			QPixmap::fromImage(QImage(filename)), static_cast<PickupTemplate::Type>(i));
		pickupTemplateLibraryLayout->addWidget(pktlw);
	}

	for (int i = FIRST_PIECE_TEMPLATE; i <= LAST_PIECE_TEMPLATE; ++i)
	{
		sprintf(filename, ":/images/piecetemplate%d.png", i);
		PieceTemplateLibraryWidget *ptlw = new PieceTemplateLibraryWidget(
			QPixmap::fromImage(QImage(filename)), static_cast<PickupTemplate::Type>(i));
		pieceTemplateLibraryLayout->addWidget(ptlw);
	}
}

void PieceEditorWidget :: updateLibraryWidgetPixmaps(AsyncPieceLibraryWidget* w)
{
	//w->updateEyePosition(niceViewWidget->eyePosition());
	w->updatePixmap();
}

void PieceEditorWidget :: setPickupParameter(int param, int value)
{
	piece->pickup->setParameter(param, value);
	emit someDataChanged();
}

void PieceEditorWidget :: setPieceTemplate(PieceTemplate* t)
{
	piece->setTemplate(t);
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
	emit someDataChanged();	
}

Piece* PieceEditorWidget :: getPiece()
{
	return piece;
}




