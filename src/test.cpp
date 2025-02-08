#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

#include <myHeaders/camera.h>
#include <myHeaders/shader.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>


void makeQuad(Shader lightShader, unsigned int VAOcube, glm::mat4 projection, glm::mat4 view, glm::vec3 lightColor, glm::vec3 pos, glm::vec3 scale);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInputFly(GLFWwindow* window);
void processInputPlayer(GLFWwindow* window);

unsigned int loadTexture(char const* path);
unsigned int loadCubemap(const char* faces[]);
void settingShader(Shader lightingShader, Camera camera, glm::vec3* pointLightPositions, glm::vec3 lightPos, glm::vec3 lightColor);
 glm::vec3 calculateFaceNormal(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2);
// settings
const unsigned int SCR_WIDTH = 1800;
const unsigned int SCR_HEIGHT = 1600;

const int resolution = 800; 

glm::vec3 start_pos = glm::vec3(-7.0f, 11.0f, 5.0f);

Camera camera(start_pos);

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

float depth = -5.0;


// set up vertex data (and buffer(s)) and configure vertex attributes
// ------------------------------------------------------------------
float skyboxVertices[] = {
    // positions          
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f
};


float cubeVertices[] = {
    //  vertices             TexCoord      Normals  
        -0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 0.0f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  0.0f,  0.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, -1.0f,  0.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  1.0f,  0.0f

};


glm::vec3 pointLightPositions[] = {
    glm::vec3(-28.5253, 3.06318, 53.6281),
    glm::vec3(20.3f, 0.9f, 5.0f),
    glm::vec3(-6.26907, 3.13895, 25.4411),
    glm::vec3(30.0f,  1.6f, 3.0f)
};
const unsigned int NUM_PLATFORMS = 5;
glm::vec3 platformPositions[] = {
    start_pos - glm::vec3(0.0f,2.0f,0.0f),
    start_pos - glm::vec3(0.0f,2.0f,-8.0f),
    start_pos - glm::vec3(0.0f,2.0f,-16.0f),
    start_pos - glm::vec3(10.0f,2.0f,-16.0f),
    start_pos - glm::vec3(-10.0f,2.0f,-16.0f)
};

glm::vec3 platformScales[] = {
    glm::vec3(2.0f, 0.1f, 4.0f),
    glm::vec3(1.5f, 0.1f, 2.0f),
    glm::vec3(1.0f, 0.1f, 1.0f),
    glm::vec3(4.0f, 0.1f, 10.0f),
    glm::vec3(8.0f, 0.1f, 10.0f)
};

std::vector<glm::vec2> texCoords;

std::vector<glm::vec3> vertices;
std::vector<glm::vec3> normals;
std::vector<glm::uint> indices;



int main()

{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    

    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    // glfw window creation
   //-----
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "UnserTollesOpenGLProjekt", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);


    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);


    // build and compile our shader program

    Shader waterShader("src/resources/shaders/Water.shader");
    Shader CubeMapShader("src/resources/shaders/cubeMap.shader");
    Shader TextureShader("src/resources/shaders/TextureShader.shader");
    Shader lightShader("src/resources/shaders/lightShader.shader");
   
    waterShader.use();


    // Calculating points
    for (int i = 0; i < resolution; ++i) {
        for (int j = 0; j < resolution; ++j) {
            glm::vec3 v = glm::vec3((i - (resolution / 2)) * 0.1f, 0.0f, j * 0.1f);
   
            vertices.push_back(v);
            //std::cout << currentPos.position.x << "\n";
            normals.push_back(glm::vec3(0, 1.0, 0));

            float u = static_cast<float>(i) / (resolution - 1);
            float vTex = static_cast<float>(j) / (resolution - 1);
            texCoords.push_back(glm::vec2(u, vTex)*glm::vec2(40.0));
        }
    }

    for (int i = 0; i < resolution - 1; ++i) {
        for (int j = 0; j < resolution - 1; ++j) {
            indices.push_back((i * resolution) + (j));
            indices.push_back(((i + 1) * resolution) + (j));
            indices.push_back((i * resolution) + (j + 1));

            indices.push_back(((i + 1) * resolution) + (j));
            indices.push_back(((i + 1) * resolution) + (j + 1));
            indices.push_back((i * resolution) + (j + 1));
        }
    }


    for (int i = 0; i < depth; ++i) {
        for (int j = 0; j < resolution; ++j) {
            glm::vec3 v = glm::vec3((i - (resolution / 2)) * 0.1f, j * 0.1, resolution);

            float u = static_cast<float>(i) / (resolution - 1);
            float vTex = static_cast<float>(j) / (resolution - 1);
            texCoords.push_back(glm::vec2(u, vTex) * glm::vec2(40.0));
        }
    }




    //cube = plattform wo drauf man steht?
    unsigned int VBOPlane,VAOplane, VBOcube,VAOcube;
    glGenVertexArrays(1, &VAOplane);
    glGenBuffers(1, &VBOPlane);
    glGenVertexArrays(1, &VAOcube);
    glGenBuffers(1, &VBOcube);
    // setup
    glBindVertexArray(VAOplane);	// note that we bind to a different VAO now
    glBindBuffer(GL_ARRAY_BUFFER, VBOPlane);	// and a different VBO
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), glm::value_ptr(vertices[0]), GL_STATIC_DRAW);
    // note that we update the lamp's position attribute's stride to reflect the updated buffer data
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);


    unsigned int IBO;
    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    //---cube---
    glGenVertexArrays(1, &VAOcube);
    glBindVertexArray(VAOcube);

    glBindBuffer(GL_ARRAY_BUFFER, VBOcube);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    // note that we update the lamp's position attribute's stride to reflect the updated buffer data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    //normals
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    unsigned int lenght = indices.size() * 4;

    unsigned int wallTexture;
    wallTexture =loadTexture("src/resources/Textures/tiles.jpg");


    const char* CubMapsdir[6] = {
    "src/resources/Textures/SkyBox/right.jpg",
    "src/resources/Textures/SkyBox/left.jpg",
    "src/resources/Textures/SkyBox/top.jpg",
    "src/resources/Textures/SkyBox/bottom.jpg",
    "src/resources/Textures/SkyBox/front.jpg",
    "src/resources/Textures/SkyBox/back.jpg"
    };
    unsigned int cubemapTexture = loadCubemap(CubMapsdir);

    //---Texture for Platforms---
    unsigned int platformTexture;
    platformTexture = loadTexture("src/resources/Textures/awesomeface.png");

    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    TextureShader.use();
    TextureShader.setInt("texture", 0);

    CubeMapShader.use();
    CubeMapShader.setInt("CubeMap", 0);

    // render loop
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    while (!glfwWindowShouldClose(window)) {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        // input
        processInputPlayer(window);

        // render
        // clear the colorbuffer
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);

        glm::vec3 lightColor = glm::vec3(0.0, 5.0, 0.8f);
        glm::vec3 lightPos = glm::vec3(-3.31996, 5.8f, 15.117);
         
       
        //Background-----------------
        TextureShader.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, wallTexture);

        model = glm::translate(model, glm::vec3(0, 0, resolution / 20));
        model = glm::scale(model, glm::vec3(resolution / 10,resolution/20, resolution / 10)); // a smaller cube

        settingShader(TextureShader, camera, pointLightPositions, lightPos, lightColor);
        TextureShader.setVec3("dirLight.ambient", 0.45f, 0.45f, 0.45f);
        TextureShader.setMat4("projection", projection);
        TextureShader.setMat4("view", view);
        TextureShader.setMat4("model", model);
        TextureShader.setVec2("texureScale", glm::vec2(-10.0,10));

        glBindVertexArray(VAOcube);
        glDrawArrays(GL_TRIANGLES, 0, 30);

        //Water-----------------

        waterShader.use();

        model = glm::mat4(1.0f);

        settingShader(waterShader, camera, pointLightPositions,lightPos,lightColor );
        
        waterShader.setMat4("model", model);
        waterShader.setMat4("projection", projection);
        waterShader.setMat4("view", view);
        
        glBindVertexArray(VAOplane);

        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, NULL);
     


        //Lights-----------------
        
        lightShader.use();
        for (unsigned int i = 0; i < 4; i++)
        {
            model = glm::mat4(1.0f); 
            model = glm::translate(model, pointLightPositions[i]);
            model = glm::scale(model, glm::vec3(0.5f)); // Make it a smaller cube

            lightShader.setMat4("projection", projection);
            lightShader.setMat4("view", view);
            lightShader.setVec3("lightColor", lightColor);
            lightShader.setMat4("model", model);

            glBindVertexArray(VAOcube);
            glDrawArrays(GL_TRIANGLES, 0, 36);

            glBindBuffer(GL_ARRAY_BUFFER, VBOPlane);	// and a different VBO
            glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);

            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), glm::value_ptr(vertices[0]), GL_STATIC_DRAW);

        
        
        }
        //Plattform, only the texture?

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, platformTexture); // Or whichever texture you want

        for (unsigned int i = 0; i < NUM_PLATFORMS; i++) {
            glm::vec3 quad_pos = platformPositions[i];
            glm::vec3 quad_scale = platformScales[i];

            makeQuad(TextureShader, VAOcube, projection, view, lightColor, quad_pos, quad_scale);

        }
        //skbox
        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
        CubeMapShader.use();
        view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
        CubeMapShader.setMat4("view", view);
        CubeMapShader.setMat4("projection", projection);
        // skybox cube
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS);


        // swap buffers and poll IO events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAOplane);
    glDeleteBuffers(1, &VBOPlane);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}
glm::vec3 acc_up = glm::vec3(0.0f, -10.0f, 0.0f);
glm::vec3 vel_up = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 deathPosition = glm::vec3(30.0f, 1.6f, 3.0f);
glm::vec3 zero = glm::vec3(0);

bool on_ground = true;

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInputFly(GLFWwindow* window)

{
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        glm::vec2 yp;
        glm::vec3 pos;
        pos = camera.getCameraData();
        yp = camera.getCameraDataYP();
        std::cout << pos.x << ", " << pos.y << ", " << pos.z<<"\n";
        std::cout << yp.x << ", " << yp.y << "\n ";

    }
    
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)

        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime, zero);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime, zero);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime, zero);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime, zero);
}

void processInputPlayer(GLFWwindow* window)

{
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        glm::vec2 yp;
        glm::vec3 pos;
        pos = camera.getCameraData();
        yp = camera.getCameraDataYP();
        std::cout << pos.x << ", " << pos.y << ", " << pos.z << "\n";
        std::cout << yp.x << ", " << yp.y << "\n ";

    }

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)

        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime, glm::vec3(0.0f, 0.0f, 0.0f));
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime, glm::vec3(0.0f, 0.0f, 0.0f));
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime, glm::vec3(0.0f, 0.0f, 0.0f));
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime, glm::vec3(0.0f, 0.0f, 0.0f));
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        // camera.ProcessKeyboard(SPACE, deltaTime, jump_timer);
        if (on_ground) {
            vel_up.y += 8.0f;
            on_ground = false;
        }
    }
    vel_up += acc_up * deltaTime;
    if (on_ground) {
        vel_up.y = 0.0f;
    }
    glm::vec3 pos;
    pos = camera.getCameraData();
    if (pos.y < 0) {
        deathPosition = pos;
        deathPosition.y = 1.6f;
        pointLightPositions[0] = deathPosition;
        camera.Position = start_pos;
        vel_up.y = 0;
        on_ground = true;
    }

    for (unsigned int i = 0; i < NUM_PLATFORMS; i++) {
        glm::vec3 quad_pos = platformPositions[i];
        glm::vec3 quad_scale = platformScales[i];

        if ((pos.x > quad_pos.x - quad_scale.x / 2) && (pos.x < quad_pos.x + quad_scale.x / 2)
            && (pos.z > quad_pos.z - quad_scale.z / 2) && (pos.z < quad_pos.z + quad_scale.z / 2)
            && (pos.y - 1 > quad_pos.y - quad_scale.y / 2) && (pos.y - 1 < quad_pos.y + quad_scale.y / 2)) {
            on_ground = true;
            vel_up.y = 0;
            camera.Position.y = quad_pos.y + 1 + quad_scale.y / 2;
            break;
        }
        else {
            on_ground = false;
        }

    }

    camera.ProcessKeyboard(SPACE, deltaTime, vel_up);


}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

glm::vec3 calculateFaceNormal(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2) {
    glm::vec3 edge1 = v1 - v0;
    glm::vec3 edge2 = v2 - v0;
    return glm::normalize(glm::cross(edge1, edge2));
}
void settingShader(Shader lightingShader, Camera camera, glm::vec3* pointLightPositions,glm::vec3 lightPos, glm::vec3 lightColor){
    float timer = glfwGetTime();

    lightingShader.setFloat("t", timer);
    lightingShader.setVec3("objectColor", .2f, 1.0f, 0.0f);
    lightingShader.setVec3("lightColor", lightColor);
    lightingShader.setVec3("lightPos", lightPos);
    lightingShader.setVec3("objPos[0].position", pointLightPositions[0]);
    lightingShader.setVec3("objPos[1].position", pointLightPositions[1]);
    lightingShader.setVec3("objPos[2].position", pointLightPositions[2]);
    lightingShader.setVec3("objPos[3].position", pointLightPositions[3]);
    lightingShader.setVec3("viewPos", camera.Position);

    lightingShader.setFloat("pointLights[0].constant", 1.0f);

    lightingShader.setVec3("material.ambient", 1.0f, 0.5f, 0.31f);
    lightingShader.setVec3("material.diffuse", 1.0f, 0.5f, 0.31f);
    lightingShader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
    lightingShader.setFloat("material.shininess", 32.0f);


    lightingShader.setVec3("dirLight.direction", glm::vec3(1.85711, 2.29324, 7.14807)-glm::vec3(0.224826, 0.828294, 1.63755));
    lightingShader.setVec3("dirLight.ambient", 0.85f, 0.85f, 0.85f);
    lightingShader.setVec3("dirLight.diffuse", 0.5f, 0.5f, 0.5f);
    lightingShader.setVec3("dirLight.specular", 0.8f, 0.8f, 0.8f);
    // point light 1
    lightingShader.setVec3("pointLights[0].position", pointLightPositions[0]);
    lightingShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
    lightingShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
    lightingShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
    lightingShader.setFloat("pointLights[0].constant", 1.0f);
    lightingShader.setFloat("pointLights[0].linear", 0.09f);
    lightingShader.setFloat("pointLights[0].quadratic", 0.032f);
    // point light 2
    lightingShader.setVec3("pointLights[1].position", pointLightPositions[1]);
    lightingShader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
    lightingShader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
    lightingShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
    lightingShader.setFloat("pointLights[1].constant", 1.0f);
    lightingShader.setFloat("pointLights[1].linear", 0.09f);
    lightingShader.setFloat("pointLights[1].quadratic", 0.032f);
    // point light 3
    lightingShader.setVec3("pointLights[2].position", pointLightPositions[2]);
    lightingShader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
    lightingShader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
    lightingShader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
    lightingShader.setFloat("pointLights[2].constant", 1.0f);
    lightingShader.setFloat("pointLights[2].linear", 0.09f);
    lightingShader.setFloat("pointLights[2].quadratic", 0.032f);
    // point light 4
    lightingShader.setVec3("pointLights[3].position", pointLightPositions[3]);
    lightingShader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
    lightingShader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
    lightingShader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
    lightingShader.setFloat("pointLights[3].constant", 1.0f);
    lightingShader.setFloat("pointLights[3].linear", 0.09f);
    lightingShader.setFloat("pointLights[3].quadratic", 0.032f);
    // spotLight
    lightingShader.setVec3("spotLight.position", camera.Position);
    lightingShader.setVec3("spotLight.direction", camera.Front);
    lightingShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
    lightingShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
    lightingShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
    lightingShader.setFloat("spotLight.constant", 1.0f);
    lightingShader.setFloat("spotLight.linear", 0.09f);
    lightingShader.setFloat("spotLight.quadratic", 0.032f);
    lightingShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
    lightingShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));


}
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
unsigned int loadTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

unsigned int loadCubemap(const char* faces[])
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < 6; i++)
    {
        unsigned char* data = stbi_load(faces[i], &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}
void makeQuad(Shader lightShader, unsigned int VAOcube, glm::mat4 projection, glm::mat4 view, glm::vec3 lightColor, glm::vec3 pos, glm::vec3 scale) {
    lightShader.use();
    lightShader.setInt("platformTexture", 0);
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, pos);
    model = glm::scale(model, scale); // Make it a smaller cube

    lightShader.setMat4("projection", projection);
    lightShader.setMat4("view", view);
    lightShader.setVec3("lightColor", lightColor);
    lightShader.setMat4("model", model);


    glBindVertexArray(VAOcube);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}