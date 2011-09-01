
#include <QMessageBox>
#include "model.h"

Model :: Model()
{
	mode = 0;
	projection = PERSPECTIVE_PROJECTION;
	cane = NULL;
	history = new CaneHistory();
	activeSubcane = -1;
	geometryHeight = 1.0;

	defaultCane = new Cane(BASE_POLYGONAL_CANETYPE);
	defaultCane->setColor(1.0, 1.0, 1.0, 1.0);
	defaultCane->shape.setByTypeAndDiameter(CIRCLE_SHAPE, 0.3, LOW_ANGULAR_RESOLUTION);

	slowGeometryUpdate();
}

void Model :: setGeometryHeight(float height)
{
	geometryHeight = height;
	slowGeometryUpdate();
	emit caneChanged();
} 


int Model :: addNewDefaultCane()
{
	addCane(defaultCane);
	cane->moveCane(cane->subcaneCount-1, 1.0, 1.0, 0.0); 
	
	slowGeometryUpdate();
	emit caneChanged();
	history->saveState(cane);
	return cane->subcaneCount-1;
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
	history->saveState(cane);
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
		activeSubcane = -1;
		break;
	case PULL_MODE:
		history->saveState(cane);
		cane->createPull();
		slowGeometryUpdate();
		activeSubcane = -1;
		break;
	case BUNDLE_MODE:
		history->saveState(cane);
		cane->createBundle();
		slowGeometryUpdate();
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
		break;
	case PULL_MODE:
		history->saveState(cane);
		c->createPull();
		slowGeometryUpdate();
		break;
	case BUNDLE_MODE:
		history->saveState(cane);
		c->createBundle();
		slowGeometryUpdate();
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

CaneShape* Model :: getSubcaneShape(int subcane)
{
	return &(cane->subcanes[subcane]->getBaseCane()->shape);
}

void Model :: setSubcaneShape(int subcane, CaneShape* newShape)
{
	newShape->copy(&(cane->subcanes[subcane]->getBaseCane()->shape));
	slowGeometryUpdate();
	emit caneChanged();
}

Color* Model :: getSubcaneColor(int subcane)
{
	return &(cane->subcanes[subcane]->getBaseCane()->color);
}

void Model :: setSubcaneColor(int subcane, Color* c)
{
	Cane* ac = cane->subcanes[subcane]->getBaseCane();
	ac->color.r = c->r;
	ac->color.g = c->g;
	ac->color.b = c->b;
	ac->color.a = c->a;
	
	slowGeometryUpdate();
	emit caneChanged();
}

void Model :: setSubcaneAlpha(int subcane, int newAlpha)
{
        Cane* ac = cane->subcanes[subcane]->getBaseCane();
        Color* newColor = new Color(ac->color);
        newColor->a = newAlpha/float(255);
        setSubcaneColor(subcane, newColor);
}

void Model :: setActiveSubcane(int subcane)
{
        if (cane == NULL)
		return;
        if (activeSubcane != subcane)
	{
		activeSubcane = subcane;
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
	if (c != NULL)
		cane = c->deepCopy();
	else
		cane = NULL;
	slowGeometryUpdate();
	emit caneChanged();
	if (cane == NULL)
		return;

	switch (cane->type)
	{
		case BUNDLE_CANETYPE:
			mode = BUNDLE_MODE;
			break;
		case PULL_CANETYPE:
			mode = PULL_MODE;
			break;
		case FLATTEN_CANETYPE:
			mode = FLATTEN_MODE;
			break;
		case BASE_POLYGONAL_CANETYPE:
			mode = NO_MODE;
			break;
	}
	emit modeChanged(mode);
}

void Model :: slowGeometryUpdate()
{
	Cane* ancestors[MAX_ANCESTORS];
	int ancestorCount = 0;

	geometry.clear();
	if (cane != NULL)
	{
		generateMesh(cane, &geometry, geometryHeight, ancestors, &ancestorCount, LOW_RESOLUTION, false);
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

	emit caneChanged();
}

void Model :: computeHighResGeometry(Geometry* geometry)
{
	Cane* ancestors[MAX_ANCESTORS];
	int ancestorCount = 0;

	geometry->clear();
	if (cane != NULL)
	{
		generateMesh(cane, geometry, geometryHeight, ancestors, &ancestorCount, HIGH_RESOLUTION, true);
	}
}

Cane* Model :: getCane()
{
	return cane;
}

Geometry* Model :: getGeometry()
{
	return &geometry;
}

void Model :: pullCane(float twistAmount, float stretchAmount)
{
	revertToCachedGeometry();
	cane->pullIntuitive(twistAmount, stretchAmount);
	applyPullTransform(&geometry, cane);

	emit caneChanged();
}

void Model :: flattenCane(float rectangle_ratio, float rectangle_theta, float flatness)
{
	revertToCachedGeometry();
	cane->flatten(rectangle_ratio, rectangle_theta, flatness);
	applyFlattenTransform(&geometry, cane);

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
		setCane(c);
	}
	else
	{
		if (mode != BUNDLE_MODE)
			setMode(BUNDLE_MODE);
		cane->add(c->deepCopy());
		slowGeometryUpdate();
		emit caneChanged();
	}
}


/*
This is like addCane(Cane*), but not quite.
It's not clear why it should be different, or
why it's copy-pasted and does not reference the
other version...maybe this can be fixed eventually?
*/
void Model :: addCane(Cane* c, Cane* d)
{
	history->saveState(cane);
	if (c != NULL)
	{
		c->add(d->deepCopy());
		slowGeometryUpdate();
		emit caneChanged();
		if (mode != BUNDLE_MODE)
			setMode(BUNDLE_MODE);
	}
}

void Model :: undo()
{
	if (history->canUndo())
	{
		cane = history->undo();
		slowGeometryUpdate();
		emit caneChanged();
	}
}

void Model :: redo()
{
	if (history->canRedo())
	{
		cane = history->redo();
		slowGeometryUpdate();
		emit caneChanged();
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

void Model::exactChange()
{
	slowGeometryUpdate();
	emit caneChanged();
}
