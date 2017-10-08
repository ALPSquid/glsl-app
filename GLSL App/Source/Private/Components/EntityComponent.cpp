#include "../stdafx.h"
#include "Components/EntityComponent.h"
#include "Entities/Entity.h"


EntityComponent::EntityComponent(Entity* owner) {
	this->owner = owner;
}

EntityComponent::~EntityComponent() {
	std::cout << "Component destroyed" << std::endl;
}

void EntityComponent::beginPlay() {
	hasBegunPlay = true;
}

void EntityComponent::update(float deltaTime) {
	if (!hasBegunPlay) beginPlay();
}
