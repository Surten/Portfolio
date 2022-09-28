//----------------------------------------------------------------------------------------
/**
 * \file    main.cpp
 * \author  Josef Bacík
 * \date    2020
 * \brief   all necessary functions to handle the scene
 */
 //----------------------------------------------------------------------------------------

#include <time.h>
#include <list>
#include "pgr.h"
#include "render.h"
#include <iostream>
#include <fstream>


typedef std::list<void*> GameObjectsList;
using namespace glm;
float SCENE_WIDTH;
float SCENE_HEIGHT;
float SCENE_DEPTH;
const char* WINDOW_TITLE = "Desert Oasis";
int WINDOW_WIDTH = 800;
int WINDOW_HEIGHT = 600;
//number of palms, range from 1 to 8, changed in config
float NUM_PALMS;
float BANNER_SIZE = 1.5f;
enum { KEY_LEFT_ARROW, KEY_RIGHT_ARROW, KEY_UP_ARROW, KEY_DOWN_ARROW, KEY_SPACE, KEYS_COUNT };
extern SCommonShaderProgram shaderProgram;
//maximum camera up and down view
float CAMERA_ELEVATION_MAX = 45.0f;

//all objects of the scene
struct GameObjects {
	GameObjectsList island;
	GameObjectsList palm;
	Object* fireplace = NULL;
	GameObjectsList cube;
	Object* water;
	HeliObject* helicopter = NULL;
	Fire* fire;
	BannerObject* banner; // NULL;
	GameObjectsList rock;
	Object* bench;
} gameObjects;

//contains information about the scene
struct GameState {
	int windowWidth;    // set by reshape callback
	int windowHeight;   // set by reshape callback
	int cameraNum = 1;

	bool freeCameraMode;        // false;
	float cameraElevationAngle; // in degrees = initially 0.0f
	float movementSpeed = 0.1f;

	bool gameOver;              // false;
	bool keyMap[KEYS_COUNT];    // false

	float elapsedTime;

	bool flashOn;
	bool sunOn;
	bool fogOn;
	bool bannerOn;
	bool fireOn;
} gameState;
struct Camera {
	vec3 position;
	vec3 direction;
	float viewAngle;
	float size = 0.1f;
}camera;

//Create objects
//    |
//   \|/
BannerObject* createBanner(void) {
	BannerObject* newBanner = new BannerObject;

	newBanner->size = BANNER_SIZE;
	newBanner->position = glm::vec3(0.0f, -2.0f, 0.0f);
	newBanner->direction = glm::vec3(0.0f, 1.0f, 0.0f);
	newBanner->speed = 0.0f;

	newBanner->destroyed = false;

	newBanner->startTime = gameState.elapsedTime;
	newBanner->currentTime = newBanner->startTime;

	return newBanner;
}

Object* createIsland(vec3 pos) {
	Object* island = new Object;

	island->destroyed = false;

	island->startTime = gameState.elapsedTime;
	island->currentTime = island->startTime;

	island->size = 1.0f;

	island->direction = glm::vec3(0.0f, 0.0f, 0.0f);
	island->direction = glm::normalize(island->direction);

	island->position = pos;

	return island;
}

Object* createWater(vec3 pos) {
	Object* water = new Object;

	water->destroyed = false;

	water->startTime = gameState.elapsedTime;
	water->currentTime = water->startTime;

	water->size = 1.0f;

	water->direction = glm::vec3(0.0f, 0.0f, 0.0f);
	water->direction = glm::normalize(water->direction);

	water->position = pos;

	return water;
}

Object* createRock(vec3 pos) {
	Object* rock = new Object;

	rock->destroyed = false;

	rock->startTime = gameState.elapsedTime;
	rock->currentTime = rock->startTime;

	rock->size = 0.1f;

	rock->direction = glm::vec3(0.0f, 0.0f, 0.0f);
	rock->direction = glm::normalize(rock->direction);

	rock->position = pos;

	return rock;
}

Object* createBench(vec3 pos) {
	Object* bench = new Object;

	bench->destroyed = false;

	bench->startTime = gameState.elapsedTime;
	bench->currentTime = bench->startTime;

	bench->size = 0.2f;

	bench->direction = glm::vec3(0.0f, 0.0f, 0.0f);
	bench->direction = glm::normalize(bench->direction);

	bench->position = pos;

	return bench;
}

Object* createFireplace(vec3 pos) {
	Object* newFireplace = new Object;

	newFireplace->destroyed = false;

	newFireplace->startTime = gameState.elapsedTime;
	newFireplace->currentTime = newFireplace->startTime;

	newFireplace->size = 0.05f;

	newFireplace->direction = glm::vec3(0.5f, 0.0f, 0.0f);
	newFireplace->direction = glm::normalize(newFireplace->direction);

	newFireplace->position = pos;

	return newFireplace;
}

HeliObject* createHeli() {
	HeliObject* helicopter = new HeliObject;

	helicopter->destroyed = false;

	helicopter->position = glm::vec3(1.0f, 0.0f, 1.0f);
	helicopter->initPosition = glm::vec3(0.0f, 1.0f, 0.0f);
	helicopter->viewAngle = 0.0f;
	helicopter->direction = glm::vec3(cos(glm::radians(helicopter->viewAngle)), sin(glm::radians(helicopter->viewAngle)), 0.0f);
	//helicopter->dirForReflector = glm::vec3(sin(glm::radians(helicopter->viewAngle)), 0.0f, -cos(glm::radians(helicopter->viewAngle)));
	helicopter->speed = 0.5f;
	helicopter->size = 0.3f;
	helicopter->startTime = gameState.elapsedTime;
	helicopter->currentTime = helicopter->startTime;

	return helicopter;
}

Object* createPalm(vec3 pos) {
	Object* palm = new Object;
	palm->destroyed = false;
	palm->startTime = gameState.elapsedTime;
	palm->currentTime = palm->startTime;
	palm->size = 0.5f;
	palm->direction = glm::vec3(0.0f, 0.0f, 0.0f);
	palm->direction = glm::normalize(palm->direction);
	palm->position = glm::vec3(pos);
	return palm;
}

Object* createCube() {
	Object* cube = new Object;
	cube->destroyed = false;
	cube->startTime = gameState.elapsedTime;
	cube->currentTime = cube->startTime;
	cube->size = 0.3f;
	cube->direction = glm::vec3(0.0f, 0.0f, 1.0f);
	cube->direction = glm::normalize(cube->direction);
	cube->position = glm::vec3(-1.0f, 0.0f, +0.2f);
	return cube;
}

Fire* insertFire(const glm::vec3& position) {
	Fire* fire = new Fire;
	fire->speed = 0.0f;
	fire->startTime = gameState.elapsedTime;
	fire->currentTime = fire->startTime;
	fire->size = 0.1f;
	fire->direction = glm::vec3(0.0f, 0.0f, 1.0f);
	fire->frameDuration = 0.1f;
	fire->textureFrames = 16;
	fire->position = position;
	return fire;
}
//   /|\
//    |

/**
Function for reading configuration from file.
\param[in] path		path of the config file
*/
void loadConfigFiles(std::string path) {
	std::ifstream afile;
	afile.open(path);
	afile >> SCENE_WIDTH;
	afile >> SCENE_HEIGHT;
	afile >> SCENE_DEPTH;
	afile >> NUM_PALMS;
	if (NUM_PALMS > 8) {
		NUM_PALMS = 8;
	}
	if (NUM_PALMS < 0) {
		NUM_PALMS = 0;
	}
	afile.close();
}

/**
Keeps free camera position in scene
\param[in]  position       Position (object center) to be checked and corrected.
\param[in]  objectSize     Size of the object which position is tested.
\return                    Valid position inside a scene.
*/
glm::vec3 checkBounds(const glm::vec3& position, float objectSize) {
	glm::vec3 newPosition = position;

	if ((newPosition.x) > (SCENE_WIDTH + objectSize)) {
		newPosition.x = SCENE_WIDTH + objectSize;
	}
	else if (newPosition.x < -(SCENE_WIDTH + objectSize)) {
		newPosition.x = -(SCENE_WIDTH + objectSize);
	}

	if ((newPosition.y) > (SCENE_HEIGHT + objectSize)) {
		newPosition.y = SCENE_HEIGHT + objectSize;
	}
	else if (newPosition.y < -(SCENE_HEIGHT + objectSize)) {
		newPosition.y = -(SCENE_HEIGHT + objectSize);
	}
	return newPosition;
}

/**
* \Updates all objects that need updating
* \param[in] elapsedTime  local time of the scene
*/
void updateScene(float elapsedTime) {

	//camera handle
	camera.position = checkBounds(camera.position, camera.size);

	if (gameState.cameraNum == 5) {
		gameState.flashOn = false;
		gameState.freeCameraMode = false;
		camera.position = gameObjects.helicopter->position;
	}
	else if (gameState.cameraNum == 2) {
		camera.position = vec3(1.5f, 0.0f, 0.2f);
		camera.direction = vec3(-1.0f, 0.0f, 0.0f);
		gameState.freeCameraMode = false;
		glutPassiveMotionFunc(NULL);
	}
	else if (gameState.cameraNum == 1) {
		camera.position = vec3(0.0, 0.0, 0.0);
		camera.direction = vec3(0.0, 0.0, 0.0);
		gameState.freeCameraMode = false;
		glutPassiveMotionFunc(NULL);
	}
	else {
		gameState.freeCameraMode = true;
		if (camera.position.b != 0) {
			camera.position.b = 0;
		}
	}
	// helicopter doing circles
	gameObjects.helicopter->currentTime = elapsedTime;
	glm::vec3 pos = gameObjects.helicopter->position;
	gameObjects.helicopter->position = (vec4(cos(gameObjects.helicopter->currentTime * gameObjects.helicopter->speed), sin(gameObjects.helicopter->currentTime * gameObjects.helicopter->speed), 1.0, 0.0));
	glm::vec3 dir = pos - gameObjects.helicopter->position;
	gameObjects.helicopter->direction = glm::normalize(dir);
	// fire local time update
	if (gameObjects.fire != NULL)
		gameObjects.fire->currentTime = elapsedTime;

}

void moveRight() {
	camera.position -= (gameState.movementSpeed * vec3(-camera.direction.y, camera.direction.x, camera.direction.z))/vec3(2);
}
void moveLeft() {
	camera.position += gameState.movementSpeed * vec3(-camera.direction.y, camera.direction.x, camera.direction.z) / vec3(2);
}
void moveForward() {
	camera.position += gameState.movementSpeed * camera.direction / vec3(2);
}
void moveBackwards() {
	camera.position -= gameState.movementSpeed * camera.direction / vec3(2);
}
void turnLeft(float deltaAngle) {
	camera.viewAngle += deltaAngle;
	if (camera.viewAngle >= 360) camera.viewAngle -= 360;
	camera.direction = vec3(cos(radians(camera.viewAngle)), sin(radians(camera.viewAngle)), 0);
}
void turnRight(float deltaAngle) {
	camera.viewAngle -= deltaAngle;
	if (camera.viewAngle < 0) camera.viewAngle += 360;
	camera.direction = vec3(cos(radians(camera.viewAngle)), sin(radians(camera.viewAngle)), 0);
}

void cleanUpObjects(void) {
	
	while (!gameObjects.island.empty()) {
		delete gameObjects.island.back();
		gameObjects.island.pop_back();
	}
	while (!gameObjects.palm.empty()) {
		delete gameObjects.palm.back();
		gameObjects.palm.pop_back();
	}
	while (!gameObjects.cube.empty()) {
		delete gameObjects.cube.back();
		gameObjects.cube.pop_back();
	}
	while (!gameObjects.rock.empty()) {
		delete gameObjects.rock.back();
		gameObjects.rock.pop_back();
	}
	
	gameObjects.bench = NULL;
	gameObjects.water = NULL;
	gameObjects.fireplace = NULL;
	gameObjects.fire = NULL;
	gameObjects.helicopter = NULL;

	// remove banner
	if (gameObjects.banner != NULL) {
		delete gameObjects.banner;
		gameObjects.banner = NULL;
	}
}

/**
* \creates all objects, sets scene parametrs
*/
void restartGame(void) {
	cleanUpObjects();
	//start of free camera
	gameState.cameraNum = 4;

	//time
	gameState.elapsedTime = 0.001f * (float)glutGet(GLUT_ELAPSED_TIME); // milliseconds => seconds

	//camera init
	camera.position = vec3(0.0f, 0.0f, 0.0f);
	camera.direction = vec3(cos(radians(camera.viewAngle)), sin(radians(camera.viewAngle)), 0.0f);
	camera.viewAngle = 90.0f;
	gameState.cameraElevationAngle = 0.0f;

	//all the objects get initialized
	Object* water = createWater(vec3(0.0f, 0.0f, -0.15f));
	gameObjects.water = water;

	Object* island1 = createIsland(vec3(1.5f, 0.0f, -0.15f));
	gameObjects.island.push_back(island1);
	Object* island2 = createIsland(vec3(1.5f, 1.5f, -0.15f));
	gameObjects.island.push_back(island2);
	Object* island3 = createIsland(vec3(0.0f, 1.5f, -0.15f));
	gameObjects.island.push_back(island3);
	Object* island4 = createIsland(vec3(-1.5f, 0.0f, -0.15f));
	gameObjects.island.push_back(island4);
	Object* island5 = createIsland(vec3(-1.5f, -1.5f, -0.15f));
	gameObjects.island.push_back(island5);
	Object* island6 = createIsland(vec3(0.0f, -1.5f, -0.15f));
	gameObjects.island.push_back(island6);
	Object* island7 = createIsland(vec3(-1.5f, 1.5f, -0.15f));
	gameObjects.island.push_back(island7);
	Object* island8 = createIsland(vec3(1.5f, -1.5f, -0.15f));
	gameObjects.island.push_back(island8);

	if (gameState.freeCameraMode == true) {
		gameState.freeCameraMode = false;
		glutPassiveMotionFunc(NULL);
	}
	gameState.cameraElevationAngle = 0.0f;

	// reset key map
	for (int i = 0; i < KEYS_COUNT; i++)
		gameState.keyMap[i] = false;

	Object* cube = createCube();
	gameObjects.cube.push_back(cube);

	Object* rock = createRock(vec3(1.0f,1.0f,-0.1f));
	gameObjects.rock.push_back(rock);

	Object* bench = createBench(vec3(1.4f, 0.0f, -0.01f));
	gameObjects.bench = bench;

	gameObjects.fireplace = createFireplace(vec3(1.2f, 0.0f, -0.09f));

	if (gameObjects.helicopter == NULL)
		gameObjects.helicopter = createHeli();

	Object* palm1 = createPalm(vec3(2.0f, 0.0f, 0.4f));
	gameObjects.palm.push_back(palm1);

	Object* palm2 = createPalm(vec3(-2.0f, 0.0f, 0.4f));
	gameObjects.palm.push_back(palm2);

	Object* palm3 = createPalm(vec3(0.0f, -2.0f, 0.4f));
	gameObjects.palm.push_back(palm3);
	
	Object* palm4 = createPalm(vec3(0.0f, 2.0f, 0.4f));
	gameObjects.palm.push_back(palm4);
	
	Object* palm5 = createPalm(vec3(-1.5f, 1.5f, 0.4f));
	gameObjects.palm.push_back(palm5);
	
	Object* palm6 = createPalm(vec3(1.5f, -1.5f, 0.4f));
	gameObjects.palm.push_back(palm6);
	
	Object* palm7 = createPalm(vec3(-1.5f, -1.5f, 0.4f));
	gameObjects.palm.push_back(palm7);
	
	Object* palm8 = createPalm(vec3(1.5f, 1.5f, 0.4f));
	gameObjects.palm.push_back(palm8);

	if (gameObjects.fire == NULL) {
		glm::vec3 a = gameObjects.fireplace->position;
		a.z = 0.00f;
		gameObjects.fire = insertFire(a);
	}

	// set scene info
	gameState.fogOn = false;
	disableFog();
	gameState.fireOn = false;
	gameState.sunOn = true;
	gameState.bannerOn = false;
	gameState.flashOn = false;
}

/**
* \draws all objects of the scene, updates camera view
*/
void drawEntireWindow() {
	loadConfigFiles("config.txt");

	// setup parallel projection
	glm::mat4 orthoProjectionMatrix = glm::ortho(
		-SCENE_WIDTH, SCENE_WIDTH,
		-SCENE_HEIGHT, SCENE_HEIGHT,
		-10.0f * SCENE_DEPTH, 10.0f * SCENE_DEPTH
	);
	// static viewpoint - top view
	glm::mat4 orthoViewMatrix = glm::lookAt(
		glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f)
	);

	// setup camera & projection transform
	glm::mat4 viewMatrix = orthoViewMatrix;
	glm::mat4 projectionMatrix = orthoProjectionMatrix;

	if (gameState.freeCameraMode == true) {
		vec3 cameraPosition = camera.position;
		vec3 cameraCenter = cameraPosition + camera.direction;
		vec3 cameraUpVector = vec3(0.0f, 0.0f, 1.0f);

		cameraCenter.x *= cos(radians(gameState.cameraElevationAngle));
		cameraCenter.y *= cos(radians(gameState.cameraElevationAngle));
		cameraCenter.z = -sin(radians(gameState.cameraElevationAngle));


		viewMatrix = glm::lookAt(
			cameraPosition,
			cameraCenter,
			cameraUpVector
		);
		projectionMatrix = glm::perspective(glm::radians(60.0f), gameState.windowWidth / (float)gameState.windowHeight, 0.1f, 10.0f);
	}
	else if (gameState.cameraNum == 2) {
		viewMatrix = glm::lookAt(
			camera.position,
			camera.position + camera.direction,
			vec3(0, 0, 1)
		);
		projectionMatrix = glm::perspective(glm::radians(60.0f), gameState.windowWidth / (float)gameState.windowHeight, 0.1f, 10.0f);
	}
	else if (gameState.cameraNum == 5) {
		viewMatrix = glm::lookAt(
			gameObjects.helicopter->position,
			vec3(0, 0, 0),
			vec3(0, 0, 1)
		);
		projectionMatrix = glm::perspective(glm::radians(60.0f), gameState.windowWidth / (float)gameState.windowHeight, 0.1f, 10.0f);
	}

	//set shader uniforms
	glUseProgram(shaderProgram.program);
	glUniform1f(shaderProgram.time, gameState.elapsedTime);
	glUniform3fv(shaderProgram.reflectorPosition, 1, glm::value_ptr(camera.position));
	glUniform3fv(shaderProgram.reflectorDirection, 1, glm::value_ptr(camera.direction));
	glUniform1i(shaderProgram.sunOn, gameState.sunOn);
	glUniform1i(shaderProgram.flashOn, gameState.flashOn);
	glUniform1i(shaderProgram.pointlightison, gameState.fireOn);
	glUniform4fv(shaderProgram.pointlightposition, 1, glm::value_ptr(glm::vec4(gameObjects.fire->position, 1.0f)));
	glUniform1i(shaderProgram.fogOn, gameState.fogOn);
	glUseProgram(0);

	// stencil test to get position of object on mouse click
	//draw all objects

	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glStencilFunc(GL_ALWAYS, 20, 0xFF);
	drawFireplace(gameObjects.fireplace, viewMatrix, projectionMatrix);
	glDisable(GL_STENCIL_TEST);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_DEPTH_TEST);

	for (GameObjectsList::iterator it = gameObjects.island.begin(); it != gameObjects.island.end(); ++it) {
		Object* island = (Object*)(*it);
		drawIsland(island, viewMatrix, projectionMatrix);
	}

	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glStencilFunc(GL_ALWAYS, 25, 0xFF);
	drawHeli(gameObjects.helicopter, viewMatrix, projectionMatrix);
	glDisable(GL_STENCIL_TEST);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glStencilFunc(GL_ALWAYS, 5, 0xFF);
	//only draw number of palms specified in the config file
	int i = 0;
	for (GameObjectsList::iterator it = gameObjects.palm.begin(); it != gameObjects.palm.end(); ++it) {
		i++;
		Object* palm = (Object*)(*it);
		drawPalm(palm, viewMatrix, projectionMatrix);
		if (i == NUM_PALMS) {
			break;
		}
	}
	glDisable(GL_STENCIL_TEST);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glStencilFunc(GL_ALWAYS, 2, 0xFF);
	for (GameObjectsList::iterator it = gameObjects.rock.begin(); it != gameObjects.rock.end(); ++it) {
		Object* rock = (Object*)(*it);
		drawRock(rock, viewMatrix, projectionMatrix);
	}
	glDisable(GL_STENCIL_TEST);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	drawWater(gameObjects.water, viewMatrix, projectionMatrix);
	glDisable(GL_STENCIL_TEST);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	drawBench(gameObjects.bench, viewMatrix, projectionMatrix);
	glDisable(GL_STENCIL_TEST);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_DEPTH_TEST);
	
	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glStencilFunc(GL_ALWAYS, 7, 0xFF);
	for (GameObjectsList::iterator it = gameObjects.cube.begin(); it != gameObjects.cube.end(); ++it) {
		Object* cube = (Object*)(*it);
		drawCube(cube, viewMatrix, projectionMatrix);
	}
	
	glDisable(GL_STENCIL_TEST);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_DEPTH_TEST);
	drawSkybox(viewMatrix, projectionMatrix);

	if (gameState.fireOn == true)
		drawFire(gameObjects.fire, viewMatrix, projectionMatrix);

	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glStencilFunc(GL_ALWAYS, 3, 0xFF);
	if (gameState.bannerOn == true) {
		// Drawing of banner object
		if (gameObjects.banner != NULL)
			drawBanner(gameObjects.banner, orthoViewMatrix, orthoProjectionMatrix);
	}
	glDisable(GL_STENCIL_TEST);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_DEPTH_TEST);

}

// on free camera mode, calculate camera direction based on mouse movement
void passiveMouseMotion(int mouseX, int mouseY) {
	if (mouseY != gameState.windowHeight / 2) {
		float cameraElevationAngleDelta = 0.5f * (mouseY - gameState.windowHeight / 2);
		if (fabs(gameState.cameraElevationAngle + cameraElevationAngleDelta) < CAMERA_ELEVATION_MAX)
			gameState.cameraElevationAngle += cameraElevationAngleDelta;
		glutWarpPointer(gameState.windowWidth / 2, gameState.windowHeight / 2);
	}
	if (mouseX != gameState.windowWidth / 2) {
		float delta = 0.5f * (mouseX - gameState.windowWidth / 2);
		if ((mouseX - gameState.windowWidth / 2) < 0)
			turnLeft(-delta);
		else
			turnRight(delta);
		glutWarpPointer(gameState.windowWidth / 2, gameState.windowHeight / 2);
	}
}

void keyboard(unsigned char keyPressed, int mouseX, int mouseY) {

	switch (keyPressed) {
	case 27: // escape
		glutLeaveMainLoop();
		break;
	case 'w':
		gameState.keyMap[KEY_UP_ARROW] = true;
		break;
	case 's':
		gameState.keyMap[KEY_DOWN_ARROW] = true;
		break;
	case 'a':
		gameState.keyMap[KEY_LEFT_ARROW] = true;
		break;
	case 'd':
		gameState.keyMap[KEY_RIGHT_ARROW] = true;
		break;
	case 'c': // switch camera
		gameState.cameraNum++;
		if (gameState.cameraNum > 5)gameState.cameraNum = 1;
		break;
	case 'r': // restart game
		restartGame();
		break;
	}
}

void keyboardUp(unsigned char keyReleased, int mouseX, int mouseY) {

	switch (keyReleased) {
	case 'w':
		gameState.keyMap[KEY_UP_ARROW] = false;
		break;
	case 's':
		gameState.keyMap[KEY_DOWN_ARROW] = false;
		break;
	case 'a':
		gameState.keyMap[KEY_LEFT_ARROW] = false;
		break;
	case 'd':
		gameState.keyMap[KEY_RIGHT_ARROW] = false;
		break;
	}
}

void specialKeyboard(int specKeyPressed, int mouseX, int mouseY) {

	if (gameState.gameOver == true)
		return;

	switch (specKeyPressed) {
	case GLUT_KEY_UP:
		gameState.keyMap[KEY_UP_ARROW] = true;
		break;
	case GLUT_KEY_RIGHT:
		gameState.keyMap[KEY_RIGHT_ARROW] = true;
		break;
	case GLUT_KEY_LEFT:
		gameState.keyMap[KEY_LEFT_ARROW] = true;
		break;
	case GLUT_KEY_DOWN:
		gameState.keyMap[KEY_DOWN_ARROW] = true;
		break;
	}
}

void specialKeyboardUp(int specKeyReleased, int mouseX, int mouseY) {

	if (gameState.gameOver == true)
		return;

	switch (specKeyReleased) {
	case GLUT_KEY_UP:
		gameState.keyMap[KEY_UP_ARROW] = false;
		break;
	case GLUT_KEY_RIGHT:
		gameState.keyMap[KEY_RIGHT_ARROW] = false;
		break;
	case GLUT_KEY_DOWN:
		gameState.keyMap[KEY_DOWN_ARROW] = false;
		break;
	case GLUT_KEY_LEFT:
		gameState.keyMap[KEY_LEFT_ARROW] = false;
		break;
	}
}

//Handles clicking on objects
void mouseCallback(int buttonPressed, int buttonState, int mouseX, int mouseY) {
	// do picking only on mouse down
	if ((buttonPressed == GLUT_LEFT_BUTTON) && (buttonState == GLUT_DOWN)) {
		// stores value from the stencil buffer (byte)
		unsigned char objectID = 0;
		glReadPixels(mouseX, gameState.windowHeight - mouseY - 1, 1, 1, GL_STENCIL_INDEX, GL_UNSIGNED_BYTE, &objectID);
		if (objectID == 20) {
			gameState.fireOn = !gameState.fireOn;
			printf("Fire ON\n");
		}
		else if (objectID == 25) {
			gameState.cameraNum = 5;
		}
		else if (objectID == 7) {
			printf("I think Steve trolled me, or maybe cubes are in desert/n");
			gameObjects.cube.clear();
		}
		else {
			printf("Clicked on unrecognized object >> %d\n", (int)objectID);
		}
	}
}

void display() {
	GLbitfield mask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;
	mask |= GL_STENCIL_BUFFER_BIT;

	glClear(mask);

	drawEntireWindow();

	glutSwapBuffers();
}

void reshape(int newWidth, int newHeight) {

	gameState.windowWidth = newWidth;
	gameState.windowHeight = newHeight;

	glViewport(0, 0, (GLsizei)newWidth, (GLsizei)newHeight);
}

/**
* \timer func called every so often to keep the scene going
*/
void timer(int) {
	// update scene time
	gameState.elapsedTime = 0.001f * (float)glutGet(GLUT_ELAPSED_TIME);

	//free camera movement
	if (gameState.cameraNum == 4) {
		if (gameState.keyMap[KEY_UP_ARROW] == true)
			moveForward();
		if (gameState.keyMap[KEY_DOWN_ARROW] == true)
			moveBackwards();
		if (gameState.keyMap[KEY_RIGHT_ARROW] == true)
			moveRight();
		if (gameState.keyMap[KEY_LEFT_ARROW] == true)
			moveLeft();
	}
	updateScene(gameState.elapsedTime);
	//create banner if bannerOn is true
	if (gameState.bannerOn == true) {
		if (gameObjects.banner == NULL) {
			//create banner
			gameObjects.banner = createBanner();
		}
	}
	if (gameObjects.banner != NULL)
		gameObjects.banner->currentTime = gameState.elapsedTime;
	
	if (gameState.freeCameraMode == true)
		glutPassiveMotionFunc(passiveMouseMotion);

	// set next timer
	glutTimerFunc(33, timer, 0);

	glutPostRedisplay();
}

/**
* \Called after the window and OpenGL are initialized. Called exactly once, before the main loop.
*/
void init() {

	// initialize random seed
	srand((unsigned int)time(NULL));

	// initialize OpenGL
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClearStencil(0);  // this is the default value
	glEnable(GL_DEPTH_TEST);
	gameObjects.banner = NULL;
	// initialize shaders
	initializeShaders();
	// create geometry for all models used
	initializeModels();

	restartGame();
}

void cleanUp (void) {

	cleanUpObjects();

	delete gameObjects.helicopter;
	gameObjects.helicopter = NULL;

	// delete buffers - banner
	cleanupModels();

	// delete shaders
	cleanupShaderPrograms();
}
//Reaction on menu item.
void gameMenu(int choice) {
	switch (choice) {
	case 1:
		//static camera 1
		gameState.cameraNum = 1;
		gameState.freeCameraMode = false;
		glutPassiveMotionFunc(NULL);
		break;
	case 2:
		//static camera 2
		gameState.cameraNum = 2;
		gameState.freeCameraMode = false;
		glutPassiveMotionFunc(NULL);
		break;
	case 3:
		//close
		exit(0);
		break;
	case 4:
		//free camera
		gameState.cameraNum = 4;
		gameState.freeCameraMode = true;
		glutPassiveMotionFunc(passiveMouseMotion);
		glutWarpPointer(gameState.windowWidth / 2, gameState.windowHeight / 2);
		break;
	case 5:
		//drone camera
		gameState.cameraNum = 5;
		gameState.freeCameraMode = false;
		glutPassiveMotionFunc(passiveMouseMotion);
		glutWarpPointer(gameState.windowWidth / 2, gameState.windowHeight / 2);
		break;
	case 6:
		gameState.flashOn = true;
		break;
	case 7:
		gameState.flashOn = false;
		break;
	case 8:
		//sun on
		gameState.sunOn = true;
		break;
	case 9:
		//sun off
		gameState.sunOn = false;
		break;
	case 10:
		//fog on if camera is 3 or 4
		gameState.fogOn = true;
		enableFog();
		break;
	case 11:
		//fog off if camera is 3 or 4
		gameState.fogOn = false;
		disableFog();
		break;
	case 14:
		//you can put the car on fire if you have wood - cut the palm
		gameState.fireOn = true;
		break;
	case 15:
		//fire off
		gameState.fireOn = false;
		break;
	case 17:
		//banner on
		printf("banner on\n");
		gameState.bannerOn = true;
		break;
	case 18:
		//banner off
		printf("banner off\n");
		gameState.bannerOn = false;
		if (gameObjects.banner != NULL) {
			gameObjects.banner = NULL;
		}
		break;
	case 27:
		//restart the game
		restartGame();
		break;
	}
	glutPostRedisplay();
}

void createMenu(void) {
	//camera setup
	int submenuCamera = glutCreateMenu(gameMenu);
	glutAddMenuEntry("Static camera 1", 1);
	glutAddMenuEntry("Static camera 2", 2);
	glutAddMenuEntry("Free Camera", 4);
	glutAddMenuEntry("Drone Camera", 5);

	//flashlight on/off
	int submenuReflector = glutCreateMenu(gameMenu);
	glutAddMenuEntry("On", 6);
	glutAddMenuEntry("Off", 7);

	//sun on/off
	int submenuSun = glutCreateMenu(gameMenu);
	glutAddMenuEntry("On", 8);
	glutAddMenuEntry("Off", 9);
	//fog on/off
	int submenuFog = glutCreateMenu(gameMenu);
	glutAddMenuEntry("On", 10);
	glutAddMenuEntry("Off", 11);

	//fire on/off
	int submenuFire = glutCreateMenu(gameMenu);
	glutAddMenuEntry("On", 14);
	glutAddMenuEntry("Off", 15);

	//banner on/of
	int submenuBanner = glutCreateMenu(gameMenu);
	glutAddMenuEntry("On", 17);
	glutAddMenuEntry("Off", 18);

	//create menu
	glutCreateMenu(gameMenu);
	glutAddSubMenu("Camera", submenuCamera);
	glutAddSubMenu("Sun", submenuSun);
	glutAddSubMenu("Flashlight", submenuReflector);
	glutAddSubMenu("Fog", submenuFog);
	glutAddSubMenu("Fire", submenuFire);
	glutAddSubMenu("Banner", submenuBanner);
	glutAddMenuEntry("Restart", 27);
	glutAddMenuEntry("Quit", 3);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

int main(int argc, char** argv) {
	loadConfigFiles("config.txt");
	// initialize windowing system
	glutInit(&argc, argv);

	glutInitContextVersion(pgr::OGL_VER_MAJOR, pgr::OGL_VER_MINOR);
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_STENCIL);

	// initial window size
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow(WINDOW_TITLE);

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutKeyboardUpFunc(keyboardUp);
	glutSpecialFunc(specialKeyboard);
	glutSpecialUpFunc(specialKeyboardUp);
	glutMouseFunc(mouseCallback);

	createMenu();

	glutTimerFunc(33, timer, 0);

	if (!pgr::initialize(pgr::OGL_VER_MAJOR, pgr::OGL_VER_MINOR))
		pgr::dieWithError("pgr init failed, required OpenGL not supported?");
	init();
	glutCloseFunc(cleanUp);


	glutMainLoop();

	return 0;
}
