set(PROJECT_NAME ayfstarterlibrs)

include(${JVX_CMAKE_DIR}/common_local_lib_head_rust.cmake)

# Generate the C header bindings for the common part
add_custom_target(
  cargo_cbindgen__${PROJECT_NAME}__common
  BYPRODUCTS ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.h
  COMMAND cbindgen --crate ${PROJECT_NAME}_common --lang c --quiet --output ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_common.h
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../ayfstarterlib-common
  COMMENT "Generating header file for Rust Library in subfolder <ayfstarterlib-common>"
)

# Link this to our product
set(JVX_CBINDGEN_ADDITIONAL_TARGETS "cargo_cbindgen__${PROJECT_NAME}__common")

include(${JVX_CMAKE_DIR}/common_local_lib_foot_rust.cmake)

# message(FATAL_ERROR "Hier")