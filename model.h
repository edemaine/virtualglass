

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
	void setActiveSubcane(int subcane);
	int getActiveSubcane();
	Geometry* getGeometry(int resolution);
	void pullCane(float twistAmount, float stretchFactor);
	void pullActiveCane(float twistAmount, float stretchFactor);
	void moveCane(float delta_x, float delta_y);
	void moveCaneTo(float delta_x, float delta_y);
	void moveCane(float delta_z);
	void flattenCane(float rectangle_ratio, float rectangle_theta, float flatness);
	void flattenActiveCane(float rectangle_ratio, float rectangle_theta, float flatness);
	void deleteActiveCane();
	void addCane(Cane* c);
	void startMoveMode();
	void saveObjFile(std::string const &filename);
	void saveRawFile(std::string const &filename);
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
	void clearActiveSnap();

signals:
	void caneChanged();
	void textMessage(QString msg);
	void modeChanged(int mode);

public slots:
	void undo();
	void setMode(int mode);
	void clearCurrentCane();
	void setCane(Cane* c);

private:
	int mode;
	Cane *cane;
	CaneHistory *history;
	Geometry lowResGeometry;
	Geometry highResGeometry;
	int lowResGeometryFresh;
	int highResGeometryFresh;
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

	void geometryOutOfDate();
	void updateLowResGeometry();
	void updateHighResGeometry();
	void resetAuxiliaries();
};

#endif

