#include <ecs.h>

void add_three_system (int &value)
{
  value += 3;
  //std::cout << "new value = " << value << std::endl;
}

void add_three_system_call (ecs::EntityManager &mgr, ecs::SystemDescription *system){
  ecs::QueryResult queryResult = mgr.perform_query( system->query );
  for ( const auto& entry : queryResult) {
    ecs::Archetype& archetype = mgr.archetypes[entry.archetype];
    add_three_system(
      *archetype.storages[archetype.componentIdToStorage[hash_str("value")]].get<int>(entry.index)
    );
  }
}

static bool filter_func(const ecs::Storage& storage, size_t index) {
  return *storage.get<int>(index) > 100;
}

static ecs::SystemDescription addThreeSystem {
  "add_three",
  &add_three_system_call,
  {{hash_str("value"), ecs::QueryMode::Presence, &filter_func}}
};

void two_arguments_system(int& value, int& second_value) {
 value += 1;
 second_value = value - 3;
}

void two_arguments_system_call(ecs::EntityManager& mgr, ecs::SystemDescription *system) {
  ecs::QueryResult queryResult = mgr.perform_query( system->query );
  for ( const auto& entry : queryResult) {
    ecs::Archetype& archetype = mgr.archetypes[entry.archetype];
    two_arguments_system(
      *archetype.storages[archetype.componentIdToStorage[hash_str("value")]].get<int>(entry.index),
      *archetype.storages[archetype.componentIdToStorage[hash_str("second_value")]].get<int>(entry.index)
    );
  }
}

static ecs::SystemDescription twoArgumentsSystem {
  "two_arguments",
  &two_arguments_system_call,
  {
    {hash_str("value") },
    {hash_str("second_value")},
  }
};

void print_system( int value, int second_value ) {
  std::cout << "value = " << value << " second_value = " << second_value << std::endl;
}

void print_system_system_call(ecs::EntityManager& mgr, ecs::SystemDescription *system) {
  ecs::QueryResult queryResult = mgr.perform_query( system->query );
  for ( const auto& entry : queryResult) {
    ecs::Archetype& archetype = mgr.archetypes[entry.archetype];
    print_system(
      *archetype.storages[archetype.componentIdToStorage[hash_str("value")]].get<int>(entry.index),
      *archetype.storages[archetype.componentIdToStorage[hash_str("second_value")]].get<int>(entry.index)
    );
  }
}

static ecs::SystemDescription printSystem {
  "print_system",
  &print_system_system_call,
  {
    {hash_str("value")},
    {hash_str("second_value")},
  }
};

void test_ecs() {
  ecs::EntityManager entityManager;

  //all components
  entityManager.register_component<int>("value");
  entityManager.register_component<int>("second_value");

  //test template
  std::vector<ecs::TemplateComponent> testTemplateComponents;
  testTemplateComponents.push_back({ "value", 5 });
  entityManager.register_template("test_template", testTemplateComponents);

  //tempalte with two components
  std::vector<ecs::TemplateComponent> templateWithTwoComponentsComponents;
  templateWithTwoComponentsComponents.push_back({"value", 0});
  templateWithTwoComponentsComponents.push_back({"second_value", 100});
  entityManager.register_template("template_with_two_components", templateWithTwoComponentsComponents);

  ecs::SystemManager systemManager;
  systemManager.add_system(&addThreeSystem);
  systemManager.add_system(&twoArgumentsSystem);
  systemManager.add_system(&printSystem);

  systemManager.call_systems(entityManager /*, stage*/);
  for (int i = 0; i < 100; ++i) {
    entityManager.create_entity("test_template");
    entityManager.create_entity("template_with_two_components");
  }

  systemManager.call_systems(entityManager);
}


int main(int argc,  const char *argv[])
{
  std::cout << "main" << std::endl;
  test_ecs();
  return 0;
}
