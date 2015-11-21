#pragma once
#include "icg_common.h"

class Mesh{
public:
    bool wireframe = true;
    
protected:
    GLuint _vao; ///< vertex array object
    GLuint _pid; ///< GLSL shader program ID
    GLuint _tex; ///< Texture ID
    GLuint _tex_night; ///< Texture ID  
    GLuint _vpoint;    ///< memory buffer
    GLuint _vnormal;   ///< memory buffer
    std::vector<vec3> _triangulation;
    std::vector<int> _triangulation_index;

public:        
    GLuint getProgramID(){ 
        return _pid; 
    }
    
    void init(int xCoord, int yCoord){
        ///--- Vertex one vertex Array
        glGenVertexArrays(1, &_vao);
        glBindVertexArray(_vao);
        check_error_gl();                      
        
        ///--- Vertex Buffer
        for(int i = 0; i < xCoord; ++i)
        {
            for(int j = 0; j < yCoord; ++j)
            {
                float xScale = float(i)/float(xCoord-1);
                float yScale = float(j)/float(yCoord-1);
                _triangulation.push_back(vec3(-1.0 + (xScale * 2), -1.0 + (yScale * 2), 0.0));
            }
        }

        glGenBuffers(1, &_vpoint);
        glBindBuffer(GL_ARRAY_BUFFER, _vpoint);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vec3)*_triangulation.size(), &_triangulation[0], GL_STATIC_DRAW);
        check_error_gl();        
    
        ///--- Normal Buffer

    
        ///--- Index Buffer
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



        GLuint _vbo_indices;
        glGenBuffers(ONE, &_vbo_indices);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vbo_indices);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, _triangulation_index.size() * sizeof(unsigned int), &_triangulation_index[0], GL_STATIC_DRAW);
        glEnable(GL_PRIMITIVE_RESTART);
        glPrimitiveRestartIndex(xCoord*yCoord);
        check_error_gl();        

        ///--- Compile the shaders
        _pid = opengp::load_shaders("Mesh/vshader.glsl", "Mesh/fshader.glsl");
        if(!_pid) exit(EXIT_FAILURE);
        glUseProgram(_pid);
        
        ///--- Load texture
        glGenTextures(1, &_tex);
//        glBindTexture(GL_TEXTURE_2D, _tex);
//        glfwLoadTexture2D("Mesh/texture.tga", 0);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glUniform1i(glGetUniformLocation(_pid, "tex"), 0 /*GL_TEXTURE0*/);
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
           
    void cleanup(){
        /// TODO
    }

    void draw(){
        bindShader();  
            ///--- Setup the texture units
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, _tex);
            glUniform1f(glGetUniformLocation(_pid, "time"), glfwGetTime());
            glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
            glDrawElements(GL_TRIANGLE_STRIP, /*#vertices*/ _triangulation_index.size(), GL_UNSIGNED_INT, ZERO_BUFFER_OFFSET);
        unbindShader();
    }
    
private:
    void bindShader() {
        glUseProgram(_pid);
        glBindVertexArray(_vao);
        check_error_gl();

        ///--- Vertex Attribute ID for Positions
        GLint vpoint_id = glGetAttribLocation(_pid, "vpoint");
        if (vpoint_id >= 0) {
            glEnableVertexAttribArray(vpoint_id);
            check_error_gl();
            glBindBuffer(GL_ARRAY_BUFFER, _vpoint);
            glVertexAttribPointer(vpoint_id, 3 /*vec3*/, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);
            check_error_gl();
        }

        ///--- Vertex Attribute ID for Normals
        GLint vnormal_id = glGetAttribLocation(_pid, "vnormal");
        if (vnormal_id >= 0) {
            glEnableVertexAttribArray(vnormal_id);
            glBindBuffer(GL_ARRAY_BUFFER, _vnormal);
            glVertexAttribPointer(vnormal_id, 3 /*vec3*/, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);
            check_error_gl();
        }
    }

    void unbindShader() {
        GLint vpoint_id = glGetAttribLocation(_pid, "vpoint");
        if (vpoint_id >= 0)
            glDisableVertexAttribArray(vpoint_id);
        GLint vnormal_id = glGetAttribLocation(_pid, "vnormal");
        if (vnormal_id >= 0)
            glDisableVertexAttribArray(vnormal_id);
        glUseProgram(0);
        glBindVertexArray(0);
    }
};
