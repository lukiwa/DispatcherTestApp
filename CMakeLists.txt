cmake_minimum_required(VERSION 3.3)
project(DispatcherTestApp)

find_package(WPEFramework REQUIRED)
find_package(${NAMESPACE}Plugins REQUIRED)
find_package(${NAMESPACE}Definitions REQUIRED)
find_package(CompileSettingsDebug CONFIG REQUIRED)

add_executable(${PROJECT_NAME}
    main.cpp
    Module.cpp
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED YES)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        CompileSettingsDebug::CompileSettingsDebug
        ${NAMESPACE}Plugins::${NAMESPACE}Plugins
        ${NAMESPACE}Definitions::${NAMESPACE}Definitions)

include_directories(/usr/include/WPEFramework)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)