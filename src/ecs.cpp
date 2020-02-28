#include <cassert>
#include <vector>

#include "simpleString.h"
#include "template.h"
#include "ecs.h"
#include "any.h"
#include "string_hash.h"

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

  //fill storages with initial values
  for (Storage& storage : archetypes[tpl->archetypeId].storages) {
    entity.instanceId = storage.get_next_free_index();
    storage.add(*tpl->get_component_initial_value(storage.componentId));
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

    archetype.storages.emplace_back(Storage());
    Storage& storage = archetype.storages.back();
    storage.type_size = desc.componentSize;
    storage.templateName = templates.back().name;
    storage.componentName = desc.name;
    storage.componentId = desc.hashed_name;
  }
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
