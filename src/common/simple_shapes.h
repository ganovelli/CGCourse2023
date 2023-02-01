#pragma once
#include <vector>
#include "renderable.h"

struct shape {
	std::vector<float> positions;
	std::vector<float> colors;
	std::vector<unsigned int> indices;
	unsigned int vn, fn;

	void to_renderable(renderable & r) {
		r.create();
		r.add_vertex_attribute<float>(&positions[0], 3*vn, 0, 3);
		if(!colors.empty())
			r.add_vertex_attribute<float>(&colors[0], 3 * vn, 1, 3);
		if(!indices.empty())
			r.add_indices(&indices[0], fn * 3, GL_TRIANGLES);
	}

};

struct shape_maker {
	 
	 static renderable cube(float r = 0.5, float g = 0.5, float b = 0.5) {
		shape  s;
		// vertices definition
		////////////////////////////////////////////////////////////
		s.positions = {
					   -1.0, -1.0, 1.0,
					   1.0, -1.0, 1.0,
					   -1.0, 1.0, 1.0,
					   1.0, 1.0, 1.0,
					   -1.0, -1.0, -1.0,
					   1.0, -1.0, -1.0,
					   -1.0, 1.0, -1.0,
					   1.0, 1.0, -1.0
		};
		s.colors = {
			r,g,b,
			r,g,b,
			r,g,b,
			r,g,b,
			r,g,b,
			r,g,b,
			r,g,b,
			r,g,b		};
		// triangles definition
		////////////////////////////////////////////////////////////

		s.indices = {
					   0, 1, 2, 2, 1, 3,  // front
					   5, 4, 7, 7, 4, 6,  // back
					   4, 0, 6, 6, 0, 2,  // left
					   1, 5, 3, 3, 5, 7,  // right
					   2, 3, 6, 6, 3, 7,  // top
					   4, 5, 0, 0, 5, 1   // bottom
		};
		s.vn = 8;
		s.fn = 12;

		renderable res;
		s.to_renderable(res);
		return res;
	}

	 static renderable frame(float scale  = 1.f) {
		 shape  s;
		 // vertices definition
		 ////////////////////////////////////////////////////////////
		 s.positions = {
			 0.0,0.0,0.0,
			 1.0,0.0,0.0,
			 0.0,0.0,0.0,
			 0.0,1.0,0.0,
			 0.0,0.0,0.0,
			 0.0,0.0,1.0 
		 };
		 for (int i = 0; i < 18; ++i)
			 s.positions[i] *= scale;

		 s.colors = {
			 1.0,0.0,0.0,
			 1.0,0.0,0.0,
			 0.0,1.0,0.0,
			 0.0,1.0,0.0,
			 0.0,0.0,1.0,
			 0.0,0.0,1.0
		 };

		 // LINES definition
		 ////////////////////////////////////////////////////////////
		 s.vn = 6;
		 renderable res;
		 s.to_renderable(res);
		 return res;
	 }

	 static renderable cylinder(int resolution, float r = 0.5, float g = 0.5, float b = 0.5){
		 // vertices definition
		 ////////////////////////////////////////////////////////////
		 shape s;
		 s.positions.resize(3 * (2 * resolution + 2));

		 float radius = 1.0;
		 float angle;
		 float step = 6.283185307179586476925286766559 / resolution;

		 // lower circle
		 int vertexoffset = 0;
		 for (int i = 0; i < resolution; i++) {

			 angle = -step * i;

			 s.positions[vertexoffset] = radius * std::cos(angle);
			 s.positions[vertexoffset + 1] = 0.0;
			 s.positions[vertexoffset + 2] = radius * std::sin(angle);
			 vertexoffset += 3;
		 }

		 // upper circle
		 for (int i = 0; i < resolution; i++) {

			 angle = -step * i;

			 s.positions[vertexoffset] = radius *  std::cos(angle);
			 s.positions[vertexoffset + 1] = 2.0;
			 s.positions[vertexoffset + 2] = radius *  std::sin(angle);
			 vertexoffset += 3;
		 }

		 s.positions[vertexoffset] = 0.0;
		 s.positions[vertexoffset + 1] = 0.0;
		 s.positions[vertexoffset + 2] = 0.0;
		 vertexoffset += 3;

		 s.positions[vertexoffset] = 0.0;
		 s.positions[vertexoffset + 1] = 2.0;
		 s.positions[vertexoffset + 2] = 0.0;

		 for (int i = 0; i < s.positions.size(); i += 3) {
			 s.colors.push_back(r);
			 s.colors.push_back(g);
			 s.colors.push_back(b);
		 }

		 // triangles definition
		 ////////////////////////////////////////////////////////////

		 s.indices.resize(3 * 4 * resolution);

		 // lateral surface
		 int triangleoffset = 0;
		 for (int i = 0; i < resolution; i++)
		 {
			 s.indices[triangleoffset] = i;
			 s.indices[triangleoffset + 1] = (i + 1) % resolution;
			 s.indices[triangleoffset + 2] = (i % resolution) + resolution;
			 triangleoffset += 3;

			 s.indices[triangleoffset] = (i % resolution) + resolution;
			 s.indices[triangleoffset + 1] = (i + 1) % resolution;
			 s.indices[triangleoffset + 2] = ((i + 1) % resolution) + resolution;
			 triangleoffset += 3;
		 }

		 // bottom of the cylinder
		 for (int i = 0; i < resolution; i++)
		 {
			 s.indices[triangleoffset] = i;
			 s.indices[triangleoffset + 1] = (i + 1) % resolution;
			 s.indices[triangleoffset + 2] = 2 * resolution;
			 triangleoffset += 3;
		 }

		 // top of the cylinder
		 for (int i = 0; i < resolution; i++)
		 {
			 s.indices[triangleoffset] = resolution + i;
			 s.indices[triangleoffset + 1] = ((i + 1) % resolution) + resolution;
			 s.indices[triangleoffset + 2] = 2 * resolution + 1;
			 triangleoffset += 3;
		 }

		 s.vn = s.positions.size()  / 3;
		 s.fn = s.indices.size() / 3;

		 renderable res;
		 s.to_renderable(res);
		 return res;
	 }


	};