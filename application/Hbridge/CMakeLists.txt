set(SOURCES Hbridge.cpp)

idf_component_register(SRCS ${SOURCES}
                        INCLUDE_DIRS .
                        REQUIRES
                            Gpio)