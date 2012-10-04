

#include "mainwindow.h"

MainWindow :: MainWindow()
{
	setWindowTitle(windowTitle());
	centralWidget = new QWidget(this);
	this->setCentralWidget(centralWidget);

	centralLayout = new QHBoxLayout(centralWidget);
	setupLibrary();
	setupEditors();
	setupMenus();
	setupConnections();

	seedEverything();
	setViewMode(EMPTY_VIEW_MODE);
	show();
}

//markus
#ifdef UNDEF 
void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
	QMenu menu(this);
	menu.exec(event->globalPos());
}

int MainWindow::getM(){
	return m;
}

void MainWindow::setM(int _m){
	m = _m;
}

void MainWindow::prepareJson(PullPlan* plan, Json::Value* root1, string nestedValue) {
	//prepare json
	Json::Value *nested_value1 = new Json::Value;
	//check dependency from piece to plan to save only dependent canes
	Json::Value *value9 = new Json::Value;
	*value9 = (plan->getTemplateType());
	Json::Value *value10 = new Json::Value;
	*value10 = (plan->hasSquareCasing());
	Json::Value *value11 = new Json::Value;
	*value11 = (plan->getCasingCount());
	Json::Value *value23 = new Json::Value;
	*value23 = (plan->getTwist());

	Json::Value *nested_value = new Json::Value;

	std::stringstream *templateTypeSstr = new std::stringstream;
	*templateTypeSstr << "templatetype" << getM();
	string templateType = (*templateTypeSstr).str();

	std::stringstream *hasSquareCasingSstr = new std::stringstream;
	*hasSquareCasingSstr << "hasSquareCasing" <<getM();
	string hasSquareCasing = (*hasSquareCasingSstr).str();

	std::stringstream *casingcountSstr = new std::stringstream;
	*casingcountSstr << "casingcount" <<getM();
	string casingCount = (*casingcountSstr).str();

	std::stringstream *twistsSstr = new std::stringstream;
	*twistsSstr << "twists" <<getM();
	string twists = (*twistsSstr).str();

	(*nested_value)[templateType] = *value9;
	(*nested_value)[hasSquareCasing] = *value10;
	(*nested_value)[casingCount] = *value11;
	(*nested_value)[twists] = *value23;

	std::stringstream *pullplannrSstr = new std::stringstream;
	*pullplannrSstr << "pullplannr" <<getM();
	string pullPlanNr = (*pullplannrSstr).str();

	(*nested_value1)[pullPlanNr] = *nested_value;

	//for (unsigned int j=0; j< plan->subs.size() ;j++){
	//    SubpullTemplate* sub = &(plan->subs[j]);
	//    cout << sub->group;
	//}

	for (unsigned int i=0; i<(plan->getCasingCount()); i++){
		Json::Value *value13 = new Json::Value;
		*value13 = (plan->getCasingThickness(i));
		Json::Value *value14 = new Json::Value;
		*value14 = (plan->getCasingShape(i));
		Json::Value *value15 = new Json::Value;
		*value15 = (plan->getCasingColor(i)->getName());
		Json::Value *value16 = new Json::Value;
		*value16 = ((plan->getCasingColor(i)->getColor())->operator [](0));
		Json::Value *value17 = new Json::Value;
		*value17 = ((plan->getCasingColor(i)->getColor())->operator [](1));
		Json::Value *value18 = new Json::Value;
		*value18 = ((plan->getCasingColor(i)->getColor())->operator [](2));
		Json::Value *value19 = new Json::Value;
		*value19 = ((plan->getCasingColor(i)->getColor())->operator [](3));
		//Json::Value value12(plan->subs.size());

		Json::Value *nested_value = new Json::Value;
		(*nested_value)["CasingThickness"] = *value13;
		(*nested_value)["CasingShape"] = *value14;
		(*nested_value)["getCasingColor"] = *value15;
		(*nested_value)["R"] = *value16;
		(*nested_value)["G"] = *value17;
		(*nested_value)["B"] = *value18;
		(*nested_value)["alpha"] = *value19;
		//nested_value["subsSize"] = value12;

		std::stringstream *casingcountSstr = new std::stringstream;
		*casingcountSstr << "casingcount" <<i;
		string casingCount = (*casingcountSstr).str();
		(*nested_value1)[casingCount] = *nested_value;
	}

	Json::Value *value20 = new Json::Value;
	*value20 =(plan->getParameterCount());

	std::stringstream *parametercountSstr = new std::stringstream;
	*parametercountSstr << "Pullparametercount" <<getM();
	string parameterCount = (*parametercountSstr).str();

	(*nested_value1)[parameterCount] = *value20;

	for(unsigned int i=0; i<(plan->getParameterCount()); i++){
		Json::Value *value21 = new Json::Value;
		Json::Value *value22 = new Json::Value;
		*value21 = (plan->getParameterName(i));
		*value22 = (plan->getParameter(i));
		Json::Value *nested_value = new Json::Value;

		std::stringstream *parameternameSstr = new std::stringstream;
		*parameternameSstr << "parametername" <<i;
		string parameterName = (*parameternameSstr).str();

		std::stringstream *parameterValueSstr = new std::stringstream;
		*parameterValueSstr << "parametervalue" <<i;
		string parameterValue = (*parameterValueSstr).str();

		std::stringstream *parameterSstr = new std::stringstream;
		*parameterSstr << "parameter" <<i;
		string parameter = (*parameterSstr).str();

		(*nested_value)[parameterName] = *value21;
		(*nested_value)[parameterValue] = *value22;
		(*nested_value1)[parameter] = *nested_value;
	}
	std::stringstream *Sstr = new std::stringstream;
	*Sstr << nestedValue <<getM();
	string name = (*Sstr).str();

	(*root1)[name] = *nested_value1;
}

void MainWindow::buildCaneTree(PullPlan* plan, PullPlan* planPrev, map<PullPlan*,int> cane, Json::Value* root1) {
	if(cane.find(plan)->first) {
		setM((getM())+1);
		std::stringstream *Sstr = new std::stringstream;
		*Sstr << "subcane" <<getM();
		string name = (*Sstr).str();

		(*root1)[name] = cane.find(plan)->second;
		//setM((getM())+1);
	}
	else {
		if((plan->subs.size())==0){
			//if case: no canes in library
			if (planPrev!=NULL)
				vector< PullPlan* > caneVec;
				//cane identifier
				caneVec.push_back(plan);
				//prevPointer
				caneVec.push_back(planPrev);
				////special case: buildCaneMap calls buildCaneTree
				caneVec.push_back(NULL);
				setM((getM())+1);
				prepareJson(plan, root1, "subcane");
			}
		}
		else{
			vector< PullPlan > caneVec;
			//cane pointer (identifier)
			caneVec.push_back(*plan);
			if (planPrev==NULL){
				caneVec.push_back(NULL);
				}
			else{
				//prevPointer
				caneVec.push_back(*planPrev);
			}
			for(int i=0; i<int(plan->subs.size()); i++){
				SubpullTemplate subplan = plan->subs[i];
				//pointer next
				caneVec.push_back(*subplan.plan);
				//add subcane to ma
				setM(1+getM());
				cane.insert(pair<PullPlan*, int>(subplan.plan,getM()));
				//go down and build tree
				buildCaneTree(subplan.plan, plan, cane, root1);
				prepareJson(plan, root1, "subcane");
            		}
		}
	}
}

void MainWindow::buildCaneMap(PullPlan* plan, Json::Value* root1, map<PullPlan*,int> cane){
	if(cane.find(plan)->first){
		setM((getM())+1);
		std::stringstream *Sstr = new std::stringstream;
		*Sstr << "cane" <<getM();
		string name = (*Sstr).str();

		(*root1)[name] = cane.find(plan)->second;
		//setM((getM())+1);
	}
	else{
		//add first cane (pickup->sub)
		setM(1+getM());
		cane.insert(pair<PullPlan*, int>(plan,getM()));
		buildCaneTree(plan, NULL, cane, root1);
		prepareJson(plan, root1, "cane");
	}
}

QString MainWindow::writeJson(Json::Value root){
	Json::StyledWriter writer;
	std::string outputConfig = writer.write( root );
	QString output = QString::fromStdString(outputConfig);
	return output;
}

#endif 

void MainWindow::open()
{
#ifdef UNDEF
	//open file dialog
	QString filename = QFileDialog::getOpenFileName(this, tr("Open Document"), 
		QDir::currentPath(), tr("VirtualGlass file (*.glass);;All files (*.*)") );
#endif
}

void MainWindow::save(){
    //open
}

void MainWindow::saveAs(){

#ifdef UNDEF
	//save file dialog
	QString filename = QFileDialog::getSaveFileName(this, tr("Save your glass piece"), QDir::currentPath(), tr("VirtualGlass (*.glass)") );
	//improve: prevent character set error in filename
	//improve: empty file name -> "no savefile choosen"

	QFile saveFile(filename);
	saveFile.open(QIODevice::WriteOnly | QIODevice::Text);
	QTextStream fileOutput(&saveFile);


	//read version and date from version.txt; write this into json file (root0)
	//first line; versionNo and date
	ifstream readHdl;
	//date has always 12 characters
	char date[11];
	char versionNo[4];

	readHdl.open(":/version.txt");
	readHdl.getline(versionNo,4,'\n');
	string versionStr;
	versionStr.assign(versionNo, 4);
	readHdl.getline(date,11,'\n');
	string dateStr = date;
	readHdl.close();
	//write json file (root0)
	Json::Value root0;
	Json::Value value1(versionStr);
	Json::Value value2(dateStr);

	root0["date"] = value1;
	root0["version"] = value2;
	fileOutput << writeJson(root0);

	//write pickuptemplate into json file (root1)
	Json::Value root1;
	Json::Value value3((pieceEditorWidget->getPiece())->pickup->getTemplateType());
	Json::Value value4((pieceEditorWidget->getPiece())->pickup->getParameterCount());
	// Json::Value value5((pieceEditorWidget->getPiece())->pickup->getParameterName(i));
	//Json::Value value6((pieceEditorWidget->getPiece())->pickup->getParameter(i));
	Json::Value value7(((pieceEditorWidget->getPiece())->pickup->overlayGlassColor->getName()));
	Json::Value value8(((pieceEditorWidget->getPiece())->pickup->underlayGlassColor->getName()));

	root1["PickupTemplateType"] = value3;
	root1["PickupParameterCount"] = value4;
	//root1["PickupParameterName"] = value5;
	//root1["PickupParameterValue"] = value6;
	root1["PickupoverlayGlassColor"] = value7;
	root1["PickupunderlayGlassColor"] = value8;

	//##pointer
	//fileOutput << "getPiece " << pieceEditorWidget->getPiece() << "\n";
	//fileOutput << "getTemplate " << (pieceEditorWidget->getPiece())->getTemplate()<< "\n";
	//fileOutput << "getPlan/pull" << pullPlanEditorWidget->getPlan()<< "\n";

	//pullupplan
	//loop over all canes
	setM(0);
	map<PullPlan*,int> cane;
	AsyncPullPlanLibraryWidget* pplw;
	for (int i = 0; i < pullPlanLibraryLayout->count(); ++i){
		pplw = dynamic_cast<AsyncPullPlanLibraryWidget*>(
			dynamic_cast<QWidgetItem *>(pullPlanLibraryLayout->itemAt(i))->widget());
		PullPlan* plan = pplw->getPullPlan();
		//        setM(0);
		buildCaneMap(plan, &root1, cane);
	}
	fileOutput << writeJson(root1);
	fileOutput << "eof";

	saveFile.close();
#endif 
}

void MainWindow::setupMenus()
{
	//file:open
	openAction = new QAction("&Open", this);
	openAction->setShortcuts(QKeySequence::Open);
	openAction->setStatusTip("Open an existing file");

	//file:save
	saveAction = new QAction("&Save", this);
	saveAction->setShortcuts(QKeySequence::Save);
	saveAction->setStatusTip("Save the document to disk");

	//file:saveAs
	saveAsAction = new QAction("&SaveAs", this);
	saveAsAction->setShortcuts(QKeySequence::SaveAs);
	saveAsAction->setStatusTip("Save the document to disk");

	//File menu
	fileMenu = menuBar()->addMenu("&File"); //create File menu
	fileMenu->addAction(openAction); //add openButton
	fileMenu->addAction(saveAction); //add saveButton
	fileMenu->addAction(saveAsAction); //add saveAsButton

	//examples:random:simple cane
	randomSimpleCaneAction = new QAction("&Simple Cane", this);
	randomSimpleCaneAction->setStatusTip("Randomly generate a simple example cane.");

	//examples:random:simple piece
	randomSimplePieceAction = new QAction("&Simple Piece", this);
	randomSimplePieceAction->setStatusTip("Randomly generate a simple example piece.");

	//examples:random:complex cane
	randomComplexCaneAction = new QAction("&Complex Cane", this);
	randomComplexCaneAction->setStatusTip("Ranomly generate a complex example cane.");

	//examples:random:complex piece
	randomComplexPieceAction = new QAction("&Complex Piece", this);
	randomComplexPieceAction->setStatusTip("Randomly generate a complex example piece.");

	// Examples menu and Examples:Random menu
	examplesMenu = menuBar()->addMenu("&Examples"); //create randomize menu
	randomExamplesMenu = examplesMenu->addMenu("&Random");
	randomExamplesMenu->addAction(randomSimpleCaneAction);
	randomExamplesMenu->addAction(randomSimplePieceAction);
	randomExamplesMenu->addAction(randomComplexCaneAction);
	randomExamplesMenu->addAction(randomComplexPieceAction);

	// toggle depth peeling
	depthPeelAction = new QAction(tr("&Depth peeling"), this);
	depthPeelAction->setCheckable(true);
	depthPeelAction->setChecked(NiceViewWidget::peelEnable);
	depthPeelAction->setStatusTip(tr("Toggle high-quality transparency rendering in 3D views"));

	// Performance menu
	perfMenu = menuBar()->addMenu(tr("Performance"));
	perfMenu->addAction(depthPeelAction);	
}


void MainWindow :: setViewMode(enum ViewMode _mode)
{
	editorStack->setCurrentIndex(_mode);
	copyColorBarButton->setEnabled(false);
	copyPullPlanButton->setEnabled(false);
	copyPieceButton->setEnabled(false);
	switch (_mode)
	{
		case EMPTY_VIEW_MODE:
			// leave all copy buttons disabled
			break;
		case COLORBAR_VIEW_MODE:
			copyColorBarButton->setEnabled(true);
			break;
		case PULLPLAN_VIEW_MODE:
			copyPullPlanButton->setEnabled(true);
			break;
		case PIECE_VIEW_MODE:
			copyPieceButton->setEnabled(true);
			break;
	}
	emit someDataChanged();
}

QString MainWindow :: windowTitle()
{
	QString title = tr("VirtualGlass");
	QFile inFile(":/version.txt");
	if (inFile.open(QIODevice::ReadOnly)) {
		QTextStream in(&inFile);
		QString revision = in.readLine();
		QString date = in.readLine();
		title += " - r" + revision + " built on " + date;
	}
	return title;
}

void MainWindow :: seedEverything()
{
	// Load color stuff
	setViewMode(COLORBAR_VIEW_MODE);
	colorEditorWidget->seedColors();
	emit someDataChanged();

	// Load pull template types
	setViewMode(PULLPLAN_VIEW_MODE);
	pullPlanEditorWidget->seedTemplates();

	// Load pickup and piece template types
	setViewMode(PIECE_VIEW_MODE);
	pieceEditorWidget->seedTemplates();
}

void MainWindow :: unhighlightAllLibraryWidgets()
{
	QLayoutItem* w;
	for (int j = 0; j < colorBarLibraryLayout->count(); ++j)
	{
		w = colorBarLibraryLayout->itemAt(j);
		static_cast<AsyncColorBarLibraryWidget*>(w->widget())->setDependancy(false); 
	}
	for (int j = 0; j < pullPlanLibraryLayout->count(); ++j)
	{
		w = pullPlanLibraryLayout->itemAt(j);
		static_cast<AsyncPullPlanLibraryWidget*>(w->widget())->setDependancy(false); 
	}
	for (int j = 0; j < pieceLibraryLayout->count(); ++j)
	{
		w = pieceLibraryLayout->itemAt(j);
		static_cast<AsyncPieceLibraryWidget*>(w->widget())->setDependancy(false); 
	}
}

void MainWindow :: keyPressEvent(QKeyEvent* e)
{
	if (e->key() == Qt::Key_Backspace || e->key() == Qt::Key_Delete)
		deleteCurrentEditingObject();
}

void MainWindow :: deleteCurrentEditingObject()
{
	QLayoutItem* w;

	switch (editorStack->currentIndex())
	{
		case COLORBAR_VIEW_MODE:
		{
			if (colorBarLibraryLayout->count() == 1)
				return;

			int i;
			for (i = 0; i < colorBarLibraryLayout->count(); ++i)
			{
				w = colorBarLibraryLayout->itemAt(i);
				GlassColor* gc = dynamic_cast<AsyncColorBarLibraryWidget*>(w->widget())->getGlassColor();
				if (gc == colorEditorWidget->getGlassColor())
				{
					if (glassColorIsDependancy(gc))
					{
						QMessageBox msgBox;
						msgBox.setText("This color cannot be deleted: other objects use it.");
						msgBox.exec();
					}
					else
					{
						// this may be a memory leak, the library widget is never explicitly deleted
						w = colorBarLibraryLayout->takeAt(i); 
						delete w->widget();
						delete w;
					}
					break;	
				}
			}

			colorEditorWidget->setGlassColor(dynamic_cast<AsyncColorBarLibraryWidget*>(colorBarLibraryLayout->itemAt(
					MIN(colorBarLibraryLayout->count()-1, i))->widget())->getGlassColor());
			emit someDataChanged();
			break;
		}
		case PULLPLAN_VIEW_MODE:
		{
			if (pullPlanLibraryLayout->count() == 1)
				return;

			int i;
			for (i = 0; i < pullPlanLibraryLayout->count(); ++i)
			{
				w = pullPlanLibraryLayout->itemAt(i);
				PullPlan* p = dynamic_cast<AsyncPullPlanLibraryWidget*>(w->widget())->getPullPlan();
				if (p == pullPlanEditorWidget->getPlan())
				{
					if (pullPlanIsDependancy(p))
					{
						QMessageBox msgBox;
						msgBox.setText("This cane cannot be deleted: other objects use it.");
						msgBox.exec();
					}
					else
					{
						// this may be a memory leak, the library widget is never explicitly deleted
						w = pullPlanLibraryLayout->takeAt(i);
						delete w->widget();
						delete w;
					}
					break;
				}
			}

			pullPlanEditorWidget->setPlan(dynamic_cast<AsyncPullPlanLibraryWidget*>(pullPlanLibraryLayout->itemAt(
					MIN(pullPlanLibraryLayout->count()-1, i))->widget())->getPullPlan());
			emit someDataChanged();
			break;
		}
		case PIECE_VIEW_MODE:
		{
			if (pieceLibraryLayout->count() == 1)
				return;

			int i;
			for (i = 0; i < pieceLibraryLayout->count(); ++i)
			{
				w = pieceLibraryLayout->itemAt(i);
				Piece* p = dynamic_cast<AsyncPieceLibraryWidget*>(w->widget())->getPiece();
				if (p == pieceEditorWidget->getPiece())
				{
					// this may be a memory leak, the library widget is never explicitly deleted
					w = pieceLibraryLayout->takeAt(i);
					delete w->widget();
					delete w;
					break;
				}
			}

			pieceEditorWidget->setPiece(dynamic_cast<AsyncPieceLibraryWidget*>(pieceLibraryLayout->itemAt(
					MIN(pieceLibraryLayout->count()-1, i))->widget())->getPiece());
			emit someDataChanged();
			break;
		}
	}
	
}

void MainWindow :: mouseReleaseEvent(QMouseEvent* event)
{
	// If this is a drag and not the end of a click, don't process (dropEvent will do it instead)
	if (isDragging && (event->pos() - dragStartPosition).manhattanLength() > QApplication::startDragDistance()) return;

	AsyncColorBarLibraryWidget* cblw = dynamic_cast<AsyncColorBarLibraryWidget*>(childAt(event->pos()));
	AsyncPullPlanLibraryWidget* plplw = dynamic_cast<AsyncPullPlanLibraryWidget*>(childAt(event->pos()));
	AsyncPieceLibraryWidget* plw = dynamic_cast<AsyncPieceLibraryWidget*>(childAt(event->pos()));

	if (cblw != NULL)
	{
		colorEditorWidget->setGlassColor(cblw->getGlassColor());
		setViewMode(COLORBAR_VIEW_MODE);
	}
	else if (plplw != NULL)
	{
		pullPlanEditorWidget->setPlan(plplw->getPullPlan());
		setViewMode(PULLPLAN_VIEW_MODE);
	}
	else if (plw != NULL)
	{
		pieceEditorWidget->setPiece(plw->getPiece());
		setViewMode(PIECE_VIEW_MODE);
	}
}


void MainWindow :: mousePressEvent(QMouseEvent* event)
{
	AsyncColorBarLibraryWidget* cblw = dynamic_cast<AsyncColorBarLibraryWidget*>(childAt(event->pos()));
	AsyncPullPlanLibraryWidget* plplw = dynamic_cast<AsyncPullPlanLibraryWidget*>(childAt(event->pos()));

	if (event->button() == Qt::LeftButton && (cblw != NULL || plplw != NULL))
	{
		isDragging = true;
		this->dragStartPosition = event->pos();
	}
	else
		isDragging = false;
}

void MainWindow :: mouseMoveEvent(QMouseEvent* event)
{
	// If the left mouse button isn't down
	if ((event->buttons() & Qt::LeftButton) == 0)
	{
		isDragging = false;	
		return;
	}

	if (!isDragging || (event->pos() - dragStartPosition).manhattanLength() < QApplication::startDragDistance())
		return;

	AsyncColorBarLibraryWidget* cblw = dynamic_cast<AsyncColorBarLibraryWidget*>(childAt(event->pos()));
	AsyncPullPlanLibraryWidget* plplw = dynamic_cast<AsyncPullPlanLibraryWidget*>(childAt(event->pos()));

	char buf[500];
	QPixmap pixmap;
	
	if (cblw != NULL)
	{
		GlassMime::encode(buf, cblw->getGlassColor()->getColor(), GlassMime::colorbar);
		pixmap = *cblw->getDragPixmap();
	}
	else if (plplw != NULL)
	{
		GlassMime::encode(buf, plplw->getPullPlan(), GlassMime::pullplan);
		pixmap = *plplw->getDragPixmap();
	}
	else
		return;

	QByteArray pointerData(buf);
	QMimeData* mimeData = new QMimeData;
	mimeData->setText(pointerData);

	QDrag *drag = new QDrag(this);
	drag->setMimeData(mimeData);
	drag->setPixmap(pixmap);
	
	drag->exec(Qt::CopyAction);
}

void MainWindow :: setupConnections()
{
	connect(this, SIGNAL(someDataChanged()), this, SLOT(updateEverything()));

	connect(newColorBarButton, SIGNAL(pressed()), this, SLOT(newColorBar()));
	connect(copyColorBarButton, SIGNAL(pressed()), this, SLOT(copyColorBar()));
	connect(colorEditorWidget, SIGNAL(someDataChanged()), this, SLOT(updateEverything()));

	connect(newPullPlanButton, SIGNAL(pressed()), this, SLOT(newPullPlan()));
	connect(copyPullPlanButton, SIGNAL(pressed()), this, SLOT(copyPullPlan()));
	connect(pullPlanEditorWidget, SIGNAL(someDataChanged()), this, SLOT(updateEverything()));

	connect(newPieceButton, SIGNAL(pressed()), this, SLOT(newPiece()));
	connect(copyPieceButton, SIGNAL(pressed()), this, SLOT(copyPiece()));
	connect(pieceEditorWidget, SIGNAL(someDataChanged()), this, SLOT(updateEverything()));

	connect(openAction, SIGNAL(triggered()), this, SLOT(open()));
	connect(saveAction, SIGNAL(triggered()), this, SLOT(save()));
	connect(saveAsAction, SIGNAL(triggered()), this, SLOT(saveAs()));

	connect(randomSimpleCaneAction, SIGNAL(triggered()), this, SLOT(randomSimpleCaneExampleActionTriggered()));
	connect(randomSimplePieceAction, SIGNAL(triggered()), this, SLOT(randomSimplePieceExampleActionTriggered()));

	connect(randomComplexCaneAction, SIGNAL(triggered()), this, SLOT(randomComplexCaneExampleActionTriggered()));
	connect(randomComplexPieceAction, SIGNAL(triggered()), this, SLOT(randomComplexPieceExampleActionTriggered()));

	connect(depthPeelAction, SIGNAL(triggered()), this, SLOT(depthPeelActionTriggered()));
}

void MainWindow :: depthPeelActionTriggered()
{
	NiceViewWidget::peelEnable = !(NiceViewWidget::peelEnable);
	depthPeelAction->setChecked(NiceViewWidget::peelEnable);
	emit someDataChanged();
}

void MainWindow :: randomSimpleCaneExampleActionTriggered()
{
	GlassColor* randomGC = randomGlassColor();
	PullPlan* randomPP = randomSimplePullPlan(CIRCLE_SHAPE, randomGC);

	colorBarLibraryLayout->addWidget(new AsyncColorBarLibraryWidget(randomGC));

	AsyncPullPlanLibraryWidget* pplw = new AsyncPullPlanLibraryWidget(randomPP);
	pullPlanLibraryLayout->addWidget(pplw);

	pullPlanEditorWidget->setPlan(randomPP);	
	setViewMode(PULLPLAN_VIEW_MODE);
}

void MainWindow :: randomComplexCaneExampleActionTriggered()
{
	GlassColor* randomGC = randomGlassColor();
	PullPlan* randomCPP = randomSimplePullPlan(CIRCLE_SHAPE, randomGC);
	PullPlan* randomSPP = randomSimplePullPlan(SQUARE_SHAPE, randomGC);
	PullPlan* randomComplexPP = randomComplexPullPlan(randomCPP, randomSPP);

	colorBarLibraryLayout->addWidget(new AsyncColorBarLibraryWidget(randomGC));
	pullPlanLibraryLayout->addWidget(new AsyncPullPlanLibraryWidget(randomComplexPP));
	// add simple plans only if they are used
	// memory leak! as unused ones never appear in library
	if (randomComplexPP->hasDependencyOn(randomCPP))
		pullPlanLibraryLayout->addWidget(new AsyncPullPlanLibraryWidget(randomCPP));
	if (randomComplexPP->hasDependencyOn(randomSPP))
		pullPlanLibraryLayout->addWidget(new AsyncPullPlanLibraryWidget(randomSPP));

	pullPlanEditorWidget->setPlan(randomComplexPP);	
	setViewMode(PULLPLAN_VIEW_MODE);
}

void MainWindow :: randomSimplePieceExampleActionTriggered()
{
	GlassColor* randomGC = randomGlassColor();
	PullPlan* randomSPP = randomSimplePullPlan(SQUARE_SHAPE, randomGC);
	Piece* randomP = randomPiece(randomPickup(randomSPP));

	colorBarLibraryLayout->addWidget(new AsyncColorBarLibraryWidget(randomGC));
	pullPlanLibraryLayout->addWidget(new AsyncPullPlanLibraryWidget(randomSPP));
	
	AsyncPieceLibraryWidget* plw = new AsyncPieceLibraryWidget(randomP);
	pieceLibraryLayout->addWidget(plw);

	pieceEditorWidget->setPiece(randomP);	
	setViewMode(PIECE_VIEW_MODE);
}

void MainWindow :: randomComplexPieceExampleActionTriggered()
{
	GlassColor* randomGC = randomGlassColor();
        PullPlan* randomCPP = randomSimplePullPlan(CIRCLE_SHAPE, randomGC);
        PullPlan* randomSPP = randomSimplePullPlan(SQUARE_SHAPE, randomGC);
        PullPlan* randomComplexPP = randomComplexPullPlan(randomCPP, randomSPP);
	Piece* randomP = randomPiece(randomPickup(randomSPP));

	// change every other plan in the pickup to be the complex cane
	// instead of the square one
	for (unsigned int i = 0; i < randomP->pickup->subs.size(); i+=2)
		randomP->pickup->subs[i].plan = randomComplexPP;
	
	colorBarLibraryLayout->addWidget(new AsyncColorBarLibraryWidget(randomGC));
	if (randomComplexPP->hasDependencyOn(randomCPP)) // memory leak
		pullPlanLibraryLayout->addWidget(new AsyncPullPlanLibraryWidget(randomCPP));
	pullPlanLibraryLayout->addWidget(new AsyncPullPlanLibraryWidget(randomSPP));
	pullPlanLibraryLayout->addWidget(new AsyncPullPlanLibraryWidget(randomComplexPP));
	pieceLibraryLayout->addWidget(new AsyncPieceLibraryWidget(randomP));
		
	pieceEditorWidget->setPiece(randomP);	
	setViewMode(PIECE_VIEW_MODE);
}

void MainWindow :: setupLibrary()
{
	QWidget* bigOleLibraryWidget = new QWidget(centralWidget);
	centralLayout->addWidget(bigOleLibraryWidget);

	QVBoxLayout* libraryAreaLayout = new QVBoxLayout(bigOleLibraryWidget);
	bigOleLibraryWidget->setLayout(libraryAreaLayout);

	QScrollArea* libraryScrollArea = new QScrollArea(bigOleLibraryWidget);
	libraryAreaLayout->addWidget(libraryScrollArea, 1);
	libraryScrollArea->setBackgroundRole(QPalette::Dark);
	libraryScrollArea->setWidgetResizable(true);
	libraryScrollArea->setFixedWidth(370);
	libraryScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	libraryScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

	QWidget* libraryWidget = new QWidget(libraryScrollArea);
	libraryScrollArea->setWidget(libraryWidget);

	QGridLayout* superlibraryLayout = new QGridLayout(libraryWidget);
	libraryWidget->setLayout(superlibraryLayout);

	newColorBarButton = new QPushButton("New Color", libraryWidget);
	newPullPlanButton = new QPushButton("New Cane", libraryWidget);
	newPieceButton = new QPushButton("New Piece", libraryWidget);
	superlibraryLayout->addWidget(newColorBarButton, 0, 0);
	superlibraryLayout->addWidget(newPullPlanButton, 0, 1);
	superlibraryLayout->addWidget(newPieceButton, 0, 2);

	copyColorBarButton = new QPushButton("Copy Color", libraryWidget);
	copyPullPlanButton = new QPushButton("Copy Cane", libraryWidget);
	copyPieceButton = new QPushButton("Copy Piece", libraryWidget);
	superlibraryLayout->addWidget(copyColorBarButton, 1, 0);
	superlibraryLayout->addWidget(copyPullPlanButton, 1, 1);
	superlibraryLayout->addWidget(copyPieceButton, 1, 2);

	colorBarLibraryLayout = new QVBoxLayout(libraryWidget);
	colorBarLibraryLayout->setDirection(QBoxLayout::BottomToTop);
	pullPlanLibraryLayout = new QVBoxLayout(libraryWidget);
	pullPlanLibraryLayout->setDirection(QBoxLayout::BottomToTop);
	pieceLibraryLayout = new QVBoxLayout(libraryWidget);
	pieceLibraryLayout->setDirection(QBoxLayout::BottomToTop);
	superlibraryLayout->addLayout(colorBarLibraryLayout, 2, 0, Qt::AlignTop);
	superlibraryLayout->addLayout(pullPlanLibraryLayout, 2, 1, Qt::AlignTop);
	superlibraryLayout->addLayout(pieceLibraryLayout, 2, 2, Qt::AlignTop);

	// make three qlabels for a legend
	QGridLayout* legendLayout = new QGridLayout(libraryWidget);
	QLabel* l1 = new QLabel("Used By Selected");
	l1->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	l1->setStyleSheet("border: 2px dashed " + QColor(0, 139, 69, 255).name() + ";");
	QLabel* l2 = new QLabel("Selected");
	l2->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	l2->setStyleSheet("border: 3px solid " + QColor(0, 0, 255, 255).name() + ";");
	QLabel* l3 = new QLabel("Uses Selected");
	l3->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	l3->setStyleSheet("border: 2px dotted " + QColor(200, 100, 0, 255).name() + ";");
	legendLayout->addWidget(l1,0,1);
	legendLayout->addWidget(l2,0,2);
	legendLayout->addWidget(l3,0,3);
	libraryAreaLayout->addLayout(legendLayout, 0);

	QLabel* descriptionLabel = new QLabel("Library - click to edit or drag to add.",
		libraryWidget);
	descriptionLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	libraryAreaLayout->addWidget(descriptionLabel, 0);
}

void MainWindow :: setupEditors()
{
	editorStack = new QStackedWidget(centralWidget);
	centralLayout->addWidget(editorStack, 2);

	// The order that the editors are added to the stacked widget
	// must match their order values in the enum EditorGUI::Mode
	// in mainwindow.h
	setupEmptyPaneEditor();
	editorStack->addWidget(emptyEditorPage);

	setupColorEditor();
	editorStack->addWidget(colorEditorWidget);

	setupPullPlanEditor();
	editorStack->addWidget(pullPlanEditorWidget);

	setupPieceEditor();
	editorStack->addWidget(pieceEditorWidget);
}

void MainWindow :: setupPieceEditor()
{
	// Setup data objects - the current plan and library widget for this plan
	pieceEditorWidget = new PieceEditorWidget(editorStack);
	pieceLibraryLayout->addWidget(new AsyncPieceLibraryWidget(pieceEditorWidget->getPiece()));
}

void MainWindow :: setupEmptyPaneEditor()
{
	emptyEditorPage = new QWidget(editorStack);
	QHBoxLayout* editorLayout = new QHBoxLayout(emptyEditorPage);
	emptyEditorPage->setLayout(editorLayout);
	whatToDoLabel = new QLabel("Click a library item at left to edit/view.", emptyEditorPage);
	whatToDoLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	editorLayout->addWidget(whatToDoLabel, 0);
}

void MainWindow :: setupColorEditor()
{
	// Setup data objects - the current plan and library widget for this plan
	AsyncColorBarLibraryWidget* starterLibraryWidget = 
		new AsyncColorBarLibraryWidget(new GlassColor(), this);
	colorEditorWidget = new ColorEditorWidget(starterLibraryWidget->getGlassColor(), editorStack);
	colorBarLibraryLayout->addWidget(starterLibraryWidget);
}

void MainWindow :: setupPullPlanEditor()
{
	// Setup data objects - the current plan and library widget for this plan
	pullPlanEditorWidget = new PullPlanEditorWidget(editorStack);
	pullPlanLibraryLayout->addWidget(new AsyncPullPlanLibraryWidget(pullPlanEditorWidget->getPlan()));
}

void MainWindow :: newPiece()
{
	// Create the new piece
	Piece* newEditorPiece = new Piece(PieceTemplate::TUMBLER);

	// Create the new library entry
	pieceLibraryLayout->addWidget(new AsyncPieceLibraryWidget(newEditorPiece));
	pieceEditorWidget->setPiece(newEditorPiece);

	// Load up the right editor
	setViewMode(PIECE_VIEW_MODE);
}

void MainWindow :: copyPiece()
{
	if (editorStack->currentIndex() != PIECE_VIEW_MODE)
		return;

	// Create the new piece
	Piece* newEditorPiece = pieceEditorWidget->getPiece()->copy();
	pieceLibraryLayout->addWidget(new AsyncPieceLibraryWidget(newEditorPiece));
	pieceEditorWidget->setPiece(newEditorPiece);

	emit someDataChanged();
}

void MainWindow :: newColorBar()
{
	GlassColor* newGlassColor = new GlassColor();

	// Create the new library entry
	colorBarLibraryLayout->addWidget(new AsyncColorBarLibraryWidget(newGlassColor, this));
	colorEditorWidget->setGlassColor(newGlassColor);

	// Load up the right editor
	setViewMode(COLORBAR_VIEW_MODE);
}

void MainWindow :: copyColorBar()
{
	if (editorStack->currentIndex() != COLORBAR_VIEW_MODE)
		return;

	GlassColor* newEditorGlassColor = colorEditorWidget->getGlassColor()->copy();

	// Create the new library entry
	colorBarLibraryLayout->addWidget(new AsyncColorBarLibraryWidget(newEditorGlassColor, this));
	colorEditorWidget->setGlassColor(newEditorGlassColor);

	// Trigger GUI updates
	emit someDataChanged();
}

void MainWindow :: newPullPlan()
{
	PullPlan *newEditorPlan = new PullPlan(PullTemplate::BASE_CIRCLE);
	emit newPullPlan(newEditorPlan);
}

void MainWindow :: copyPullPlan()
{
	if (editorStack->currentIndex() != PULLPLAN_VIEW_MODE)
		return;

	PullPlan *newEditorPlan = pullPlanEditorWidget->getPlan()->copy();
	emit newPullPlan(newEditorPlan);
}

void MainWindow :: newPullPlan(PullPlan* newPlan)
{
	pullPlanLibraryLayout->addWidget(new AsyncPullPlanLibraryWidget(newPlan));

	// Give the new plan to the editor
	pullPlanEditorWidget->setPlan(newPlan);

	// Load up the right editor
	setViewMode(PULLPLAN_VIEW_MODE);
}

void MainWindow :: updateEverything()
{
	switch (editorStack->currentIndex())
	{
		case COLORBAR_VIEW_MODE:
			colorEditorWidget->updateEverything();
			break;
		case PULLPLAN_VIEW_MODE:
			pullPlanEditorWidget->updateEverything();
			break;
		case PIECE_VIEW_MODE:
			pieceEditorWidget->updateEverything();
			break;
	}

	updateLibrary();
}

// returns whether the pull plan is a dependancy of something in the library
// (either a pull plan or a piece)
bool MainWindow :: glassColorIsDependancy(GlassColor* color)
{
	AsyncPullPlanLibraryWidget* pplw;
	for (int i = 0; i < pullPlanLibraryLayout->count(); ++i)
	{
		pplw = dynamic_cast<AsyncPullPlanLibraryWidget*>(
			dynamic_cast<QWidgetItem *>(pullPlanLibraryLayout->itemAt(i))->widget());
		if (pplw->getPullPlan()->hasDependencyOn(color))
		{
			return true;
		}
	}

	AsyncPieceLibraryWidget* plw;
	for (int i = 0; i < pieceLibraryLayout->count(); ++i)
	{
		plw = dynamic_cast<AsyncPieceLibraryWidget*>(
			dynamic_cast<QWidgetItem *>(pieceLibraryLayout->itemAt(i))->widget());
		if (plw->getPiece()->hasDependencyOn(color))
		{
			return true;
		}
	}

	return false;
}

// returns whether the pull plan is a dependancy of something in the library
// (either another pull plan or a piece)
bool MainWindow :: pullPlanIsDependancy(PullPlan* plan)
{
	AsyncPullPlanLibraryWidget* pplw;
	for (int i = 0; i < pullPlanLibraryLayout->count(); ++i)
	{
		pplw = dynamic_cast<AsyncPullPlanLibraryWidget*>(
				dynamic_cast<QWidgetItem *>(pullPlanLibraryLayout->itemAt(i))->widget());
		// Check whether the pull plan in the library is:
		// 1. the parameter plan
		// 2. a plan with the plan currently being edited as a subplan
		if (pplw->getPullPlan() == plan)
		{
			continue;
		}
		else if (pplw->getPullPlan()->hasDependencyOn(plan))
		{
			return true;
		}
	}

	AsyncPieceLibraryWidget* plw;
	for (int i = 0; i < pieceLibraryLayout->count(); ++i)
	{
		plw = dynamic_cast<AsyncPieceLibraryWidget*>(
			dynamic_cast<QWidgetItem *>(pieceLibraryLayout->itemAt(i))->widget());
		if (plw->getPiece()->hasDependencyOn(plan))
		{
			return true;
		}
	}

	return false;	
}

void MainWindow :: updateLibrary()
{
	unhighlightAllLibraryWidgets();

	switch (editorStack->currentIndex())
	{
		case COLORBAR_VIEW_MODE:
		{
                        AsyncColorBarLibraryWidget* cblw;
                        for (int i = 0; i < colorBarLibraryLayout->count(); ++i)
                        {
                                cblw = dynamic_cast<AsyncColorBarLibraryWidget*>(
                                        dynamic_cast<QWidgetItem *>(colorBarLibraryLayout->itemAt(i))->widget());
                                if (colorEditorWidget->getGlassColor() == cblw->getGlassColor())
				{
					cblw->updatePixmaps();
					cblw->setDependancy(true, IS_DEPENDANCY);
				}
                        }


			AsyncPullPlanLibraryWidget* pplw;
			for (int i = 0; i < pullPlanLibraryLayout->count(); ++i)
			{
				pplw = dynamic_cast<AsyncPullPlanLibraryWidget*>(
					dynamic_cast<QWidgetItem *>(pullPlanLibraryLayout->itemAt(i))->widget());
				if (pplw->getPullPlan()->hasDependencyOn(colorEditorWidget->getGlassColor()))
				{
					pplw->updatePixmaps();
					pplw->setDependancy(true, USES_DEPENDANCY);
				}
			}

			AsyncPieceLibraryWidget* plw;
			for (int i = 0; i < pieceLibraryLayout->count(); ++i)
			{
				plw = dynamic_cast<AsyncPieceLibraryWidget*>(
					dynamic_cast<QWidgetItem *>(pieceLibraryLayout->itemAt(i))->widget());
				if (plw->getPiece()->hasDependencyOn(colorEditorWidget->getGlassColor()))
				{
					plw->updatePixmap();
					plw->setDependancy(true, USES_DEPENDANCY);
				}
			}

			break;
		}
		case PULLPLAN_VIEW_MODE:
		{
			AsyncColorBarLibraryWidget* cblw;
			for (int i = 0; i < colorBarLibraryLayout->count(); ++i)
			{
				cblw = dynamic_cast<AsyncColorBarLibraryWidget*>(
					dynamic_cast<QWidgetItem *>(colorBarLibraryLayout->itemAt(i))->widget());
				if (pullPlanEditorWidget->getPlan()->hasDependencyOn(cblw->getGlassColor()))
					cblw->setDependancy(true, USEDBY_DEPENDANCY);
			}

			AsyncPullPlanLibraryWidget* pplw;
			for (int i = 0; i < pullPlanLibraryLayout->count(); ++i)
			{
				pplw = dynamic_cast<AsyncPullPlanLibraryWidget*>(
						dynamic_cast<QWidgetItem *>(pullPlanLibraryLayout->itemAt(i))->widget());
				// Check whether the pull plan in the library is:
				// 1. the plan currently being edited 
				// 2. a subplan of the plan current being edited
				// 3. a plan with the plan currently being edited as a subplan
				if (pullPlanEditorWidget->getPlan() == pplw->getPullPlan())
				{
					pplw->updatePixmaps();
					pplw->setDependancy(true, IS_DEPENDANCY);
				}
				else if (pullPlanEditorWidget->getPlan()->hasDependencyOn(pplw->getPullPlan()))
				{
					pplw->setDependancy(true, USEDBY_DEPENDANCY);
				}
				else if (pplw->getPullPlan()->hasDependencyOn(pullPlanEditorWidget->getPlan()))	
				{
					pplw->updatePixmaps();
					pplw->setDependancy(true, USES_DEPENDANCY);
				}
			}

			AsyncPieceLibraryWidget* plw;
			for (int i = 0; i < pieceLibraryLayout->count(); ++i)
			{
				plw = dynamic_cast<AsyncPieceLibraryWidget*>(
					dynamic_cast<QWidgetItem *>(pieceLibraryLayout->itemAt(i))->widget());
				if (plw->getPiece()->hasDependencyOn(pullPlanEditorWidget->getPlan()))
				{
					plw->updatePixmap();
					plw->setDependancy(true, USES_DEPENDANCY);
				}
			}

			break;
		}
		case PIECE_VIEW_MODE:
		{
			AsyncColorBarLibraryWidget* cblw;
			for (int i = 0; i < colorBarLibraryLayout->count(); ++i)
			{
				cblw = dynamic_cast<AsyncColorBarLibraryWidget*>(
					dynamic_cast<QWidgetItem *>(colorBarLibraryLayout->itemAt(i))->widget());
				if (pieceEditorWidget->getPiece()->hasDependencyOn(cblw->getGlassColor()))
					cblw->setDependancy(true, USEDBY_DEPENDANCY);
			}

			AsyncPullPlanLibraryWidget* pplw;
			for (int i = 0; i < pullPlanLibraryLayout->count(); ++i)
			{
				pplw = dynamic_cast<AsyncPullPlanLibraryWidget*>(
					dynamic_cast<QWidgetItem*>(pullPlanLibraryLayout->itemAt(i))->widget());
				if (pieceEditorWidget->getPiece()->hasDependencyOn(pplw->getPullPlan()))
					pplw->setDependancy(true, USEDBY_DEPENDANCY);
			}

                        AsyncPieceLibraryWidget* plw;
                        for (int i = 0; i < pieceLibraryLayout->count(); ++i)
                        {
                                plw = dynamic_cast<AsyncPieceLibraryWidget*>(
                                        dynamic_cast<QWidgetItem *>(pieceLibraryLayout->itemAt(i))->widget());
                                if (plw->getPiece() == pieceEditorWidget->getPiece())
                                {
                                        plw->updatePixmap();
					plw->setDependancy(true, IS_DEPENDANCY);
                                }
                        }

			break;
		}
	}
}


