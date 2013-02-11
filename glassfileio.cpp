
#include "glassfileio.h"

#include <map>
#include <vector>
#include <string>
#include <stdio.h> // for sscanf
#include <QTextStream>
#include <QFile>

#include "glasscolor.h"
#include "pullplan.h"
#include "piece.h"
#include "globalglass.h"

using std::string;
using std::vector;
using std::map;
	
QString GlassFileIO :: writeJson(Json::Value& root)
{
        Json::StyledWriter writer;
        string outputConfig = writer.write(root);
        QString output = QString::fromStdString(outputConfig);
        return output;
}

// write
string GlassFileIO::orientationToString(enum PickupCaneOrientation ori)
{
	switch (ori)
	{
		case HORIZONTAL_PICKUP_CANE_ORIENTATION:
			return "Horizontal";
		case VERTICAL_PICKUP_CANE_ORIENTATION:
			return "Vertical";
		case MURRINE_PICKUP_CANE_ORIENTATION:
		default:
			return "Murrine";
	}
}	

// read
enum PickupCaneOrientation GlassFileIO::stringToOrientation(string s)
{
	if (s == "Horizontal")
		return HORIZONTAL_PICKUP_CANE_ORIENTATION;
	if (s == "Vertical")
		return VERTICAL_PICKUP_CANE_ORIENTATION;
	if (s == "Murrine")
		return MURRINE_PICKUP_CANE_ORIENTATION;

	return MURRINE_PICKUP_CANE_ORIENTATION;
}

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
string GlassFileIO::pickupTemplateToString(enum PickupTemplate::Type type)
{
	switch (type)
	{
		case PickupTemplate::VERTICAL:
			return "Vertical";
		case PickupTemplate::RETICELLO_VERTICAL_HORIZONTAL:
			return "Reticello vertical horizontal";
		case PickupTemplate::MURRINE_COLUMN:
			return "Murrine column";
		case PickupTemplate::VERTICALS_AND_HORIZONTALS:
			return "Verticals and horizontals";
		case PickupTemplate::VERTICAL_HORIZONTAL_VERTICAL:
			return "Vertical horizontal vertical";
		case PickupTemplate::VERTICAL_WITH_LIP_WRAP:
			return "Vertical with lip wrap";
		case PickupTemplate::MURRINE_ROW:
			return "Murrine row";
		case PickupTemplate::MURRINE:
		default:
			return "Murrine";
	}
}

// read 
enum PickupTemplate::Type GlassFileIO::stringToPickupTemplate(string s)
{
	if (s == "Vertical")
		return PickupTemplate::VERTICAL;
	if (s == "Reticello vertical horizontal")
		return PickupTemplate::RETICELLO_VERTICAL_HORIZONTAL;
	if (s == "Murrine column")
		return PickupTemplate::MURRINE_COLUMN;
	if (s == "Verticals and horizontals")
		return PickupTemplate::VERTICALS_AND_HORIZONTALS;
	if (s == "Vertical horizontal vertical")
		return PickupTemplate::VERTICAL_HORIZONTAL_VERTICAL;
	if (s == "Vertical with lip wrap")
		return PickupTemplate::VERTICAL_WITH_LIP_WRAP;
	if (s == "Murrine row")
		return PickupTemplate::MURRINE_ROW;
	if (s == "Murrine")
		return PickupTemplate::MURRINE;	

	return PickupTemplate::VERTICAL;
}

// write
string GlassFileIO::pieceTemplateToString(enum PieceTemplate::Type type)
{
	switch (type)
	{
		case PieceTemplate::TUMBLER:
			return "Tumbler";
		case PieceTemplate::BOWL:
			return "Bowl";
		case PieceTemplate::VASE:
			return "Vase";
		case PieceTemplate::POT:
			return "Pot";
		case PieceTemplate::WAVY_PLATE:
			return "Wavy plate";
		case PieceTemplate::PICKUP:
		default:
			return "Pickup";
	}
}

// read
enum PieceTemplate::Type GlassFileIO::stringToPieceTemplate(string s)
{
	if (s == "Tumbler")
		return PieceTemplate::TUMBLER;
	if (s == "Bowl")
		return PieceTemplate::BOWL;
	if (s == "Vase")
		return PieceTemplate::VASE;
	if (s == "Pot")
		return PieceTemplate::POT;
	if (s == "Wavy plate")
		return PieceTemplate::WAVY_PLATE;
	if (s == "Pickup")
		return PieceTemplate::PICKUP;

	return PieceTemplate::TUMBLER;
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

	unsigned int revisionNumber = 0;
	sscanf(revision.toStdString().c_str(), "%u", &revisionNumber);
	root["Build information"]["Revision"] = revisionNumber; 
	root["Build information"]["Date"] = date.toStdString();
}

// read
void GlassFileIO::readBuildInformation(Json::Value& root, unsigned int& revision, string& date)
{
	revision = 0;
	date = "unknown";

	if (!root.isMember("Build information"))
		return;

	revision = root["Build information"]["Revision"].asUInt();
	date = root["Build information"]["Date"].asString();
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
	// clear out readColors for consistency with readCanes()
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
			index
			name
			value
			lower limit
			upper limit
		parameter2
		...
	subpulls
		subpull1
			index
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
void GlassFileIO::writeCane(Json::Value& root, PullPlan* cane, unsigned int caneIndex, map<PullPlan*, unsigned int>& caneMap, 
	map<GlassColor*, unsigned int>& colorMap)
{
	string canename = idAndNameToString(caneIndex, "Cane");
	
	// write singletons: pull template, twist
	root[canename]["Pull template"] = pullTemplateToString(cane->getTemplateType());
	root[canename]["Twist"] = cane->getTwist();

	// write casings	
	root[canename]["Casings"];
	for (unsigned int i = 0; i < cane->getCasingCount(); ++i)
	{
		string casingName = idAndNameToString(i, "Casing");
		root[canename]["Casings"][casingName]["Casing shape"] = geometricShapeToString(cane->getCasingShape(i));
		root[canename]["Casings"][casingName]["Casing thickness"] = cane->getCasingThickness(i);
		root[canename]["Casings"][casingName]["Casing color"] = colorMap[cane->getCasingColor(i)];
	}

	// write pull template parameters
	root[canename]["Pull template parameters"];
	for (unsigned int i = 0; i < cane->getParameterCount(); ++i)
	{
		TemplateParameter tmpParam;
		cane->getParameter(i, &tmpParam);

		string paramName = tmpParam.name;
		root[canename]["Pull template parameters"][paramName]["Index"] = i;
		root[canename]["Pull template parameters"][paramName]["Name"] = tmpParam.name;
		root[canename]["Pull template parameters"][paramName]["Value"] = tmpParam.value;
		root[canename]["Pull template parameters"][paramName]["Lower limit"] = tmpParam.lowerLimit;
		root[canename]["Pull template parameters"][paramName]["Upper limit"] = tmpParam.upperLimit;
	}

	// loop over subpulls
	root[canename]["Subpulls"];
	for (unsigned int i = 0; i < cane->subs.size(); ++i)
	{
		string subpullName = idAndNameToString(i, "Subpull");
		root[canename]["Subpulls"][subpullName]["Index"] = i;
		root[canename]["Subpulls"][subpullName]["Cane"] = caneMap[cane->subs[i].plan];
		root[canename]["Subpulls"][subpullName]["Diameter"] = cane->subs[i].diameter;
		root[canename]["Subpulls"][subpullName]["Shape"] = geometricShapeToString(cane->subs[i].shape);
		root[canename]["Subpulls"][subpullName]["X"] = cane->subs[i].location.x;
		root[canename]["Subpulls"][subpullName]["Y"] = cane->subs[i].location.y;
		root[canename]["Subpulls"][subpullName]["Z"] = cane->subs[i].location.z;
	}
}


// read
PullPlan* GlassFileIO::readCane(string canename, Json::Value& root, unsigned int& caneIndex, 
	map<unsigned int, GlassColor*>& colorMap)
{
        caneIndex = stringToId(canename);

	// read singletons: pull template, twist
	PullPlan* cane = new PullPlan(stringToPullTemplate(root[canename]["Pull template"].asString()));
	cane->setTwist(root[canename]["Twist"].asInt());

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
		if (i == 1 && cane->getCasingCount() == 2) // you already got your casing
			cane->setOutermostCasingShape(tempCasings[i].shape);
		else 
			cane->addCasing(tempCasings[i].shape);
	}
	// make a second pass to set thickness and color
	for (unsigned int i = 0; i < root[canename]["Casings"].getMemberNames().size(); ++i) 
	{
		cane->setCasingThickness(tempCasings[i].thickness, i);
		cane->setCasingColor(tempCasings[i].glassColor, i);
	}

	// loop over pull template parameters
	for (unsigned int i = 0; i < root[canename]["Pull template parameters"].getMemberNames().size(); ++i)
	{
		string paramname = root[canename]["Pull template parameters"].getMemberNames()[i];
		unsigned int paramIndex = root[canename]["Pull template parameters"][paramname]["Index"].asUInt();
		cane->setParameter(paramIndex, root[canename]["Pull template parameters"][paramname]["Value"].asInt());
		// don't bother to read name or lower/upper limits
	}

	return cane;
}

void GlassFileIO::readCaneSubpulls(Json::Value& root, PullPlan* cane, map<unsigned int, PullPlan*>& caneMap)
{
	// loop over subpulls
	for (unsigned int i = 0; i < root["Subpulls"].getMemberNames().size(); ++i)
	{
		string subpullname = root["Subpulls"].getMemberNames()[i];
		unsigned int subpullIndex = root["Subpulls"][subpullname]["Index"].asUInt();

		cane->subs[subpullIndex].plan = caneMap[root["Subpulls"][subpullname]["Cane"].asUInt()];
		cane->subs[subpullIndex].diameter = root["Subpulls"][subpullname]["Diameter"].asFloat();
		cane->subs[subpullIndex].shape = stringToGeometricShape(root["Subpulls"][subpullname]["Shape"].asString());
		cane->subs[subpullIndex].location.x = root["Subpulls"][subpullname]["X"].asFloat();
		cane->subs[subpullIndex].location.y = root["Subpulls"][subpullname]["Y"].asFloat();
		cane->subs[subpullIndex].location.z = root["Subpulls"][subpullname]["Z"].asFloat();
	}
}

// write 
void GlassFileIO::writeCanes(Json::Value& root, vector<PullPlan*>& canes, vector<GlassColor*>& colors)
{
	// generate color map
	map<GlassColor*, unsigned int> colorMap;
	colorMap[GlobalGlass::color()] = 0;
	for (unsigned int i = 0; i < colors.size(); ++i)
		colorMap[colors[i]] = i+1;

	// generate pull plan map
	map<PullPlan*, unsigned int> caneMap;
	caneMap[GlobalGlass::circlePlan()] = 0;
	caneMap[GlobalGlass::squarePlan()] = 1;
	for (unsigned int i = 0; i < canes.size(); ++i)
		caneMap[canes[i]] = i+2;

	// call writeCane() on each cane
	for (unsigned int i = 0; i < canes.size(); ++i)
	{
		writeCane(root, canes[i], i+2, caneMap, colorMap);
	}
}

// read
void GlassFileIO::readCanes(Json::Value& canesRoot, map<unsigned int, PullPlan*>& caneMap, 
	map<unsigned int, GlassColor*>& colorMap, vector<PullPlan*>& readCanes)
{
	// clear out readCanes, since the correspondence between 
	// readCanes[i] and canesRoot.getMemberNames()[i] is used
	readCanes.clear();

	// add global plan to map for completeness
	caneMap[0] = GlobalGlass::circlePlan();
	caneMap[1] = GlobalGlass::squarePlan();

	// loop over canes
	for (unsigned int i = 0; i < canesRoot.getMemberNames().size(); ++i)
	{
		unsigned int caneIndex;
		string canename = canesRoot.getMemberNames()[i];
		PullPlan* cane = readCane(canename, canesRoot, caneIndex, colorMap);
		if (cane != NULL)
		{
			readCanes.push_back(cane);
			caneMap[caneIndex] = cane;
		}	
	}

	// loop again to fill in subpulls
	for (unsigned int i = 0; i < canesRoot.getMemberNames().size(); ++i)
	{
		Json::Value caneRoot = canesRoot[canesRoot.getMemberNames()[i]];	
		readCaneSubpulls(caneRoot, readCanes[i], caneMap);
	}
}

/*
following layers of pickup information in the Json representation:

pickup
	pickup template
	casing color
	overlay color
	underlay color
	pickup template parameters
		parameter1
			index
			name
			value
			lower limit
			upper limit
		parameter2
		...
	subpulls
		subpull1
			index
			pull id
			length
			width
			orientation
			shape
			x
			y
			z
		subpull2
		...
*/
void GlassFileIO::writePickup(Json::Value& root, PickupPlan* pickup, unsigned int pickupIndex,
	map<PullPlan*, unsigned int>& caneMap, map<GlassColor*, unsigned int>& colorMap)
{
	string pickupname = idAndNameToString(pickupIndex, "Pickup");

	// write singletons: casing color, overlay color, underlay color, pickup template
	root[pickupname]["Pickup template"] = pickupTemplateToString(pickup->getTemplateType()); 
	root[pickupname]["Casing color"] = colorMap[pickup->casingGlassColor]; 
	root[pickupname]["Overlay color"] = colorMap[pickup->overlayGlassColor];
	root[pickupname]["Underlay color"] = colorMap[pickup->underlayGlassColor];
		
	// write pickup template parameters
	root[pickupname]["Pickup template parameters"];
	for (unsigned int i = 0; i < pickup->getParameterCount(); ++i)
	{
		TemplateParameter tmpParam;
		pickup->getParameter(i, &tmpParam);

		string paramName = idAndNameToString(i, "PickupTemplateParam");
		root[pickupname]["Pickup template parameters"][paramName]["Index"] = i;
		root[pickupname]["Pickup template parameters"][paramName]["Name"] = tmpParam.name;
		root[pickupname]["Pickup template parameters"][paramName]["Value"] = tmpParam.value;
		root[pickupname]["Pickup template parameters"][paramName]["Lower limit"] = tmpParam.lowerLimit;
		root[pickupname]["Pickup template parameters"][paramName]["Upper limit"] = tmpParam.upperLimit;
	}

	// write subpulls
	root[pickupname]["Subpulls"];
	for (unsigned int i = 0; i < pickup->subs.size(); ++i)
	{
		string subpullName = idAndNameToString(i, "Subpull");
		root[pickupname]["Subpulls"][subpullName]["Index"] = i;
		root[pickupname]["Subpulls"][subpullName]["Cane"] = caneMap[pickup->subs[i].plan];
		root[pickupname]["Subpulls"][subpullName]["Length"] = pickup->subs[i].length;
		root[pickupname]["Subpulls"][subpullName]["Width"] = pickup->subs[i].width;
		root[pickupname]["Subpulls"][subpullName]["Orientation"] = orientationToString(pickup->subs[i].orientation);
		root[pickupname]["Subpulls"][subpullName]["Shape"] = geometricShapeToString(pickup->subs[i].shape);
		root[pickupname]["Subpulls"][subpullName]["X"] = pickup->subs[i].location.x;
		root[pickupname]["Subpulls"][subpullName]["Y"] = pickup->subs[i].location.y;
		root[pickupname]["Subpulls"][subpullName]["Z"] = pickup->subs[i].location.z;
	}
}

PickupPlan* GlassFileIO::readPickup(string pickupname, Json::Value& root,
	map<unsigned int, PullPlan*>& caneMap, map<unsigned int, GlassColor*>& colorMap)
{
	// read singletons: casing color, overlay color, underlay color, pickup template
	PickupPlan* pickup = new PickupPlan(stringToPickupTemplate(root[pickupname]["Pickup template"].asString()));
	pickup->casingGlassColor = colorMap[root[pickupname]["Casing color"].asUInt()];
	pickup->overlayGlassColor = colorMap[root[pickupname]["Overlay color"].asUInt()];
	pickup->underlayGlassColor = colorMap[root[pickupname]["Underlay color"].asUInt()];

	// read pickup template parameters
	for (unsigned int i = 0; i < root[pickupname]["Pickup template parameters"].getMemberNames().size(); ++i)
	{
		string paramname = root[pickupname]["Pickup template parameters"].getMemberNames()[i];
		unsigned int paramIndex = root[pickupname]["Pickup template parameters"][paramname]["Index"].asUInt();	

		int paramValue = root[pickupname]["Pickup template parameters"][paramname]["Value"].asInt();	
		pickup->setParameter(paramIndex, paramValue); 
		// we just ignore name and lower/upper limits for now...they are redundant with pickup template type
	}

	// read subpulls
	for (unsigned int i = 0; i < root[pickupname]["Subpulls"].getMemberNames().size(); ++i)
	{
		string subpullname = root[pickupname]["Subpulls"].getMemberNames()[i];
		unsigned int subIndex = root[pickupname]["Subpulls"][subpullname]["Index"].asUInt();
	
		pickup->subs[subIndex].plan = caneMap[root[pickupname]["Subpulls"][subpullname]["Cane"].asUInt()];
		pickup->subs[subIndex].length = root[pickupname]["Subpulls"][subpullname]["Length"].asFloat();
		pickup->subs[subIndex].width = root[pickupname]["Subpulls"][subpullname]["Width"].asFloat();
		pickup->subs[subIndex].orientation 
			= stringToOrientation(root[pickupname]["Subpulls"][subpullname]["Orientation"].asString());
		pickup->subs[subIndex].shape 
			= stringToGeometricShape(root[pickupname]["Subpulls"][subpullname]["Shape"].asString());
		pickup->subs[subIndex].location.x = root[pickupname]["Subpulls"][subpullname]["X"].asFloat();
		pickup->subs[subIndex].location.y = root[pickupname]["Subpulls"][subpullname]["Y"].asFloat();
		pickup->subs[subIndex].location.z = root[pickupname]["Subpulls"][subpullname]["Z"].asFloat();
	}

	return pickup;
}

/* 
following layers of piece information in the Json representation:
piece
	piece template
	piece template parameters
		parameter1
			value 
			lower limit
			upper limit
		parameter2
		...
	pickups (done separately)
*/
void GlassFileIO::writePiece(Json::Value& root, Piece* piece, unsigned int pieceIndex, map<PullPlan*, unsigned int>& caneMap,
	map<GlassColor*, unsigned int>& colorMap)
{
	string piecename = idAndNameToString(pieceIndex, "Piece");

	// write singletons: piece template	
	root[piecename]["Piece template"] = pieceTemplateToString(piece->getTemplateType());

	// write piece template parameters
	root[piecename]["Piece template parameters"];
	for (unsigned int i = 0; i < piece->getParameterCount(); ++i)
	{
		TemplateParameter tmpParam;
		piece->getParameter(i, &tmpParam);

		string paramName = idAndNameToString(i, "PieceTemplateParam");
		root[piecename]["Piece template parameters"][paramName]["Index"] = i;
		root[piecename]["Piece template parameters"][paramName]["Name"] = tmpParam.name;
		root[piecename]["Piece template parameters"][paramName]["Value"] = tmpParam.value;
		root[piecename]["Piece template parameters"][paramName]["Lower limit"] = tmpParam.lowerLimit;
		root[piecename]["Piece template parameters"][paramName]["Upper limit"] = tmpParam.upperLimit;
	}

	// write pickups (currently only one)
	writePickup(root[piecename]["Pickups"], piece->pickup, 0, caneMap, colorMap);
}

Piece* GlassFileIO::readPiece(string piecename, Json::Value& root, map<unsigned int, PullPlan*>& caneMap, 
	map<unsigned int, GlassColor*>& colorMap)
{
	// read singletons: piece template
	Piece* piece = new Piece(stringToPieceTemplate(root[piecename]["Piece template"].asString()));

	// read piece template parameters
	for (unsigned int i = 0; i < root[piecename]["Piece template parameters"].getMemberNames().size(); ++i)
	{
		string paramname = root[piecename]["Piece template parameters"].getMemberNames()[i];
		unsigned int paramIndex = root[piecename]["Piece template parameters"][paramname]["Index"].asUInt();

		int paramValue = root[piecename]["Pickup template parameters"][paramname]["Value"].asInt();	
		piece->setParameter(paramIndex, paramValue); 
	}

	// read pickups (currently only one)
	for (unsigned int i = 0; i < root[piecename]["Pickups"].getMemberNames().size(); ++i)
	{
		string pickupname = root[piecename]["Pickups"].getMemberNames()[i];
		piece->pickup/*s[i]*/ = readPickup(pickupname, root[piecename]["Pickups"], caneMap, colorMap);
	}

	return piece;
}

void GlassFileIO::writePieces(Json::Value& root, vector<Piece*>& pieces, vector<PullPlan*>& canes, 
	vector<GlassColor*>& colors)
{
        // generate color map
        map<GlassColor*, unsigned int> colorMap;
        colorMap[GlobalGlass::color()] = 0;
        for (unsigned int i = 0; i < colors.size(); ++i)
                colorMap[colors[i]] = i+1;

        // generate pull plan map
        map<PullPlan*, unsigned int> caneMap;
        caneMap[GlobalGlass::circlePlan()] = 0;
        caneMap[GlobalGlass::squarePlan()] = 1;
        for (unsigned int i = 0; i < canes.size(); ++i)
                caneMap[canes[i]] = i+2;

        // call writeCane() on each cane
        for (unsigned int i = 0; i < pieces.size(); ++i)
                writePiece(root, pieces[i], i, caneMap, colorMap);
}

void GlassFileIO::readPieces(Json::Value& piecesRoot, map<unsigned int, PullPlan*>& caneMap, map<unsigned int, 
	GlassColor*>& colorMap, vector<Piece*>& readPieces)
{
        // clear out readPieces, since the correspondence between 
        // readPieces[i] and piecesRoot.getMemberNames()[i] is used
        readPieces.clear();

        // loop over pieces
        for (unsigned int i = 0; i < piecesRoot.getMemberNames().size(); ++i)
        {
                Piece* piece = readPiece(piecesRoot.getMemberNames()[i], piecesRoot, caneMap, colorMap);
                if (piece != NULL)
                {
                        readPieces.push_back(piece);
                }      
        }
}

// write
bool GlassFileIO::write(QString filename, vector<GlassColor*>& colors, vector<PullPlan*>& canes, 
	vector<Piece*>& pieces)
{
	Json::Value root;
	writeBuildInformation(root); 
	writeColors(root["Colors"], colors);
	writeCanes(root["Canes"], canes, colors);
	writePieces(root["Pieces"], pieces, canes, colors);

	QFile saveFile(filename);
	saveFile.open(QIODevice::WriteOnly | QIODevice::Text);
	QTextStream fileOutput(&saveFile);
	fileOutput << writeJson(root);
	saveFile.close();

	return true; // successlol
}

// read
bool GlassFileIO::read(QString filename, vector<GlassColor*>& colors, vector<PullPlan*>& canes, 
	vector<Piece*>& pieces)
{
	// TODO:fail when error is encountered, rather than charging blindly into the chars
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
	readBuildInformation(root, revision, date);
	if (0 < revision && revision < 865) // if you got a valid number and it's not current
		return false;

	map<unsigned int, GlassColor*> colorMap;
	if (root.isMember("Colors"))
		readColors(root["Colors"], colorMap, colors);

	map<unsigned int, PullPlan*> caneMap;
	if (root.isMember("Canes"))
		readCanes(root["Canes"], caneMap, colorMap, canes);

	if (root.isMember("Pieces"))
		readPieces(root["Pieces"], caneMap, colorMap, pieces);

	return true;
}



