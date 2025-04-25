add_executable(topotide-match
        main.cpp
        Context.cpp
        ProgressReporter.cpp
        RiverData.cpp
        GraphComputer.cpp
        DualFrameViewer.cpp
        SGraphicsView.cpp
        PathFrame.cpp
        Frame.cpp
        PathSimilarityComputer.cpp
        Reach.cpp
        NetworkGraphEdgeConnector.cpp
        ReachNetwork.cpp
        Counters.cpp
        ReachNetworkDisplayFrame.cpp
        NetworkConverter.cpp
)
target_link_libraries(topotide-match PRIVATE topotidelib)
target_link_libraries(topotide-match PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets Qt6::OpenGL Qt6::OpenGLWidgets)