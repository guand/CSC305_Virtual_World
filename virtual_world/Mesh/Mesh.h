#pragma once
#include "icg_common.h"

class Mesh{
public:
    bool wireframe = true;
    
protected:
    GLuint _vao; ///< vertex array object
    GLuint _pid; ///< GLSL shader program ID
    GLuint _tex; ///< Texture ID
    GLuint _grass;
    GLuint _rock;
    GLuint _sand;
    GLuint _snow;
    GLuint _water;
    GLuint _vpoint;    ///< memory buffer
    GLuint _vnormal;   ///< memory buffer
    GLuint _vtexcoord;
    std::vector<vec3> _triangulation;
    std::vector<vec3> _normal_vertices;
    std::vector<vec2> _triangulation_tex;
    std::vector<int> _triangulation_index;
    typedef Eigen::Matrix<Eigen::Vector3f, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> bufferMatrix;

public:        
    GLuint getProgramID(){ 
        return _pid; 
    }

    void init(bufferMatrix & image, int mHeight, int mWidth){
        ///--- Vertex one vertex Array
        glGenVertexArrays(1, &_vao);
        glBindVertexArray(_vao);
        check_error_gl();

        float f_tex_u = float(mWidth) * .2f;
        float f_tex_v = float(mHeight) * .2f;
        bufferMatrix _triangulation_matrix(mWidth, mHeight);
        ///--- Vertex Buffer
        for(int i = 0; i < mWidth; ++i)
        {
            for(int j = 0; j < mHeight; ++j)
            {
                float x_scale = float(i)/float(mWidth-1);
                float y_scale = float(j)/float(mHeight-1);
                float x_vertex_pt = -1.0 + (x_scale * 2);
                float y_vertex_pt = -1.0 + (y_scale * 2);
                float img_scale_width = image.rows()/float(mWidth);
                float img_scale_height = image.cols()/float(mHeight);
                vec3 iColor = image(i * img_scale_width, j * img_scale_height);
                float z_height = iColor(0)*0.5;
                if(iColor(0) < 0.0f) z_height = 0.0;
                _triangulation.push_back(vec3(x_vertex_pt, y_vertex_pt, z_height));
                _triangulation_tex.push_back(vec2(x_scale * f_tex_u, y_scale * f_tex_v));
            }
        }

        glGenBuffers(1, &_vpoint);
        glBindBuffer(GL_ARRAY_BUFFER, _vpoint);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vec3)*_triangulation.size(), &_triangulation[0], GL_STATIC_DRAW);
        check_error_gl();        
    
        ///--- Normal Buffer
        bufferMatrix _normal_vector0(mWidth-1, mHeight-1);
        bufferMatrix _normal_vector1(mWidth-1, mHeight-1);
        int nIncrementor = 0;
        for(int i = 0; i < mWidth-1; ++i)
        {
            for(int j = 0; j < mHeight-1; ++j)
            {
                vec3 vTriangle0[] = {
                    _triangulation.at(nIncrementor),
                    _triangulation.at(nIncrementor+mWidth),
                    _triangulation.at(nIncrementor+1)
                };
                vec3 vTriangle1[] = {
                    _triangulation.at(nIncrementor+mWidth+1),
                    _triangulation.at(nIncrementor+1),
                    _triangulation.at(nIncrementor+mWidth)
                };

                vec3 vTriangleNormal0 = (vTriangle0[0] - vTriangle0[1]).cross(vTriangle0[1] - vTriangle0[2]);
                vec3 vTriangleNormal1 = (vTriangle1[0] - vTriangle1[1]).cross(vTriangle1[1] - vTriangle1[2]);
                vTriangleNormal0.normalize();
                vTriangleNormal1.normalize();
                _normal_vector0(i, j) = vTriangleNormal0;
                _normal_vector1(i, j) = vTriangleNormal1;
                nIncrementor++;
            }
        }

        for(int i = 0; i < mWidth-1; ++i)
        {
            for(int j = 0; j < mHeight-1; ++j)
            {
                vec3 normal_vertex = vec3(0.0, 0.0, 0.0);
                if(i != 0 && j != 0)
                {
                    normal_vertex += _normal_vector0(i-1, j-1);
                    normal_vertex += _normal_vector1(i-1, j-1);
                }
                if(i != 0 && j != mHeight-1)
                {
                    normal_vertex += _normal_vector0(i-1, j);
                }
                if(i != mWidth-1 && j != mHeight-1)
                {
                    normal_vertex += _normal_vector0(i, j);
                    normal_vertex += _normal_vector1(i, j);
                }
                if(i != mWidth-1 && j != 0)
                {
                         normal_vertex += _normal_vector1(i, j-1);
                }
                normal_vertex.normalize();
                _normal_vertices.push_back(normal_vertex);
            }
        }

        glGenBuffers(ONE, &_vnormal);
        glBindBuffer(GL_ARRAY_BUFFER, _vnormal);
        glBufferData(GL_ARRAY_BUFFER, _normal_vertices.size() * sizeof(vec3), &_normal_vertices[0], GL_STATIC_DRAW);
        check_error_gl();
    
        ///--- Index Buffer
        int incrementor = 0;
        for(int i = 0; i < mWidth-1; ++i)
        {
            for(int j = 0; j < mHeight; ++j)
            {
                _triangulation_index.push_back(incrementor);
                _triangulation_index.push_back(incrementor+mWidth);
                incrementor++;
            }
            if(i < image.rows()-2) _triangulation_index.push_back(mWidth*mHeight);
        }



        GLuint _vbo_indices;
        glGenBuffers(ONE, &_vbo_indices);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vbo_indices);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, _triangulation_index.size() * sizeof(unsigned int), &_triangulation_index[0], GL_STATIC_DRAW);
        glEnable(GL_PRIMITIVE_RESTART);
        glPrimitiveRestartIndex(mWidth*mHeight);
        check_error_gl();        

        ///--- Compile the shaders
        _pid = opengp::load_shaders("Mesh/vshader.glsl", "Mesh/fshader.glsl");
        if(!_pid) exit(EXIT_FAILURE);
        glUseProgram(_pid);
        
        {

            glGenBuffers(1, &_vtexcoord);
            glBindBuffer(GL_ARRAY_BUFFER, _vtexcoord);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vec2)*_triangulation_tex.size(), &_triangulation_tex[0], GL_STATIC_DRAW);
            GLuint vtexcoord_id = glGetAttribLocation(_pid, "vtexcoord");
            glEnableVertexAttribArray(vtexcoord_id);
            glVertexAttribPointer(vtexcoord_id, 2, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);
        }
        ///--- Load texture
        glGenTextures(1, &_grass);
        glBindTexture(GL_TEXTURE_2D, _grass);
        glfwLoadTexture2D("Mesh/grass.tga", 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glUniform1i(glGetUniformLocation(_pid, "grass_tex"), 0 /*GL_TEXTURE0*/);
        glGenTextures(1, &_rock);
        glBindTexture(GL_TEXTURE_2D, _rock);
        glfwLoadTexture2D("Mesh/rock.tga", 1);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glUniform1i(glGetUniformLocation(_pid, "rock_tex"), 1 /*GL_TEXTURE1*/);
        glGenTextures(1, &_sand);
        glBindTexture(GL_TEXTURE_2D, _sand);
        glfwLoadTexture2D("Mesh/sand.tga", 2);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glUniform1i(glGetUniformLocation(_pid, "sand_tex"), 2 /*GL_TEXTURE2*/);
        glGenTextures(1, &_snow);
        glBindTexture(GL_TEXTURE_2D, _snow);
        glfwLoadTexture2D("Mesh/snow.tga", 3);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glUniform1i(glGetUniformLocation(_pid, "snow_tex"), 3 /*GL_TEXTURE3*/);
        glGenTextures(1, &_water);
        glBindTexture(GL_TEXTURE_2D, _water);
        glfwLoadTexture2D("Mesh/water.tga", 4);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glUniform1i(glGetUniformLocation(_pid, "water_tex"), 4 /*GL_TEXTURE4*/);
    }

    void cleanup(){
        /// TODO
    }

    void textureDraw(GLuint id, int index)
    {
        glActiveTexture(GL_TEXTURE0+index);
        glBindTexture(GL_TEXTURE_2D, id);
    }

    void draw(){
        bindShader();  
            ///--- Setup the texture units
            textureDraw(_grass, 0);
            textureDraw(_rock, 1);
            textureDraw(_sand, 2);
            textureDraw(_snow, 3);
            textureDraw(_water, 4);
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
