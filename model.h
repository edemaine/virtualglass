

#ifndef MODEL_H
#define MODEL_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <vector>
#include <string>
#include "cane.h"
#include "constants.h"
#include "primitives.h"
#include "geometry.h"
#include "mesh.h"

class Model
{

	public:
		Model(Cane* c);
		void setCane(Cane* c);
		Cane* getCane();
		Geometry* getGeometry(int resolution);
		void twistAndStretchCane(float twistAmount, float stretchFactor);
		void moveCane(float delta_x, float delta_y);
		void flattenCane(float rectangle_ratio, float rectangle_theta, float flatness);
		void addCane(Cane* c);
		void advanceActiveSubcane();
		void startMoveMode();
		void saveObjFile(std::string const &filename);

	private:
		Cane *cane;
		Geometry lowResGeometry;
		Geometry highResGeometry;
		int lowResDataUpToDate;
		int highResDataUpToDate;
		int activeSubcane;

		void updateLowResData();
		void updateHighResData();
};

#endif

