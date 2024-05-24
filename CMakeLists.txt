cmake_minimum_required(VERSION 3.5)

project(Repertorizator VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Test)

qt_standard_project_setup()

set(PROJECT_SOURCES
    src/repertorizator.cpp src/repertorizator.h
    src/repview.cpp src/repview.h
    src/repmodel.cpp src/repmodel.h

    src/rubric.cpp src/rubric.h

    resources.qrc
)


qt_add_executable(Repertorizator
    src/main.cpp
    ${PROJECT_SOURCES}
)

target_link_libraries(Repertorizator PRIVATE
    Qt6::Widgets
)

set_target_properties(Repertorizator PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

install(TARGETS Repertorizator
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Repertorizator)
endif()


# # Unit Tests


# qt_add_executable(TestRepertorizator
#     tests/test_repertorizator.cpp
#     src/repertorizator.cpp
#     src/repertorizator.h
# )
# target_link_libraries(TestRepertorizator PRIVATE
#     Qt${QT_VERSION_MAJOR}::Test
# )

# qt_add_executable(TestRepModel
#     tests/test_repmodel.cpp
#     src/repmodel.h
#     src/repmodel.cpp
# )
# target_link_libraries(TestRepModel PRIVATE
#     Qt${QT_VERSION_MAJOR}::Test
# )


# include(CTest)
# enable_testing(true)
# add_test(NAME TestRepertorizator
#          COMMAND TestRepertorizator)
