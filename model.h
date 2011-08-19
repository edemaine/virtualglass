

#ifndef MODEL_H
#define MODEL_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <vector>
#include <algorithm>
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
	CaneHistory* getHistory();
	void setActiveSubcane(int subcane);
	int getActiveSubcane();
	Geometry* getGeometry();
	void pullCane(float twistAmount, float stretchFactor);
	void pullActiveCane(float twistAmount, float stretchFactor);
	void flattenCane(float rectangle_ratio, float rectangle_theta, float flatness);
	void flattenActiveCane(float rectangle_ratio, float rectangle_theta, float flatness);
	void moveCane(float delta_x, float delta_y, float delta_z);
	bool deleteActiveCane();
	void addCane(Cane* c);
	void addCane(Cane* c, Cane* d);
	void slowGeometryUpdate();
	void saveObjFile(std::string const &filename);
	void saveRawFile(std::string const &filename);
	void setProjection(int p);

	int getProjection();
	int getMode();

	int addSnapPoint(int snapMode,Point p);
	void modifySnapPoint(float radii,int index);
	void modifySnapPoint(float radii);
	void modifySnapPoint(Point p);
	void modifySnapPoint(Point p,int index);
	Point finalizeSnapPoint(int index);
	Point finalizeSnapPoint();

	int snapPointCount(int snapMode);
	Point snapPoint(int snapMode, int index);
	Point snapPoint2(int snapMode, int index);
	Point snapPoint2(int index);
	float snapPointRadius(int snapMode, int index);

	int getActiveSnapMode();
	int getActiveSnapIndex();
	void clearActiveSnap(bool holdSnap);

	void deleteSnapPoint(Point p);

	void toggle2D();

	float snapLineParam;
	float snapCircleParam;

signals:
	void caneChanged();
	void textMessage(QString msg);
	void modeChanged(int mode);
	void projectionChanged();

public slots:
	void undo();
	void redo();
	void setMode(int mode);
	void setMode(int mode, bool viewRecipe, Cane* c);
	void insertMode(Cane* c, int mode);
	void clearCurrentCane();
	void setCane(Cane* c);
	void exactChange();
	void setOrthographicProjection();
	void setPerspectiveProjection();

private:
	int mode;
	int projection;
	Cane *cane;
		CaneHistory *history;
	Geometry cachedGeometry;
	Geometry geometry;
	int geometryFresh;
	int activeSubcane;

	Point snapPoints[MAX_SNAP];
	float snapPointRadii[MAX_SNAP];
	int snapPointsCount;

	Point snapSegments1[MAX_SNAP];
	Point snapSegments2[MAX_SNAP];
	int snapLinesCount;

	Point snapCircles[MAX_SNAP];
	float snapCircleRadii[MAX_SNAP];
	int snapCirclesCount;

	int activeSnapMode; // Which snap is being activated, or 0 if not;
	int activeSnapIndex;
	int activeSnapPlacementMode;

	Point snapHoldPoint;

	bool show2D;

	void computeHighResGeometry(Geometry* highResGeometry);
	void cacheGeometry();
	void revertToCachedGeometry();
	void resetAuxiliaries();
};

#endif

