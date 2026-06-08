cmake_minimum_required(VERSION 4.0)
project(reader)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE SOURCES "src/*.c" "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.h" "include/*.hpp")

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(
  reader 
  ${SOURCES} 
  ${HEADERS}
)

find_library(ZBAR_LIBRARY zbar)
# find_package(ZXing REQUIRED)

# target_link_libraries(reader ${OpenCV_LIBS} ${ZBAR_LIBRARY} ZXing::ZXing)
target_link_libraries(reader ${OpenCV_LIBS} ${ZBAR_LIBRARY})
target_include_directories(reader PRIVATE src)
