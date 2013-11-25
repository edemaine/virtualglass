
#ifndef GLASSFILEIO_H
#define GLASSFILEIO_H

#include <map>
#include <vector>
#include <jsoncpp/include/json/json.h>

#include "shape.h"
#include "canetemplate.h"
#include "pickuptemplate.h"
#include "piecetemplate.h"
#include "subpickuptemplate.h"

class QString;
class QBuffer;
class GlassColor;
class Cane;
class Pickup;
class Piece;

using std::map;
using std::vector;
using std::string;

// returns whether the read or write succeeded (true) or ran into issues (false) 
bool writeColorFile(QString filename, QString collectionName, vector<GlassColor*>& colors); 
bool readColorFile(QString filename, QString& collectionName, vector<GlassColor*>& colors); 
bool writeGlassFile(QString filename, vector<GlassColor*>& colors, vector<Cane*>& canes, vector<Piece*>& pieces);
bool writeGlassFile(QBuffer& buffer, vector<GlassColor*>& colors, vector<Cane*>& canes, vector<Piece*>& pieces);
bool readGlassFile(QString filename, vector<GlassColor*>& colors, vector<Cane*>& canes, vector<Piece*>& pieces);

namespace GlassFileIOInternal
{
	void writeJsonToFile(QString& filename, Json::Value& root);
	void writeJsonToBuffer(QBuffer& buffer, Json::Value& root);

	void writeDocumentation(string& filename, Json::Value& root);

	GlassColor* safeColorMap(map<unsigned int, GlassColor*>& colorMap, unsigned int index);
	Cane* safeCaneMap(map<unsigned int, Cane*>& caneMap, unsigned int index);
		
	void writeBuildInformation(Json::Value& root);
	void readBuildInformation(Json::Value& root, unsigned int& revision, string& date);

	string orientationToString(enum PickupCaneOrientation ori);
	enum PickupCaneOrientation stringToOrientation(string s);

	string geometricShapeToString(enum GeometricShape shape);
	enum GeometricShape stringToGeometricShape(string id);

	string idAndNameToString(unsigned int id, string name);
	unsigned int stringToId(string s);

	string pullTemplateToString(enum CaneTemplate::Type type); 
	enum CaneTemplate::Type stringToCaneTemplate(string s);

	string pickupTemplateToString(enum PickupTemplate::Type type);
	enum PickupTemplate::Type stringToPickupTemplate(string s);

	string pieceTemplateToString(enum PieceTemplate::Type type);
	enum PieceTemplate::Type stringToPieceTemplate(string s);
	
	void writeColor(Json::Value& root, GlassColor* color, unsigned int colorIndex);
	GlassColor* readColor(Json::Value& root, string rootname);

	void writeColors(Json::Value& root, vector<GlassColor*>& colors);
	void readColors(Json::Value& root, map<unsigned int, GlassColor*>& colorMap, vector<GlassColor*>& readColors);

	void writeCane(Json::Value& root, Cane* cane, unsigned int caneIndex, map<const Cane*, unsigned int>& caneMap, 
		map<const GlassColor*, unsigned int>& colorMap);
	Cane* readCane(string canename, Json::Value& root, map<unsigned int, GlassColor*>& colorMap);

	void writeCanes(Json::Value& root, vector<Cane*>& canes, vector<GlassColor*>& colors);
	void readCaneSubcanes(Json::Value& caneRoot, Cane* cane, map<unsigned int, Cane*>& caneMap);
	void readCanes(Json::Value& root, map<unsigned int, Cane*>& caneMap, map<unsigned int, GlassColor*>& colorMap, 
		vector<Cane*>& readPlans);

	void writePiece(Json::Value& root, Piece* piece, unsigned int pieceIndex, map<Cane*, unsigned int>& caneMap,
		map<GlassColor*, unsigned int>& colorMap);
	Piece* readPiece(string piecename, Json::Value& root, map<unsigned int, Cane*>& caneMap, 
		map<unsigned int, GlassColor*>& colorMap);
		
	void writePieces(Json::Value& root, vector<Piece*>& pieces, vector<Cane*>& canes, 
		vector<GlassColor*>& colors);
	void readPieces(Json::Value& root, map<unsigned int, Cane*>& caneMap, map<unsigned int, GlassColor*>& colorMap,
		vector<Piece*>& readPieces);
}

#endif



