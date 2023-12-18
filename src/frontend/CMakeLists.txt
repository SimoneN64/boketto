file(GLOB SOURCES *.c)
file(GLOB HEADERS *.h)

add_library(frontend ${SOURCES} ${HEADERS})