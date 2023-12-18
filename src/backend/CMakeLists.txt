file(GLOB SOURCES *.c)
file(GLOB HEADERS *.h)

add_library(backend ${SOURCES} ${HEADERS})