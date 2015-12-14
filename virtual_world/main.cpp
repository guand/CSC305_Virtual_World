#include "icg_common.h"
#include "FrameBuffer.h"
#include "Mesh/mesh.h"
#include "Noise/perlin.h"
#include "Noise/perlinfractal.h"
#include "Noise/simplexfractal.h"
#include "Skybox/skybox.h"
#include "Camera/camera.h"
#include "Water/water.h"

int window_width = 1024;
int window_height = 768;
int noise_width = 768;
int noise_height = 768;
int mesh_width = 256;
int mesh_height = 256;
float octave = 6.0;
int period = 256;
int seed = 434;
float lacunarity = 2.0;
float gain = 1.0;
float offset = 0.7;
typedef Eigen::Matrix<Eigen::Vector3f, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> RGBImage;
enum turbulance {
    NORMAL_T,
    RIGID_T,
    IQ_T,
    HYBRID_T

};
turbulance fractal_type = HYBRID_T;
FrameBuffer fb(window_width, window_height);
FrameBuffer fb_refract(window_width, window_height);
Mesh mesh;
Water water;
Skybox skybox;
Camera camera;
float camera_distance = 45.0;
float horizontalAngle = 0.0f;
// vertical angle : 0, look at the horizon
float verticalAngle = 0.0f;
float initialFoV = 45.0f;
vec3 camera_pos(0, 0, 1.0);
vec3 direction;
vec3 right_pos;
vec3 up;
float mouseSpeed = 0.005f;
float speed = 1.0f;
float deltaTime;
float FoV;
int xpos, ypos;
int old_xpos, old_ypos;

void init(){
    glfwEnable(GLFW_KEY_REPEAT);     
    glClearColor(1,1,1, /*solid*/1.0 );    

    glEnable(GL_DEPTH_TEST);
    glfwDisable(GLFW_MOUSE_CURSOR);
    PerlinFractal pFractal = PerlinFractal(octave, period, seed, noise_width, noise_height, lacunarity, gain, offset);
    pFractal.init();
    RGBImage perlin_fractal;
    SimplexFractal sFractal = SimplexFractal(octave, period, seed, noise_width, noise_height, lacunarity, gain, offset);
    sFractal.init();
    RGBImage simplex_fractal;
    switch(fractal_type)
    {
    case NORMAL_T:
        perlin_fractal = pFractal.setNormalNoise();
        simplex_fractal = sFractal.setNormalNoise();
        break;
    case RIGID_T:
        perlin_fractal = pFractal.setRigidNoise();
        simplex_fractal = sFractal.setRigidNoise();
        break;
    case IQ_T:
        perlin_fractal = pFractal.setIQNoise();
        break;
    case HYBRID_T:
        perlin_fractal = pFractal.setHybridNoise();
        simplex_fractal = sFractal.setHybridNoise();
        break;
    default:
        std::cout << "Something Broke!!!" << std::endl;
        break;
    }
    mesh.init(simplex_fractal, mesh_height, mesh_width);
    GLuint fb_tex = fb.init();
    GLuint fb_rtex = fb_refract.init();
    skybox.init();
    water.init(mesh_height, mesh_width, fb_tex, fb_rtex);
    camera.init(camera_distance, horizontalAngle, verticalAngle, initialFoV, camera_pos, mouseSpeed, speed);
}

void display(){
    opengp::update_title_fps("Procedural terrain Generation");
    glViewport(0,0,window_width,window_height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ///--- Upload viewing matrices externally
    GLuint pid = mesh.getProgramID();
    glUseProgram(pid);
        camera.cameraMovement();
        float height_pos = mesh.getMapHeight(camera.getCameraPosition()(0), camera.getCameraPosition()(1));
        camera.setHeight(height_pos);
        camera.cameraState();
        mat4 MODEL = mat4::Identity();

        MODEL.row(2) << 0,0,-1,0;

        glUniformMatrix4fv(glGetUniformLocation(pid, "MODEL"), 1, GL_FALSE, MODEL.data());


        vec3 new_position = camera.getCameraPosition() + camera.getDirection();
        mat4 VIEW = Eigen::lookAt( camera.getCameraPosition(), new_position, camera.getUp() ); //< "z" up on screen
        glUniformMatrix4fv(glGetUniformLocation(pid, "VIEW"), 1, GL_FALSE, VIEW.data());
        mat4 PROJ = Eigen::perspective(75.0f, window_width/(float)window_height, 0.1f, 100.0f);

        glUniformMatrix4fv(glGetUniformLocation(pid, "PROJ"), 1, GL_FALSE, PROJ.data());
        vec3 cam_pos_mirror = camera.getReverseCameraPosition();
        vec3 reverse_up = camera.getUp();
        vec3 reverse_position = camera.getReverseCameraPosition() + camera.getDirection();

        glUniform4f(glGetUniformLocation(pid, "plane"), 0.0, 0.0, -1.0, -.01);
    glUseProgram(pid);
    GLuint c_pid = skybox.getProgramID();
    glUseProgram(c_pid);
    mat4 VIEW2 = mat4::Identity();

    for(int i = 0; i < 3; ++i)
        for(int j = 0; j < 3; ++j)
            VIEW2(i, j) = VIEW(i, j);
        glUniformMatrix4fv(glGetUniformLocation(c_pid, "MODEL"), 1, GL_FALSE, MODEL.data());
        glUniformMatrix4fv(glGetUniformLocation(c_pid, "VIEW"), 1, GL_FALSE, VIEW2.data());
        glUniformMatrix4fv(glGetUniformLocation(c_pid, "PROJ"), 1, GL_FALSE, PROJ.data());
        glUniform1f(glGetUniformLocation(c_pid, "reverse"), -1.0);
    glUseProgram(c_pid);
    fb.bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        mesh.draw();
        skybox.draw();
    fb.unbind();

    glUseProgram(pid);
        MODEL = mat4::Identity();
        glUniformMatrix4fv(glGetUniformLocation(pid, "MODEL"), 1, GL_FALSE, MODEL.data());
        glUniform4f(glGetUniformLocation(pid, "plane"), 0.0, 0.0, -1.0, .1);
    glUseProgram(pid);
    fb_refract.bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        mesh.draw();
    fb_refract.unbind();
    glUseProgram(pid);
        glUniform4f(glGetUniformLocation(pid, "plane"), 0.0, 0.0, 0.0, 0.0);
    glUseProgram(pid);
    mesh.draw();



    GLuint w_pid = water.getProgramID();
    glUseProgram(w_pid);
        glUniformMatrix4fv(glGetUniformLocation(w_pid, "MODEL"), 1, GL_FALSE, MODEL.data());
        glUniformMatrix4fv(glGetUniformLocation(w_pid, "VIEW"), 1, GL_FALSE, VIEW.data());
        glUniformMatrix4fv(glGetUniformLocation(w_pid, "PROJ"), 1, GL_FALSE, PROJ.data());
    glUseProgram(w_pid);
    water.draw();

    glUseProgram(c_pid);


        glUniformMatrix4fv(glGetUniformLocation(c_pid, "VIEW"), 1, GL_FALSE, VIEW2.data());
        glUniform1f(glGetUniformLocation(c_pid, "reverse"), 1.0);
    glUseProgram(c_pid);

    skybox.draw();



}

/// NOTE: see glfwEnable(GLFW_KEY_REPEAT)
void keyboard(int key, int action){
    if (action==GLFW_PRESS && key==GLFW_KEY_SPACE) {
        mesh.wireframe = !mesh.wireframe;
        skybox.wireframe = !skybox.wireframe;
        water.wireframe = !water.wireframe;
    }
    if(action==GLFW_PRESS) {
        if (key==87)
            camera.moveUp();
        if (key==83)
            camera.moveDown();
        if (key==68)
            camera.moveRight();
        if (key==65)
            camera.moveLeft();
        if (key==70)
            camera.toggleFlymode();
    } else if(action==GLFW_RELEASE) {
        if (key==87)
            camera.stopUp();
        if (key==83)
            camera.stopDown();
        if (key==68)
            camera.stopRight();
        if (key==65)
            camera.stopLeft();
    }
}



int main(int, char**){
    glfwInitWindowSize(window_width, window_height);
    glfwCreateWindow();
    glfwSetKeyCallback(keyboard);
    glfwDisplayFunc(display);
    init();
    glfwMainLoop();
    return EXIT_SUCCESS;
}
