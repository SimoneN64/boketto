file(GLOB SOURCES *.c)
file(GLOB HEADERS *.h)

add_library(arm-instructions ${SOURCES} ${HEADERS})