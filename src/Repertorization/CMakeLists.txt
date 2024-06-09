qt_add_library(Repertorization STATIC
    RepModel.h RepModel.cpp
    RepView.h RepView.cpp
    Rubric.h Rubric.cpp
    RepRroxyModel.h RepRroxyModel.cpp
    Repertorization.h Repertorization.cpp
)

qt_add_resources(Repertorization repIcons
    PREFIX "/"
    FILES
        ../../resources/icons/remove.svg
        ../../resources/icons/add.svg
)


target_link_libraries(Repertorization PRIVATE Common Qt6::Widgets)
target_include_directories(Repertorization INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

