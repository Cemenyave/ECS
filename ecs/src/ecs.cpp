#include <cassert>
#include <vector>

#include "../include/simpleString.h"
#include "template.h"
#include "../include/ecs.h"
#include "any.h"
#include "../include/string_hash.h"

#include <iostream>

namespace ecs {

EntityManager::ComponentDescription::ComponentDescription() = default;
EntityManager::ComponentDescription::ComponentDescription(const char *n, uint32_t size)
  : name(n)
  , hashed_name(hash_str(n))
  , componentSize(size)
{}

EntityManager::~EntityManager() {
  //while (!entities.empty()) {
    //destroy_entity(entities.back().id);
  //}
}

EntityManager::entity_id_t EntityManager::create_entity(const char* templateName) {
  Template* tpl = get_template(templateName);
  assert(tpl != nullptr && "attempt to create entity with invalid template name");

  Entity entity;
  entity.templateName = SimpleString(templateName);
  entity.instantiated = false;
  entity.id = entities.size();

  archetypes[tpl->archetypeId].indexes.emplace_back(0);
  uint32_t& indexRef = archetypes[tpl->archetypeId].indexes.back();

  //fill storages with initial values
  for (Storage& storage : archetypes[tpl->archetypeId].storages) {
    entity.instanceId = storage.get_next_free_index();
    indexRef = storage.add(*tpl->get_component_initial_value(storage.componentId));
  }

  entities.push_back(entity);
  return entity.id;
}

void EntityManager::destroy_entity(entity_id_t entityId)
{
  //if (entityId != INVALID_ENTITY_ID) {
  //  Archetype& archetype = entities[entityId]
  //}
}

void EntityManager::register_template(
    const char *template_name,
    std::vector<TemplateComponent> components) {
  templates.emplace_back(Template(template_name));
  Template& tpl = templates.back();
  archetypes.emplace_back(Archetype());
  Archetype& archetype = archetypes.back();
  tpl.archetypeId = archetypes.size() - 1;
  for (auto &templateComp : components) {
    ComponentDescription desc =
      get_component_description(templateComp.component_name);

    tpl.add_component(desc, templateComp.initial_value);

    archetype.componentIdToStorage[desc.hashed_name] = archetype.storages.size();
    archetype.storages.emplace_back(Storage());
    Storage& storage = archetype.storages.back();
    storage.type_size = desc.componentSize;
    storage.templateName = templates.back().name;
    storage.componentName = desc.name;
    storage.componentId = desc.hashed_name;
  }
}


static bool is_archetype_passes_conditions(const ecs::Archetype& archetype, const ecs::Query& query) {
  for (const Condition& cond : query) {
    bool compPresented = archetype.componentIdToStorage.find(cond.componentId) != archetype.componentIdToStorage.end();
    if ((cond.mode == QueryMode::Absence && compPresented) || (cond.mode != QueryMode::Absence && !compPresented)) {
      return false;
    }
  }
  return true;
}

QueryResult EntityManager::perform_query(const Query& query) const {
  QueryResult result;
  for (size_t i = 0; i < archetypes.size(); ++i) {
    const Archetype& archetype = archetypes[i];
    if (is_archetype_passes_conditions(archetype, query)) {
      for (const uint32_t index : archetype.indexes) {
        bool passed = true;
        for (const Condition& cond : query) {
          passed = true;
          passed = !cond.func || cond.func(archetype.storages[archetype.componentIdToStorage.at(cond.componentId)], index);
        }
        if (passed) {
          Entry e;
          e.archetype = i;
          e.index = index;
          result.emplace_back(e);
        }
      }
    }
  }

  return result;
}

EntityManager::ComponentDescription EntityManager::get_component_description(const char* component_name) {
  string_hash_t compId = hash_str(component_name);
  for (auto &compDesc : descriptions) {
    if (compDesc.hashed_name == compId)
      return compDesc;
  }

  // TODO: how to handle this properly? Now it's only used for template
  // creation (not template instanse creation, but template itself).
  return ComponentDescription();
}

EntityManager::Template* EntityManager::get_template(SimpleString templateName) {
  for (Template& tpl : templates) {
    if (templateName == tpl.name) { 
      return &tpl;
    }
  }

  return nullptr;
}

void SystemManager::add_system(SystemDescription *sys_desc) {
  systems.push_back(sys_desc);
}

void SystemManager::call_systems(EntityManager& entityManager /*, stage*/) {
  for ( int i = 0; i < systems.size(); ++i) {
    systems[i]->system(entityManager, systems[i]);
  }
}

}
