configure_file(config.h.in "${CMAKE_CURRENT_BINARY_DIR}/config.h")

set(SRC
    cell.h
    cell.cpp
    main.cpp
    mainwindow.h
    mainwindow.cpp
    multilinedelegate.h
    multilinedelegate.cpp
    printtask.h
    printtask.cpp
    queuewidget.h
    queuewidget.cpp
    resource.qrc
    resource.rc
    sheet.h
    sheet.cpp
    sheetwidget.h
    sheetwidget.cpp
)

add_executable(box-labeler WIN32 ${SRC})
set_target_properties(box-labeler PROPERTIES
    CXX_STANDARD          11
    CXX_STANDARD_REQUIRED ON
)

target_include_directories(box-labeler PRIVATE
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
)

target_link_libraries(box-labeler Qt5::PrintSupport Qt5::Widgets)

install(TARGETS box-labeler RUNTIME DESTINATION bin)

set(CMAKE_INSTALL_UCRT_LIBRARIES TRUE)
include(InstallRequiredSystemLibraries)
