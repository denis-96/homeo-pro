qt_add_library(Common STATIC
    HomeoEntity.h HomeoEntity.cpp
)

target_link_libraries(Common PRIVATE Qt6::Widgets)
target_include_directories(Common INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
