
#ifndef GLASSFILEREADER_H
#define GLASSFILEREADER_H

#include <json/json.h>

#include <map>
#include <vector>

class QString;
class GlassColor;
class PullPlan;
class Piece;

using std::map;
using std::vector;

class GlassFileReader
{
	public:
		// returns whether load occured (true) or ran into issues (false) 
                static bool load(QString filename, vector<GlassColor*>* colors, vector<PullPlan*>* plans, vector<Piece*>* pieces);

	private:
                static void openColors(Json::Value, map<GlassColor*, int>*, vector<GlassColor*>* readColors);
                static void openCanes(Json::Value, map<PullPlan*, int>*, map<GlassColor*, int>* colorMap, 
			vector<PullPlan*>* readPlans);
                static void openPieces(Json::Value, map<PullPlan *, int> *caneMap, map<GlassColor *, int> *colorMap, 
			vector<Piece*>* readPieces);
};

#endif



