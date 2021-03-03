file(GLOB LIB_SOURCES *.c)
file(GLOB LIB_HEADERS *.h)

add_executable(slice main.c)

add_library(libs ${LIB_SOURCES} ${LIB_HEADERS})
target_link_libraries(slice libs m)