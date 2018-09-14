set(SRC
    main.cpp
    mainwindow.h
    mainwindow.cpp
)

add_executable(box-labeler WIN32 ${SRC})
set_target_properties(box-labeler PROPERTIES
    CXX_STANDARD          11
    CXX_STANDARD_REQUIRED ON
)

target_link_libraries(box-labeler Qt5::Widgets)

install(TARGETS box-labeler RUNTIME DESTINATION bin)
