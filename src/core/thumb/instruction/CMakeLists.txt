file(GLOB SOURCES *.c)
file(GLOB HEADERS *.h)

add_library(thumb-instructions ${SOURCES} ${HEADERS})