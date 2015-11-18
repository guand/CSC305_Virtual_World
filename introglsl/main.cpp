#include "icg_common.h"

#include "Triangle/Triangle.h"
#include "Quad/Quad.h"
Quad quad;

void init(){
    ///--- Sets background color
    glClearColor(/*gray*/ .937,.937,.937, /*solid*/1.0 );
    quad.init();
}

void display(){
    glClear(GL_COLOR_BUFFER_BIT);
    quad.draw();
}

int main(int, char**){
    glfwInitWindowSize(512, 512);
    glfwCreateWindow("introglsl");
    glfwDisplayFunc(display);
    init();
    glfwMainLoop();
    quad.cleanup();
    return EXIT_SUCCESS;
}
