#pragma once
#include <tinygltf/tiny_gltf.h>
#include "texture.h"
#include "debugging.h"
#include "octree.h"

struct gltf_model {

	tinygltf::Model model;
	tinygltf::TinyGLTF loader;

	std::string err;
	std::string warn;

    std::vector<texture> positions;
    int n_vert, n_tri;
    octree o;

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

    template <class type>
    void copy_triplet(int* dst, type* src) {
        *(int*)&dst[0] = (int)*((type*)&src[0]);
        *(int*)&dst[1] = (int)*((type*)&src[1]);
        *(int*)&dst[2] = (int)*((type*)&src[2]);
    }
    // take a model and fill the buffers to be passed to the compute shader (for ray tracing)
    bool create_buffers() {

        unsigned char* _data_vert[2] = { 0,0 };
        unsigned char * _data = 0;
        int texture_height, max_texture_width = 2048;
        GLuint  texId;

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
                (it->first.compare("TEXCOORD_0") == 0)
                ) {
                 
                if (it->first.compare("TEXCOORD_0") == 0) 
                    continue;

                int tu = (it->first.compare("POSITION") == 0) ? 1 : 2; // 1 position, 2 normal


                // Compute byteStride from Accessor + BufferView combination.
                int byteStride =
                    accessor.ByteStride(model.bufferViews[accessor.bufferView]);
                assert(byteStride != -1);

                GLuint texPos;
                glGenTextures(1, &texPos);
                glBindTexture(GL_TEXTURE_2D, texPos);

                // one long texture, just a stub implementation
                int buffer = model.bufferViews[accessor.bufferView].buffer;
                int bufferviewOffset = model.bufferViews[accessor.bufferView].byteOffset;
               
                
                n_vert = accessor.count ;
                
                // size of the texture to contain the data
                texture_height = n_vert / max_texture_width + 1;

                // accessor.count how many normals/position
                // 4 = 3 component + 1 padding because shader does not support rgb32f (without a)
                // 4 byte for each float
                unsigned char*  _data = new unsigned char[texture_height* max_texture_width*4*4];
                _data_vert[tu - 1] = _data;
                memset(_data, 0, texture_height * max_texture_width * 4 * 4);

                for (int i = 0; i < n_vert;++i)
                    memcpy_s(& _data[i*16],12, &model.buffers[buffer].data[bufferviewOffset+accessor.byteOffset + i*12 ],12);
               
                if(0)
                for (int i = 0; i < n_vert;++i)
                {
                    float x = *(float*)(&_data[i * 16]);
                    float y = *(float*)(&_data[i * 16+4]);
                    float z = *(float*)(&_data[i * 16+8]);
                    printf("%d: %f %f %f \n", i,x, y, z);
                }

               // accessor.count
                //accessor.componentType, & model.buffers[buffer].data[0];
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, max_texture_width, texture_height,0, GL_RGBA, GL_FLOAT,  _data );
               // delete[] _data;

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

                glBindImageTexture(tu, texPos, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
            }
        }

        const tinygltf::Accessor& indexAccessor =
            model.accessors[primitive.indices];

        int mode = -1;
        if (primitive.mode == TINYGLTF_MODE_TRIANGLES) {
            mode = GL_TRIANGLES;
        }
        //else if (primitive.mode == TINYGLTF_MODE_TRIANGLE_STRIP) {
        //    mode = GL_TRIANGLE_STRIP;
        //}
        //else if (primitive.mode == TINYGLTF_MODE_TRIANGLE_FAN) {
        //    mode = GL_TRIANGLE_FAN;
        //}
        //else if (primitive.mode == TINYGLTF_MODE_POINTS) {
        //    mode = GL_POINTS;
        //}
        //else if (primitive.mode == TINYGLTF_MODE_LINE) {
        //    mode = GL_LINES;
        //}
        //else if (primitive.mode == TINYGLTF_MODE_LINE_LOOP) {
        //    mode = GL_LINE_LOOP;
        //}
        else {
            assert(0);
        }

        // Compute byteStride from Accessor + BufferView combination.
            int byteStride =
                indexAccessor.ByteStride(model.bufferViews[indexAccessor.bufferView]);
        assert(byteStride != -1);

        // one long texture, just a stub implementation
        int buffer = model.bufferViews[indexAccessor.bufferView].buffer;
        int bufferviewOffset = model.bufferViews[indexAccessor.bufferView].byteOffset;

        n_tri = indexAccessor.count / 3;

        

        // size of the texture to contain the data. Every pixels store the indices for one trinagles in its rgb components
        texture_height = n_tri / max_texture_width + 1;

        
        // accessor.count how many indices
        // 4 = 3 component + 1 padding because shader does not support rgb32i (without a)
        // 4 byte for each integer
        _data = new unsigned char[texture_height * max_texture_width * 4 * 4];
        memset(_data, 0, texture_height* max_texture_width * 4 * 4);
        int x, y, z;


        for (int i = 0; i < n_tri;++i) {
            switch (indexAccessor.componentType) {
            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:copy_triplet<unsigned char>( & ((int*) _data)[i * 4], (unsigned char*)&model.buffers[buffer].data[bufferviewOffset + indexAccessor.byteOffset + i * 3]);break;
            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT: copy_triplet<unsigned short>(&((int*)_data)[i * 4], (unsigned short*)&model.buffers[buffer].data[bufferviewOffset  + indexAccessor.byteOffset + i * 3 * sizeof(unsigned short)]);    break;
            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT: copy_triplet<unsigned int>(&((int*)_data)[i * 4], (unsigned int*)&model.buffers[buffer].data[bufferviewOffset + indexAccessor.byteOffset + i * 3 * sizeof(unsigned int)]);    break;
            }
        }
        check_gl_errors(__LINE__, __FILE__);
    
 if(0)
     for (int i = 0; i < n_tri;++i)
        {
            int x = ((int*)_data)[i * 4];
            int y = ((int*)_data)[i * 4 + 1];
            int z = ((int*)_data)[i * 4 + 2];
            int _ = ((int*)_data)[i * 4 + 3];
            printf("%d: %d %d %d %d \n", i, x, y, z,_);
        }

        
        glGenTextures(1, &texId);
        glBindTexture(GL_TEXTURE_2D, texId);    
        
        // accessor.count
        //accessor.componentType, & model.buffers[buffer].data[0];
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32I, max_texture_width, texture_height, 0, GL_RGBA_INTEGER, GL_UNSIGNED_INT, _data);
        
        check_gl_errors(__LINE__, __FILE__);


        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        glBindImageTexture(3, texId, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32UI);
        check_gl_errors(__LINE__, __FILE__);


  /*      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
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

    bool use_octree = true;
    if (use_octree) {
       
        o.set((int*)_data, n_tri, (float*)_data_vert[0], n_vert,20, 5);

        GLuint  texIdOct;
        glGenTextures(1, &texIdOct);
        glBindTexture(GL_TEXTURE_2D, texIdOct);

        texture_height = o.nodes.size() / max_texture_width + 1;

        // pad to texture size
        o.nodes.insert(o.nodes.end(), max_texture_width* texture_height - o.nodes.size(), octree::rgbai());
        // accessor.count
        //accessor.componentType, & model.buffers[buffer].data[0];
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32I, max_texture_width, texture_height, 0, GL_RGBA_INTEGER, GL_UNSIGNED_INT, &o.nodes[0]);

        check_gl_errors(__LINE__, __FILE__);


        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        glBindImageTexture(4, texIdOct, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32UI);
        check_gl_errors(__LINE__, __FILE__);



        glBindTexture(GL_TEXTURE_2D, texId);
        texture_height = o.triangles_id.size() / max_texture_width + 1;
        // pad to texture size
        this->n_tri = o.triangles_id.size();
        o.triangles_id.insert(o.triangles_id.end(), max_texture_width* texture_height - o.triangles_id.size(), octree::rgbai());

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32I, max_texture_width, texture_height, 0, GL_RGBA_INTEGER, GL_UNSIGNED_INT, &o.triangles_id[0]);
        glBindImageTexture(3, texId, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32UI);
        check_gl_errors(__LINE__, __FILE__);
       
    }

    return true;
    }

};