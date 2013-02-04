
#include "pickupplaneditorviewwidget.h"
#include "pieceeditorwidget.h"
#include "mainwindow.h"
#include <sstream>
#include "bubble.h"

PieceEditorWidget :: PieceEditorWidget(QWidget* parent) : QWidget(parent)
{
	resetPiece();

	this->pickupViewWidget = new PickupPlanEditorViewWidget(piece->pickup, this);	
	this->niceViewWidget = new NiceViewWidget(NiceViewWidget::PIECE_CAMERA_MODE, this);
	niceViewWidget->setGeometry(&geometry);

	setupLayout();
	setupConnections();
	updateEverything();
}

void PieceEditorWidget :: resetPiece()
{
	piece = new Piece(PieceTemplate::TUMBLER);
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
	if(viewCheckBox->isChecked())
		pickupViewWidget->setPickup(piece->pickup, true);
	else
		pickupViewWidget->setPickup(piece->pickup, false);
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

		pickupParamStacks[i]->setCurrentIndex(0); // show
	}
	for (; i < pickupParamStacks.size(); ++i)
	{
		pickupParamStacks[i]->setCurrentIndex(1); // hide
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

		if(viewCheckBox->isChecked())
			mesher.generateMesh(piece, &geometry, true);
		else
		{
			geometry.clear();
			mesher.generateMesh(piece, &geometry, false);
		}
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

		pieceParamStacks[i]->setCurrentIndex(0); // show
	}
	for (; i < pieceParamStacks.size(); ++i)
	{
		pieceParamStacks[i]->setCurrentIndex(1); // hide
	}
}

void PieceEditorWidget :: viewAllPieces()
{
	/*for(unsigned long i = 0; i < vecLayerComboBox.size(); i++)
	{
		getPiece()->pickup = &(vecLayerComboBox[i]);
		if(i==0)
			allGeometry = geometry;
		else
		{
			allGeometry.groups.insert(allGeometry.groups.end(),geometry.groups.begin(),geometry.groups.end());
			allGeometry.triangles.insert(allGeometry.triangles.end(), geometry.triangles.begin(), geometry.triangles.end());
			allGeometry.vertices.insert(allGeometry.vertices.end(), geometry.vertices.begin(), geometry.vertices.end());
		}
	}*/
	allLayersPiece = getPiece()->copy();
	allLayersPiece->pickup = (&(vecLayerComboBox[0]))->copy();
	//allLayersPiece->vecLayerVerticesPushBack(vecLayerComboBox[0].getVertices());
	mesher.generateMesh(allLayersPiece, &geometry, true);


	//todo instead loop i = 0
	if(viewCheckBox->isChecked())
	{
		if (vecLayerComboBox[0].subs[0].orientation == MURRINE_PICKUP_CANE_ORIENTATION)
		{
			if(allLayersPiece->getInnerZ() > (vecLayerComboBox[0].subs[0].location[2] - vecLayerComboBox[0].subs[0].length/2))
				allLayersPiece->setInnerZ(vecLayerComboBox[0].subs[0].location[2] - vecLayerComboBox[0].subs[0].length/2);
			if(allLayersPiece->getOuterZ() < (vecLayerComboBox[0].subs[0].location[2] + vecLayerComboBox[0].subs[0].length/2))
				allLayersPiece->setOuterZ(vecLayerComboBox[0].subs[0].location[2] + vecLayerComboBox[0].subs[0].length/2);
		}
		else
		{
			if(allLayersPiece->getInnerZ() > (vecLayerComboBox[0].subs[0].location[2] - vecLayerComboBox[0].subs[0].width/2))
				allLayersPiece->setInnerZ(vecLayerComboBox[0].subs[0].location[2] - vecLayerComboBox[0].subs[0].width/2);
			if(allLayersPiece->getOuterZ() < (vecLayerComboBox[0].subs[0].location[2] + vecLayerComboBox[0].subs[0].width/2))
				allLayersPiece->setOuterZ(vecLayerComboBox[0].subs[0].location[2] + vecLayerComboBox[0].subs[0].width/2);
		}

		allLayersPiece->vecLayerVerticesSetValue(vecLayerComboBox[0].getVertices(),0);

		for(unsigned long i = 1; i < vecLayerComboBox.size(); i++)
		{
			allLayersPiece->pickup->subs.insert(allLayersPiece->pickup->subs.end(),vecLayerComboBox[i].subs.begin(),vecLayerComboBox[i].subs.end());
			allLayersPiece->vecLayerVerticesSetValue(vecLayerComboBox[i].getVertices(),i);

			if (vecLayerComboBox[i].subs[0].orientation == MURRINE_PICKUP_CANE_ORIENTATION)
			{
				if(allLayersPiece->getInnerZ() > (vecLayerComboBox[i].subs[0].location[2] - vecLayerComboBox[i].subs[0].length/2))
					allLayersPiece->setInnerZ(vecLayerComboBox[i].subs[0].location[2] - vecLayerComboBox[i].subs[0].length/2);
				if(allLayersPiece->getOuterZ() < (vecLayerComboBox[i].subs[0].location[2] + vecLayerComboBox[i].subs[0].length/2))
					allLayersPiece->setOuterZ(vecLayerComboBox[i].subs[0].location[2] + vecLayerComboBox[i].subs[0].length/2);
			}
			else
			{
				if(allLayersPiece->getInnerZ() > (vecLayerComboBox[i].subs[0].location[2] - vecLayerComboBox[i].subs[0].width/2))
					allLayersPiece->setInnerZ(vecLayerComboBox[i].subs[0].location[2] - vecLayerComboBox[i].subs[0].width/2);
				if(allLayersPiece->getOuterZ() < (vecLayerComboBox[i].subs[0].location[2] + vecLayerComboBox[i].subs[0].width/2))
					allLayersPiece->setOuterZ(vecLayerComboBox[i].subs[0].location[2] + vecLayerComboBox[i].subs[0].width/2);
			}
		}
		//std::cout << "innerZ " << allLayersPiece->getInnerZ()*2.5 << " outerZ " << allLayersPiece->getOuterZ()*2.5;
		//std::cout << std::endl;
		mesher.generateMesh(allLayersPiece, &geometry, true);
	}
}

//calculates the number of vertices from a cane with the length 2
/*int PieceEditorWidget :: calculatePullVertices(PullPlan* plan, int vertices)
{
	if(plan->getCasingCount()>2)
		vertices= vertices+((plan->getCasingCount()-2)*1920); //add casing = 1920 vertices (cane with length 2)

	for(unsigned long i = 0; i < plan->subs.size(); i++) //check diameter
	{
		if(plan->subs[i].diameter<0.6)
			vertices = vertices+974; //small diameter
		else
		{
			if(plan->subs[i].diameter>=1.0)
				vertices = vertices+2918;//large diameter
			else
				vertices = vertices+1946;//middle diameter
		}
	}
*/
	/*switch(plan->getTemplateType())
	{
	case PullTemplate::BASE_CIRCLE : vertices = vertices+2918; break;
	case PullTemplate::BASE_SQUARE : vertices = vertices+2918; break;
	case PullTemplate::HORIZONTAL_LINE_CIRCLE : ; break;
	case PullTemplate::HORIZONTAL_LINE_SQUARE : ; break;
	case PullTemplate::TRIPOD : ; break;
	case PullTemplate::CROSS : ; break;
	case PullTemplate::SQUARE_OF_CIRCLES : ; break;
	case PullTemplate::SQUARE_OF_SQUARES : ; break;
	case PullTemplate::SURROUNDING_CIRCLE : ; break;
	case PullTemplate::CUSTOM : ; break;
	default : std::cout << "wrong PullPlan Template Type? (PieceEditorWidget::recurseCanes)" << endl;
	}*/
/*
	for(unsigned long i = 0; i< plan->subs.size();i++)
		calculatePullVertices(plan->subs[i].plan, vertices);


	return vertices;
}*/

/*void PieceEditorWidget :: recurseLayers(vector<PickupPlan*> vecLayers, vector<int> vecLayerTwist, int vertices)
{
	for(unsigned long i = 0; i < vecLayers.size(); i++) //in this case: column count == subs.size
	{
		for(unsigned long j = 0; j < vecLayers[i]->subs.size(); j++)
		{
			//calculatePullVertices(vecLayers[i]->subs[j].plan, vertices);

			switch(vecLayers[i]->getTemplateType())
			{
				case PickupTemplate::VERTICAL : vertices = vertices+calculatePullVertices(vecLayers[i]->subs[j].plan, vertices); break;
				case PickupTemplate::RETICELLO_VERTICAL_HORIZONTAL : ;break;
				case PickupTemplate::MURRINE_COLUMN : ;break;
				case PickupTemplate::VERTICALS_AND_HORIZONTALS : ;break;
				case PickupTemplate::VERTICAL_HORIZONTAL_VERTICAL : ;break;
				case PickupTemplate::VERTICAL_WITH_LIP_WRAP : ;break;
				case PickupTemplate::MURRINE_ROW : ;break;
				case PickupTemplate::MURRINE : ;break;
				default: cout << "Error in recursePickup. Wrong PickupTemplateType?";
			}
		}
		vecLayerTwist.push_back(vertices);
	}
}*/

void PieceEditorWidget :: pieceParameterSliderChanged(int)
{
	for (unsigned int i = 0; i < piece->getParameterCount(); ++i)
	{
		TemplateParameter tp;
		piece->getParameter(i, &tp);
		if (tp.value != pieceParamSliders[i]->value())
		{
			this->getPiece()->setParameter(i, pieceParamSliders[i]->value());
			if(i==2)
				this->getPiece()->vecLayerTwistSetValue(pieceParamSliders[2]->value(), layerComboBox->currentIndex());
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
	//Bubble sphere(10, 12, 24);
	//this->niceViewWidget->displayBubble(sphere);
	//this->niceViewWidget->drawBubble();
}

void PieceEditorWidget :: setupLayout()
{
	QGridLayout* editorLayout = new QGridLayout(this);
	this->setLayout(editorLayout);

	// two 3D views in the first row (and stretched to take up all the slack space
	editorLayout->addWidget(pickupViewWidget, 0, 0);
	editorLayout->addWidget(niceViewWidget, 0, 1);
	editorLayout->setRowStretch (0, 10);


	QWidget* editorWidget = new QWidget(this);
	QVBoxLayout* qvBoxEditorLayout = new QVBoxLayout(editorWidget);
	editorWidget->setLayout(qvBoxEditorLayout);

	pickupLayerViewLayout = new QHBoxLayout(editorWidget);
	layerLabel = new QLabel("Layers:");
	addLayerButton = new QPushButton("&+");
	removeLayerButton = new QPushButton("&-");
	viewCheckBox = new QCheckBox("View &all", this); //shortcut ALT and 'a'
	layerComboBox = new QComboBox(this);
	layerComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	layerComboBox->setInsertPolicy(QComboBox::InsertAtBottom);
	layerComboBox->addItem("Layer 1");
	pickupLayerViewLayout->addWidget(layerLabel, 1, 0);
	pickupLayerViewLayout->addWidget(addLayerButton, 1, 0);
	pickupLayerViewLayout->addWidget(removeLayerButton, 1, 0);
	pickupLayerViewLayout->addWidget(viewCheckBox, 1, 0);
	pickupLayerViewLayout->addWidget(layerComboBox, 1, 0);
	editorLayout->addLayout(pickupLayerViewLayout, 1, 0);

	// pickup template and piece template selectors in the second row
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
	editorLayout->addWidget(pickupTemplateLibraryScrollArea, 3, 0);

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
	editorLayout->addWidget(pieceTemplateLibraryScrollArea, 3, 1);

	// layouts containing pickup and piece template parameters in the third row

	// Pickup parameter layout
	QVBoxLayout* pickupParamLayout = new QVBoxLayout(this);
	editorLayout->addLayout(pickupParamLayout, 4, 0);

	pickupParamStacks.push_back(new QStackedWidget(this));
	QWidget* pickupParameter1Widget = new QWidget(pickupParamStacks[0]);
	QLabel* pickupParameter1Label = new QLabel("Parameter 1");
	pickupParamLabels.push_back(pickupParameter1Label);
	QSpinBox* pickupParameter1SpinBox = new QSpinBox(pickupParameter1Widget);
	pickupParamSpinboxes.push_back(pickupParameter1SpinBox);
	QSlider* pickupParameter1Slider = new QSlider(Qt::Horizontal, pickupParameter1Widget);
	pickupParamSliders.push_back(pickupParameter1Slider);
	QHBoxLayout* pickupParameter1Layout = new QHBoxLayout(pickupParameter1Widget);
	pickupParameter1Widget->setLayout(pickupParameter1Layout);
	pickupParameter1Layout->setContentsMargins(0, 0, 0, 0);
	pickupParameter1Layout->addWidget(pickupParameter1Label);
	pickupParameter1Layout->addWidget(pickupParameter1SpinBox);
	pickupParameter1Layout->addWidget(pickupParameter1Slider);
	pickupParamStacks[0]->addWidget(pickupParameter1Widget);
	pickupParamStacks[0]->addWidget(new QWidget(pickupParamStacks[0]));
	pickupParamLayout->addWidget(pickupParamStacks[0]);

	pickupParamStacks.push_back(new QStackedWidget(this));
	QWidget* pickupParameter2Widget = new QWidget(pickupParamStacks[1]);
	QLabel* pickupParameter2Label = new QLabel("Parameter 2");
	pickupParamLabels.push_back(pickupParameter2Label);
	QSpinBox* pickupParameter2SpinBox = new QSpinBox(pickupParameter2Widget);
	pickupParamSpinboxes.push_back(pickupParameter2SpinBox);
	QSlider* pickupParameter2Slider = new QSlider(Qt::Horizontal, pickupParameter2Widget);
	pickupParamSliders.push_back(pickupParameter2Slider);
	QHBoxLayout* pickupParameter2Layout = new QHBoxLayout(pickupParameter2Widget);
	pickupParameter2Widget->setLayout(pickupParameter2Layout);
	pickupParameter2Layout->setContentsMargins(0, 0, 0, 0);
	pickupParameter2Layout->addWidget(pickupParameter2Label);
	pickupParameter2Layout->addWidget(pickupParameter2SpinBox);
	pickupParameter2Layout->addWidget(pickupParameter2Slider);
	pickupParamStacks[1]->addWidget(pickupParameter2Widget);
	pickupParamStacks[1]->addWidget(new QWidget(pickupParamStacks[1]));
	pickupParamLayout->addWidget(pickupParamStacks[1]);

	QLabel* pieceEditorDescriptionLabel = new QLabel("Pickup editor - drag canes in.", this);
	pieceEditorDescriptionLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	pickupParamLayout->addWidget(pieceEditorDescriptionLabel);

	// Piece parameter layout 
	QVBoxLayout* pieceParamLayout = new QVBoxLayout(this);
	editorLayout->addLayout(pieceParamLayout, 4, 1);

	pieceParamStacks.push_back(new QStackedWidget(this));
	QWidget* pieceParameter1Widget = new QWidget(pieceParamStacks[0]);
	QLabel* pieceParameter1Label = new QLabel("Parameter 1");
	pieceParamLabels.push_back(pieceParameter1Label);
	QSlider* pieceParameter1Slider = new QSlider(Qt::Horizontal, pieceParameter1Widget);
	pieceParamSliders.push_back(pieceParameter1Slider);
	QHBoxLayout* pieceParameter1Layout = new QHBoxLayout(pieceParameter1Widget);
	pieceParameter1Widget->setLayout(pieceParameter1Layout);
	pieceParameter1Layout->setContentsMargins(0, 0, 0, 0);
	pieceParameter1Layout->addWidget(pieceParameter1Label);
	pieceParameter1Layout->addWidget(pieceParameter1Slider);
	pieceParamStacks[0]->addWidget(pieceParameter1Widget);
	pieceParamStacks[0]->addWidget(new QWidget());
	pieceParamLayout->addWidget(pieceParamStacks[0]);

	pieceParamStacks.push_back(new QStackedWidget(this));
	QWidget* pieceParameter2Widget = new QWidget(pieceParamStacks[1]);
	QLabel* pieceTemplateParameter2Label = new QLabel("Parameter 2");
	pieceParamLabels.push_back(pieceTemplateParameter2Label);
	QSlider* pieceTemplateParameter2Slider = new QSlider(Qt::Horizontal, pieceParameter2Widget);
	pieceParamSliders.push_back(pieceTemplateParameter2Slider);
	QHBoxLayout* pieceParameter2Layout = new QHBoxLayout(pieceParameter2Widget);
	pieceParameter2Widget->setLayout(pieceParameter2Layout);
	pieceParameter2Layout->setContentsMargins(0, 0, 0, 0);
	pieceParameter2Layout->addWidget(pieceTemplateParameter2Label);
	pieceParameter2Layout->addWidget(pieceTemplateParameter2Slider);
	pieceParamStacks[1]->addWidget(pieceParameter2Widget);
	pieceParamStacks[1]->addWidget(new QWidget());
	pieceParamLayout->addWidget(pieceParamStacks[1]);

	pieceParamStacks.push_back(new QStackedWidget(this));
	QWidget* pieceParameter3Widget = new QWidget(pieceParamStacks[2]);
	QLabel* pieceParameter3Label = new QLabel("Parameter 3");
	pieceParamLabels.push_back(pieceParameter3Label);
	QSlider* pieceParameter3Slider = new QSlider(Qt::Horizontal, pieceParameter3Widget);
	pieceParamSliders.push_back(pieceParameter3Slider);
	QHBoxLayout* pieceParameter3Layout = new QHBoxLayout(pieceParameter3Widget);
	pieceParameter3Widget->setLayout(pieceParameter3Layout);
	pieceParameter3Layout->setContentsMargins(0, 0, 0, 0);
	pieceParameter3Layout->addWidget(pieceParameter3Label);
	pieceParameter3Layout->addWidget(pieceParameter3Slider);
	pieceParamStacks[2]->addWidget(pieceParameter3Widget);
	pieceParamStacks[2]->addWidget(new QWidget());
	pieceParamLayout->addWidget(pieceParamStacks[2]);
	
	QLabel* niceViewDescriptionLabel = new QLabel("Piece editor.", this);
	niceViewDescriptionLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	pieceParamLayout->addWidget(niceViewDescriptionLabel);
}

void PieceEditorWidget :: setMainWindow(MainWindow* MW)
{
	MainWin = MW;
}

void PieceEditorWidget :: addLayer()
{
	TemplateParameter templPara;

	if((layerComboBox->count()==1)&&(layerComboBox->currentIndex()==0))
	{
		vecLayerComboBox.push_back(*(getPiece()->pickup->copy()));
		getPiece()->getParameter(2, &templPara);
	}
	this->getPiece()->vecLayerTwistPushBack(0);
	this->getPiece()->vecLayerVerticesPushBack(0);
	PickupPlan* pp = MainWin->newPickupPlan();
	vecLayerComboBox.push_back(*pp);
	getPiece()->pickup = pp;
	std::stringstream layerName;
	layerName << "Layer " << (layerComboBox->count())+1;
	layerComboBox->addItem(QString::fromStdString(layerName.str()));
	layerComboBox->setCurrentIndex(layerComboBox->count()-1);

	someDataChanged();
}

void PieceEditorWidget :: changeLayer()
{
	emit this->someDataChanged();
	cout << "index " << getPiece()->vecLayerTwistGetValue(layerComboBox->currentIndex());
	cout << endl;
	getPiece()->pickup = &(vecLayerComboBox[layerComboBox->currentIndex()]);
	getPiece()->setParameter(2, getPiece()->vecLayerTwistGetValue(layerComboBox->currentIndex()));
	this->pickupViewWidgetDataChanged();
	emit this->someDataChanged();
}

void PieceEditorWidget :: removeLayer()
{
	if(layerComboBox->count()>1)
	{
		if(layerComboBox->currentIndex()==layerComboBox->count()-1)
		{
			layerComboBox->removeItem(layerComboBox->currentIndex());
			vecLayerComboBox.erase(vecLayerComboBox.begin()+layerComboBox->currentIndex());
		}
		else
		{
			;
		}
		getPiece()->pickup = &(vecLayerComboBox[layerComboBox->currentIndex()]);
		someDataChanged();
	}
}

void PieceEditorWidget :: combineLayers()
{
	if(vecLayerComboBox.size()>1)
	{
		if(viewCheckBox->isChecked())
		{
			this->pickupViewWidget->setViewAll(true);
			this->pickupViewWidget->setViewAllPickupEditorView();
			addLayerButton->setEnabled(false);
			removeLayerButton->setEnabled(false);
			layerComboBox->setEnabled(false);
			for (unsigned int i = 0; i < pickupParamSpinboxes.size(); ++i)
				pickupParamSpinboxes[i]->setEnabled(false);
			for (unsigned int i = 0; i < pickupParamSliders.size(); ++i)
				pickupParamSliders[i]->setEnabled(false);
			for (unsigned int i = 0; i < pieceParamSliders.size(); ++i)
				pieceParamSliders[i]->setEnabled(false);

			viewCheckBox->setChecked(false);
			getPiece()->pickup = (&(vecLayerComboBox[0]))->copy();
			someDataChanged();
			viewCheckBox->setChecked(true);

			if(vecLayerComboBox.size()>1)
			{
				for(unsigned long i = 0; i < vecLayerComboBox.size(); i++)
				{
					getPiece()->pickup = (&(vecLayerComboBox[i]));//->copy();
					getPiece()->pickup->setViewAll(true);
					someDataChanged();
					getPiece()->pickup->viewLayer(i);
					someDataChanged();
				}
				viewAllPieces();
			}
		}
		else
		{
			for(unsigned long i = 0; i < vecLayerComboBox.size(); i++)
				(&(vecLayerComboBox[i]))->setViewAll(false);//->copy();

			this->pickupViewWidget->setViewAll(false);
			addLayerButton->setEnabled(true);
			removeLayerButton->setEnabled(true);
			layerComboBox->setEnabled(true);
			for (unsigned int i = 0; i < pickupParamSpinboxes.size(); ++i)
				pickupParamSpinboxes[i]->setEnabled(true);
			for (unsigned int i = 0; i < pickupParamSliders.size(); ++i)
				pickupParamSliders[i]->setEnabled(true);
			for (unsigned int i = 0; i < pieceParamSliders.size(); ++i)
				pieceParamSliders[i]->setEnabled(true);
			layerComboBox->setCurrentIndex(layerComboBox->count()-1);
			this->pickupViewWidget->resetPickupEditorView();
			changeLayer();
		}
	}
}

void PieceEditorWidget :: mousePressEvent(QMouseEvent* event)
{
	if(piece->pickup->getViewAll())
	{
		event->ignore();
		return;
	}
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

	connect(layerComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(changeLayer()));
	connect(pickupViewWidget, SIGNAL(someDataChanged()), this, SLOT(pickupViewWidgetDataChanged()));
	connect(this, SIGNAL(someDataChanged()), this, SLOT(updateEverything()));
	connect(addLayerButton, SIGNAL(pressed()), this, SLOT(addLayer()));
	connect(removeLayerButton, SIGNAL(pressed()), this, SLOT(removeLayer()));
	connect(viewCheckBox, SIGNAL(stateChanged(int)), this, SLOT(combineLayers()));
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
