

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
#include "canefile.h"

using std::vector;	

class Model : public QObject
{
	Q_OBJECT

public:
	Model();
	Cane* getCane();
	CaneHistory* getHistory();
	void setActiveSubcane(int subcane);
	int getActiveSubcane();
	int addNewDefaultCane();
	Color* getSubcaneColor(int subcane);
	CaneShape* getSubcaneShape(int subcane);
	bool subcaneHasColorAndShape(int subcane);
        void setSubcaneLocation(int subcane, float x, float y, float z);
	void setSubcaneColor(int subcane, Color* c);
	void setSubcaneShape(int subcane, CaneShape* shape);
        void setSubcaneAlpha(int subcane, int newAlpha);
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
	void setGeometryHeight(float height);

	int getProjection();
	int getMode();

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
	float geometryHeight;
	int mode;
	int projection;
	Cane *cane;
	Cane *defaultCane;
	CaneHistory *history;
	Geometry cachedGeometry;
	Geometry geometry;
	int activeSubcane;

	void computeHighResGeometry(Geometry* highResGeometry);
	void cacheGeometry();
	void revertToCachedGeometry();
};

#endif

