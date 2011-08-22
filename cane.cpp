/*
Initializes a cane node (i.e. a constructed top-level
definition of a cane).
*/

#include "cane.h"

Cane :: Cane()
{
	reset();
	this->type = UNASSIGNED_CANETYPE;
}

Cane :: Cane(int type)
{
	reset();
	this->type = type;
}

// Resets the object to the default values of everything
void Cane :: reset()
{
	int i;

	type = UNASSIGNED_CANETYPE;
	vertices.clear();
	for (i = 0; i < MAX_AMT_TYPES; ++i)
	{
		amts[i] = 0.0;
	}

	subcaneCount = 0;
	for (i = 0; i < MAX_SUBCANE_COUNT; ++i)
	{
		subcanes[i] = NULL;
		subcaneLocations[i] = make_vector(0.0f, 0.0f, 0.0f);
	}

	color.r = color.g = color.b = color.a = 1.0;
	libraryIndex=-1;
}

// Returns the number of nodes in the cane's DAG
int Cane :: leafNodes()
{
	if (this->subcaneCount == 0)
	{
		return 1;
	}
	else
	{
		int total = 0;
		for (int i = 0; i < this->subcaneCount; ++i)
			total += this->subcanes[i]->leafNodes();
		return total;
	}
}

// Copies the information in a cane object into
// the destination cane object
void Cane :: shallowCopy(Cane* dest)
{
	int i;

	dest->type = this->type;
	for (unsigned int v = 0; v < this->vertices.size(); ++v)
	{
		dest->vertices.push_back(this->vertices[v]);
	}
	for (i = 0; i < MAX_AMT_TYPES; ++i)
	{
		dest->amts[i] = this->amts[i];
	}

	dest->subcaneCount = this->subcaneCount;
	for (i = 0; i < MAX_SUBCANE_COUNT; ++i)
	{
		if (this->subcanes[i] != NULL)
		{
			dest->subcanes[i] = this->subcanes[i]->deepCopy();
			dest->subcaneLocations[i].x = this->subcaneLocations[i].x;
			dest->subcaneLocations[i].y = this->subcaneLocations[i].y;
			dest->subcaneLocations[i].z = this->subcaneLocations[i].z;
		}
		else
			dest->subcanes[i] = NULL;
	}
	dest->color = this->color;
	dest->libraryIndex = this->libraryIndex;
}

void Cane :: pullLinear(float twistFactor, float stretchFactor)
{
	if (this->type != PULL_CANETYPE)
		return;

	this->amts[0] += twistFactor;
	this->amts[0] += stretchFactor;
}

/*
Returns the height of the cane DAG, i.e. the length of
the longest path.
*/
int Cane :: height()
{
	int max = 0;
	for (int i = 0; i < this->subcaneCount; ++i)
		max = MAX(max, this->subcanes[i]->height());
	return (max + 1);
}

void Cane :: pullIntuitive(float twistFactor, float stretchFactor)
{
	// The amount twist and stretch are changed are functions
	// of the amount already present. The exact function is
	// determined by feel/playing with the tool.
	if (this->type != PULL_CANETYPE)
		return;

	if (this->amts[0] < 0.0)
	{
		if (this->amts[0] > -1.0)
			this->amts[0] -= 8 * -twistFactor;
		else
			this->amts[0] *= (1.0 + -twistFactor);
	}
	else
	{
		if (this->amts[0] < 1.0)
			this->amts[0] += 8 * twistFactor;
		else
			this->amts[0] *= (1.0 + twistFactor);
	}
	this->amts[1] *= (1.0 + stretchFactor);
}



/*
Cane::flatten() creates a new root node that deforms the cane
from a circular shape to an approximation of a rectangle.
The `rectangle_ratio' specifies the relative dimensions of
the rectangle the cane will be flattened into. A ratio of
1.0 is square, 2.0 is a rectangle twice as wide as it is tall, etc.
`rectangle_theta' specifies the orientation of the x-axis of the
rectangle relative to the global cane x-axis. `flatness' specifies
how closely the cane is squished into the goal rectangle. A
flatness of 0 means the cane remains circular, while a ratio of 1 means
the cane is deformed into a perfect rectangle.
*/
void Cane :: flatten(float rectangle_ratio, float rectangle_theta, float flatness)
{
	if (this->type != FLATTEN_CANETYPE || this->amts[1] != rectangle_theta)
	{
		Cane* copy = new Cane(UNASSIGNED_CANETYPE);
		this->shallowCopy(copy);
		this->reset();
		this->type = FLATTEN_CANETYPE;
		this->subcaneCount = 1;
		this->subcanes[0] = copy;
		this->amts[0] = 1.0; // rectangle_ratio
		this->amts[1] = rectangle_theta;
		this->amts[2] = 0.0; // flatness
	}
	this->amts[0] *= (1.0 + rectangle_ratio);
	this->amts[2] += flatness;
	this->amts[2] = MIN(1.0, MAX(0.0, amts[2]));
}

void Cane :: createFlatten()
{
	Cane* copy = new Cane(UNASSIGNED_CANETYPE);
	this->shallowCopy(copy);
	this->reset();
	this->type = FLATTEN_CANETYPE;
        this->amts[0] = 0.0;
	this->amts[1] = 0.0;
	this->amts[2] = 0.0;
	this->subcaneCount = 1;
	this->subcanes[0] = copy;
}

void Cane :: createPull()
{
	Cane* copy = new Cane(UNASSIGNED_CANETYPE);
	this->shallowCopy(copy);
	this->reset();
	this->type = PULL_CANETYPE;
	this->amts[0] = 0.0;
	this->amts[1] = 1.0;
	this->subcaneCount = 1;
	this->subcanes[0] = copy;
}

void Cane :: createBundle()
{
	Cane* copy = new Cane(UNASSIGNED_CANETYPE);
	this->shallowCopy(copy);
	this->reset();
	this->type = BUNDLE_CANETYPE;
	this->subcaneCount = 1;
	this->subcanes[0] = copy;
}

void Cane :: moveCane(int subcane, float delta_x, float delta_y, float delta_z)
{
	if ((unsigned)subcane >= (unsigned)subcaneCount) {
		std::cerr << "Trying to move non-existent subcane " << subcane << std::endl;
		return;
	}
	subcaneLocations[subcane].x += delta_x;
	subcaneLocations[subcane].y += delta_y;
	subcaneLocations[subcane].z += delta_z;
}

void Cane :: add(Cane* addl)
{
	if (this->type != BUNDLE_CANETYPE)
		createBundle();

	if (subcaneCount == MAX_SUBCANE_COUNT) {
		std::cerr << "Trying to add another subcane when already maxed out." << std::endl;
		return;
	}
	// Add the new cane to the bundle
	subcanes[subcaneCount] = addl;
	subcaneLocations[subcaneCount].x = 0;
	subcaneLocations[subcaneCount].y = 0;
	subcaneLocations[subcaneCount].z = 0;
	subcaneCount += 1;
}

Cane* Cane :: getTopBundleNode()
{
	if (this->type == BUNDLE_CANETYPE)
		return this;
	else if (this->subcaneCount == 0)
	{
		return NULL;
	}

	return this->subcanes[0]->getTopBundleNode();
}

void Cane :: deleteCane(int subcane)
{
	Cane* b = this->getTopBundleNode();

	b->subcaneCount--;
	for (int i = subcane; i < b->subcaneCount; i++)
	{
		b->subcanes[i] = b->subcanes[i+1];
		b->subcaneLocations[i] = b->subcaneLocations[i+1];
	}
}

Cane* Cane :: deepCopy()
{
	int i;
	Cane* copy;

	copy = new Cane(this->type);
	for (unsigned int v = 0; v < this->vertices.size(); ++v)
	{
		copy->vertices.push_back(this->vertices[v]);
	}

	for (i = 0; i < MAX_AMT_TYPES; ++i)
	{
		copy->amts[i] = this->amts[i];
	}

	copy->subcaneCount = this->subcaneCount;
	for (i = 0; i < MAX_SUBCANE_COUNT; ++i)
	{
		if (this->subcanes[i] != NULL)
		{
			copy->subcanes[i] = this->subcanes[i]->deepCopy();
			copy->subcaneLocations[i].x = this->subcaneLocations[i].x;
			copy->subcaneLocations[i].y = this->subcaneLocations[i].y;
			copy->subcaneLocations[i].z = this->subcaneLocations[i].z;
		}
		else
			copy->subcanes[i] = NULL;
	}
	copy->color = this->color;
	copy->libraryIndex = this->libraryIndex;

	return copy;
}

void Cane :: setColor(Color color)
{
	this->color = color;
}

bool isNaN(float n)
{
	return n!=n;
}

float changeIfNaN(float n,float base)
{
	if (isNaN(n))
		return base;
	else
		return n;
}

float zeroIfNaN(float n)
{
	return changeIfNaN(n,0);
}

std::string typeToName(int type)
{
	switch(type)
	{
	case PULL_CANETYPE:
		return "Pull Cane";
	case BUNDLE_CANETYPE:
		return "Bundle Cane";
	case FLATTEN_CANETYPE:
		return "Flatten Cane";
	case BASE_CIRCLE_CANETYPE:
		return "Base Circle Cane";
	case BASE_SQUARE_CANETYPE:
		return "Base Square Cane";
	case BASE_POLYGONAL_CANETYPE:
		return "Base Polygonal Cane";
	case UNASSIGNED_CANETYPE:
		return "Unassigned Cane";
	default:
		return UNDEFINED;
	}
}

std::string typeToType(int type)
{
	switch(type)
	{
	case PULL_CANETYPE:
		return "Pull";
	case BUNDLE_CANETYPE:
		return "Bundle";
	case FLATTEN_CANETYPE:
		return "Flatten";
	case BASE_CIRCLE_CANETYPE:
		return "Base Circle";
	case BASE_SQUARE_CANETYPE:
		return "Base Square";
	case BASE_POLYGONAL_CANETYPE:
		return "Base Polygon";
	case UNASSIGNED_CANETYPE:
		return "Unassigned";
	default:
		return UNDEFINED;
	}
}

QString Cane :: typeName()
{
	return QString(typeToType(type).c_str());
}

std::string amtTypeToName(int type,int index)
{
	switch(type)
	{
	case PULL_CANETYPE:
		switch(index)
		{
		case 0:
			return "Twist";
		case 1:
			return "Stretch";
		default:
			return UNDEFINED;
		}
	case BUNDLE_CANETYPE:
		return UNDEFINED;
	case FLATTEN_CANETYPE:
		switch(index)
		{
		case 0:
			return "Rectangle Ratio";
		case 1:
			return "Rectangle Theta";
		case 2:
			return "Flattened Ratio";
		default:
			return UNDEFINED;
		}
	case BASE_CIRCLE_CANETYPE:
		return UNDEFINED;
	default:
		return UNDEFINED;
	}
}

QString Cane :: typeAmt(int type, int index)
{
	return QString(amtTypeToName(type,index).c_str());
}

std::string Cane :: yamlRepresentation()
{
	YAML::Emitter out;
	out << YAML::BeginMap;
	out << YAML::Key << "Type";
	out << YAML::Value << type;

	out << YAML::Key << "Description";
	out << YAML::Value << typeToName(type);

	out << YAML::Key << "Defined Amounts";
	out << YAML::Value << YAML::BeginSeq;
	for (unsigned int j=0;j<sizeof(amts);j++){
		out << amtTypeToName(type,j);
		out << zeroIfNaN(amts[j]);
	}
	out << YAML::EndSeq;

	out << YAML::Key << "Vertices";
	out << YAML::Value << YAML::BeginSeq;
	for (unsigned int j=0;j<sizeof(this->vertices);j++){
		out << zeroIfNaN(vertices[j].x);
		out << zeroIfNaN(vertices[j].y);
	}
	out << YAML::EndSeq;

	out << YAML::Key << "Number of Subcanes";
	out << YAML::Value << subcaneCount;

	out << YAML::Key << "Subcane Locations";
	out << YAML::Value << YAML::BeginSeq;
	for (int j=0;j<subcaneCount;j++)
	{
		Point loc = subcaneLocations[j];
		out << YAML::BeginSeq;

		out << zeroIfNaN(loc.x) << zeroIfNaN(loc.y) << zeroIfNaN(loc.z);
		out << YAML::EndSeq;
	}
	out << YAML::EndSeq;

	out << YAML::Key << "RGBA Color";
	out << YAML::Value << YAML::BeginSeq;
	out << color.r << color.g << color.b << color.a;
	out << YAML::EndSeq;

	out << YAML::Key << "Subcanes";
	out << YAML::Value << YAML::BeginSeq;
	for (int j=0;j<subcaneCount;j++){
		Cane* cane = subcanes[j];

		out << YAML::Literal << cane->yamlRepresentation();
	}
	out << YAML::EndSeq;

	out << YAML::EndMap;

	return out.c_str();
}

QColor Cane :: qcolor()
{
	QColor result;
	result.setRgbF(color.r, color.g, color.b, color.a);
	return result;
}

void Cane :: setColor(QColor color)
{
	Color newColor;
	newColor.r = color.redF();
	newColor.g = color.greenF();
	newColor.b = color.blueF();
	newColor.a = color.alphaF();
	setColor(newColor);
}

void Cane :: changeLibraryIndex(int index)
{
	libraryIndex = index;
}
