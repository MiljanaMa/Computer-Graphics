#define _CRT_SECURE_NO_WARNINGS
#define CRES 30 // Circle Resolution = Rezolucija kruga
#include <iostream>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string.h>
using namespace std;

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp> // For glm::radians function
#include "shader.h"
#include "camera.h"
#include "model.h"
#include "texture.h"

struct Input
{
    bool MoveLeft;
    bool MoveRight;
    bool LookLeft;
    bool LookRight;
    bool LookUp;
    bool LookDown;
    bool GoUp;
    bool GoDown;
};

struct EngineState
{
    Input* mInput;
    Camera* mCamera;
    unsigned mShadingMode;
    double mDT;
};
static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    EngineState* State = (EngineState*)glfwGetWindowUserPointer(window);
    Input* UserInput = State->mInput;
    bool IsDown = action == GLFW_PRESS || action == GLFW_REPEAT;
    switch (key)
    {
    case GLFW_KEY_A: UserInput->MoveLeft = IsDown; break;
    case GLFW_KEY_D: UserInput->MoveRight = IsDown; break;
    case GLFW_KEY_W: UserInput->GoUp = IsDown; break;
    case GLFW_KEY_S: UserInput->GoDown = IsDown; break;

    case GLFW_KEY_RIGHT: UserInput->LookLeft = IsDown; break;
    case GLFW_KEY_LEFT: UserInput->LookRight = IsDown; break;
    case GLFW_KEY_UP: UserInput->LookUp = IsDown; break;
    case GLFW_KEY_DOWN: UserInput->LookDown = IsDown; break;
    case GLFW_KEY_ESCAPE: glfwSetWindowShouldClose(window, GLFW_TRUE); break;
    }
}
static void HandleInput(EngineState* state)
{
    Input* UserInput = state->mInput;
    Camera* FPSCamera = state->mCamera;
    if (UserInput->MoveLeft) FPSCamera->Move(-1.0f, 0.0f, state->mDT);
    if (UserInput->MoveRight) FPSCamera->Move(1.0f, 0.0f, state->mDT);
    if (UserInput->LookLeft) FPSCamera->Rotate(1.0f, 0.0f, state->mDT);
    if (UserInput->LookRight) FPSCamera->Rotate(-1.0f, 0.0f, state->mDT);
    if (UserInput->LookDown) FPSCamera->Rotate(0.0f, -1.0f, state->mDT);
    if (UserInput->LookUp) FPSCamera->Rotate(0.0f, 1.0f, state->mDT);
    if (UserInput->GoUp) FPSCamera->UpDown(1);
    if (UserInput->GoDown) FPSCamera->UpDown(-1);
}

static void DrawSea(unsigned vao, const Shader& shader, unsigned diffuse, unsigned specular, double time, float rotationSpeed)
{
    glUseProgram(shader.GetId());
    glBindVertexArray(vao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuse);
    constexpr int sea_size = 10;
    glm::mat4 model_matrix(1.0f);
    float size = 4.0f;

    // Waves
    for (int i = -sea_size; i < sea_size; ++i)
    {
        for (int j = -sea_size; j < sea_size; ++j)
        {
            constexpr float size = 4.0f;
            glm::mat4 model_matrix(1.0f);

            // Waves
            model_matrix = glm::translate(model_matrix, glm::vec3(i * size, (abs(sin(time * rotationSpeed))) - size * 1.6, j * size));
            model_matrix = glm::rotate(model_matrix, glm::radians(static_cast<float>(time * rotationSpeed  * (45 + i))), glm::vec3(0.11, 0, 2));
            model_matrix = glm::scale(model_matrix, glm::vec3(size, size, size));
            shader.SetModel(model_matrix);
            glDrawArrays(GL_TRIANGLES, 0, 36);

            // Steady sea
            model_matrix = glm::mat4(1);
            model_matrix = glm::translate(model_matrix, glm::vec3(i * size, (abs(sin(time * rotationSpeed))) - size * 1.5, j * size));
            model_matrix = glm::scale(model_matrix, glm::vec3(size, size, size));
            shader.SetModel(model_matrix);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
    }
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);
}

int main(void)
{

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++ INITIALIZATION ++++++++++++++++++++++++++++++++++++++++++++++++++++++

    if (!glfwInit())
    {
        std::cout << "GLFW library didn't load! :(\n";
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window;
    unsigned int wWidth = 1920;
    unsigned int wHeight = 1080;
    const char wTitle[] = "[Islands]";
    int width;
    int height;
    glfwGetMonitorPhysicalSize(glfwGetPrimaryMonitor(), &width, &height);
    float scale = static_cast<float>(width) / height;
    int WindowWidth = glfwGetVideoMode(glfwGetPrimaryMonitor())->width;
    int WindowHeight = glfwGetVideoMode(glfwGetPrimaryMonitor())->height;

    window = glfwCreateWindow(WindowWidth, WindowHeight, wTitle, glfwGetPrimaryMonitor(), NULL);

    if (window == NULL)
    {
        std::cout << "Window is not made! :(\n";
        glfwTerminate();
        return 2;
    }

    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK)
    {
        std::cout << "GLEW didn't load! :'(\n";
        return 3;
    }
    EngineState State = { 0 };
    Camera FPSCamera;
    Input UserInput = { 0 };
    State.mCamera = &FPSCamera;
    State.mInput = &UserInput;
    glfwSetWindowUserPointer(window, &State);
    glfwSetKeyCallback(window, KeyCallback);
    
    Model shark("res/shark/SHARK.obj");
    if (!shark.Load())
    {
        std::cerr << "Failed to load model\n";
        glfwTerminate();
        return -1;
    }
    Model palm("res/palm/MY_PALM.obj");
    if (!palm.Load())
    {
        std::cerr << "Failed to load model\n";
        glfwTerminate();
        return -1;
    }

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++ PROMJENLJIVE I BAFERI +++++++++++++++++++++++++++++++++++++++++++++++++
    std::vector<float> CubeVertices =
    {
        // X     Y     Z     NX    NY    NZ    U     V    FRONT SIDE
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // L D
         0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, // R D
        -0.5f,  0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, // L U
         0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, // R D
         0.5f,  0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // R U
        -0.5f,  0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, // L U
        // LEFT SIDE
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // L D
        -0.5f, -0.5f,  0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // R D
        -0.5f,  0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // L U
        -0.5f, -0.5f,  0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // R D
        -0.5f,  0.5f,  0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // R U
        -0.5f,  0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // L U
        // RIGHT SIDE
        0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // L D
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // R D
        0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // L U
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // R D
        0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // R U
        0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // L U
        // BOTTOM SIDE
        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, // L D
         0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, // R D
        -0.5f, -0.5f,  0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // L U
         0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, // R D
         0.5f, -0.5f,  0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f, // R U
        -0.5f, -0.5f,  0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // L U
        // TOP SIDE
        -0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, // L D
         0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // R D
        -0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // L U
         0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // R D
         0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, // R U
        -0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // L U
        // BACK SIDE
        0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // L D
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, // R D
         0.5f,  0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, // L U
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, // R D
        -0.5f,  0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, // R U
         0.5f,  0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, // L U
    };
    float signatureVertices[] =
    {
        //  X     Y      H    V
          -1.0, -1.0,   0.0, 0.0,
           1.0, -1.0,   1.0, 0.0,
           1.0,  1.0,   1.0, 1.0,

          -1.0, -1.0,   0.0, 0.0,
           1.0,  1.0,   1.0, 1.0,
          -1.0,  1.0,   0.0, 1.0
    };

    unsigned CubeVAO;
    glGenVertexArrays(1, &CubeVAO);
    glBindVertexArray(CubeVAO);
    unsigned CubeVBO;
    glGenBuffers(1, &CubeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, CubeVBO);
    glBufferData(GL_ARRAY_BUFFER, CubeVertices.size() * sizeof(float), CubeVertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), static_cast<void*>(0));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    unsigned int VAO_signature, VBO_signature;
    glGenVertexArrays(1, &VAO_signature);
    glGenBuffers(1, &VBO_signature);

    // index and name attributes
    glBindVertexArray(VAO_signature);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_signature);

    glBufferData(GL_ARRAY_BUFFER, sizeof(signatureVertices), signatureVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, (2 + 2) * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, (2 + 2) * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // index and name texture
    unsigned signatureTexture = Texture::LoadImageToTexture("res/RA-123-2020.png");
    glBindTexture(GL_TEXTURE_2D, signatureTexture);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);


    Shader IndexShader("shaders/index.vert", "shaders/index.frag");
    Shader BlinnPhongShader("shaders/basic.vert", "shaders/blinn_phong.frag");
    glUseProgram(BlinnPhongShader.GetId());

    //fire
    BlinnPhongShader.SetUniform3f("uFireLight.Ka", glm::vec3(1.0, 1.0, 0.0));
    BlinnPhongShader.SetUniform3f("uFireLight.Kd", glm::vec3(1.0, 1.0, 0.0));
    BlinnPhongShader.SetUniform3f("uFireLight.Ks", glm::vec3(1.0, 1.0, 0.0));

    // Reflector 
    BlinnPhongShader.SetUniform3f("uReflectorLight.Ka", glm::vec3(0.5f, 0.0f, 0.5f));  // Ambient reflection coefficient (violet)
    BlinnPhongShader.SetUniform3f("uReflectorLight.Kd", glm::vec3(0.5f, 0.0f, 0.5f));  // Diffuse reflection coefficient (violet)
    BlinnPhongShader.SetUniform3f("uReflectorLight.Ks", glm::vec3(0.5f, 0.0f, 0.5f));  // Specular reflection coefficient (violet)
    BlinnPhongShader.SetUniform1f("uReflectorLight.Kc", 1.0f);                         // Constant attenuation
    BlinnPhongShader.SetUniform1f("uReflectorLight.Kl", 0.001f);                       // Linear attenuation
    BlinnPhongShader.SetUniform1f("uReflectorLight.Kq", 0.001f);                       // Quadratic attenuation
    BlinnPhongShader.SetUniform1f("uReflectorLight.InnerCutOff", glm::cos(glm::radians(3.0f)));  // Inner cutoff angle
    BlinnPhongShader.SetUniform1f("uReflectorLight.OuterCutOff", glm::cos(glm::radians(5.0f)));  // Outer cutoff angle

    // Materials
    BlinnPhongShader.SetUniform1i("uMaterial.Kd", 0.0);
    BlinnPhongShader.SetUniform1i("uMaterial.Ks", 1.0);
    BlinnPhongShader.SetUniform1f("uMaterial.Shininess", 32);

    //Textures
    unsigned SunDiffuseTexture = Texture::LoadImageToTexture("res/sun.jpg");
    unsigned MoonDiffuseTexture = Texture::LoadImageToTexture("res/moonTexture.jpg");

    unsigned SeaDiffuseTexture = Texture::LoadImageToTexture("res/sea_d.jpg");
    unsigned SeaSpecularTexture = Texture::LoadImageToTexture("res/sea_s.jpg");

    unsigned SandDiffuseTexture = Texture::LoadImageToTexture("res/sand_d.jpg");
    unsigned SandSpecularTexture = Texture::LoadImageToTexture("res/sea_s.jpg");

    unsigned FireDiffuseTexture = Texture::LoadImageToTexture("res/fire.jpg");
    unsigned CloudDiffuseTexture = Texture::LoadImageToTexture("res/cloudTexture.jpg");
    unsigned ReflectorDiffuseTexture = Texture::LoadImageToTexture("res/reflectorTexture.jpg");

    Shader* CurrentShader = &BlinnPhongShader;

    glm::mat4 projectionO = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, 0.00001f, 10000.0f);
    glm::mat4 projectionP = glm::perspective(glm::radians(45.0f), (float)WindowWidth / (float)WindowHeight, 0.001f, 1000.0f);
    glm::mat4 currentProjection = projectionO;
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    //parameters
    double pi = atan(1) * 4;
    double time;
    glm::mat4 model_matrix(1.0f);
    double cloudOffest = 0.0;
    float magnitude = 5.0;
    bool first = true;
    float speed = 0.2;
    float big_island_radius1 = 10.0f;
    float big_island_radius2 = 18.0f;
    float small_island_radius = 6.5f;
    float angle_of_shark = pi;
    float sunRadius = 55.0f;
    float rotationSpeed = 0.5;
    //sky
    double cosineValue;
    glm::vec3 nightColor = glm::vec3(0.0f, 0.0f, 0.2f);
    glm::vec3 duskColor = glm::vec3(0.5f, 0.5f, 0.8f);
    glm::vec3 dayColor = glm::vec3(0.8f, 0.8f, 1.0f);
    float red, green, blue;
    //sun
    glm::vec3 sun_position(0.0f, -8.0f, 0.0f);
    float sunColorIntensity;
    const float minSunColorIntensity = 0.1f;
    const float maxSunColorIntensity = 0.8f;
    float intensityPower = 0.3f;
    //moon
    float moonColorIntensity;
    //fire
    float yOffset;
    float attenuationFactor;
    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++ RENDER LOOP ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    while (!glfwWindowShouldClose(window))
    {
        time = glfwGetTime();
        CurrentShader = &BlinnPhongShader;
        glUseProgram(CurrentShader->GetId());
        cosineValue = cos(time * rotationSpeed);

        red = glm::mix(nightColor.r, dayColor.r, 0.5f + 0.5f * cosineValue);
        green = glm::mix(nightColor.g, dayColor.g, 0.5f + 0.5f * cosineValue);
        blue = glm::mix(nightColor.b, dayColor.b, 0.5f + 0.5f * cosineValue);

        glClearColor(red, green, blue, 1.0);

        if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
        {
            FPSCamera.SetOrthogonal();
            currentProjection = projectionO;
        }
        if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
        {
            currentProjection = projectionP;
        }
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
            glEnable(GL_PROGRAM_POINT_SIZE);
            glPointSize(8);
        }
        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        }
        if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        if (glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS)
        {
            magnitude += 0.5;
            rotationSpeed += 0.0005;
            speed += 0.0005;
        }
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
        {
            magnitude = 5.0;
            rotationSpeed = 0.2;
            cloudOffest = 0.0;
        }
        if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS)
        {
            if (magnitude >= 0.1) {
                magnitude -= 0.5;
            }
            if (rotationSpeed >= 0.1) {
                rotationSpeed -= 0.0005;
            }
        }
        if (cloudOffest >= 190.0) {
            cloudOffest = 0.0;
        }
        else
        {
            cloudOffest += 0.05 + (magnitude / 80.0) * 0.5;
        }

        glfwPollEvents();
        HandleInput(&State);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        CurrentShader->SetProjection(currentProjection);
        CurrentShader->SetView(glm::lookAt(FPSCamera.GetPosition(), FPSCamera.GetTarget(), FPSCamera.GetUp()));
        CurrentShader->SetUniform3f("uViewPos", FPSCamera.GetPosition());

        //Sun
        sunColorIntensity = 0.5f + 0.5f * sin(time * rotationSpeed + pi);
        sunColorIntensity = glm::clamp(sunColorIntensity, minSunColorIntensity, maxSunColorIntensity);
        glm::vec3 sunColor = glm::vec3(1.0f, 1.0f, 1.0f) * glm::pow(sunColorIntensity, intensityPower);
        CurrentShader->SetUniform3f("uSunLight.Position", glm::vec3(sunRadius * sin(time * rotationSpeed), sunRadius * cos(time * rotationSpeed), 0.0f));
        model_matrix = glm::mat4(1.0f);
        model_matrix = glm::translate(model_matrix, sun_position);
        model_matrix = glm::translate(model_matrix, glm::vec3(sunRadius * sin(time * rotationSpeed), sunRadius * cos(time * rotationSpeed), 0.0f));
        model_matrix = glm::scale(model_matrix, glm::vec3(7));

        if ((sunRadius * cos(time * rotationSpeed)) > 0.0f) {
            CurrentShader->SetUniform3f("uSunLight.Ka", sunColor);
            CurrentShader->SetUniform3f("uSunLight.Kd", glm::vec3(1.0f, 1.0f, 1.0f));
            CurrentShader->SetUniform3f("uSunLight.Ks", glm::vec3(1.0f, 1.0f, 1.0f));
            CurrentShader->SetModel(model_matrix);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, SunDiffuseTexture);
            glBindVertexArray(CubeVAO);
            glDrawArrays(GL_TRIANGLES, 0, CubeVertices.size() / 8);
        }
        else {
            CurrentShader->SetUniform3f("uSunLight.Ka", glm::vec3(0.0f));
            CurrentShader->SetUniform3f("uSunLight.Kd", glm::vec3(0.0f));
            CurrentShader->SetUniform3f("uSunLight.Ks", glm::vec3(0.0f));
        }
        
        //moon
        moonColorIntensity = 0.5f + 0.5f * sin(2 * time * rotationSpeed + pi);
        glm::vec3 moonColor = glm::vec3(0.4f, 0.4f, 0.4f) * (moonColorIntensity);
        CurrentShader->SetUniform3f("uMoonLight.Position", glm::vec3(sunRadius * sin(rotationSpeed * time + pi), sunRadius * cos(rotationSpeed * time + pi), 0.0f));
        model_matrix = glm::mat4(1.0f);
        model_matrix = glm::translate(model_matrix, sun_position);
        model_matrix = glm::translate(model_matrix, glm::vec3(sunRadius * sin(rotationSpeed * time + pi), sunRadius * cos(rotationSpeed * time + pi), 0.0f));
        model_matrix = glm::scale(model_matrix, glm::vec3(7));
        if ((sunRadius * cos(time * rotationSpeed + pi)) > 0.0f) {
            CurrentShader->SetUniform3f("uMoonLight.Ka", moonColor);
            CurrentShader->SetUniform3f("uMoonLight.Kd", glm::vec3(1.0f, 1.0f, 1.0f));
            CurrentShader->SetUniform3f("uMoonLight.Ks", glm::vec3(1.0f, 1.0f, 1.0f));
            CurrentShader->SetModel(model_matrix);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, MoonDiffuseTexture);
            glBindVertexArray(CubeVAO);
            glDrawArrays(GL_TRIANGLES, 0, CubeVertices.size() / 8);
        }
        else {
            CurrentShader->SetUniform3f("uMoonLight.Ka", glm::vec3(0.0f));
            CurrentShader->SetUniform3f("uMoonLight.Kd", glm::vec3(0.0f));
            CurrentShader->SetUniform3f("uMoonLight.Ks", glm::vec3(0.0f));
        }

        
        //fire
        yOffset = sin(time * 1.6f * rotationSpeed) * 0.2f;
        attenuationFactor = 0.1 / (abs(sin(time/2 * 1.6f *  rotationSpeed)) * 0.1f);
        CurrentShader->SetUniform1f("uFireLight.Kc", attenuationFactor);
        CurrentShader->SetUniform1f("uFireLight.Kq", attenuationFactor);
        CurrentShader->SetUniform1f("uFireLight.Kl", attenuationFactor);
        BlinnPhongShader.SetUniform3f("uFireLight.Kd", glm::vec3(1.0, 1.0f - (abs(sin(time/2 * 1.6f * rotationSpeed)) * 0.2f), 0.0));
        glm::vec3 firePosition(1.0f, -0.9f + yOffset, 1.0f);
        CurrentShader->SetUniform3f("uFireLight.Position", firePosition);
        model_matrix = glm::mat4(1.0f);
        model_matrix = glm::translate(model_matrix, firePosition);
        model_matrix = glm::scale(model_matrix, glm::vec3(1.0, 1.5, 0.5));
        CurrentShader->SetModel(model_matrix);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, FireDiffuseTexture);
        glBindVertexArray(CubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, CubeVertices.size() / 8);

        //reflector pillar
        model_matrix = glm::mat4(1.0f);
        model_matrix = glm::translate(model_matrix, glm::vec3(0.0f, 0.0f, 0.0f));
        model_matrix = glm::scale(model_matrix, glm::vec3(0.1, 7.0, 0.1));
        CurrentShader->SetModel(model_matrix);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, ReflectorDiffuseTexture);
        glDrawArrays(GL_TRIANGLES, 0, CubeVertices.size() / 8);

        //reflector head
        CurrentShader->SetUniform3f("uReflectorLight.Position", glm::vec3(0.0f, 3.3f, 0.0f));
        CurrentShader->SetUniform3f("uReflectorLight.Direction", glm::normalize(glm::vec3(10.0f * sin(pi + time * rotationSpeed), -4.25f, 10.0f * cos(pi + time * rotationSpeed)) - glm::vec3(0.0f, 2.3f, 0.0f)));
        model_matrix = glm::mat4(1.0f);
        model_matrix = glm::translate(model_matrix, glm::vec3(0.0f, 3.3f, 0.0f));
        model_matrix = glm::rotate(model_matrix, (float)time, glm::vec3(0.0f, 1.0f, 0.0f));
        model_matrix = glm::scale(model_matrix, glm::vec3(0.3));
        CurrentShader->SetModel(model_matrix);
        glDrawArrays(GL_TRIANGLES, 0, CubeVertices.size() / 8);
        
        //sharks
        model_matrix = glm::mat4(1.0f);
        for (int i = 0; i < 3; i++) {
            model_matrix = glm::mat4(1.0f);
            if (i == 0) {
                model_matrix = glm::translate(model_matrix, glm::vec3(big_island_radius1 * sin(angle_of_shark + time * rotationSpeed), -4.25f, big_island_radius1 * cos(angle_of_shark + time * rotationSpeed)));
            }
            else if (i == 1) {
                model_matrix = glm::translate(model_matrix, glm::vec3(2.0f, 0.0f, 2.0f));
                model_matrix = glm::translate(model_matrix, glm::vec3(big_island_radius2 * sin(angle_of_shark + time * rotationSpeed), -4.25f, big_island_radius2 * cos(angle_of_shark + time * rotationSpeed)));
            }
            else {
                model_matrix = glm::translate(model_matrix, glm::vec3(25.0f, 0.0f, 25.0f));
                model_matrix = glm::translate(model_matrix, glm::vec3(small_island_radius * sin(angle_of_shark + time * rotationSpeed), -4.25f, small_island_radius * cos(angle_of_shark + time * rotationSpeed)));
            }
            model_matrix = glm::rotate(model_matrix, (float)time * rotationSpeed, glm::vec3(0.0f, 1.0f, 0.0f));
            model_matrix = glm::scale(model_matrix, glm::vec3(1.2));
            CurrentShader->SetModel(model_matrix);
            shark.Render();
           
        }
        
        //palm
        model_matrix = glm::mat4(1.0f);
        model_matrix = glm::translate(model_matrix, glm::vec3(-1.5f, -1.5f, -1.5f));
        model_matrix = glm::scale(model_matrix, glm::vec3(0.01));
        model_matrix = glm::rotate(model_matrix, glm::radians(155.0f), glm::vec3(0, 1, 0));
        CurrentShader->SetModel(model_matrix);
        palm.Render();
        
        // Sea
        DrawSea(CubeVAO, *CurrentShader, SeaDiffuseTexture, SeaSpecularTexture, time, rotationSpeed);

        // Small island
        model_matrix = glm::mat4(1.0f);
        model_matrix = glm::translate(model_matrix, glm::vec3(25.0f, -2.7f, 25.0f));
        model_matrix = glm::scale(model_matrix, glm::vec3(4));
        CurrentShader->SetModel(model_matrix);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, SandDiffuseTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, SandSpecularTexture);
        glBindVertexArray(CubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, CubeVertices.size() / 8);

        // Big island
        model_matrix = glm::mat4(1.0f);
        model_matrix = glm::translate(model_matrix, glm::vec3(0.0f, -3.0f, 0.0f));
        model_matrix = glm::scale(model_matrix, glm::vec3(10, 3, 10));
        CurrentShader->SetModel(model_matrix);
        glBindVertexArray(CubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, CubeVertices.size() / 8);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, 0);
        
        //Clouds
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        model_matrix = glm::mat4(1.0f);
        model_matrix = glm::translate(model_matrix, glm::vec3(-50.0 + cloudOffest, 26.0, -15.0));
        model_matrix = glm::scale(model_matrix, glm::vec3(3));
        CurrentShader->SetModel(model_matrix);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, CloudDiffuseTexture);
        glBindVertexArray(CubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, CubeVertices.size() / 8);
        
        model_matrix = glm::mat4(1.0f);
        model_matrix = glm::translate(model_matrix, glm::vec3(-80.0 + cloudOffest, 28.0, -15.0));
        model_matrix = glm::scale(model_matrix, glm::vec3(4));
        CurrentShader->SetModel(model_matrix);
        glBindVertexArray(CubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, CubeVertices.size() / 8);
        
        glDisable(GL_DEPTH_TEST);

        CurrentShader = &IndexShader;
        glUseProgram(CurrentShader->GetId());
        glBindVertexArray(VAO_signature);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, signatureTexture);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);

        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);

        glBindVertexArray(0);
        glUseProgram(0);
        glfwSwapBuffers(window);
        State.mDT = glfwGetTime() - time;
        
    }
    glfwTerminate();
    return 0;
}

