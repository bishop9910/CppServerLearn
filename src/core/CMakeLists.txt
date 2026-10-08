set(SOURCES 
  core.cpp
)

add_library(BS_CORE STATIC ${SOURCES})

target_include_directories(BS_CORE PUBLIC ${PROJECT_SOURCE_DIR}/include)

find_package(unofficial-uwebsockets CONFIG REQUIRED)

target_link_libraries(BS_CORE PRIVATE unofficial::uwebsockets::uwebsockets) # The library or executable that require yaml-cpp library
