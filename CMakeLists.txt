cmake_minimum_required(VERSION 3.8)
project(ros2_genicam)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()





set(GENICAM_INSTALL_LIBDIRS lib/ganicam)
set(GENICAM_INSTALL_INCLUDEDIRS include/ganicam)

add_subdirectory(genicam)
find_package(Genicam REQUIRED NO_CMAKE_PATH PATHS ${CMAKE_CURRENT_SOURCE_DIR}/genicam CONFIG NO_DEFAULT_PATH)
add_library(${PROJECT_NAMESPACE}::genicam ALIAS genicam)
include_directories(${Genicam_INCLUDES})


include_directories(
  include
)

set(build_dependencies
  Genicam
)

# find dependencies
find_package(ament_cmake REQUIRED)

add_executable(main 
  src/main.cpp)
ament_target_dependencies(main ${build_dependencies})
# target_include_directories(main PRIVATE 
#   include
#   ${GENICAM_INSTALL_INCLUDEDIRS}
# )
# amet_include_directories(main ${GENICAM_INCLUDE_DIRS})




install(TARGETS
  main
  DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY include/ DESTINATION include)
install(DIRECTORY genicam/library/CPP/include/ DESTINATION include/ganicam)

# install(DIRECTORY cmake DESTINATION share/${PROJECT_NAME})
install(DIRECTORY config DESTINATION share/${PROJECT_NAME})
install(DIRECTORY launch DESTINATION share/${PROJECT_NAME})

ament_package()
