#pragma once
#include "icg_common.h"

class Mesh{
private:
    GLuint _vao; ///< vertex array object
    GLuint _pid; ///< GLSL shader program ID 
    GLuint _vbo_vpoint; ///< memory buffer
    GLuint _ibo;
    GLuint _vbo_vtexcoord;
    GLuint _tex;
    std::vector<vec3> _triangulation;
    std::vector<int> _triangulation_index;
public:

    void init(int xCoord, int yCoord){
        ///--- Compile the shaders
        _pid = opengp::load_shaders("Mesh/vshader.glsl", "Mesh/fshader.glsl");
        if(!_pid) exit(EXIT_FAILURE);       
        glUseProgram(_pid);
        
        ///--- Vertex coordinates
        {
            for(int i = 0; i < xCoord; ++i)
            {
                for(int j = 0; j < yCoord; ++j)
                {
                    float xScale = float(i)/float(xCoord-1);
                    float yScale = float(j)/float(yCoord-1);
                    _triangulation.push_back(vec3(-1.0 + (xScale * 2), -1.0 + (yScale * 2), 0.0));
                }
            }

            int incrementor = 0;
            for(int i = 0; i < xCoord-1; ++i)
            {
                for(int j = 0; j < yCoord; ++j)
                {
                    _triangulation_index.push_back(incrementor);
                    _triangulation_index.push_back(incrementor+xCoord);
                    incrementor++;
                }
                if(i < xCoord-2) _triangulation_index.push_back(xCoord*yCoord);
            }

            for(int i = 0; i < _triangulation_index.size(); ++i)
                std::cout << _triangulation_index.at(i) << std::endl;

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
            glPrimitiveRestartIndex(xCoord*yCoord);

        }

        ///--- Texture coordinates
        {
            const GLfloat vtexcoord[] = { 1.0f, 0.0f,
                                          1.0f, 1.0f,
                                          0.0f, 0.0f,
                                          0.0f, 1.0f,
                                            };

            ///--- Buffer
            glGenBuffers(1, &_vbo_vtexcoord);
            glBindBuffer(GL_ARRAY_BUFFER, _vbo_vtexcoord);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vtexcoord), vtexcoord, GL_STATIC_DRAW);

            ///--- Attribute
            GLuint vtexcoord_id = glGetAttribLocation(_pid, "vtexcoord");
            glEnableVertexAttribArray(vtexcoord_id);
            glVertexAttribPointer(vtexcoord_id, 2, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);
        }


        ///--- Load texture
        glGenTextures(1, &_tex);

        GLuint tex_id = glGetUniformLocation(_pid, "tex");
        glUniform1i(tex_id, 0 /*GL_TEXTURE0*/);
        ///--- to avoid the current object being polluted
        glBindVertexArray(0);
        glUseProgram(0);
    }
       
    void cleanup(){
        glBindVertexArray(0);
        glUseProgram(0);
        glDeleteBuffers(1, &_vbo_vpoint);
        glDeleteBuffers(1, &_vbo_vtexcoord);
        glDeleteProgram(_pid);
        glDeleteVertexArrays(1, &_vao);
        glDeleteTextures(1, &_tex);
    }

    void loadTextureR32F(void * pTex, int width, int height)
    {
        glBindTexture(GL_TEXTURE_2D, _tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width,
                        height, 0, GL_RED, GL_FLOAT,
                        pTex);
    }

    void loadTextureRGB32F(void * pTex, int width, int height)
    {
        glBindTexture(GL_TEXTURE_2D, _tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width,
                        height, 0, GL_RGB, GL_FLOAT,
                        pTex);
    }

    void loadTextureRGBA8UBYTE(void * pTex, int width, int height)
    {
        glBindTexture(GL_TEXTURE_2D, _tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width,
                        height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                        pTex);
    }
    
    void draw(){
        glUseProgram(_pid);
        glBindVertexArray(_vao);

        ///--- Bind textures
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, _tex);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDrawElements(GL_TRIANGLE_STRIP, _triangulation_index.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);        
        glUseProgram(0);
    }
};
