#include <iostream>
#include "vgl.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"#include <math.h>
#include "LoadShaders.h"
#include "glm/glm/glm.hpp"
#include "glm/glm/gtc/matrix_transform.hpp"
#include "glm/glm/gtc/type_ptr.hpp"
#include "camera.h"
#include "triangle.h"
#include "TextureLoader.h"

#define FRONT "negz.jpg"
#define BACK "posz.jpg"
#define TOP "posy.jpg"
#define BOTTOM "negy.jpg"
#define LEFT "negx.jpg"
#define RIGHT "posx.jpg"
#define DEPTH 32
#define NUM_VERTICES (DEPTH+1)*(DEPTH+1)
#define NUM_INDICES 2*3*DEPTH*DEPTH

using namespace std;


enum Attrib_IDs { vPosition = 0 };
//0 - wheel 1- cube
enum VAO_IDs {wheel, cube, grid, normal, skybox};
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
GLuint cube_map_texture;
int numVertices;
glm::vec4 lightPos = glm::vec4(0.0, 1.5, 2.5, 1.0);

void create_wheel(float radius);
void create_cube(GLuint MyShader);
void create_grid(GLuint MyShader);
void create_skybox();

/* use stb_image to load an image file into memory, and then into one side of
a cube-map texture. */
bool load_cube_map_side(
    GLuint texture, GLenum side_target, const char* file_name
) {
    glBindTexture (GL_TEXTURE_CUBE_MAP, texture);

    int x, y, n;
    int force_channels = 4;
    unsigned char*  image_data = stbi_load(file_name, &x, &y, &n, force_channels);
    if (!image_data) {
        fprintf (stderr, "ERROR: could not load %s\n", file_name);
        return false;
    }
    // non-power-of-2 dimensions check
    if ((x & (x - 1)) != 0 || (y & (y - 1)) != 0) {
        fprintf (
            stderr, "WARNING: image %s is not power-of-2 dimensions\n", file_name
        );
    }

    // copy image data into 'target' side of cube map
    glTexImage2D (
        side_target,
        0,
        GL_RGBA,
        x,
        y,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        image_data
    );
    free (image_data);
    return true;
}

/* load all 6 sides of the cube-map from images, then apply formatting to the
final texture */
void create_cube_map (
    const char* front,
    const char* back,
    const char* top,
    const char* bottom,
    const char* left,
    const char* right,
    GLuint* tex_cube
) {
    // generate a cube-map texture to hold all the sides
    glActiveTexture (GL_TEXTURE0);
    glGenTextures (1, tex_cube);

    // load each image and copy into a side of the cube-map texture
    assert (load_cube_map_side (*tex_cube, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, front));
    assert (load_cube_map_side (*tex_cube, GL_TEXTURE_CUBE_MAP_POSITIVE_Z, back));
    assert (load_cube_map_side (*tex_cube, GL_TEXTURE_CUBE_MAP_POSITIVE_Y, top));
    assert (load_cube_map_side (*tex_cube, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, bottom));
    assert (load_cube_map_side (*tex_cube, GL_TEXTURE_CUBE_MAP_NEGATIVE_X, left));
    assert (load_cube_map_side (*tex_cube, GL_TEXTURE_CUBE_MAP_POSITIVE_X, right));
    // format cube map texture
    glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}
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

    GLuint lightUni = glGetUniformLocation(program, "lightPos" );
    glUniform4fv(lightUni, 1, glm::value_ptr(lightPos));
    glBindVertexArray(VAOs[skybox]);

    //Draw the cube
    glDrawArrays(GL_TRIANGLES, 0, 12*3);

    glBindVertexArray(VAOs[grid]);
    MVP = Projection * View * glm::mat4(1.0);
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
    //glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(VAOs[wheel]);
    MVP = Projection * View * Wheel_Model;
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 62);


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


    create_wheel(2.0);
    create_cube(shaders[0].shader);
  //  create_grid(shaders[0].shader);

    create_skybox();
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

void arrows(int key, int x, int y)
{
   switch(key)
   {
   case(GLUT_KEY_LEFT):
       lightPos.x -= 0.1;
       break;
   case(GLUT_KEY_RIGHT):
       lightPos.x += 0.1;
       break;
   case(GLUT_KEY_UP):
       lightPos.y += 0.1;
       break;
   case(GLUT_KEY_DOWN):
       lightPos.y -= 0.1;
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
    //Load Texture
    static const GLfloat g_uv_buffer_data[] = {
        1.1, 1.0,
        1.1, 0.0,
        0.1, 1.0,

        1.1, 0.0,
        0.1, 0.0,
        0.1, 1.0,
        0.667979f, 1.0f-0.335851f,
        0.336024f, 1.0f-0.671877f,
        0.667969f, 1.0f-0.671889f,
        1.000023f, 1.0f-0.000013f,
        0.668104f, 1.0f-0.000013f,
        0.667979f, 1.0f-0.335851f,
        0.000059f, 1.0f-0.000004f,
        0.335973f, 1.0f-0.335903f,
        0.336098f, 1.0f-0.000071f,
        0.667979f, 1.0f-0.335851f,
        0.335973f, 1.0f-0.335903f,
        0.336024f, 1.0f-0.671877f,
        1.000004f, 1.0f-0.671847f,
        0.999958f, 1.0f-0.336064f,
        0.667979f, 1.0f-0.335851f,
        0.668104f, 1.0f-0.000013f,
        0.335973f, 1.0f-0.335903f,
        0.667979f, 1.0f-0.335851f,
        0.335973f, 1.0f-0.335903f,
        0.668104f, 1.0f-0.000013f,
        0.336098f, 1.0f-0.000071f,
        0.000103f, 1.0f-0.336048f,
        0.000004f, 1.0f-0.671870f,
        0.336024f, 1.0f-0.671877f,
        0.000103f, 1.0f-0.336048f,
        0.336024f, 1.0f-0.671877f,
        0.335973f, 1.0f-0.335903f,
        0.667969f, 1.0f-0.671889f,
        1.000004f, 1.0f-0.671847f,
        0.667979f, 1.0f-0.335851f
    };

    GLuint uvbuffer;
    glGenBuffers(1, &uvbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_uv_buffer_data), g_uv_buffer_data, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glVertexAttribPointer(
        3,                                // attribute. No particular reason for 1, but must match the layout in the shader.
        2,                                // size : U+V => 2
        GL_FLOAT,                         // type
        GL_FALSE,                         // normalized?
        0,                                // stride
        (void*)0                          // array buffer offset
    );
    GLuint Texture = loadBMP_custom("uvtemplate.bmp");
        // Two UV coordinatesfor each vertex. They were created withe Blender.
    glEnableVertexAttribArray(3);
    glBindTexture(GL_TEXTURE_2D, Texture);
    GLuint TextureID = glGetUniformLocation(program, "myTextureSampler");
    glUniform1i(TextureID, 0);

    // Bind our texture in Texture Unit 0
    glActiveTexture(GL_TEXTURE0);
    // Set our "myTextureSampler" sampler to user Texture Unit 0
    glUniform1i(TextureID, 0);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
}

void create_wheel(float radius)
{
    int sides = 30;
const float theta = 2. * M_PI / (float)sides;
    float c = cos(theta);
    float s = sin(theta);
// coordinates on top of the circle, on xz plane
float x2 = radius, z2 = 0;
float x = 0.0;
float y = 0.0;
float z = 0.0;
int height = 5;

glm::vec3 Normals[sides + 4][2];
// make the strip
glm::vec3 Vertices[sides + 4][2];
glm::vec2 Texture[sides + 4][2];
for(int i=0; i<=sides; i++) {
    // texture coord
    const float tx = (float)i/sides;
    // normal
    const float nf = 1./sqrt(x2*x2+z2*z2),
        xn = x2*nf, zn = z2*nf;
    Vertices[i][0].x =  x+x2;
    Vertices[i][0].y =  y;
    Vertices[i][0].z =  z+z2;
    Vertices[i][1].x = x+x2;
    Vertices[i][1].y = y+height;
    Vertices[i][1].z = z+z2;
    Normals[i][0].x = xn;
    Normals[i][0].y = 0.0;
    Normals[i][0].z = zn;
    Normals[i][1].x = xn;
    Normals[i][1].y = 0.0;
    Normals[i][1].z = zn;
    Texture[i][0].x = tx;
    Texture[i][0].y = 0;
    Texture[i][1].x = tx;
    Texture[i][1].y = 1;
    // next position
    const float x3 = x2;
    x2 = c * x2 - s * z2;
    z2 = s * x3 + c * z2;
}
     glGenVertexArrays(1, &VAOs[wheel]);
     glBindVertexArray(VAOs[wheel]);
     GLuint wheelbuffer;
     glGenBuffers(1, &wheelbuffer);
     glBindBuffer(GL_ARRAY_BUFFER, wheelbuffer);
     glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices),Vertices, GL_STATIC_DRAW);
     glEnableVertexAttribArray(0);
     glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

     GLuint normalBuffer;
     glGenBuffers(1, &normalBuffer);
     glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
     glBufferData(GL_ARRAY_BUFFER, sizeof(Normals), Normals, GL_STATIC_DRAW);
     glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 0, BUFFER_OFFSET(0));
     glEnableVertexAttribArray(1);

     GLuint textureBuffer;
     glGenBuffers(1, &textureBuffer);
     glBindBuffer(GL_ARRAY_BUFFER, textureBuffer);
     glBufferData(GL_ARRAY_BUFFER, sizeof(Texture), Texture, GL_STATIC_DRAW);
     glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
     glEnableVertexAttribArray(2);
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

void create_skybox()
{
    float points[] = {
  -15.0f,  15.0f, -15.0f,
  -15.0f, -15.0f, -15.0f,
   15.0f, -15.0f, -15.0f,
   15.0f, -15.0f, -15.0f,
   15.0f,  15.0f, -15.0f,
  -15.0f,  15.0f, -15.0f,

  -15.0f, -15.0f,  15.0f,
  -15.0f, -15.0f, -15.0f,
  -15.0f,  15.0f, -15.0f,
  -15.0f,  15.0f, -15.0f,
  -15.0f,  15.0f,  15.0f,
  -15.0f, -15.0f,  15.0f,

   15.0f, -15.0f, -15.0f,
   15.0f, -15.0f,  15.0f,
   15.0f,  15.0f,  15.0f,
   15.0f,  15.0f,  15.0f,
   15.0f,  15.0f, -15.0f,
   15.0f, -15.0f, -15.0f,

  -15.0f, -15.0f,  15.0f,
  -15.0f,  15.0f,  15.0f,
   15.0f,  15.0f,  15.0f,
   15.0f,  15.0f,  15.0f,
   15.0f, -15.0f,  15.0f,
  -15.0f, -15.0f,  15.0f,

  -15.0f,  15.0f, -15.0f,
   15.0f,  15.0f, -15.0f,
   15.0f,  15.0f,  15.0f,
   15.0f,  15.0f,  15.0f,
  -15.0f,  15.0f,  15.0f,
  -15.0f,  15.0f, -15.0f,

  -15.0f, -15.0f, -15.0f,
  -15.0f, -15.0f,  15.0f,
   15.0f, -15.0f, -15.0f,
   15.0f, -15.0f, -15.0f,
  -15.0f, -15.0f,  15.0f,
   15.0f, -15.0f,  15.0f
};

    glGenVertexArrays(1, &VAOs[skybox]);
    glBindVertexArray(VAOs[skybox]);
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glBindBuffer (GL_ARRAY_BUFFER, buffer);
    glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    GLuint cube_map_texture;
    create_cube_map (FRONT, BACK, TOP, BOTTOM, LEFT, RIGHT, &cube_map_texture);
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
    glutInitContextVersion(3, 3);
    glutInitContextProfile(GLUT_CORE_PROFILE);
    glutCreateWindow("Shader example");

    glutReshapeFunc(resize);

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(arrows);
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

