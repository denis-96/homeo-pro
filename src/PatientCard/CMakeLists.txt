qt_add_library(PatientCard STATIC
    PatientCard.h PatientCard.cpp
    PassportDetailsTab.h PassportDetailsTab.cpp

    DiseaseHistoryTab.h DiseaseHistoryTab.cpp
    PCardTab.h PCardTab.cpp
    LifeHistoryTab.h LifeHistoryTab.cpp

    PCardField.h PCardField.cpp
    TableField.h TableField.cpp
)

qt_add_resources(PatientCard pcardIcons
    PREFIX "/"
    FILES
        ../../resources/icons/remove.svg
        ../../resources/icons/add.svg
)


target_link_libraries(PatientCard PRIVATE Common Qt6::Widgets)
target_include_directories(PatientCard INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
