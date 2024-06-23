cmake_minimum_required(VERSION 3.8)
project(ros2_genicam)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()


include_directories(
  include
)

# find dependencies
find_package(ament_cmake REQUIRED)


set(GENICAM_INSTALL_LIBDIRS lib/ganicam)
set(GENICAM_INSTALL_INCLUDEDIRS include/ganicam)

add_subdirectory(genicam)
find_package(Genicam REQUIRED NO_CMAKE_PATH PATHS ${CMAKE_CURRENT_SOURCE_DIR}/genicam CONFIG NO_DEFAULT_PATH)
add_library(${PROJECT_NAMESPACE}::genicam ALIAS genicam)
include_directories(${Genicam_INCLUDES})

set(build_dependencies
  Genicam
)

add_executable(main 
  src/main.cpp)
ament_target_dependencies(main ${build_dependencies})


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
