# Needed for chlorine to work
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_GNU_SOURCE -DCHLORINE_NO_COLORS")

set(TESTS
    basic.c
)

foreach(file ${TESTS})
    get_filename_component(test ${file} NAME_WE)
    string(CONCAT test ${test} "_test")
    add_executable(${test} ${file} "chlorine/chlorine.h")
    # For chlorine.h
    target_include_directories(${test} PRIVATE "chlorine")
    target_link_libraries(${test} rts pthread)
endforeach(file)
