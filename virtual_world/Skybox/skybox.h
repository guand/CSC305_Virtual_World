#ifndef SKYBOX
#define SKYBOX

#pragma once
#include "icg_common.h"

class Skybox{
public:
    bool wireframe = true;

protected:
    GLuint c_vao; ///< vertex array object
    GLuint c_vbo;
    GLuint c_pid; ///< GLSL shader program ID
    GLuint _tex; ///< Texture ID
    GLuint _tex_night; ///< Texture ID

    GLuint _skybox;
    GLuint _vpoint;    ///< memory buffer
    GLuint _vnormal;   ///< memory buffer
    const char *_cube_tex[6] = {"Skybox/front.tga", "Skybox/back.tga", "Skybox/top.tga", "Skybox/bottom.tga", "Skybox/right.tga", "Skybox/left.tga"};

public:
    GLuint getProgramID(){
        return c_pid;
    }

    void init(){

        ///--- Vertex one vertex Array


        ///--- Vertex Buffer
        ///--- Vertex coordinates
        GLfloat points[] = {
            -6.0f,  6.0f, -6.0f,
              -6.0f, -6.0f, -6.0f,
               6.0f, -6.0f, -6.0f,
               6.0f, -6.0f, -6.0f,
               6.0f,  6.0f, -6.0f,
              -6.0f,  6.0f, -6.0f,

              -6.0f, -6.0f,  6.0f,
              -6.0f, -6.0f, -6.0f,
              -6.0f,  6.0f, -6.0f,
              -6.0f,  6.0f, -6.0f,
              -6.0f,  6.0f,  6.0f,
              -6.0f, -6.0f,  6.0f,

               6.0f, -6.0f, -6.0f,
               6.0f, -6.0f,  6.0f,
               6.0f,  6.0f,  6.0f,
               6.0f,  6.0f,  6.0f,
               6.0f,  6.0f, -6.0f,
               6.0f, -6.0f, -6.0f,

              -6.0f, -6.0f,  6.0f,
              -6.0f,  6.0f,  6.0f,
               6.0f,  6.0f,  6.0f,
               6.0f,  6.0f,  6.0f,
               6.0f, -6.0f,  6.0f,
              -6.0f, -6.0f,  6.0f,

              -6.0f,  6.0f, -6.0f,
               6.0f,  6.0f, -6.0f,
               6.0f,  6.0f,  6.0f,
               6.0f,  6.0f,  6.0f,
              -6.0f,  6.0f,  6.0f,
              -6.0f,  6.0f, -6.0f,

              -6.0f, -6.0f, -6.0f,
              -6.0f, -6.0f,  6.0f,
               6.0f, -6.0f, -6.0f,
               6.0f, -6.0f, -6.0f,
              -6.0f, -6.0f,  6.0f,
               6.0f, -6.0f,  6.0f
        };

        glGenVertexArrays(1, &c_vao);
        glBindVertexArray(c_vao);
        check_error_gl();

        glGenBuffers (1, &c_vbo);
        glBindBuffer (GL_ARRAY_BUFFER, c_vbo);
        glBufferData (GL_ARRAY_BUFFER, sizeof(points), &points, GL_STATIC_DRAW);

        glEnableVertexAttribArray (0);
        glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), (GLvoid*)0);

        vector<const GLchar*> faces;
        for(int i = 0; i < 6; ++i) {
            faces.push_back(_cube_tex[i]);
        }

        ///--- Compile the shaders
        c_pid = opengp::load_shaders("Skybox/vshader.glsl", "Skybox/fshader.glsl");
        if(!c_pid) exit(EXIT_FAILURE);
        glUseProgram(c_pid);

        ///--- Load texture
        _skybox = loadCubemap(faces);
    }

    GLuint loadCubemap(vector<const GLchar*> faces)
    {
        GLuint textureID;
            glGenTextures(1, &textureID);
            glActiveTexture(GL_TEXTURE0);

            int width,height;
            unsigned char* image;

            glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
            for(GLuint i = 0; i < faces.size(); i++)
            {
                GLFWimage image_object;
                glfwReadImage ( faces[i], &image_object, GLFW_ORIGIN_UL_BIT );
                glTexImage2D(
                    GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
                    GL_RGB, image_object.Width, image_object.Height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_object.Data
                );
            }
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

            return textureID;
    }

    void cleanup(){
        /// TODO
    }

    void draw(){
        glDepthMask (GL_FALSE);
        bindShader();
            ///--- Setup the texture units
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, _skybox);
            glUniform1i(glGetUniformLocation(c_pid, "skybox"), 0);
            glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
            glDrawArrays(GL_TRIANGLES, 0 , 36);

        unbindShader();
        glDepthMask (GL_TRUE);
    }

private:

    void bindShader() {
        glUseProgram(c_pid);
        glBindVertexArray(c_vao);
        check_error_gl();

        ///--- Vertex Attribute ID for Positions
        GLint vpoint_id = glGetAttribLocation(c_pid, "vpoint");
        if (vpoint_id >= 0) {
            glEnableVertexAttribArray(vpoint_id);
            check_error_gl();
            glBindBuffer(GL_ARRAY_BUFFER, c_vbo);
            glVertexAttribPointer(vpoint_id, 3 /*vec3*/, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);
            check_error_gl();
        }
    }

    void unbindShader() {
        GLint vpoint_id = glGetAttribLocation(c_pid, "vpoint");
        if (vpoint_id >= 0)
            glDisableVertexAttribArray(vpoint_id);
        glUseProgram(0);
        glBindVertexArray(0);
    }
};

#endif // SKYBOX

