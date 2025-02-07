#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

float cubeVertices[] = {
    //  vertices             TexCoord      
        -0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,  
         0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  
         0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  
        -0.5f,  0.5f, -0.5f,  0.0f, 0.0f,  
        -0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  

        -0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  
         0.5f, -0.5f,  0.5f,  1.0f, 1.0f,  
         0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  
         0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,  
        -0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, -
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,  
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,  
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f  

};
class CubeMap
{
public:

    CubeMap(const char* facesCubeMapdir[6]) {

        unsigned int VAO, VBO;
        glGenBuffers(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
        // note that we update the lamp's position attribute's stride to reflect the updated buffer data
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // texture coord attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        unsigned int CubeTexture;

        glGenTextures(1, &CubeTexture);
        glBindTexture(GL_TEXTURE_CUBE_MAP, CubeTexture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
        // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	// set texture wrapping to GL_REPEAT (default wrapping method)
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // load image, create texture and generate mipmaps

        for (int i = 0; i < 6; i++) {
            int width, height, nrChannels;
            // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
            unsigned char* data = stbi_load(facesCubeMapdir[i], &width, &height, &nrChannels, 0);
            if (data)
            {
                glTexImage2D(GL_TEXTURE_CUBE_MAP, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
                glGenerateMipmap(GL_TEXTURE_2D);
            }
            else
            {
                std::cout << "Failed to load texture" << std::endl;
            }
            stbi_image_free(data);

        }

      }


	

};