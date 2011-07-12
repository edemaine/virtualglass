

#ifndef MODEL_H
#define MODEL_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <vector>
#include <string>
#include <QObject>
#include "canehistory.h"
#include "cane.h"
#include "constants.h"
#include "primitives.h"
#include "geometry.h"
#include "mesh.h"

class Model : public QObject
{
	Q_OBJECT

	public:
		Model();
		Cane* getCane();
		void setCane(Cane* c);
		void setActiveSubcane(int subcane);
		int getActiveSubcane();
		Geometry* getSelectionGeometry();
		Geometry* getGeometry(int resolution);
		void pullCane(float twistAmount, float stretchFactor);
		void moveCane(float delta_x, float delta_y);
		void flattenCane(float rectangle_ratio, float rectangle_theta, float flatness);
		void addCane(Cane* c);
		void startMoveMode();
		void saveObjFile(std::string const &filename);
		int getMode();
		void addSnapPoint(Point p);
		int snap_count();
		Point snapPoint(int index);

	signals:
		void caneChanged();
		void textMessage(QString msg);
		void modeChanged(int mode);

	public slots:
		void undo();
		void setMode(int mode);
		void clearCurrentCane();

	private:
		int mode;
		Cane *cane;
		CaneHistory *history;
		Geometry selectGeometry;
		Geometry lowResGeometry;
		Geometry highResGeometry;
		int lowResGeometryFresh;
		int highResGeometryFresh;
		int selectGeometryFresh;
		int activeSubcane;
		Point snapPoints[10];
		int snapCount;

		void geometryOutOfDate();
		void updateSelectGeometry();
		void updateLowResGeometry();
		void updateHighResGeometry();
};

#endif

