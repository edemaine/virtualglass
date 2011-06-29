

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
		Geometry* getSelectionGeometry();
		Geometry* getGeometry(int resolution);
		void pullCane(float twistAmount, float stretchFactor);
		void moveCane(float delta_x, float delta_y);
		void flattenCane(float rectangle_ratio, float rectangle_theta, float flatness);
		void addCane(Cane* c);
		void startMoveMode();
		void saveObjFile(std::string const &filename);
		int getMode();

	signals:
		void caneChangedSig();
		void textMessageSig(QString msg);
		void modeChangedSig(int mode);

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
		int lowResDataUpToDate;
		int highResDataUpToDate;
		int selectDataUpToDate;
		int activeSubcane;

		void geometryOutOfDate();
		void updateSelectData();
		void updateLowResData();
		void updateHighResData();
};

#endif

