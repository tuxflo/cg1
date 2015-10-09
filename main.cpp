#include <iostream>
#include "vgl.h"
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
int x_position = 30;
int z_position = 60;
int y_position = 0;
GLuint program;

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
    glm::mat4 Model      = glm::mat4(1.0f);  // Changes for each model !
    // Our ModelViewProjection : multiplication of our 3 matrices
    glm::mat4 MVP        = Projection * View * Model; // Remember, matrix multiplication is the other way around

    GLuint MatrixID = glGetUniformLocation(program, "MVP");
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);


    glDrawArrays(GL_TRIANGLES, 0, Numvertices);
    glDrawArrays(GL_LINES, 6, 8);
    glDrawArrays(GL_TRIANGLES, 14, Numvertices);
    glDrawArrays(GL_LINES, 20, 4);
    //glDrawArrays(GL_TRIANGLES, 24, Numvertices);
    glDrawArraysInstanced(GL_TRIANGLES, 24, Numvertices, 3);

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


    GLfloat vertices[36][2] =
    {
        { 0.0, 0.0 },
        { 3.0, 0.0 },
        { 0.0, 15.0 },
        { 0.0, 15.0 },
        { 3.0, 15.0 },
        { 3.0, 0.0 },

        //Line 4
        { 3.0, 7.5 },
        { 10.0, 7.5 },
        { 10.0, 7.5 },
        { 10.0, 0.0 },
        { 10.0, 0.0 },
        { 10.0, 15.0 },
        { 10.0, 15.0 },
        { 15.0, 15.0 },

        //Triangles
        { 0.0 + 15.0, 0.0 },
        { 3.0 + 15.0, 0.0 },
        { 0.0 + 15.0, 15.0 },
        { 0.0 +15.0, 15.0 },
        { 3.0 + 15.0, 15.0 },
        { 3.0 + 15.0, 0.0 },

        //Line 6
        { 22.0, 0.0 },
        { 22.0, 15.0 },
        { 22.0, 15.0 },
        { 26.0, 15.0 },

        //Triangle
        { 22.0, 0.0 },
        { 25.0, 0.0 },
        { 31.0, 15.0 },
        { 31.0, 15.0 },
        { 34.0, 15.0 },
        { 25.0, 0.0 },

        //Triangle
        { 27.0, 0.0 },
        { 30.0, 0.0 },
        { 36.0, 15.0 },
        { 36.0, 15.0 },
        { 39.0, 15.0 },
        { 30.0, 0.0 }
    };

    GLuint trianglebuffer;
    glGenBuffers(1, &trianglebuffer);
    glBindBuffer(GL_ARRAY_BUFFER, trianglebuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

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
    if(glewInit()){
        cerr << "unable to init GLEW!" << endl;
        exit(EXIT_FAILURE);
    }
    init();
    glutMainLoop();

    return 0;
}

