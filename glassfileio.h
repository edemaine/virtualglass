
#ifndef GLASSFILEIO_H
#define GLASSFILEIO_H

#include <json/json.h>

#include <map>
#include <vector>

class QString;
class GlassColor;
class PullPlan;
class Piece;

using std::map;
using std::vector;
using std::ifstream;

class GlassFileIO
{
	public:
                static void save(QString filename, vector<GlassColor*> colors, vector<PullPlan*> plans, vector<Piece*> pieces);
                //static void load(QString filename, vector<GlassColor*>* colors, vector<PullPlan*>* plans, vector<Piece*>* pieces);

	private:
		static QString writeJson(Json::Value);
                static void buildCaneMap(vector<PullPlan *>*, vector<GlassColor*>*, vector<Piece*>* pieces);
                static void buildCaneTree(PullPlan* , vector<PullPlan*>*, vector<GlassColor*>*);
                static void writeColors(Json::Value*, map<GlassColor*, int>*, vector<GlassColor*>, 
			vector<GlassColor*>* colors);
                static void writeCanes(Json::Value*, map<PullPlan*, int>*, map<GlassColor*, int>, vector<PullPlan*>,
			vector<PullPlan*>* plans);
                static void writePieces(Json::Value*, map<Piece*, int>*, map<PullPlan*, int>*, map<GlassColor*, int>,
			vector<Piece*>* pieces);

#ifdef UNDEF
                static void openColors(Json::Value, std::map<GlassColor*, int>*);
                static void openCanes(Json::Value, std::map<PullPlan*, int>*, std::map<GlassColor*, int>* colorMap);
                static void openPieces(Json::Value, std::map<PullPlan *, int> *caneMap, std::map<GlassColor *, int> *colorMap);
#endif
};



#endif



