#pragma once
#include "Mesh.h"

class EigenVisualizer{
public:
    typedef Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> GreyLevelImage;
    typedef Eigen::Matrix<Eigen::Vector3f, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> RGBImage;

    void showImage(GreyLevelImage & image, const char * windowTitle = NULL, Mesh mesh)
    {
        unsigned int width = image.rows();
        unsigned int height = image.cols();

        glfwInitWindowSize(width, height);
        if (windowTitle != NULL)
        {
            glfwCreateWindow(windowTitle);
        } else
        {
            glfwCreateWindow("Image");
        }
        //After the openGL context is set up, copy data into an openGL texture
        mesh.loadTextureR32F(image.data(), width, height);

        while(glfwGetKey(GLFW_KEY_ESC)!=GLFW_PRESS && glfwGetWindowParam(GLFW_OPENED)){
            mesh.draw();
            glfwSwapBuffers();
        }

        /// Close OpenGL window and terminate GLFW
        glfwTerminate();
    }

    void showImage(RGBImage & image, const char * windowTitle = NULL, Mesh mesh)
    {
        unsigned int width = image.rows();
        unsigned int height = image.cols();

        glfwInitWindowSize(width, height);
        if (windowTitle != NULL)
        {
            glfwCreateWindow(windowTitle);
        } else
        {
            glfwCreateWindow("Image");
        }
        //After the openGL context is set up, copy data into an openGL texture
        mesh.loadTextureRGB32F(image.data(), width, height);

        while(glfwGetKey(GLFW_KEY_ESC)!=GLFW_PRESS && glfwGetWindowParam(GLFW_OPENED)){
            mesh.draw();
            glfwSwapBuffers();
        }

        /// Close OpenGL window and terminate GLFW
        glfwTerminate();
        }
};

