add_executable(topotide-match
        main.cpp
        Context.cpp
        ProgressReporter.cpp
        RiverData.cpp
        GraphComputer.cpp
        AbstractChannel.cpp
        AbstractGraph.cpp
        AbstractFrame.cpp
        DualFrameViewer.cpp
        SGraphicsView.cpp
        PathFrame.cpp
        Frame.cpp
        SubdividedAbstractGraph.cpp
)
target_link_libraries(topotide-match PRIVATE topotidelib)
target_link_libraries(topotide-match PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets Qt6::OpenGL Qt6::OpenGLWidgets)