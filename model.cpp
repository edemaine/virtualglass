
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

void Model :: setCaneToNull()
{
	setCane(NULL);
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

bool Model :: subcaneHasColorAndShape(int subcane)
{
	if (cane == NULL || subcane < 0 || subcane >= cane->subcaneCount)
		return false;

	Cane* baseCane = cane->subcanes[subcane]->getBaseCane();
	
	if (baseCane == NULL)
		return false; 

	return true;
}

void Model :: setSubcaneShape(int subcane, int shape, float size)
{
	if (!subcaneHasColorAndShape(subcane))
		return;

	Cane* ac = cane->subcanes[subcane]->getBaseCane();
	ac->setShape(shape, LOW_ANGULAR_RESOLUTION, size);
	slowGeometryUpdate();
	emit caneChanged();
}

void Model :: setSubcaneColor(int subcane, float r, float g, float b, float a)
{
	if (!subcaneHasColorAndShape(subcane))
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
		slowGeometryUpdate();
		cacheGeometry();
		setMode(BUNDLE_MODE);
		emit caneChanged();
	}
	else
	{
		if (mode != BUNDLE_MODE)
			setMode(BUNDLE_MODE);
		cane->add(c->deepCopy());
		slowGeometryUpdate();
		cacheGeometry();
		emit caneChanged();
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
