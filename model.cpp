
#include "model.h"
#include <QMessageBox>

Model :: Model()
{
	mode = 0;
	cane = NULL;
	history = new CaneHistory();
	lowResGeometryFresh = 0;
	highResGeometryFresh = 0;
	activeSubcane = -1;

	activeSnapMode = NO_SNAP;
	activeSnapIndex = 0;
	activeSnapPlacementMode = NO_SNAP;
	for (int i = 0; i < MAX_SNAP; ++i)
	{
		snapPoints[i].x=snapPoints[i].y=snapPoints[i].z=0.0;
		snapPointRadii[i]=0.0;
		snapSegments1[i].x=snapSegments1[i].y=snapSegments1[i].z=0.0;
		snapSegments2[i].x=snapSegments2[i].y=snapSegments2[i].z=0.0;
		snapCircles[i].x=snapCircles[i].y=snapCircles[i].z=0.0;
		snapCircleRadii[i]=0.0;
	}
	snapPointsCount=snapLinesCount=snapCirclesCount=0;

	geometryOutOfDate();
}

void Model :: clearCurrentCane()
{
	mode = 0;
	activeSnapMode = NO_SNAP;
	activeSnapIndex = 0;
	activeSnapPlacementMode = NO_SNAP;
	for (int i = 0; i < MAX_SNAP; ++i)
	{
		snapPoints[i].x=snapPoints[i].y=snapPoints[i].z=0.0;
		snapPointRadii[i]=0.0;
		snapSegments1[i].x=snapSegments1[i].y=snapSegments1[i].z=0.0;
		snapSegments2[i].x=snapSegments2[i].y=snapSegments2[i].z=0.0;
		snapCircles[i].x=snapCircles[i].y=snapCircles[i].z=0.0;
		snapCircleRadii[i]=0.0;
	}
	snapPointsCount=snapLinesCount=snapCirclesCount=0;
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
	if (mode == SNAP_MODE)
	{
		switch(prev_mode)
		{
		case SNAP_MODE:
			this->mode = SNAP_LINE_MODE;
			break;
		case SNAP_LINE_MODE:
			this->mode = SNAP_CIRCLE_MODE;
			break;
		case SNAP_CIRCLE_MODE:
			this->mode = SNAP_MODE;
			break;
		default:
			break;
		}
	}

	emit modeChanged(this->mode);
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

int Model :: addSnapPoint(int snapMode, Point p)
{
	activeSnapPlacementMode = snapMode;
	switch(snapMode)
	{
	case SNAP_POINT:
		this->snapPoints[snapPointsCount] = p;
		return snapPointsCount;
	case SNAP_LINE:
		this->snapSegments1[snapLinesCount] = p;
		this->snapSegments2[snapLinesCount] = p;
		return snapLinesCount;
	case SNAP_CIRCLE:
		this->snapCircles[snapCirclesCount] = p;
		return snapCirclesCount;
	default:
		activeSnapPlacementMode = NO_SNAP;
		return -1;
	}
}

void Model :: modifySnapPoint(float radii,int index)
{
	switch(activeSnapPlacementMode)
	{
	case SNAP_POINT:
		this->snapPointRadii[index] = radii;
		break;
	case SNAP_CIRCLE:
		this->snapCircleRadii[index] = radii;
		break;
	default:
		break;
	}
}

Point Model :: finalizeSnapPoint(int index)
{
	switch(activeSnapPlacementMode)
	{
	case SNAP_POINT:
		activeSnapPlacementMode = NO_SNAP;
		if (snapPointRadii[index]<0)
			snapPointRadii[index]=-snapPointRadii[index];
		if (snapPointsCount<MAX_SNAP-1 && snapPointRadii[index]!=0)
			snapPointsCount++;
		return this->snapPoints[snapPointsCount-1];
	case SNAP_LINE:
		activeSnapPlacementMode = NO_SNAP;
		if (snapLinesCount<MAX_SNAP-1 && snapSegments2[index]!=snapSegments1[index])
			snapLinesCount++;
		return this->snapSegments2[snapLinesCount-1];
	case SNAP_CIRCLE:
		activeSnapPlacementMode = NO_SNAP;
		if (snapCircleRadii[index]<0)
			snapCircleRadii[index]=-snapCircleRadii[index];
		if (snapCirclesCount<MAX_SNAP-1 && snapCircleRadii[index]!=0)
			snapCirclesCount++;
		return this->snapCircles[snapCirclesCount-1];
	default:
		return Point();
	}

}

void Model :: modifySnapPoint(float radii)
{
	switch(activeSnapPlacementMode)
	{
	case SNAP_POINT:
		modifySnapPoint(radii,snapPointsCount);
		break;
	case SNAP_CIRCLE:
		modifySnapPoint(radii,snapCirclesCount);
		break;
	default:
		break;
	}
}

void Model :: modifySnapPoint(Point p, int index)
{
	Point dir;
	switch(activeSnapPlacementMode)
	{
	case SNAP_POINT:
		dir = p-snapPoints[index];
		modifySnapPoint(length(dir),index);
		break;
	case SNAP_LINE:
		snapSegments2[index] = p;
		break;
	case SNAP_CIRCLE:
		dir = p-snapCircles[index];
		modifySnapPoint(length(dir),index);
		break;
	default:
		break;
	}
}

void Model :: modifySnapPoint(Point p)
{
	switch(activeSnapPlacementMode)
	{
	case SNAP_POINT:
		modifySnapPoint(p,snapPointsCount);
		break;
	case SNAP_LINE:
		modifySnapPoint(p,snapLinesCount);
		break;
	case SNAP_CIRCLE:
		modifySnapPoint(p,snapCirclesCount);
		break;
	default:
		return;
	}
}

Point Model :: finalizeSnapPoint()
{
	switch(activeSnapPlacementMode)
	{
	case SNAP_POINT:
		return finalizeSnapPoint(snapPointsCount);
	case SNAP_LINE:
		return finalizeSnapPoint(snapLinesCount);
	case SNAP_CIRCLE:
		return finalizeSnapPoint(snapCirclesCount);
	default:
		return Point();
	}
}

int Model :: snapPointCount(int snapMode)
{
	switch(snapMode)
	{
	case SNAP_POINT:
		return snapPointsCount;
	case SNAP_LINE:
		return snapLinesCount;
	case SNAP_CIRCLE:
		return snapCirclesCount;
	default:
		return -1;
	}
}

Point Model :: snapPoint(int snapMode, int index)
{
	switch(snapMode)
	{
	case SNAP_POINT:
		return snapPoints[index];
	case SNAP_LINE:
		return snapSegments1[index];
	case SNAP_CIRCLE:
		return snapCircles[index];
	default:
		return Point();
	}
}

Point Model :: snapPoint2(int snapMode, int index)
{
	if (snapMode == SNAP_LINE)
		return snapSegments2[index];
	return Point();
}

Point Model :: snapPoint2(int index)
{
	return snapSegments2[index];
}

float Model :: snapPointRadius(int snapMode, int index)
{
	switch(snapMode)
	{
	case SNAP_POINT:
		return snapPointRadii[index];
	case SNAP_CIRCLE:
		return snapCircleRadii[index];
	default:
		return -1;
	}
}
