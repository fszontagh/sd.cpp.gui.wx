macro(add_definition_if_enabled VAR)
    if(${VAR})
        add_definitions(-D${VAR})
    endif()
endmacro()

add_definition_if_enabled(SDGUI_AVX)
add_definition_if_enabled(SDGUI_AVX2)
add_definition_if_enabled(SDGUI_AVX512)
add_definition_if_enabled(SDGUI_CUDA)
add_definition_if_enabled(SDGUI_VULKAN)
add_definition_if_enabled(SDGUI_METAL)
add_definition_if_enabled(SDGUI_HIPBLAS)
add_definition_if_enabled(SDGUI_SERVER)
add_definition_if_enabled(SDGUI_LLAMA_DISABLE)
add_definition_if_enabled(SDGUI_LOCAL_DISABLE)
add_definition_if_enabled(SDGUI_WINDOWLESS)