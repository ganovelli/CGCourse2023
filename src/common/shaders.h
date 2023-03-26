#pragma once

#include <GL/glew.h>
#include <stdio.h>
#include <string>
#include <sstream>
#include <iostream>
#include <map>
#include <fstream>
#include <regex>
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

		void bind_uniform_variables(std::string code) {
			
			code.erase(std::remove(code.begin(), code.end(), '\n'), code.end());
			code.erase(std::remove(code.begin(), code.end(), '\t'), code.end());
			code.erase(std::remove(code.begin(), code.end(), '\b'), code.end());

			int pos;
			std::istringstream check1(code);

			std::string intermediate;
			std::vector <std::string> tokens;
			// Tokenizing w.r.t. space ' '
			while (getline(check1, intermediate, ';'))
			{
				std::regex_replace(intermediate, std::regex("  "), " ");

				if (intermediate.find(" ") == 0)
					intermediate.erase(0, 1);

				if (intermediate.find("uniform") == 0) {
					pos = intermediate.find_last_of(" ");
					std::string uniform_name = intermediate.substr(pos+1, intermediate.length() - pos);
					this->bind(uniform_name);
					tokens.push_back(intermediate.substr(pos+1, intermediate.length() - pos));
				}
			}
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

			bind_uniform_variables(vs_src_code);
			bind_uniform_variables(fs_src_code);

			check_shader(vs);
			check_shader(fs);
			validate_shader_program(pr);
		}

};

 
