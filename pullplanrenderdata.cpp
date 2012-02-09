
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

PullPlanRenderData::PullPlanRenderData(PullPlan const *_plan) : plan(NULL) {
	//hack-y deep copy of plan:
	unordered_map< const PullPlan *, PullPlan * > copies;
	plan = _plan->copy();
	copies.insert(make_pair(_plan, plan));

	vector< PullPlan * > to_update;
	to_update.push_back(plan);
	//update sub-templates to point to copies as well:
	while (!to_update.empty()) {
		PullPlan *t = to_update.back();
		to_update.pop_back();
		for (vector< SubpullTemplate >::iterator s = t->subs.begin(); s != t->subs.end(); ++s) {
			unordered_map< const PullPlan *, PullPlan * >::iterator f = copies.find(s->plan);
			if (f == copies.end()) {
				f = copies.insert(make_pair(s->plan, s->plan->copy())).first;
				to_update.push_back(f->second);
			}
			s->plan = f->second;
		}
	}
}

PullPlanRenderData::~PullPlanRenderData() {
	//Because pull plans don't delete their children (which is right):
	unordered_set< PullPlan * > marked;
	vector< PullPlan * > to_delete;
	to_delete.push_back(plan);
	while (!to_delete.empty()) {
		PullPlan *t = to_delete.back();
		to_delete.pop_back();
		for (vector< SubpullTemplate >::iterator s = t->subs.begin(); s != t->subs.end(); ++s) {
			if (marked.insert(s->plan).second) {
				to_delete.push_back(s->plan);
			}
			s->plan = NULL;
		}
		delete t;
	}
	plan = NULL;
}

Geometry *PullPlanRenderData::getGeometry() {
	Geometry *geom = new Geometry();
	Mesher mesher;
	mesher.generatePullMesh(plan, geom);
	return geom;
}

ColorBarRenderData::ColorBarRenderData(PullPlan const *_plan) : PullPlanRenderData(_plan) {
}

ColorBarRenderData::~ColorBarRenderData() {
}

Geometry *ColorBarRenderData::getGeometry() {
	Geometry *geom = new Geometry();
	Mesher mesher;
	mesher.generateColorMesh(plan, geom);
	return geom;

}
