
#include "model.h"

Model :: Model(Cane* c)
{
	lowResDataUpToDate = 0;
	highResDataUpToDate = 0;
	setCane(c);
}

void Model :: setCane(Cane* c)
{
	cane = c;
	updateLowResData();
	updateHighResData();
}

void Model :: updateLowResData()
{
	Cane* ancestors[MAX_ANCESTORS];
	int ancestorCount;

	ancestorCount = 0;
	lowResGeometry.clear();
	if (cane != NULL && cane->type == BUNDLE_CANETYPE)
		generateMesh(cane, &lowResGeometry, ancestors, &ancestorCount, 
			LOW_RESOLUTION, cane->subcanes[activeSubcane], false);
	else
		generateMesh(cane, &lowResGeometry, ancestors, &ancestorCount, 
			LOW_RESOLUTION, NULL, false);
	lowResDataUpToDate = 1;
}

void Model :: updateHighResData()
{
	Cane* ancestors[MAX_ANCESTORS];
	int ancestorCount;

	ancestorCount = 0;
	highResGeometry.clear();
	if (cane != NULL && cane->type == BUNDLE_CANETYPE)
		generateMesh(cane, &highResGeometry, ancestors, &ancestorCount, 
			HIGH_RESOLUTION, cane->subcanes[activeSubcane], false);
	else
		generateMesh(cane, &highResGeometry, ancestors, &ancestorCount, 
			HIGH_RESOLUTION, NULL, false);
	highResDataUpToDate = 1;
}

Cane* Model :: getCane()
{
	return cane;
}

Geometry* Model :: getGeometry(int resolution)
{
	if (resolution == LOW_RESOLUTION)
	{
		if (!lowResDataUpToDate)
			updateLowResData();
		return &lowResGeometry;
	}
	else
	{
		if (!highResDataUpToDate)
			updateHighResData();
		return &highResGeometry;
	}
}

void Model :: twistAndStretchCane(float twistAmount, float stretchAmount)
{
	if (cane == NULL)
		return;
	cane->twistAndStretch(twistAmount, stretchAmount);
	lowResDataUpToDate = 0;
	highResDataUpToDate = 0;
}

void Model :: flattenCane(float rectangle_ratio, float rectangle_theta, float flatness)
{
	if (cane == NULL)
		return;
	cane->flatten(rectangle_ratio, rectangle_theta, flatness);
	lowResDataUpToDate = 0;
	highResDataUpToDate = 0;
}

void Model :: startMoveMode()
{
	activeSubcane = 0;
	if (cane != NULL)
		cane->createBundle();
}

void Model :: moveCane(float delta_x, float delta_y)
{
	if (cane == NULL)
		return;
	cane->moveCane(activeSubcane, delta_x, delta_y);
	lowResDataUpToDate = 0;
	highResDataUpToDate = 0;
}

void Model :: addCane(Cane* c)
{
	if (cane == NULL)
		cane = c->deepCopy();
	else
		cane->add(c->deepCopy(), &activeSubcane);
	lowResDataUpToDate = highResDataUpToDate = 0;
}

/*
Model::advanceActiveSubcane() is used when in bundle
mode (i.e. the root node of the cane being operated on
is a bundle) to change the subcane currently under user
control. It will (i.e. in the future)
also change the colors of the subcanes
to illuminate the newly selected subcane.
*/
void Model :: advanceActiveSubcane()
{
	if (cane->type != BUNDLE_CANETYPE)
		return;
	activeSubcane += 1;
	activeSubcane %= cane->subcaneCount;
	lowResDataUpToDate = highResDataUpToDate = 0;
	updateHighResData();
	updateLowResData();
}


void Model :: saveObjFile(std::string const &filename)
{
	updateHighResData();
	highResGeometry.save_obj_file(filename);

	updateHighResData();
	highResGeometry.save_obj_file(filename);
}


