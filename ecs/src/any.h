#pragma once
class Any {
  template<typename T>
  static void deleter_method(void *value_ptr)
  {
    if (value_ptr != nullptr) {
      delete (T*)value_ptr;
    }
  }

public:
  Any() = default;
  template<typename T>
  Any(T &value) {
    data = new T(value);
    deleter = &deleter_method<T>;
    dataSize = sizeof(T);
  }

  Any(const Any& other) {
    data = malloc(dataSize);
    deleter = other.deleter;
    memcpy(data, other.data, dataSize);
  }

  //Any(Any&& other)
    //: data(std::move(other.data)
    //, deleter(std::move(other.deleter)
    //, dataSize(std::move(other.dataSize)
  //{}

  ~Any() {
    deleter(data);
  }

  template<typename T>
  T &get() {
    return *((T*)data);
  }

private:
  void* data = nullptr;
  void (* deleter)(void* value) = nullptr;
  size_t dataSize = 0;
};

