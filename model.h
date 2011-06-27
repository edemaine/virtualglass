

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
		void setCane(Cane* c);
		Cane* getCane();
		Geometry* getGeometry(int resolution);
		void pullCane(float twistAmount, float stretchFactor);
		void moveCane(float delta_x, float delta_y);
		void flattenCane(float rectangle_ratio, float rectangle_theta, float flatness);
		void addCane(Cane* c);
		void advanceActiveSubcane();
		void startMoveMode();
		void saveObjFile(std::string const &filename);
		void undo();
		void redo();
		bool canRedo();
		int getMode();

	signals:
		void caneChangedSig();
		void modeChangedSig(int mode);
		void textMessageSig(QString msg);

	public slots:
		void modeChangedSlot(int mode);
		void advanceActiveSubcaneCommandSlot();
	
	private:
		int mode;
		Cane *cane;
		CaneHistory *history;
		Geometry lowResGeometry;
		Geometry highResGeometry;
		int lowResDataUpToDate;
		int highResDataUpToDate;
		int activeSubcane;

		void updateLowResData();
		void updateHighResData();
};

#endif

