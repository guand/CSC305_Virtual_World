#include "icg_common.h"
#include "Mesh/mesh.h"
#include "Noise/perlin.h"
#include "Noise/fractal.h"

int window_width = 1024;
int window_height = 640;
int noise_width = 512;
int noise_height = 512;
int mesh_width = 128;
int mesh_height = 128;
float octave = 4.0;
int period = 128;
int seed = 3;
float lacunarity = 2.0;
float gain = 0.5;
float offset = 0.5;
typedef Eigen::Matrix<Eigen::Vector3f, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> RGBImage;
enum turbulance {
    NORMAL_T,
    RIGID_T,
    IQ_T,
    HYBRID_T
};
turbulance fractal_type = HYBRID_T;
Mesh mesh;
float theta = 30; //< camera angle

void init(){
    glfwEnable(GLFW_KEY_REPEAT);     
    glClearColor(1,1,1, /*solid*/1.0 );    
    glEnable(GL_DEPTH_TEST);
    Fractal fractal = Fractal(octave, period, seed, noise_width, noise_height, lacunarity, gain, offset);
    fractal.init();
    RGBImage perlin_fractal;
    switch(fractal_type)
    {
    case NORMAL_T:
        perlin_fractal = fractal.setNormalNoise();
        break;
    case RIGID_T:
        perlin_fractal = fractal.setRigidNoise();
        break;
    case IQ_T:
        perlin_fractal = fractal.setIQNoise();
        break;
    case HYBRID_T:
        perlin_fractal = fractal.setHybridNoise();
    default:
        std::cout << "Something Broke!!!" << std::endl;
        break;
    }
    mesh.init(perlin_fractal, mesh_height, mesh_width);
//    mesh.setImage(perlin_fractal);
}



void display(){
    opengp::update_title_fps("Intro. to Computer Graphics");   
    glViewport(0,0,window_width,window_height);    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    ///--- Upload viewing matrices externally
    GLuint pid = mesh.getProgramID();
    glUseProgram(pid);
        mat4 MODEL = mat4::Identity();
        glUniformMatrix4fv(glGetUniformLocation(pid, "MODEL"), 1, GL_FALSE, MODEL.data());

        float theta_rad = M_PI/180.0*theta;
        vec3 camera_pos( 0, -2*cos(theta_rad), 2*sin(theta_rad));
        mat4 VIEW = Eigen::lookAt( camera_pos, vec3(0,0,0), vec3(0,0,1) ); //< "z" up on screen
        glUniformMatrix4fv(glGetUniformLocation(pid, "VIEW"), 1, GL_FALSE, VIEW.data());
        
        mat4 PROJ = Eigen::perspective(75.0f, window_width/(float)window_height, 0.1f, 10.0f);
        glUniformMatrix4fv(glGetUniformLocation(pid, "PROJ"), 1, GL_FALSE, PROJ.data());
    glUseProgram(pid);
    
    mesh.draw();
}

/// NOTE: see glfwEnable(GLFW_KEY_REPEAT)
void keyboard(int key, int action){
    if (action==GLFW_PRESS && key==GLFW_KEY_SPACE)
        mesh.wireframe = !mesh.wireframe;
    if (action==GLFW_PRESS && key==GLFW_KEY_UP)
        theta = std::min( theta+1, +89.f );
    if (action==GLFW_PRESS && key==GLFW_KEY_DOWN)
        theta = std::max( theta-1, -89.f );
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
