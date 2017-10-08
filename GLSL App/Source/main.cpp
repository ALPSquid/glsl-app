#include "stdafx.h"
#include <Windows.h>
#include <iostream>
#include <fstream>

#include "glew.h"
#include "glm/glm.hpp"
#include "glm/gtx/string_cast.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "gl/GL.h"
#include "gl/GLU.h"
#include "GL/freeglut.h"

#include "Utils/Utils.h"
#include "Graphics/Model.h"
#include "World.h"
#include "Components/RotatingComponent.h"
#include "Components/InteractableComponent.h"
#include "Utils/MeshUtils.h"

void init();
void idle();
void display();
void reshape(int w, int h);
void moveCamera();
void onMouse(int button, int state, int x, int y);
void onMouseMoved(int x, int y);
void onKeyDown(unsigned char key, int x, int y);
void onKeyUp(unsigned char key, int x, int y);

Entity::EntityPtr createTorus(float outerRadius, float innerRadius, int majorSegments, int minorSegments);


// Current world instance.
World world;

// Delta time and FPS vars.
int lastElapsedTime = 0; // Last time the delta time was calculated.
float frameTimeCounter = 0; // Frame time counter for counting seconds.
int frame; // frame counter for calculating fps.
float deltaTime; // Last frame delta time.
int fps; // Current FPS.



int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitWindowPosition(10, 10);
	glutInitWindowSize(1280, 720);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow("GLSL App - Andy Palmer");

	glutIdleFunc(idle);
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(onKeyDown);
	glutKeyboardUpFunc(onKeyUp);
	glutMouseFunc(onMouse);
	glutMotionFunc(onMouseMoved);

	glewInit();
	init();

	glutMainLoop();

	return 0;
}

void init() {
	glEnable(GL_DEPTH_TEST);

	world.init();
	world.setSkyboxTexture(
		"assets/skybox/sea/sea_rt.jpg",
		"assets/skybox/sea/sea_lf.jpg",
		"assets/skybox/sea/sea_up.jpg",
		"assets/skybox/sea/sea_dn.jpg",
		"assets/skybox/sea/sea_bk.jpg",
		"assets/skybox/sea/sea_ft.jpg"
	);

	Entity::EntityPtr cube1 = world.createEntity("assets/models/cube.obj");
	cube1->model.addTexture("assets/models/crate_diffuse.jpg", ShaderLoader::Vars::MAT_DIFFUSE);
	cube1->model.addTexture("assets/models/crate_specular.jpg", ShaderLoader::Vars::MAT_SPECULAR);
	cube1->model.addTexture("assets/models/crate_normal.jpg", ShaderLoader::Vars::MAT_NORMAL);
	cube1->model.setMaterial(
		glm::vec3(1), // diffuse
		glm::vec3(4), // specular
		64 // shininess,
	);
	cube1->setPosition(2, 0, 0);
	cube1->addComponent<InteractableComponent>();
	RotatingComponent* cubeRotComp = cube1->addComponent<RotatingComponent>();
	cubeRotComp->axis = UP_VECTOR - RIGHT_VECTOR;
	cubeRotComp->speed = 10;

	Entity::EntityPtr ship = world.createEntity("assets/models/ship/ship.obj");
	ship->model.addTexture("assets/models/ship/SF_Corvette-F3_specular.jpg", ShaderLoader::Vars::MAT_SPECULAR);
	ship->setPosition(20, -5, 0);
	ship->addComponent<InteractableComponent>();
	RotatingComponent* shipRotComp = ship->addComponent<RotatingComponent>();
	shipRotComp->axis = UP_VECTOR;
	shipRotComp->speed = 20;

	Entity::EntityPtr wall = world.createEntity("assets/models/wall/wall.obj");
	wall->model.addTexture("assets/models/wall/brickwall_normal.jpg", ShaderLoader::Vars::MAT_NORMAL);
	wall->model.setMaterial(
		glm::vec3(1),
		glm::vec3(3),
		100
	);
	wall->setPosition(10, 0, -10);
	wall->rotateBy(90, wall->getRightVector());
	wall->addComponent<InteractableComponent>();
	RotatingComponent* wallRotComp = wall->addComponent<RotatingComponent>();
	wallRotComp->axis = wall->getUpVector();
	wallRotComp->speed = 5;

	Model::ImportSettings hulkImportSettings;
	hulkImportSettings.invertYCoord = true; // Y texture coord needs inverting.
	auto hulk = world.createEntity("assets/models/Hulk/Hulk.obj", hulkImportSettings);
	hulk->setPosition(20, -5, 10);
	hulk->addComponent<InteractableComponent>();

	auto torus = world.createEntity(MeshUtils::createTorus(1, 0.8f, 25, 20));
	torus->model.addTexture("assets/models/crate_diffuse.jpg", ShaderLoader::Vars::MAT_DIFFUSE);
	torus->model.setMaterial(
		glm::vec3(1),
		glm::vec3(4),
		100
	);
	torus->setPosition(-10, 0, 0);
	torus->addComponent<InteractableComponent>();
	// Rotation
	RotatingComponent* torusRotComp = torus->addComponent<RotatingComponent>();
	torusRotComp->speed = 5;
	torusRotComp->axis = RIGHT_VECTOR;

	auto torus2 = world.createEntity(MeshUtils::createTorus(2, 1, 30, 30));
	torus2->model.addTexture("assets/models/wall/brickwall.jpg", ShaderLoader::Vars::MAT_DIFFUSE);
	torus2->model.setMaterial(
		glm::vec3(1),
		glm::vec3(4),
		100
	);
	torus2->setPosition(-5, 0, -5);
	torus2->addComponent<InteractableComponent>();
	// Rotation
	RotatingComponent* torus2RotComp = torus2->addComponent<RotatingComponent>();
	torus2RotComp->speed = 10;
	torus2RotComp->axis = UP_VECTOR + RIGHT_VECTOR;
}

void idle() {
	// Update delta time.
	int elapsedTime = glutGet(GLUT_ELAPSED_TIME);
	deltaTime = (elapsedTime - lastElapsedTime) * 0.001f;
	lastElapsedTime = elapsedTime;

	// Calculate frames per second.
	frame++;
	frameTimeCounter += deltaTime;
	if (frameTimeCounter >= 1.f) {
		// If it's been 1 second since the last check.
		fps = frame;
		frameTimeCounter -= 1.f; // Minus 1, rather than setting to 0, so no time is discarded.
		frame = 0;
	}
	std::cout << "\rFPS: " << fps << std::flush;
	//

	glutPostRedisplay();
}

void display() {
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Update the world.
	world.update(deltaTime);
	// Render the world.
	world.render();

	glutSwapBuffers();
}

void reshape(int w, int h) {
	if (h == 0) h = 1;
	glViewport(0, 0, w, h);

	// Update camera rendering settings.
	world.getCamera().updateMatrices(70.f, w, h, 1.f, 300.f);

	glutPostRedisplay();
}


//--- Input.
void onMouse(int button, int state, int x, int y) {
	world.getInputManager().onMouse(button, state, x, y);
}

void onMouseMoved(int x, int y) {
	world.getInputManager().onMouseMoved(x, y);
}

void onKeyDown(unsigned char key, int x, int y) {
	world.getInputManager().onKeyDown(key, x, y);
}

void onKeyUp(unsigned char key, int x, int y) {
	world.getInputManager().onKeyUp(key, x, y);
}