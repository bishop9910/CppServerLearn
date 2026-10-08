add_subdirectory(config)
add_subdirectory(core)
add_subdirectory(utils)

set(SOURCES 
  main.cpp
)

add_executable(CppServerLearn 
  ${SOURCES}
)

target_include_directories(CppServerLearn PUBLIC ${PROJECT_SOURCE_DIR}/include)

target_link_libraries(CppServerLearn PRIVATE 
  BS_CONFIG 
  BS_CORE
  BS_UTILS
)