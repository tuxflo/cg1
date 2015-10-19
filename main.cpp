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
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Camera matrix
    cam->calculatePositions();
    glm::mat4 Projection = cam->getProjectionMatrix();
    glm::mat4 View  = cam->getViewMatrix();    // Model matrix : an identity matrix (model will be at the origin)
    //Cube_Model = glm::translate(glm::mat4(1.0), glm::vec3(0.0, 1.5, 0.0));
    glm::mat4 MVP        = Projection * View * glm::mat4(1.0);

    //pass updated model to the shader (wheel)
    GLuint MatrixID = glGetUniformLocation(program, "MVP");
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

    GLuint lightUni = glGetUniformLocation(program, "lightPos");
    glm::vec3 light_position = glm::vec3(0.0, 3.0, 1.0);
    glUniformMatrix3fv(lightUni, 1, GL_FALSE, &light_position[0]);

    GLuint lightPosUni = glGetUniformLocation(program, "eyedirection");
    glm::vec3 tmp = cam->getPosition();
    glUniformMatrix3fv(lightPosUni, 1, GL_FALSE,  &tmp[0]);

    //Access Light uniform
    GLuint ambientLightID = glGetUniformLocation(program, "ambientLight");
    glm::vec3 ambientLight = glm::vec3(0.3, 0.3, 0.3);
    glUniform3fv(ambientLightID, 1, &ambientLight[0]);
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
   // create_grid(shaders[0].shader);

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
    Triangle cubeVertices[12];
    cubeVertices[0] = Triangle(
                glm::vec3(-1.0f,-1.0f,-1.0f), // triangle 1 : begin
                glm::vec3(-1.0f,-1.0f, 1.0f),
                glm::vec3(-1.0f, 1.0f, 1.0f)
                );
    cubeVertices[1] = Triangle(
                glm::vec3(1.0f, 1.0f,-1.0f), // triangle 1 : begin
                glm::vec3(-1.0f,-1.0f,-1.0f),
                glm::vec3(-1.0f, 1.0f,-1.0f)
                );
    cubeVertices[2] = Triangle(
                glm::vec3(1.0f,-1.0f, 1.0f), // triangle 1 : begin
                glm::vec3(-1.0f,-1.0f,-1.0f),
                glm::vec3(1.0f,-1.0f,-1.0f)
                );
    cubeVertices[3] = Triangle(
                glm::vec3(1.0f, 1.0f,-1.0f), // triangle 1 : begin
                glm::vec3(1.0f,-1.0f,-1.0f),
                glm::vec3(-1.0f,-1.0f,-1.0f)
                );
    cubeVertices[4] = Triangle(
                glm::vec3(-1.0f,-1.0f,-1.0f), // triangle 1 : begin
                glm::vec3(-1.0f, 1.0f, 1.0f),
                glm::vec3(-1.0f, 1.0f,-1.0f)
                );
    cubeVertices[5] = Triangle(
                glm::vec3(1.0f,-1.0f, 1.0f), // triangle 1 : begin
                glm::vec3(-1.0f,-1.0f, 1.0f),
                glm::vec3(-1.0f,-1.0f,-1.0f)
                );
    cubeVertices[6] = Triangle(
                glm::vec3(-1.0f, 1.0f, 1.0f), // triangle 1 : begin
                glm::vec3(-1.0f,-1.0f, 1.0f),
                glm::vec3(1.0f,-1.0f, 1.0f)
                );
    cubeVertices[7] = Triangle(
                glm::vec3(1.0f, 1.0f, 1.0f), // triangle 1 : begin
                glm::vec3(1.0f,-1.0f,-1.0f),
                glm::vec3(1.0f, 1.0f,-1.0f)
                );
    cubeVertices[8] = Triangle(
                glm::vec3(1.0f,-1.0f,-1.0f), // triangle 1 : begin
                glm::vec3(1.0f, 1.0f, 1.0f),
                glm::vec3(1.0f,-1.0f, 1.0f)
                );
    cubeVertices[9] = Triangle(
                glm::vec3(1.0f, 1.0f, 1.0f), // triangle 1 : begin
                glm::vec3(1.0f, 1.0f,-1.0f),
                glm::vec3(-1.0f, 1.0f,-1.0f)
                );
    cubeVertices[10] = Triangle(
                glm::vec3(1.0f, 1.0f, 1.0f), // triangle 1 : begin
                glm::vec3(-1.0f, 1.0f,-1.0f),
                glm::vec3(-1.0f, 1.0f, 1.0f)
                );
    cubeVertices[11] = Triangle(
                glm::vec3(1.0f, 1.0f, 1.0f), // triangle 1 : begin
                glm::vec3(-1.0f, 1.0f, 1.0),
                glm::vec3(1.0f,-1.0f, 1.0f)
                );

    GLuint posLoc = glGetAttribLocation(1, "pos");
    GLuint colorLoc = glGetAttribLocation(1, "Incolor");
    GLuint normalLoc = glGetAttribLocation(1, "normal");
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
        0.583f,  0.771f,  0.014f,
        0.609f,  0.115f,  0.436f,
        0.327f,  0.483f,  0.844f,
        0.822f,  0.569f,  0.201f,
        0.435f,  0.602f,  0.223f,
        0.310f,  0.747f,  0.185f,
        0.597f,  0.770f,  0.761f,
        0.559f,  0.436f,  0.730f,
        0.359f,  0.583f,  0.152f,
        0.483f,  0.596f,  0.789f,
        0.559f,  0.861f,  0.639f,
        0.195f,  0.548f,  0.859f,
        0.014f,  0.184f,  0.576f,
        0.771f,  0.328f,  0.970f,
        0.406f,  0.615f,  0.116f,
        0.676f,  0.977f,  0.133f,
        0.971f,  0.572f,  0.833f,
        0.140f,  0.616f,  0.489f,
        0.997f,  0.513f,  0.064f,
        0.945f,  0.719f,  0.592f,
        0.543f,  0.021f,  0.978f,
        0.279f,  0.317f,  0.505f,
        0.167f,  0.620f,  0.077f,
        0.347f,  0.857f,  0.137f,
        0.055f,  0.953f,  0.042f,
        0.714f,  0.505f,  0.345f,
        0.783f,  0.290f,  0.734f,
        0.722f,  0.645f,  0.174f,
        0.302f,  0.455f,  0.848f,
        0.225f,  0.587f,  0.040f,
        0.517f,  0.713f,  0.338f,
        0.053f,  0.959f,  0.120f,
        0.393f,  0.621f,  0.362f,
        0.673f,  0.211f,  0.457f,
        0.820f,  0.883f,  0.371f,
        0.982f,  0.099f,  0.879f
    };

    GLuint colorbuffer;
    glGenBuffers(1, &colorbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);

    // 2nd attribute buffer : colors
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
    glVertexAttribPointer(
                colorLoc,                                // attribute. No particular reason for 1, but must match the layout in the shader.
                3,                                // size
                GL_FLOAT,                         // type
                GL_FALSE,                         // normalized?
                0,                                // stride
                (void*)0                          // array buffer offset
                );

    glm::vec3 g_normal_buffer_data[12];
    for(int i=0; i<11;i++)
    {
        g_normal_buffer_data[i] = cubeVertices[i].getNormal();
    }

    GLuint normalbuffer;
    glGenBuffers(1, &normalbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_normal_buffer_data), g_normal_buffer_data, GL_STATIC_DRAW);

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

    glGenVertexArrays(1, &VAOs[grid]);
    glBindVertexArray(VAOs[grid]);
    GLuint trianglebuffer;
    glGenBuffers(1, &trianglebuffer);
    glBindBuffer(GL_ARRAY_BUFFER, trianglebuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

    GLuint colorbuffer;
    glGenBuffers(1, &colorbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

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

