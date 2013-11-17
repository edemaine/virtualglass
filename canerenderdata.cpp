
#include <time.h>

#include "glasscolor.h"
#include "pullplan.h"
#include "geometry.h"
#include "mesh.h"
#include "canerenderdata.h"
#include "globalgraphicssetting.h"

#include <tr1/unordered_map>
#include <tr1/unordered_set>
using std::tr1::unordered_map;
using std::tr1::unordered_set;

using std::pair;
using std::make_pair;
using std::vector;

CaneRenderData::CaneRenderData(PullPlan const *_plan) : plan(deep_copy(_plan)) 
{
}

CaneRenderData::~CaneRenderData() 
{
	deep_delete(plan);
	plan = NULL;
}

Geometry *CaneRenderData::getGeometry() 
{
	Geometry *geom = new Geometry();
	generateMesh(plan, geom, GlobalGraphicsSetting::HIGH);
	return geom;
}

GlassColorRenderData::GlassColorRenderData(GlassColor const *_glassColor) : glassColor(deep_copy(_glassColor)) 
{
}

GlassColorRenderData::~GlassColorRenderData() 
{
}

Geometry *GlassColorRenderData::getGeometry() 
{
	Geometry *geom = new Geometry();
	generateMesh(glassColor, geom, GlobalGraphicsSetting::HIGH);
	return geom;

}
