
#include "model.h"

Model :: Model()
{
	mode = 0;
	cane = NULL;
	history = new CaneHistory();
	lowResGeometryFresh = 0;
	highResGeometryFresh = 0;
	activeSubcane = -1;
	for (int i = 0; i < MAX_SNAP; ++i)
	{
		snapPoints[i].x=snapPoints[i].y=snapPoints[i].z=0.0;
		snapRadii[i]=0.0;
	}
	snapCount=0;
	geometryOutOfDate();
}

void Model :: clearCurrentCane()
{
	mode = 0;
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

	emit modeChanged(mode);
}

void Model :: setActiveSubcane(int subcane)
{
	if (cane == NULL)
		return;
	//if (subcane<0 || subcane >= cane->subcaneCount)
	//	return;
	if (activeSubcane != subcane)
	{
		activeSubcane = subcane;
		if (activeSubcane < 0 || activeSubcane >= cane->subcaneCount) {
			activeSubcane = -1;
		}
		geometryOutOfDate();
		emit caneChanged();
	}
}

int Model :: getActiveSubcane()
{
	return activeSubcane;
}

void Model :: setCane(Cane* c)
{
	history->saveState(cane);
	cane = c;
	geometryOutOfDate();
	emit caneChanged();
}

void Model :: updateLowResGeometry()
{
	Cane* ancestors[MAX_ANCESTORS];
	int ancestorCount;

	ancestorCount = 0;
	lowResGeometry.clear();
	if (cane != NULL)
		generateMesh(cane, &lowResGeometry, ancestors, &ancestorCount, LOW_RESOLUTION);
	lowResGeometryFresh = 1;
}

void Model :: updateHighResGeometry()
{
	Cane* ancestors[MAX_ANCESTORS];
	int ancestorCount;

	ancestorCount = 0;
	highResGeometry.clear();
	if (cane != NULL)
		generateMesh(cane, &highResGeometry, ancestors, &ancestorCount, HIGH_RESOLUTION);
	highResGeometryFresh = 1;
}

Cane* Model :: getCane()
{
	return cane;
}

Geometry* Model :: getGeometry(int resolution)
{
	if (resolution == LOW_RESOLUTION)
	{
		if (!lowResGeometryFresh)
			updateLowResGeometry();
		return &lowResGeometry;
	}
	else
	{
		if (!highResGeometryFresh)
			updateHighResGeometry();
		return &highResGeometry;
	}
}

void Model :: geometryOutOfDate()
{
	lowResGeometryFresh = 0;
	highResGeometryFresh = 0;
}

void Model :: pullCane(float twistAmount, float stretchAmount)
{
	if (cane == NULL)
		return;
	if (cane->type != PULL_CANETYPE)
		history->saveState(cane);
	cane->pullIntuitive(twistAmount, stretchAmount);
	geometryOutOfDate();
	emit caneChanged();
}

void Model :: pullActiveCane(float twistAmount, float stretchAmount)
{
	if (cane == NULL || activeSubcane == -1)
		return;
	if (cane->subcanes[activeSubcane]->type != PULL_CANETYPE)
		history->saveState(cane);
	cane->pullIntuitive(activeSubcane,twistAmount, stretchAmount);
	geometryOutOfDate();
	emit caneChanged();
}

void Model :: flattenCane(float rectangle_ratio, float rectangle_theta, float flatness)
{
	if (cane == NULL)
		return;
	if (cane->type != FLATTEN_CANETYPE)
		history->saveState(cane);
	cane->flatten(rectangle_ratio, rectangle_theta, flatness);
	geometryOutOfDate();
	emit caneChanged();
}

void Model :: flattenActiveCane(float rectangle_ratio, float rectangle_theta, float flatness)
{
	if (cane == NULL || activeSubcane == -1)
		return;
	if (cane->subcanes[activeSubcane]->type != FLATTEN_CANETYPE)
		history->saveState(cane);
	cane->flatten(activeSubcane, rectangle_ratio, rectangle_theta, flatness);
	geometryOutOfDate();
	emit caneChanged();
}

void Model :: moveCane(float delta_x, float delta_y)
{
	if (cane == NULL || activeSubcane == -1)
		return;
	cane->moveCane(activeSubcane, delta_x, delta_y);
	geometryOutOfDate();
	emit caneChanged();
}

void Model :: moveCane(float delta_z)
{
	if (cane == NULL || activeSubcane == -1)
		return;
	cane->moveCane(activeSubcane, delta_z);
	geometryOutOfDate();
	emit caneChanged();
}

void Model :: deleteActiveCane()
{
	if (cane == NULL || activeSubcane == -1)
		return;
	history->saveState(cane);
	cane->deleteCane(activeSubcane);
	geometryOutOfDate();
	emit caneChanged();
}

void Model :: addCane(Cane* c)
{
	history->saveState(cane);
	if (cane == NULL)
		cane = c->deepCopy();
	else
		cane->add(c->deepCopy());
	geometryOutOfDate();
	emit caneChanged();
	setMode(BUNDLE_MODE);
}

void Model :: undo()
{
	if (history->isInitial())
		return;
	cane = history->getState();
	history->undo();
	geometryOutOfDate();
	emit caneChanged();
}

void Model :: saveObjFile(std::string const &filename)
{
	updateHighResGeometry();
	highResGeometry.save_obj_file(filename);

	updateHighResGeometry();
	highResGeometry.save_obj_file(filename);
}

int Model :: addSnapPoint(Point p)
{
	this->snapPoints[snapCount] = p;
	return snapCount;
}

void Model :: modifySnapPoint(float radii,int index)
{
	this->snapRadii[index] += radii;
}

Point Model :: finalizeSnapPoint(int index)
{
	if (snapRadii[index]<0)
		snapRadii[index]=-snapRadii[index];
	if (snapCount<MAX_SNAP-1 && snapRadii[index]!=0)
		snapCount++;
	return this->snapPoints[snapCount-1];
}

void Model :: modifySnapPoint(float radii)
{
	modifySnapPoint(radii,snapCount);
}

Point Model :: finalizeSnapPoint()
{
	return finalizeSnapPoint(snapCount);
}

int Model :: snap_count()
{
	return snapCount;
}

Point Model :: snapPoint(int index)
{
	return snapPoints[index];
}

float Model :: snapRadius(int index)
{
	return snapRadii[index];
}
