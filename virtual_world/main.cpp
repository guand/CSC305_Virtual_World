#include "icg_common.h"
#include "Mesh/mesh.h"
#include "Noise/perlin.h"
#include "Noise/perlinfractal.h"
#include "Noise/simplexfractal.h"
#include "Skybox/skybox.h"

int window_width = 1024;
int window_height = 768;
int noise_width = 512;
int noise_height = 512;
int mesh_width = 256;
int mesh_height = 256;
float octave = 8.0;
int period = 256;
int seed = 2506611879;
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
Mesh mesh;
Skybox skybox;
float camera_distance = 45.0;
float horizontalAngle = 0;
// vertical angle : 0, look at the horizon
float verticalAngle = 0.0f;
float initialFoV = 45.0f;
vec3 camera_pos(0, 0, .5);
vec3 direction;
vec3 right_pos;
vec3 up;
float mouseSpeed = 0.005f;
float speed = 3.0f;
float deltaTime;
float FoV;
int xpos, ypos;
int old_xpos, old_ypos;




void init(){
    glfwEnable(GLFW_KEY_REPEAT);     
    glClearColor(1,1,1, /*solid*/1.0 );    

    glEnable(GL_DEPTH_TEST);
//    glfwSetMousePos(window_width/2, window_height/2);
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
    skybox.init();
}

void movementCalculations(){
    static double lastTime = glfwGetTime();
    double currentTime = glfwGetTime();
    deltaTime = currentTime - lastTime;

    glfwGetMousePos(&xpos, &ypos);

    // Reset mouse position for next frame
    horizontalAngle += mouseSpeed * float(old_xpos - xpos );
    verticalAngle   += mouseSpeed * float( old_ypos - ypos );
    old_xpos = xpos;
    old_ypos = ypos;

    // Compute new orientation

    direction = vec3(cos(verticalAngle) * sin(horizontalAngle), cos(verticalAngle) * cos(horizontalAngle), sin(verticalAngle));
    right_pos = vec3(sin(horizontalAngle - M_PI/2.0f), cos(horizontalAngle - M_PI/2.0f), 0);
    up = right_pos.cross(direction);
    FoV = initialFoV - 5 * glfwGetMouseWheel();

    lastTime = currentTime;
}


void display(){
    opengp::update_title_fps("Procedural terrain Generation");
    glViewport(0,0,window_width,window_height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    ///--- Upload viewing matrices externally
    GLuint pid = mesh.getProgramID();
    glUseProgram(pid);
        movementCalculations();

        mat4 MODEL = mat4::Identity();
        glUniformMatrix4fv(glGetUniformLocation(pid, "MODEL"), 1, GL_FALSE, MODEL.data());


        vec3 new_position = camera_pos + direction;

        mat4 VIEW = Eigen::lookAt( camera_pos, new_position, up ); //< "z" up on screen
        glUniformMatrix4fv(glGetUniformLocation(pid, "VIEW"), 1, GL_FALSE, VIEW.data());
        
        mat4 PROJ = Eigen::perspective(75.0f, window_width/(float)window_height, 0.1f, 100.0f);
        glUniformMatrix4fv(glGetUniformLocation(pid, "PROJ"), 1, GL_FALSE, PROJ.data());
    glUseProgram(pid);
    mesh.draw();
    glDepthMask(GL_FALSE);
    GLuint c_pid = skybox.getProgramID();
    glUseProgram(c_pid);
//        mat4 VIEW1 = Eigen::lookAt( camera_pos, vec3(0,0,0), vec3(0,0,1) ); //< "z" up on screen
        glUniformMatrix4fv(glGetUniformLocation(c_pid, "MODEL"), 1, GL_FALSE, MODEL.data());
        glUniformMatrix4fv(glGetUniformLocation(c_pid, "VIEW"), 1, GL_FALSE, VIEW.data());
        glUniformMatrix4fv(glGetUniformLocation(c_pid, "PROJ"), 1, GL_FALSE, PROJ.data());
    glUseProgram(c_pid);
    skybox.draw();
    glDepthMask(GL_TRUE);
}

/// NOTE: see glfwEnable(GLFW_KEY_REPEAT)
void keyboard(int key, int action){
    if (action==GLFW_PRESS && key==GLFW_KEY_SPACE) {
        mesh.wireframe = !mesh.wireframe;
        skybox.wireframe = !skybox.wireframe;
    }
    if (action==GLFW_PRESS && key==87)
        camera_pos += direction * deltaTime * speed;
    if (action==GLFW_PRESS && key==83)
        camera_pos -= direction * deltaTime * speed;
    if (action==GLFW_PRESS && key==68)
        camera_pos += right_pos * deltaTime * speed;
    if (action==GLFW_PRESS && key==65)
        camera_pos -= right_pos * deltaTime * speed;

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
