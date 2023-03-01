#pragma once


#include <tiny_obj_loader.h>
#include "renderable.h"
#include <glm/glm.hpp>  
#include <glm/ext.hpp>  
#include <glm/gtx/string_cast.hpp>

#include <iostream>

static void load_obj(std::vector<renderable> & rs, std::string inputfile) {

	tinyobj::ObjReaderConfig reader_config = tinyobj::ObjReaderConfig();
	reader_config.mtl_search_path = "./"; // Path to material files

	tinyobj::ObjReader reader;

	if (!reader.ParseFromFile(inputfile, reader_config)) {
		if (!reader.Error().empty()) {
			std::cerr << "TinyObjReader: " << reader.Error();
		}
		exit(1);
	}

	if (!reader.Warning().empty()) {
		std::cout << "TinyObjReader: " << reader.Warning();
	}

	auto attrib = reader.GetAttrib();
	auto shapes = reader.GetShapes();
	auto materials = reader.GetMaterials();


	rs.resize(shapes.size());
	// create the vertex attributes
	std::vector<tinyobj::real_t>   pos = attrib.GetVertices();
	std::vector<tinyobj::real_t>   norms = attrib.normals;
	std::vector<tinyobj::real_t>   v_norm,v_pos;


	if (!norms.empty()) {
		unsigned int nv = pos.size() / 3;
		std::vector<int> norm_id, skip_to, pos_id;
		norm_id.resize(nv, -1);
		pos_id.resize(nv, 0);
		for (unsigned int ii = 0; ii < pos_id.size(); ii++) pos_id[ii] = ii;

		skip_to.resize(nv, -1);

		for (unsigned int is = 0; is < shapes.size(); ++is) {
			std::vector<unsigned int> inds;
			for (size_t f = 0; f < shapes[is].mesh.indices.size(); f++)
			{
				tinyobj::index_t &  id = shapes[is].mesh.indices[f];
				int vi = id.vertex_index;
				while (skip_to[vi] != -1 && (norm_id[vi] != -1) && (norm_id[vi] != id.normal_index)) {
					vi = skip_to[vi];
				}
				if ((norm_id[vi] == id.normal_index)) { // a vertex with the right normal is found
					id.vertex_index = vi;
					norm_id[vi] = id.normal_index;
				}
				else {
					if (norm_id[vi] == -1) { // a vertex with unassigned normal is found
						id.vertex_index = vi;
						norm_id[vi] = id.normal_index;
					}
					else {// a new vertex must be added
						assert(skip_to[vi] == -1);
						skip_to[vi] = pos_id.size();
						skip_to.push_back(-1);
						pos_id.push_back(pos_id[id.vertex_index]); // make a new vertex with the same position..
						id.vertex_index = pos_id.size() - 1;
						norm_id.push_back(id.normal_index);		 // ..and the new normal		
					}
				}
			}
		}

		// check
		for (unsigned int is = 0; is < shapes.size(); ++is) {
			std::vector<unsigned int> inds;
			for (size_t f = 0; f < shapes[is].mesh.indices.size(); f++)
			{
				assert(shapes[is].mesh.indices[f].vertex_index >= 0);
				assert(shapes[is].mesh.indices[f].vertex_index < pos_id.size());
				assert(shapes[is].mesh.indices[f].normal_index >= 0);
				assert(norm_id[shapes[is].mesh.indices[f].normal_index] >= 0);
			}
		}

		for (unsigned int i = 0; i < pos_id.size(); ++i) {
			v_pos.push_back(pos[pos_id[i] * 3]);
			v_pos.push_back(pos[pos_id[i] * 3 + 1]);
			v_pos.push_back(pos[pos_id[i] * 3 + 2]);
		}

		for (unsigned int i = 0; i < pos_id.size(); ++i) {
			v_norm.push_back(norms[norm_id[i] * 3]);
			v_norm.push_back(norms[norm_id[i] * 3 + 1]);
			v_norm.push_back(norms[norm_id[i] * 3 + 2]);
		}
	}
	else
		v_pos = pos; // silly: fix the wasteful copy

	// compute the bounding box of the vertices
	box3 bbox;
	for (unsigned int i = 0; i < v_pos.size() / 3; ++i)
		bbox.add(glm::vec3(v_pos[3 * i], v_pos[3 * i + 1], v_pos[3 * i + 2]));



    // create the vertex array buffers and share them between all renderables
    rs[0].create();
    rs[0].add_vertex_attribute(&v_pos[0], (unsigned int)v_pos.size(), 0, 3);
    rs[0].bbox = bbox;


	if (!v_norm.empty())
		rs[0].add_vertex_attribute(&v_norm[0], (unsigned int)v_norm.size(), 2, 3);


    for (unsigned int  i = 1; i < shapes.size();++i) {
        rs[i].create();
        rs[i].assign_vertex_attribute(rs[0].vbos[0], (unsigned int)v_pos.size() / 3, 0, 3,GL_FLOAT);

		if (!v_norm.empty())
			rs[i].assign_vertex_attribute(rs[0].vbos[1], (unsigned int)v_norm.size() / 3, 2, 3, GL_FLOAT);
	}

    for (unsigned int is = 0; is < shapes.size();++is) { 
        std::vector<unsigned int> inds;
         for (size_t f = 0; f < shapes[is].mesh.indices.size(); f++) 
             inds.push_back(static_cast<unsigned int>(shapes[is].mesh.indices[f].vertex_index));
        rs[is].add_element_array(&inds[0], shapes[is].mesh.indices.size(), GL_TRIANGLES);
    }

    // Loop over shapes
    //for (size_t s = 0; s < shapes.size(); s++) {
    //    // Loop over faces(polygon)
    //    size_t index_offset = 0;
    //    for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
    //        size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);

    //        // Loop over vertices in the face.
    //        for (size_t v = 0; v < fv; v++) {
    //            // access to vertex
    //            tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
    //            tinyobj::real_t vx = attrib.vertices[3 * size_t(idx.vertex_index) + 0];
    //            tinyobj::real_t vy = attrib.vertices[3 * size_t(idx.vertex_index) + 1];
    //            tinyobj::real_t vz = attrib.vertices[3 * size_t(idx.vertex_index) + 2];

    //            // Check if `normal_index` is zero or positive. negative = no normal data
    //            if (idx.normal_index >= 0) {
    //                tinyobj::real_t nx = attrib.normals[3 * size_t(idx.normal_index) + 0];
    //                tinyobj::real_t ny = attrib.normals[3 * size_t(idx.normal_index) + 1];
    //                tinyobj::real_t nz = attrib.normals[3 * size_t(idx.normal_index) + 2];
    //            }

    //            // Check if `texcoord_index` is zero or positive. negative = no texcoord data
    //            if (idx.texcoord_index >= 0) {
    //                tinyobj::real_t tx = attrib.texcoords[2 * size_t(idx.texcoord_index) + 0];
    //                tinyobj::real_t ty = attrib.texcoords[2 * size_t(idx.texcoord_index) + 1];
    //            }

    //            // Optional: vertex colors
    //            // tinyobj::real_t red   = attrib.colors[3*size_t(idx.vertex_index)+0];
    //            // tinyobj::real_t green = attrib.colors[3*size_t(idx.vertex_index)+1];
    //            // tinyobj::real_t blue  = attrib.colors[3*size_t(idx.vertex_index)+2];
    //        }
    //        index_offset += fv;

    //        // per-face material
    //        shapes[s].mesh.material_ids[f];
    //    }
    //}



}
