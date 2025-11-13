cmake_minimum_required(VERSION 3.20)
project(PCOptimizer VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/W4 /WX- /permissive-)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
    add_compile_definitions(NOMINMAX)
endif()

find_package(imgui CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

set(MONITORING_SOURCES
    src/monitoring/monitoring_engine.cpp
)

set(MONITORING_HEADERS
    src/monitoring/monitoring_engine.h
)

set(AI_SOURCES
    src/ai/ai_analyzer.cpp
)

set(AI_HEADERS
    src/ai/ai_analyzer.h
)

set(OPTIMIZER_SOURCES
    src/optimizers/thread_optimizer.cpp
    src/optimizers/timer_optimizer.cpp
    src/optimizers/power_optimizer.cpp
    src/optimizers/profile_manager.cpp
    src/optimizers/interrupt_optimizer.cpp
    src/optimizers/memory_optimizer.cpp
    src/optimizers/quantum_tweaker.cpp
    src/optimizers/network_optimizer.cpp
)

set(OPTIMIZER_HEADERS
    src/optimizers/thread_optimizer.h
    src/optimizers/timer_optimizer.h
    src/optimizers/power_optimizer.h
    src/optimizers/profile_manager.h
    src/optimizers/interrupt_optimizer.h
    src/optimizers/memory_optimizer.h
    src/optimizers/quantum_tweaker.h
    src/optimizers/network_optimizer.h
)

add_executable(PCOptimizer
    src/main_new.cpp
    ${MONITORING_SOURCES}
    ${MONITORING_HEADERS}
    ${AI_SOURCES}
    ${AI_HEADERS}
    ${OPTIMIZER_SOURCES}
    ${OPTIMIZER_HEADERS}
)

target_include_directories(PCOptimizer PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(PCOptimizer PRIVATE
    imgui::imgui
    nlohmann_json::nlohmann_json
    spdlog::spdlog
    d3d11.lib
    pdh.lib
    PowrProf.lib
)

if(WIN32)
    set_target_properties(PCOptimizer PROPERTIES
        WIN32_EXECUTABLE FALSE
    )
endif()

install(TARGETS PCOptimizer
    RUNTIME DESTINATION bin
)
