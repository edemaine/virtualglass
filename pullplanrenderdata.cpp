
#include "pullplan.h"
#include "pullplanrenderdata.h"
#include "geometry.h"
#include "mesh.h"

#ifdef UNORDERED_MAP_WORKAROUND
#include <tr1/unordered_map>
#include <tr1/unordered_set>
using std::tr1::unordered_map;
using std::tr1::unordered_set;
#else
#include <unordered_map>
#include <unordered_set>
using std::unordered_map;
using std::unordered_set;
#endif

using std::pair;
using std::make_pair;
using std::vector;

PullPlanRenderData::PullPlanRenderData(PullPlan const *_plan) : plan(deep_copy(_plan)) {
}

PullPlanRenderData::~PullPlanRenderData() {
	deep_delete(plan);
	plan = NULL;
}

Geometry *PullPlanRenderData::getGeometry() {
	Geometry *geom = new Geometry();
	Mesher mesher;
	mesher.generatePullMesh(plan, geom);
	return geom;
}

GlassColorRenderData::GlassColorRenderData(GlassColor const *_glassColor) : glassColor(deep_copy(_glassColor)) {
}

GlassColorRenderData::~GlassColorRenderData() {
}

Geometry *GlassColorRenderData::getGeometry() {
	Geometry *geom = new Geometry();
	Mesher mesher;
	mesher.generateColorMesh(glassColor, geom);
	return geom;

}
