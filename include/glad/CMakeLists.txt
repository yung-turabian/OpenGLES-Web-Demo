add_library(glad STATIC
    glad.c
)

# Explicitly set the language for the target
set_target_properties(glad PROPERTIES LINKER_LANGUAGE C)

# Set IPO if supported
if(ipo_supported)
    set_target_properties(glad PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

target_include_directories(glad PUBLIC include)

