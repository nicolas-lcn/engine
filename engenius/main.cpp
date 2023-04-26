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
#include <glm/gtx/rotate_vector.hpp>
#include <iostream>

using namespace glm;

#include <common/shader.hpp>
#include <common/objloader.hpp>
#include <common/vboindexer.hpp>
#include <common/controls.hpp>
#include <common/stb_image.h>

#include "Entity.h"

void processInput(GLFWwindow *window);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
bool computeCollisions(Entity &a, Entity &terrain, const char* pathHeightMap, float offset, float &moveY);
bool computeCollisions(Entity &e);

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

//character
float characterSpeed = 5.0f;
glm::vec3 move;

bool isJumping;
bool isMoving;
float rebound = 0.8f;




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


///////////////////   SOLAR SYSTEM ////////////////////////////////// 
    // Entity sun("../data/models/planet.obj", 3);
    // sun.addChild("../data/models/planet.obj", 3);
    // sun.transform.setScale({2.f, 2.f, 2.f});
    // Entity* earth = sun.children.back().get();
    // earth->addChild("../data/models/planet.obj", 3);
    // Entity* moon = earth->children.back().get();
    // moon->transform.setScale({.5,0.5,0.5});
    // moon->transform.setLocalPosition({1.0, 0.5, 0.5});
    // earth->transform.setScale({1.5, 1.5, 1.5}); 
    // earth->transform.setLocalPosition({6.0, 0.0, 0.0});
    // earth->transform.setEulerRot({0.f, 0.f, 23.0f});
    // sun.updateSelfAndChild();
    // sun.meshes[0].addTexture("../data/textures/sun.bmp", "tex");
    // earth->meshes[0].addTexture("../data/textures/earth.bmp", "tex");
    // moon->meshes[0].addTexture("../data/textures/moon.bmp", "tex");

/////////////////////////////////////////////////////////////////////

    ////////////////// TERRAIN ///////////////////

    Entity terrain("dummy terrain", 2);
    terrain.transform.setScale(glm::vec3(3.0f));
    terrain.updateSelfAndChild();
    terrain.meshes[0]->addTexture("../data/textures/grass.bmp", "ground");
    // terrain.meshes[0]->addTexture("../data/textures/rock.bmp", "h_ground");
    // terrain.meshes[0]->addTexture("../data/textures/snowrocks.bmp", "heights");
    // terrain.meshes[0]->addTexture("../data/textures/Heightmap_Mountain.bmp", "heightmap");

    // // Entity character = Entity("dummy sphere", 3);
    // // character.transform.setLocalPosition(glm::vec3(1.0f));
    // // character.transform.setScale(glm::vec3(3.0f));
    // // character.updateSelfAndChild();
    // // character.meshes[0]->addTexture("../data/textures/leopard.bmp", "objectTex");

    // ////////// LOD BUNNY //////////////
    // LODEntity character("../data/models/icosphere.off", 1);
    // character.addMesh("../data/models/icosphere1.off", 1);
    // character.addMesh("../data/models/icosphere2.off", 1);
    

    // // character.transform.setScale(glm::vec3(2.0f));
    // character.updateSelfAndChild();


    ////////// CUBE ///////////
    Entity cube("../data/models/cube.off", 1);
    RigidBody rb = RigidBody(1.0);
    cube.setRigidBody(rb);
    cube.transform.setLocalPosition(glm::vec3( 0, 3.0, 9.0 ));
    glm::vec3 init_v = glm::vec3(0.0, 0.04, -0.04);
    cube.getRigidBody()->setSpeed(init_v);
    cube.updateSelfAndChild();

//////////////////////////////////////////////////////////////////////

    // Create and compile our GLSL program from the shaders
    // GLuint programID = LoadShaders( "./shaders/vertex_solarsystem.glsl", "./shaders/fragment_solarsystem.glsl" );
    GLuint programID = LoadShaders( "./shaders/vertex_terrain.glsl", "./shaders/fragment_terrain.glsl" );

    /*****************TODO***********************/
    // Get a handle for our "Model View Projection" matrices uniforms

    GLuint modelID = glGetUniformLocation(programID, "model");
    GLuint projectionID = glGetUniformLocation(programID, "projection");
    GLuint viewID = glGetUniformLocation(programID, "view");

    // Get a handle for our "LightPosition" uniform
    glUseProgram(programID);
    GLuint LightID = glGetUniformLocation(programID, "LightPosition_worldspace");

    // GLuint m_heightmap = loadBMP_custom("../data/textures/Heightmap_Mountain.bmp");

    // GLuint m_earth = loadBMP_custom("../data/textures/earth.bmp");
    
    
    glfwSetKeyCallback(window, key_callback);


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
        
        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use our shader
        glUseProgram(programID);

        // Model matrix : an identity matrix (model will be at the origin) then change

        model = glm::mat4(1.0f);

        // View matrix : camera/view transformation lookat() utiliser camera_position camera_target camera_up

        view = getViewMatrix();

        // Projection matrix : 45 Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units

        projection = getProjectionMatrix();

        // Send our transformation to the currently bound shader,
        // in the "Model View Projection" to the shader uniforms

        glUniformMatrix4fv(modelID, 1, false, glm::value_ptr(model));
        glUniformMatrix4fv(projectionID, 1, false, glm::value_ptr(projection));
        glUniformMatrix4fv(viewID, 1, false, glm::value_ptr(view));


        /************ LOD **********/
        // glm::vec3 camPos = getCamPosition();
        // if(glm::length(character.transform.getLocalPosition() - camPos) < 10) character.setCurrentLevel(0);
        // else if(glm::length(character.transform.getLocalPosition() - camPos) < 15) character.setCurrentLevel(1);
        // // else if(glm::length(character.transform.getLocalPosition() - camPos) < 6) character.setCurrentLevel(2);
        // // else if(glm::length(character.transform.getLocalPosition() - camPos) < 8) character.setCurrentLevel(3);
        // // else if(glm::length(character.transform.getLocalPosition() - camPos) < 10) character.setCurrentLevel(4);
        // // else character.setCurrentLevel(5);
        // else character.setCurrentLevel(2);


        /****************************************/
        // Entity * entity = &sun;
        glUniform1i(glGetUniformLocation(programID, "isVertTerrain"), true);
        glUniform1i(glGetUniformLocation(programID, "isFragTerrain"), true);
        Entity * entity = &terrain;
        while(entity != nullptr)
        {
            glUniformMatrix4fv(modelID, 1, false, glm::value_ptr(entity->transform.getModelMatrix()));
            entity->Draw(programID);
            entity = entity->children.back().get();
        }
        if( !isInFreeMode) 
        {
            terrain.transform.setEulerRot({terrain.transform.getEulerRot().x, terrain.transform.getEulerRot().y + orbitAngle, terrain.transform.getEulerRot().z});
            terrain.updateSelfAndChild();      
        }
        glUniform1i(glGetUniformLocation(programID, "isVertTerrain"), false);
        glUniform1i(glGetUniformLocation(programID, "isFragTerrain"), false);
        cube.update(deltaTime);
        glm::vec3 noSpeed(0.0, 0.0, 0.0);
        if(isJumping)
        {
            cube.getRigidBody()->applyForce(glm::vec3(0.0, 7.0, -3.0));
            isJumping = false;
        }
        if(isMoving)
        {
            cube.getRigidBody()->applyForce(move);
            isMoving = false;
        }
        if(computeCollisions(cube))
        {
            // cube.getRigidBody()->applyForce(glm::vec3(0.0, 9.81, 0.0));
            // cube.getRigidBody()->setSpeed(noSpeed);
            glm::vec3 reboundVec = cube.getRigidBody()->computeRebound(glm::vec3(0.0, 1.0, 0.0));
            reboundVec = rebound * reboundVec;
            cube.getRigidBody()->setSpeed(reboundVec); 
            if(glm::length(cube.getRigidBody()->getSpeed()) <= 0.01f)
            {
                cube.getRigidBody()->applyForce(glm::vec3(0.0, 0.5, 0.0));
                cube.getRigidBody()->setSpeed(noSpeed);

            }
            
        }
        glUniformMatrix4fv(modelID, 1, false, glm::value_ptr(cube.transform.getModelMatrix()));
        cube.Draw(programID);
        // float moveY = 0.0f;
        // bool collides = computeCollisions(character, terrain, "../data/textures/Heightmap_Mountain.bmp", 3.5, moveY);
        // move += glm::vec3(0.0f, moveY, 0.0f);
        // character.transform.setLocalPosition(character.transform.getLocalPosition() + (characterSpeed * deltaTime * move));
        // character.updateSelfAndChild();
        // move = glm::vec3(0.0f);

        // earth->transform.setEulerRot({0.f, earth->transform.getEulerRot().y + (50.0f * deltaTime), 0.f});
        // sun.transform.setEulerRot({0.f, sun.transform.getEulerRot().y + (20.0f * deltaTime), 0.f});
        // sun.updateSelfAndChild();

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

    } // Check if the ESC key was pressed or the window was closed
    while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0 );

    // Cleanup VBO and shader
    
    glDeleteProgram(programID);
    
    // sun.meshes[0].deleteBuffers();
    // earth->meshes[0].deleteBuffers();
    // moon->meshes[0].deleteBuffers();
    terrain.meshes[0]->deleteBuffers();
    cube.meshes[0]->deleteBuffers();

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

    if(glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
    {
        move = glm::vec3(0, 0, 0.2);
        isMoving = true;
    }
    if(glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
    {
        move = glm::vec3(0, 0, -0.2);
        isMoving = true;

    }
    if(glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
    { 
        move = glm::vec3(-0.2, 0, 0);
        isMoving = true;

    }
    if(glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) 
    {
        move = glm::vec3(0.2, 0, 0);
        isMoving = true;

    }

    computeMatricesFromInputs();
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    // if(key == GLFW_KEY_EQUAL && action == GLFW_PRESS)
    // {
    //     terrain_resolution *= 2;
    // }

    // if(key == GLFW_KEY_6 && action == GLFW_PRESS)
    // {
    //     if(terrain_resolution >= 4) terrain_resolution /= 2;
    // }

    if(key == GLFW_KEY_C && action == GLFW_PRESS)
    {
        isInFreeMode = !isInFreeMode;
        glm::vec3 position = isInFreeMode ? glm::vec3( 0, 2.0, 10 ) : glm::vec3(0.0,20.0,20.0);
        float verticalAngle = isInFreeMode ? 0.2f : -M_PI/4;
        setParameters(position, verticalAngle, 3.14f, isInFreeMode);
    }

    if(key == GLFW_KEY_UP && action == GLFW_PRESS)
    {
        orbitAngle += 2.0f;

    }
    if(key == GLFW_KEY_DOWN && action == GLFW_PRESS)
    {
        orbitAngle -= 2.0f;

    }

    if(key == GLFW_KEY_SPACE && action == GLFW_PRESS)
    {
        isJumping = true;
    }
}

// Evaluates collisions between an entity and the terrain
bool computeCollisions(Entity &e, Entity &terrain, const char* pathHeightMap, float offset, float &moveY)
{
    glm::vec3 centerA = e.transform.getLocalPosition();
    int height, width, nbChannels;
    unsigned char *data = stbi_load(pathHeightMap, &width, &height, &nbChannels, 0);
    // Cartesian equation terrain
    Square* s = dynamic_cast<Square*>(terrain.meshes[0]);
    glm::vec3 normal = s->normal();
    glm::vec3 point = s->bottomLeft();
    float a = normal.x; float b = normal.y; float c = normal.z;
    float d = -(a * point.x) - (b * point.y) - (c * point.z);
    // Projection;
    float lambda = (a * centerA.x + b * centerA.y + c * centerA.z + d)/(a*a + b*b + c*c);
    glm::vec3 projection = glm::vec3(centerA.x - lambda*a, centerA.y - lambda*b, centerA.z - lambda*c);
    float u,v;
    glm::vec3 X = s->bottomRight() - s->bottomLeft();
    glm::vec3 Y = s->upLeft() - s->bottomLeft();
    glm::vec3 p_bottom = projection - s->bottomLeft();
    glm::vec3 projP_X = (glm::dot(p_bottom, X)/glm::length(X) * glm::length(X))*X;
    glm::vec3 projP_Y = (glm::dot(p_bottom, Y)/glm::length(Y) * glm::length(X))*Y;
    u = glm::length(projP_X)/glm::length(X);
    v = glm::length(projP_Y)/glm::length(Y);
    unsigned char *texel = data + (int)(v + width * u) * nbChannels;
    unsigned char heightTexel = texel[0];
    moveY = ((float)(heightTexel)/255.0f + offset) - centerA.y;
    return (centerA.y >= 0 && centerA.y < ((float)(heightTexel)/255.0f + offset)); 
}

bool computeCollisions(Entity &e)
{
    return (e.transform.getLocalPosition() - glm::vec3(0.1, 0.1, 0.1))[1] <= 0;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
