#pragma once

#include <climits>
#include "string_hash.h"

namespace ecs {
  using component_id_t = string_hash_t;
  const component_id_t INVALID_COMPONENT_ID = UINT_MAX;
}
