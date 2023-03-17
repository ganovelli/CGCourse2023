#pragma once
#ifndef  STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#endif

struct texture {
	texture() { data = (unsigned char *)0; }
	~texture() {  }
	unsigned char * data;
	int x_size, y_size;
	int n_components;
	GLuint id;
	GLuint load(std::string name, GLuint tu) {
		data = stbi_load(name.c_str(), &x_size, &y_size, &n_components, 0);
		stbi__vertical_flip(data, x_size, y_size, n_components);
		glActiveTexture(GL_TEXTURE0 + tu);
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);
		int channels;
		switch (n_components) {
		case 1: channels = GL_RED; break;
		case 3: channels = GL_RGB; break;
		case 4: channels = GL_RGBA; break;
		default: assert(0);
		}
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x_size, y_size, 0, channels, GL_UNSIGNED_BYTE, data);
		stbi_image_free(data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glGenerateMipmap(GL_TEXTURE_2D);
		return id;
	}
};