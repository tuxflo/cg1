#include <iostream>
#include "vgl.h"
#include <math.h>
#include "LoadShaders.h"
#include "glm/glm/glm.hpp"
#include "glm/glm/gtc/matrix_transform.hpp"
#include "glm/glm/gtc/type_ptr.hpp"
#include "camera.h"

#define DEPTH 32
#define NUM_VERTICES (DEPTH+1)*(DEPTH+1)
#define NUM_INDICES 2*3*DEPTH*DEPTH

using namespace std;


enum Attrib_IDs { vPosition = 0 };
//0 - wheel 1- cube
enum VAO_IDs {wheel, cube, grid};
// Number of VAOs: 3
GLuint VAOs[3];
const GLuint Numvertices = 6;
GLuint program;
glm::mat4 Wheel_Model      = glm::mat4(1.0f);  // Changes for each model !
glm::mat4 Cube_Model = glm::mat4(1.0);
Camera cam;
static double lastTime;
GLuint wheelbuffer;
GLuint wheelarray;
GLuint cubearray;

void create_wheel(float radius);
void create_cube();
void create_grid();

void display()
{
    //white background
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    // Camera matrix
    cam.calculatePositions();
    glm::mat4 Projection = cam.getProjectionMatrix();
    glm::mat4 View  = cam.getViewMatrix();    // Model matrix : an identity matrix (model will be at the origin)
    glm::mat4 MVP        = Projection * View * Wheel_Model;

    //pass updated model to the shader (wheel)
    GLuint MatrixID = glGetUniformLocation(program, "MVP");
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
    glBindVertexArray(VAOs[wheel]);
    //Draw the circle
    glDrawArrays(GL_LINE_LOOP, 0, 1000);
    //Draw the lines
    glDrawArrays(GL_LINES, 1000, 1000);

    glBindVertexArray(VAOs[cube]);

    Cube_Model = glm::translate(glm::mat4(1.0), glm::vec3(3.0, 0.0, 0.0));
    MVP = Projection * View * Cube_Model; // Remember, matrix multiplication is the other way around

    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
    glDrawArrays(GL_TRIANGLES, 0, 12*3);

    glBindVertexArray(VAOs[grid]);
    MVP = Projection * View * glm::mat4(1.0); // Remember, matrix multiplication is the other way around
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

    //glDrawArrays(GL_TRIANGLES, 0, 3*2);

    glFlush();
    glutSwapBuffers();
    double currentTime = glutGet(GLUT_ELAPSED_TIME);
    float deltaTime = float(currentTime - lastTime);
    cam.setDeltaTime(deltaTime/1000);

    glutPostRedisplay();
    lastTime = currentTime;
}

void resize(int w, int h)
{
    glViewport(0, 0, w, h);
}

void init()
{
    lastTime = glutGet(GLUT_ELAPSED_TIME);
    //Load shader files
    ShaderInfo shaders[] = {
        {GL_VERTEX_SHADER, "./vertex.vert" },
        {GL_FRAGMENT_SHADER, "./fragment.frag" },
        {GL_NONE, NULL}
    };
    program = LoadShaders(shaders);
    glUseProgram(program);


    create_wheel(2.0);
    create_cube();
    create_grid();

    glutWarpPointer(1024/2, 768/2);
}

void keyboard(unsigned char key, int x, int y)
{
    switch(key)
    {
    case 'w':
        cam.up();
        break;
    case 's':
        cam.down();
        break;
    case 'a':
        cam.left();
        break;
    case 'd':
        cam.right();
        break;
    }
}

void create_cube()
{
    static const GLfloat g_vertex_buffer_data[] = {
        -1.0f,-1.0f,-1.0f, // triangle 1 : begin
        -1.0f,-1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f, // triangle 1 : end
        1.0f, 1.0f,-1.0f, // triangle 2 : begin
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f,-1.0f, // triangle 2 : end
        1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f,-1.0f,
        1.0f,-1.0f,-1.0f,
        1.0f, 1.0f,-1.0f,
        1.0f,-1.0f,-1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f,-1.0f,
        1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f,-1.0f, 1.0f,
        1.0f,-1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f,-1.0f,-1.0f,
        1.0f, 1.0f,-1.0f,
        1.0f,-1.0f,-1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f,-1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f,-1.0f,
        -1.0f, 1.0f,-1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f,-1.0f, 1.0f
    };

    //called trigangle but is drawing the cube
    glGenVertexArrays(1, &VAOs[cube]);
    glBindVertexArray(VAOs[cube]);
    GLuint trianglebuffer;
    glGenBuffers(1, &trianglebuffer);
    glBindBuffer(GL_ARRAY_BUFFER, trianglebuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
}

void create_wheel(float radius)
{
    int amount = 1000;
    glm::vec2 circle_pos[amount*2];
    // calc degree to rad: PI / 180°
    int count = 1000;
    for( int i =0; i <= amount; i++ )
    {
        float twicePI = 2*M_PI;
        circle_pos[i].x += cos((float)i * twicePI/amount)*radius;
        circle_pos[i].y += sin((float)i * twicePI/amount)*radius;
        if(count < 2000)
        {
            circle_pos[count].x -= cos((float)i * twicePI/25)*radius;
            circle_pos[count].y -= sin((float)i * twicePI/25)*radius;;

            circle_pos[count+1].x += cos((float)i * twicePI/25)*radius;
            circle_pos[count+1].y += sin((float)i * twicePI/25)*radius;
        }
        count += 2;
    }
     glGenVertexArrays(1, &VAOs[wheel]);
     glBindVertexArray(VAOs[wheel]);
     GLuint wheelbuffer;
     glGenBuffers(1, &wheelbuffer);
     glBindBuffer(GL_ARRAY_BUFFER, wheelbuffer);
     glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * 2 * amount,circle_pos, GL_STATIC_DRAW);
     glEnableVertexAttribArray(0);
     glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
}

void create_grid()
{
    static const GLfloat g_vertex_buffer_data[] = {
        -100.0f,0.0f,100.0f, // triangle 1 : begin
        100.0f,0.0f, 100.0f,
        -100.0f, 0.0f, -100.0f, // triangle 1 : end
        -100.0f, 0.0f,-100.0f, // triangle 2 : begin
        100.0f, 0.0f,-100.0f,
        100.0f, 0.0f,100.0f, // triangle 2 : end
    };
    glGenVertexArrays(1, &VAOs[grid]);
    glBindVertexArray(VAOs[grid]);
    GLuint trianglebuffer;
    glGenBuffers(1, &trianglebuffer);
    glBindBuffer(GL_ARRAY_BUFFER, trianglebuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
}

void mouse_func(int x, int y)
{
    cam.setPos(x, y);
    glutPostRedisplay();
}

void timer(int value)
{
    Wheel_Model = glm::rotate(Wheel_Model, -0.3f, glm::vec3(0.0f, 0.0f, 1.0f));
    glutPostRedisplay();
    glutTimerFunc(25,timer,0);
}

int main(int argc, char** argv)
{
    glewExperimental = GL_TRUE;
    glutInit(&argc, argv);
    if(glewIsSupported("GL_VERSION_1_0"))
    {
        cout << "ready for shading!" << endl;
    }
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(1024, 768);
    glutInitContextVersion(2, 1);
    glutInitContextProfile(GLUT_CORE_PROFILE);
    glutCreateWindow("Shader example");

    glutReshapeFunc(resize);

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutIdleFunc(display);
    glutPassiveMotionFunc(mouse_func);
   glutTimerFunc(25, timer, 0);
    if(glewInit()){
        cerr << "unable to init GLEW!" << endl;
        exit(EXIT_FAILURE);
    }
    init();
    glutMainLoop();

    return 0;
}

