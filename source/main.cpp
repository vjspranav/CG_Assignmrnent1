#include "main.h"
#include "timer.h"
#include <unistd.h>
#include "ball.h"
#include "player.h"
#include "maze.h"

using namespace std;

GLMatrices Matrices;
GLuint     programID;
GLFWwindow *window;

/**************************
* Customizable functions *
**************************/

Player player;
vector<pair<int, int>> walls;

float screen_zoom = 0.2, screen_center_x = 0, screen_center_y = 0;
float camera_rotation_angle = 0;
int dark=0;
int x_grid=0, y_grid=0;
int score=0;
float health = 20;
Timer t60(1.0 / 60);

void print_values(){
    system("clear");
    cout << "VENDOR: " << glGetString(GL_VENDOR) << endl;
    cout << "RENDERER: " << glGetString(GL_RENDERER) << endl;
    cout << "VERSION: " << glGetString(GL_VERSION) << endl;
    cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
    cout << "Score: " << score << "\tHealth: " << health << endl;
}

bool has_wall(int x, int y){
    for(auto wall : walls){ 
        if(wall.first == x && wall.second == y){ 
            return 1;
        }
    }
    return 0;
}

bool checkULDR(Player player, int x, int y){
    int x1 = player.position.x;
    int y1 = player.position.y;
    if(x1==x){
        if(abs(y-y1)==1){
            return 1;
        }
    }
    if(y1==y){
        if(abs(x-x1)==1){
            return 1;
        }
    }
    if(abs(x-x1)==1 && abs(y-y1)==1){
            return 1;
    }
    return 0;
}

bool check_player_objects(int x, int y){
    if(player.position.x == x && player.position.y == y)
        return 0;
    return 1;
}

/* Render the scene with openGL */
/* Edit this function according to your assignment */
void draw() {
    // clear the color and depth in the frame buffer
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // use the loaded shader program
    // Don't change unless you know what you are doing
    glUseProgram (programID);

    // Eye - Location of camera. Don't change unless you are sure!!
    glm::vec3 eye ( 5*cos(camera_rotation_angle*M_PI/180.0f), 0, 5*sin(camera_rotation_angle*M_PI/180.0f) );
    // Target - Where is the camera looking at.  Don't change unless you are sure!!
    glm::vec3 target (0, 0, 0);
    // Up - Up vector defines tilt of camera.  Don't change unless you are sure!!
    glm::vec3 up (0, 1, 0);

    // Compute Camera matrix (view)
    Matrices.view = glm::lookAt( eye, target, up ); // Rotating Camera for 3D
    // Don't change unless you are sure!!
    // Matrices.view = glm::lookAt(glm::vec3(0, 0, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)); // Fixed camera for 2D (ortho) in XY plane

    // Compute ViewProject matrix as view/camera might not be changed for this frame (basic scenario)
    // Don't change unless you are sure!!
    glm::mat4 VP = Matrices.projection * Matrices.view;

    // Send our transformation to the currently bound shader, in the "MVP" uniform
    // For each model you render, since the MVP will be different (at least the M part)
    // Don't change unless you are sure!!
    glm::mat4 MVP;  // MVP = Projection * View * Model

    // Scene render
    player.draw(VP);
    for(auto wall: walls){
        if(check_player_objects(wall.first, wall.second)){
            if(dark && !checkULDR(player, wall.first, wall.second)){
                Ball(wall.first, wall.second, COLOR_BLACK).draw(VP);
            }else{
                Ball(wall.first, wall.second, COLOR_GREEN).draw(VP);
            }
        }
    }

    for(int i=0;i<x_grid;i++){
        for(int j=0;j<y_grid;j++){
            if(!has_wall(i, j))
                if (dark && !checkULDR(player, i, j)){
                    Ball(i, j, COLOR_BLACK).draw(VP);
                }else{
                    Ball(i, j, COLOR_RED).draw(VP);
                }
        }
    }
    
}

void tick_input(GLFWwindow *window) {
    static double lastTime = glfwGetTime();
    double currentTime = glfwGetTime();
    float deltaTime = float(currentTime - lastTime);
    sleep(deltaTime);
    int w  = glfwGetKey(window, GLFW_KEY_W);
    int s  = glfwGetKey(window, GLFW_KEY_S);
    int a = glfwGetKey(window, GLFW_KEY_A);
    int d = glfwGetKey(window, GLFW_KEY_D);
    int l = glfwGetKey(window, GLFW_KEY_L);
    int new_x = player.position.x;
    int new_y = player.position.y;
    if (a) {
        new_x += 1;
    }
    if (d) {
        new_x -= 1;
    }
    if (w) {
        new_y += 1;
    }
    if (s) {
        new_y -= 1;
    }
    if (l) {
        dark = !dark;
    }
    if (has_wall(new_x, new_y) && (player.position.x != new_x || player.position.y != new_y)){
            player.set_position(new_x, new_y);
            score += dark ? 2 : 1;
    }
    health -= 0.01;
    lastTime = currentTime;
}

void tick_elements() {
    //camera_rotation_angle += 1;
}

/* Initialize the OpenGL rendering properties */
/* Add all the models to be created here */
void initGL(GLFWwindow *window, int width, int height) {
    /* Objects should be created before any other gl function and shaders */
    // Create the models

    player       = Player(0, 0, COLOR_BLUE);
    // Create and compile our GLSL program from the shaders
    programID = LoadShaders("../source/shaders/shader.vert", "../source/shaders/shader.frag");
    // Get a handle for our "MVP" uniform
    Matrices.MatrixID = glGetUniformLocation(programID, "MVP");


    reshapeWindow (window, width, height);

    // Background color of the scene
    glClearColor (COLOR_BACKGROUND.r / 256.0, COLOR_BACKGROUND.g / 256.0, COLOR_BACKGROUND.b / 256.0, 0.0f); // R, G, B, A
    glClearDepth (1.0f);

    glEnable (GL_DEPTH_TEST);
    glDepthFunc (GL_LEQUAL);

}


int main(int argc, char **argv) {
    srand(time(0));
    int width  = 800;
    int height = 800;
    camera_rotation_angle -= 90;
    window = initGLFW(width, height);

    pair<int, int> gridsize = create_walls(walls);
    x_grid = gridsize.first;
    y_grid = gridsize.second;

    initGL (window, width, height);

    /* Draw in loop */
    while (!glfwWindowShouldClose(window) && health > 0) {
        // Process timers
        if (t60.processTick()) {
            // 60 fps
            // OpenGL Draw commands
            draw(); 
            // Swap Frame Buffer in double buffering
            glfwSwapBuffers(window);

            tick_elements();
            tick_input(window);
            print_values();
        }

        // Poll for Keyboard and mouse events
        glfwPollEvents();
    }

    quit(window);
}

bool detect_collision(bounding_box_t a, bounding_box_t b) {
    return (abs(a.x - b.x) * 2 < (a.width + b.width)) &&
           (abs(a.y - b.y) * 2 < (a.height + b.height));
}

void reset_screen() {
    float top    = screen_center_y + 4 / screen_zoom;
    float bottom = screen_center_y - 4 / screen_zoom;
    float left   = screen_center_x - 4 / screen_zoom;
    float right  = screen_center_x + 4 / screen_zoom;
    Matrices.projection = glm::ortho(left, right, bottom, top, 0.1f, 500.0f);
}
