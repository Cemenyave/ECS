
set(HEADER_FILES

)

set(SOURCE_FILES
  src/main.cpp
)

add_executable( sandbox
  ${HEADER_FILES}
  ${SOURCE_FILES}
  )


target_link_libraries( sandbox
  ecs)

