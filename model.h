

#ifndef MODEL_H
#define MODEL_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <vector>
#include <string>
#include "cane.h"
#include "constants.h"
#include "primitives.h"

// Mesh stuff
class Vertex
{
	public:
	Vertex(Point const &_position = make_vector(0.0f, 0.0f, 0.0f), Point const &_normal = make_vector(0.0f, 0.0f, 0.0f), Color const &_color = make_vector(1.0f, 1.0f, 1.0f, 1.0f)) : position(_position), normal(_normal), color(_color) {
	}
	Point position;
	Point normal;
	Color color;
};
class Triangle
{
	public:
	Triangle(uint32_t _v1 = -1U, uint32_t _v2 = -1U, uint32_t _v3 = -1U) : v1(_v1), v2(_v2), v3(_v3) {
	}
	uint32_t v1;
	uint32_t v2;
	uint32_t v3;
};

class Geometry
{
	public:
	std::vector< Vertex > vertices;
	std::vector< Triangle > triangles;
	void clear() {
		vertices.clear();
		triangles.clear();
	}
	bool valid() const {
		for (std::vector< Triangle >::const_iterator t = triangles.begin(); t != triangles.end(); ++t) {
			if (t->v1 >= vertices.size()) return false;
			if (t->v2 >= vertices.size()) return false;
			if (t->v3 >= vertices.size()) return false;
		}
		return true;
	}
	void compute_normals_from_triangles();
	void save_obj_file(std::string const &filename) const;
};

class Model
{

	public:
		Model(Cane* c);
		void setCane(Cane* c);
		Cane* getCane();
		Geometry* getGeometry(int resolution);
		int getNumMeshTriangles(int resolution);
		void twistAndStretchCane(float twistAmount, float stretchFactor);
		void moveCane(float delta_x, float delta_y);
		void flattenCane(float rectangle_ratio, float rectangle_theta, float flatness);
		void addCane(Cane* c);
		void advanceActiveSubcane();
		void startMoveMode();
		Cane* getActiveSubcane();
		void saveObjFile(std::string const &filename);

	private:
		Cane *cane;
		Geometry lowResGeometry;
		Geometry highResGeometry;
		int lowResDataUpToDate;
		int highResDataUpToDate;
		int activeSubcane;
		//Point* activePoints;

		void generateMesh(Cane* c, Geometry *geometry, 
			Cane** ancestors, int* ancestorCount, int resolution, bool isActive);
		static void meshCircularBaseCane(Geometry *geometry, 
			Cane** ancestors, int ancestorCount, Color color, int resolution);
		static float computeTotalStretch(Cane** ancestors, int ancestorCount);
		static void applyFlattenTransform(Vertex* v, float rectangleRatio,
		float rectangleTheta, float flatness);
		static void applyBundleTransform(Vertex* v, Point location);
		static void applyStretchTransform(Vertex* v, float amount);
		static void applyTwistTransform(Vertex* v, float amount);
		static Vertex applyTransforms(Vertex p, Cane** ancestors, int ancestorCount);
		void updateLowResData();
		void updateHighResData();
		float matrixDeterminant3by3(Point a,Point b,Point c);
		float intersectTriangle(Triangle plane,Point camera,Point camDir,float tCurrent);
};

#endif

