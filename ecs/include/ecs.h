#pragma once

#include "ecs_types.h"
#include "storage.h"
#include "string_hash.h"

#include <vector>
#include <map>

/**
 * TODO:
 * Consider Lua as language for code generator
 * https://github.com/mkottman/luaclang-parser
 */

/**
 * Entity manager keep's tracka of entities lifetime?
 */

namespace ecs {

struct TemplateComponent;

struct Archetype {
  std::vector<Storage> storages;

  // Sorted array of all used indexes to avoid addresing to "empty" slots
  std::vector<uint32_t> indexes;
  std::map<component_id_t, size_t> componentIdToStorage;
};

enum class QueryMode {
  Presence,
  Absence,
  ValueEq,
  ValueNotEq,
  ValueGreater,
  ValueLess,
  ValueGraeterOrEq,
  ValueLessOrEq,
};

struct Condition;

using Query = std::vector<Condition>;
using Functor = bool(*)(const Storage& s, size_t index);

struct Condition {
  Condition() = default;
  Condition(const component_id_t& _componentId, const QueryMode& _mode, const Functor _func = nullptr)
    : componentId(_componentId)
    , mode(_mode)
    , func(_func)
  {}

  Condition(component_id_t&& _componentId, QueryMode&& _mode = QueryMode::Presence,  Functor&& _func = nullptr)
    : componentId(std::move(_componentId))
    , mode(std::move(_mode))
    , func(std::move(_func))
  {}

  component_id_t componentId = INVALID_COMPONENT_ID;
  QueryMode mode = QueryMode::Presence;
  Functor func;
};

struct Entry {
  size_t archetype;
  size_t index;
};

using QueryResult = std::vector<Entry>;

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
  void register_template(const char *template_name, std::vector<TemplateComponent> components);

  // First iteration: perform query inside this call and return all arguments
  QueryResult perform_query(const Query& query) const;

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

  Query query;
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
struct TemplateComponent {
  const char* component_name;
  int initial_value;
};

}
