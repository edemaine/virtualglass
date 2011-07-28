

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
	Geometry* getGeometry(int resolution);
	void pullCane(float twistAmount, float stretchFactor);
	void pullActiveCane(float twistAmount, float stretchFactor);
	void moveCane(float delta_x, float delta_y);
	void moveCane(float delta_z);
	void flattenCane(float rectangle_ratio, float rectangle_theta, float flatness);
	void flattenActiveCane(float rectangle_ratio, float rectangle_theta, float flatness);
	void deleteActiveCane();
	void addCane(Cane* c);
	void startMoveMode();
	void saveObjFile(std::string const &filename);
	int getMode();
	int addSnapPoint(Point p);
	void modifySnapPoint(float radii,int index);
	void modifySnapPoint(float radii);
	Point finalizeSnapPoint(int index);
	Point finalizeSnapPoint();
	int snap_count();
	Point snapPoint(int index);
	float snapRadius(int index);



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
	Geometry lowResGeometry;
	Geometry highResGeometry;
	int lowResGeometryFresh;
	int highResGeometryFresh;
	int activeSubcane;
	Point snapPoints[MAX_SNAP];
	float snapRadii[MAX_SNAP];
	int snapCount;
	int activeSnapMode; // Which snap is being activated, or 0 if not;
	int activeSnapIndex;

	void geometryOutOfDate();
	void updateLowResGeometry();
	void updateHighResGeometry();
};

#endif

