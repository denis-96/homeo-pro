qt_add_library(PatientCard STATIC
    PatientCard.h PatientCard.cpp

    PCardTab.h PCardTab.cpp
    Field.h Field.cpp
    TableField.h TableField.cpp
    TextField.h TextField.cpp
    LineField.h LineField.cpp
    NumberField.h NumberField.cpp
    DateField.h DateField.cpp
)

qt_add_resources(PatientCard pcardIcons
    PREFIX "/"
    FILES
        ../../resources/icons/remove.svg
        ../../resources/icons/add.svg
)


target_link_libraries(PatientCard PRIVATE Common Qt6::Widgets)
target_include_directories(PatientCard INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
