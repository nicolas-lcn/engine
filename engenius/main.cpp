// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> 
#include <iostream>

using namespace glm;

#include <common/shader.hpp>
#include <common/objloader.hpp>
#include <common/vboindexer.hpp>
#include <common/controls.hpp>

#include "Entity.h"

void processInput(GLFWwindow *window);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
glm::vec3 camera_position   = glm::vec3(0.0f, 0.0f,  3.0f);
glm::vec3 camera_target = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 camera_up    = glm::vec3(0.0f, 1.0f,  0.0f);
bool isInFreeMode = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

//rotation
float orbitAngle = 0.;
float orbitSpeed = 0.;
float zoom = 1.;

// Model matrix
glm::mat4 model(1.0f);
// View matrix : camera/view transformation lookat() utiliser camera_position camera_target camera_up
glm::mat4 view(1.0f);
// Projection matrix : 45 Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
glm::mat4 projection(1.0f);

int terrain_resolution = 4;
int previous_res = terrain_resolution;



/*******************************************************************************/

int main( void )
{
    // Initialise GLFW
    if( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        getchar();
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Open a window and create its OpenGL context
    window = glfwCreateWindow( 1024, 768, "Engenius", NULL, NULL);
    if( window == NULL ){
        fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
        getchar();
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        return -1;
    }

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    // glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, GL_TRUE);
    // Hide the mouse and enable unlimited mouvement
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Set the mouse at the center of the screen
    glfwPollEvents();
    glfwSetCursorPos(window, 1024/2, 768/2);

    // Dark blue background
    glClearColor(0.8f, 0.8f, 0.8f, 0.0f);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);

    // Cull triangles which normal is not towards the camera
    //glEnable(GL_CULL_FACE);

    // GLuint VertexArrayID;
    // glGenVertexArrays(1, &VertexArrayID);
    // glBindVertexArray(VertexArrayID);

    Entity sun("../data/models/planet.obj", 3);
    sun.addChild("../data/models/planet.obj", 3);
    sun.transform.setScale({2.f, 2.f, 2.f});
    Entity* earth = sun.children.back().get();
    earth->addChild("../data/models/planet.obj", 3);
    Entity* moon = earth->children.back().get();
    moon->transform.setScale({.5,0.5,0.5});
    moon->transform.setLocalPosition({1.0, 0.5, 0.5});
    earth->transform.setScale({1.5, 1.5, 1.5}); 
    earth->transform.setLocalPosition({6.0, 0.0, 0.0});
    earth->transform.setEulerRot({0.f, 0.f, 23.0f});
    sun.updateSelfAndChild();

    // Create and compile our GLSL program from the shaders
    GLuint programID = LoadShaders( "./shaders/vertex_solarsystem.glsl", "./shaders/fragment_solarsystem.glsl" );

    /*****************TODO***********************/
    // Get a handle for our "Model View Projection" matrices uniforms

    GLuint modelID = glGetUniformLocation(programID, "model");
    GLuint projectionID = glGetUniformLocation(programID, "projection");
    GLuint viewID = glGetUniformLocation(programID, "view");

    
    /****************************************/
    // std::vector<unsigned short> indices; //Triangles concaténés dans une liste
    // std::vector<std::vector<unsigned short> > triangles;
    // std::vector<glm::vec3> indexed_vertices;
    // std::vector<glm::vec2> uvs;
    // std::vector<glm::vec3> normals;

    // //Chargement du fichier de maillage
    // loadOBJ("../data/models/icosphere.obj", indexed_vertices, uvs, normals);
    // indexVBO(indexed_vertices, uvs, normals, indices, indexed_vertices, uvs, normals);
    // std::string filename("chair.off");
    // loadOFF(filename, indexed_vertices, indices, triangles );

    // Square s = Square(glm::vec3(-1.0,0.0,-1.0), glm::vec3(1.0,0.0,-1.0), glm::vec3(1.0,0.0,1.0), glm::vec3(-1.0,0.0,1.0), terrain_resolution);
    // //Square s = Square(glm::vec3(0.0,1.0,0.0), glm::vec3(1.0,1.0,0.0), glm::vec3(1.0,0.0,0.0), glm::vec3(0.0,0.0,0.0), 2);
    // for (int i = 0; i < s.indexes().size(); ++i)
    // {
    //     indices.push_back(s.indexes()[i]);
    // }
    // for (int i = 0; i < s.vertices().size(); ++i)
    // {
    //     indexed_vertices.push_back(s.vertices()[i]);
    //     uvs.push_back(s.texCoords()[i]);
    //     normals.push_back(s.normals()[i]);
    // }

    // camera_target = s.center();
    //initCameraParameters(camera_target);
    

    // Load it into a VBO

    // GLuint vertexbuffer;
    // glGenBuffers(1, &vertexbuffer);
    // glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    // glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(glm::vec3), &indexed_vertices[0], GL_STATIC_DRAW);

    // // Generate a buffer for the indices as well
    // GLuint elementbuffer;
    // glGenBuffers(1, &elementbuffer);
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0] , GL_STATIC_DRAW);

    // GLuint normalsBuffer;
    // glGenBuffers(1, &normalsBuffer);
    // glBindBuffer(GL_ARRAY_BUFFER, normalsBuffer);
    // glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);

    // GLuint uvsBuffer;
    // glGenBuffers(1, &uvsBuffer);
    // glBindBuffer(GL_ARRAY_BUFFER, uvsBuffer);
    // glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);

    // Get a handle for our "LightPosition" uniform
    glUseProgram(programID);
    GLuint LightID = glGetUniformLocation(programID, "LightPosition_worldspace");

    // GLuint m_ground = loadBMP_custom("../data/textures/grass.bmp");
    // GLuint m_h_ground = loadBMP_custom("../data/textures/rock.bmp");
    // GLuint m_heights = loadBMP_custom("../data/textures/snowrocks.bmp");

    // GLuint m_heightmap = loadBMP_custom("../data/textures/Heightmap_Mountain.bmp");

    // GLuint m_earth = loadBMP_custom("../data/textures/earth.bmp");
    sun.meshes[0].addTexture("../data/textures/sun.bmp", "tex");
    earth->meshes[0].addTexture("../data/textures/earth.bmp", "tex");
    moon->meshes[0].addTexture("../data/textures/moon.bmp", "tex");
    
    // glfwSetKeyCallback(window, key_callback);


    // For speed computation
    double lastTime = glfwGetTime();
    int nbFrames = 0;

    do{

        // Measure speed
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // if(previous_res != terrain_resolution)
        // {
        //     s.setResolution(terrain_resolution);
        //     indices.clear();
        //     indexed_vertices.clear();
        //     uvs.clear();
        //     normals.clear();
        //     for (int i = 0; i < s.indexes().size(); ++i)
        //     {
        //         indices.push_back(s.indexes()[i]);
        //     }
        //     for (int i = 0; i < s.vertices().size(); ++i)
        //     {
        //         indexed_vertices.push_back(s.vertices()[i]);
        //         uvs.push_back(s.texCoords()[i]);
        //         normals.push_back(s.normals()[i]);
        //     }
        //     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
        //     glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0] , GL_STATIC_DRAW);
        //     glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        //     glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(glm::vec3), &indexed_vertices[0], GL_STATIC_DRAW);
        //     glBindBuffer(GL_ARRAY_BUFFER, normalsBuffer);
        //     glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
        //     glBindBuffer(GL_ARRAY_BUFFER, uv0Buffer);
        //     glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);
        //     previous_res = terrain_resolution;
        // }
        



        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use our shader
        glUseProgram(programID);


        /*****************TODO***********************/
        // Model matrix : an identity matrix (model will be at the origin) then change

        // View matrix : camera/view transformation lookat() utiliser camera_position camera_target camera_up

        // Projection matrix : 45 Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units

        // Send our transformation to the currently bound shader,
        // in the "Model View Projection" to the shader uniforms
        //computeMatricesFromInputs();

        model = glm::mat4(1.0f);
        if(!isInFreeMode) model = glm::rotate(model, glm::radians(orbitAngle), glm::vec3(0.f, 1.f, 0.f));
        orbitAngle += orbitSpeed;

        view = getViewMatrix();

        projection = getProjectionMatrix();

        glUniformMatrix4fv(modelID, 1, false, glm::value_ptr(model));
        glUniformMatrix4fv(projectionID, 1, false, glm::value_ptr(projection));
        glUniformMatrix4fv(viewID, 1, false, glm::value_ptr(view));

        // glUniformMatrix4fv(MVP, 1, false, &mvp[0][0]);

        /****************************************/
        // earth.Draw(programID);
        Entity * entity = &sun;
        while(entity != nullptr)
        {
            glUniformMatrix4fv(modelID, 1, false, glm::value_ptr(entity->transform.getModelMatrix()));
            entity->Draw(programID);
            entity = entity->children.back().get();
        }
        earth->transform.setEulerRot({0.f, earth->transform.getEulerRot().y + (50.0f * deltaTime), 0.f});
        sun.transform.setEulerRot({0.f, sun.transform.getEulerRot().y + (20.0f * deltaTime), 0.f});
        sun.updateSelfAndChild();




        // // 1rst attribute buffer : vertices
        // glEnableVertexAttribArray(0);
        // glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        // glVertexAttribPointer(
        //             0,                  // attribute
        //             3,                  // size
        //             GL_FLOAT,           // type
        //             GL_FALSE,           // normalized?
        //             0,                  // stride
        //             (void*)0            // array buffer offset
        //             );

        // glBindBuffer(GL_ARRAY_BUFFER, normalsBuffer);
        // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);

        // glBindBuffer(GL_ARRAY_BUFFER, uvsBuffer);
        // glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);

        // // Index buffer
        // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);

        
        // // glBindTexture(GL_TEXTURE_2D, m_heightmap);
        // // glActiveTexture(GL_TEXTURE0);
        // // glUniform1i(glGetUniformLocation(programID, "heightmap"), 0);

        // // glActiveTexture(GL_TEXTURE1);
        // // glBindTexture(GL_TEXTURE_2D, m_ground);
        // // glUniform1i(glGetUniformLocation(programID, "ground"), 1);

        // // glActiveTexture(GL_TEXTURE2);
        // // glBindTexture(GL_TEXTURE_2D, m_h_ground);
        // // glUniform1i(glGetUniformLocation(programID, "h_ground"), 2);

        // // glActiveTexture(GL_TEXTURE3);
        // // glBindTexture(GL_TEXTURE_2D, m_heights);
        // // glUniform1i(glGetUniformLocation(programID, "heights"), 3);

        // glBindTexture(GL_TEXTURE_2D, m_earth);
        // glActiveTexture(GL_TEXTURE0);
        // glUniform1i(glGetUniformLocation(programID, "earth"), 0);

        // glEnableVertexAttribArray(2);

        
       

        // // Draw the triangles !
        // glDrawElements(
        //             GL_TRIANGLES,      // mode
        //             indices.size(),    // count
        //             GL_UNSIGNED_SHORT,   // type
        //             (void*)0           // element array buffer offset
        //             );

       

        // glDisableVertexAttribArray(0);


        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

    } // Check if the ESC key was pressed or the window was closed
    while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0 );

    // Cleanup VBO and shader
    
    glDeleteProgram(programID);
    // glDeleteBuffers(1, &vertexbuffer);
    // glDeleteBuffers(1, &elementbuffer);
    // glDeleteBuffers(1, &normalsBuffer);
    // glDeleteBuffers(1, &uvsBuffer);
    // glDeleteVertexArrays(1, &VertexArrayID);
    sun.meshes[0].deleteBuffers();
    earth->meshes[0].deleteBuffers();

    // Close OpenGL window and terminate GLFW
    glfwTerminate();

    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    computeMatricesFromInputs();
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if(key == GLFW_KEY_EQUAL && action == GLFW_PRESS)
    {
        terrain_resolution *= 2;
    }

    if(key == GLFW_KEY_6 && action == GLFW_PRESS)
    {
        if(terrain_resolution >= 4) terrain_resolution /= 2;
    }

    if(key == GLFW_KEY_C && action == GLFW_PRESS)
    {
        isInFreeMode = !isInFreeMode;
        glm::vec3 position = isInFreeMode ? glm::vec3( 0, 0, 5 ) : glm::vec3(0,5,5);
        float verticalAngle = isInFreeMode ? 0.2f : -M_PI/4;
        //setParameters(position, verticalAngle, 3.14f, isInFreeMode);
    }

    if(key == GLFW_KEY_UP && action == GLFW_PRESS)
    {
        orbitSpeed += 2.0f;
    }
    if(key == GLFW_KEY_DOWN && action == GLFW_PRESS)
    {
        orbitSpeed -= 2.0f;
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
