cmake_minimum_required (VERSION 3.13)


project (ECS)
set( CMAKE_CXX_STANDARD 11 )


add_executable(ecs
  src/ecs.cpp
  src/main.cpp
  src/string_hash.cpp
  )

target_link_options( ecs PRIVATE "LINKER:-v" )
