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
    std::vector<vec3> _triangulation;
    std::vector<int> _triangulation_index;
    std::vector<vec3> _normal_vertices;
    typedef Eigen::Matrix<Eigen::Vector3f, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> RGBImage;

public:        
    GLuint getProgramID(){ 
        return _pid; 
    }
    
    void init(RGBImage & image, int mHeight, int mWidth){
        ///--- Vertex one vertex Array
        glGenVertexArrays(1, &_vao);
        glBindVertexArray(_vao);
        check_error_gl();                      
        
        ///--- Vertex Buffer
        for(int i = 0; i < mWidth; ++i)
        {
            for(int j = 0; j < mHeight; ++j)
            {
                float xScale = float(i)/float(mWidth-1);
                float yScale = float(j)/float(mHeight-1);

                vec3 iColor = image(i * (image.rows()/mWidth), j * (image.cols()/mHeight));
                float zheight = iColor(0) * 0.5;
                if(iColor(0) < 0.0f) zheight = 0.0;
                _triangulation.push_back(vec3(-1.0 + (xScale * 2), -1.0 + (yScale * 2), zheight));
            }
        }

        glGenBuffers(1, &_vpoint);
        glBindBuffer(GL_ARRAY_BUFFER, _vpoint);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vec3)*_triangulation.size(), &_triangulation[0], GL_STATIC_DRAW);
        check_error_gl();        
    
        ///--- Normal Buffer
        RGBImage _normal_vector0(mWidth-1, mHeight-1);
        RGBImage _normal_vector1(mWidth-1, mHeight-1);
        int nIncrementor = 0;
        for(int i = 0; i < mWidth-1; ++i)
        {
            for(int j = 0; j < mHeight-1; ++j)
            {
                vec3 vTriangle0[] = {
                    _triangulation.at(nIncrementor),
                    _triangulation.at(nIncrementor+mWidth),
                    _triangulation.at(nIncrementor+mWidth+1)
                };
                vec3 vTriangle1[] = {
                    _triangulation.at(nIncrementor+mWidth+1),
                    _triangulation.at(nIncrementor+1),
                    _triangulation.at(nIncrementor)
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
        
        ///--- Load texture
        glGenTextures(1, &_tex);
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

    void setImage(RGBImage & image)
    {
        unsigned int _width = image.rows();
        unsigned int _height = image.cols();
        //After the openGL context is set up, copy data into an openGL texture
        loadTextureRGB32F(image.data(), _width, _height);
    }
           
    void cleanup(){
        /// TODO
    }

    void draw(){
        bindShader();  
            ///--- Setup the texture units
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, _tex);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, _grass);
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
