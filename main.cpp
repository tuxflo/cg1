#include <iostream>
#include "vgl.h"
#include <math.h>
#include "LoadShaders.h"
#include "glm/glm/glm.hpp"
#include "glm/glm/gtc/matrix_transform.hpp"
#include "glm/glm/gtc/type_ptr.hpp"
#include "camera.h"
#include "triangle.h"

#define DEPTH 32
#define NUM_VERTICES (DEPTH+1)*(DEPTH+1)
#define NUM_INDICES 2*3*DEPTH*DEPTH

using namespace std;


enum Attrib_IDs { vPosition = 0 };
//0 - wheel 1- cube
enum VAO_IDs {wheel, cube, grid, normal};
// Number of VAOs: 3
GLuint VAOs[3];
const GLuint Numvertices = 6;
GLuint program;
glm::mat4 Wheel_Model      = glm::mat4(1.0f);  // Changes for each model !
glm::mat4 Cube_Model = glm::mat4(1.0);
Camera *cam;
Camera static_cam(true);
Camera fps_cam(false);
static double lastTime;
GLuint wheelbuffer;
GLuint wheelarray;
GLuint cubearray;

void create_wheel(float radius);
void create_cube(GLuint MyShader);
void create_grid(GLuint MyShader);

void display()
{
    //white background
    glClearColor(0.3, 0.3, 0.3, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Camera matrix
    cam->calculatePositions();
    glm::mat4 Projection = cam->getProjectionMatrix();
    glm::mat4 View  = cam->getViewMatrix();    // Model matrix : an identity matrix (model will be at the origin)
    //Cube_Model = glm::translate(glm::mat4(1.0), glm::vec3(0.0, 1.0, 0.0));
    Cube_Model = glm::mat4(1.0);
    glm::mat4 MVP        = Projection * View * Cube_Model;

    //pass updated model to the shader (wheel)
    GLuint MatrixID = glGetUniformLocation(program, "MVP");
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

    //pass Model Matrix to shader
    GLuint M_ID = glGetUniformLocation(program, "m");
    glm::mat4 M = Cube_Model;
    glUniformMatrix4fv(M_ID, 1, GL_FALSE, &M[0][0]);
    //pass Model Matrix to shader

    GLuint V_ID = glGetUniformLocation(program, "v");
    glm::mat4 V = View;
    glUniformMatrix4fv(V_ID, 1, GL_FALSE, &V[0][0]);

    GLuint P_ID = glGetUniformLocation(program, "p");
    glm::mat4 P = cam->getProjectionMatrix();
    glUniformMatrix4fv(P_ID, 1, GL_FALSE, &P[0][0]);

    GLuint uniform_m_3x3_inv_transp = glGetUniformLocation(program, "m_3x3_inv_transp");
    glm::mat3 m_3x3_inv_transp = glm::transpose(glm::inverse(glm::mat3(Cube_Model)));
  glUniformMatrix3fv(uniform_m_3x3_inv_transp, 1, GL_FALSE, glm::value_ptr(m_3x3_inv_transp));

    GLuint lightUni = glGetUniformLocation(program, "LightPos" );
    glm::vec3 light_position = glm::vec3(0.0,1.0, 0.0);
    glUniformMatrix3fv(lightUni, 1, GL_FALSE, &light_position[0]);

    glBindVertexArray(VAOs[cube]);

    //Draw the cube
    glDrawArrays(GL_TRIANGLES, 0, 12*3);

    glBindVertexArray(VAOs[grid]);
    MVP = Projection * View * glm::mat4(1.0);
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
    //glDrawArrays(GL_TRIANGLES, 0, 6);

    glFlush();
    glutSwapBuffers();
    double currentTime = glutGet(GLUT_ELAPSED_TIME);
    float deltaTime = float(currentTime - lastTime);
    cam->setDeltaTime(deltaTime/1000);

    glutPostRedisplay();
    lastTime = currentTime;
}

void resize(int w, int h)
{
    glViewport(0, 0, w, h);
}

void init()
{
    cam = &fps_cam;
    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);

    lastTime = glutGet(GLUT_ELAPSED_TIME);
    //Load shader files
    ShaderInfo shaders[] = {
        {GL_VERTEX_SHADER, "./vertex.vert" },
        {GL_FRAGMENT_SHADER, "./fragment.frag" },
        {GL_NONE, NULL}
    };
    program = LoadShaders(shaders);


//    create_wheel(2.0);
    create_cube(shaders[0].shader);
    create_grid(shaders[0].shader);

    glUseProgram(program);
    glutWarpPointer(1024/2, 768/2);
}

void keyboard(unsigned char key, int x, int y)
{
    switch(key)
    {
    case 'w':
        cam->up();
        break;
    case 's':
        cam->down();
        break;
    case 'a':
        cam->left();
        break;
    case 'd':
        cam->right();
        break;
    case '1':
        std::cout << "switching to static camera" << std::endl;
        cam = &static_cam;
        break;
    case '2':
        std::cout << "switching to fps camera" << std::endl;
        cam = &fps_cam;
        break;
    }
}

void create_cube(GLuint MyShader)
{
    GLfloat cubeVertices[] =
        {
                // In OpenGL counter-clockwise winding is default. This means that when we look at a triangle,
                // if the points are counter-clockwise we are looking at the "front". If not we are looking at
                // the back. OpenGL has an optimization where all back-facing triangles are culled, since they
                // usually represent the backside of an object and aren't visible anyways.

                // Front face
                -1.0f, 1.0f, 1.0f,
                -1.0f, -1.0f, 1.0f,
                1.0f, 1.0f, 1.0f,
                -1.0f, -1.0f, 1.0f,
                1.0f, -1.0f, 1.0f,
                1.0f, 1.0f, 1.0f,

                // Right face
                1.0f, 1.0f, 1.0f,
                1.0f, -1.0f, 1.0f,
                1.0f, 1.0f, -1.0f,
                1.0f, -1.0f, 1.0f,
                1.0f, -1.0f, -1.0f,
                1.0f, 1.0f, -1.0f,

                // Back face
                1.0f, 1.0f, -1.0f,
                1.0f, -1.0f, -1.0f,
                -1.0f, 1.0f, -1.0f,
                1.0f, -1.0f, -1.0f,
                -1.0f, -1.0f, -1.0f,
                -1.0f, 1.0f, -1.0f,

                // Left face
                -1.0f, 1.0f, -1.0f,
                -1.0f, -1.0f, -1.0f,
                -1.0f, 1.0f, 1.0f,
                -1.0f, -1.0f, -1.0f,
                -1.0f, -1.0f, 1.0f,
                -1.0f, 1.0f, 1.0f,

                // Top face
                -1.0f, 1.0f, -1.0f,
                -1.0f, 1.0f, 1.0f,
                1.0f, 1.0f, -1.0f,
                -1.0f, 1.0f, 1.0f,
                1.0f, 1.0f, 1.0f,
                1.0f, 1.0f, -1.0f,

                // Bottom face
                1.0f, -1.0f, -1.0f,
                1.0f, -1.0f, 1.0f,
                -1.0f, -1.0f, -1.0f,
                1.0f, -1.0f, 1.0f,
                -1.0f, -1.0f, 1.0f,
                -1.0f, -1.0f, -1.0f,
        };
    GLuint posLoc = glGetAttribLocation(1, "v_coord");
    GLuint colorLoc = glGetAttribLocation(1, "v_color");
    GLuint normalLoc = glGetAttribLocation(1, "v_normal");
    //called trigangle but is drawing the cube
    glGenVertexArrays(1, &VAOs[cube]);
    glBindVertexArray(VAOs[cube]);
    GLuint trianglebuffer;
    glGenBuffers(1, &trianglebuffer);
    glBindBuffer(GL_ARRAY_BUFFER, trianglebuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

    // One color for each vertex. They were generated randomly.
    static const GLfloat g_color_buffer_data[] = {
                // Front face (red)
                1.0f, 0.0f, 0.0f, 1.0f,
                1.0f, 0.0f, 0.0f, 1.0f,
                1.0f, 0.0f, 0.0f, 1.0f,
                1.0f, 0.0f, 0.0f, 1.0f,
                1.0f, 0.0f, 0.0f, 1.0f,
                1.0f, 0.0f, 0.0f, 1.0f,

                // Right face (green)
                0.0f, 1.0f, 0.0f, 1.0f,
                0.0f, 1.0f, 0.0f, 1.0f,
                0.0f, 1.0f, 0.0f, 1.0f,
                0.0f, 1.0f, 0.0f, 1.0f,
                0.0f, 1.0f, 0.0f, 1.0f,
                0.0f, 1.0f, 0.0f, 1.0f,

                // Back face (blue)
                0.0f, 0.0f, 1.0f, 1.0f,
                0.0f, 0.0f, 1.0f, 1.0f,
                0.0f, 0.0f, 1.0f, 1.0f,
                0.0f, 0.0f, 1.0f, 1.0f,
                0.0f, 0.0f, 1.0f, 1.0f,
                0.0f, 0.0f, 1.0f, 1.0f,

                // Left face (yellow)
                1.0f, 1.0f, 0.0f, 1.0f,
                1.0f, 1.0f, 0.0f, 1.0f,
                1.0f, 1.0f, 0.0f, 1.0f,
                1.0f, 1.0f, 0.0f, 1.0f,
                1.0f, 1.0f, 0.0f, 1.0f,
                1.0f, 1.0f, 0.0f, 1.0f,

                // Top face (cyan)
                0.0f, 1.0f, 1.0f, 1.0f,
                0.0f, 1.0f, 1.0f, 1.0f,
                0.0f, 1.0f, 1.0f, 1.0f,
                0.0f, 1.0f, 1.0f, 1.0f,
                0.0f, 1.0f, 1.0f, 1.0f,
                0.0f, 1.0f, 1.0f, 1.0f,

                // Bottom face (magenta)
                1.0f, 0.0f, 1.0f, 1.0f,
                1.0f, 0.0f, 1.0f, 1.0f,
                1.0f, 0.0f, 1.0f, 1.0f,
                1.0f, 0.0f, 1.0f, 1.0f,
                1.0f, 0.0f, 1.0f, 1.0f,
                1.0f, 0.0f, 1.0f, 1.0f
        };
    GLuint colorbuffer;
    glGenBuffers(1, &colorbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);

    // 2nd attribute buffer : colors
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
    glVertexAttribPointer(
                colorLoc,                                // attribute. No particular reason for 1, but must match the layout in the shader.
                4,                                // size
                GL_FLOAT,                         // type
                GL_FALSE,                         // normalized?
                0,                                // stride
                (void*)0                          // array buffer offset
                );

    GLfloat cubeNormals[] = {
                // Front face
                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f,

                // Right face
                1.0f, 0.0f, 0.0f,
                1.0f, 0.0f, 0.0f,
                1.0f, 0.0f, 0.0f,
                1.0f, 0.0f, 0.0f,
                1.0f, 0.0f, 0.0f,
                1.0f, 0.0f, 0.0f,

                // Back face
                0.0f, 0.0f, -1.0f,
                0.0f, 0.0f, -1.0f,
                0.0f, 0.0f, -1.0f,
                0.0f, 0.0f, -1.0f,
                0.0f, 0.0f, -1.0f,
                0.0f, 0.0f, -1.0f,

                // Left face
                -1.0f, 0.0f, 0.0f,
                -1.0f, 0.0f, 0.0f,
                -1.0f, 0.0f, 0.0f,
                -1.0f, 0.0f, 0.0f,
                -1.0f, 0.0f, 0.0f,
                -1.0f, 0.0f, 0.0f,

                // Top face
                0.0f, 1.0f, 0.0f,
                0.0f, 1.0f, 0.0f,
                0.0f, 1.0f, 0.0f,
                0.0f, 1.0f, 0.0f,
                0.0f, 1.0f, 0.0f,
                0.0f, 1.0f, 0.0f,

                // Bottom face
                0.0f, -1.0f, 0.0f,
                0.0f, -1.0f, 0.0f,
                0.0f, -1.0f, 0.0f,
                0.0f, -1.0f, 0.0f,
                0.0f, -1.0f, 0.0f,
                0.0f, -1.0f, 0.0f
    };
    GLuint normalbuffer;
    glGenBuffers(1, &normalbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeNormals), cubeNormals, GL_STATIC_DRAW);

    glVertexAttribPointer(
                normalLoc,                                // attribute. No particular reason for 1, but must match the layout in the shader.
                3,                                // size
                GL_FLOAT,                         // type
                GL_FALSE,                         // normalized?
                0,                                // stride
                (void*)0                          // array buffer offset
                );
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
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

void create_grid(GLuint MyShader)
{
    static const GLfloat g_vertex_buffer_data[] = {
        -100.0f,0.0f,100.0f, // triangle 1 : begin
        100.0f,0.0f, 100.0f,
        -100.0f, 0.0f, -100.0f, // triangle 1 : end
        -100.0f, 0.0f,-100.0f, // triangle 2 : begin
        100.0f, 0.0f,-100.0f,
        100.0f, 0.0f,100.0f, // triangle 2 : end
    };

    static const GLfloat g_color_buffer_data[] = {
        0.0f,1.0f,0.0f, // triangle 1 : begin
        0.0f,1.0f, 0.0f,
        0.0f, 1.0f, 0.0f, // triangle 1 : end
        0.0f, 1.0f,0.0f, // triangle 2 : begin
        0.0f, 1.0f,0.0f,
        0.0f, 1.0f,0.0f, // triangle 2 : end
    };

    GLuint posLoc = glGetAttribLocation(1, "pos");
    GLuint colorLoc = glGetAttribLocation(1, "Incolor");
    GLuint normalLoc = glGetAttribLocation(1, "normal");
    glGenVertexArrays(1, &VAOs[grid]);
    glBindVertexArray(VAOs[grid]);
    GLuint trianglebuffer;
    glGenBuffers(1, &trianglebuffer);
    glBindBuffer(GL_ARRAY_BUFFER, trianglebuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

    GLuint colorbuffer;
    glGenBuffers(1, &colorbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

    static const GLfloat normal_data[] = {
        0.0, 1.0, 0.0,
        0.0, 1.0, 0.0,
        0.0, 1.0, 0.0,
        0.0, 1.0, 0.0,
        0.0, 1.0, 0.0,
        0.0, 1.0, 0.0,
    };
    GLuint normalBuffer;
    glGenBuffers(1, &normalBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(normal_data), normal_data, GL_STATIC_DRAW);

    glVertexAttribPointer(
                normalLoc,
                3,
                GL_FLOAT,
                GL_TRUE,
                0,
                (void*)0
                );
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
}

void mouse_func(int x, int y)
{
    cam->setPos(x, y);
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

