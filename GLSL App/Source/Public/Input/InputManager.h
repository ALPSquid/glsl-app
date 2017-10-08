#pragma once
#include <map>
#include <vector>
#include "glm/glm.hpp"
#include <functional>
#include <memory>
#include "Entities/Entity.h"

/** Manages input bindings. 

Two bindings exist, a trigger binding, which is called when the specified button is pressed or released,
and an axis binding which is called with the axis value, such as mouse movement delta.
*/
class InputManager {

public:
	// Function type for a binding that takes an input value.
	typedef std::function<void(float)> ValueBinding;
	// Binding that is triggered on input.
	typedef std::function<void()> TriggerBinding;

	// Trigger types.
	enum EInputTrigger {
		INPUT_RELEASED, INPUT_PRESSED
	};

	// Bindable mouse buttons.
	enum EMouseButton {
		MOUSE_LEFT, MOUSE_RIGHT, MOUSE_MIDDLE
	};

	// Bindable mouse axes.
	enum EMouseAxis {
		MOUSE_X, MOUSE_Y
	};

	// Entity specific events, such as mouse over.
	enum EInputEvent {
		MOUSE_OVER, MOUSE_OUT
	};

	float mouseSensitivity = 0.1f;

protected:
	typedef std::map<EInputTrigger, std::vector<TriggerBinding>> TriggerMap;
	typedef std::map<EInputEvent, std::vector<TriggerBinding>> EventMap;

	// Stores trigger bindings for a key.
	std::map<unsigned char, TriggerMap> triggerBindings;
	std::map<EMouseButton, TriggerMap> mouseTriggerBindings;
	// Stores mouse value bindings.
	std::map<EMouseAxis, std::vector<ValueBinding>> mouseAxisBindings;

	// Event bindings
	std::map<Entity*, EventMap> entityEventBindings;

	glm::vec2 currentMousePos, lastMousePos;
	// Entity currently under the mouse.
	Entity* selectedEntity;

	// Shader used for rendering the selection buffer.
	GLuint selectionShader;

public:
	InputManager();

	// Call once the OpenGL context has been created.
	void init();

	// Call each frame. World is used for entity selection.
	void update(std::vector<Entity::EntityPtr> entities);

	// Glut input callbacks.
	void onMouse(int button, int state, int x, int y);
	void onMouseMoved(int x, int y);
	void onKeyDown(unsigned char key, int x, int y);
	void onKeyUp(unsigned char key, int x, int y);


	/**
	* Adds a trigger binding for a key.
	* Parameter: unsigned char key  Key to add a binding for.
	* Parameter: EInputTrigger triggerType  Input type that triggers the callback.
	* Parameter: triggerBinding callback  Callback to trigger.
	*/
	void addTriggerBinding(unsigned char key, EInputTrigger triggerType, TriggerBinding callback);
	// Adds a trigger binding for multiple keys.
	void addTriggerBinding(std::vector<unsigned char> keys, EInputTrigger triggerType, TriggerBinding callback);

	/**
	* Adds a trigger binding for a mouse button.
	* Parameter: EMouseButton mouseButton  Mouse button to add a binding for.
	* Parameter: EInputTrigger triggerType  Input type that triggers the callback.
	* Parameter: triggerBinding callback  Callback to trigger.
	*/
	void addTriggerBinding(EMouseButton mouseButton, EInputTrigger triggerType, TriggerBinding callback);

	/**
	* Adds a value binding for a mouse axis.
	* Parameter: EMouseAxis mouseAxis  Axis to add a binding for.
	* Parameter: valueBinding callback  Callback to trigger, passing the axis value.
	*/
	void addValueBinding(EMouseAxis mouseAxis, ValueBinding callback);

	void adddEventBinding(Entity* target, EInputEvent inputEvent, TriggerBinding callback);

protected:
	// Fires callbacks for a specified trigger.
	void processTriggers(EInputTrigger triggerType, TriggerMap& triggers);
	// Fires callbacks for a specific event on a specific entity.
	void processEvents(EInputEvent inputEvent, EventMap& events);
};

