set(SOURCES 
  config.cpp
)

add_library(BS_CONFIG STATIC ${SOURCES})

target_include_directories(BS_CONFIG PUBLIC ${PROJECT_SOURCE_DIR}/include)

find_package(yaml-cpp CONFIG REQUIRED)

target_link_libraries(BS_CONFIG PUBLIC yaml-cpp::yaml-cpp) # The library or executable that require yaml-cpp library
