#pragma once
#include "icg_common.h"
#define XVAL 128
#define YVAL 128

class Quad{
private:
    GLuint _vao; ///< vertex array object
    GLuint _pid; ///< GLSL shader program ID 
    GLuint _vbo_vpoint; ///< memory buffer
    GLuint _vbo_vtexcoord;
    GLuint _tex;
    GLuint _ibo;
    std::vector<vec3> _triangulation;
    std::vector<vec2> _triangulation_tex;
    std::vector<int> _triangulation_index;
public:

    void init(){
        ///--- Compile the shaders
        _pid = opengp::load_shaders("Quad/vshader.glsl", "Quad/fshader.glsl");
        if(!_pid) exit(EXIT_FAILURE);       
        glUseProgram(_pid);
        

        float fTexU = float(XVAL)*0.00f;
        float fTexV = float(YVAL)*0.00f;

        ///--- Vertex coordinates
        {
            for(int i = 0; i < XVAL; ++i)
            {
                for(int j = 0; j < YVAL; ++j)
                {
                    float xScale = float(i)/float(XVAL-1);
                    float yScale = float(j)/float(YVAL-1);
                    _triangulation.push_back(vec3(-.5 + xScale, -.5 + yScale, 0.0));
                    _triangulation_tex.push_back(vec2(fTexU*xScale, fTexV*yScale));
                }
            }

            for(int i = 0; i < _triangulation.size()-XVAL; ++i)
            {
                if(i%XVAL == 0 && i != 0){
                    _triangulation_index.push_back(XVAL*YVAL);
                }
                _triangulation_index.push_back(i);
                _triangulation_index.push_back(i+XVAL);

            }

            ///--- Vertex one vertex Array
            glGenVertexArrays(1, &_vao);
            glBindVertexArray(_vao);


            ///--- Buffer
            glGenBuffers(1, &_vbo_vtexcoord);
            glBindBuffer(GL_ARRAY_BUFFER, _vbo_vtexcoord);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vec2)*_triangulation_tex.size(), &_triangulation_tex[0], GL_STATIC_DRAW);

            ///--- Attribute
            GLuint vtexcoord_id = glGetAttribLocation(_pid, "vtexcoord");
            glEnableVertexAttribArray(vtexcoord_id);
            glVertexAttribPointer(vtexcoord_id, 2, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);

            ///--- Load texture
            glGenTextures(1, &_tex);
            glBindTexture(GL_TEXTURE_2D, _tex);
            glfwLoadTexture2D("Quad/mrt.tga", 0);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            GLuint tex_id = glGetUniformLocation(_pid, "tex");
            glUniform1i(tex_id, 0 /*GL_TEXTURE0*/);

            ///--- Buffer
            glGenBuffers(1, &_vbo_vpoint);
            glBindBuffer(GL_ARRAY_BUFFER, _vbo_vpoint);
            glBufferData(GL_ARRAY_BUFFER, sizeof(Vec3)*_triangulation.size(), &_triangulation[0], GL_STATIC_DRAW);


            ///--- Attribute
            GLuint vpoint_id = glGetAttribLocation(_pid, "vpoint");
            glEnableVertexAttribArray(vpoint_id);
            glVertexAttribPointer(vpoint_id, 3, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);

            glGenBuffers(1, &_ibo );
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int)*_triangulation_index.size(), &_triangulation_index[0], GL_STATIC_DRAW);
            glEnable(GL_PRIMITIVE_RESTART);
            glPrimitiveRestartIndex(XVAL*YVAL);

        }

        ///--- to avoid the current object being polluted
        glBindVertexArray(0);
        glUseProgram(0);
    }
       
    void cleanup(){
        glBindVertexArray(0);
        glUseProgram(0);
        glDeleteBuffers(1, &_vbo_vpoint);
        glDeleteProgram(_pid);
        glDeleteVertexArrays(1, &_vao);
    }
    
    void draw(){
        glUseProgram(_pid);
        glBindVertexArray(_vao);
            ///--- Uniform for animation
            ///--- Draw
//            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glDrawElements(GL_TRIANGLE_STRIP, _triangulation_index.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);        
        glUseProgram(0);
    }
};
