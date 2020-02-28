#pragma once

#include "ecs_types.h"
#include "storage.h"
#include "string_hash.h"

#define ECS_API
/**
 * Entity manager keep's tracka of entities lifetime?
 */

namespace ecs {

struct ECS_API TemplateComponent;

struct Archetype {
  std::vector<Storage> storages;
};

struct EntityManager
{
  using entity_id_t = int32_t;
  const entity_id_t INVALID_ENTITY_ID = INT_MAX;

  using template_instance_id_t = int32_t;
  friend class SystemManager;

private:
  struct Entity {
    entity_id_t id;
    SimpleString templateName;
    template_instance_id_t instanceId;
    bool instantiated;
  };

  struct ComponentDescription {
    ComponentDescription();
    ComponentDescription(const char *n, uint32_t size);

    SimpleString name;
    component_id_t hashed_name = 0;
    uint32_t componentSize = 0;
  };

  struct Template {
    SimpleString name;
    std::vector<component_id_t> components;
    std::vector<int> initial_values;
    Template() = default;
    Template(const char *n) : name(n) {}
    int archetypeId = -1;

    void add_component(ComponentDescription &c_desc, int initial_value) {
      components.push_back(c_desc.hashed_name);
      initial_values.push_back(initial_value);
    }

    int* get_component_initial_value( component_id_t componentId) {
      for (int i = 0; i < components.size(); ++i) {
        if ( componentId == components[i] ) {
          return &initial_values[i];
        }
      }
      assert( !"Attempt to get initial value for component not existing in template" );
      return nullptr;
    }
  };

  public:
  ~EntityManager();
  entity_id_t create_entity(const char *templateName);
  void destroy_entity(entity_id_t entityId);
  void register_template( const char *template_name, std::vector<TemplateComponent> components);

  template<typename T>
  void register_component(const char *component_name) {
    descriptions.push_back({component_name, sizeof(T)});
  }

private:
  ComponentDescription get_component_description(const char* component_name);
  Template* get_template(SimpleString name);

  static entity_id_t lastUsedId;

public:
  std::vector<Entity> entities;
  std::vector<Template> templates;
  std::vector<ComponentDescription> descriptions;
  std::vector<Archetype> archetypes;
};

struct SystemDescription {
  const char *name;

  // pointer to system function
  void (*system)(EntityManager &entityManager, SystemDescription* system);

  const std::vector<component_id_t> components;
};

struct SystemManager
{
  void add_system(SystemDescription *sys_desc);
  void set_systems_order(const char* sys_names[], size_t count);
  void call_systems(EntityManager &entitytManager /*, stage*/);

  std::vector<SystemDescription*> systems;
};


// Tempalte component is an intermidiate API structure that should be created
// externally. It's main purpose is to hide all template complexity from user.
// All user needs to know about template is that it's a list of components
// with initial values.
// TODO: replace inital_value type with Any class to be able to store
// all component's initial values in one array
struct ECS_API TemplateComponent {
  const char* component_name;
  int initial_value;
};

}
