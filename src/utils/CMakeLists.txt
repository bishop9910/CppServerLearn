set(SOURCES 
  logger.cpp
)

add_library(BS_UTILS STATIC ${SOURCES})

target_include_directories(BS_UTILS PUBLIC ${PROJECT_SOURCE_DIR}/include)