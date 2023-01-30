#pragma once
#include <GL/glew.h>
#include <vector>

struct renderable {
	// vertex array object
	GLuint vao;

	// vertex buffer objects
	std::vector<GLuint> vbos;

	// element array
	GLuint ind;

	// primitive type
	unsigned int elem_type;
	
	// program shader
	GLuint sp;

	void create() {
		glGenVertexArrays(1, &vao);
	}

	void bind() {
		glBindVertexArray(vao);
	}

	template <class T>
	GLuint add_vertex_attribute(T* values, unsigned int count,
		unsigned int attribute_index,
		unsigned int num_components,
		unsigned int TYPE,
		unsigned int stride = 0,
		unsigned int offset = 0) {

		glBindVertexArray(vao);

		/* create a buffer for the render data in video RAM */
		vbos.push_back(0);
		glGenBuffers(1, &vbos.back());

		glBindBuffer(GL_ARRAY_BUFFER, vbos.back());

		/* declare what data in RAM are filling the buffering video RAM */
		glBufferData(GL_ARRAY_BUFFER, sizeof(T) * count, values, GL_STATIC_DRAW);
		glEnableVertexAttribArray(attribute_index);

		/* specify the data format */
		glVertexAttribPointer(attribute_index, num_components, TYPE, false, stride,(void*)  (size_t) offset  );

		glBindVertexArray(NULL);
		return vbos.back();
	}

	template <class T>
	GLuint add_vertex_attribute(	T * values,unsigned int count,
							unsigned int attribute_index,
							unsigned int num_components,
							unsigned int stride = 0,
							unsigned int offset = 0){ }

	GLuint add_indices(unsigned int * indices, unsigned int count, unsigned int ELEM_TYPE) {
		glBindVertexArray(vao);
		glGenBuffers(1, &ind);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ind);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * count, indices, GL_STATIC_DRAW);
		glBindVertexArray(NULL);
		elem_type = ELEM_TYPE;
		return ind;
	};
};

template <>
GLuint renderable::add_vertex_attribute(float * values, unsigned int count,
	unsigned int attribute_index,
	unsigned int num_components,
	unsigned int stride,
	unsigned int offset) { 
	return this->add_vertex_attribute(values, count, attribute_index, num_components, (unsigned int) GL_FLOAT, stride, offset);
}