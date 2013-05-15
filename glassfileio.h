
#ifndef GLASSFILEIO_H
#define GLASSFILEIO_H

#include <map>
#include <vector>
#include <jsoncpp/include/json/json.h>
#include "shape.h"
#include "pulltemplate.h"
#include "pickuptemplate.h"
#include "piecetemplate.h"
#include "subpickuptemplate.h"

class QString;
class QBuffer;
class GlassColor;
class PullPlan;
class PickupPlan;
class Piece;

using std::map;
using std::vector;
using std::string;

// returns whether the read or write succeeded (true) or ran into issues (false) 
bool writeColorFile(QString filename, QString collectionName, vector<GlassColor*>& colors); 
bool readColorFile(QString filename, QString& collectionName, vector<GlassColor*>& colors); 
bool writeGlassFile(QString filename, vector<GlassColor*>& colors, vector<PullPlan*>& canes, vector<Piece*>& pieces);
bool writeGlassFile(QBuffer& buffer, vector<GlassColor*>& colors, vector<PullPlan*>& canes, vector<Piece*>& pieces);
bool readGlassFile(QString filename, vector<GlassColor*>& colors, vector<PullPlan*>& canes, vector<Piece*>& pieces);

namespace GlassFileIOInternal
{
	void writeJsonToFile(QString& filename, Json::Value& root);

	void writeDocumentation(string& filename, Json::Value& root);

	GlassColor* safeColorMap(map<unsigned int, GlassColor*>& colorMap, unsigned int index);
	PullPlan* safeCaneMap(map<unsigned int, PullPlan*>& caneMap, unsigned int index);
		
	void writeBuildInformation(Json::Value& root);
	void readBuildInformation(Json::Value& root, unsigned int& revision, string& date);

	string orientationToString(enum PickupCaneOrientation ori);
	enum PickupCaneOrientation stringToOrientation(string s);

	string geometricShapeToString(enum GeometricShape shape);
	enum GeometricShape stringToGeometricShape(string id);

	string idAndNameToString(unsigned int id, string name);
	unsigned int stringToId(string s);

	string pullTemplateToString(enum PullTemplate::Type type); 
	enum PullTemplate::Type stringToPullTemplate(string s);

	string pickupTemplateToString(enum PickupTemplate::Type type);
	enum PickupTemplate::Type stringToPickupTemplate(string s);

	string pieceTemplateToString(enum PieceTemplate::Type type);
	enum PieceTemplate::Type stringToPieceTemplate(string s);
	
	void writeColor(Json::Value& root, GlassColor* color, unsigned int colorIndex);
	GlassColor* readColor(Json::Value& root, string rootname);

	void writeColors(Json::Value& root, vector<GlassColor*>& colors);
	void readColors(Json::Value& root, map<unsigned int, GlassColor*>& colorMap, vector<GlassColor*>& readColors);

	void writeCane(Json::Value& root, PullPlan* cane, unsigned int caneIndex, map<PullPlan*, unsigned int>& caneMap, 
		map<GlassColor*, unsigned int>& colorMap);
	PullPlan* readCane(string canename, Json::Value& root, map<unsigned int, GlassColor*>& colorMap);

	void writeCanes(Json::Value& root, vector<PullPlan*>& canes, vector<GlassColor*>& colors);
	void readCaneSubcanes(Json::Value& caneRoot, PullPlan* cane, map<unsigned int, PullPlan*>& caneMap);
	void readCanes(Json::Value& root, map<unsigned int, PullPlan*>& caneMap, map<unsigned int, GlassColor*>& colorMap, 
		vector<PullPlan*>& readPlans);

	void writePickup(Json::Value& root, PickupPlan* pickup, unsigned int pickupIndex, 
		map<PullPlan*, unsigned int>& caneMap, map<GlassColor*, unsigned int>& colorMap);
	PickupPlan* readPickup(string pickupname, Json::Value& root, 
		map<unsigned int, PullPlan*>& caneMap, map<unsigned int, GlassColor*>& colorMap);

	void writePiece(Json::Value& root, Piece* piece, unsigned int pieceIndex, map<PullPlan*, unsigned int>& caneMap,
		map<GlassColor*, unsigned int>& colorMap);
	Piece* readPiece(string piecename, Json::Value& root, map<unsigned int, PullPlan*>& caneMap, 
		map<unsigned int, GlassColor*>& colorMap);
		
	void writePieces(Json::Value& root, vector<Piece*>& pieces, vector<PullPlan*>& canes, 
		vector<GlassColor*>& colors);
	void readPieces(Json::Value& root, map<unsigned int, PullPlan*>& caneMap, map<unsigned int, GlassColor*>& colorMap,
		vector<Piece*>& readPieces);
}

#endif



