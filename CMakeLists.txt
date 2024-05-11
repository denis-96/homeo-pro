cmake_minimum_required(VERSION 3.5)

project(Repertorizator VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Test)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Test)

set(PROJECT_SOURCES
    repertorizator.cpp repertorizator.h
    repview.cpp repview.h
    repmodel.cpp repmodel.h

    rubric.cpp rubric.h
    drug.cpp drug.h
    resources.qrc
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(Repertorizator
        MANUAL_FINALIZATION
        main.cpp
        ${PROJECT_SOURCES}
        example.rep
        .gitignore
    )
endif()

target_link_libraries(Repertorizator PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.Repertorizator)
endif()
set_target_properties(Repertorizator PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS Repertorizator
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Repertorizator)
endif()


# Unit Test

# include(CTest)

# qt_add_executable(RepertorizatorTester
#     test.cpp
#     ${PROJECT_SOURCES}
# )

# target_link_libraries(RepertorizatorTester PRIVATE
#     Qt${QT_VERSION_MAJOR}::Widgets
#     Qt${QT_VERSION_MAJOR}::Test
# )


# add_test(NAME RepertorizatorTester
#          COMMAND RepertorizatorTester)
