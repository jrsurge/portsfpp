cmake_minimum_required(VERSION 3.1.1)

include_directories(
    include
    src/portsf/include
)

set(portsf_srcs
    src/portsf/include/portsf.h
    src/portsf/portsf.c
    src/portsf/ieee80.h
    src/portsf/ieee80.c
)

set(portsfpp_srcs
    include/portsf.hpp
    src/portsf.cpp
)

add_library(portsfpp ${portsf_srcs} ${portsfpp_srcs})