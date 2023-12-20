#pragma once
#include <tinygltf/tiny_gltf.h>
#include "texture.h"

struct gltf_model {

	tinygltf::Model model;
	tinygltf::TinyGLTF loader;

	std::string err;
	std::string warn;

    std::vector<texture> positions;


    static std::string GetFilePathExtension(const std::string& FileName) {
        if (FileName.find_last_of(".") != std::string::npos)
            return FileName.substr(FileName.find_last_of(".") + 1);
        return "";
    }

    bool load(std::string input_filename) {
        std::string ext = GetFilePathExtension(input_filename);

        bool ret = false;
        if (ext.compare("glb") == 0) {
            // assume binary glTF.
            ret =
                loader.LoadBinaryFromFile(&model, &err, &warn, input_filename.c_str());
        }
        else {
            // assume ascii glTF.
            ret = loader.LoadASCIIFromFile(&model, &err, &warn, input_filename.c_str());
        }

        if (!warn.empty()) {
            printf("Warn: %s\n", warn.c_str());
        }

        if (!err.empty()) {
            printf("ERR: %s\n", err.c_str());
        }
        if (!ret) {
            printf("Failed to load .glTF : %s\n", input_filename.c_str());
            exit(-1);
        }
    }

    // take a model and fill the buffers to be passed to the compute shader (for ray tracing)
    bool create_buffers() {

        tinygltf::Mesh* mesh_ptr = 0;
        assert(model.scenes.size() > 0);

        // just look for the first mesh 
        int scene_to_display = model.defaultScene > -1 ? model.defaultScene : 0;
        const tinygltf::Scene& scene = model.scenes[scene_to_display];
        for (size_t i = 0; i < scene.nodes.size(); i++) {
            if(model.nodes[scene.nodes[i]].mesh > -1 )
                mesh_ptr = &model.meshes[model.nodes[scene.nodes[i]].mesh];
        }

        if (mesh_ptr == 0)
            return false;

        tinygltf::Mesh & mesh = *mesh_ptr;
        for (size_t i = 0; i < mesh.primitives.size(); i++) {
        const tinygltf::Primitive& primitive = mesh.primitives[i];

        if (primitive.indices < 0) return false;

        // Assume TEXTURE_2D target for the texture object.
        // glBindTexture(GL_TEXTURE_2D, gMeshState[mesh.name].diffuseTex[i]);

        std::map<std::string, int>::const_iterator it(primitive.attributes.begin());
        std::map<std::string, int>::const_iterator itEnd(primitive.attributes.end());

        for (; it != itEnd; it++) {
            assert(it->second >= 0);
            const tinygltf::Accessor& accessor = model.accessors[it->second];
            int n_chan = 1;
            if (accessor.type == TINYGLTF_TYPE_SCALAR) {
                n_chan = 1;
            }
            else if (accessor.type == TINYGLTF_TYPE_VEC2) {
                n_chan = 2;
            }
            else if (accessor.type == TINYGLTF_TYPE_VEC3) {
                n_chan = 3;
            }
            else if (accessor.type == TINYGLTF_TYPE_VEC4) {
                n_chan = 4;
            }
            else {
                assert(0);
            }
            // it->first would be "POSITION", "NORMAL", "TEXCOORD_0", ...
            if ((it->first.compare("POSITION") == 0) ||
                (it->first.compare("NORMAL") == 0) ||
                (it->first.compare("TEXCOORD_0") == 0)) {
                 
                // Compute byteStride from Accessor + BufferView combination.
                int byteStride =
                    accessor.ByteStride(model.bufferViews[accessor.bufferView]);
                assert(byteStride != -1);

     

                GLuint id;
                glGenTextures(1, &id);
                glBindTexture(GL_TEXTURE_2D, id);

                GLint internalformat;
                switch (n_chan) {
                case 1: internalformat = GL_RED; break;
                case 3: internalformat = GL_RGB; break;
                case 4: internalformat = GL_RGBA; break;
                default: assert(0);
                }
                GLint type;

                // one long texture, just a stub implementation
                glTexImage2D(GL_TEXTURE_2D, 0, internalformat, accessor.count, 1, 0, internalformat, accessor.componentType, NULL);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

                glBindImageTexture(0, id, 0, GL_FALSE, 0, GL_READ_ONLY, internalformat);


                //glVertexAttribPointer(gGLProgramState.attribs[it->first], size,
                //    accessor.componentType,
                //    accessor.normalized ? GL_TRUE : GL_FALSE,
                //    byteStride, BUFFER_OFFSET(accessor.byteOffset));
                //CheckErrors("vertex attrib pointer");
                //glEnableVertexAttribArray(gGLProgramState.attribs[it->first]);
                //CheckErrors("enable vertex attrib array");
                 
            }
        }

 /*       const tinygltf::Accessor& indexAccessor =
            model.accessors[primitive.indices];
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
            gBufferState[indexAccessor.bufferView].vb);
        CheckErrors("bind buffer");
        int mode = -1;
        if (primitive.mode == TINYGLTF_MODE_TRIANGLES) {
            mode = GL_TRIANGLES;
        }
        else if (primitive.mode == TINYGLTF_MODE_TRIANGLE_STRIP) {
            mode = GL_TRIANGLE_STRIP;
        }
        else if (primitive.mode == TINYGLTF_MODE_TRIANGLE_FAN) {
            mode = GL_TRIANGLE_FAN;
        }
        else if (primitive.mode == TINYGLTF_MODE_POINTS) {
            mode = GL_POINTS;
        }
        else if (primitive.mode == TINYGLTF_MODE_LINE) {
            mode = GL_LINES;
        }
        else if (primitive.mode == TINYGLTF_MODE_LINE_LOOP) {
            mode = GL_LINE_LOOP;
        }
        else {
            assert(0);
        }
        glDrawElements(mode, indexAccessor.count, indexAccessor.componentType,
            BUFFER_OFFSET(indexAccessor.byteOffset));
        CheckErrors("draw elements");

        {
            std::map<std::string, int>::const_iterator it(
                primitive.attributes.begin());
            std::map<std::string, int>::const_iterator itEnd(
                primitive.attributes.end());

            for (; it != itEnd; it++) {
                if ((it->first.compare("POSITION") == 0) ||
                    (it->first.compare("NORMAL") == 0) ||
                    (it->first.compare("TEXCOORD_0") == 0)) {
                    if (gGLProgramState.attribs[it->first] >= 0) {
                        glDisableVertexAttribArray(gGLProgramState.attribs[it->first]);
                    }
                }
            }
        }*/
    }

    return true;
    }

};