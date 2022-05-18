project(jmi)

add_library(JmiLib STATIC)
add_library(3rd::jmi ALIAS JmiLib)

target_sources(JmiLib PRIVATE
    JMI/jmi.cpp
    JMI/jmi.h
    jmi_macro.h
)

target_include_directories(JmiLib INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/JMI
)
