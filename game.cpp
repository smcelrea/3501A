#include <iostream>
#include <time.h>
#include <sstream>

#include "game.h"
#include "bin/path_config.h"

namespace game {

// Some configuration constants
// They are written here as global variables, but ideally they should be loaded from a configuration file

// Main window settings
const std::string window_title_g = "Demo";
const unsigned int window_width_g = 800;
const unsigned int window_height_g = 600;
const bool window_full_screen_g = false;

// Viewport and camera settings
float camera_near_clip_distance_g = 0.01;
float camera_far_clip_distance_g = 1000.0;
float camera_fov_g = 25.0; // Field-of-view of camera
const glm::vec3 viewport_background_color_g(0.3, 0.1, 0.1);
glm::vec3 camera_position_g(0.5, 0.5, 10.0);
glm::vec3 camera_look_at_g(0.0, 0.0, 0.0);
glm::vec3 camera_up_g(0.0, 1.0, 0.0);

// Materials 
const std::string material_directory_g = MATERIAL_DIRECTORY;


Game::Game(void){

    // Don't do work in the constructor, leave it for the Init() function
}


void Game::Init(void){

    // Run all initialization steps
    InitWindow();
    InitView();
    InitEventHandlers();

    // Set variables
    animating_ = true;
	std::string keymap[] = { "w", "a", "s", "d", " ", "lshift", "lctrl" };
	for (int i = 0; i < sizeof(keymap) / sizeof(*keymap); i++) {
		keys.insert(std::pair<std::string, bool> (keymap[i], false));
	}
}

       
void Game::InitWindow(void){

    // Initialize the window management library (GLFW)
    if (!glfwInit()){
        throw(GameException(std::string("Could not initialize the GLFW library")));
    }

    // Create a window and its OpenGL context
    if (window_full_screen_g){
        window_ = glfwCreateWindow(window_width_g, window_height_g, window_title_g.c_str(), glfwGetPrimaryMonitor(), NULL);
    } else {
        window_ = glfwCreateWindow(window_width_g, window_height_g, window_title_g.c_str(), NULL, NULL);
    }
    if (!window_){
        glfwTerminate();
        throw(GameException(std::string("Could not create window")));
    }

    // Make the window's context the current one
    glfwMakeContextCurrent(window_);

    // Initialize the GLEW library to access OpenGL extensions
    // Need to do it after initializing an OpenGL context
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK){
        throw(GameException(std::string("Could not initialize the GLEW library: ")+std::string((const char *) glewGetErrorString(err))));
    }
}


void Game::InitView(void){

    // Set up z-buffer
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Set viewport
    int width, height;
    glfwGetFramebufferSize(window_, &width, &height);
    glViewport(0, 0, width, height);

    // Set up camera
    // Set current view
    camera_.SetView(camera_position_g, camera_look_at_g, camera_up_g);
    // Set projection
    camera_.SetProjection(camera_fov_g, camera_near_clip_distance_g, camera_far_clip_distance_g, width, height);
}


void Game::InitEventHandlers(void){

    // Set event callbacks
    glfwSetKeyCallback(window_, KeyCallback);
    glfwSetFramebufferSizeCallback(window_, ResizeCallback);

    // Set pointer to game object, so that callbacks can access it
    glfwSetWindowUserPointer(window_, (void *) this);
}


void Game::SetupResources(void){

    // Create a torus
    resman_.CreateTorus("TorusMesh");
	resman_.CreateCylinder("CylinderMesh");

    // Load material to be applied to torus
    //std::string filename = std::string(MATERIAL_DIRECTORY) + std::string("/three-term_shiny_blue");
	std::string filename = std::string(MATERIAL_DIRECTORY) + std::string("/three-term_textured");
    resman_.LoadResource(Material, "3TTexturedMaterial", filename.c_str());

	filename = std::string(MATERIAL_DIRECTORY) + std::string("/metal");
	resman_.LoadResource(Material, "ShinyBlueMetal", filename.c_str());

	filename = std::string(MATERIAL_DIRECTORY) + std::string("/plastic");
	resman_.LoadResource(Material, "PlasticMaterial", filename.c_str());

	filename = std::string(MATERIAL_DIRECTORY) + std::string("/three-term_toon");
	resman_.LoadResource(Material, "ToonMaterial", filename.c_str());

    // Load a cube from an obj file
    filename = std::string(MATERIAL_DIRECTORY) + std::string("/cube.obj");
    resman_.LoadResource(Mesh, "CubeMesh", filename.c_str());

    // Load texture to be applied to the cube
    filename = std::string(MATERIAL_DIRECTORY) + std::string("/randomspace.png");
    resman_.LoadResource(Texture, "Space", filename.c_str());

	// Load texture to be applied to the cube
	filename = std::string(MATERIAL_DIRECTORY) + std::string("/crumpled.png");
	resman_.LoadResource(Texture, "Crumpled", filename.c_str());

	// Load texture to be applied to the cube
	filename = std::string(MATERIAL_DIRECTORY) + std::string("/checker.png");
	resman_.LoadResource(Texture, "Checker", filename.c_str());

    // Load material to be applied to the cube
    filename = std::string(MATERIAL_DIRECTORY) + std::string("/textured_material");
    resman_.LoadResource(Material, "TexturedMaterial", filename.c_str());

	// Load plane mesh
	resman_.CreatePlane("PlaneMesh", glm::vec3(1.0, 1.0, 1.0));

	// Load sphere mesh
	resman_.CreateSphere("SimpleSphereMesh", 1, 20, 20);
}


void Game::SetupScene(void){

    // Set background color for the scene
    scene_.SetBackgroundColor(viewport_background_color_g);
	//CreateAsteroidField();

    // Create a turret
	// base
    game::SceneNode *chopperbase = CreateInstance("HelicopterBase", "CylinderMesh", "3TTexturedMaterial", "Crumpled");
    // Adjust the instance
	chopperbase->Translate(glm::vec3(1.4, 2.0, 0.0));
	chopperbase->Rotate(glm::angleAxis(-glm::pi<float>() / 180.0f * 90.0f, glm::vec3(1.0, 0.0, 0.0)));
    chopperbase->Scale(glm::vec3(0.5, 0.7, 0.5));

	// rotating base
	game::SceneNode *gunbase = CreateInstance("CylinderInstance2", "CylinderMesh", "3TTexturedMaterial", "Crumpled");
	// Adjust the instance
	gunbase->Scale(glm::vec3(0.7, 0.4, 0.7));
	gunbase->Translate(glm::vec3(0.0, 0.3, 0.0));
	gunbase->Rotate(glm::angleAxis(glm::pi<float>() / 180.0f * 45.0f, glm::vec3(0.0, 1.0, 0.0)));

	// first part of gun
	game::SceneNode *gunback = CreateInstance("CylinderInstance3", "CylinderMesh", "3TTexturedMaterial", "Crumpled");
	// Adjust the instance
	gunback->Scale(glm::vec3(0.1, 0.5, 0.1));

	// second part of gun
	game::SceneNode *gunfront = CreateInstance("CylinderInstance4", "CylinderMesh", "3TTexturedMaterial", "Space");
	// Adjust the instance
	gunfront->Scale(glm::vec3(0.075, 0.5, 0.075));
	chopperbase->AddNode(gunbase);
	gunbase->AddNode(gunback);
	gunback->AddNode(gunfront);

	// Ground Plane
	game::SceneNode *plane = CreateInstance("PlaneInstance1", "PlaneMesh", "3TTexturedMaterial", "Crumpled");
	// Adjust the instance
	plane->Scale(glm::vec3(50.0, 50.0, 50.0));
	plane->Rotate(glm::angleAxis(glm::pi<float>() / 180.0f * 90.0f, glm::vec3(1.0, 0.0, 0.0)));
}


void Game::MainLoop(void){

    // Loop while the user did not close the window
    while (!glfwWindowShouldClose(window_)){

        // Animate the scene
        if (animating_){
            static double last_time = 0;
            double current_time = glfwGetTime();
            if ((current_time - last_time) > 0.01){
                scene_.Update();

                // Animate the cube

                // Animate the turret
				SceneNode *node = scene_.GetNode("HelicopterBase");
				if (keys.at("w")) {
					node->Translate(node->GetUp());
				} else
				if (keys.at("s")) {
					node->Translate(-node->GetUp());
				}
				if (keys.at("a")) {
					node->Translate(-node->GetSide());
				} else
				if (keys.at("d")) {
					node->Translate(node->GetSide());
				}
				if (keys.at(" ")) {
					node->Translate(node->GetForward());
				} else
				if (keys.at("lshift")) {
					node->Translate(-node->GetForward());
				}
				camera_.SetPosition(node->GetPosition() - camera_.GetForward()*10.0f);

                node = scene_.GetNode("CylinderInstance2");
				glm::quat rotation = glm::angleAxis(glm::pi<float>() / 180.0f / 2.0f, glm::vec3(0.0, 1.0, 0.0));
				node->Rotate(rotation);
				

				node = scene_.GetNode("CylinderInstance3");
				node->SetOrientation(glm::angleAxis(glm::pi<float>() / 180.0f * (90.0f + (float)cos(glfwGetTime()*5.0f)*20.0f), glm::vec3(0.0, 0.0, 1.0)));
				node->SetOrbit(glm::vec3(0.0, 0.5, 0.0));

				node = scene_.GetNode("CylinderInstance4");
				node->SetPosition(glm::vec3(0.0, 0.25 + sin(glfwGetTime()*20.0f)*0.125, 0.0));

				
            }
        }
		if (materialToggle) {
			
		}
		else {
			
		}

        // Draw the scene
        scene_.Draw(&camera_);

        // Push buffer drawn in the background onto the display
        glfwSwapBuffers(window_);

        // Update other events like input handling
        glfwPollEvents();
    }
}


void Game::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods){

    // Get user data with a pointer to the game class
    void* ptr = glfwGetWindowUserPointer(window);
    Game *game = (Game *) ptr;

    // Quit game if 'q' is pressed
    if (key == GLFW_KEY_Q && action == GLFW_PRESS){
        glfwSetWindowShouldClose(window, true);
    }

    // Stop animation if space bar is pressed
    /*if (key == GLFW_KEY_SPACE && action == GLFW_PRESS){
        game->animating_ = (game->animating_ == true) ? false : true;
    }*/

    // View control
    float rot_factor(glm::pi<float>() / 180);
    float trans_factor = 1.0;
	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_UP) {
			game->camera_.Pitch(rot_factor);
		}
		if (key == GLFW_KEY_DOWN) {
			game->camera_.Pitch(-rot_factor);
		}
		if (key == GLFW_KEY_LEFT) {
			game->camera_.Yaw(rot_factor);
		}
		if (key == GLFW_KEY_RIGHT) {
			game->camera_.Yaw(-rot_factor);
		}
		if (key == GLFW_KEY_W) {
			game->keys.at("w") = true;
		}
		if (key == GLFW_KEY_A) {
			game->keys.at("a") = true;
		}
		if (key == GLFW_KEY_S) {
			game->keys.at("s") = true;
		}
		if (key == GLFW_KEY_D) {
			game->keys.at("d") = true;
		}
		if (key == GLFW_KEY_X) {
			game->camera_.Roll(rot_factor);
		}
		if (key == GLFW_KEY_Z) {
			game->camera_.Roll(-rot_factor);
		}
		if (key == GLFW_KEY_SPACE) {
			game->keys.at(" ") = true;
		}
		if (key == GLFW_KEY_LEFT_SHIFT) {
			game->keys.at("lshift") = true;
		}
	}
	else if (action == GLFW_RELEASE) {
		if (key == GLFW_KEY_W) {
			game->keys.at("w") = false;
		}
		if (key == GLFW_KEY_A) {
			game->keys.at("a") = false;
		}
		if (key == GLFW_KEY_S) {
			game->keys.at("s") = false;
		}
		if (key == GLFW_KEY_D) {
			game->keys.at("d") = false;
		}
		if (key == GLFW_KEY_SPACE) {
			game->keys.at(" ") = false;
		}
		if (key == GLFW_KEY_LEFT_SHIFT) {
			game->keys.at("lshift") = false;
		}
	}
	if (key == GLFW_KEY_R && action == GLFW_PRESS) {
		if (game->materialToggle) {
			game->materialToggle = false;
		}
		else {
			game->materialToggle = true;
		}
	}
}


void Game::ResizeCallback(GLFWwindow* window, int width, int height){

    // Set up viewport and camera projection based on new window size
    glViewport(0, 0, width, height);
    void* ptr = glfwGetWindowUserPointer(window);
    Game *game = (Game *) ptr;
    game->camera_.SetProjection(camera_fov_g, camera_near_clip_distance_g, camera_far_clip_distance_g, width, height);
}


Game::~Game(){
    
    glfwTerminate();
}


Asteroid *Game::CreateAsteroidInstance(std::string entity_name, std::string object_name, std::string material_name, std::string texture_name){

    // Get resources
    Resource *geom = resman_.GetResource(object_name);
    if (!geom){
        throw(GameException(std::string("Could not find resource \"")+object_name+std::string("\"")));
    }

    Resource *mat = resman_.GetResource(material_name);
    if (!mat){
        throw(GameException(std::string("Could not find resource \"")+material_name+std::string("\"")));
    }

	Resource *tex = NULL;
	if (texture_name != "") {
		tex = resman_.GetResource(texture_name);
		if (!mat) {
			throw(GameException(std::string("Could not find resource \"") + material_name + std::string("\"")));
		}
	}

    // Create asteroid instance
    Asteroid *ast = new Asteroid(entity_name, geom, mat, tex);
    scene_.AddNode(ast);
    return ast;
}


void Game::CreateAsteroidField(int num_asteroids){

    // Create a number of asteroid instances
    for (int i = 0; i < num_asteroids; i++){
        // Create instance name
        std::stringstream ss;
        ss << i;
        std::string index = ss.str();
        std::string name = "AsteroidInstance" + index;

        // Create asteroid instance
        Asteroid *ast = CreateAsteroidInstance(name, "SimpleSphereMesh", "ShinyBlueMaterial", "Checker");

        // Set attributes of asteroid: random position, orientation, and
        // angular momentum
        ast->SetPosition(glm::vec3(-300.0 + 600.0*((float) rand() / RAND_MAX), -300.0 + 600.0*((float) rand() / RAND_MAX), 600.0*((float) rand() / RAND_MAX)));
        ast->SetOrientation(glm::normalize(glm::angleAxis(glm::pi<float>()*((float) rand() / RAND_MAX), glm::vec3(((float) rand() / RAND_MAX), ((float) rand() / RAND_MAX), ((float) rand() / RAND_MAX)))));
        ast->SetAngM(glm::normalize(glm::angleAxis(0.05f*glm::pi<float>()*((float) rand() / RAND_MAX), glm::vec3(((float) rand() / RAND_MAX), ((float) rand() / RAND_MAX), ((float) rand() / RAND_MAX)))));
    }
}


SceneNode *Game::CreateInstance(std::string entity_name, std::string object_name, std::string material_name, std::string texture_name){

    Resource *geom = resman_.GetResource(object_name);
    if (!geom){
        throw(GameException(std::string("Could not find resource \"")+object_name+std::string("\"")));
    }

    Resource *mat = resman_.GetResource(material_name);
    if (!mat){
        throw(GameException(std::string("Could not find resource \"")+material_name+std::string("\"")));
    }

    Resource *tex = NULL;
    if (texture_name != ""){
        tex = resman_.GetResource(texture_name);
        if (!mat){
            throw(GameException(std::string("Could not find resource \"")+material_name+std::string("\"")));
        }
    }

    SceneNode *scn = scene_.CreateNode(entity_name, geom, mat, tex);
    return scn;
}

} // namespace game
