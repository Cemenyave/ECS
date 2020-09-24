#pragma once
#include "string_hash.h"
#include <limits>

namespace ecs {
  using component_id_t = string_hash_t;
  const component_id_t INVALID_COMPONENT_ID = std::numeric_limits<component_id_t>::max();
}
