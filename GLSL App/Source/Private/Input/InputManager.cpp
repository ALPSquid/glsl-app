#include "../stdafx.h"
#include "Input/InputManager.h"
#include "glew.h"
#include "GL/freeglut.h"
#include "World.h"
#include "utils\Utils.h"

InputManager::InputManager() {}

void InputManager::init() {
	selectionShader = ShaderLoader::createShaderProgram("shaders/SelectionShader/SelectionVertex.glsl", "shaders/SelectionShader/SelectionFragment.glsl");
}

void InputManager::update(std::vector<Entity::EntityPtr> entities) {
	// Update last mouse position so stationary input is registered.
	lastMousePos = currentMousePos;

	// --- Selection buffer ---
	// Each entity is rendered with a unique colour which can then be sampled using the mouse position to detect the selected entity.
	glUseProgram(selectionShader);

	// Update matrices.
	if (entities.size() >= 1) entities[0]->getWorld()->updateVP(selectionShader);

	for (int i = 0; i < entities.size(); i++) {
		// Use entity index as the colour code. For more than 255 objects, this would be changed to use a vec4 (RGBA, rather than just R);
		ShaderLoader::setShaderValue(selectionShader, ShaderLoader::Vars::COLOUR_CODE, (GLuint)i+1);
		// Render the entity.
		entities[i]->render(selectionShader);
	}

	// Get selected entity.
	GLint viewport[4];
	unsigned char sample[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	// Get the colour of the pixel under the mouse. Y is flipped as screen space uses negative Y and buffer space uses positive Y.
	glReadPixels(currentMousePos.x, viewport[3] - currentMousePos.y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, sample);
	int colourCode = sample[0];
	if (colourCode > 0) {
		// Since the colourCode used was the entity index + 1, then the selected entity is at colourCode - 1.
		// Skip, though, if the selected entity is already selected.
		if (entities.size() >= colourCode && selectedEntity != entities[colourCode - 1].get()) {
			// Notify the currently selected entity that the mouse has stopped hovering over it.
			if (selectedEntity) processEvents(MOUSE_OUT, entityEventBindings[selectedEntity]);
			// Get the new selected entity.
			selectedEntity = entities[colourCode - 1].get();
			// Notify the entity that the mouse has entered it.
			if (selectedEntity) processEvents(MOUSE_OVER, entityEventBindings[selectedEntity]);
		}		
	} else {
		// Mouse is in open space.
		if (selectedEntity) processEvents(MOUSE_OUT, entityEventBindings[selectedEntity]);
		selectedEntity = nullptr;
	}

	// Clear the buffer for normal rendering.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

// Input functions.
void InputManager::onMouse(int button, int state, int x, int y) {	
	currentMousePos = glm::vec2(x, y);
	lastMousePos = currentMousePos;

	// Select trigger based on the button state.
	EInputTrigger trigger = (state == GLUT_DOWN) ? INPUT_PRESSED : INPUT_RELEASED;
	// Process triggers for the target button.
	switch (button) {
		case GLUT_LEFT_BUTTON:
			processTriggers(trigger, mouseTriggerBindings[EMouseButton::MOUSE_LEFT]);
			break;
		case GLUT_RIGHT_BUTTON:
			processTriggers(trigger, mouseTriggerBindings[EMouseButton::MOUSE_RIGHT]);
			break;
		case GLUT_MIDDLE_BUTTON:
			processTriggers(trigger, mouseTriggerBindings[EMouseButton::MOUSE_MIDDLE]);
			break;
	}
}

void InputManager::onMouseMoved(int x, int y) {
	lastMousePos = currentMousePos;
	currentMousePos = glm::vec2(x, y);
	glm::vec2 delta = currentMousePos - lastMousePos;
	// Apply mouse sensitivity scaling.
	delta *= mouseSensitivity;

	// Trigger axis callback with the delta values.
	for (auto& callback : mouseAxisBindings[EMouseAxis::MOUSE_X]) {
		callback(delta.x);
	}
	// Y
	for (auto& callback : mouseAxisBindings[EMouseAxis::MOUSE_Y]) {
		callback(delta.y);
	}
}

void InputManager::onKeyDown(unsigned char key, int x, int y) {
	// Trigger pressed callbacks.
	processTriggers(EInputTrigger::INPUT_PRESSED, triggerBindings[key]);
	
}

void InputManager::onKeyUp(unsigned char key, int x, int y) {
	// Trigger released callbacks.
	processTriggers(EInputTrigger::INPUT_RELEASED, triggerBindings[key]);
}


// Bindings.
void InputManager::addTriggerBinding(unsigned char key, EInputTrigger triggerType, TriggerBinding callback) {
	triggerBindings[key][triggerType].push_back(callback);
}

void InputManager::addTriggerBinding(EMouseButton mouseButton, EInputTrigger triggerType, TriggerBinding callback) {
	mouseTriggerBindings[mouseButton][triggerType].push_back(callback);
}

void InputManager::addTriggerBinding(std::vector<unsigned char> keys, EInputTrigger triggerType, TriggerBinding callback) {
	for (auto key : keys) {
		addTriggerBinding(key, triggerType, callback);
	}
}

void InputManager::addValueBinding(EMouseAxis mouseAxis, ValueBinding callback) {
	mouseAxisBindings[mouseAxis].push_back(callback);
}


void InputManager::adddEventBinding(Entity* target, EInputEvent inputEvent, TriggerBinding callback) {
	entityEventBindings[target][inputEvent].push_back(callback);
}

void InputManager::processTriggers(EInputTrigger triggerType, TriggerMap& triggers) {
	for (auto& callback : triggers[triggerType]) {
		callback();
	}
}

void InputManager::processEvents(EInputEvent inputEvent, EventMap& events) {
	for (auto& callback : events[inputEvent]) {
		callback();
	}
}
