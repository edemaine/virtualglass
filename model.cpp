
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
	snapLineParam = 0.1;
	snapCircleParam = 0.2;
	snapPointsCount=snapLinesCount=snapCirclesCount=0;
	snapHoldPoint = Point();
	geometryOutOfDate();
}

void Model :: resetAuxiliaries()
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
	snapHoldPoint = Point();
}

void Model :: clearCurrentCane()
{
	resetAuxiliaries();
	setCane(NULL);
}

CaneHistory* Model::getHistory()
{
	return history;
}

int Model :: getMode()
{
	return mode;
}

void Model :: setMode(int mode)
{
	int prev_mode = this->mode;
	this->mode = mode;

	//if (cane == NULL)
	//	return;

	switch(this->mode)
	{
	case BUNDLE_MODE:
		history->saveState(cane);
		activeSubcane = -1;
		cane->createBundle();
		break;
	case CASING_MODE:
	{
		Cane* ancestors[MAX_ANCESTORS];
		int ancestorCount = 0;

		lowResGeometry.clear();
		cane->createCasing(generateMesh(cane, &lowResGeometry, ancestors,
										&ancestorCount, LOW_RESOLUTION, true, true));
		geometryOutOfDate();
		emit caneChanged();
	}
	break;
	case SNAP_MODE:
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
		break;
	default:
		break;
	}
	emit modeChanged(this->mode);
}

void Model :: setActiveSubcane(int subcane)
{
	if (cane == NULL)
		return;
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
	if (c==NULL)
		resetAuxiliaries();
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
		generateMesh(cane, &lowResGeometry, ancestors, &ancestorCount, LOW_RESOLUTION, true);
	lowResGeometryFresh = 1;
}

void Model :: updateHighResGeometry()
{
	Cane* ancestors[MAX_ANCESTORS];
	int ancestorCount;

	ancestorCount = 0;
	highResGeometry.clear();
	if (cane != NULL)
		generateMesh(cane, &highResGeometry, ancestors, &ancestorCount, HIGH_RESOLUTION, true);
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

	if (activeSnapMode != NO_SNAP)
	{
		cane->moveCaneTo(activeSubcane,snapHoldPoint);
	}

	cane->moveCane(activeSubcane, delta_x, delta_y);

	// check if cane hits any snaps


	Point loc = cane->subcaneLocations[activeSubcane];
	for (int i=0; i<snapPointsCount;i++){
		if (length(loc-snapPoint(SNAP_POINT,i))<snapPointRadius(SNAP_POINT,i))
		{
			snapHoldPoint = loc;
			activeSnapMode = SNAP_POINT;
			activeSnapIndex = i;
			cane->moveCaneTo(activeSubcane,snapPoint(SNAP_POINT,i));
			geometryOutOfDate();
			emit caneChanged();
			return;
		}
	}

	for (int i=0; i<snapCirclesCount;i++){
		Point p=snapPoint(SNAP_CIRCLE,i);
		Point disp = loc-p;
		float dist = length(disp);
		float radi = snapPointRadius(SNAP_CIRCLE,i);
		if (radi*(1-snapCircleParam)<dist && dist<radi*(1+snapCircleParam))
		{
			snapHoldPoint = loc;
			activeSnapMode = SNAP_CIRCLE;
			activeSnapIndex = i;
			disp = disp*radi/dist + p;
			cane->moveCaneTo(activeSubcane,disp);
			geometryOutOfDate();
			emit caneChanged();
			return;
		}
	}

	for (int i=0; i<snapLinesCount;i++){
		Point p1 = snapPoint(SNAP_LINE,i);
		Point p2 = snapPoint2(SNAP_LINE,i);
		Point a = loc-p1;
		Point b = p2-p1;
		Point dist = (a*b/(b*b))*b;
		Point p = dist+p1;
		Point displacement = loc - p;
		if (length(displacement)<snapLineParam && length(dist)<length(b) && length(dist)>0)
		{
			snapHoldPoint = loc;
			activeSnapMode = SNAP_LINE;
			activeSnapIndex = i;
			cane->moveCaneTo(activeSubcane,p);
			geometryOutOfDate();
			emit caneChanged();
			return;
		}
	}

	clearActiveSnap(true);
	geometryOutOfDate();
	emit caneChanged();
}

void Model :: moveCane(float delta_x, float delta_y, bool snaps)
{
	if (snaps)
	{
		moveCane(delta_x,delta_y);
		return;
	}

	if (cane == NULL || activeSubcane == -1)
		return;

	cane->moveCane(activeSubcane, delta_x, delta_y);

	// check if cane hits any snaps

	clearActiveSnap(true);
	geometryOutOfDate();
	emit caneChanged();
}

void Model :: moveCaneTo(float delta_x, float delta_y)
{
	if (cane == NULL || activeSubcane == -1)
		return;
	cane->moveCaneTo(activeSubcane, delta_x, delta_y);

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

void Model :: changeCaneCasing(float delta_x)
{
	if (cane == NULL)
		return;
	cane->adjustCasing(delta_x);
	geometryOutOfDate();
	emit caneChanged();
}

void Model :: changeCaneCasingTo(float radi)
{
	if (cane == NULL)
		return;
	cane->adjustCasingTo(radi);
	geometryOutOfDate();
	emit caneChanged();
}

void Model :: changeCaneCasingTo(Point radi)
{
	changeCaneCasingTo(length(radi));
}

bool Model :: deleteActiveCane()
{
	if (cane == NULL || activeSubcane == -1)
		return false;
	history->saveState(cane);
	cane->deleteCane(activeSubcane);
	geometryOutOfDate();
	emit caneChanged();
	return true;
}

void Model :: addCane(Cane* c)
{
	history->saveState(cane);
	if (cane == NULL)
	{
		cane = c->deepCopy();
		setMode(BUNDLE_MODE);
	}
	else
	{
		if (mode != BUNDLE_MODE)
			setMode(BUNDLE_MODE);
		cane->add(c->deepCopy());
	}
	geometryOutOfDate();
	emit caneChanged();
}

void Model :: undo()
{
	if (history->isAvailable())
	{
		history->setBusy(true);
	}
	else
	{
		return;
	}
	if (history->isMostRecent())
	{
		history->saveState(cane);
		history->undo();
	}
	Cane* temp = history->undo();
	if (temp != NULL)
	{
		cane = temp;
		geometryOutOfDate();
		emit caneChanged();
	}
	else
	{
		history->setBusy(false);
	}
}

void Model :: redo()
{
	if (history->isAvailable())
	{
		history->setBusy(true);
	}
	else
	{
		return;
	}
	Cane* temp = history->redo();
	if (temp != NULL)
	{
		cane = temp;
		geometryOutOfDate();
		emit caneChanged();
	}
	else
	{
		history->setBusy(false);
	}
}

void Model :: saveObjFile(std::string const &filename)
{
	if (!highResGeometryFresh) {
		updateHighResGeometry();
	}
	highResGeometry.save_obj_file(filename);
}

void Model :: saveRawFile(std::string const &filename)
{
	if (!highResGeometryFresh) {
		updateHighResGeometry();
	}
	highResGeometry.save_raw_file(filename);
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

int Model :: getActiveSnapMode()
{
	return activeSnapMode;
}

int Model :: getActiveSnapIndex()
{
	return activeSnapIndex;
}

void Model :: clearActiveSnap(bool holdSnap)
{
	if (activeSnapMode != NO_SNAP && holdSnap)
		cane->moveCaneTo(activeSubcane,snapHoldPoint);
	activeSnapMode = NO_SNAP;
	activeSnapIndex = -1;
	snapHoldPoint = Point();
}

void Model :: deleteSnapPoint(Point loc)
{
	for (int i=0; i<snapPointsCount;i++){
		if (length(loc-snapPoint(SNAP_POINT,i))<snapPointRadius(SNAP_POINT,i))
		{
			for (int j=i;j<snapPointsCount-1;j++)
			{
				snapPoints[j]=snapPoints[j+1];
				snapPointRadii[j]=snapPointRadii[j+1];
			}
			snapPoints[snapPointsCount-1]=Point();
			snapPointRadii[snapPointsCount-1]=0;
			snapPointsCount--;
			return;
		}
	}

	for (int i=0; i<snapCirclesCount;i++){
		Point p=snapPoint(SNAP_CIRCLE,i);
		Point disp = loc-p;
		float dist = length(disp);
		float radi = snapPointRadius(SNAP_CIRCLE,i);
		if (radi*(1-snapCircleParam)<dist && dist<radi*(1+snapCircleParam))
		{
			for (int j=i;j<snapCirclesCount-1;j++)
			{
				snapCircles[j]=snapCircles[j+1];
				snapCircleRadii[j]=snapCircleRadii[j+1];
			}
			snapCircles[snapCirclesCount-1]=Point();
			snapCircleRadii[snapCirclesCount-1]=0;
			snapCirclesCount--;
			return;
		}
	}

	for (int i=0; i<snapLinesCount;i++){
		Point p1 = snapPoint(SNAP_LINE,i);
		Point p2 = snapPoint2(SNAP_LINE,i);
		Point a = loc-p1;
		Point b = p2-p1;
		Point dist = (a*b/(b*b))*b;
		Point p = dist+p1;
		Point displacement = loc - p;
		if (length(displacement)<snapLineParam && length(dist)<length(b) && length(dist)>0)
		{
			for (int j=i;j<snapLinesCount-1;j++)
			{
				snapSegments1[j]=snapSegments1[j+1];
				snapSegments2[j]=snapSegments2[j+1];
			}
			snapSegments1[snapLinesCount-1]=Point();
			snapSegments2[snapLinesCount-1]=Point();
			snapLinesCount--;
			return;
		}
	}

	clearActiveSnap(false);
}
