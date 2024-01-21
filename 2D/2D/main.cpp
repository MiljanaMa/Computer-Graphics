#define _CRT_SECURE_NO_WARNINGS
#define CRES 30 // Circle Resolution
#include <iostream>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string.h>
using namespace std;

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

unsigned int compileShader(GLenum type, const char* source);
unsigned int createShader(const char* vsSource, const char* fsSource);
static unsigned loadImageToTexture(const char* filePath);

int main(void)
{

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++ INITIALIZATION ++++++++++++++++++++++++++++++++++++++++++++++++++++++

    if (!glfwInit())
    {
        std::cout << "GLFW Biblioteka se nije ucitala! :(\n";
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
    window = glfwCreateWindow(wWidth, wHeight, wTitle, glfwGetPrimaryMonitor(), NULL);

    if (window == NULL)
    {
        std::cout << "Window is not made! :(\n";
        glfwTerminate();
        return 2;
    }

    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK)
    {
        std::cout << "GLEW did not load! :'(\n";
        return 3;
    }

    float seaVertices[] =
    {
        -1.0, -1.0, 0.0, 0.0,
        1.0, -1.0, 1.0, 0.0,
        -1.0, 0.0, 0.0, 1.0,
        1.0, 0.0, 1.0, 1.0,
    };
    float indexVertices[] =
    {
        0.9, 0.9, 0.0, 0.0,
        1.0, 0.9, 1.0, 0.0,
        0.9, 1.0, 0.0, 1.0,
        1.0, 1.0, 1.0, 1.0,
    };
    float islandVertices[] =
    {
        -0.7, 0.0, 0.0, 0.0,
        -0.6, 0.0, 1.0, 0.0,
        -0.7, 0.1, 0.0, 1.0,
        -0.6, 0.1, 1.0, 1.0,
    };
    float bigIsland[] =
    {
        -0.1, -0.1, 0.0, 0.0,
        0.2, -0.1, 1.0, 0.0,
        -0.1, 0.1, 0.0, 1.0,
        0.2, 0.1, 1.0, 1.0,
    };
    float cloudVertices[] =
    {
        1.0, 0.6, 0.0, 0.0,
        1.15, 0.6, 1.0, 0.0,
        1.0, 0.75, 0.0, 1.0,
        1.15, 0.75, 1.0, 1.0,
    };

    float sharksVertices[] =
    {
        -0.7, -0.5, 0.0, 0.0, 0.0, 0.0,
        -0.6, -0.5, 0.0, 0.0, 0.0, 0.0,
        -0.65, -0.45, 0.0, 0.0, 0.0, 0.0,
    };
    float fireVertices[] =
    {
        0.1, 0.1,
        0.12, 0.13,
        0.14, 0.1
    };

    //moon and sun
    float circle[CRES * 2 + 4];
    float r = 0.1;

    circle[0] = 0.0;
    circle[1] = 0.0;
    int i;
    for (i = 0; i <= CRES; i++)
    {

        circle[2 + 2 * i] = r * cos((3.141592 / 180) * (i * 360 / CRES)) / scale; //+ 0.8;
        circle[2 + 2 * i + 1] = r * sin((3.141592 / 180) * (i * 360 / CRES));
    }

    float palmTreeVertices[] =
    {
        0.0, 0.0, 0.0, 0.0,
        0.2, 0.0, 1.0, 0.0,
        0.0, 0.5, 0.0, 1.0,
        0.2, 0.5, 1.0, 1.0,
    };

    float draganMarkovicPalma[] =
    {
        0.0, 0.1, 0.0, 0.0,
        0.2, 0.1, 1.0, 0.0,
        0.0, 0.5, 0.0, 1.0,
        0.2, 0.5, 1.0, 1.0,
    };
    unsigned int indices[] =
    {
        0, 1, 2,
        0, 2, 3,
    };
    unsigned int stride = (2 + 4) * sizeof(float); 

    unsigned int VAO[9];
    glGenVertexArrays(9, VAO);
    unsigned int VBO[9];
    glGenBuffers(9, VBO);

    //sea
    glBindVertexArray(VAO[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(seaVertices), seaVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, (2 + 2) * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, (2 + 2) * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //small islands
    glBindVertexArray(VAO[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(islandVertices), islandVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, (2 + 2) * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, (2 + 2) * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //big island
    glBindVertexArray(VAO[2]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(bigIsland), bigIsland, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, (2 + 2) * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, (2 + 2) * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //sharks
    glBindVertexArray(VAO[3]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sharksVertices), sharksVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //sun and moon
    glBindVertexArray(VAO[4]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[4]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(circle), circle, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    //clouds
    glBindVertexArray(VAO[5]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[5]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cloudVertices), cloudVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, (2 + 2) * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, (2 + 2) * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //palm tree
    glBindVertexArray(VAO[6]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[6]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(palmTreeVertices), palmTreeVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, (2 + 2) * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, (2 + 2) * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //fire
    glBindVertexArray(VAO[7]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[7]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(fireVertices), fireVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    //index
    glBindVertexArray(VAO[8]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[8]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(indexVertices), indexVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, (2 + 2) * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, (2 + 2) * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //islands
    unsigned int bottomOscilationShader = createShader("bottomOscilationShader.vert", "texture.frag");
    unsigned int bottomOscilation = glGetUniformLocation(bottomOscilationShader, "uPos");
    //sea
    unsigned int topOscilationShader = createShader("topOscilationShader.vert", "texture.frag");
    unsigned int topOscilation = glGetUniformLocation(topOscilationShader, "uPos");
    //palm, index, clouds
    unsigned int transparentShader = createShader("cloud.vert", "transparentTexture.frag");
    unsigned int transparent = glGetUniformLocation(transparentShader, "uPos");
    //sun, moon
    unsigned int basicShader = createShader("basic.vert", "basic.frag");
    unsigned int basicPos = glGetUniformLocation(basicShader, "uPos");
    unsigned int basicCol = glGetUniformLocation(basicShader, "inColVec");
    //fire
    unsigned int fireShader = createShader("fire.vert", "basic.frag");
    unsigned int firePos = glGetUniformLocation(fireShader, "uPos");
    unsigned int fireCol = glGetUniformLocation(fireShader, "inColVec");

    //sea
    unsigned seaTexture = loadImageToTexture("res/seaNew.jpg");
    glBindTexture(GL_TEXTURE_2D, seaTexture);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
    unsigned uTexLoc = glGetUniformLocation(topOscilationShader, "uTex");
    glUniform1i(uTexLoc, 0);

    //island
    unsigned sandTexture = loadImageToTexture("res/island.jpg");
    glBindTexture(GL_TEXTURE_2D, sandTexture);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
    unsigned uTexLoc5 = glGetUniformLocation(bottomOscilationShader, "uTex");
    glUniform1i(uTexLoc5, 0);

    //index
    unsigned indexTexture = loadImageToTexture("res/index.jpg");
    glBindTexture(GL_TEXTURE_2D, indexTexture);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
    unsigned uTexLoc1 = glGetUniformLocation(bottomOscilationShader, "uTex");
    glUniform1i(uTexLoc1, 0);

    //palm tree
    unsigned palmTexture = loadImageToTexture("res/palm.png");
    glBindTexture(GL_TEXTURE_2D, palmTexture);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
    unsigned uTexLoc2 = glGetUniformLocation(transparentShader, "uTex");
    glUniform1i(uTexLoc2, 0);

    //Dragan Markovic Palma
    unsigned draganMarkovicTexture = loadImageToTexture("res/markovic.png");
    glBindTexture(GL_TEXTURE_2D, draganMarkovicTexture);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
    unsigned uTexLoc3 = glGetUniformLocation(transparentShader, "uTex");
    glUniform1i(uTexLoc3, 0);

    unsigned cloudTexture = loadImageToTexture("res/cloud.png");
    glBindTexture(GL_TEXTURE_2D, cloudTexture);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
    unsigned uTexLoc4 = glGetUniformLocation(transparentShader, "uTex");
    glUniform1i(uTexLoc4, 0);

    float magnitude = 5.0;
    float rotationSpeed = 0.2;
    double cloudOffest = 0.0;
    float skyColor[3];
    float put = 0;
    bool showPalm = false;
    float speed = 3199.0;
    double time;
    double sineValue;

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++ RENDER LOOP  +++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    while (!glfwWindowShouldClose(window))
    {
        time = glfwGetTime();
        glfwPollEvents();
        if (cloudOffest >= 2.4) {
            cloudOffest = 0.0;
        }
        else 
        {
            cloudOffest += 0.0001 + (magnitude / 80.0) * 0.005;
        }

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, GL_TRUE);
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
            magnitude += 0.005;
            rotationSpeed += 0.0005;
            speed += 10.0;
        }
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
        {
            magnitude = 3.0;
            rotationSpeed = 0.2;
        }
        if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS)
        {
            if (magnitude >= 0.1) {
                magnitude -= 0.005;
            }
            if (rotationSpeed >= 0.1) {
                rotationSpeed -= 0.0005;
            }
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            showPalm = true;
        }
        if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
        {
            showPalm = false;
        }
        
        sineValue = sin(time * rotationSpeed);
        
        float red = 0.5f + 0.2f * sineValue;
        float green = 0.6f + 0.2f * sineValue;
        float blue = 1.0f + 0.5f * sineValue;

        glClearColor(red, green, blue, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        //index
        glUseProgram(transparentShader);
        glBindVertexArray(VAO[8]);
        glUniform2f(transparent, 0.0, 0.0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, indexTexture);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindTexture(GL_TEXTURE_2D, 0);

        glUseProgram(basicShader);
        glBindVertexArray(VAO[4]);
        //sun
        glUniform2f(basicPos, 1.05 * sin(time * rotationSpeed - 1.5707963268), 0.8 * cos(time * rotationSpeed - 1.5707963268));
        glUniform4f(basicCol, 1.0, 1.0, 0.0, 0.0);
        glDrawArrays(GL_TRIANGLE_FAN, 0, sizeof(circle) / (2 * sizeof(float)));
        //moon
        glUniform2f(basicPos,  1.05 * sin(time * rotationSpeed + 1.5707963268), 0.8 * cos(time * rotationSpeed + 1.5707963268));
        glUniform4f(basicCol, 0.5, 0.5, 0.5, 0.0);
        glDrawArrays(GL_TRIANGLE_FAN, 0, sizeof(circle) / (2 * sizeof(float)));

        //palm
        glUseProgram(transparentShader);
        glBindVertexArray(VAO[6]);
        glUniform2f(transparent, 0.1, 0.0);
        glActiveTexture(GL_TEXTURE0);
        if (!showPalm) {
            glBindTexture(GL_TEXTURE_2D, palmTexture);
        }
        else {
            glBindTexture(GL_TEXTURE_2D, draganMarkovicTexture);
        }
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindTexture(GL_TEXTURE_2D, 0);

        //fire
        glUseProgram(fireShader);
        glBindVertexArray(VAO[7]);
        glUniform2f(firePos, 0.0, 0.01 * sin(time * magnitude / 2.0));
        glUniform4f(fireCol, 1.0, abs(sin((magnitude / 2.0 ) * time)), 0.0, 0.0);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        //clouds
        glUseProgram(transparentShader);
        glBindVertexArray(VAO[5]);
        //first cloud
        glUniform2f(transparent, cloudOffest, 0.0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, cloudTexture);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        //second cloud
        glUniform2f(transparent, cloudOffest - 0.16, 0.01);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        //third cloud
        glUniform2f(transparent, cloudOffest - 0.25, 0.1);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        //up cloud
        glUniform2f(transparent, cloudOffest - 0.1, 0.1);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindTexture(GL_TEXTURE_2D, 0);
        
        //sea
        glUseProgram(topOscilationShader);
        glBindVertexArray(VAO[0]);
        glUniform2f(topOscilation, 0.0, 0.02 * sin(time * magnitude));
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, seaTexture);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindTexture(GL_TEXTURE_2D, 0);
        
        //shark behind
        glUseProgram(basicShader);
        glBindVertexArray(VAO[3]);
        glUniform2f(basicPos, -0.8 + 0.9 * abs(sin(rotationSpeed * time)), (0.4 + 0.02 * sin(time * magnitude)));
        glUniform4f(basicCol, 0.0, 0.0, 0.0, 0.0);
        glDrawArrays(GL_TRIANGLES, 0, sizeof(sharksVertices) / stride);

        glUniform2f(basicPos, -1.5 + 0.4 * abs(sin(rotationSpeed * time)), (0.2 + 0.02 * sin(magnitude * time)));
        glDrawArrays(GL_TRIANGLES, 0, sizeof(sharksVertices) / stride);
        
        //small islands
        glUseProgram(bottomOscilationShader);
        glBindVertexArray(VAO[1]);
        glUniform2f(bottomOscilation, 0.0, 0.02 * sin(time * magnitude));
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, sandTexture);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glUniform2f(bottomOscilation, 1.2, 0.02 * sin(time * magnitude));
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        //big island
        glBindVertexArray(VAO[2]);
        glUniform2f(bottomOscilation, 0.0, 0.02 * sin(time * magnitude));
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindTexture(GL_TEXTURE_2D, 0);

        //shark front
        glUseProgram(basicShader);
        glBindVertexArray(VAO[3]);
        glUniform2f(basicPos, -1.0 + 1.1 * abs(sin((rotationSpeed + 0.1) * time)), 0.02 * sin(magnitude * glfwGetTime()));
        glUniform4f(basicCol, 0.0, 0.0, 0.0, 0.0);
        glDrawArrays(GL_TRIANGLES, 0, sizeof(sharksVertices) / stride);
        
        glBindVertexArray(0);
        glUseProgram(0);
        glfwSwapBuffers(window);
        
    }

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++ CLEAN +++++++++++++++++++++++++++++++++++++++++++++++++
    glDeleteTextures(1, &topOscilationShader);
    glDeleteTextures(1, &bottomOscilationShader);
    glDeleteTextures(1, &transparentShader);
    glDeleteBuffers(9, VBO);
    glDeleteVertexArrays(9, VAO);
    glDeleteProgram(basicShader);
    glDeleteProgram(fireShader);
    glDeleteProgram(bottomOscilationShader);
    glDeleteProgram(topOscilationShader);
    glDeleteProgram(transparentShader);
    glfwTerminate();
    return 0;
}

unsigned int compileShader(GLenum type, const char* source)
{
    std::string content = "";
    std::ifstream file(source);
    std::stringstream ss;
    if (file.is_open())
    {
        ss << file.rdbuf();
        file.close();
        std::cout << "Successfully read the file from the path. \"" << source << "\"!" << std::endl;
    }
    else {
        ss << "";
        std::cout << "Error reading file from path.\"" << source << "\"!" << std::endl;
    }
     std::string temp = ss.str();
     const char* sourceCode = temp.c_str();

    int shader = glCreateShader(type);
    
    int success;
    char infoLog[512];
    glShaderSource(shader, 1, &sourceCode, NULL);
    glCompileShader(shader);

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        if (type == GL_VERTEX_SHADER)
            printf("VERTEX");
        else if (type == GL_FRAGMENT_SHADER)
            printf("FRAGMENT");
        printf("Shader has an error! Error: \n");
        printf(infoLog);
    }
    return shader;
}
unsigned int createShader(const char* vsSource, const char* fsSource)
{
    unsigned int program;
    unsigned int vertexShader;
    unsigned int fragmentShader;

    program = glCreateProgram();

    vertexShader = compileShader(GL_VERTEX_SHADER, vsSource);
    fragmentShader = compileShader(GL_FRAGMENT_SHADER, fsSource);

    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    glLinkProgram(program);
    glValidateProgram(program);

    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_VALIDATE_STATUS, &success);
    if (success == GL_FALSE)
    {
        glGetShaderInfoLog(program, 512, NULL, infoLog);
        std::cout << "Unified shader has an error! Error: \n";
        std::cout << infoLog << std::endl;
    }

    glDetachShader(program, vertexShader);
    glDeleteShader(vertexShader);
    glDetachShader(program, fragmentShader);
    glDeleteShader(fragmentShader);

    return program;
}
static unsigned loadImageToTexture(const char* filePath) {
    int TextureWidth;
    int TextureHeight;
    int TextureChannels;
    unsigned char* ImageData = stbi_load(filePath, &TextureWidth, &TextureHeight, &TextureChannels, 0);
    if (ImageData != NULL)
    {
        stbi__vertical_flip(ImageData, TextureWidth, TextureHeight, TextureChannels);

        GLint InternalFormat = -1;
        switch (TextureChannels) {
        case 1: InternalFormat = GL_RED; break;
        case 3: InternalFormat = GL_RGB; break;
        case 4: InternalFormat = GL_RGBA; break;
        default: InternalFormat = GL_RGB; break;
        }

        unsigned int Texture;
        glGenTextures(1, &Texture);
        glBindTexture(GL_TEXTURE_2D, Texture);
        glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, TextureWidth, TextureHeight, 0, InternalFormat, GL_UNSIGNED_BYTE, ImageData);
        glBindTexture(GL_TEXTURE_2D, 0);
        stbi_image_free(ImageData);
        return Texture;
    }
    else
    {
        std::cout << "Texture not loaded! Path to texture: " << filePath << std::endl;
        stbi_image_free(ImageData);
        return 0;
    }
}
