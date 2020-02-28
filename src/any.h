class Any {
  template<typename T>
    void deleter_method(void *value_ptr)
    {
      if (value_ptr != nullptr) {
        delete (T*)value_ptr;
      }
    }
public:
  template<typename T>
  Any(T &value) {
    data = new T(value);
    deleter = &deleter_method<T>;
  }

  ~Any() {
    deleter(data);
  }

  template<typename T>
  T &get() {
    return *((T*)data);
  }
private:
  void* data;
  void (* deleter)(void* value);
};

