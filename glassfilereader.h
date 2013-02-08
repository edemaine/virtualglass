
#ifndef GLASSFILEIO_H
#define GLASSFILEIO_H

#include <json/json.h>
#include <map>
#include <vector>
#include "shape.h"
#include "pulltemplate.h"

class QString;
class GlassColor;
class PullPlan;
class Piece;

using std::map;
using std::vector;
using std::string;

class GlassFileIO
{
	public:
		// returns whether load occured (true) or ran into issues (false) 
                static bool read(QString filename, vector<GlassColor*>& colors, vector<PullPlan*>& plans, vector<Piece*>& pieces);
                static bool write(QString filename, vector<GlassColor*>& colors, vector<PullPlan*>& plans, vector<Piece*>& pieces);

	private:
		static QString writeJson(Json::Value& root);

		static void writeBuildInformation(Json::Value& root);
		static void readBuildInformation(Json::Value& root, unsigned int* revision, string* date);

		static string geometricShapeToString(enum GeometricShape shape);
		static enum GeometricShape stringToGeometricShape(string id);

		static string idAndNameToString(unsigned int id, string name);
		static int stringToId(string s);
		static string stringToName(string s);

		static string pullTemplateToString(enum PullTemplate::Type type); 
		static enum PullTemplate::Type stringToPullTemplate(string s);

		static void writeColor(Json::Value& root, GlassColor* color, unsigned int colorIndex);
		static GlassColor* readColor(Json::Value& root, string rootname, unsigned int& colorIndex);

		static void writeColors(Json::Value& root, vector<GlassColor*>& colors);
                static void readColors(Json::Value& root, map<unsigned int, GlassColor*>& colorMap, vector<GlassColor*>& readColors);

		static void writeCane(Json::Value& root, PullPlan* plan, unsigned int planIndex, map<PullPlan*, unsigned int>& caneMap, 
			map<GlassColor*, unsigned int>& colorMap);
		static PullPlan* readCane(string canename, Json::Value& root, unsigned int& planIndex, 
			map<unsigned int, GlassColor*>& colorMap);

		static void writeCanes(Json::Value& root, vector<PullPlan*>& plans, vector<GlassColor*>& colors);
		static void readCaneSubpulls(Json::Value& caneRoot, PullPlan* plan, map<unsigned int, PullPlan*>& caneMap);
                static void readCanes(Json::Value& root, map<unsigned int, PullPlan*>& caneMap, map<unsigned int, GlassColor*>& colorMap, 
			vector<PullPlan*>& readPlans);

#ifdef UNDEF
		static Json::Value writePieces(vector<Pieces*> pieces, vector<PullPlan*>* plans, vector<GlassColor*>* colors);
                static void readPieces(Json::Value, map<PullPlan *, int> *caneMap, map<GlassColor *, int> *colorMap, 
			vector<Piece*>* readPieces);
#endif
};

#endif



