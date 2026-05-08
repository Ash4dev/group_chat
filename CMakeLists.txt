# CMAKE commands (but not variables) are case-insensitive
set(CMAKE_POLICY_VERSION_MINIMUM 3.5) # cJSON has this limit
cmake_minimum_required(VERSION 3.10)

# clangd uses compile_commands.json to understand include paths
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ensures all executables land in build/bin
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

project(tcp-json-server LANGUAGES C)

# load CMAKE code from a file/module
include(FetchContent)

# FetchContent alternative to ExternalProject_Add
  # FetchContent make's project targets available right away
  # Unlike ExternalProject_Add which adds during build phase

# declaration
# HTTPS: DaveGamble/cJSON/commit/c859b25da02955fef659d658b8f324b5cde87be3
fetchcontent_declare(
  cjson
  GIT_REPOSITORY https://github.com/DaveGamble/cJSON.git
  GIT_TAG c859b25da02955fef659d658b8f324b5cde87be3 # release 1.7.19
)

# Optional: Disable tests to speed up your build
set(ENABLE_CJSON_TEST OFF CACHE BOOL "" FORCE)

# definition
fetchcontent_makeavailable(cjson)
# CMAKE creates a <name>_SOURCE_DIR pointing to the download location

# INTERFACE_INCLUDE_DIRECTORIES : like a business card for the library
  # Available in newer CMAKE projects
  # cJSON is old: had to add cjson_SOURCE_DIR

                              # ===Server===
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(server
  source/server/server.c
  source/server/server_utils.c
  source/utils/common_utils.c
  source/utils/task_queue.c
)

target_include_directories(server PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include/server
  ${CMAKE_CURRENT_SOURCE_DIR}/include/utils
  ${cjson_SOURCE_DIR}
)

target_link_libraries(server PRIVATE
  Threads::Threads
  cjson
)

                              # ===Client====
add_executable(client
  source/client/client.c
  source/client/client_utils.c
  source/utils/common_utils.c
)

target_include_directories(client PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include/client
  ${CMAKE_CURRENT_SOURCE_DIR}/include/utils
  ${cjson_SOURCE_DIR}
)

target_link_libraries(client PRIVATE
  cjson
)
