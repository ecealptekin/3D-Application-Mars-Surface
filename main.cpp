#include <iostream>
#include <vector>

#define GLM_FORCE_LEFT_HANDED
#include "GLM/glm.hpp"
#include "GLM/gtc/constants.hpp"
#include "GLM/gtc/matrix_transform.hpp"
#include "GLM/gtx/rotate_vector.hpp"
#include "GLM/gtx/string_cast.hpp"
#include "GLM/gtc/type_ptr.hpp"
#include "GLAD/glad.h"
#include "GLFW/glfw3.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "opengl_utilities.hpp"
#include "extras.hpp"

#include "Transform.h"
#include "camera.h"

Movement_Status movement = Movement_Status::START;

/* Keep the global state inside this struct */
static struct {
    glm::dvec2 mouse_position;
    glm::ivec2 screen_dimensions = glm::ivec2(600, 600);
    
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
    
    glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f,  3.0f);
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);
    
    float lastX = 400;
    float lastY = 300;
    bool firstMouse = true;
    
} Globals;

Transform player_wheels1;
Transform player_wheels2;
Transform player_wheels3;
Transform player_wheels4;

Transform enemy_rover;
Transform enemy_wheels1;
Transform enemy_wheels2;
Transform enemy_wheels3;
Transform enemy_wheels4;

Transform enemy1_rover;
Transform enemy1_wheels1;
Transform enemy1_wheels2;
Transform enemy1_wheels3;
Transform enemy1_wheels4;

//position, rotation, scale
Transform player_rover(glm::vec3(0.0,0.0,+0.5),glm::mat4(1),glm::vec3(0.08));

//position, transform(child)
Camera camera(glm::vec3(0.1f,0.1f,+2.5f), player_rover);

bool game_stops = false;

/* GLFW Callback functions */
static void ErrorCallback(int error, const char* description)
{
    std::cerr << "Error: " << description << std::endl;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if(Globals.firstMouse)
    {
        Globals.lastX = xpos;
        Globals.lastY = ypos;
        Globals.firstMouse = false;
    }
    
    float xoffset = xpos - Globals.lastX;
    float yoffset = Globals.lastY - ypos;
    
    Globals.lastX = xpos;
    Globals.lastY = ypos;
    
    if(movement != Movement_Status::START)
        camera.ProcessMouseMovement(xoffset, yoffset);

}

static void WindowSizeCallback(GLFWwindow* window, int width, int height)
{
    Globals.screen_dimensions.x = width;
    Globals.screen_dimensions.y = height;

    glViewport(0, 0, width, height);
}

static void processInput(GLFWwindow* window)
{
    
    if(glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
    {
        movement = Movement_Status::CAMERA_ONLY;
    }
        
    else if(glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
    {
        movement = Movement_Status::PLAYER_ONLY;
    }
    
    else if(glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    {
           movement = Movement_Status::START;
    }
    
    if(movement == Movement_Status::CAMERA_ONLY)
    {
    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, movement, Globals.deltaTime);
     
     if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
         camera.ProcessKeyboard(BACKWARD, movement, Globals.deltaTime);
     
     if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
         camera.ProcessKeyboard(LEFT, movement, Globals.deltaTime);
     
     if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
         camera.ProcessKeyboard(RIGHT, movement, Globals.deltaTime);
    }
    
    if(game_stops == false)
    {
    
    if(movement == Movement_Status::PLAYER_ONLY)
    {
     if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
         camera.ProcessKeyboard(FORWARD, movement, Globals.deltaTime);
      
      if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
          camera.ProcessKeyboard(BACKWARD, movement, Globals.deltaTime);
      
      if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
          camera.ProcessKeyboard(LEFT, movement, Globals.deltaTime);
      
      if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
          camera.ProcessKeyboard(RIGHT, movement, Globals.deltaTime);
    }
        
   }
}


bool CheckCollision(glm::vec3 playerposition, glm::vec3 playerscale, glm::vec3 enemyposition, glm::vec3 enemyscale) // AABB - AABB collision
{
    // collision x-axis?
    bool collisionX = playerposition.x + playerscale.x >= enemyposition.x && enemyposition.x + enemyscale.x >= playerposition.x;
    // collision y-axis?
    bool collisionY = playerposition.y + playerscale.x >= enemyposition.y && enemyposition.y + enemyscale.x >= playerposition.y;
    // collision only if on both axes
 
    return collisionX && collisionY;
}


int main(void)
{
    /* Set GLFW error callback */
    glfwSetErrorCallback(ErrorCallback);

    /* Initialize the library */
    if (!glfwInit())
    {
        std::cout << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    /* Create a windowed mode window and its OpenGL context */
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    //glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
    GLFWwindow* window = glfwCreateWindow(
        Globals.screen_dimensions.x, Globals.screen_dimensions.y,
        "Ece Alptekin", NULL, NULL
    );
    if (!window)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    /* Load OpenGL extensions with GLAD */
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    glfwSwapInterval(2);

    /* Set GLFW Callbacks */
    glfwSetWindowSizeCallback(window, WindowSizeCallback);
    glfwSetCursorPosCallback(window, mouse_callback);

    /* Configure OpenGL */
    glClearColor(0, 0, 0, 0.1f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    /* Creating OpenGL objects */
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> uvs;
    std::vector<GLuint> indices;
    
    /* Creating OpenGL objects */
    std::vector<glm::vec3> positions1;
    std::vector<glm::vec3> normals1;
    std::vector<glm::vec2> uvs1;
    std::vector<GLuint> indices1;
    
    /* Creating OpenGL objects */
    std::vector<glm::vec3> positionsw;
    std::vector<glm::vec3> normalsw;
    std::vector<glm::vec2> uvsw;
    std::vector<GLuint> indicesw;


    GenerateParametricShapeFrom2D(positions, normals, uvs, indices, ParametricHalfCircle, 32, 16);
    GenerateParametricShapeFrom2D(positions1, normals1, uvs1, indices1, ParametricHalfCircle, 32, 16);
    GenerateParametricShapeFrom2D(positionsw, normalsw, uvsw, indicesw, ParametricCircle, 32, 16);
    VAO marsVAO(positions, normals, uvs, indices);
    VAO sphereVAO(positions1, normals1, uvs1, indices1);
    VAO wheelVAO(positionsw, normalsw, uvsw, indicesw);

    
    stbi_set_flip_vertically_on_load(true);

    auto filename = "/Users/macbookpro/Desktop/Project2/Project2/mars.JPEG";
    int x, y, n;
    unsigned char *texture_data = stbi_load(filename, &x, &y, &n, 0);
    if (texture_data == NULL)
    {
        std::cout << "Texture " << filename << " failed to load." << std::endl;
        std::cout << "Error: " << stbi_failure_reason() << std::endl;
    }
    else
    {
        std::cout << "Texture " << filename << " is loaded, X:" << x << " Y:" << y << " N:" << n << std::endl;
    }

        GLuint texture;
        glGenTextures(1, &texture);
    
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGBA,
            x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, texture_data
        );
    
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    
        stbi_image_free(texture_data);
    
   
       GLuint program = CreateProgramFromSources(
           R"VERTEX(
   #version 330 core
   
   layout(location = 0) in vec3 a_position;
   layout(location = 1) in vec3 a_normal;
   layout(location = 2) in vec2 a_uv;
   
   uniform mat4 u_transform;
   
   out vec3 vertex_position;
   out vec3 vertex_normal;
   out vec2 vertex_uv;
   
   void main()
   {
       gl_Position = u_transform * vec4(a_position, 1);
       vertex_normal = vec3(u_transform * vec4(a_normal, 0));
       vertex_position = vec3(gl_Position);
       vertex_uv = a_uv;
   }
           )VERTEX",
   
           R"FRAGMENT(
   #version 330 core
   
   uniform vec2 u_mouse_position;
   uniform sampler2D u_texture;
   
   in vec3 vertex_position;
   in vec3 vertex_normal;
   in vec2 vertex_uv;
   
   out vec4 out_color;
   
   void main()
   {
       vec3 color = vec3(0);
   
       vec3 surface_position = vertex_position;
       vec3 surface_normal = normalize(vertex_normal);
       vec2 surface_uv = vertex_uv;
       surface_uv.x *= 6;
       vec3 surface_color = texture(u_texture, surface_uv).rgb;
   
       vec3 ambient_color = vec3(0.7);
       color += ambient_color * surface_color;
   
       vec3 light_direction = normalize(vec3(-1, 1, 1));
       vec3 to_light = -light_direction;
   
       vec3 light_color = vec3(0.3);
   
       float diffuse_intensity = max(0, dot(to_light, surface_normal));
       color += diffuse_intensity * light_color * surface_color;
   
       vec3 view_dir = vec3(0, 0, -1);    //    Because we are using an orthograpic projection, and because of the direction of the projection
       vec3 halfway_dir = normalize(view_dir + to_light);
       float shininess = 4;
       float specular_intensity = max(0, dot(halfway_dir, surface_normal));
       color += pow(specular_intensity, shininess) * light_color;
   
   
       out_color = vec4(color, 1);
   }
           )FRAGMENT");
       if (program == NULL)
       {
           glfwTerminate();
           return -1;
       }
        
        GLuint program_object = CreateProgramFromSources(
            R"VERTEX(
    #version 330 core
    
    layout(location = 0) in vec3 a_position;
    layout(location = 1) in vec3 a_normal;
    layout(location = 2) in vec2 a_uv;
    
    uniform mat4 u_model;
    uniform mat4 u_projection_view;
    
    out vec4 world_space_position;
    out vec3 world_space_normal;
    out vec2 vertex_uv;
    
    void main()
    {
        world_space_position = u_model * vec4(a_position, 1);
        world_space_normal = vec3(u_model * vec4(a_normal, 0));
        vertex_uv = a_uv;
                                                         
        gl_Position = u_projection_view * world_space_position;
    }
            )VERTEX",
    
            R"FRAGMENT(
    #version 330 core
    
    uniform vec2 u_mouse_position;
    uniform vec3 u_color;
    
    in vec4 world_space_position;
    in vec3 world_space_normal;
    in vec2 vertex_uv;
    
    out vec4 out_color;
    
    void main()
    {
        vec3 color = vec3(0);
    
        vec3 surface_position = world_space_position.xyz;
        vec3 surface_normal = normalize(world_space_normal);
        vec2 surface_uv = vertex_uv;
        vec3 surface_color = u_color;
    
        vec3 ambient_color = vec3(0.7);
        color += ambient_color * surface_color;
    
        vec3 light_direction = normalize(vec3(-1, -1, 1));
        vec3 to_light = -light_direction;
    
        vec3 light_color = vec3(0.3);
    
        float diffuse_intensity = max(0, dot(to_light, surface_normal));
        color += diffuse_intensity * light_color * surface_color;
    
        vec3 view_dir = vec3(0, 0, -1);    //    Because we are using an orthograpic projection, and because of the direction of the projection
        vec3 halfway_dir = normalize(view_dir + to_light);
        float shininess = 4;
        float specular_intensity = max(0, dot(halfway_dir, surface_normal));
        color += pow(specular_intensity, shininess) * light_color;
    
    
        out_color = vec4((color), 1);
    }
            )FRAGMENT");
        if (program_object == NULL)
        {
            glfwTerminate();
            return -1;
        }
    
    
       auto texture_location = glGetUniformLocation(program, "u_texture");
       glUniform1i(texture_location, 0);
   
       glActiveTexture(GL_TEXTURE0); // activate the texture unit first before binding texture
       glBindTexture(GL_TEXTURE_2D, texture);
      
       auto mouse_location = glGetUniformLocation(program, "u_mouse_position");
       auto transform_location = glGetUniformLocation(program, "u_transform");
    
       auto model_location = glGetUniformLocation(program_object, "u_model");
       auto projection_view_location = glGetUniformLocation(program_object, "u_projection_view");
       auto color_location = glGetUniformLocation(program_object, "u_color");
    
    
       player_wheels1.Parent   = &player_rover;
       player_wheels1.Position = glm::vec3(1.0,0.3,-0.1);
       player_wheels1.Rotation = glm::mat4(1);
       player_wheels1.Scale    = glm::vec3(0.5);
    
       player_wheels2.Parent   = &player_rover;
       player_wheels2.Position = glm::vec3(-1.0,0.3,-0.1);
       player_wheels2.Rotation = glm::mat4(1);
       player_wheels2.Scale    = glm::vec3(0.5);
    
       player_wheels3.Parent   = &player_rover;
       player_wheels3.Position = glm::vec3(-1.0,-0.4,-0.1);
       player_wheels3.Rotation = glm::mat4(1);
       player_wheels3.Scale    = glm::vec3(0.5);

       player_wheels4.Parent   = &player_rover;
       player_wheels4.Position = glm::vec3(1.0,-0.3,-0.1);
       player_wheels4.Rotation = glm::mat4(1);
       player_wheels4.Scale    = glm::vec3(0.5);
    
    
    
         enemy_rover.Parent     = NULL;
         //enemy_rover.Position   = glm::vec3(4.0,2.0,-0.1);
         enemy_rover.Rotation   = glm::mat4(1);
         enemy_rover.Scale      = glm::vec3(0.5);
    
         enemy_wheels1.Parent   = &enemy_rover;
         enemy_wheels1.Position = glm::vec3(1.0,0.3,-0.1);
         enemy_wheels1.Rotation = glm::mat4(1);
         enemy_wheels1.Scale    = glm::vec3(0.5);
      
         enemy_wheels2.Parent   = &enemy_rover;
         enemy_wheels2.Position = glm::vec3(-1.0,0.3,-0.1);
         enemy_wheels2.Rotation = glm::mat4(1);
         enemy_wheels2.Scale    = glm::vec3(0.5);
      
         enemy_wheels3.Parent   = &enemy_rover;
         enemy_wheels3.Position = glm::vec3(-1.0,-0.4,-0.1);
         enemy_wheels3.Rotation = glm::mat4(1);
         enemy_wheels3.Scale    = glm::vec3(0.5);

         enemy_wheels4.Parent   = &enemy_rover;
         enemy_wheels4.Position = glm::vec3(1.0,-0.3,-0.1);
         enemy_wheels4.Rotation = glm::mat4(1);
         enemy_wheels4.Scale    = glm::vec3(0.5);
    
    
          enemy1_rover.Parent     = NULL;
          //enemy1_rover.Position   = glm::vec3(-4.0,-2.0,-0.1);
          enemy1_rover.Rotation   = glm::mat4(1);
          enemy1_rover.Scale      = glm::vec3(0.1);
     
          enemy1_wheels1.Parent   = &enemy1_rover;
          enemy1_wheels1.Position = glm::vec3(1.0,0.3,-0.1);
          enemy1_wheels1.Rotation = glm::mat4(1);
          enemy1_wheels1.Scale    = glm::vec3(0.3);
       
          enemy1_wheels2.Parent   = &enemy1_rover;
          enemy1_wheels2.Position = glm::vec3(-1.0,0.3,-0.1);
          enemy1_wheels2.Rotation = glm::mat4(1);
          enemy1_wheels2.Scale    = glm::vec3(0.3);
       
          enemy1_wheels3.Parent   = &enemy1_rover;
          enemy1_wheels3.Position = glm::vec3(-1.0,-0.4,-0.1);
          enemy1_wheels3.Rotation = glm::mat4(1);
          enemy1_wheels3.Scale    = glm::vec3(0.3);

          enemy1_wheels4.Parent   = &enemy1_rover;
          enemy1_wheels4.Position = glm::vec3(1.0,-0.3,-0.1);
          enemy1_wheels4.Rotation = glm::mat4(1);
          enemy1_wheels4.Scale    = glm::vec3(0.3);
    
    
    glm::vec3 chasing_pos  = glm::vec3(-2.0,-2.0,0);
    
       /* Loop until the user closes the window */
       while (!glfwWindowShouldClose(window))
       {
           /* Render here */
           glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   
           // Calculate mouse position
           auto mouse_position = Globals.mouse_position;
           mouse_position /= glm::dvec2(Globals.screen_dimensions);
           mouse_position.y = 1. - mouse_position.y;
           mouse_position = mouse_position * 2. - 1.;
           
           float currentFrame = glfwGetTime();
           Globals.deltaTime = currentFrame - Globals.lastFrame;
           Globals.lastFrame = currentFrame;
           
           processInput(window);
   
           glUniform2fv(mouse_location, 1, glm::value_ptr(glm::vec2(mouse_position)));
 
           glm::mat4 view = camera.GetViewMatrix();
           glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)Globals.screen_dimensions.x/(float)Globals.screen_dimensions.y, 0.1f, 100.f);
            
           glUniformMatrix4fv(projection_view_location, 1, GL_FALSE, glm::value_ptr(projection * view));
           
           // Draw texture
           glUseProgram(program);
           glBindVertexArray(marsVAO.id);
           glm::mat4 transform_mars(1.0);
           transform_mars = glm::scale(transform_mars, glm::vec3(0.8));
           transform_mars = glm::translate(transform_mars,glm::vec3(0.0,-0.5,0.0));
           glUniformMatrix4fv(transform_location, 1, GL_FALSE, glm::value_ptr(projection * view * transform_mars));
           glDrawElements(GL_TRIANGLES, marsVAO.element_array_count, GL_UNSIGNED_INT, NULL);
  
           //Draw object
           glUseProgram(program_object);
           glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
           
           //body
           auto transform = player_rover.GetModel();
           glBindVertexArray(sphereVAO.id);
           glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(projection * view * transform));
           glUniform3fv(color_location, 1, glm::value_ptr(glm::vec3(0,0,1)));
           glDrawElements(GL_TRIANGLES, sphereVAO.element_array_count, GL_UNSIGNED_INT, NULL);
      
           //wheels
           auto transform1 = player_wheels1.GetModel();
           glBindVertexArray(wheelVAO.id);
           glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(projection * view * transform1));
           glUniform3fv(color_location, 1, glm::value_ptr(glm::vec3(0,1,0)));
           glDrawElements(GL_TRIANGLES, wheelVAO.element_array_count, GL_UNSIGNED_INT, NULL);
    
           auto transform2 = player_wheels2.GetModel();
           glBindVertexArray(wheelVAO.id);
           glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(projection * view * transform2));
           glUniform3fv(color_location, 1, glm::value_ptr(glm::vec3(0,1,0)));
           glDrawElements(GL_TRIANGLES, wheelVAO.element_array_count, GL_UNSIGNED_INT, NULL);
     
           auto transform3 = player_wheels3.GetModel();
           glBindVertexArray(wheelVAO.id);
           glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(projection * view * transform3));
           glUniform3fv(color_location, 1, glm::value_ptr(glm::vec3(0,1,0)));
           glDrawElements(GL_TRIANGLES, wheelVAO.element_array_count, GL_UNSIGNED_INT, NULL);
 
           auto transform4 = player_wheels4.GetModel();
           glBindVertexArray(wheelVAO.id);
           glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(projection * view * transform4));
           glUniform3fv(color_location, 1, glm::value_ptr(glm::vec3(0,1,0)));
           glDrawElements(GL_TRIANGLES, wheelVAO.element_array_count, GL_UNSIGNED_INT, NULL);
           
           
           //enemy1
           glm::mat4 chasing_transform(1.0);
           glm::vec3 pos = player_rover.Position;
           chasing_pos = glm::mix(pos, chasing_pos, 0.99);
           auto chasing_translate = glm::translate(chasing_pos);
           auto scale = glm::scale(glm::vec3(0.2));
           chasing_transform = chasing_translate * scale;
           enemy_rover.Position = chasing_pos;

           //body
           auto enemytransform = enemy_rover.GetModel();
           glBindVertexArray(sphereVAO.id);
           glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(projection * view * chasing_transform * enemytransform));
           glUniform3fv(color_location, 1, glm::value_ptr(glm::vec3(1,0,0)));
           glDrawElements(GL_TRIANGLES, sphereVAO.element_array_count, GL_UNSIGNED_INT, NULL);
                
           //wheels
           auto enemytransform1 = enemy_wheels1.GetModel();
           glBindVertexArray(wheelVAO.id);
           glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(projection * view * chasing_transform * enemytransform1));
           glUniform3fv(color_location, 1, glm::value_ptr(glm::vec3(0,1,1)));
           glDrawElements(GL_TRIANGLES, wheelVAO.element_array_count, GL_UNSIGNED_INT, NULL);
              
           auto enemytransform2 = enemy_wheels2.GetModel();
           glBindVertexArray(wheelVAO.id);
           glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(projection * view * chasing_transform * enemytransform2));
           glUniform3fv(color_location, 1, glm::value_ptr(glm::vec3(0,1,1)));
           glDrawElements(GL_TRIANGLES, wheelVAO.element_array_count, GL_UNSIGNED_INT, NULL);
               
           auto enemytransform3 = enemy_wheels3.GetModel();
           glBindVertexArray(wheelVAO.id);
           glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(projection * view * chasing_transform * enemytransform3));
           glUniform3fv(color_location, 1, glm::value_ptr(glm::vec3(0,1,1)));
           glDrawElements(GL_TRIANGLES, wheelVAO.element_array_count, GL_UNSIGNED_INT, NULL);
           
           auto enemytransform4 = enemy_wheels4.GetModel();
           glBindVertexArray(wheelVAO.id);
           glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(projection * view * chasing_transform * enemytransform4));
           glUniform3fv(color_location, 1, glm::value_ptr(glm::vec3(0,1,1)));
           glDrawElements(GL_TRIANGLES, wheelVAO.element_array_count, GL_UNSIGNED_INT, NULL);
           
           
           //enemy2
           enemy1_rover.Position = chasing_pos;

           //body
           auto enemytransform5 = enemy1_rover.GetModel();
           glBindVertexArray(sphereVAO.id);
           glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(projection * view * enemytransform5));
           glUniform3fv(color_location, 1, glm::value_ptr(glm::vec3(1,1,0)));
           glDrawElements(GL_TRIANGLES, sphereVAO.element_array_count, GL_UNSIGNED_INT, NULL);
                
           //wheels
           auto enemytransform6 = enemy1_wheels1.GetModel();
           glBindVertexArray(wheelVAO.id);
           glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(projection * view * enemytransform6));
           glUniform3fv(color_location, 1, glm::value_ptr(glm::vec3(1,0,1)));
           glDrawElements(GL_TRIANGLES, wheelVAO.element_array_count, GL_UNSIGNED_INT, NULL);
              
           auto enemytransform7 = enemy1_wheels2.GetModel();
           glBindVertexArray(wheelVAO.id);
           glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(projection * view * enemytransform7));
           glUniform3fv(color_location, 1, glm::value_ptr(glm::vec3(1,0,1)));
           glDrawElements(GL_TRIANGLES, wheelVAO.element_array_count, GL_UNSIGNED_INT, NULL);
               
           auto enemytransform8 = enemy1_wheels3.GetModel();
           glBindVertexArray(wheelVAO.id);
           glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(projection * view * enemytransform8));
           glUniform3fv(color_location, 1, glm::value_ptr(glm::vec3(1,0,1)));
           glDrawElements(GL_TRIANGLES, wheelVAO.element_array_count, GL_UNSIGNED_INT, NULL);
           
           auto enemytransform9 = enemy1_wheels4.GetModel();
           glBindVertexArray(wheelVAO.id);
           glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(projection * view * enemytransform9));
           glUniform3fv(color_location, 1, glm::value_ptr(glm::vec3(1,0,1)));
           glDrawElements(GL_TRIANGLES, wheelVAO.element_array_count, GL_UNSIGNED_INT, NULL);

           glm::vec3 playerpos = player_rover.Position;
           glm::vec3 playersc  = glm::vec3(0.1);
           glm::vec3 enemypos  = enemy_rover.Position;
           glm::vec3 enemysc   = glm::vec3(0.1);
           glm::vec3 enemy2pos = enemy1_rover.Position;
           glm::vec3 enemy2sc  = glm::vec3(0.1);
           
           if(CheckCollision(playerpos, playersc, enemypos, enemysc))
           {
               game_stops = true;
           }
           
           else if(CheckCollision(playerpos, playersc, enemy2pos, enemy2sc))
           {
               game_stops = true;
           }
      
           /* Swap front and back buffers */
           glfwSwapBuffers(window);
   
           /* Poll for and process events */
           glfwPollEvents();
           
       }
   
       glfwTerminate();
       return 0;
   }
