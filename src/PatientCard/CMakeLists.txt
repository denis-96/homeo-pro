qt_add_library(PatientCard STATIC
    PatientCard.h PatientCard.cpp
    PassportDetailsTab.h PassportDetailsTab.cpp
    ComplaintsTab.h ComplaintsTab.cpp
)

# qt_add_resources(PatientCard pcardIcons
#     PREFIX "/"
#     FILES
# )


target_link_libraries(PatientCard PRIVATE Common Qt6::Widgets)
target_include_directories(PatientCard INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
