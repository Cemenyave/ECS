
set(HEADER_FILES
  include/ecs.h
  include/ecs_types.h
  include/simpleString.h
  include/storage.h
  include/string_hash.h
)

set(SOURCE_FILES
  src/ecs.cpp
  src/string_hash.cpp
)

add_library( ecs STATIC
  ${HEADER_FILES}
  ${SOURCE_FILES}
  )

add_library( ECS::ECS ALIAS ecs)

target_compile_features( ecs
  PUBLIC cxx_std_17
  )

# Defines the headers for this library.
# https://cmake.org/cmake/help/v3.11/command/target_include_directories.html
target_include_directories( ecs
        PUBLIC
            # Used when building the library:
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            # Used when installing the library:
            $<INSTALL_INTERFACE:include/${PROJECT_PREFIX}>
        PRIVATE
            # Used only when building the library:
            src)

# The INSTALL(TARGET) command specifies where to install specific targets.
# By specifying EXPORT here, we add these targets to an export group.
install(TARGETS ecs
        EXPORT ecs-targets
        RUNTIME DESTINATION ${INSTALL_RUNTIME_DIR}
        LIBRARY DESTINATION ${INSTALL_LIBRARY_DIR}
        ARCHIVE DESTINATION ${INSTALL_ARCHIVE_DIR})

# The INSTALL(FILES) command specifies where to install specific files, such as headers.
install(FILES ${HEADER_FILES}
        DESTINATION ${INSTALL_INCLUDE_DIR})


