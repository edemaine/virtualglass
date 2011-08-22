
#include "model.h"
#include <QMessageBox>

Model :: Model()
{
	mode = 0;
	projection = PERSPECTIVE_PROJECTION;
	show2D = false;
	cane = NULL;
	history = new CaneHistory();
	geometryFresh = 0;
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

	geometryFresh = 0;
}

Cane* Model :: getSubcane(int subcane)
{
	if (cane == NULL || subcane < 0 || subcane >= cane->subcaneCount)
		return NULL;
	
	return cane->subcanes[subcane];
}

void Model :: setOrthographicProjection()
{
	setProjection(ORTHOGRAPHIC_PROJECTION);
}

void Model :: setPerspectiveProjection()
{
	setProjection(PERSPECTIVE_PROJECTION);
}

void Model :: setProjection(int p)
{
	if (p == ORTHOGRAPHIC_PROJECTION)
	{
		projection = p;
		emit projectionChanged();
	}
	else if (p == PERSPECTIVE_PROJECTION)
	{
		projection = p;
		emit projectionChanged();
	}
}

int Model :: getProjection()
{
	return projection;
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

void Model :: cacheGeometry()
{
	cachedGeometry.clear();
	cachedGeometry.vertices.assign(geometry.vertices.begin(), geometry.vertices.end());
	cachedGeometry.triangles.assign(geometry.triangles.begin(), geometry.triangles.end());
	cachedGeometry.groups.assign(geometry.groups.begin(), geometry.groups.end());
}

void Model :: revertToCachedGeometry()
{
	geometry.clear();
	geometry.vertices.assign(cachedGeometry.vertices.begin(), cachedGeometry.vertices.end());
	geometry.triangles.assign(cachedGeometry.triangles.begin(), cachedGeometry.triangles.end());
	geometry.groups.assign(cachedGeometry.groups.begin(), cachedGeometry.groups.end());
}

void Model :: setMode(int mode)
{
	if (cane == NULL)
		return;

	int prev_mode = this->mode;
	this->mode = mode;

	switch(this->mode)
	{
	case FLATTEN_MODE:
		history->saveState(cane);
		cane->createFlatten();
		slowGeometryUpdate();
		cacheGeometry();
		activeSubcane = -1;
		break;
	case PULL_MODE:
		history->saveState(cane);
		cane->createPull();
		slowGeometryUpdate();
		cacheGeometry();
		activeSubcane = -1;
		break;
	case BUNDLE_MODE:
		history->saveState(cane);
		cane->createBundle();
		slowGeometryUpdate();
		cacheGeometry();
		activeSubcane = -1;
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
			this->mode = SNAP_MODE;
			break;
		}
		break;
	default:
		break;
	}
	emit modeChanged(this->mode);
}

void Model :: setMode (int mode, bool viewRecipe, Cane* c)
{
	if (viewRecipe && c!=NULL)
		insertMode(c, mode);
	else
		setMode(mode);
}

void Model :: insertMode(Cane* c, int mode)
{
	if (c == NULL)
		return;

	switch(mode)
	{
	case FLATTEN_MODE:
		history->saveState(cane);
		c->createFlatten();
		slowGeometryUpdate();
		cacheGeometry();
		break;
	case PULL_MODE:
		history->saveState(cane);
		c->createPull();
		slowGeometryUpdate();
		cacheGeometry();
		break;
	case BUNDLE_MODE:
		history->saveState(cane);
		c->createBundle();
		slowGeometryUpdate();
		cacheGeometry();
		activeSubcane = -1;
		break;
	}
}

bool Model :: subcaneHasColor(int subcane)
{
	if (cane == NULL || subcane < 0 || subcane >= cane->subcaneCount)
		return false;

	if (cane->subcanes[subcane]->getTopBundleNode() != NULL)
		return false;

	Cane* baseCane = cane->subcanes[subcane]->getBaseCane();
	
	if (baseCane == NULL)
		return false; 

	return true;
}

void Model :: setSubcaneColor(int subcane, float r, float g, float b, float a)
{
	if (!subcaneHasColor(subcane))
		return;

	Cane* ac = cane->subcanes[subcane]->getBaseCane();
	ac->color.r = r;
	ac->color.g = g;
	ac->color.b = b;
	ac->color.a = a;
	
	slowGeometryUpdate();
	emit caneChanged();
}

void Model :: setActiveSubcane(int subcane)
{
        if (cane == NULL)
		return;
        if (activeSubcane != subcane)
	{
		activeSubcane = subcane;
		geometryFresh = 0;
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
	if (c == NULL)
		resetAuxiliaries();
	cane = c;
	geometryFresh = 0;
	emit caneChanged();
}

void Model :: slowGeometryUpdate()
{
	Cane* ancestors[MAX_ANCESTORS];
	int ancestorCount = 0;

	geometry.clear();
	if (cane != NULL)
	{
		if (show2D)
		{
			generate2DMesh(cane, &geometry, ancestors, &ancestorCount, LOW_RESOLUTION, false);
		}
		else
		{
			generateMesh(cane, &geometry, ancestors, &ancestorCount, LOW_RESOLUTION, false);
		}
	}
	else
		return;

	cacheGeometry();
	switch (cane->type)
	{
	case PULL_CANETYPE:
		applyPullTransform(&geometry, cane);
		break;
	case FLATTEN_CANETYPE:
		applyFlattenTransform(&geometry, cane);
		break;
	default:
		break;
	}
	geometryFresh = 1;

	emit caneChanged();
}

void Model :: computeHighResGeometry(Geometry* geometry)
{
	Cane* ancestors[MAX_ANCESTORS];
	int ancestorCount = 0;

	geometry->clear();
	if (cane != NULL)
	{
		if (show2D)
		{
			generate2DMesh(cane, geometry, ancestors, &ancestorCount, HIGH_RESOLUTION, true);
		}
		else
		{
			generateMesh(cane, geometry, ancestors, &ancestorCount, HIGH_RESOLUTION, true);
		}
	}
}

Cane* Model :: getCane()
{
	return cane;
}

Geometry* Model :: getGeometry()
{
	if (!geometryFresh)
		slowGeometryUpdate();
	return &geometry;
}

void Model :: pullCane(float twistAmount, float stretchAmount)
{
	revertToCachedGeometry();
	cane->pullIntuitive(twistAmount, stretchAmount);
	applyPullTransform(&geometry, cane);

	geometryFresh = 1;
	emit caneChanged();
}

void Model :: flattenCane(float rectangle_ratio, float rectangle_theta, float flatness)
{
	revertToCachedGeometry();
	cane->flatten(rectangle_ratio, rectangle_theta, flatness);
	applyFlattenTransform(&geometry, cane);

	geometryFresh = 1;
	emit caneChanged();
}

void Model :: moveCane(float delta_x, float delta_y, float delta_z)
{
	if (cane == NULL || activeSubcane == -1)
		return;

        revertToCachedGeometry();
        cane->moveCane(activeSubcane, delta_x, delta_y, delta_z);
	applyPartialMoveTransform(&geometry, cane, activeSubcane, delta_x, delta_y, delta_z);
	cacheGeometry();

	geometryFresh = 1;
	emit caneChanged();



	/*
	The snapping functionality is commented out until it has a reasonable, intuitive
	interface and works. -Andrew
	*/
#ifdef UNDEF
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
#endif
}

bool Model :: deleteActiveCane()
{
	if (cane == NULL || activeSubcane == -1)
		return false;
	history->saveState(cane);
	cane->deleteCane(activeSubcane);
	slowGeometryUpdate();
	emit caneChanged();
	return true;
}

void Model :: addCane(Cane* c)
{
	history->saveState(cane);
	if (cane == NULL)
	{
		cane = c->deepCopy();
		geometryFresh = 0;
		emit caneChanged();
		setMode(BUNDLE_MODE);
	}
	else
	{
		cane->add(c->deepCopy());
		geometryFresh = 0;
		emit caneChanged();
		if (mode != BUNDLE_MODE)
			setMode(BUNDLE_MODE);
		else
		{
			slowGeometryUpdate();
			cacheGeometry();
		}
	}
}

void Model :: addCane(Cane* c, Cane* d)
{
	history->saveState(cane);
	if (c != NULL)
	{
		c->add(d->deepCopy());
		geometryFresh = 0;
		emit caneChanged();
		if (mode != BUNDLE_MODE)
			setMode(BUNDLE_MODE);
		else
		{
			slowGeometryUpdate();
			cacheGeometry();
		}
	}
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
		geometryFresh = 0;
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
		geometryFresh = 0;
		emit caneChanged();
	}
	else
	{
		history->setBusy(false);
	}
}

void Model :: saveObjFile(std::string const &filename)
{
	Geometry highResGeometry;
	computeHighResGeometry(&highResGeometry);
	highResGeometry.save_obj_file(filename);
}

void Model :: saveRawFile(std::string const &filename)
{
	Geometry highResGeometry;
	computeHighResGeometry(&highResGeometry);
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
		cane->moveCane(activeSubcane, 0, 0, 0); // To(activeSubcane,snapHoldPoint);
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

void Model :: toggle2D()
{
	show2D = !show2D;
	geometryFresh = 0;
	emit caneChanged();
}

void Model::exactChange()
{
	geometryFresh = 0;
	emit caneChanged();
}
