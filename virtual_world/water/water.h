#pragma once
#include "icg_common.h"

class Water{
public:
    bool wireframe = true;

protected:
    GLuint _vao; ///< vertex array object
    GLuint _pid; ///< GLSL shader program ID
    GLuint _vbo; ///< memory buffer
    GLuint _vbo_normal;
    GLuint _tex; ///< Texture ID
    GLuint _tex_mirror; ///< Texture ID of mirror texture
    GLuint _tex_refract;
    std::vector<vec3> _triangulation;
    std::vector<int> _triangulation_index;
    std::vector<vec3> _normal_vertices;

public:
    GLuint getProgramID(){
        return _pid;
    }

    void init(int mHeight, int mWidth, GLuint tex_mirror = -1, GLuint tex_refract = -1){
        ///--- Compile the shaders
        _pid = opengp::load_shaders("Water/vshader.glsl", "Water/fshader.glsl");
        if(!_pid) exit(EXIT_FAILURE);
        glUseProgram(_pid);

        ///--- Vertex one vertex Array
        glGenVertexArrays(1, &_vao);
        glBindVertexArray(_vao);

        ///--- Vertex coordinates
        {
            _triangulation.push_back(vec3(-2.0f, -2.0f, 0.0f));
            _triangulation.push_back(vec3(+2.0f, -2.0f, 0.0f));
            _triangulation.push_back(vec3(-2.0f, +2.0f, 0.0f));
            _triangulation.push_back(vec3(+2.0f, +2.0f, 0.0f));


            ///--- Index Buffer

            ///--- Buffer
            glGenBuffers(1, &_vbo);
            glBindBuffer(GL_ARRAY_BUFFER, _vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vec3)*_triangulation.size(), &_triangulation[0], GL_STATIC_DRAW);

            ///--- Attribute
        }


        {
            vec3 normal_vertex = (_triangulation.at(1) - _triangulation.at(0)).cross(_triangulation.at(2) - _triangulation.at(0));
            normal_vertex.normalize();
            _normal_vertices.push_back(normal_vertex);
            _normal_vertices.push_back(normal_vertex);
            _normal_vertices.push_back(normal_vertex);
            _normal_vertices.push_back(normal_vertex);

            glGenBuffers(1, &_vbo_normal);
            glBindBuffer(GL_ARRAY_BUFFER, _vbo_normal);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vec3)*_normal_vertices.size(), &_normal_vertices[0], GL_STATIC_DRAW);

//            GLuint vnormal_id = glGetAttribLocation(_pid, "vnormal");
//            glEnableVertexAttribArray(vnormal_id);
//            glVertexAttribPointer(vnormal_id, 3, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);
        }



        ///--- Texture coordinates
//        {
//            const GLfloat vtexcoord[] = { /*V1*/ 0.0f, 0.0f,
//                                          /*V2*/ 1.0f, 0.0f,
//                                          /*V3*/ 0.0f, 1.0f,
//                                          /*V4*/ 1.0f, 1.0f};

//            ///--- Buffer
//            glGenBuffers(1, &_vbo);
//            glBindBuffer(GL_ARRAY_BUFFER, _vbo);
//            glBufferData(GL_ARRAY_BUFFER, sizeof(vtexcoord), vtexcoord, GL_STATIC_DRAW);

//            ///--- Attribute
//            GLuint vtexcoord_id = glGetAttribLocation(_pid, "vtexcoord");
//            glEnableVertexAttribArray(vtexcoord_id);
//            glVertexAttribPointer(vtexcoord_id, 2, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);
//        }

        ///--- Load texture
        glGenTextures(1, &_tex);
        glBindTexture(GL_TEXTURE_2D, _tex);
        glfwLoadTexture2D("Water/Floor_texture.tga", 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        _tex_mirror = (tex_mirror==-1)? _tex : tex_mirror;
        _tex_refract = (tex_refract==-1)? _tex : tex_refract;

        ///--- Texture uniforms
        GLuint tex_id = glGetUniformLocation(_pid, "tex");
        glUniform1i(tex_id, 0 /*GL_TEXTURE0*/);
        GLuint tex_mirror_id = glGetUniformLocation(_pid, "tex_mirror");
        glUniform1i(tex_mirror_id, 1 /*GL_TEXTURE1*/);
        GLuint tex_refract_id = glGetUniformLocation(_pid, "tex_refract");
        glUniform1i(tex_refract_id, 2 /*GL_TEXTURE2*/);

        ///--- to avoid the current object being polluted
        glBindVertexArray(0);
        glUseProgram(0);
    }

    void draw(){
//        glUseProgram(_pid);
//        glBindVertexArray(_vao);
        bindShader();
            ///--- Bind textures
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, _tex);

            ///--- Bind textures
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, _tex_mirror);

            ///--- Bind textures
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, _tex_refract);

            ///--- Draw
            glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
//            glDrawElements(GL_TRIANGLE_STRIP, /*#vertices*/ _triangulation_index.size(), GL_UNSIGNED_INT, ZERO_BUFFER_OFFSET);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, _triangulation.size());
          unbindShader();
//        glBindVertexArray(0);
//        glUseProgram(0);
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
            glBindBuffer(GL_ARRAY_BUFFER, _vbo);
            glVertexAttribPointer(vpoint_id, 3 /*vec3*/, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);
            check_error_gl();
        }

        GLint vnormal_id = glGetAttribLocation(_pid, "vnormal");
        if (vnormal_id >= 0) {
            glEnableVertexAttribArray(vnormal_id);
            check_error_gl();
            glBindBuffer(GL_ARRAY_BUFFER, _vbo_normal);
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
