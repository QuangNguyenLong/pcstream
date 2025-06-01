cmake_minimum_required(VERSION 3.21)

project(pcstreamExamples C)

include(../cmake/folders.cmake)

if(PROJECT_IS_TOP_LEVEL)
  find_package(pcstream REQUIRED)
endif()

add_custom_target(run-examples)

function(add_example NAME)
  add_executable("${NAME}" "${NAME}.c")
  target_link_libraries("${NAME}" PRIVATE pcstream::pcstream)
  target_compile_features("${NAME}" PRIVATE c_std_17)
  add_custom_target("run_${NAME}" COMMAND "${NAME}" VERBATIM)
  add_dependencies("run_${NAME}" "${NAME}")
  add_dependencies(run-examples "run_${NAME}")
endfunction()

add_example(hull)
# add_example(tmc2_decode)

if(EMSCRIPTEN)
    add_example(pcs_wasm)
    set_target_properties(pcs_wasm PROPERTIES
        OUTPUT_NAME "pcs_wasm"
        SUFFIX ".js"
        )
    target_compile_options(pcs_wasm PRIVATE
    )

    target_link_options(pcs_wasm PRIVATE
        "-sMODULARIZE=1"
        "-sEXPORT_ES6=1"
        "-sEXPORT_NAME=pcs_wasm_init"
        "-sEXPORTED_RUNTIME_METHODS=['ccall','cwrap']"
        "-sALLOW_MEMORY_GROWTH=1"
    )

endif()

add_folders(Example)
