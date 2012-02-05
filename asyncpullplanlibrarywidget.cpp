#include "asyncpullplanlibrarywidget.h"
#include "mesh.h"
#include "qgraphicshighlighteffect.h"

#include <tr1/unordered_map>
#include <tr1/unordered_set>

using std::tr1::unordered_map;
using std::tr1::unordered_set;
using std::make_pair;
using std::vector;

class PullPlanRenderData : public RenderData
{
	public:
		PullPlanRenderData(PullPlan const *_plan) : plan(NULL) {
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
		virtual ~PullPlanRenderData() {
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
		virtual Geometry *getGeometry() {
			Geometry *geom = new Geometry();
			Mesher mesher;
			mesher.generatePullMesh(plan, geom);
			return geom;
		}
	private:
		PullPlan *plan;
};

AsyncPullPlanLibraryWidget :: AsyncPullPlanLibraryWidget(PullPlan *_pullPlan, QWidget *parent) : AsyncRenderWidget(parent), pullPlan(_pullPlan)
{
	//setBackgroundRole(QPallette::Base);
	setFixedSize(100, 100);
	//setScaledContents(true);
	setMouseTracking(true);
	setAttribute(Qt::WA_LayoutUsesWidgetRect);

	setGraphicsEffect(new QGraphicsHighlightEffect());
	connect(graphicsEffect(), SIGNAL(enabledChanged(bool)), graphicsEffect(), SLOT(setStyleSheet(bool)));
	connect(graphicsEffect(), SIGNAL(styleSheetString(QString)), this, SLOT(setStyleSheet(QString)));

	QPixmap pixmap(100, 100);
	pixmap.fill(Qt::white);
	updatePixmaps(pixmap);
}

PullPlan* AsyncPullPlanLibraryWidget :: getPullPlan()
{
	return pullPlan;
}

void AsyncPullPlanLibraryWidget :: updatePixmaps(QPixmap const &_editorPixmap)
{
	editorPixmap = _editorPixmap;

	//queue up an async update:
	Camera camera;
	camera.eye = make_vector(0.0f, 11.5f, 5.0f);
	camera.lookAt = make_vector(0.0f, 0.0f, 5.0f);
	camera.up = make_vector(0.0f, 0.0f, 1.0f);
	camera.isPerspective = false;
	camera.size = make_vector(300U, 300U);
	setScene(camera, new PullPlanRenderData(pullPlan));
}

const QPixmap* AsyncPullPlanLibraryWidget :: getEditorPixmap()
{
	return &(this->editorPixmap);
}

