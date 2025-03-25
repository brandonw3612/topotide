add_executable(topotide-cli
        main.cpp
        Context.cpp
        ProgressReporter.cpp
        RiverData.cpp
        GraphComputer.cpp
        AbstractChannel.cpp
)
target_link_libraries(topotide-cli PRIVATE topotidelib)
target_link_libraries(topotide-cli PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets Qt6::OpenGL Qt6::OpenGLWidgets)