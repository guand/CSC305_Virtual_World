#pragma once
#include "icg_common.h"
#define XVAL 4
#define YVAL 4

class Quad{
private:
    GLuint _vao; ///< vertex array object
    GLuint _pid; ///< GLSL shader program ID 
    GLuint _vbo_vpoint; ///< memory buffer
    GLuint _ibo;
    std::vector<vec3> _triangulation;
    std::vector<int> _triangulation_index;
public:

    void init(){
        ///--- Compile the shaders
        _pid = opengp::load_shaders("Quad/vshader.glsl", "Quad/fshader.glsl");
        if(!_pid) exit(EXIT_FAILURE);       
        glUseProgram(_pid);
        


        ///--- Vertex coordinates
        {
            for(int i = 0; i < XVAL; ++i)
            {
                for(int j = 0; j < YVAL; ++j)
                {
                    float xScale = float(i)/float(XVAL);
                    float yScale = float(j)/float(YVAL);
                    _triangulation.push_back(vec3(-0.5 + xScale, -0.5 + yScale, 0.0));
                }
            }

            for(int i = 0; i < _triangulation.size()-XVAL; ++i)
            {
                if(i%XVAL == 0){
                    _triangulation_index.push_back(XVAL*YVAL);
                }
                _triangulation_index.push_back(i);
                _triangulation_index.push_back(i+XVAL);

            }

            ///--- Vertex one vertex Array
            glGenVertexArrays(1, &_vao);
            glBindVertexArray(_vao);

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
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glDrawElements(GL_TRIANGLE_STRIP, _triangulation_index.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);        
        glUseProgram(0);
    }
};
