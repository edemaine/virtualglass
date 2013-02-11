
#ifndef GLASSFILEIO_H
#define GLASSFILEIO_H

#include <json/json.h>
#include <map>
#include <vector>
#include "shape.h"
#include "pulltemplate.h"
#include "pickuptemplate.h"
#include "piecetemplate.h"
#include "subpickuptemplate.h"

class QString;
class GlassColor;
class PullPlan;
class PickupPlan;
class Piece;

using std::map;
using std::vector;
using std::string;

class GlassFileIO
{
	public:
		// returns whether the read or write succeeded (true) or ran into issues (false) 
                static bool read(QString filename, vector<GlassColor*>& colors, vector<PullPlan*>& canes, vector<Piece*>& pieces);
                static bool write(QString filename, vector<GlassColor*>& colors, vector<PullPlan*>& canes, vector<Piece*>& pieces);

	private:
		static void writeDocumentation(Json::Value& root);

		static GlassColor* safeColorMap(map<unsigned int, GlassColor*>& colorMap, unsigned int index);
		static PullPlan* safeCaneMap(map<unsigned int, PullPlan*>& caneMap, unsigned int index);
			
		static void writeBuildInformation(Json::Value& root);
		static void readBuildInformation(Json::Value& root, unsigned int& revision, string& date);

		static string orientationToString(enum PickupCaneOrientation ori);
		static enum PickupCaneOrientation stringToOrientation(string s);
	
		static string geometricShapeToString(enum GeometricShape shape);
		static enum GeometricShape stringToGeometricShape(string id);

		static string idAndNameToString(unsigned int id, string name);
		static unsigned int stringToId(string s);

		static string pullTemplateToString(enum PullTemplate::Type type); 
		static enum PullTemplate::Type stringToPullTemplate(string s);

		static string pickupTemplateToString(enum PickupTemplate::Type type);
		static enum PickupTemplate::Type stringToPickupTemplate(string s);

		static string pieceTemplateToString(enum PieceTemplate::Type type);
		static enum PieceTemplate::Type stringToPieceTemplate(string s);
		
		static void writeColor(Json::Value& root, GlassColor* color, unsigned int colorIndex);
		static GlassColor* readColor(Json::Value& root, string rootname, unsigned int& colorIndex);

		static void writeColors(Json::Value& root, vector<GlassColor*>& colors);
                static void readColors(Json::Value& root, map<unsigned int, GlassColor*>& colorMap, vector<GlassColor*>& readColors);

		static void writeCane(Json::Value& root, PullPlan* cane, unsigned int caneIndex, map<PullPlan*, unsigned int>& caneMap, 
			map<GlassColor*, unsigned int>& colorMap);
		static PullPlan* readCane(string canename, Json::Value& root, unsigned int& caneIndex, 
			map<unsigned int, GlassColor*>& colorMap);

		static void writeCanes(Json::Value& root, vector<PullPlan*>& canes, vector<GlassColor*>& colors);
		static void readCaneSubcanes(Json::Value& caneRoot, PullPlan* cane, map<unsigned int, PullPlan*>& caneMap);
                static void readCanes(Json::Value& root, map<unsigned int, PullPlan*>& caneMap, map<unsigned int, GlassColor*>& colorMap, 
			vector<PullPlan*>& readPlans);

		static void writePickup(Json::Value& root, PickupPlan* pickup, unsigned int pickupIndex, 
			map<PullPlan*, unsigned int>& caneMap, map<GlassColor*, unsigned int>& colorMap);
		static PickupPlan* readPickup(string pickupname, Json::Value& root, 
			map<unsigned int, PullPlan*>& caneMap, map<unsigned int, GlassColor*>& colorMap);

		static void writePiece(Json::Value& root, Piece* piece, unsigned int pieceIndex, map<PullPlan*, unsigned int>& caneMap,
			map<GlassColor*, unsigned int>& colorMap);
		static Piece* readPiece(string piecename, Json::Value& root, map<unsigned int, PullPlan*>& caneMap, 
			map<unsigned int, GlassColor*>& colorMap);
			
		static void writePieces(Json::Value& root, vector<Piece*>& pieces, vector<PullPlan*>& canes, 
			vector<GlassColor*>& colors);
                static void readPieces(Json::Value& root, map<unsigned int, PullPlan*>& caneMap, map<unsigned int, GlassColor*>& colorMap,
			vector<Piece*>& readPieces);
};

#endif



