#pragma once

#include <stddef.h>
#include <stdint.h>
#include <vector>

#include "simpleString.h"
#include "ecs_types.h"

namespace ecs {
struct Storage {
  using storage_index_t = int32_t;
  template<size_t chunk_size>
  struct Chunk {
    uint8_t* data = nullptr;
    uint8_t* tail = nullptr;
    static constexpr size_t size = chunk_size;

    Chunk() {
      data = new uint8_t[chunk_size];
      tail = data;
    }
    Chunk(Chunk&& other) {
      data = other.data;
      tail = other.tail;
      other.data = nullptr;
      other.tail = nullptr;
    }
    ~Chunk() {
      if (data != nullptr) {
        delete[] data;
      }
    }

    template<size_t item_size>
    bool fits() {
      return tail - data + item_size <= size;
    }
  };

  struct FreeSpace {
    size_t chunkNumber;
    size_t offset;
    storage_index_t id;
  };

  using storage_chunk_t = Chunk<128>;
  std::vector<storage_chunk_t> chunks;
  std::vector<FreeSpace> fs;
  size_t type_size = 0;
  storage_index_t count = 0;

  SimpleString templateName;
  SimpleString componentName;
  component_id_t componentId;

  storage_index_t get_next_free_index()
  {
    if (!fs.empty()) {
      return fs.back().id;
    } else {
      return count;
    }
  }

  template<typename T>
  void add(T &value) {
    // If there is a free slot somwhere in the middle, then put new item there
    if (!fs.empty()) {
      FreeSpace& fsItem = fs.back();
      *((T*)(chunks[fsItem.chunkNumber].data + fsItem.offset)) = value;
      fs.pop_back();
      return;
    }
    if (chunks.empty() || !chunks.back().fits<sizeof(T)>())
      chunks.emplace_back(storage_chunk_t());
    *(T*)chunks.back().tail = value;
    chunks.back().tail += sizeof(value);
    ++count;

    return;
  }

  template<typename T>
  void remove(size_t id) {
    constexpr uint32_t itemsPerChunk = storage_chunk_t::size / sizeof(T);
    size_t offset = id % itemsPerChunk;

    // We are not actualy delete anything yet, just mark as a free space in storage
    // because all we keep in this storages is POD structures or atomic data
    fs.push_back(FreeSpace());
    fs.back().chunkNumber = id / itemsPerChunk;
    fs.back().offset = offset;
    fs.back().id = (storage_index_t)id;
  }
};

}
