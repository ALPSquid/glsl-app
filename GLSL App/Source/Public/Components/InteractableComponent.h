#pragma once
#include "EntityComponent.h"

/** A component that manages callbacks for mouse over/out. */
class InteractableComponent : public EntityComponent {


protected:
	/** Whether the entity is currently being rotated. */
	bool selected;
	/** Whether the rotate button is pressed. */
	bool selectionPressed;
	/** Whether the move button is pressed. */
	bool movePressed;
	/** Whether the scale button is pressed. */
	bool scalePressed;
	/** Whether the mouse is hovering over the object. */
	bool isHovered;

public:
	InteractableComponent(Entity* owner);

	inline bool isSelected() { return selected; };

protected:
	void manipulateUp(float val);
	void manipulateRight(float val);
};

