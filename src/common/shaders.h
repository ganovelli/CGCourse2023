#pragma once

#include <GL/glew.h>
#include <stdio.h>
#include <string>
#include <map>
#include <fstream>
#include "../common/debugging.h"

struct shader{
        GLuint   vs, gs, fs, pr;

        std::map<std::string,int> uni;

        void bind(std::string name){
            uni[name] = glGetUniformLocation(pr, name.c_str());
        }

        int operator[](std::string name){
            return uni[name];
        }

        static  std::string textFileRead(const char *fn) {
		std::ifstream ifs(fn);
		std::string content((std::istreambuf_iterator<char>(ifs)),
			(std::istreambuf_iterator<char>()));
		if (content.empty()) {
			std::cout << "No content for " << fn << std::endl;
			exit(0);
		}
		return content;
		}

		bool create_shader(const GLchar* src, unsigned int SHADER_TYPE) {
			GLuint s;
			switch (SHADER_TYPE) {
			case GL_VERTEX_SHADER:   s = vs = glCreateShader(GL_VERTEX_SHADER);break;
			case GL_FRAGMENT_SHADER: s = fs = glCreateShader(GL_FRAGMENT_SHADER);break;
			}  

			glShaderSource(s, 1, &src, NULL);
			glCompileShader(s);
			int status;
			glGetShaderiv(s, GL_COMPILE_STATUS, &status);
			if (status != GL_TRUE) {
				check_shader(s);
				return false;
			}
			return true;
		}


        void  create_program( const GLchar *nameV, const char *nameF){
		
			std::string vs_src_code  = textFileRead(nameV);
			std::string  fs_src_code = textFileRead(nameF);

			create_shader(vs_src_code.c_str(), GL_VERTEX_SHADER);
			create_shader(fs_src_code.c_str(), GL_FRAGMENT_SHADER);

			pr = glCreateProgram();
			glAttachShader(pr,vs);
			glAttachShader(pr,fs);

			glLinkProgram(pr);
	}

};

 
