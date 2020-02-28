#include "ecs.h"
#include "string_hash.h"


#include <iostream>


///////////////////////////////////////////////////////////////////////////////
////Preforemance test
//
#ifdef WIN32
#include <Windows.h>


inline long long PerformanceCounter() noexcept {
  LARGE_INTEGER li;
  ::QueryPerformanceCounter(&li);
  return li.QuadPart;
}


inline long long PerformanceFrequency() noexcept {
  LARGE_INTEGER li;    
  ::QueryPerformanceFrequency(&li);    
  return li.QuadPart;
}
#else
inline long long PerformanceCounter() noexcept { return 0; }
inline long long PerformanceFrequency() noexcept { return 0; }

#endif


///////////////////////////////////////////////////////////////////////////////

const unsigned int test_size = 1000000;
int values[test_size];


static inline int test_without_cast(const int *arr, int size)
{
  int res = 0;
  for (int i = 0; i < size; ++i)
    res += arr[i];
  return res;
}


static inline int test_with_cast(const void *arr, int size)
{
  int res = 0;
  const int * casted = (const int *)arr;
  for (int i = 0; i < size; ++i)
    res += *((const int*)((const char *)arr + i * sizeof(int)));
    //res += casted[i];
  return res;
}


void run_test()
{
  for (int i = 0; i < test_size; ++i)
  {
    values[i] = i % 2 == 0 ? 1 : -1;
  }

  int res1 = 0, res2 = 0;
  long long start_without_cast = PerformanceCounter();
  res1 = test_without_cast(values, (int) test_size);
  long long end_without_cast = PerformanceCounter();

  long long start_with_cast = PerformanceCounter();
  res2 = test_with_cast(values, (int) test_size);
  long long end_with_cast = PerformanceCounter();

  long long frq = PerformanceFrequency();
  std::cout << "Reading without cast: " <<
    (((end_without_cast - start_without_cast) * 1000.0) / frq) << "ms" << std::endl;


  std::cout << "Reading with cast: " <<
    (((end_with_cast - start_with_cast) * 1000.0) / frq) << "ms" << std::endl;

  std::cout << res1 << res2 << std::endl;
}


void add_three_system (int &value)
{
  value += 3;
  std::cout << "new value = " << value << std::endl;
}

void add_three_system_call (ecs::EntityManager &mgr, ecs::SystemDescription *system){
  for (ecs::Archetype& archetype : mgr.archetypes) {
    for (ecs::Storage& storage : archetype.storages) {
      if (storage.componentId == system->components[0]) {
        for (ecs::Storage::storage_chunk_t& chunk : storage.chunks ) {
          for (int* value = (int*)chunk.data, i = 0; value < (int*)chunk.tail; ++value, ++i) {
            add_three_system(*value);
          }
        }
      }
    }
  }
}

static ecs::SystemDescription addThreeSystem {
  "add_three",
  &add_three_system_call,
  {hash_str("value")}
};


void two_arguments_system(int& value, int& second_value) {
 value += 1;
 second_value = value - 3;
}

void two_arguments_system_call(ecs::EntityManager& mgr, ecs::SystemDescription *system) {
}

static ecs::SystemDescription twoArgumentsSystem {
  "two_arguments",
  &two_arguments_system_call,
  {hash_str("value"), hash_str("second_value")}
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

  systemManager.call_systems(entityManager /*, stage*/);

  for (int i = 0; i < 100; ++i) {
    entityManager.create_entity("test_template");
    entityManager.create_entity("template_with_two_components");
  }

  systemManager.call_systems(entityManager);
}

void test_attribute_map() {
}

int main (int argc,  const char *argv[])
{
  std::cout << "main" << std::endl;
  test_attribute_map();
  test_ecs();
  return 0;
}

///////////////////////////////////////////////////////////////////////////////

/*

class Attribute {
private:
  struct Head {
    virtual ~Head() {}
    virtual void *copy() = 0;
    const type_info& type;
    Head(const type_info& type): type(type) {}
    void *data() { return this + 1; }
  };

  template <class T>
  struct THead: public Head {
    THead(): Head(typeid(T)) {}
    virtual ~THead() override { ((T*)data())->~T(); }
    virtual void *copy() override {
      return new(new(malloc(sizeof(Head) + sizeof(T)))
          THead() + 1) T(*(const T*)data()); }
  };
  void *data;
  Head *head() const { return (Head*)data - 1; }
  void *copy() const { return data ? head()->copy() : nullptr; }

public:
  Attribute() : data(nullptr) {}
  Attribute(const Attribute& src) : data(src.copy()) {}
  Attribute(Attribute&& src): data(src.data) { src.data = nullptr; }
  template <class T>
  Attribute(const T& src) :
    data(
      new(
        new(malloc(sizeof(Head) + sizeof(T))) THead<T>() + 1
      ) T(src)
    )
  {}
  ~Attribute() {
    if(!data) return;
    Head* head = this->head();
    head->~Head(); free(head); }
  bool empty() const { return data == nullptr; }

  const type_info& type() const {
    assert(data);
    return ((Head*)data - 1)->type;
  }

  template <class T>
  T& value() {
      if (!data || type() != typeid(T))
        throw bad_cast();
      return *(T*)data;
  }

  template <class T>
  const T& value() const {
      if (!data || type() != typeid(T))
        throw bad_cast();
      return *(T*)data;
  }

  template <class T>
  void setValue(const T& it) {
    if(!data)
      data = new(new(malloc(sizeof(Head) + sizeof(T))) THead<T>() + 1) T(it);
    else {
      if (type() != typeid(T))
        throw bad_cast();
      *(T*)data = it;
    }
  }

public:
  static void test_me() {
    vector<Attribute> list;
    list.push_back(Attribute(1));
    list.push_back(3.14);
    list.push_back(string("hello world"));
    list[1].value<double>() = 3.141592;
    list.push_back(Attribute());
    list[3].setValue(1.23f);
    for (auto& a : list) {
      cout << "type = " << a.type().name()
        << " value = ";
      if(a.type() == typeid(int)) cout << a.value<int>();
      else if (a.type() == typeid(double)) cout << a.value<double>();
      else if (a.type() == typeid(string)) cout << a.value<string>();
      else if (a.type() == typeid(float))  cout << a.value<float>();
      cout << endl;
    }
  }
};
*/
