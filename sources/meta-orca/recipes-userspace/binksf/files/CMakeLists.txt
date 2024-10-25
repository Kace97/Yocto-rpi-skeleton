cmake_minimum_required(VERSION 3.22)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/Modules")

find_package(Libnl REQUIRED)
add_library(libnl INTERFACE)
target_link_libraries(libnl INTERFACE ${libnl_LIBRARIES})
target_include_directories(libnl SYSTEM INTERFACE ${LIBNL_INCLUDE_DIRS})

set(CMAKE_SDK_DIR $ENV{CMAKE_SDK_DIR})

add_executable( binksf
    main.cpp
)

find_path(KERNELSOCKETFUN_DIR kernelsocketfun/kernelsocketfun.h
    /usr/include)

target_link_libraries(binksf libnl)

target_include_directories(binksf SYSTEM PUBLIC
    ${LIBNL_INCLUDE_DIRS}
    ${KERNELSOCKETFUN_DIR}
)

# Ran into a really nasty
# install(TARGETS binksf
#         DESTINATION bin)
