
#include "geometry.h"
#include "cane.h"

#include <iostream>
#include <fstream>
#include <unistd.h>

using std::vector;


//compute normals by using area-weighted normals of adjacent triangles.
//this may not be the correct approximation, but I just wanted some sort of hack
//to hold us until proper normal transforms could be written.
void Geometry::compute_normals_from_triangles() {
	assert(valid());
	vector< Vector3f > norms(vertices.size(), make_vector(0.0f, 0.0f, 0.0f));
	for (vector< Triangle >::const_iterator tri = triangles.begin(); tri != triangles.end(); ++tri) {
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
	for (vector< Vertex >::const_iterator v = vertices.begin(); v != vertices.end(); ++v) {
		file << "vn " << v->normal.x << " "
			 << v->normal.y << " "
			 << v->normal.z << "\n";
		file << "v " << v->position.x << " "
			 << v->position.y << " "
			 << v->position.z << "\n";
	}
	for (vector< Triangle >::const_iterator t = triangles.begin(); t != triangles.end(); ++t) {
		file << "f " << (t->v1+1) << "//" << (t->v1+1) << " "
			 << (t->v2+1) << "//" << (t->v2+1) << " "
			 << (t->v3+1) << "//" << (t->v3+1) << "\n";
	}
}

void Geometry::save_raw_file(std::string const &filename) const {
	unlink(filename.c_str()); //remove file, if it exists
	{ //write to temp file:
		std::ofstream file((filename + ".temp").c_str(), std::ios::binary);

		file.write("RCG1", 4); //magic
		{ //count of groups:
			uint32_t size = groups.size();
			file.write(reinterpret_cast< const char * >(&size), 4);
		}

		for (vector< Group >::const_iterator g = groups.begin(); g != groups.end(); ++g) {
			vector< Vertex > group_verts;
			vector< uint32_t > vert_to_group(vertices.size(), -1U);
			assert(g->triangle_begin + g->triangle_size <= triangles.size());
			vector< Triangle > group_tris(triangles.begin() + g->triangle_begin, triangles.begin() + g->triangle_begin + g->triangle_size);
			assert(group_tris.size() == g->triangle_size);
			//Translate group_triangles indices:
			for (vector< Triangle >::iterator t = group_tris.begin(); t != group_tris.end(); ++t) {
				assert(t->v1 < vertices.size());
				assert(t->v2 < vertices.size());
				assert(t->v3 < vertices.size());
				if (vert_to_group[t->v1] >= group_verts.size()) {
					vert_to_group[t->v1] = group_verts.size();
					group_verts.push_back(vertices[t->v1]);
				}
				t->v1 = vert_to_group[t->v1];
				if (vert_to_group[t->v2] >= group_verts.size()) {
					vert_to_group[t->v2] = group_verts.size();
					group_verts.push_back(vertices[t->v2]);
				}
				t->v2 = vert_to_group[t->v2];
				if (vert_to_group[t->v3] >= group_verts.size()) {
					vert_to_group[t->v3] = group_verts.size();
					group_verts.push_back(vertices[t->v3]);
				}
				t->v3 = vert_to_group[t->v3];
				assert(t->v1 < group_verts.size());
				assert(t->v2 < group_verts.size());
				assert(t->v3 < group_verts.size());
			}
			file.write("COL0",4);
			{ //write group color:
				assert(g->cane);
				//Glass has diffuse skin coat over absorbing inner coat.
				Vector4f skin_color = make_vector(g->cane->color.xyz, g->cane->color.w);
				//Filter-per-unit-length:
				Vector3f filter_after_unit = g->cane->color.xyz;
				//intensify color:
				for (unsigned int i = 0; i < 3; ++i) {
					filter_after_unit.c[i] = powf(filter_after_unit.c[i], 20.0f);
				}
				file.write(reinterpret_cast< const char * >(&skin_color), sizeof(Vector4f));
				file.write(reinterpret_cast< const char * >(&filter_after_unit), sizeof(Vector3f));
			}

			file.write("VER0",4);
			//write group geometry:
			{ //count of verts:
				uint32_t size = group_verts.size();
				file.write(reinterpret_cast< const char * >(&size), 4);
			}
			//Verts should just be six floats:
			assert(sizeof(Vertex) == 4 * (3 + 3));
			//write verts:
			file.write(reinterpret_cast< const char *>(&(group_verts[0])), sizeof(Vertex) * group_verts.size());
			file.write("TRI0",4);
			{ //count of triangles:
				uint32_t size = group_tris.size();
				file.write(reinterpret_cast< const char * >(&size), 4);
			}
			//Triangles should be 3 32-bit unsigned integer inds:
			assert(sizeof(Triangle) == 4 * 3);
			//write triangles:
			file.write(reinterpret_cast< const char *>(&(group_tris[0])), sizeof(Triangle) * group_tris.size());
		}

	}
	//rename temp file to desired name:
	rename((filename + ".temp").c_str(), filename.c_str());
}
