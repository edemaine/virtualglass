
#include "canefile.h"

void saveCanesToFile(QString fileName, vector<Cane*> canes)
{
	if (!fileName.endsWith(".glass") || fileName.length() < 6)
		return;

	YAML::Emitter out;
	out << canes.size();
	out << YAML::BeginSeq;

	for (unsigned int i = 0; i < canes.size(); ++i)
	{
		Cane* cane = canes[i];
		out << YAML::Literal << cane->yamlRepresentation();
	}

	out << YAML::EndSeq;

	QFile file(fileName);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
		return;

	file.reset();

	QTextStream outStream(&file);
	outStream << out.c_str() << "\n";
	outStream.flush();
	file.close();
}

void parseCaneFromYAML(const YAML::Node& node, Cane* cane)
{

	std::string caneLiteral;
	node.GetScalar(caneLiteral);

	istringstream sstream ( caneLiteral );
	YAML::Parser newParser (sstream);
	YAML::Node newNode;
	newParser.GetNextDocument(newNode);

	newNode["Type"] >> cane->type;

	const YAML::Node& caneAmts = newNode["Defined Amounts"];

	int amtsCount = 0;

	for(YAML::Iterator it2 = caneAmts.begin(); it2 != caneAmts.end(); ++it2)
	{
		std::string temp;
		*it2 >> temp;
		++it2;
		*it2 >> cane->amts[amtsCount];
		amtsCount++;
	}

	const YAML::Node& caneVertices = newNode["Vertices"];
	vector< Vector2f > vertices;

	for(YAML::Iterator it2 = caneVertices.begin(); it2 != caneVertices.end(); ++it2)
	{
		const YAML::Node& caneVertex = *it2;
		Vector2f loc;

		caneVertex[0] >> loc.x;
		caneVertex[1] >> loc.y;
		vertices.push_back(loc);
	}

	cane->shape.setByVertices(vertices);

	const YAML::Node& caneTriangles = newNode["Triangles"];
	vector< Vector3ui > tris;

	for(YAML::Iterator it2 = caneTriangles.begin(); it2 != caneTriangles.end(); ++it2)
	{
		const YAML::Node& caneTriangle = *it2;
		Vector3ui tri;
		caneTriangle[0] >> tri.c[0];
		caneTriangle[1] >> tri.c[1];
		caneTriangle[2] >> tri.c[2];
		tris.push_back(tri);
	}

	cane->shape.tris = tris;


	newNode["Number of Subcanes"] >> cane->subcaneCount;

	const YAML::Node& subLocations = newNode["Subcane Locations"];
	int subLocationCount = 0;
	for(YAML::Iterator it3 = subLocations.begin(); it3 != subLocations.end(); ++it3) {
		const YAML::Node& subCaneLocation = *it3;
		subCaneLocation[0] >> cane->subcaneLocations[subLocationCount].x;
		subCaneLocation[1] >> cane->subcaneLocations[subLocationCount].y;
		subCaneLocation[2] >> cane->subcaneLocations[subLocationCount].z;
		subLocationCount++;
	}

	newNode["RGBA Color"][0] >> cane->color.r;
	newNode["RGBA Color"][1] >> cane->color.g;
	newNode["RGBA Color"][2] >> cane->color.b;
	newNode["RGBA Color"][3] >> cane->color.a;

	const YAML::Node& subCanes = newNode["Subcanes"];
	int subCaneCount = 0;
	for(YAML::Iterator it4 = subCanes.begin(); it4 != subCanes.end(); ++it4) {
		const YAML::Node& subCane = *it4;
		Cane* loadCane = new Cane(UNASSIGNED_CANETYPE);
		parseCaneFromYAML(subCane, loadCane);
		cane->subcanes[subCaneCount] = loadCane;
		subCaneCount++;
	}
}

vector<Cane*> loadCanesFromFile(QString fileName)
{
	std::ifstream fin(fileName.toStdString().c_str());
	YAML::Parser parser(fin);

	YAML::Node doc;
	parser.GetNextDocument(doc);
	parser.GetNextDocument(doc);

	vector<Cane*> canes;
	canes.clear();
	for(unsigned i = 0; i < doc.size(); ++i) {
		Cane* cane = new Cane(UNASSIGNED_CANETYPE);
		parseCaneFromYAML(doc[i], cane);
		canes.push_back(cane);
	}

	return canes;
}

