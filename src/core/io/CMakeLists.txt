file(GLOB SOURCES *.c)
file(GLOB HEADERS *.h)

add_library(io ${SOURCES} ${HEADERS})