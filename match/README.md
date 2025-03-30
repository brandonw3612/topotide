### What does `main` do?

1. Read frames from multiple data files.
2. Read the boundary.
3. Compute the `NetworkGraph` for all frames.
4. Create an `AbstractGraph` for each frame.
5. Create a GUI to display 2 frames using the same "filter".

### How do we create an `AbstractGraph` from a `NetworkGraph`?

1. Sort and group all edges by their `delta` value.
The edges with `delta == +inf` could be connected to form the main channel.
2. For each group of edges, we connect all edges to form channels, if possible.
We also keep record of the special points (vertices in the `NetworkGraph`) along each channel.
The channels are wrapped as nodes (`AGNode`) in the abstract graph.
3. Create topological structure from the nodes, using the vertex records. (SplittingParent, MergingParent, Children)
4. Take the main channel (`delta == +inf`) as the root node of the abstract graph.

### Build and run the program

1. In `main.cpp`, set `dataFolderPath` to the path of the data files.
2. Go to the root directory of the project (`.../topotide`).
3. Create a build directory and build the project with CMake.
4. Go to the build directory for `match`.
5. Build the executable with `make`.

Suppose we are in the root directory of the project (`.../topotide`):

```bash
mkdir build
cd build
cmake ..
cd match
make
./topotide-match
```