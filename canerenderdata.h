
#ifndef CANERENDERDATA_HPP
#define CANERENDERDATA_HPP

#include "asyncrenderinternal.h"

class GlassColor;
class Cane;
class Geometry;

class CaneRenderData : public RenderData
{
	public:
		CaneRenderData(Cane const *_cane);
		virtual ~CaneRenderData();
		virtual Geometry *getGeometry();
	protected:
		Cane *cane;
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
