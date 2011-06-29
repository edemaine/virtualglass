
#include "model.h"

Model :: Model()
{
	history = new CaneHistory();
	cane = NULL;
	geometryOutOfDate();
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
	int prev_mode = this->mode;
	this->mode = mode;
	if (cane != NULL)
	{
		if (prev_mode != BUNDLE_MODE && this->mode == BUNDLE_MODE)
		{
			history->saveState(cane);
			activeSubcane = -1;
			cane->createBundle();
		}
	}
	
	emit modeChangedSig(mode);
}

void Model :: setActiveSubcane(int subcane)
{
	if (cane == NULL)
		return;
	if (activeSubcane != subcane)
	{
		activeSubcane = subcane;
		geometryOutOfDate();
		emit caneChangedSig();
	}
}

void Model :: setCane(Cane* c)
{
	history->saveState(cane);
	cane = c;
	geometryOutOfDate();
	emit caneChangedSig();
}

void Model :: updateSelectData()
{
	Cane* ancestors[MAX_ANCESTORS];
	int ancestorCount;

	ancestorCount = 0;
	selectGeometry.clear();
	if (cane != NULL) // && cane->type == BUNDLE_CANETYPE) // && mode == BUNDLE_MODE)
		generateMesh(cane, &selectGeometry, ancestors, &ancestorCount,
			LOW_RESOLUTION, NULL, false, true, -1);
	selectDataUpToDate = 1;
}

void Model :: updateLowResData()
{
	Cane* ancestors[MAX_ANCESTORS];
	int ancestorCount;

	ancestorCount = 0;
	lowResGeometry.clear();
	if (cane != NULL && cane->type == BUNDLE_CANETYPE && mode == BUNDLE_MODE && activeSubcane != -1)
	{
		generateMesh(cane, &lowResGeometry, ancestors, &ancestorCount,
			LOW_RESOLUTION, cane->subcanes[activeSubcane], false, false, 0);
	}
	else
		generateMesh(cane, &lowResGeometry, ancestors, &ancestorCount,
			LOW_RESOLUTION, NULL, false, false, 0);
	lowResDataUpToDate = 1;
}

void Model :: updateHighResData()
{
	Cane* ancestors[MAX_ANCESTORS];
	int ancestorCount;

	ancestorCount = 0;
	highResGeometry.clear();
	if (cane != NULL && cane->type == BUNDLE_CANETYPE && mode == BUNDLE_MODE && activeSubcane != -1)
		generateMesh(cane, &highResGeometry, ancestors, &ancestorCount,
			HIGH_RESOLUTION, cane->subcanes[activeSubcane], false, false, 0);
	else
		generateMesh(cane, &highResGeometry, ancestors, &ancestorCount,
			HIGH_RESOLUTION, NULL, false, false, 0);
	highResDataUpToDate = 1;
}

Cane* Model :: getCane()
{
	return cane;
}

Geometry* Model :: getSelectionGeometry()
{
	if (!selectDataUpToDate)
		updateSelectData();
	return &selectGeometry;	
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

void Model :: geometryOutOfDate()
{
	selectDataUpToDate = 0;
	lowResDataUpToDate = 0;
	highResDataUpToDate = 0;
}

void Model :: pullCane(float twistAmount, float stretchAmount)
{
	if (cane == NULL)
		return;
	if (cane->type != PULL_CANETYPE)
		history->saveState(cane);
	cane->pullIntuitive(twistAmount, stretchAmount);
	geometryOutOfDate();
	emit caneChangedSig();
}

void Model :: flattenCane(float rectangle_ratio, float rectangle_theta, float flatness)
{
	if (cane == NULL)
		return;
	if (cane->type != FLATTEN_CANETYPE)
		history->saveState(cane);
	cane->flatten(rectangle_ratio, rectangle_theta, flatness);
	geometryOutOfDate();
	emit caneChangedSig();
}

void Model :: moveCane(float delta_x, float delta_y)
{
	if (cane == NULL || activeSubcane == -1)
		return;
	cane->moveCane(activeSubcane, delta_x, delta_y);
	geometryOutOfDate();
	emit caneChangedSig();
}

void Model :: addCane(Cane* c)
{
	history->saveState(cane);
	if (cane == NULL)
		cane = c->deepCopy();
	else
		cane->add(c->deepCopy());
	geometryOutOfDate();
	emit caneChangedSig();
}

void Model :: undo()
{
	cane = history->getState();
	history->undo();
	geometryOutOfDate();
	emit caneChangedSig();
}

void Model :: saveObjFile(std::string const &filename)
{
	updateHighResData();
	highResGeometry.save_obj_file(filename);

	updateHighResData();
	highResGeometry.save_obj_file(filename);
}


