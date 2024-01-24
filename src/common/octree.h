#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "aabb.h"



struct octree {

	struct rgbai {
		rgbai() {}
		rgbai(int v) { rgba[0] = v;}
		rgbai(glm::ivec3 v) { rgba[0] = v.x; rgba[1] = v.y; rgba[2] = v.z;}
		int rgba[4];
		bool operator <(rgbai o) const { return rgba[0] < o.rgba[0]; }
	};

	// nodes[i][0] = 0 -> empty leaf
	// nodes[i][0] = 1 -> internal node
	// nodes[i][0] >= 2 -> non-empty leaf, index +2 to the first triangle
	// nodes[i][1]    -> number of primitives
	std::vector<rgbai > nodes;
	std::vector<rgbai > triangles_id;

	AABB bbox;

	int n_levels;
	int n_non_empty_leaves, max_level;

	bool intersect(AABB a, glm::vec3 p0, glm::vec3 p1, glm::vec3 p2) {
		AABB b(p0, p1);
		b.extend(p2);
	return a.overlaps(b);
	//	return (a.intersect(b) == AABB::INSIDE);
		//	return a.intersect(b) == AABB::INSIDE;
	}

	AABB octant(int o, AABB a) {
		glm::vec3 c = a.getCenter();
		glm::vec3 min_, max_;
		min_.x = (o % 2 == 0) ?			a.getMin().x : c.x;
		min_.y = ((o / 2) % 2 == 0) ?	a.getMin().y : c.y;
		min_.z = ( o / 4 == 0) ?		a.getMin().z : c.z;

		max_.x = (o % 2 == 0) ?			c.x:a.getMax().x;
		max_.y = ((o / 2) % 2 == 0) ?	c.y:a.getMax().y;
		max_.z = (o / 4 == 0) ?			c.z:a.getMax().z;

		return AABB(min_, max_);
	}

	void set(int in, AABB b, int* tr_id, int n_tri, float* pos, int n_pos, int max_elem_in_leaf, int max_levs) {
	//	b.scale(glm::vec3(0.99), b.getCenter());
	//	printf("%f %f %f - %f %f %f \n",
	//		b.getMin().x, b.getMin().y, b.getMin().z, b.getDiagonal().x, b.getDiagonal().y, b.getDiagonal().z);
		int curr_lev = log(in+1) / log(8);
		if (max_level < curr_lev)
			max_level = curr_lev;

		std::vector<rgbai> intersected;
		for (int it = 0; it < n_tri; ++it) {
			glm::ivec3 id(tr_id[it * 4], tr_id[it * 4 + 1], tr_id[it * 4 + 2]);
			glm::vec3 p0(pos[id[0] * 4], pos[id[0] * 4 + 1], pos[id[0] * 4 + 2]);
			glm::vec3 p1(pos[id[1] * 4], pos[id[1] * 4 + 1], pos[id[1] * 4 + 2]);
			glm::vec3 p2(pos[id[2] * 4], pos[id[2] * 4 + 1], pos[id[2] * 4 + 2]);

			if (intersect(b, p0, p1, p2))
				intersected.push_back(rgbai(id));
		}

		if (intersected.empty()) // empty
		{
			nodes[in].rgba[0] = 0;
			nodes[in].rgba[1] = 0;
		}
		else
			if (intersected.size() < max_elem_in_leaf || log(in+1)/log(8) > max_levs ) { // a non empty leaf
				nodes[in].rgba[0] = triangles_id.size()+2;
				nodes[in].rgba[1] = intersected.size();
				triangles_id.insert(triangles_id.end(),intersected.begin(), intersected.end());
				n_non_empty_leaves++;
			}
			else
			{ // expand to children
				nodes[in].rgba[0] = 1;
				nodes[in].rgba[1] = 0;
				for (int o = 0; o < 8;++o)
					set(in * 8 + 1 + o, octant(o, b), tr_id, n_tri, pos, n_pos, max_elem_in_leaf, max_levs);
			}
	}

	// tr_id must be a vector of int as rgba quadruple, pos a vector of float as rgba float quadruple
	void set(int* tr_id, int n_tri, float * pos, int n_pos,int max_elem_in_leaf, int max_levs) {
		for (int i = 0; i < n_pos; ++i) {
			glm::vec3 p0(pos[i * 4], pos[i * 4+1], pos[i * 4+2]);
			bbox.extend(p0);
		}
		glm::vec3 d = bbox.getDiagonal();
		int il = 0;
		if(fabs(d[1]) > fabs(d[0])) il = 1;
		if (fabs(d[2]) > fabs(d[il])) il = 2;
		
	 	bbox.scale(glm::vec3(fabs(d[il]) / fabs(d[0]), fabs(d[il]) / fabs(d[1]), fabs(d[il]) / fabs(d[2])),
	 		bbox.getCenter());

		int size_linear_octree = (pow(8, max_levs + 1) - 1) / 7;
		nodes.resize(size_linear_octree );

		n_non_empty_leaves = 0;
		max_level = 0;
		set(0, bbox, tr_id,   n_tri,  pos,   n_pos,  max_elem_in_leaf, max_levs);

	}
};