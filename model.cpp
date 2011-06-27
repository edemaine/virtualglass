
#include "model.h"

Model :: Model()
{
	history = new CaneHistory();
	cane = NULL;	
	lowResDataUpToDate = 0;
	highResDataUpToDate = 0;
}

void Model :: clearCurrentCane()
{
	setCane(NULL);
}

int Model :: getMode()
{
	return mode;
}

void Model :: setMode(int mode)
{
	this->mode = mode;
	emit modeChangedSig(mode);
}

void Model :: setCane(Cane* c)
{
	history->saveState(cane);
	cane = c;
	lowResDataUpToDate = highResDataUpToDate = 0; 
	emit caneChangedSig();
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

void Model :: pullCane(float twistAmount, float stretchAmount)
{
	if (cane == NULL)
		return;
	if (cane->type != PULL_CANETYPE)
		history->saveState(cane);
	cane->pull(twistAmount, stretchAmount);
	lowResDataUpToDate = 0;
	highResDataUpToDate = 0;
	emit caneChangedSig();
}

void Model :: flattenCane(float rectangle_ratio, float rectangle_theta, float flatness)
{
	if (cane == NULL)
		return;
	if (cane->type != FLATTEN_CANETYPE)
		history->saveState(cane);
	cane->flatten(rectangle_ratio, rectangle_theta, flatness);
	lowResDataUpToDate = 0;
	highResDataUpToDate = 0;
	emit caneChangedSig();
}

void Model :: startMoveMode()
{
	if (cane == NULL)
		return;
	activeSubcane = 0;
	if (cane->type != BUNDLE_CANETYPE)
	{
		history->saveState(cane);
		cane->createBundle();
	}
}

void Model :: moveCane(float delta_x, float delta_y)
{
	if (cane == NULL)
		return;
	cane->moveCane(activeSubcane, delta_x, delta_y);
	lowResDataUpToDate = 0;
	highResDataUpToDate = 0;
	emit caneChangedSig();
}

void Model :: addCane(Cane* c)
{
	history->saveState(cane);	
	if (cane == NULL)
		cane = c->deepCopy();
	else
		cane->add(c->deepCopy(), &activeSubcane);
	lowResDataUpToDate = highResDataUpToDate = 0;
	emit caneChangedSig();
}

/*
Model::advanceActiveSubcane() is used when in bundle
mode (i.e. the root node of the cane being operated on
is a bundle) to change the subcane currently under user
control. It will (i.e. in the future)
also change the colors of the subcanes
to illuminate the newly selected subcane.
*/
void Model :: advanceActiveSubcaneSlot()
{
	if (cane->type != BUNDLE_CANETYPE)
		return;
	activeSubcane += 1;
	activeSubcane %= cane->subcaneCount;
	lowResDataUpToDate = highResDataUpToDate = 0;
	emit caneChangedSig();
}

void Model :: undo()
{
        cane = history->getState();
	history->undo();
        lowResDataUpToDate = highResDataUpToDate = 0;
        emit caneChangedSig();
}

void Model :: saveObjFile(std::string const &filename)
{
	updateHighResData();
	highResGeometry.save_obj_file(filename);

	updateHighResData();
	highResGeometry.save_obj_file(filename);
}


