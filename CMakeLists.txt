cmake_minimum_required(VERSION 3.10)

# clangd uses compile_commands.json to understand include paths
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ensures all executables land in build/bin
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

project(tcp-json-server LANGUAGES C)

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
)

target_link_libraries(server PRIVATE Threads::Threads)

                              # ===Client====
add_executable(client
  source/client/client.c
  source/client/client_utils.c
  source/utils/common_utils.c
)

target_include_directories(client PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include/client
  ${CMAKE_CURRENT_SOURCE_DIR}/include/utils
)
