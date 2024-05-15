cmake_minimum_required(VERSION 3.15)
project(runner LANGUAGES CXX)

set(AYF_ATTACH_CONSOLE FALSE)
set(AYF_EFFICIENCY_PATCH TRUE)
set(AYF_COMPILE_DEFINITIONS "NOMINMAX")

add_executable(${BINARY_NAME} WIN32
  "flutter_window.cpp"
  "main.cpp"
  "utils.cpp"
  "win32_window.cpp"
  "${FLUTTER_MANAGED_DIR}/generated_plugin_registrant.cc"
  "Runner.rc"
  "runner.exe.manifest"
)
apply_standard_settings(${BINARY_NAME})

# Here patch the original flutter generated code
if(AYF_ATTACH_CONSOLE)
	set(AYF_COMPILE_DEFINITIONS "${AYF_COMPILE_DEFINITIONS};AYF_FORCE_CONSOLE")
endif()
if(AYF_EFFICIENCY_PATCH)
	set(AYF_COMPILE_DEFINITIONS "${AYF_COMPILE_DEFINITIONS};AYF_APPLY_EFFICIENCY_FIX")
endif()

target_compile_definitions(${BINARY_NAME} PRIVATE ${AYF_COMPILE_DEFINITIONS})
target_link_libraries(${BINARY_NAME} PRIVATE flutter flutter_wrapper_app)
target_include_directories(${BINARY_NAME} PRIVATE "${CMAKE_SOURCE_DIR}")
add_dependencies(${BINARY_NAME} flutter_assemble)
