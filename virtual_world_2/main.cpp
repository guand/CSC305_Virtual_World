#include "icg_common.h"

#include "Mesh/Mesh.h"
Mesh mesh;

void init(){
    ///--- Sets background color
    glClearColor(/*gray*/ .937,.937,.937, /*solid*/1.0 );
    mesh.init(32, 32);
}

void display(){
    glClear(GL_COLOR_BUFFER_BIT);
    mesh.draw();
}

int main(int, char**){
    glfwInitWindowSize(512, 512);
    glfwCreateWindow("Virtual World");
    glfwDisplayFunc(display);
    init();
    glfwMainLoop();
    mesh.cleanup();
    return EXIT_SUCCESS;
}
