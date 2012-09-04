#ifndef PULLPLANRENDERDATA_HPP
#define PULLPLANRENDERDATA_HPP

#include "asyncrenderinternal.h"

class PullPlan;

class PullPlanRenderData : public RenderData
{
	public:
		PullPlanRenderData(PullPlan const *_plan);
		virtual ~PullPlanRenderData();
		virtual Geometry *getGeometry();
	protected:
		PullPlan *plan;
};

class GlassColorRenderData : public RenderData
{
	public:
		GlassColorRenderData(GlassColor const *_gc);
		virtual ~GlassColorRenderData();
		virtual Geometry *getGeometry();
	protected:
		GlassColor* glassColor;

};


#endif //PULLPLANRENDERDATA_HPP
