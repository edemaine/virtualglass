
#include "glassfilereader.h"

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <stdio.h>

#include <QTextStream>
#include <QFile>

#include "glasscolor.h"
#include "pullplan.h"
#include "piece.h"
#include "globalglass.h"

using std::ifstream;
using std::cout;
using std::endl;
using std::string;

void GlassFileReader::openColors(Json::Value rootColor, map<GlassColor*, int>* colorMap, vector<GlassColor*>* readColors)
{
        //rootXXX variables are Json::Values; getMemberNames available
        //vecXXX variables are vectors; operator [] available
        vector<string> vecColorMembers = rootColor.getMemberNames(); //vector for colornames
        enum colors{
                R,
                G,
                B,
                alpha
        };

        static map<string, int> colorMapEnum;
        colorMapEnum["R"] = R;
        colorMapEnum["G"] = G;
        colorMapEnum["B"] = B;
        colorMapEnum["Alpha"] = alpha;

	unsigned int *index = new unsigned int[vecColorMembers.size()];
        unsigned j = 0;
        for (unsigned int i = 0; i < vecColorMembers.size(); i++) //loop over all canes
        {
                string member = vecColorMembers[i];
                unsigned int number = 0;
                //fixes "wrong" order (0, 10, 11..., 1, 2 ,.., 9)
                if(member.find("_") != string::npos){
                        member.resize(vecColorMembers[i].find("_"));
                        number = atoi(member.c_str());
                }

                if(i+1 == number)
                        index[i] = number-1;
                else
                {
                        for (unsigned int l = 0; l < vecColorMembers.size(); l++) //loop over all canes
                        {
                                string member = vecColorMembers[l];
                                if(member.find("_") != string::npos){
                                        member.resize(vecColorMembers[l].find("_"));
                                        number = atoi(member.c_str());
                                }
                                if(i+1 == number)
                                {
                                        index[i] = l;
                                        l = vecColorMembers.size();
                                }
                        }
                }
        }

        for (unsigned int i = 0; i < vecColorMembers.size(); i++) //loop over all pieces; adds pieces in correct order
        {
                j=index[i];
                Json::Value rootColorValue = rootColor[vecColorMembers[j]];
                vector<std::string> vecColorValueMembers = rootColorValue.getMemberNames(); //vector for RGBalpha

                Color color;
                GlassColor *glasscolor = new GlassColor;

                if(vecColorValueMembers.size()==4){
                        for(int k=0; k<4; k++){
                                switch(colorMapEnum[vecColorValueMembers.at(k)])
				{
					case R: color.r = rootColorValue["R"].asFloat(); break;
					case G: color.g = rootColorValue["G"].asFloat(); break;
					case B: color.b = rootColorValue["B"].asFloat(); break;
					case alpha: color.a = rootColorValue["Alpha"].asFloat(); break;
                                }
                                glasscolor->setColor(color);
                        }
                        string colorNumberSt = (vecColorMembers.at(j));
                        colorNumberSt.resize((vecColorMembers.at(j)).find("_")); //extracts number from string
                        int colorNumberInt = atoi(colorNumberSt.c_str());
                        (*colorMap)[glasscolor] = colorNumberInt;

                        string colorName = (vecColorMembers.at(j)).substr ((vecColorMembers.at(j)).find("_")+1);
                        (*glasscolor).setName(QString::fromStdString(colorName));
			readColors->push_back(glasscolor);
                }
                else
                        cout << "error in color " << vecColorValueMembers[j];
        }

    delete index;
}

void GlassFileReader::openCanes(Json::Value rootCane, map<PullPlan*, int>* caneMap, map<GlassColor*, int>* colorMap, 
	vector<PullPlan*>* readPlans)
{
	vector<std::string> vecCaneMembers = rootCane.getMemberNames(); //vector for canes
	map<GlassColor*, int>::iterator iter;

	enum caneKeywords{
		Subpullplans,
		casingcount,
		PullTemplate,
		PullTemplateParameters,
		twists,
		Column,
		Count, //Column count
		Row, //Row count
		Radial, //Radial count
		CasingColor,
		CasingShape,
		CasingThickness,
		Undefined,
		BaseCircle,
		BaseSquare,
		HorizontalLineCircle,
		HorizontalLineSquare,
		Tripod,
		Cross,
		SquareofCircles,
		SquareofSquares,
		SurroundingCircle,
		SurroundingSquare,
		Custom
	};

	static std::map<std::string, int> caneMapEnum;
	caneMapEnum["SubPullPlans"] = Subpullplans;
	caneMapEnum["PullTemplate"] = PullTemplate;
	caneMapEnum["PullTemplateParameters"] = PullTemplateParameters;
	caneMapEnum["Casing count"] = casingcount;
	caneMapEnum["Twists"] = twists;
	caneMapEnum["Column"] = Column;
	caneMapEnum["Count"] = Count;
	caneMapEnum["Row"] = Row;
	caneMapEnum["Radial"] = Radial;
	caneMapEnum["CasingColor"] = CasingColor;
	caneMapEnum["CasingShape"] = CasingShape;
	caneMapEnum["CasingThickness"] = CasingThickness;
	caneMapEnum["Undefined"] = Undefined;
	caneMapEnum["Base Circle"] = BaseCircle;
	caneMapEnum["Base Square"] = BaseSquare;
	caneMapEnum["Horizontal Line Circle"] = HorizontalLineCircle;
	caneMapEnum["Horizontal Line Square"] = HorizontalLineSquare;
	caneMapEnum["Tripod"] = Tripod;
	caneMapEnum["Cross"] = Cross;
	caneMapEnum["Square of Circles"] = SquareofCircles;
	caneMapEnum["Square of Squares"] = SquareofSquares;
	caneMapEnum["Surrounding Circle"] = SurroundingCircle;
	caneMapEnum["Surrounding Square"] = SurroundingSquare;
	caneMapEnum["Custom"] = Custom;

	unsigned int *index = new unsigned int[vecCaneMembers.size()];
	unsigned k = 0;
	for (unsigned int i = 0; i < vecCaneMembers.size(); i++) //loop over all canes
	{
		string member = vecCaneMembers[i];
		unsigned int number =0;
		//fixes "wrong" order (0, 10, 11..., 1, 2 ,.., 9)
		if(member.find("_")!=std::string::npos){
			member.resize(vecCaneMembers[i].find("_"));
			number = atoi(member.c_str());
		}

		if(i+1==number)
			index[i]=number-1;
		else
		{
			for (unsigned int l = 0; l < vecCaneMembers.size(); l++) //loop over all canes
			{
				string member = vecCaneMembers[l];
				if(member.find("_")!=std::string::npos){
					member.resize(vecCaneMembers[l].find("_"));
					number = atoi(member.c_str());
				}
				if(i+1==number)
				{
					index[i]=l;
					l=vecCaneMembers.size();
				}
			}
		}
	}

	for (unsigned int i = 0; i < vecCaneMembers.size(); i++) //loop over all pieces; adds pieces in correct order
	{
		k=index[i];
		cout << "k " << vecCaneMembers.at(k);
		cout << endl;

		Json::Value rootCaneValue = rootCane[vecCaneMembers.at(k)];
		vector<std::string> vecCaneValueMembers = rootCaneValue.getMemberNames(); //vector for CaneValues
		PullPlan *plan = new PullPlan(PullTemplate::BASE_CIRCLE);
		//AsyncPullPlanLibraryWidget *p = new AsyncPullPlanLibraryWidget(plan);
		readPlans->push_back(plan);
		string caneNumberSt = (vecCaneMembers.at(k));
		caneNumberSt.resize((vecCaneMembers.at(k)).find("_")); //extracts number from string
		int caneNumberInt = atoi(caneNumberSt.c_str());
		for(unsigned int l = 0; l < vecCaneValueMembers.size(); l++){
			Json::Value rootCaneCasing;
			switch(caneMapEnum[vecCaneValueMembers.at(l)]){
			case casingcount:
			{
				rootCaneCasing = rootCaneValue["0_Casing"];
				if(rootCaneCasing["CasingShape"].asBool())
					plan->setTemplateType(PullTemplate::BASE_SQUARE);
				else
					plan->setTemplateType(PullTemplate::BASE_CIRCLE);

				for(unsigned int i = 0; i < rootCaneValue["Casing count"].asUInt()-2; i++){
					std::stringstream *sstr = new std::stringstream;
					*sstr << i+2<<"_Casing";
					string casing = sstr->str();
					Json::Value rootCaneCasing = rootCaneValue[casing];
					//if(i==plan->getCasingCount()-1){
						if(rootCaneCasing["CasingShape"].asBool())
							plan->addCasing(SQUARE_SHAPE);
						else
							plan->addCasing(CIRCLE_SHAPE);
					//}
				}
				for(int i = 0; i < rootCaneValue["Casing count"].asInt(); i++){
					std::stringstream *sstr = new std::stringstream;
					*sstr << i<<"_Casing";
					string casing = sstr->str();
					for(iter = colorMap->begin(); iter != colorMap->end(); iter++){
						if(iter->second == rootCaneValue[casing]["CasingColor"].asInt())
							plan->setCasingColor(iter->first,i);
					}
				}
				int i=plan->getCasingCount();
				while(i>0){
					std::stringstream *sstr = new std::stringstream;
					*sstr << i-1<<"_Casing";
					string casing = sstr->str();
					Json::Value rootCaneCasing = rootCaneValue[casing];
					plan->setCasingThickness(rootCaneCasing["CasingThickness"].asFloat(),i-1);
					i--;
				}
			}
			break;
			case PullTemplate:
			{
			switch(caneMapEnum[rootCaneValue["PullTemplate"].asString()]){
			case BaseCircle:
				plan->setTemplateType(PullTemplate::BASE_CIRCLE);
				break;
			case BaseSquare:
				plan->setTemplateType(PullTemplate::BASE_SQUARE);
				break;
			case HorizontalLineCircle:
				plan->setTemplateType(PullTemplate::HORIZONTAL_LINE_CIRCLE);
				break;
			case HorizontalLineSquare:
				plan->setTemplateType(PullTemplate::HORIZONTAL_LINE_SQUARE);
				break;
			case Tripod:
				plan->setTemplateType(PullTemplate::TRIPOD);
				break;
			case Cross:
				plan->setTemplateType(PullTemplate::CROSS);
				break;
			case SquareofCircles:
				plan->setTemplateType(PullTemplate::SQUARE_OF_CIRCLES);
				break;
			case SquareofSquares:
				plan->setTemplateType(PullTemplate::SQUARE_OF_SQUARES);
				break;
			case SurroundingCircle:
				plan->setTemplateType(PullTemplate::SURROUNDING_CIRCLE);
				break;
			case SurroundingSquare:
				plan->setTemplateType(PullTemplate::SURROUNDING_SQUARE);
				break;
			case Custom:
				plan->setTemplateType(PullTemplate::CUSTOM);
				break;
		}
		break;
			}
			case twists:
				plan->setTwist(rootCaneValue["Twists"].asInt());
				break;
			case PullTemplateParameters:
				if(rootCaneValue.isMember("PullTemplateParameters"))
				{
					vector<std::string> pullTemplateParameterValues = rootCaneValue["PullTemplateParameters"].getMemberNames();
					plan->setParameter(0,rootCaneValue["PullTemplateParameters"][pullTemplateParameterValues[0]].asInt()); //first parameter!
					break;
				}
			}
		}
		//pullPlanLibraryLayout->addWidget(p);
		//pullPlanLibraryLayout->update();
		//emit someDataChanged();
		(*caneMap)[plan] = caneNumberInt;
	}
	//fill subs
	map<PullPlan*, int>::iterator pullIter;
	map<PullPlan*, int>::iterator subpullIter;
	for(pullIter = caneMap->begin(); pullIter != caneMap->end(); pullIter++)
	{
		PullPlan *plan = new PullPlan(PullTemplate::BASE_CIRCLE);
		plan = pullIter->first;
		if(plan->subs.size()>1)
		{
			for(unsigned int j = 0; j<vecCaneMembers.size(); j++)
			{
				string numberSt = vecCaneMembers.at(j);
				int numberInt = 0;
				if(numberSt.find("_") != std::string::npos){
					numberSt.resize(numberSt.find("_"));
					numberInt = atoi(numberSt.c_str());
				}
				if(numberInt==pullIter->second){
					Json::Value rootCaneValue = rootCane[vecCaneMembers.at(j)];
					if(rootCaneValue["SubPullPlans"] != rootCaneValue["NULL"]){
						Json::Value rootCaneSubpull = rootCaneValue["SubPullPlans"];
						vector<std::string> vecCaneSubpullMembers = rootCaneSubpull.getMemberNames();

						for (unsigned int i = 0; i < vecCaneSubpullMembers.size(); i++){
							Json::Value rootSubcane = rootCaneSubpull[vecCaneSubpullMembers.at(i)];
							string member = vecCaneSubpullMembers.at(i);
							int number =0;
							if(member.find("_") != std::string::npos){
								member.resize((vecCaneSubpullMembers.at(i)).find("_"));
								number=atoi(member.c_str());
							}

							for(subpullIter = caneMap->begin(); subpullIter != caneMap->end(); subpullIter++) {
								if (subpullIter->second == rootSubcane["Cane"].asInt())
								{
									PullPlan *subplan = new PullPlan(PullTemplate::BASE_CIRCLE);
									subplan = subpullIter->first;
									GeometricShape shape;
									if (rootSubcane["Shape"].asInt() == 0)
										shape = CIRCLE_SHAPE;
									else
										shape = SQUARE_SHAPE;
									Point location;
									location.x = rootSubcane["X"].asFloat();
									location.y = rootSubcane["Y"].asFloat();
									location.z = rootSubcane["Z"].asFloat();

									SubpullTemplate *sub = new SubpullTemplate(subplan, shape, location, rootSubcane["Diameter"].asFloat());
									plan->subs[number] =  *sub;
								}
							}
						}
					}
				}
			}
		}
	}
	delete index;
}

void GlassFileReader::openPieces(Json::Value root, map<PullPlan*, int>* caneMap, map<GlassColor*, int>* colorMap, 
	vector<Piece*>* readPieces)
{
	std::vector<std::string> vecPieceMembers = root.getMemberNames(); //vec with pieces

	enum piece{
		subPickupTemplateParameters,
		subPickupTemplate,
		subPickups,
		overlayGlassColor,
		underlayGlassColor,
		casingGlassColor,
		Tumbler,
		Vase,
		Bowl,
		Pot,
		Pickup,
		Wavy_Plate,
		vertical,
		reticello,
		murrinecolumn,
		verticalsandhorizontals,
		verthorizontalvert,
		verticalwithlipwrap,
		murrinerow,
		murrine,
		cane,
		length,
		orientation,
		shape,
		width,
		x,
		y,
		z,
		row,
		thickness,
		column,
		lipwidth,
		bodywidth,
		twists
	};

	static std::map<std::string, int> mapEnum;
	mapEnum["OverlayGlassColor"] = overlayGlassColor;
	mapEnum["UnderlayGlassColor"] = underlayGlassColor;
	mapEnum["CasingGlassColor"] = casingGlassColor;
	mapEnum["SubPickupTemplate"] = subPickupTemplate;
	mapEnum["SubPickupTemplateParameters"] = subPickupTemplateParameters;
	mapEnum["SubPickups"] = subPickups;
	mapEnum["Tumbler"] = Tumbler;
	mapEnum["Vase"] = Vase;
	mapEnum["Bowl"] = Bowl;
	mapEnum["Pot"] = Pot;
	mapEnum["Pickup"] = Pickup;
	mapEnum["Wavy Plate"] = Wavy_Plate;
	mapEnum["Vertical"] = vertical;
	mapEnum["Reticello Vertical Horizontal"] = reticello;
	mapEnum["Murrine Column"] = murrinecolumn;
	mapEnum["Verticals and Horizontals"] = verticalsandhorizontals;
	mapEnum["Vertical Horizontal Vertical"] = verthorizontalvert;
	mapEnum["Vertical With Lip Wrap"] = verticalwithlipwrap;
	mapEnum["Murrine Row"] = murrinerow;
	mapEnum["Murrine"] = murrine;
	mapEnum["Cane"] = cane;
	mapEnum["Length"] = length;
	mapEnum["Orientation"] = orientation;
	mapEnum["Shape"] = shape;
	mapEnum["Width"] = width;
	mapEnum["X"] = x;
	mapEnum["Y"] = y;
	mapEnum["Z"] = z;
	mapEnum["Row/Column count"] = row;
	mapEnum["Thickness"] = thickness;
	mapEnum["Column count"] = column;
	mapEnum["Lip width"] = lipwidth;
	mapEnum["Body width"] = bodywidth;
	mapEnum["Twists"] = twists;

    unsigned int *index = new unsigned int[vecPieceMembers.size()];
	unsigned k = 0;
	for (unsigned int i = 0; i < vecPieceMembers.size(); i++) //loop over all pieces
	{
		string member = vecPieceMembers[i];
		unsigned int number =0;
		//fixes "wrong" order (0, 10, 11..., 1, 2 ,.., 9)
		if(member.find("_")!=std::string::npos){
			member.resize(vecPieceMembers[i].find("_"));
			number = atoi(member.c_str());
		}

		if(i==number)
		{
			index[i]=number;
			cout << "bla";
			cout << endl;
		}
		else
		{
			for (unsigned int l = 0; l < vecPieceMembers.size(); l++) //loop over all pieces
			{
				string member = vecPieceMembers[l];
				if(member.find("_")!=std::string::npos){
					member.resize(vecPieceMembers[l].find("_"));
					number = atoi(member.c_str());
				}
				if(i==number)
				{
					index[i]=l;
					l=vecPieceMembers.size();
				}
			}
		}
	}

	for (unsigned int i = 0; i < vecPieceMembers.size(); i++) //loop over all pieces; adds pieces in correct order
	{
		k=index[i];

		Json::Value rootPieceValues = root[vecPieceMembers[k]];
        PieceTemplate::Type ptt = PieceTemplate::TUMBLER;
		switch(mapEnum[rootPieceValues["PieceTemplate"].asString()])
		{
			case 	Tumbler :
				ptt = PieceTemplate::TUMBLER;
				break;
			case Vase :
				ptt = PieceTemplate::VASE;
				break;
			case Bowl :
				ptt = PieceTemplate::BOWL;
				break;
			case Pot :
				ptt = PieceTemplate::POT;
				break;
			case Pickup :
				ptt = PieceTemplate::PICKUP;
				break;
			case Wavy_Plate :
				ptt = PieceTemplate::WAVY_PLATE;
				break;
		}

		Piece *piece = new Piece(ptt);
		map<GlassColor*,int>::iterator iter;
		for(iter = colorMap->begin(); iter != colorMap->end(); iter++){
			if(rootPieceValues["CasingGlassColor"].asInt()==iter->second)
				piece->pickup->casingGlassColor = iter->first;
			if(rootPieceValues["OverlayGlassColor"].asInt()==iter->second)
				piece->pickup->overlayGlassColor = iter->first;
			if(rootPieceValues["UnderlayGlassColor"].asInt()==iter->second)
				piece->pickup->underlayGlassColor = iter->first;
		}
		if(rootPieceValues.isMember("PieceTemplateParameters"))
		{
			std::vector<std::string> vecPieceTemplParameters = rootPieceValues["PieceTemplateParameters"].getMemberNames();
			for(unsigned int i=0; i< vecPieceTemplParameters.size(); i++){
				if((vecPieceTemplParameters[i].find("_") != std::string::npos)&&(rootPieceValues["PieceTemplateParameters"].isMember(vecPieceTemplParameters[i])))
						piece->setParameter(i, rootPieceValues["PieceTemplateParameters"][vecPieceTemplParameters[i]].asInt());
			}
		}

		if(rootPieceValues["SubPickups"]["SubPickupTemplate"]!=rootPieceValues["NULL"])
		{
			switch(mapEnum[rootPieceValues["SubPickups"]["SubPickupTemplate"].asString()])
			{
				case vertical:
				piece->pickup->setTemplateType(PickupTemplate::VERTICAL);
					break;
				case reticello:
					piece->pickup->setTemplateType(PickupTemplate::RETICELLO_VERTICAL_HORIZONTAL);
					break;
				case murrinecolumn:
					piece->pickup->setTemplateType(PickupTemplate::MURRINE_COLUMN);
					break;
				case verticalsandhorizontals:
					piece->pickup->setTemplateType(PickupTemplate::VERTICALS_AND_HORIZONTALS);
					break;
				case verthorizontalvert:
					piece->pickup->setTemplateType(PickupTemplate::VERTICAL_HORIZONTAL_VERTICAL);
					break;
				case verticalwithlipwrap:
					piece->pickup->setTemplateType(PickupTemplate::VERTICAL_WITH_LIP_WRAP);
					break;
				case murrinerow:
					piece->pickup->setTemplateType(PickupTemplate::MURRINE_ROW);
					break;
				case murrine:
					piece->pickup->setTemplateType(PickupTemplate::MURRINE);
					break;
				}
		}
		if(rootPieceValues["SubPickups"].isMember("SubPickupTemplateParameters"))  //in the moment not necessary. safety first!
		{
			std::vector<std::string> vecSubPickupPara = rootPieceValues["SubPickups"]["SubPickupTemplateParameters"].getMemberNames();
			if(vecSubPickupPara.size()<3)
			{
				if((rootPieceValues["SubPickups"]["SubPickupTemplateParameters"].isMember("Column count"))&&(rootPieceValues["SubPickups"]["SubPickupTemplateParameters"]["Column count"]!=rootPieceValues["NULL"]))
					piece->pickup->setParameter(0, rootPieceValues["SubPickups"]["SubPickupTemplateParameters"]["Column count"].asInt());

				if((rootPieceValues["SubPickups"]["SubPickupTemplateParameters"].isMember("Row/Column count"))&&(rootPieceValues["SubPickups"]["SubPickupTemplateParameters"]["Row/Column count"]!=rootPieceValues["NULL"]))
					piece->pickup->setParameter(0, rootPieceValues["SubPickups"]["SubPickupTemplateParameters"]["Row/Column count"].asInt());

				if(rootPieceValues["SubPickups"]["SubPickupTemplateParameters"].isMember("Thickness")&&(rootPieceValues["SubPickups"]["SubPickupTemplateParameters"]["Thickness"]!=rootPieceValues["NULL"]))
					piece->pickup->setParameter(1, rootPieceValues["SubPickups"]["SubPickupTemplateParameters"]["Thickness"].asInt());
			}
		}

		std::vector<std::string> vecSubPickups = rootPieceValues["SubPickups"].getMemberNames();

		for(unsigned int j = 0; j < vecSubPickups.size()-2; j++)
		{

			string member = vecSubPickups[j];
			int number =0;
			//fixes "wrong" order (0, 10, 11..., 1, 2 ,.., 9)
			if(member.find("_")!=std::string::npos){
				member.resize(vecSubPickups[j].find("_"));
				number = atoi(member.c_str());
			}

			std::ostringstream help;
			help << number << "_Cane";
			std::string var = help.str();

			if(rootPieceValues["SubPickups"].isMember(var))
			{
				std::vector<std::string> vecSubPickupCanes = rootPieceValues["SubPickups"][vecSubPickups[number]].getMemberNames();
				Json::Value rootSubPickupCanes = rootPieceValues["SubPickups"][vecSubPickups[number]];

				if(vecSubPickupCanes.size()==8)
				{
					map<PullPlan*, int>::iterator iter;
					PullPlan* plan = new PullPlan(PullTemplate::BASE_CIRCLE);
					for(iter = caneMap->begin();iter != caneMap->end();iter++){
						if(iter->second==(rootSubPickupCanes["Cane"].asInt()))
						{
							plan = iter->first;
						}
					}
					Point location;
					location[0] = rootSubPickupCanes["X"].asFloat();
					location[1] = rootSubPickupCanes["Y"].asFloat();
					location[2] = rootSubPickupCanes["Z"].asFloat();
					GeometricShape shape;
					if(rootSubPickupCanes["Shape"].asInt()==0){
						shape = CIRCLE_SHAPE;
					} else
					{
						shape = SQUARE_SHAPE;
					}

					SubpickupTemplate *pick = new SubpickupTemplate(plan, location, rootSubPickupCanes["Orientation"].asInt(),
						rootSubPickupCanes["Length"].asFloat(), rootSubPickupCanes["Width"].asFloat(), shape);
					piece->pickup->subs.at(number) = *pick;
					//pullPlanLibraryLayout->update();
					//emit someDataChanged();
					//this->updateEverything();
				}
			} else
			{
				cout << "Not enough SubPickups available!" << endl;
			}
		}

		readPieces->push_back(piece);
		//AsyncPieceLibraryWidget *w = new AsyncPieceLibraryWidget(piece, this);
		//pieceLibraryLayout->addWidget(w);
		//pieceLibraryLayout->update();
		//w->updatePixmap();
		//emit someDataChanged();
	}

	//repaint pullplan library
	//AsyncPullPlanLibraryWidget* pplw;
	//for (int i = 0; i < pullPlanLibraryLayout->count(); ++i)
	//{
	//	pplw = dynamic_cast<AsyncPullPlanLibraryWidget*>(
	//		dynamic_cast<QWidgetItem *>(pullPlanLibraryLayout->itemAt(i))->widget());
	//	pplw->updatePixmaps();
	//}

	delete index;
}

bool GlassFileReader::load(QString filename, vector<GlassColor*>* colors, vector<PullPlan*>* plans, vector<Piece*>* pieces)
{
	QFile openFile(filename);
	openFile.open(QIODevice::ReadOnly | QIODevice::Text);
	QTextStream fileInput(&openFile);
	QString QStr = fileInput.readAll();
	std::string str = QStr.toStdString();
	Json::Value root;
	Json::Reader reader;

	bool parsedSuccess = reader.parse(str,root,false);
	if (!parsedSuccess)
	{
		cout<<"Failed to parse JSON"<<endl<<reader.getFormatedErrorMessages()<<endl; //debugging
		return false;
	}

	map<GlassColor*, int> colorMap;
	map<PullPlan*, int> caneMap;

	colorMap[new GlassColor] = 0;

	//ready to use if necessary
	//if(((root["Build information"]["Number"]).asString())<"700")
	//	cout << "we do not support your build number " << (root["Build information"]["Number"]).asString() << " any more!" << endl;


	if (root.size() != 4)
	{
		cout << "error in file";
		return false;
	}

	if (root["Pieces"].size() == 0)
		return false;

	openColors(root["Colors"], &colorMap, colors);
	openCanes(root["Canes"], &caneMap, &colorMap, plans);
	openPieces(root["Pieces"], &caneMap, &colorMap, pieces);

	return true;
}



