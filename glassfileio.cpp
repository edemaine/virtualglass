
#include "glassfileio.h"

#include <map>
#include <vector>
#include <string>
#include <cstdio> // for sscanf

#include <QTextStream>
#include <QFile>
#include <QBuffer>

#include "glasscolor.h"
#include "pullplan.h"
#include "piece.h"
#include "globalglass.h"
#include "constants.h" // for MAX

using std::string;
using std::vector;
using std::map;


// write
bool writeColorFile(QString filename, QString collectionName, vector<GlassColor*>& colors)
{
	Json::Value root;
	string docFilename(":/colorfile_inline_doc.txt"); 
	GlassFileIOInternal::writeDocumentation(docFilename, root);
	GlassFileIOInternal::writeBuildInformation(root); 
	root["Collection name"] = collectionName.toStdString();
	GlassFileIOInternal::writeColors(root["Colors"], colors);

	GlassFileIOInternal::writeJsonToFile(filename, root);

	return true;
}

// read
bool readColorFile(QString filename, QString& collectionname, vector<GlassColor*>& colors)
{
	// TODO:fail when error is encountered, rather than charging blindly into the chars
	QFile openFile(filename);
	openFile.open(QIODevice::ReadOnly | QIODevice::Text);
	QTextStream fileInput(&openFile);
	QString qStr = fileInput.readAll();
	string str = qStr.toStdString();
	openFile.close();
	Json::Value root;
	Json::Reader reader;

	bool parsedSuccess = reader.parse(str, root, false);
	if (!parsedSuccess)
		return false;

	unsigned int revision;
	string date;
	GlassFileIOInternal::readBuildInformation(root, revision, date);
	if (0 < revision && revision < 943) // if you got a valid number and it's not current
		return false;

	if (!root.isMember("Collection name"))
		return false;
	collectionname = root["Collection name"].asString().c_str();

	map<unsigned int, GlassColor*> colorMap;
	if (root.isMember("Colors"))
		GlassFileIOInternal::readColors(root["Colors"], colorMap, colors);

	return true;
}

// write
bool writeGlassFile(QString filename, vector<GlassColor*>& colors, vector<PullPlan*>& canes, vector<Piece*>& pieces)
{
	// produce the json tree representation
	Json::Value root;
	string docFilename(":/glassfile_inline_doc.txt"); 
	GlassFileIOInternal::writeDocumentation(docFilename, root);
	GlassFileIOInternal::writeBuildInformation(root); 
	GlassFileIOInternal::writeColors(root["Colors"], colors);
	GlassFileIOInternal::writeCanes(root["Canes"], canes, colors);
	GlassFileIOInternal::writePieces(root["Pieces"], pieces, canes, colors);

	GlassFileIOInternal::writeJsonToFile(filename, root);
	
	return true; // successlol
}

bool writeGlassFile(QBuffer& buffer, vector<GlassColor*>& colors, vector<PullPlan*>& canes, vector<Piece*>& pieces)
{
	// produce the json tree representation
	Json::Value root;
	string docFilename(":/glassfile_inline_doc.txt"); 
	GlassFileIOInternal::writeDocumentation(docFilename, root);
	GlassFileIOInternal::writeBuildInformation(root); 
	GlassFileIOInternal::writeColors(root["Colors"], colors);
	GlassFileIOInternal::writeCanes(root["Canes"], canes, colors);
	GlassFileIOInternal::writePieces(root["Pieces"], pieces, canes, colors);

	GlassFileIOInternal::writeJsonToBuffer(buffer, root);

	return true; // successlol
}

// read
bool readGlassFile(QString filename, vector<GlassColor*>& colors, vector<PullPlan*>& canes, 
	vector<Piece*>& pieces)
{
	// TODO:fail when error is encountered, rather than charging blindly into the chars
	QFile openFile(filename);
	openFile.open(QIODevice::ReadOnly | QIODevice::Text);
	QTextStream fileInput(&openFile);
	QString qStr = fileInput.readAll();
	string str = qStr.toStdString();
	Json::Value root;
	Json::Reader reader;

	bool parsedSuccess = reader.parse(str, root, false);
	if (!parsedSuccess)
		return false;

	unsigned int revision;
	string date;
	GlassFileIOInternal::readBuildInformation(root, revision, date);
	if (0 < revision && revision < 943) // if you got a valid number and it's not current
		return false;

	map<unsigned int, GlassColor*> colorMap;
	if (root.isMember("Colors"))
		GlassFileIOInternal::readColors(root["Colors"], colorMap, colors);

	map<unsigned int, PullPlan*> caneMap;
	if (root.isMember("Canes"))
		GlassFileIOInternal::readCanes(root["Canes"], caneMap, colorMap, canes);

	if (root.isMember("Pieces"))
		GlassFileIOInternal::readPieces(root["Pieces"], caneMap, colorMap, pieces);

	return true;
}

namespace GlassFileIOInternal
{

void writeJsonToBuffer(QBuffer& buffer, Json::Value& root)
{
	// convert it to text using json library
	Json::StyledWriter writer;
	string outputText = writer.write(root);

	// open a savefile and write the string to it
	buffer.open(QIODevice::WriteOnly | QIODevice::Text);
	buffer.write(outputText.c_str());
	buffer.close();
}

void writeJsonToFile(QString& filename, Json::Value& root)
{
	// convert it to text using json library
	Json::StyledWriter writer;
	string outputText = writer.write(root);

	// open a savefile and write the string to it
	QFile saveFile(filename);
	saveFile.open(QIODevice::WriteOnly | QIODevice::Text);
	QTextStream fileOutput(&saveFile);
	fileOutput << QString::fromStdString(outputText);
	saveFile.close();
} 

// write
void writeDocumentation(string& filename, Json::Value& root)
{
	QFile docFile(filename.c_str());

	if (!docFile.open(QIODevice::ReadOnly))
		return;

	QTextStream docStream(&docFile);
	string docComment;

	QString line = docStream.readLine();
	while (!line.isNull())
	{
		docComment += "// " + line.toStdString() + "\n"; 
		line = docStream.readLine();
	}
	docFile.close();

	root.setComment(docComment, Json::commentBefore);
}

// read
GlassColor* safeColorMap(map<unsigned int, GlassColor*>& colorMap, unsigned int index)
{
	if (colorMap.find(index) == colorMap.end())
		return GlobalGlass::color();
	return colorMap[index];
}

PullPlan* safeCaneMap(map<unsigned int, PullPlan*>& caneMap, unsigned int index)
{
	if (caneMap.find(index) == caneMap.end())
		// not quite right in some cases, should depend on expected shape
		return GlobalGlass::circlePlan(); 
	return caneMap[index];
}

// write
string orientationToString(enum PickupCaneOrientation ori)
{
	switch (ori)
	{
		case HORIZONTAL_PICKUP_CANE_ORIENTATION:
			return "Horizontal";
		case VERTICAL_PICKUP_CANE_ORIENTATION:
			return "Vertical";
		case MURRINE_PICKUP_CANE_ORIENTATION:
			return "Murrine";
		default:
			return "Horizontal";
	}
}	

// read
enum PickupCaneOrientation stringToOrientation(string s)
{
	if (s == "Horizontal")
		return HORIZONTAL_PICKUP_CANE_ORIENTATION;
	if (s == "Vertical")
		return VERTICAL_PICKUP_CANE_ORIENTATION;
	if (s == "Murrine")
		return MURRINE_PICKUP_CANE_ORIENTATION;

	return HORIZONTAL_PICKUP_CANE_ORIENTATION;
}

// write
string geometricShapeToString(enum GeometricShape shape)
{
	switch (shape)
	{
		case CIRCLE_SHAPE:
			return "Circle";
		case SQUARE_SHAPE:
			return "Square";
		default:
			return "Circle";
	}
}

// read
enum GeometricShape stringToGeometricShape(string id)
{
	if (id == "Square")
		return SQUARE_SHAPE;
	else
		return CIRCLE_SHAPE;
}

// write
string idAndNameToString(unsigned int id, string name)
{
	char tmp[200];
	sprintf(tmp, "_%d", id);
	return name + string(tmp);
}

// read
unsigned int stringToId(string id)
{
	// if string doesn't contain a valid id, return a dummy ID that 
	// does its best to avoid collision with any real ID
	if (id.find("_") == string::npos)
		return UINT_MAX; 
	// otherwise return the best interpretation according to atoi()
	return atoi(id.substr(id.find("_") + 1).c_str()); 
}

// write
string pullTemplateToString(enum PullTemplate::Type type)
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
			return "Custom";
		default:
			return "Custom";
	}
}

// read
enum PullTemplate::Type stringToPullTemplate(string s)
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
string pickupTemplateToString(enum PickupTemplate::Type type)
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
			return "Murrine";
		default:
			return "Vertical";
	}
}

// read 
enum PickupTemplate::Type stringToPickupTemplate(string s)
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
string pieceTemplateToString(enum PieceTemplate::Type type)
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
		case PieceTemplate::PLATE:
			return "Plate";
		case PieceTemplate::FISHTRAP:
			return "Fishtrap";
		case PieceTemplate::CUSTOM:
			return "Custom";
		default:
			return "Custom";
	}
}

// read
enum PieceTemplate::Type stringToPieceTemplate(string s)
{
	if (s == "Tumbler")
		return PieceTemplate::TUMBLER;
	if (s == "Bowl")
		return PieceTemplate::BOWL;
	if (s == "Vase")
		return PieceTemplate::VASE;
	if (s == "Pot")
		return PieceTemplate::POT;
	if (s == "Plate")
		return PieceTemplate::PLATE;
	if (s == "Fishtrap")
		return PieceTemplate::FISHTRAP;
	if (s == "Custom")
		return PieceTemplate::CUSTOM;

	return PieceTemplate::CUSTOM;
}

// write
void writeBuildInformation(Json::Value& root)
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
void readBuildInformation(Json::Value& root, unsigned int& revision, string& date)
{
	revision = 0;
	date = "unknown";

	if (!root.isMember("Build information"))
		return;

	revision = root["Build information"]["Revision"].asUInt();
	date = root["Build information"]["Date"].asString();
}

// write
void writeColor(Json::Value& root, GlassColor* color, unsigned int colorIndex)
{
	string colorname = idAndNameToString(colorIndex, "Color");
	Color c = color->color();

	root[colorname]["Short name"] = color->shortName();
	root[colorname]["Long name"] = color->longName();
	root[colorname]["R"] = c.r;
	root[colorname]["G"] = c.g;
	root[colorname]["B"] = c.b;
	root[colorname]["Alpha"] = c.a;
}

// read
GlassColor* readColor(Json::Value& root, string colorname)
{
	string shortName = root[colorname]["Short name"].asString();
	string longName = root[colorname]["Long name"].asString();

	Color color;
	color.r = root[colorname]["R"].asFloat();
	color.g = root[colorname]["G"].asFloat();
	color.b = root[colorname]["B"].asFloat();
	color.a = root[colorname]["Alpha"].asFloat();

	return new GlassColor(color, shortName, longName);
}

void writeColors(Json::Value& root, vector<GlassColor*>& colors)
{
	for (unsigned int i = 0; i < colors.size(); ++i)
		writeColor(root, colors[i], i+1);
}

void readColors(Json::Value& colorRoot, map<unsigned int, GlassColor*>& colorMap, vector<GlassColor*>& readColors)
{
	// add global color for map completeness
	colorMap[0] = GlobalGlass::color();
	
	//loop over all colors; read each and add it to the map
	for (unsigned int i = 0; i < colorRoot.getMemberNames().size(); i++) 
	{
		string colorname = colorRoot.getMemberNames()[i];
		unsigned int colorIndex = stringToId(colorname);
		GlassColor* gc = readColor(colorRoot, colorname);
		colorMap[colorIndex] = gc;
	}

	// create sorted readColors
	vector<int> colorIndices;
	for (map<unsigned int, GlassColor*>::iterator cI = colorMap.begin(); cI != colorMap.end(); ++cI)
	{
		// ignore global glass index 0
		if (cI->first >= 1)
			colorIndices.push_back(cI->first);
	}
	readColors.clear();
	for (unsigned int i = 0; i < colorIndices.size(); ++i)
		readColors.push_back(colorMap[colorIndices[i]]);	
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
	subcanes
		subcane1
			index
			pull id
			diameter
			shape
			X
			Y
			Z
		subcane2
		...
*/

// write
void writeCane(Json::Value& root, PullPlan* cane, unsigned int caneIndex, map<const PullPlan*, unsigned int>& caneMap, 
	map<const GlassColor*, unsigned int>& colorMap)
{
	string canename = idAndNameToString(caneIndex, "Cane");
	
	// write singletons: pull template, twist
	root[canename]["Pull template"] = pullTemplateToString(cane->templateType());
	root[canename]["Twist"] = cane->twist();

	// write casings	
	root[canename]["Casings"];
	for (unsigned int i = 0; i < cane->casingCount(); ++i)
	{
		string casingName = idAndNameToString(i, "Casing");
		root[canename]["Casings"][casingName]["Index"] = i;
		root[canename]["Casings"][casingName]["Shape"] = geometricShapeToString(cane->getCasingShape(i));
		root[canename]["Casings"][casingName]["Thickness"] = cane->getCasingThickness(i);
		root[canename]["Casings"][casingName]["Color pointer"] = colorMap[cane->getCasingColor(i)];
	}

	// write pull template parameters
	root[canename]["Count"] = cane->count();

	// loop over subpulls
	root[canename]["Subcanes"];
	for (unsigned int i = 0; i < cane->subpullCount(); ++i)
	{
		string subpullName = idAndNameToString(i, "Subcane");
		root[canename]["Subcanes"][subpullName]["Index"] = i;
		SubpullTemplate sub = cane->getSubpullTemplate(i);
		root[canename]["Subcanes"][subpullName]["Cane pointer"] = caneMap[sub.plan];
		root[canename]["Subcanes"][subpullName]["Diameter"] = sub.diameter;
		root[canename]["Subcanes"][subpullName]["Shape"] = geometricShapeToString(sub.shape);
		root[canename]["Subcanes"][subpullName]["X"] = sub.location.x;
		root[canename]["Subcanes"][subpullName]["Y"] = sub.location.y;
	}
}


// read
PullPlan* readCane(string canename, Json::Value& root, map<unsigned int, GlassColor*>& colorMap)
{
	// read singletons: pull template, twist
	PullPlan* cane = new PullPlan(stringToPullTemplate(root[canename]["Pull template"].asString()));
	cane->setTwist(root[canename]["Twist"].asFloat());

	// make a temporary list of casings that can be modified freely, unlike those stashed inside
	// a cane, which have constraints due to collision, etc.
	// make sure you get at least once casing, even if the file doesn't contain any
	vector<Casing> tempCasings;
	for (unsigned int i = 0; i < MAX(root[canename]["Casings"].getMemberNames().size(), 1); ++i) // initialize temporary list
		tempCasings.push_back(Casing(1.0, CIRCLE_SHAPE, GlobalGlass::color()));
	// now set this temporary list to the values of casings in the file
	for (unsigned int i = 0; i < root[canename]["Casings"].getMemberNames().size(); ++i) // read in json to list
	{
		string casingname = root[canename]["Casings"].getMemberNames()[i];
		unsigned int casingIndex = root[canename]["Casings"][casingname]["Index"].asUInt();

		GeometricShape casingShape = stringToGeometricShape(
			root[canename]["Casings"][casingname]["Shape"].asString());
		float casingThickness = root[canename]["Casings"][casingname]["Thickness"].asFloat();
		unsigned int casingColorIndex = root[canename]["Casings"][casingname]["Color pointer"].asUInt();

		tempCasings[casingIndex].shape = casingShape;
		tempCasings[casingIndex].thickness = casingThickness;
		tempCasings[casingIndex].glassColor = safeColorMap(colorMap, casingColorIndex);
	}
	// make an initial pass to get casings with the right shapes piled on
	cane->setOutermostCasingShape(tempCasings[0].shape);
	for (unsigned int i = 1; i < tempCasings.size(); ++i)
		cane->addCasing(tempCasings[i].shape);
	// make a second pass to set thickness and color
	for (unsigned int i = 0; i < tempCasings.size(); ++i) 
	{
		cane->setCasingThickness(tempCasings[i].thickness, i);
		cane->setCasingColor(tempCasings[i].glassColor, i);
	}

	cane->setCount(root[canename]["Count"].asUInt());

	return cane;
}

void readCaneSubcanes(Json::Value& root, PullPlan* cane, map<unsigned int, PullPlan*>& caneMap)
{
	// if the cane's template type is custom, then it is impossible to know how many subcanes it has
	// so we make an initial pass over the subcanes just to build an initial list of the correct size
	if (cane->templateType() == PullTemplate::CUSTOM)
	{
		Point2D location;
		for (unsigned int i = 0; i < root["Subcanes"].getMemberNames().size(); ++i)
			cane->addSubpullTemplate(SubpullTemplate(cane, CIRCLE_SHAPE, location, 1.0));
	}

	for (unsigned int i = 0; i < root["Subcanes"].getMemberNames().size(); ++i)
	{
		string subpullname = root["Subcanes"].getMemberNames()[i];
		unsigned int subpullIndex = root["Subcanes"][subpullname]["Index"].asUInt();

		SubpullTemplate sub = cane->getSubpullTemplate(subpullIndex);
		sub.plan = safeCaneMap(caneMap, root["Subcanes"][subpullname]["Cane pointer"].asUInt());
		sub.diameter = root["Subcanes"][subpullname]["Diameter"].asFloat();
		sub.shape = stringToGeometricShape(root["Subcanes"][subpullname]["Shape"].asString());
		sub.location.x = root["Subcanes"][subpullname]["X"].asFloat();
		sub.location.y = root["Subcanes"][subpullname]["Y"].asFloat();
		cane->setSubpullTemplate(sub, subpullIndex);
	}
}

// write 
void writeCanes(Json::Value& root, vector<PullPlan*>& canes, vector<GlassColor*>& colors)
{
	// generate color map
	map<const GlassColor*, unsigned int> colorMap;
	colorMap[GlobalGlass::color()] = 0;
	for (unsigned int i = 0; i < colors.size(); ++i)
		colorMap[colors[i]] = i+1;

	// generate pull plan map
	map<const PullPlan*, unsigned int> caneMap;
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
void readCanes(Json::Value& canesRoot, map<unsigned int, PullPlan*>& caneMap, 
	map<unsigned int, GlassColor*>& colorMap, vector<PullPlan*>& readCanes)
{
	// add global plan to map for completeness
	caneMap[0] = GlobalGlass::circlePlan();
	caneMap[1] = GlobalGlass::squarePlan();

	// loop over canes
	for (unsigned int i = 0; i < canesRoot.getMemberNames().size(); ++i)
	{
		string canename = canesRoot.getMemberNames()[i];
		unsigned int caneIndex = stringToId(canename);
		PullPlan* cane = readCane(canename, canesRoot, colorMap);
		caneMap[caneIndex] = cane;
	}

	// loop again to fill in subcanes
	for (unsigned int i = 0; i < canesRoot.getMemberNames().size(); ++i)
	{
		unsigned int caneIndex;
		string canename = canesRoot.getMemberNames()[i];
		caneIndex = stringToId(canename);
		readCaneSubcanes(canesRoot[canename], caneMap[caneIndex], caneMap);
	}

	// now sort readCanes
	vector<int> caneIndices;
	for (map<unsigned int, PullPlan*>::iterator cI = caneMap.begin(); cI != caneMap.end(); ++cI)
	{
		// ignore global glass indices 0 and 1
		if (cI->first >= 2)
			caneIndices.push_back(cI->first);
	}
	readCanes.clear();
	for (unsigned int i = 0; i < caneIndices.size(); ++i)
		readCanes.push_back(caneMap[caneIndices[i]]);	
}

/*
following layers of pickup information in the Json representation:

pickup
	pickup template
	casing color
	overlay color
	underlay color
	pickup template parameters
		parameter_1
			index
			name
			value
			lower limit
			upper limit
		parameter_2
		...
	subcanes
		subcane_1
			index
			pull id
			length
			width
			orientation
			shape
			x
			y
			z
		subcane_2
		...
*/
void writePickup(Json::Value& root, PickupPlan* pickup, unsigned int pickupIndex,
	map<PullPlan*, unsigned int>& caneMap, map<GlassColor*, unsigned int>& colorMap)
{
	string pickupname = idAndNameToString(pickupIndex, "Pickup");

	// write singletons: casing color, overlay color, underlay color, pickup template
	root[pickupname]["Pickup template"] = pickupTemplateToString(pickup->templateType()); 
	root[pickupname]["Casing color pointer"] = colorMap[pickup->casingGlassColor()]; 
	root[pickupname]["Overlay color pointer"] = colorMap[pickup->overlayGlassColor()];
	root[pickupname]["Underlay color pointer"] = colorMap[pickup->underlayGlassColor()];
		
	// write pickup template parameters
	root[pickupname]["Pickup template parameters"];
	for (unsigned int i = 0; i < pickup->parameterCount(); ++i)
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
	root[pickupname]["Subcanes"];
	for (unsigned int i = 0; i < pickup->subpickupCount(); ++i)
	{
		string subpullName = idAndNameToString(i, "Subcane");
		root[pickupname]["Subcanes"][subpullName]["Index"] = i;
		SubpickupTemplate t = pickup->getSubpickupTemplate(i);
		root[pickupname]["Subcanes"][subpullName]["Cane pointer"] = caneMap[t.plan];
		root[pickupname]["Subcanes"][subpullName]["Length"] = t.length;
		root[pickupname]["Subcanes"][subpullName]["Width"] = t.width;
		root[pickupname]["Subcanes"][subpullName]["Orientation"] = orientationToString(t.orientation);
		root[pickupname]["Subcanes"][subpullName]["Shape"] = geometricShapeToString(t.shape);
		root[pickupname]["Subcanes"][subpullName]["X"] = t.location.x;
		root[pickupname]["Subcanes"][subpullName]["Y"] = t.location.y;
		root[pickupname]["Subcanes"][subpullName]["Z"] = t.location.z;
	}
}

PickupPlan* readPickup(string pickupname, Json::Value& root,
	map<unsigned int, PullPlan*>& caneMap, map<unsigned int, GlassColor*>& colorMap)
{
	// read singletons: casing color, overlay color, underlay color, pickup template
	PickupPlan* pickup = new PickupPlan(stringToPickupTemplate(root[pickupname]["Pickup template"].asString()));
	pickup->setCasingGlassColor(safeColorMap(colorMap, root[pickupname]["Casing color pointer"].asUInt()));
	pickup->setOverlayGlassColor(safeColorMap(colorMap, root[pickupname]["Overlay color pointer"].asUInt()));
	pickup->setUnderlayGlassColor(safeColorMap(colorMap, root[pickupname]["Underlay color pointer"].asUInt()));

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
	for (unsigned int i = 0; i < root[pickupname]["Subcanes"].getMemberNames().size(); ++i)
	{
		string subpullname = root[pickupname]["Subcanes"].getMemberNames()[i];
		unsigned int subIndex = root[pickupname]["Subcanes"][subpullname]["Index"].asUInt();

		SubpickupTemplate t = pickup->getSubpickupTemplate(subIndex);
		t.plan = safeCaneMap(caneMap, root[pickupname]["Subcanes"][subpullname]["Cane pointer"].asUInt());
		t.length = root[pickupname]["Subcanes"][subpullname]["Length"].asFloat();
		t.width = root[pickupname]["Subcanes"][subpullname]["Width"].asFloat();
		t.orientation = stringToOrientation(root[pickupname]["Subcanes"][subpullname]["Orientation"].asString());
		t.shape = stringToGeometricShape(root[pickupname]["Subcanes"][subpullname]["Shape"].asString());
		t.location.x = root[pickupname]["Subcanes"][subpullname]["X"].asFloat();
		t.location.y = root[pickupname]["Subcanes"][subpullname]["Y"].asFloat();
		t.location.z = root[pickupname]["Subcanes"][subpullname]["Z"].asFloat();
		pickup->setSubpickupTemplate(t, subIndex);
	}

	return pickup;
}

/* 
following layers of piece information in the Json representation:
piece
	piece template
	twist
	basedness
	piece spline parameters
		parameter1
			index
			value 
		parameter2
		...
	pickups (done separately)
*/
void writePiece(Json::Value& root, Piece* piece, unsigned int pieceIndex, map<PullPlan*, unsigned int>& caneMap,
	map<GlassColor*, unsigned int>& colorMap)
{
	string piecename = idAndNameToString(pieceIndex, "Piece");

	// write singletons: piece template, twist
	root[piecename]["Piece template"] = pieceTemplateToString(piece->templateType());
	root[piecename]["Twist"] = piece->twist();

	// write piece template parameters
	root[piecename]["Piece spline control points"];
	Spline spline = piece->spline();
	for (unsigned int i = 0; i < spline.controlPoints().size(); ++i)
	{
		string paramName = idAndNameToString(i, "PieceSplineCtrlPt");
		root[piecename]["Piece spline control points"][paramName]["Index"] = i;
		root[piecename]["Piece spline control points"][paramName]["X"] = spline.controlPoints()[i].x;
		root[piecename]["Piece spline control points"][paramName]["Y"] = spline.controlPoints()[i].y;
	}

	// write pickups (currently only one)
	writePickup(root[piecename]["Pickups"], piece->pickupPlan(), 0, caneMap, colorMap);
}

Piece* readPiece(string piecename, Json::Value& root, map<unsigned int, PullPlan*>& caneMap, 
	map<unsigned int, GlassColor*>& colorMap)
{
	// read singletons: piece template
	Piece* piece = new Piece(stringToPieceTemplate(root[piecename]["Piece template"].asString()));
	piece->setTwist(root[piecename]["Twist"].asFloat());

	// read piece template parameters
	Spline spline = piece->spline();
	for (unsigned int i = spline.controlPoints().size(); 
		i < root[piecename]["Piece spline control points"].getMemberNames().size(); ++i)
		spline.addPoint(Point2D()); // pad spline to be big enough for all the parameters
	for (unsigned int i = 0; i < root[piecename]["Piece spline control points"].getMemberNames().size(); ++i)
	{
		string paramname = root[piecename]["Piece spline control points"].getMemberNames()[i];
		unsigned int paramIndex = root[piecename]["Piece spline control points"][paramname]["Index"].asUInt();

		Point2D paramValue;
		paramValue.x = root[piecename]["Piece spline control points"][paramname]["X"].asFloat();	
		paramValue.y = root[piecename]["Piece spline control points"][paramname]["Y"].asFloat();	
		spline.set(paramIndex, paramValue);
	}
	piece->setSpline(spline);

	// read pickups (currently only one)
	for (unsigned int i = 0; i < root[piecename]["Pickups"].getMemberNames().size(); ++i)
	{
		string pickupname = root[piecename]["Pickups"].getMemberNames()[i];
		piece->setPickupPlan(readPickup(pickupname, root[piecename]["Pickups"], caneMap, colorMap));
	}

	return piece;
}

void writePieces(Json::Value& root, vector<Piece*>& pieces, vector<PullPlan*>& canes, 
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

void readPieces(Json::Value& piecesRoot, map<unsigned int, PullPlan*>& caneMap, map<unsigned int, 
	GlassColor*>& colorMap, vector<Piece*>& readPieces)
{
	// doesn't exist otherwise, so we make it up just to sort them
	map<unsigned int, Piece*> pieceMap;

	// loop over pieces
	for (unsigned int i = 0; i < piecesRoot.getMemberNames().size(); ++i)
	{
		string piecename = piecesRoot.getMemberNames()[i];
		unsigned int pieceIndex = stringToId(piecename);
		Piece* piece = readPiece(piecename, piecesRoot, caneMap, colorMap);
		pieceMap[pieceIndex] = piece;
	}

	// create sorted readColors
	vector<int> pieceIndices;
	for (map<unsigned int, Piece*>::iterator cI = pieceMap.begin(); cI != pieceMap.end(); ++cI)
		pieceIndices.push_back(cI->first);
	readPieces.clear();
	for (unsigned int i = 0; i < pieceIndices.size(); ++i)
		readPieces.push_back(pieceMap[pieceIndices[i]]);	

}

}

