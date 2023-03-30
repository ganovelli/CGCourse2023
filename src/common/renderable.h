#pragma once
#include <GL/glew.h>
#include <vector>
#include <glm/glm.hpp>  
#include "texture.h"

struct box3
{
	/// min coordinate point
	glm::vec3 min;

	/// max coordinate point
	glm::vec3 max;

	/// The bounding box constructor (make it of size s centered in 0^3
	inline  box3(float s) { min =  glm::vec3(-s/2.f); max = glm::vec3(s / 2.f); }

	/// The bounding box constructor (make it empty)
	inline  box3() { min = glm::vec3(1.f); max = glm::vec3(-1.f); }

	/// Min Max constructor
	inline  box3(const glm::vec3& mi, const glm::vec3& ma) { min = mi; max = ma; }

	/// The bounding box distructor
	inline ~box3() { }

	/** Modify the current bbox to contain also the passed point
	*/
	void add(const glm::vec3& p)
	{
		if (min.x > max.x) { min = max = p; }
		else
		{
			if (min.x > p.x) min.x = p.x;
			if (min.y > p.y) min.y = p.y;
			if (min.z > p.z) min.z = p.z;

			if (max.x < p.x) max.x = p.x;
			if (max.y < p.y) max.y = p.y;
			if (max.z < p.z) max.z = p.z;
		}
	}

	bool is_empty() const { return min == max; }

	float diagonal() const
	{
		return glm::length(min - max);
	}

	glm::vec3 center() const
	{
		return (min + max) * 0.5f;
	}

};


struct material {
	std::string name;

	float  ambient[3];
	float  diffuse[3];
	float  specular[3];
	float  transmittance[3];
	float  emission[3];
	float  shininess;
	float  ior;       // index of refraction
	float  dissolve;  // 1 == opaque; 0 == fully transparent

	texture ambient_texture,diffuse_texture,specular_texture;
};

struct renderable {

	struct element_array {
		GLuint ind, elem_type, count;
		element_array() { }
	};

	// vertex array object
	GLuint vao;

	// vertex buffer objects
	std::vector<GLuint> vbos;

	// element array
	GLuint ind;

	// vector of element array
	std::vector<element_array > inds;

	// element arrays (backward compatible implementation)

	// primitive type
	unsigned int elem_type;

	// number of vertices
	unsigned int vn;

	// number of indices
	unsigned int in;

	// bounding box of the shape
	box3 bbox;

	material mtl;

	void create() {
		glGenVertexArrays(1, &vao);
	}

	void bind() {
		glBindVertexArray(vao);
	}

	GLuint assign_vertex_attribute(unsigned int va_id, unsigned int count,
		unsigned int attribute_index,
		unsigned int num_components,
		unsigned int TYPE,
		unsigned int stride = 0,
		unsigned int offset = 0) {

		vn = count;

		glBindVertexArray(vao);

		/* create a buffer for the render data in video RAM */
		vbos.push_back(va_id);
		
		glBindBuffer(GL_ARRAY_BUFFER, vbos.back());
		glEnableVertexAttribArray(attribute_index);

		/* specify the data format */
		glVertexAttribPointer(attribute_index, num_components, TYPE, false, stride, (void*)(size_t)offset);

		glBindVertexArray(NULL);
		return vbos.back();
	}


	template <class T>
	GLuint add_vertex_attribute(T* values, unsigned int count,
		unsigned int attribute_index,
		unsigned int num_components,
		unsigned int TYPE,
		unsigned int stride = 0,
		unsigned int offset = 0) {

		vn = count;

		glBindVertexArray(vao);

		/* create a buffer for the render data in video RAM */
		vbos.push_back(0);
		glGenBuffers(1, &vbos.back());

		glBindBuffer(GL_ARRAY_BUFFER, vbos.back());

		/* declare what data in RAM are filling the buffering video RAM */
		glBufferData(GL_ARRAY_BUFFER, sizeof(T) * count, values, GL_STATIC_DRAW);
		glEnableVertexAttribArray(attribute_index);

		/* specify the data format */
		glVertexAttribPointer(attribute_index, num_components, TYPE, false, stride, (void*)(size_t)offset);

		glBindVertexArray(NULL);
		return vbos.back();
	}

	template <class T>
	GLuint add_vertex_attribute(const T* values, unsigned int count,
		unsigned int attribute_index,
		unsigned int num_components,
		unsigned int stride = 0,
		unsigned int offset = 0) { }


	GLuint add_indices(unsigned int* indices, unsigned int count, unsigned int ELEM_TYPE) {
		std::cout << "deprecated (but working) : use add_element_array" << std::endl;
		add_element_array(indices, count, ELEM_TYPE);
		in = count;
		elem_type = ELEM_TYPE;
		ind = inds.back().ind;
		return ind;
	};

	GLuint add_element_array(void* indices, unsigned int count, unsigned int ELEM_TYPE) {
		inds.push_back(element_array());
		glBindVertexArray(vao);
		glGenBuffers(1, &inds.back().ind);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, inds.back().ind);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * count, indices, GL_STATIC_DRAW);
		glBindVertexArray(NULL);
		inds.back().elem_type = ELEM_TYPE;
		inds.back().count = count;
		return inds.back().ind;
	};

};

template <>
GLuint renderable::add_vertex_attribute(const float* values, unsigned int count,
	unsigned int attribute_index,
	unsigned int num_components,
	unsigned int stride,
	unsigned int offset) {
	return this->add_vertex_attribute(values, count, attribute_index, num_components, (unsigned int)GL_FLOAT, stride, offset);
}

