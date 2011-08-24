

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
	bool subcaneHasColorAndShape(int subcane);
	void setSubcaneColor(int subcane, float r, float g, float b, float a);
	void setSubcaneShape(int subcane, int shape);
	Cane* getSubcane(int subcane);
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

	void toggle2D();

signals:
	void caneChanged();
	void textMessage(QString msg);
	void modeChanged(int mode);
	void projectionChanged();

public slots:
	void setCaneToNull();	
	void undo();
	void redo();
	void setMode(int mode);
	void setMode(int mode, bool viewRecipe, Cane* c);
	void insertMode(Cane* c, int mode);
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

	bool show2D;

	void computeHighResGeometry(Geometry* highResGeometry);
	void cacheGeometry();
	void revertToCachedGeometry();
};

#endif

