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

class ColorBarRenderData : public PullPlanRenderData
{
	public:
		ColorBarRenderData(PullPlan const *_plan);
		virtual ~ColorBarRenderData();
		virtual Geometry *getGeometry();
};


#endif //PULLPLANRENDERDATA_HPP
