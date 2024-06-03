qt_add_library(repertorizator_lib STATIC
    repertorizator.cpp repertorizator.h
    repmodel.cpp repmodel.h
    repview.cpp repview.h
    rubric.cpp rubric.h
    repproxymodel.h repproxymodel.cpp
)
target_link_libraries(repertorizator_lib PRIVATE Qt6::Widgets)
target_include_directories(repertorizator_lib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

qt_add_executable(repertorizator
    main.cpp
    ../resources/resource.rc
)

target_link_libraries(repertorizator PRIVATE repertorizator_lib Qt6::Widgets)

set_target_properties(repertorizator PROPERTIES
    WIN32_EXECUTABLE ON
)


qt_add_resources(repertorizator_lib iconresources
    PREFIX "/"
    FILES
        ../resources/icons/main-icon.svg
        ../resources/icons/new-file.svg
        ../resources/icons/open-file.svg
        ../resources/icons/save-file.svg
        ../resources/icons/exit.svg
        ../resources/icons/remove.svg
        ../resources/icons/add.svg
)
