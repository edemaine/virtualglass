
#ifndef CANERENDERDATA_HPP
#define CANERENDERDATA_HPP

#include "asyncrenderinternal.h"

class GlassColor;
class PullPlan;
class Geometry;

class CaneRenderData : public RenderData
{
	public:
		CaneRenderData(PullPlan const *_plan);
		virtual ~CaneRenderData();
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
