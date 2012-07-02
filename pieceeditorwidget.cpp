
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
        fillRuleComboBox->setCurrentIndex(pickupViewWidget->getFillRule()-1);

	// update pickup stuff
	int value = piece->pickup->getParameter(0);
	pickupParameter1SpinBox->setValue(value);
	pickupParameter1Slider->setValue(value);
	pickupViewWidget->setPickup(piece->pickup);
	overlayWidget->repaint();	
	underlayWidget->repaint();	

	for (int i = 0; i < pickupTemplateLibraryLayout->count(); ++i)
	{
		if (i + FIRST_PICKUP_TEMPLATE == piece->pickup->getTemplateType())
			highlightLibraryWidget(dynamic_cast<PickupTemplateLibraryWidget*>(
				dynamic_cast<QWidgetItem *>(pickupTemplateLibraryLayout->itemAt(i))->widget()));
		else
			unhighlightLibraryWidget(dynamic_cast<PickupTemplateLibraryWidget*>(
				dynamic_cast<QWidgetItem *>(pickupTemplateLibraryLayout->itemAt(i))->widget()));
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

	unsigned int i = 0;
	for (; i < piece->getTemplate()->parameterNames.size(); ++i)
	{
		pieceParamLabels[i]->setText(piece->getTemplate()->parameterNames[i]);
		pieceParamLabels[i]->show();
		pieceParamSliders[i]->setValue(piece->getTemplate()->parameterValues[i]);
		pieceParamSliders[i]->show();
	}
	for (; i < pieceParamLabels.size(); ++i)
	{
		pieceParamLabels[i]->hide();
		pieceParamSliders[i]->hide();
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
	int value = pieceTemplateParameter3Slider->sliderPosition();

	if (value == piece->getTemplate()->parameterValues[2])
		return;
	piece->getTemplate()->parameterValues[2] = value;
	emit someDataChanged();
}

void PieceEditorWidget :: pieceTemplateParameterSlider2Changed(int)
{
	int value = pieceTemplateParameter2Slider->sliderPosition();

	if (value == piece->getTemplate()->parameterValues[1])
		return;
	piece->getTemplate()->parameterValues[1] = value;
	emit someDataChanged();
}

void PieceEditorWidget :: pieceTemplateParameterSlider1Changed(int)
{
	int value = pieceTemplateParameter1Slider->sliderPosition();

	if (value == piece->getTemplate()->parameterValues[0])
		return;
	piece->getTemplate()->parameterValues[0] = value;
	emit someDataChanged();
}

void PieceEditorWidget :: pickupParameter1SpinBoxChanged(int)
{
	int value = pickupParameter1SpinBox->value();

	if (value == piece->pickup->getParameter(0))
		return;

	piece->pickup->setParameter(0, value);
	emit someDataChanged();
}

void PieceEditorWidget :: pickupParameter1SliderChanged(int)
{
	int value = pickupParameter1Slider->value();

	if (value == piece->pickup->getParameter(0))
		return;

	piece->pickup->setParameter(0, value);
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

        QHBoxLayout* fillRuleLayout = new QHBoxLayout(this);
        fillRuleComboBox = new QComboBox(this);
        fillRuleComboBox->addItem("Single");
        fillRuleComboBox->addItem("Every second");
        fillRuleComboBox->addItem("Every third");
        fillRuleComboBox->addItem("Group");
        fillRuleComboBox->addItem("All");
        fillRuleLayout->addWidget(new QLabel("Fill rule:", this), 0);
        fillRuleLayout->addWidget(fillRuleComboBox, 0);
        fillRuleLayout->addStretch(1);
        leftLayout->addLayout(fillRuleLayout);

	QHBoxLayout* overlayLayout = new QHBoxLayout(this);	
	overlayLayout->addWidget(new QLabel("Casing color:"));
	overlayWidget = new OverlayColorWidget(&(piece->pickup->overlayColor), this);
	overlayLayout->addWidget(overlayWidget);
	overlayLayout->addWidget(new QLabel("Underlay color:"));
	underlayWidget = new OverlayColorWidget(&(piece->pickup->underlayColor), this);
	overlayLayout->addWidget(underlayWidget);
	overlayLayout->addStretch(1);
	leftLayout->addLayout(overlayLayout);

	pickupTemplateParameter1Label = new QLabel(piece->pickup->getParameterName(0));
	pickupParameter1SpinBox = new QSpinBox(this);
	pickupParameter1SpinBox->setRange(6, 40);
	pickupParameter1SpinBox->setSingleStep(1);
	pickupParameter1SpinBox->setValue(1);
	QLabel* lp1label = new QLabel("6", this);
	pickupParameter1Slider = new QSlider(Qt::Horizontal, this);
	pickupParameter1Slider->setRange(6, 40);
	pickupParameter1Slider->setValue(1);
	QLabel* rp1label = new QLabel("40", this);

	QHBoxLayout* parameter1Layout = new QHBoxLayout(this);
	leftLayout->addLayout(parameter1Layout, 0);
	parameter1Layout->addWidget(pickupTemplateParameter1Label, 0);
	parameter1Layout->addWidget(pickupParameter1SpinBox, 0);
	parameter1Layout->addWidget(lp1label, 0);
	parameter1Layout->addWidget(pickupParameter1Slider, 1);
	parameter1Layout->addWidget(rp1label, 0);

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

	pieceTemplateParameter1Label = new QLabel(piece->getTemplate()->parameterNames[0]);
	pieceParamLabels.push_back(pieceTemplateParameter1Label);
	pieceTemplateParameter1Slider = new QSlider(Qt::Horizontal, this);
	pieceParamSliders.push_back(pieceTemplateParameter1Slider);
	pieceTemplateParameter1Slider->setRange(0, 100);
	pieceTemplateParameter1Slider->setSliderPosition(0);

	QHBoxLayout* pieceParameter1Layout = new QHBoxLayout(this);
	rightLayout->addLayout(pieceParameter1Layout);
	pieceParameter1Layout->addWidget(pieceTemplateParameter1Label);
	pieceParameter1Layout->addWidget(pieceTemplateParameter1Slider);

	pieceTemplateParameter2Label = new QLabel(piece->getTemplate()->parameterNames[1]);
	pieceParamLabels.push_back(pieceTemplateParameter2Label);
	pieceTemplateParameter2Slider = new QSlider(Qt::Horizontal, this);
	pieceParamSliders.push_back(pieceTemplateParameter2Slider);
	pieceTemplateParameter2Slider->setRange(0, 100);
	pieceTemplateParameter2Slider->setSliderPosition(0);

	QHBoxLayout* parameter2Layout = new QHBoxLayout(this);
	rightLayout->addLayout(parameter2Layout);
	parameter2Layout->addWidget(pieceTemplateParameter2Label);
	parameter2Layout->addWidget(pieceTemplateParameter2Slider);

	pieceTemplateParameter3Label = new QLabel(piece->getTemplate()->parameterNames[1]);
	pieceParamLabels.push_back(pieceTemplateParameter3Label);
	pieceTemplateParameter3Slider = new QSlider(Qt::Horizontal, this);
	pieceParamSliders.push_back(pieceTemplateParameter3Slider);
	pieceTemplateParameter3Slider->setRange(0, 100);
	pieceTemplateParameter3Slider->setSliderPosition(0);

	QHBoxLayout* parameter3Layout = new QHBoxLayout(this);
	rightLayout->addLayout(parameter3Layout);
	parameter3Layout->addWidget(pieceTemplateParameter3Label);
	parameter3Layout->addWidget(pieceTemplateParameter3Slider);

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

void PieceEditorWidget :: fillRuleComboBoxChanged(int)
{
        pickupViewWidget->setFillRule(fillRuleComboBox->currentIndex()+1);
}

void PieceEditorWidget :: setupConnections()
{
	connect(overlayWidget, SIGNAL(colorChanged()),
		this, SLOT(updateEverything()));
	connect(underlayWidget, SIGNAL(colorChanged()),
		this, SLOT(updateEverything()));
        connect(fillRuleComboBox, SIGNAL(currentIndexChanged(int)), 
		this, SLOT(fillRuleComboBoxChanged(int)));
	connect(pickupParameter1SpinBox, SIGNAL(valueChanged(int)),
		this, SLOT(pickupParameter1SpinBoxChanged(int)));
	connect(pickupParameter1Slider, SIGNAL(valueChanged(int)),
		this, SLOT(pickupParameter1SliderChanged(int)));
	connect(pieceTemplateParameter1Slider, SIGNAL(valueChanged(int)),
		this, SLOT(pieceTemplateParameterSlider1Changed(int)));
	connect(pieceTemplateParameter2Slider, SIGNAL(valueChanged(int)),
		this, SLOT(pieceTemplateParameterSlider2Changed(int)));
	connect(pieceTemplateParameter3Slider, SIGNAL(valueChanged(int)),
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
	for (int i = FIRST_PICKUP_TEMPLATE; i <= LAST_PICKUP_TEMPLATE; ++i)
	{
		sprintf(filename, ":/images/pickuptemplate%d.png", i);
		PickupTemplateLibraryWidget *pktlw = new PickupTemplateLibraryWidget(
			QPixmap::fromImage(QImage(filename)), i);
		pickupTemplateLibraryLayout->addWidget(pktlw);
	}

	for (int i = FIRST_PIECE_TEMPLATE; i <= LAST_PIECE_TEMPLATE; ++i)
	{
		sprintf(filename, ":/images/piecetemplate%d.png", i);
		PieceTemplateLibraryWidget *ptlw = new PieceTemplateLibraryWidget(
			QPixmap::fromImage(QImage(filename)), i);
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

void PieceEditorWidget :: setPickupTemplateType(int templateType)
{
	piece->pickup->setTemplateType(templateType);
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




