
#include "geometry.h"

//compute normals by using area-weighted normals of adjacent triangles.
//this may not be the correct approximation, but I just wanted some sort of hack
//to hold us until proper normal transforms could be written.
void Geometry::compute_normals_from_triangles() {
	assert(valid());
	std::vector< Vector3f > norms(vertices.size(), make_vector(0.0f, 0.0f, 0.0f));
	for (std::vector< Triangle >::const_iterator tri = triangles.begin(); tri != triangles.end(); ++tri) {
		Vector3f p1 = vertices[tri->v1].position;
		Vector3f p2 = vertices[tri->v2].position;
		Vector3f p3 = vertices[tri->v3].position;
		norms[tri->v1] += cross_product(p2 - p1, p3 - p1);
		norms[tri->v2] += cross_product(p3 - p2, p1 - p2);
		norms[tri->v3] += cross_product(p1 - p3, p2 - p3);
	}
	for (unsigned int v = 0; v < norms.size(); ++v) {
		if (norms[v] != make_vector(0.0f, 0.0f, 0.0f)) {
			vertices[v].normal = normalize(norms[v]);
		}
	}
}



void Geometry::save_obj_file(std::string const &filename) const {
	std::ofstream file(filename.c_str());
	for (std::vector< Vertex >::const_iterator v = vertices.begin(); v != vertices.end(); ++v) {
		file << "vn " << v->normal.x << " "
			 << v->normal.y << " "
			 << v->normal.z << "\n";
		file << "v " << v->position.x << " "
			 << v->position.y << " "
			 << v->position.z << "\n";
	}
	for (std::vector< Triangle >::const_iterator t = triangles.begin(); t != triangles.end(); ++t) {
		file << "f " << (t->v1+1) << "//" << (t->v1+1) << " "
			 << (t->v2+1) << "//" << (t->v2+1) << " "
			 << (t->v3+1) << "//" << (t->v3+1) << "\n";
	}
}


