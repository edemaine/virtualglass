
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
using std::stringstream;
using std::vector;
using std::map;

// write dat jay-sahn
QString GlassFileIO :: writeJson(Json::Value& root)
{
        Json::StyledWriter writer;
        string outputConfig = writer.write(root);
        QString output = QString::fromStdString(outputConfig);
        return output;
}

// write 

// write
string GlassFileIO::geometricShapeToString(enum GeometricShape shape)
{
	switch (shape)
	{
		case SQUARE_SHAPE:
			return "Square";
		case CIRCLE_SHAPE:
		default:
			return "Circle";
	}
}

// read
enum GeometricShape GlassFileIO::stringToGeometricShape(string id)
{
	if (id == "Square")
		return SQUARE_SHAPE;
	else 
		return CIRCLE_SHAPE;
}

// write
string GlassFileIO::idAndNameToString(unsigned int id, string name)
{
	char tmp[200];
	sprintf(tmp, "%d_", id);
	return string(tmp) + name;
}

// read
int GlassFileIO::stringToId(string id)
{
	return atoi(id.substr(0, id.find("_")).c_str()); 
}

string GlassFileIO::stringToName(string id)
{
	return id.substr(id.find("_") + 1);
}

// write
string GlassFileIO::pullTemplateToString(enum PullTemplate::Type type)
{
	switch (type)
	{
		case PullTemplate::BASE_CIRCLE:
			return "Base Circle";
		case PullTemplate::BASE_SQUARE:
			return "Base Square";
		case PullTemplate::HORIZONTAL_LINE_CIRCLE:
			return "Horizontal Line Circle";
		case PullTemplate::HORIZONTAL_LINE_SQUARE:
			return "Horizontal Line Square";
		case PullTemplate::TRIPOD:
			return "Tripod";
		case PullTemplate::CROSS:
			return "Cross";
		case PullTemplate::SQUARE_OF_CIRCLES:
			return "Square of Circles";
		case PullTemplate::SQUARE_OF_SQUARES:
			return "Square of Squares"; 
		case PullTemplate::SURROUNDING_CIRCLE:
			return "Surrounding Circle";
		case PullTemplate::SURROUNDING_SQUARE:
			return "Surrounding Square";
		case PullTemplate::CUSTOM:
		default:
			return "Custom";
	}
}

// read
enum PullTemplate::Type GlassFileIO::stringToPullTemplate(string s)
{
	if (s == "Base Circle")
		return PullTemplate::BASE_CIRCLE;
	if (s == "Base Square")
		return PullTemplate::BASE_SQUARE;
	if (s == "Horizontal Line Circle")
		return PullTemplate::HORIZONTAL_LINE_CIRCLE;
	if (s == "Horizontal Line Square")
		return PullTemplate::HORIZONTAL_LINE_SQUARE;
	if (s == "Tripod")
		return PullTemplate::TRIPOD;
	if (s == "Cross")
		return PullTemplate::CROSS;
	if (s == "Square of Circles")
		return PullTemplate::SQUARE_OF_CIRCLES;
	if (s == "Square of Squares") 
		return PullTemplate::SQUARE_OF_SQUARES;
	if (s == "Surrounding Circle")
		return PullTemplate::SURROUNDING_CIRCLE;
	if (s == "Surrounding Square")
		return PullTemplate::SURROUNDING_SQUARE;
	if (s == "Custom")
		return PullTemplate::CUSTOM;
		
	return PullTemplate::CUSTOM;
}

// write
void GlassFileIO::writeBuildInformation(Json::Value& root)
{
	QFile versionFile(":/version.txt");
	QString revision;
	QString date;
	if (versionFile.open(QIODevice::ReadOnly))
	{
		QTextStream in(&versionFile);
		revision = in.readLine();
		date = in.readLine();
	}
	versionFile.close();

	root["Revision"] = revision.toStdString();
	root["Date"] = date.toStdString();
}

// read
void GlassFileIO::readBuildInformation(Json::Value& root, unsigned int* revision, string* date)
{
	*revision = root["Revision"].asUInt();
	*date = root["Date"].asString();
}

// write
void GlassFileIO::writeColor(Json::Value& root, GlassColor* color, unsigned int colorIndex)
{
	string colorname = idAndNameToString(colorIndex, color->getName()->toStdString());
	Color c = *(color->getColor());

	root[colorname]["R"] = c.r;
	root[colorname]["G"] = c.g;
	root[colorname]["B"] = c.b;
	root[colorname]["Alpha"] = c.a;
}

// read
GlassColor* GlassFileIO::readColor(Json::Value& root, string colorname, unsigned int& colorIndex)
{
	string colorName = stringToName(colorname);
	colorIndex = stringToId(colorname);

	Color color;
	color.r = root[colorname]["R"].asFloat();
	color.g = root[colorname]["G"].asFloat();
	color.b = root[colorname]["B"].asFloat();
	color.a = root[colorname]["Alpha"].asFloat();

	return new GlassColor(color, QString::fromStdString(colorName));
}

void GlassFileIO::writeColors(Json::Value& root, vector<GlassColor*>& colors)
{
	for (unsigned int i = 0; i < colors.size(); ++i)
		writeColor(root, colors[i], i+1);
}

void GlassFileIO::readColors(Json::Value& colorRoot, map<unsigned int, GlassColor*>& colorMap, vector<GlassColor*>& readColors)
{
	// clear out readColors for consistency with readPlans()
	readColors.clear();

	// add global color for map completeness
	colorMap[0] = GlobalGlass::color();
	
	//loop over all colors; read each and add it to the map
        for (unsigned int i = 0; i < colorRoot.getMemberNames().size(); i++) 
        {
		unsigned int colorIndex;
		GlassColor* gc = readColor(colorRoot, colorRoot.getMemberNames()[i], colorIndex);
		if (gc != NULL)
		{
			readColors.push_back(gc);
			colorMap[colorIndex] = gc;
		}
	}
}


/* 
following layers of cane information in the Json representation:
cane
	pull template
	twist
	casings
		casing1
			color
			shape
			thickness
		casing2
		...
	pull template parameters
		parameter1
			value
			lower limit
			upper limit
		parameter2
		...
	subpulls
		subpull1
			pull id
			diameter
			shape
			X
			Y
			Z
		subpull2
		...


So nested levels of the json are:

caneRoot
	casingRoot
		casingRoot
	pullTemplateParamsRoot
		pullTemplateParamRoot
	subpullsRoot
		subpullRoot
*/

// write
void GlassFileIO::writeCane(Json::Value& root, PullPlan* plan, unsigned int planIndex, map<PullPlan*, unsigned int>& planMap, 
	map<GlassColor*, unsigned int>& colorMap)
{
	string canename = idAndNameToString(planIndex, "Cane");
	
	// write singletons: pull template, twist
	root[canename]["Pull template"] = pullTemplateToString(plan->getTemplateType());
	root[canename]["Twist"] = plan->getTwist();

	// write casings	
	root[canename]["Casings"];
	for (unsigned int i = 0; i < plan->getCasingCount(); ++i)
	{
		string casingName = idAndNameToString(i, "Casing");
		root[canename]["Casings"][casingName]["Casing shape"] = geometricShapeToString(plan->getCasingShape(i));
		root[canename]["Casings"][casingName]["Casing thickness"] = plan->getCasingThickness(i);
		root[canename]["Casings"][casingName]["Casing color"] = colorMap[plan->getCasingColor(i)];
	}

	// write pull template parameters
	root[canename]["Pull template parameters"];
	for (unsigned int i = 0; i < plan->getParameterCount(); ++i)
	{
		TemplateParameter tmpParam;
		plan->getParameter(i, &tmpParam);

		string paramName = tmpParam.name;
		root[canename]["Pull template parameters"][paramName]["Value"] = tmpParam.value;
		root[canename]["Pull template parameters"][paramName]["Lower limit"] = tmpParam.lowerLimit;
		root[canename]["Pull template parameters"][paramName]["Upper limit"] = tmpParam.upperLimit;
	}

	// loop over subpulls
	root[canename]["Subpulls"];
	for (unsigned int i = 0; i < plan->subs.size(); ++i)
	{
		string subpullName = idAndNameToString(i+1, "Subpull");
		root[canename]["Subpulls"][subpullName]["Cane"] = planMap[plan->subs[i].plan];
		root[canename]["Subpulls"][subpullName]["Diameter"] = plan->subs[i].diameter;
		root[canename]["Subpulls"][subpullName]["Shape"] = geometricShapeToString(plan->subs[i].shape);
		root[canename]["Subpulls"][subpullName]["X"] = plan->subs[i].location.x;
		root[canename]["Subpulls"][subpullName]["Y"] = plan->subs[i].location.y;
		root[canename]["Subpulls"][subpullName]["Z"] = plan->subs[i].location.z;
	}
}


// read
PullPlan* GlassFileIO::readCane(string canename, Json::Value& root, unsigned int& planIndex, 
	map<unsigned int, GlassColor*>& colorMap)
{
	// TODO:fail when error is encountered, rather than charging blindly into the chars
        planIndex = stringToId(canename);


	// read singletons: pull template, twist
	PullPlan* plan = new PullPlan(stringToPullTemplate(root[canename]["Pull template"].asString()));
	plan->setTwist(root[canename]["Twist"].asInt());


	// loop over casings
	vector<Casing> tempCasings;
	for (unsigned int i = 0; i < root[canename]["Casings"].getMemberNames().size(); ++i) // initialize temporary list
		tempCasings.push_back(Casing(1.0, CIRCLE_SHAPE, NULL));
	for (unsigned int i = 0; i < root[canename]["Casings"].getMemberNames().size(); ++i) // read in json to list
	{
		string casingname = root[canename]["Casings"].getMemberNames()[i];
		int casingIndex = stringToId(casingname);

		GeometricShape casingShape = stringToGeometricShape(
			root[canename]["Casings"][casingname]["Casing shape"].asString());
		float casingThickness = root[canename]["Casings"][casingname]["Casing thickness"].asFloat();
		unsigned int casingColorIndex = root[canename]["Casings"][casingname]["Casing color"].asUInt();

		tempCasings[casingIndex].shape = casingShape;
		tempCasings[casingIndex].thickness = casingThickness;
		tempCasings[casingIndex].glassColor = colorMap[casingColorIndex];
	}
	// make an initial pass to get casings with the right shapes piled on
	// note: innermost casing is determined by pull template
	for (unsigned int i = 1; i < root[canename]["Casings"].getMemberNames().size(); ++i) 
	{
		if (i == 1 && plan->getCasingCount() == 2) // you already got your casing
			plan->setOutermostCasingShape(tempCasings[i].shape);
		else 
			plan->addCasing(tempCasings[i].shape);
	}
	// make a second pass to set thickness and color
	for (unsigned int i = 0; i < root[canename]["Casings"].getMemberNames().size(); ++i) 
	{
		plan->setCasingThickness(tempCasings[i].thickness, i);
		plan->setCasingColor(tempCasings[i].glassColor, i);
	}


	// loop over pull template parameters
	for (unsigned int i = 0; i < root[canename]["Pull template parameters"].getMemberNames().size(); ++i)
	{
		string paramName = root[canename]["Pull template parameters"].getMemberNames()[i];
		int paramValue = root[canename]["Pull template parameters"][paramName]["Value"].asInt();
		// don't bother to read lower and upper limits?

		// a total hack and will fail to work if plan templates 
		// ever have multiple parameters 
		// TODO:fix
		plan->setParameter(0, paramValue); 
	}

	return plan;
}

void GlassFileIO::readCaneSubpulls(Json::Value& root, PullPlan* plan, map<unsigned int, PullPlan*>& caneMap)
{
	// loop over subpulls
	for (unsigned int i = 0; i < root["Subpulls"].getMemberNames().size(); ++i)
	{
		string subpullname = root["Subpulls"].getMemberNames()[i];
		plan->subs[i].plan = caneMap[root["Subpulls"][subpullname]["Cane"].asUInt()];
		plan->subs[i].diameter = root["Subpulls"][subpullname]["Diameter"].asFloat();
		plan->subs[i].shape = stringToGeometricShape(root["Subpulls"][subpullname]["Shape"].asString());
		plan->subs[i].location.x = root["Subpulls"][subpullname]["X"].asFloat();
		plan->subs[i].location.y = root["Subpulls"][subpullname]["Y"].asFloat();
		plan->subs[i].location.z = root["Subpulls"][subpullname]["Z"].asFloat();
	}
}

// write 
void GlassFileIO::writeCanes(Json::Value& root, vector<PullPlan*>& plans, vector<GlassColor*>& colors)
{
	// generate color map
	map<GlassColor*, unsigned int> colorMap;
	colorMap[GlobalGlass::color()] = 0;
	for (unsigned int i = 0; i < colors.size(); ++i)
		colorMap[colors[i]] = i+1;

	// generate pull plan map
	map<PullPlan*, unsigned int> planMap;
	planMap[GlobalGlass::circlePlan()] = 0;
	planMap[GlobalGlass::squarePlan()] = 1;
	for (unsigned int i = 0; i < plans.size(); ++i)
		planMap[plans[i]] = i+2;

	// call writeCane() on each cane
	for (unsigned int i = 0; i < plans.size(); ++i)
	{
		writeCane(root, plans[i], i+2, planMap, colorMap);
	}
}

// read
void GlassFileIO::readCanes(Json::Value& canesRoot, map<unsigned int, PullPlan*>& caneMap, 
	map<unsigned int, GlassColor*>& colorMap, vector<PullPlan*>& readPlans)
{
	// clear out readPlans, since the correspondence between 
	// readPlans[i] and canesRoot.getMemberNames()[i] is used
	readPlans.clear();

	// add global plan to map for completeness
	caneMap[0] = GlobalGlass::circlePlan();
	caneMap[1] = GlobalGlass::squarePlan();

	// loop over canes
	for (unsigned int i = 0; i < canesRoot.getMemberNames().size(); ++i)
	{
		unsigned int planIndex;
		string canename = canesRoot.getMemberNames()[i];
		PullPlan* plan = readCane(canename, canesRoot, planIndex, colorMap);
		if (plan != NULL)
		{
			readPlans.push_back(plan);
			caneMap[planIndex] = plan;
		}	
	}

	// loop again to fill in subpulls
	for (unsigned int i = 0; i < canesRoot.getMemberNames().size(); ++i)
	{
		Json::Value caneRoot = canesRoot[canesRoot.getMemberNames()[i]];	
		readCaneSubpulls(caneRoot, readPlans[i], caneMap);
	}
}

#ifdef UNDEF
void GlassFileIO::readPieces(Json::Value root, map<PullPlan*, unsigned int>* caneMap, 
	map<GlassColor*, unsigned int>* colorMap, vector<Piece*>* readPieces)
{
	vector<string> vecPieceMembers = root.getMemberNames(); //vec with pieces

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

	static map<string, int> mapEnum;
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
		if(member.find("_")!=string::npos){
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
				if(member.find("_")!=string::npos){
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
			case Tumbler :
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
			vector<string> vecPieceTemplParameters = rootPieceValues["PieceTemplateParameters"].getMemberNames();
			for(unsigned int i=0; i< vecPieceTemplParameters.size(); i++)
			{
				if ((vecPieceTemplParameters[i].find("_") != string::npos) 
					&& (rootPieceValues["PieceTemplateParameters"].isMember(vecPieceTemplParameters[i])))
				{
						piece->setParameter(i, 
							rootPieceValues["PieceTemplateParameters"][vecPieceTemplParameters[i]].asInt());
				}
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
			vector<string> vecSubPickupPara = rootPieceValues["SubPickups"]["SubPickupTemplateParameters"].getMemberNames();
			if (vecSubPickupPara.size() < 3)
			{
				if ((rootPieceValues["SubPickups"]["SubPickupTemplateParameters"].isMember("Column count"))
					&&(rootPieceValues["SubPickups"]["SubPickupTemplateParameters"]["Column count"]!=rootPieceValues["NULL"]))
				{
					piece->pickup->setParameter(0, 
						rootPieceValues["SubPickups"]["SubPickupTemplateParameters"]["Column count"].asInt());
				}
				if ((rootPieceValues["SubPickups"]["SubPickupTemplateParameters"].isMember("Row/Column count"))
					&& (rootPieceValues["SubPickups"]["SubPickupTemplateParameters"]["Row/Column count"] 
						!= rootPieceValues["NULL"]))
				{
					piece->pickup->setParameter(0, 
						rootPieceValues["SubPickups"]["SubPickupTemplateParameters"]["Row/Column count"].asInt());
				}
				if (rootPieceValues["SubPickups"]["SubPickupTemplateParameters"].isMember("Thickness") 
					&& (rootPieceValues["SubPickups"]["SubPickupTemplateParameters"]["Thickness"]!=rootPieceValues["NULL"]))
				{
					piece->pickup->setParameter(1, 
						rootPieceValues["SubPickups"]["SubPickupTemplateParameters"]["Thickness"].asInt());
				}
			}
		}

		vector<string> vecSubPickups = rootPieceValues["SubPickups"].getMemberNames();

		for(unsigned int j = 0; j < vecSubPickups.size()-2; j++)
		{

			string member = vecSubPickups[j];
			int number =0;
			//fixes "wrong" order (0, 10, 11..., 1, 2 ,.., 9)
			if(member.find("_")!=string::npos){
				member.resize(vecSubPickups[j].find("_"));
				number = atoi(member.c_str());
			}

			std::ostringstream help;
			help << number << "_Cane";
			string var = help.str();

			if(rootPieceValues["SubPickups"].isMember(var))
			{
				vector<string> vecSubPickupCanes = rootPieceValues["SubPickups"][vecSubPickups[number]].getMemberNames();
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
					} 
					else
					{
						shape = SQUARE_SHAPE;
					}

					SubpickupTemplate *pick = new SubpickupTemplate(plan, location, rootSubPickupCanes["Orientation"].asInt(),
						rootSubPickupCanes["Length"].asFloat(), rootSubPickupCanes["Width"].asFloat(), shape);
					piece->pickup->subs[number] = *pick;
				}
			} 
			else
			{
				cout << "Not enough SubPickups available!" << endl;
			}
		}

		readPieces->push_back(piece);
	}

	delete index;
}
#endif

// write
bool GlassFileIO::write(QString filename, vector<GlassColor*>& colors, vector<PullPlan*>& plans, 
	vector<Piece*>& /*pieces*/)
{
	Json::Value root;
	writeBuildInformation(root["Build information"]);
	writeColors(root["Colors"], colors);
	writeCanes(root["Canes"], plans, colors);

	QFile saveFile(filename);
	saveFile.open(QIODevice::WriteOnly | QIODevice::Text);
	QTextStream fileOutput(&saveFile);
	fileOutput << writeJson(root);
	saveFile.close();

	return true; // successlol
}

// read
bool GlassFileIO::read(QString filename, vector<GlassColor*>& colors, vector<PullPlan*>& plans, 
	vector<Piece*>& /*pieces*/)
{
	QFile openFile(filename);
	openFile.open(QIODevice::ReadOnly | QIODevice::Text);
	QTextStream fileInput(&openFile);
	QString QStr = fileInput.readAll();
	string str = QStr.toStdString();
	Json::Value root;
	Json::Reader reader;

	bool parsedSuccess = reader.parse(str, root, false);
	if (!parsedSuccess)
		return false;

	unsigned int revision;
	string date;
	readBuildInformation(root["Build Information"], &revision, &date);
	//if (revision < 855)
	//	return false;

	map<unsigned int, GlassColor*> colorMap;
	if (root.isMember("Colors"))
		readColors(root["Colors"], colorMap, colors);
	map<unsigned int, PullPlan*> planMap;
	if (root.isMember("Canes"))
		readCanes(root["Canes"], planMap, colorMap, plans);
	//if (root.hasMember("Pieces"))
	//	readPieces(root["Pieces"], &caneMap, &colorMap, pieces);

	return true;
}



