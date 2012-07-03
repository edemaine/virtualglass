
#include <QtGui>
#include "pullplan.h"

class PullPlanRenderPixmap
{
	static const int size = 200;

	public:
		static QPixmap getEditorPixmap(PullPlan* plan);

	private:
		static void paintShape(float x, float y, float size, int shape, QPainter* painter);
		static void setBoundaryPainter(QPainter* painter, bool outermostLevel);
		static void drawSubplan(float x, float y, float drawWidth, float drawHeight,
			PullPlan* plan, int mandatedShape, bool outermostLevel, QPainter* painter);


};

