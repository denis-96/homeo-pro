qt_add_executable(HomeoPro
    main.cpp
    HomeoPro.cpp
    HomeoPro.h
    ../resources/resource.rc
)
qt_add_resources(HomeoPro icons
    PREFIX "/"
    FILES
        ../resources/icons/main-icon.svg
        ../resources/icons/new-file.svg
        ../resources/icons/open-file.svg
        ../resources/icons/save-file.svg
        ../resources/icons/close-file.svg
        ../resources/icons/exit.svg
)

target_link_libraries(HomeoPro PRIVATE Common Repertorization PatientCard Qt6::Widgets)
set_target_properties(HomeoPro PROPERTIES
    WIN32_EXECUTABLE ON
)

add_subdirectory(Common)
add_subdirectory(Repertorization)
add_subdirectory(PatientCard)
