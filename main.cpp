#include <iostream>
#include "vgl.h"
#include <math.h>
#include "LoadShaders.h"
#include "glm/glm/glm.hpp"
#include "glm/glm/gtc/matrix_transform.hpp"
#include "glm/glm/gtc/type_ptr.hpp"

using namespace std;

enum VAO_IDs  { Triangles, Lines, NumVAOs };
enum Buffer_IDs { ArrayBuffer, NumBuffers };
enum Attrib_IDs { vPosition = 0 };

GLuint VAOs[NumVAOs];
GLuint Buffers[NumBuffers];

const GLuint Numvertices = 6;
int x_position = 0;
int z_position = 5;
int y_position = 0;
GLuint program;
glm::mat4 Model      = glm::mat4(1.0f);  // Changes for each model !


void display()
{

    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glBindVertexArray(VAOs[Triangles]);

    glm::mat4 Projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
    // Camera matrix
    glm::mat4 View       = glm::lookAt(
        glm::vec3(x_position, y_position, z_position), // Camera is at (4,3,3), in World Space
        glm::vec3(x_position, y_position, 0), // and looks at the origin
        glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
    );
    // Model matrix : an identity matrix (model will be at the origin)
    // Our ModelViewProjection : multiplication of our 3 matrices
    glm::mat4 MVP        = Projection * View * Model; // Remember, matrix multiplication is the other way around

    GLuint MatrixID = glGetUniformLocation(program, "MVP");
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

    //Draw the circle
    glDrawArrays(GL_LINE_LOOP, 0, 1000);
    //Draw the lines
    glDrawArrays(GL_LINES, 1000, 1000);

    glFlush();
    glutSwapBuffers();
}

void resize(int w, int h)
{
    glViewport(0, 0, w, h);
}

void init()
{
    //Load shader files
    ShaderInfo shaders[] = {
        {GL_VERTEX_SHADER, "./vertex.vert" },
        {GL_FRAGMENT_SHADER, "./fragment.frag" },
        {GL_NONE, NULL}
    };
    program = LoadShaders(shaders);
    glUseProgram(program);

    int amount = 1000;
    glm::vec2 circle_pos[amount*2];

    // calc degree to rad: PI / 180°
    int radius = 1.0;
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

    GLuint trianglebuffer;
    glGenBuffers(1, &trianglebuffer);
    glBindBuffer(GL_ARRAY_BUFFER, trianglebuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * 2 * amount,circle_pos, GL_STATIC_DRAW);

    glGenVertexArrays(NumVAOs, VAOs);
    glBindVertexArray(VAOs[Triangles]);

    glBindBuffer(GL_ARRAY_BUFFER, trianglebuffer);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

    glEnableVertexAttribArray(0);
}

void arrow_keys(int key, int x, int y)
{
    switch(key)
    {
    case GLUT_KEY_LEFT:
        x_position -=1;
        display();
        break;
    case GLUT_KEY_RIGHT:
        x_position += 1;
        display();
        break;
    case GLUT_KEY_UP:
        y_position += 1;
        display();
        break;
    case GLUT_KEY_DOWN:
        y_position -= 1;
        display();
        break;
    }
}

void keyboard(unsigned char key, int x, int y)
{
    switch(key)
    {
    case '-':
        z_position += 1;
        display();
        break;
    case '+':
        z_position -= 1;
        display();
        break;
    }
}

void timer(int value)
{
    Model = glm::rotate(Model, -0.3f, glm::vec3(0.0f, 0.0f, 1.0f));
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
    glutInitWindowSize(800, 600);
    glutInitContextVersion(2, 1);
    glutInitContextProfile(GLUT_CORE_PROFILE);
    glutCreateWindow("Shader example");

    glutReshapeFunc(resize);

    glutDisplayFunc(display);
    glutSpecialFunc(arrow_keys);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(25, timer, 0);
    if(glewInit()){
        cerr << "unable to init GLEW!" << endl;
        exit(EXIT_FAILURE);
    }
    init();
    glutMainLoop();

    return 0;
}

