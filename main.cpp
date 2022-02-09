#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp> //core glm functionality
#include <glm/gtc/matrix_transform.hpp> //glm extension for generating common transformation matrices
#include <glm/gtc/matrix_inverse.hpp> //glm extension for computing inverse matrices
#include <glm/gtc/type_ptr.hpp> //glm extension for accessing the internal data structure of glm types

#include "Window.h"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Model3D.hpp"
#include "SkyBox.hpp"
#include "irrKlang/include/irrKlang.h";

#include <windows.h>
#include <mmsystem.h>
#include <iostream>
#include <time.h>
#include <ctime>


using namespace irrklang;



const unsigned int SHADOW_WIDTH = 8192;
const unsigned int SHADOW_HEIGHT = 8192;

// window
gps::Window myWindow;

// matrices
glm::mat4 model;
glm::mat4 view;
glm::mat4 projection;
glm::mat3 normalMatrix;


// light parameters
glm::vec3 lightDir;
glm::vec3 lightColor;
glm::vec3 lampColor;

glm::mat4 lightRotation;

GLfloat constant = 1.0f;
GLfloat linear = 0.1f;
GLfloat quadratic = 0.1f;

GLfloat fogDensity = 0.0f;

glm::vec3 position = glm::vec3(4.50f, 0.60f, 0.0f);
glm::vec3 positions[] = { glm::vec3(-40.0f, 0.60f, -4.50f) ,glm::vec3(-30.0f, 0.60f, -4.50f),  glm::vec3(-20.0f, 0.60f, -4.50f), glm::vec3(-10.0f, 0.60f, -4.50f), glm::vec3(0.0f, 0.60f,  -4.50f), glm::vec3(10.0f, 0.60f,  -4.50f), glm::vec3(20.0f, 0.60f,  -4.50f), glm::vec3(30.0f, 0.60f,  -4.50f), glm::vec3(40.0f, 0.60f,  -4.50f), glm::vec3(50.0f, 0.60f,  -4.50f),
                        glm::vec3(-40.0f, 0.60f, 15.0f), glm::vec3(-30.0f, 0.60f, 15.0f),  glm::vec3(-20.0f, 0.60f, 15.0f), glm::vec3(-10.0f, 0.60f, 15.0f),  glm::vec3(0.0f, 0.60f, 15.0f), glm::vec3(10.0f, 0.60f, 15.0f), glm::vec3(20.0f, 0.60f, 15.0f), glm::vec3(30.0f, 0.60f, 15.0f), glm::vec3(40.0f, 0.60f, 15.0f), glm::vec3(50.0f, 0.60f, 15.0f) };

GLint day = 1;

bool fogOn = false;
float fogSpeed = 0.001f;

bool changeCol = false;
float r = 1.0;
float g = 0.35;
float b = 1.0;

bool rotate = false;


// shader uniform locations
GLuint modelLoc;
GLuint viewLoc;
GLuint projectionLoc;
GLuint normalMatrixLoc;
GLuint lightDirLoc;
GLuint lightColorLoc;
GLuint lampColorLoc;
GLuint constantLoc;
GLuint linearLoc;
GLuint quadraticLoc;
GLuint positionLoc;
GLuint positionsLoc;
GLuint dayLoc;
GLuint fogLoc;

// camera
gps::Camera myCamera(
    glm::vec3(0.0f, 0.0f, 3.0f),
    glm::vec3(0.0f, 0.0f, -10.0f),
    glm::vec3(0.0f, 1.0f, 0.0f));

GLfloat cameraSpeed = 0.1f;

GLboolean pressedKeys[1024];
double xcurr = 0.0f;
double ycurr = 0.0f;

// models
gps::Model3D scene;
gps::Model3D car;
gps::Model3D cat;

//skybox
gps::SkyBox mySkyBox;

GLfloat angle;
GLfloat pitch = 0.0f;
GLfloat yaw = 0.0f;
GLfloat lightAngle = 0.0f;
//animatie
GLfloat animatie = 0.0f;
bool forward = true;
float moveSpeed = 2;
double lastTimeStamp = glfwGetTime();

bool cameraAnimation = false;
int mom_of_anim = 0;
float camera_move = 0;
float rotationangle = 0.0f;

// shaders
gps::Shader myBasicShader;
gps::Shader skyboxShader;
gps::Shader depthMapShader;

GLuint shadowMapFBO;
GLuint depthMapTexture;

//for skybox
std::vector<const GLchar*> faces;

//for music
ISoundEngine* SoundEngine = createIrrKlangDevice();

//interactiune ob
glm::vec3 catPosition = glm::vec3(3.0f, -2.35f, -5.0f);


GLenum glCheckError_(const char* file, int line)
{
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR) {
        std::string error;
        switch (errorCode) {
        case GL_INVALID_ENUM:
            error = "INVALID_ENUM";
            break;
        case GL_INVALID_VALUE:
            error = "INVALID_VALUE";
            break;
        case GL_INVALID_OPERATION:
            error = "INVALID_OPERATION";
            break;
        case GL_STACK_OVERFLOW:
            error = "STACK_OVERFLOW";
            break;
        case GL_STACK_UNDERFLOW:
            error = "STACK_UNDERFLOW";
            break;
        case GL_OUT_OF_MEMORY:
            error = "OUT_OF_MEMORY";
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            error = "INVALID_FRAMEBUFFER_OPERATION";
            break;
        }
        std::cout << error << " | " << file << " (" << line << ")" << std::endl;
    }
    return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow* window, int width, int height) {
    fprintf(stdout, "Window resized! New width: %d , and height: %d\n", width, height);
    //TODO
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS) {
            pressedKeys[key] = true;
        }
        else if (action == GLFW_RELEASE) {
            pressedKeys[key] = false;
        }
    }
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    //TODO
    if (cameraAnimation == false) {
        if (xcurr == 0.0f && ycurr == 0.0f) {
            xcurr = xpos;
            ycurr = ypos;
        }
        else {
            double incrx = (xpos - xcurr) / 10;
            double incry = (ypos - ycurr) / 50;
            if ((pitch + incry < 79.0f) && (pitch + incry) > -89.0f) {
                pitch += incry;
            }

            yaw += incrx;
            myCamera.rotate(pitch, yaw);
            //update view matrix
            view = myCamera.getViewMatrix();
            myBasicShader.useShaderProgram();
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
            // compute normal matrix for teapot
            normalMatrix = glm::mat3(glm::inverseTranspose(view * model));

            xcurr = xpos;
            ycurr = ypos;
        }
    }
}

void initFaces()
{
    faces.push_back("skybox/posx.jpg"); //right
    faces.push_back("skybox/negx.jpg"); //left
    faces.push_back("skybox/posy.jpg"); //top
    faces.push_back("skybox/negy.jpg"); //bottom
    faces.push_back("skybox/posz.jpg"); //back   
    faces.push_back("skybox/negz.jpg"); //front
}

void initFacesNight()
{
    faces.push_back("skybox/noapteposx.jpeg"); //right
    faces.push_back("skybox/noaptenegx.jpeg"); //left
    faces.push_back("skybox/noapteposy.jpeg"); //top
    faces.push_back("skybox/noaptenegy.jpeg"); //bottom
    faces.push_back("skybox/noapteposz.jpeg"); //back   
    faces.push_back("skybox/noaptenegz.jpeg"); //front
}
void initSkyBoxShader()
{
    mySkyBox.Load(faces);
    skyboxShader.loadShader("shaders/skyboxShader.vert", "shaders/skyboxShader.frag");
    skyboxShader.useShaderProgram();

    //glUniformMatrix4fv(glGetUniformLocation(skyboxShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    view = myCamera.getViewMatrix();
    glUniformMatrix4fv(glGetUniformLocation(skyboxShader.shaderProgram, "view"), 1, GL_FALSE,
        glm::value_ptr(view));

    projection = glm::perspective(glm::radians(45.0f),
        (float)myWindow.getWindowDimensions().width / (float)myWindow.getWindowDimensions().height,
        0.1f, 100.0f);
    glUniformMatrix4fv(glGetUniformLocation(skyboxShader.shaderProgram, "projection"), 1, GL_FALSE,
        glm::value_ptr(projection));
}

void processMovement() {
    //normal
    if (pressedKeys[GLFW_KEY_1]) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    //wireframe
    if (pressedKeys[GLFW_KEY_2]) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    //point
    if (pressedKeys[GLFW_KEY_3]) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
    }
    if (pressedKeys[GLFW_KEY_W]) {
        myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    if (pressedKeys[GLFW_KEY_S]) {
        myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    if (pressedKeys[GLFW_KEY_A]) {
        if (cameraAnimation == false) {
            myCamera.move(gps::MOVE_LEFT, cameraSpeed);
            //update view matrix
            view = myCamera.getViewMatrix();
            myBasicShader.useShaderProgram();
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
            // compute normal matrix for teapot
            normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        }
        
    }

    if (pressedKeys[GLFW_KEY_D]) {
        if (cameraAnimation == false) {
            myCamera.move(gps::MOVE_RIGHT, cameraSpeed);
            //update view matrix
            view = myCamera.getViewMatrix();
            myBasicShader.useShaderProgram();
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
            // compute normal matrix for teapot
            normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        }
        
    }

    if (pressedKeys[GLFW_KEY_Q]) {
        angle -= 1.0f;
        // update model matrix for teapot
        model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
        // update normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    if (pressedKeys[GLFW_KEY_E]) {
        angle += 1.0f;
        // update model matrix for teapot
        model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
        // update normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }
    if (pressedKeys[GLFW_KEY_K])
    {
        if (linear < 1.5f && quadratic < 1.5f) {
            linear += 0.001f;
            quadratic += 0.001f;
        }
    }

    if (pressedKeys[GLFW_KEY_I])
    {
        if (linear > 0 && quadratic > 0) {
            linear -= 0.001f;
            quadratic -= 0.001f;
        }
    }
    //NOAPTE
    if (pressedKeys[GLFW_KEY_N])
    {
        faces.pop_back();
        faces.pop_back();
        faces.pop_back();
        faces.pop_back();
        faces.pop_back();
        faces.pop_back();
        initFacesNight();
        initSkyBoxShader();
        day = 0;
        dayLoc = glGetUniformLocation(myBasicShader.shaderProgram, "day");
        glUniform1i(dayLoc, day);
    }
    if (pressedKeys[GLFW_KEY_Z])
    {
        faces.pop_back();
        faces.pop_back();
        faces.pop_back();
        faces.pop_back();
        faces.pop_back();
        faces.pop_back();
        initFaces();
        initSkyBoxShader();
        day = 1;
        dayLoc = glGetUniformLocation(myBasicShader.shaderProgram, "day");
        glUniform1i(dayLoc, day);
    }
    if (pressedKeys[GLFW_KEY_F])
    {
        if (!fogOn) {
            if (fogDensity < 0.02f) {
                printf("fog on");
                fogOn = true;
            }
        }
        else {
            if (fogDensity > 0.02f) {
                printf("fog off");
                fogOn = false;
            }
        }
    }
    if (pressedKeys[GLFW_KEY_M]) {
        
        SoundEngine->play2D("audio/on-the-street-where-you-live.mp3", true);
    }
    if (pressedKeys[GLFW_KEY_V]) {

        cameraAnimation = true;
    }
    if (pressedKeys[GLFW_KEY_B]) {

        cameraAnimation = false;

    }
    if (pressedKeys[GLFW_KEY_P]) {
        changeCol = true;
        
    }

}

void initOpenGLWindow() {
    myWindow.Create(1024, 768, "OpenGL Project Core");

}

void setWindowCallbacks() {
    glfwSetWindowSizeCallback(myWindow.getWindow(), windowResizeCallback);
    glfwSetKeyCallback(myWindow.getWindow(), keyboardCallback);
    glfwSetCursorPosCallback(myWindow.getWindow(), mouseCallback);
    glfwSetInputMode(myWindow.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void initOpenGLState() {
    glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
    glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);
    glEnable(GL_FRAMEBUFFER_SRGB);
    glEnable(GL_DEPTH_TEST); // enable depth-testing
    glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
    glEnable(GL_CULL_FACE); // cull face
    glCullFace(GL_BACK); // cull back face
    glFrontFace(GL_CCW); // GL_CCW for counter clock-wise
}

void initModels() {
    scene.LoadModel("models/scene/Scene.obj");
    car.LoadModel("models/masina/car.obj");
    cat.LoadModel("models/pisica/pisica.obj");

}

void initShaders() {
    myBasicShader.loadShader(
        "shaders/basic.vert",
        "shaders/basic.frag");
    myBasicShader.useShaderProgram();
    depthMapShader.loadShader("shaders/depthMapShader.vert", "shaders/depthMapShader.frag");
    depthMapShader.useShaderProgram();
}


void initUniforms() {
    myBasicShader.useShaderProgram();

    // create model matrix for teapot
    model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    modelLoc = glGetUniformLocation(myBasicShader.shaderProgram, "model");

    // get view matrix for current camera
    view = myCamera.getViewMatrix();
    viewLoc = glGetUniformLocation(myBasicShader.shaderProgram, "view");
    // send view matrix to shader
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    // compute normal matrix for teapot
    normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    normalMatrixLoc = glGetUniformLocation(myBasicShader.shaderProgram, "normalMatrix");

    // create projection matrix
    projection = glm::perspective(glm::radians(45.0f),
        (float)myWindow.getWindowDimensions().width / (float)myWindow.getWindowDimensions().height,
        0.1f, 100.0f);
    projectionLoc = glGetUniformLocation(myBasicShader.shaderProgram, "projection");
    // send projection matrix to shader
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    //set the light direction (direction towards the light)
    lightDir = glm::vec3(0.0f, 6.0f, 6.0f);
    lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
    lightDirLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightDir");
    // send light dir to shader
    glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));
    

    //set light color
    lightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white light
    lightColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightColor");
    // send light color to shader
    glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

    //set lamps color
    lampColor = glm::vec3(1.0f, 0.5f, 0.1f); //orange light 06 02
    lampColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lampColor");
    // send light color to shader
    glUniform3fv(lampColorLoc, 1, glm::value_ptr(lampColor));
    //send constants loc
    constantLoc = glGetUniformLocation(myBasicShader.shaderProgram, "constant");
    glUniform1f(constantLoc, constant);//constant
    linearLoc = glGetUniformLocation(myBasicShader.shaderProgram, "linear");
    glUniform1f(linearLoc, linear);//linear
    quadraticLoc = glGetUniformLocation(myBasicShader.shaderProgram, "quadratic");
    glUniform1f(quadraticLoc, quadratic);//quadratic
    positionLoc = glGetUniformLocation(myBasicShader.shaderProgram, "position");
    glUniform3fv(positionLoc, 1, glm::value_ptr(position));

    positionsLoc = glGetUniformLocation(myBasicShader.shaderProgram, "positions");//vector
    glUniform3fv(positionsLoc, 20, glm::value_ptr(positions[0]));

    dayLoc = glGetUniformLocation(myBasicShader.shaderProgram, "day");
    glUniform1i(dayLoc, day);
    fogLoc = glGetUniformLocation(myBasicShader.shaderProgram, "fogDensity");
    glUniform1f(fogLoc, fogDensity);//fog

}

void initFBO() {
    //TODO - Create the FBO, the depth texture and attach the depth texture to the FBO
    glGenFramebuffers(1, &shadowMapFBO);

    //create depth texture for FBO
    glGenTextures(1, &depthMapTexture);
    glBindTexture(GL_TEXTURE_2D, depthMapTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
        SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    float borderColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    //attach texture to FBO
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture, 0);

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

glm::mat4 computeLightSpaceTrMatrix() {
    glm::mat4 lightView = glm::lookAt(glm::inverseTranspose(glm::mat3(lightRotation)) * lightDir, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    const GLfloat near_plane = -50.0f, far_plane = 50.0f;
    glm::mat4 lightProjection = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, near_plane, far_plane);
    glm::mat4 lightSpaceTrMatrix = lightProjection * lightView;

    return lightSpaceTrMatrix;
}




/*glm::mat4 computeLightSpaceTrMatrix() {
    //TODO - Return the light-space transformation matrix
    glm::mat4 lightView = glm::lookAt(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    const GLfloat near_plane = 0.1f, far_plane = 100.0f;
    glm::mat4 lightProjection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, near_plane, far_plane);
    glm::mat4 lightSpaceTrMatrix = lightProjection * lightView;
    return lightSpaceTrMatrix;
}*/

void renderTeapot(gps::Shader shader) {
    // select active shader program
    shader.useShaderProgram();

    //send teapot model matrix data to shader
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    //send teapot normal matrix data to shader
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    // draw teapot
    scene.Draw(shader);
}

void calcFogDensity(double elapsedseconds) {
    if (fogOn) {
        if (fogDensity < 0.05f)
            fogDensity += fogSpeed * elapsedseconds;
        else
            printf("max fog\n");
        fogLoc = glGetUniformLocation(myBasicShader.shaderProgram, "fogDensity");
        glUniform1f(fogLoc, fogDensity);//fog

    }
    else {
        if (fogDensity > 0.0f)
            fogDensity -= fogSpeed * elapsedseconds;
        fogLoc = glGetUniformLocation(myBasicShader.shaderProgram, "fogDensity");
        glUniform1f(fogLoc, fogDensity);
    }


}

void updateAnimatie(double elapsedSeconds) {
    if (forward) {
        if (animatie > 40.0f) {
            forward = false;
            rotate = true;
        }
            
    }
    else {
        if (animatie < -40.0f) {
            forward = true;
            rotate = true;
        }
            
    }
    if (forward) {
        if(!rotate)
            animatie = animatie + moveSpeed * elapsedSeconds;
    }
    else {
        if (!rotate)
            animatie = animatie - moveSpeed * elapsedSeconds;
    }

}

void drawObjects(gps::Shader shader, bool depthPass) {

    shader.useShaderProgram();
    model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));

    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

   

    if (!depthPass) {
        constantLoc = glGetUniformLocation(shader.shaderProgram, "constant");
        glUniform1f(constantLoc, constant);//constant
        linearLoc = glGetUniformLocation(shader.shaderProgram, "linear");
        glUniform1f(linearLoc, linear);//linear
        quadraticLoc = glGetUniformLocation(shader.shaderProgram, "quadratic");
        glUniform1f(quadraticLoc, quadratic);//quadratic
        positionLoc = glGetUniformLocation(shader.shaderProgram, "position");
        glUniform3fv(positionLoc, 1, glm::value_ptr(position));
        dayLoc = glGetUniformLocation(shader.shaderProgram, "day");
        glUniform1i(dayLoc, day);
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }


    // render the teapot
    scene.Draw(shader);

    

    glm::mat4 model1 = model;

    if (glm::length(myCamera.cameraPosition - catPosition) < 5.0f) {
        if (abs(myCamera.cameraPosition.x - catPosition.x) < 3.0f) {
            if (catPosition.x > myCamera.cameraPosition.x)
                catPosition.x = catPosition.x + 0.05f;
            else
                catPosition.x = catPosition.x - 0.05f;
        }
        if (abs(myCamera.cameraPosition.z - catPosition.z) < 3.0f) {
            if (catPosition.z > myCamera.cameraPosition.z)
                catPosition.z = catPosition.z + 0.05f;
            else
                catPosition.z = catPosition.z - 0.05f;
        }
    }
  
    model = glm::translate(model, catPosition);
    
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    cat.Draw(shader);
    model = model1;
    //animatie
    double currentTimeStamp = glfwGetTime();
    updateAnimatie(currentTimeStamp - lastTimeStamp);
    calcFogDensity(currentTimeStamp - lastTimeStamp);

    if (cameraAnimation == true) {
        if (mom_of_anim==0) {
            yaw = -90;
            myCamera.rotate(pitch, yaw);
            myCamera.move(gps::MOVE_LEFT, cameraSpeed* (currentTimeStamp - lastTimeStamp)*10);
            camera_move = camera_move + cameraSpeed * (currentTimeStamp - lastTimeStamp)*10;
            if (camera_move > 20)
                mom_of_anim = 1;
        }
        if (mom_of_anim == 1) {
            //yaw = 0;
            //myCamera.move(gps::MOVE_LEFT, cameraSpeed * (currentTimeStamp - lastTimeStamp) * 10);
            //camera_move = camera_move + cameraSpeed * (currentTimeStamp - lastTimeStamp) * 10;
            yaw = yaw + 1;
            myCamera.rotate(pitch, yaw);
            if (yaw>90)
                mom_of_anim = 2;
        }
        if (mom_of_anim == 2) {
            yaw = 90;
            myCamera.rotate(pitch, yaw);
            myCamera.move(gps::MOVE_LEFT, cameraSpeed * (currentTimeStamp - lastTimeStamp)*10);
            camera_move = camera_move - cameraSpeed * (currentTimeStamp - lastTimeStamp)*10;
            if (camera_move < -20)
                mom_of_anim = 3;
        }
        if (mom_of_anim == 3) {
            //yaw = 0;
            //myCamera.move(gps::MOVE_LEFT, cameraSpeed * (currentTimeStamp - lastTimeStamp) * 10);
            //camera_move = camera_move + cameraSpeed * (currentTimeStamp - lastTimeStamp) * 10;
            yaw = yaw - 1;
            myCamera.rotate(pitch, yaw);
            if (yaw < -90)
                mom_of_anim = 0;
        }
       
        //update view matrix
        shader.useShaderProgram();
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }
    //animatie culori lampadare
    if (changeCol) {

        b = b - (currentTimeStamp - lastTimeStamp) / 50;
        g = g + (currentTimeStamp - lastTimeStamp) / 500;
        if (b < 0.1f) {
            changeCol = false;
            r = 1.0;
            g = 0.35;
            b = 1.0;
        }           
        lampColor = glm::vec3(r, g, b);
        lampColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lampColor");
        // send light color to shader
        glUniform3fv(lampColorLoc, 1, glm::value_ptr(lampColor));
    }
    else {

        lampColor = glm::vec3(1.0f, 0.5f, 0.1f);
        lampColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lampColor");
        // send light color to shader
        glUniform3fv(lampColorLoc, 1, glm::value_ptr(lampColor));
    }



    lastTimeStamp = currentTimeStamp;
    model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    if (forward) {
        if (!rotate)
            model = glm::translate(model, glm::vec3(0.0f, -2.35f, animatie));
        else {
            rotationangle = rotationangle - 1.0f;
            
            model = glm::translate(model, glm::vec3(-4.0f, -2.35f, animatie));
            model = glm::rotate(model, glm::radians(rotationangle), glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::translate(model, glm::vec3(4.0f, 0.0f, 0.0f));
            if (rotationangle <= 0)
                rotate = false;
        }

    }    
    else {
        if (!rotate) {
            model = glm::translate(model, glm::vec3(-8.0f, -2.35f, animatie));
            model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        }
        else {
            rotationangle = rotationangle + 1.0f;
            model = glm::translate(model, glm::vec3(-4.0f, -2.35f, animatie));
            model = glm::rotate(model, glm::radians(-rotationangle), glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::translate(model, glm::vec3(4.0f, 0.0f , 0.0f));
            if (rotationangle >=180)
                rotate = false;
        }
        
    }
        
   
    

    // do not send the normal matrix if we are rendering in the depth map


    if (!depthPass) {
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }

    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    car.Draw(shader);


}

void renderScene() {

    depthMapShader.useShaderProgram();
    glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "lightSpaceTrMatrix"), 1, GL_FALSE, glm::value_ptr(computeLightSpaceTrMatrix()));
    glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);

    drawObjects(depthMapShader, true);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    //a doua randare

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);
    //render the scene
    myBasicShader.useShaderProgram();

    view = myCamera.getViewMatrix();
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));


    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, depthMapTexture);
    glUniform1i(glGetUniformLocation(myBasicShader.shaderProgram, "shadowMap"), 3);

    glUniformMatrix4fv(glGetUniformLocation(myBasicShader.shaderProgram, "lightSpaceTrMatrix"),
        1,
        GL_FALSE,
        glm::value_ptr(computeLightSpaceTrMatrix()));

    drawObjects(myBasicShader, false);
    mySkyBox.Draw(skyboxShader, view, projection);

}

void cleanup() {
    myWindow.Delete();
    //cleanup code for your own data
}

int main(int argc, const char* argv[]) {

    try {
        initOpenGLWindow();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    initFaces();

    initOpenGLState();
    initModels();
    initShaders();
    initUniforms();
    setWindowCallbacks();
    initFBO();
    initSkyBoxShader();

    lastTimeStamp = glfwGetTime();

    glCheckError();
    // application loop
    while (!glfwWindowShouldClose(myWindow.getWindow())) {
        processMovement();
        renderScene();

        glfwPollEvents();
        glfwSwapBuffers(myWindow.getWindow());

        glCheckError();
    }

    cleanup();

    return EXIT_SUCCESS;
}
