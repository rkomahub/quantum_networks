cmake_minimum_required(VERSION 3.10)
project(QuantumNetworkSimulation)

# C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add all your source files here
set(SOURCES
    main.cpp
    node.cpp
    link.cpp
    triangle.cpp
    network.cpp
    growth_engine.cpp
    metrics.cpp
)

add_executable(quantum_net ${SOURCES})