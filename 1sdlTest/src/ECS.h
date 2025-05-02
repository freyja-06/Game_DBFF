#pragma once
#include <vector>
#include <memory>
#include <algorithm>
#include <bitset>
#include <array>
#include <iostream> 

class Component;
class Entity;
class Manager;

using ComponentID = std::size_t;
using Group = std::size_t;

inline ComponentID getComponentID() {
	static ComponentID lastID = 0;
	return lastID++;
}

template <typename T> inline ComponentID getComponentTypeID() noexcept {
	static ComponentID typeID = getComponentID();
	return typeID;
}

constexpr std::size_t maxComponents = 32;
constexpr std::size_t maxGroups = 32; 

using ComponentBitSet = std::bitset<maxComponents>;
using GroupBitset = std::bitset<maxGroups>;
using ComponentArray = std::array<Component*, maxComponents>;

class Component {
public:
	Entity* entity = nullptr;

	virtual void init() {}
	virtual void update(float dt) {}
	virtual void draw() {}
	virtual ~Component() {}
};

class Entity {
private:
	Manager& manager;
	bool active = true;
	std::vector<std::unique_ptr<Component>> components;

	ComponentArray componentArray{};
	ComponentBitSet componentBitSet{};

public:
	Entity(Manager& mManager) : manager(mManager) {}

	void update(float dt) {
		if (!active) return;
		for (auto& c : components) c->update(dt);
	}

	void draw() {
		if (!active) return;
		for (auto& c : components) c->draw();
	}

	bool isActive() const { return active; }
	void destroy() { active = false; }

	void setActive(bool isActive) {
		active = isActive;
	}

	template <typename T> bool hasComponent() const {
		return componentBitSet[getComponentTypeID<T>()];
	}

	template <typename T, typename... TArgs>
	T& addComponent(TArgs&&... mArgs) {
		if (hasComponent<T>()) {
			std::cerr << "Warning: Component type already exists on entity." << std::endl;
			return getComponent<T>();
		}

		T* c = new T(std::forward<TArgs>(mArgs)...);
		c->entity = this;
		std::unique_ptr<Component> uPtr{ c };
		components.emplace_back(std::move(uPtr));

		componentArray[getComponentTypeID<T>()] = c;
		componentBitSet[getComponentTypeID<T>()] = true;

		c->init();
		return *c;
	}

	template<typename T> T& getComponent() const {
		auto ptr = componentArray[getComponentTypeID<T>()];
		if (!ptr) {
			throw std::runtime_error("Entity does not have component");
		}
		return *static_cast<T*>(ptr);
	}

};

class Manager {
private:
	std::vector<std::unique_ptr<Entity>> entities;
public:
	void update(float dt) {
		for (auto& e : entities) e->update(dt);
	}

	void draw() {
		for (auto& e : entities) e->draw();
	}

	void refresh() {
		entities.erase(
			std::remove_if(std::begin(entities), std::end(entities),
				[](const std::unique_ptr<Entity>& mEntity) {
					return !mEntity->isActive();
				}),
			std::end(entities));
	}

	Entity& addEntity() {
		Entity* e = new Entity(*this);
		std::unique_ptr<Entity> uPtr{ e };
		entities.emplace_back(std::move(uPtr));
		return *e;
	}
	Entity& getEntity(std::size_t index) {
		if (index < entities.size()) {
			return *entities[index];
		}
		throw std::out_of_range("Index out of range");
	}

	const std::vector<std::unique_ptr<Entity>>& getEntities() const {
		return entities;
	}
};