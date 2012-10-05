
#include "pieceeditorwidget.h"

PieceEditorWidget :: PieceEditorWidget(QWidget* parent) : QWidget(parent)
{
	this->piece = new Piece(PieceTemplate::TUMBLER);

	this->pickupViewWidget = new PickupPlanEditorViewWidget(piece->pickup, this);	
	this->niceViewWidget = new NiceViewWidget(NiceViewWidget::PIECE_CAMERA_MODE, this);
	niceViewWidget->setGeometry(&geometry);

	setupLayout();
	setupConnections();
}

void PieceEditorWidget :: updateEverything()
{
	// update pickup stuff
	PickupTemplateLibraryWidget* pktlw;
	for (int i = 0; i < pickupTemplateLibraryLayout->count(); ++i)
	{
		pktlw = dynamic_cast<PickupTemplateLibraryWidget*>(
				dynamic_cast<QWidgetItem *>(pickupTemplateLibraryLayout->itemAt(i))->widget());
		if (pktlw->type == piece->pickup->getTemplateType())
			pktlw->setDependancy(true, IS_DEPENDANCY);
		else
			pktlw->setDependancy(false);
	}

	pickupViewWidget->setPickup(piece->pickup);

	unsigned int i = 0;
	for (; i < piece->pickup->getParameterCount(); ++i)
	{
		// get parameter info
		TemplateParameter tp;
		piece->pickup->getParameter(i, &tp);
		pickupParamLabels[i]->setText(tp.name.c_str());

		// temporarily disconnect the signals because 
		// it appears that (bugged?) setRange() calls valueChanged()
		disconnect(pickupParamSpinboxes[i], SIGNAL(valueChanged(int)),
			this, SLOT(pickupParameterSpinBoxChanged(int)));
		disconnect(pickupParamSliders[i], SIGNAL(valueChanged(int)),
			this, SLOT(pickupParameterSliderChanged(int)));
		pickupParamSpinboxes[i]->setRange(tp.lowerLimit, tp.upperLimit);
		pickupParamSpinboxes[i]->setValue(tp.value);
		pickupParamSliders[i]->setRange(tp.lowerLimit, tp.upperLimit);
		pickupParamSliders[i]->setValue(tp.value);
		connect(pickupParamSpinboxes[i], SIGNAL(valueChanged(int)),
			this, SLOT(pickupParameterSpinBoxChanged(int)));
		connect(pickupParamSliders[i], SIGNAL(valueChanged(int)),
			this, SLOT(pickupParameterSliderChanged(int)));

		pickupParamWidgets[i]->show();
	}
	for (; i < pickupParamWidgets.size(); ++i)
	{
		pickupParamWidgets[i]->hide();
	}

	// update piece stuff
	PieceTemplateLibraryWidget* ptlw;
	for (int i = 0; i < pieceTemplateLibraryLayout->count(); ++i)
	{
		ptlw = dynamic_cast<PieceTemplateLibraryWidget*>(
			dynamic_cast<QWidgetItem *>(pieceTemplateLibraryLayout->itemAt(i))->widget());
		if (ptlw->type == piece->getTemplateType())
			ptlw->setDependancy(true, IS_DEPENDANCY);
		else
			ptlw->setDependancy(false);
	}

	geometry.clear();
	mesher.generateMesh(piece, &geometry);
	niceViewWidget->repaint();

	i = 0;
	for (; i < piece->getParameterCount(); ++i)
	{
		TemplateParameter tp;
		piece->getParameter(i, &tp);
		pieceParamLabels[i]->setText(tp.name.c_str());
		// temporarily disconnect the signals because 
		// it appears that (bugged?) setRange() calls valueChanged()
		disconnect(pieceParamSliders[i], SIGNAL(valueChanged(int)),
			this, SLOT(pieceParameterSliderChanged(int)));
		pieceParamSliders[i]->setRange(tp.lowerLimit, tp.upperLimit);
		pieceParamSliders[i]->setValue(tp.value);
		connect(pieceParamSliders[i], SIGNAL(valueChanged(int)),
			this, SLOT(pieceParameterSliderChanged(int)));
		pieceParamWidgets[i]->show();
	}
	for (; i < pieceParamWidgets.size(); ++i)
	{
		pieceParamWidgets[i]->hide();
	}
}

void PieceEditorWidget :: pieceParameterSliderChanged(int)
{
	for (unsigned int i = 0; i < piece->getParameterCount(); ++i)
	{
		TemplateParameter tp;
		piece->getParameter(i, &tp);
		if (tp.value != pieceParamSliders[i]->value())
		{
			piece->setParameter(i, pieceParamSliders[i]->value());
			emit someDataChanged();
		}
	}
}

void PieceEditorWidget :: pickupParameterSpinBoxChanged(int)
{
	for (unsigned int i = 0; i < piece->pickup->getParameterCount(); ++i)
	{
		TemplateParameter tp;
		piece->pickup->getParameter(i, &tp);
                if (tp.value != pickupParamSpinboxes[i]->value())
                {
                        piece->pickup->setParameter(i, pickupParamSpinboxes[i]->value());
                        emit someDataChanged();
                }
	}
}

void PieceEditorWidget :: pickupParameterSliderChanged(int)
{
	for (unsigned int i = 0; i < piece->pickup->getParameterCount(); ++i)
	{
		TemplateParameter tp;
		piece->pickup->getParameter(i, &tp);
                if (tp.value != pickupParamSliders[i]->value())
                {
                        piece->pickup->setParameter(i, pickupParamSliders[i]->value());
                        emit someDataChanged();
                }
	}
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
	pickupParamSpinboxes.push_back(pickupParameter1SpinBox);
	QSlider* pickupParameter1Slider = new QSlider(Qt::Horizontal, pickupParameter1Widget);
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
	pickupParamSpinboxes.push_back(pickupParameter2SpinBox);
	QSlider* pickupParameter2Slider = new QSlider(Qt::Horizontal, pickupParameter2Widget);
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
		if (pktlw->type != piece->pickup->getTemplateType())
		{
			piece->pickup->setTemplateType(pktlw->type);
			emit someDataChanged();
		}
	}
	else if (ptlw != NULL)
	{
		if (ptlw->type != piece->getTemplateType())
		{
			piece->setTemplateType(ptlw->type);
			emit someDataChanged();
		}
	}
}

void PieceEditorWidget :: setupConnections()
{
	for (unsigned int i = 0; i < pickupParamSpinboxes.size(); ++i)
	{
		connect(pickupParamSpinboxes[i], SIGNAL(valueChanged(int)),
			this, SLOT(pickupParameterSpinBoxChanged(int)));
	}
	for (unsigned int i = 0; i < pickupParamSliders.size(); ++i)
	{
		connect(pickupParamSliders[i], SIGNAL(valueChanged(int)),
			this, SLOT(pickupParameterSliderChanged(int)));
	}
	for (unsigned int i = 0; i < pieceParamSliders.size(); ++i)
	{
		connect(pieceParamSliders[i], SIGNAL(valueChanged(int)),
			this, SLOT(pieceParameterSliderChanged(int)));
	}

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

	for (int i = PickupTemplate::firstSeedTemplate(); i <= PickupTemplate::lastSeedTemplate(); ++i)
	{
		sprintf(filename, ":/images/pickuptemplate%d.png", i);
		PickupTemplateLibraryWidget *pktlw = new PickupTemplateLibraryWidget(
			QPixmap::fromImage(QImage(filename)), static_cast<PickupTemplate::Type>(i));
		pickupTemplateLibraryLayout->addWidget(pktlw);
	}

	for (int i = PieceTemplate::firstSeedTemplate(); i <= PieceTemplate::lastSeedTemplate(); ++i)
	{
		sprintf(filename, ":/images/piecetemplate%d.png", i);
		PieceTemplateLibraryWidget *ptlw = new PieceTemplateLibraryWidget(
			QPixmap::fromImage(QImage(filename)), static_cast<PieceTemplate::Type>(i));
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
	emit someDataChanged();	
}

Piece* PieceEditorWidget :: getPiece()
{
	return piece;
}




