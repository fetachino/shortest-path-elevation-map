# Shortest Path on an Elevation Map

[![C++](https://img.shields.io/badge/C++-17-00599C?logo=cplusplus&logoColor=white)](https://isocpp.org/)
[![Algorithm](https://img.shields.io/badge/Algorithm-Dijkstra-6C5CE7)](https://en.wikipedia.org/wiki/Dijkstra%27s_algorithm)
[![Data](https://img.shields.io/badge/Input-250x200_RAW_raster-2A9D8F)](#build)

A C++ implementation of Dijkstra's algorithm for finding least-cost routes across a grayscale elevation map.

## Raster preview

| Elevation input | Direction output |
| --- | --- |
| ![Grayscale elevation raster](docs/images/elevation-input.png) | ![Direction raster produced by the algorithm](docs/images/direction-map.png) |

![Shortest-path raster produced from the elevation map](docs/images/shortest-path-output.png)

![Shortest-path raster produced from the elevation map](docs/images/shortest-path-output.png)

The PNG previews were converted losslessly from the project's 250×200 raw raster files for browser viewing.

## Problem model

```mermaid
flowchart LR
    A[8-bit elevation raster] --> B[Pixel-to-vertex graph]
    B --> C[Elevation-aware edge costs]
    C --> D[Dijkstra priority queue]
    D --> E[Direction raster]
    D --> F[Shortest-path raster]
```

Unlike a simple shortest-step calculation, each move is weighted by elevation change, so flatter routes can be preferred over geometrically direct but steeper routes.

## Approach

- Models each pixel as a graph vertex.
- Connects each location to its north, east, south, and west neighbors.
- Calculates edge cost from horizontal movement and elevation change.
- Uses a min-priority queue to compute shortest distances efficiently.
- Produces raw direction and path-visualization outputs.

## Complexity

For `V` pixels and at most four edges per pixel, the priority-queue implementation runs in approximately `O((V + E) log V)`, which simplifies to `O(V log V)` for this grid.

## Build

Open `ShortestPathElevationMap.vcxproj` in Visual Studio 2022, or compile directly:

```console
g++ -std=c++17 -O2 main.cpp -o elevation-path
```

Place a 250x200, 8-bit grayscale elevation file named `map1.raw` in the working directory, then run the executable. It writes `direction.raw` and `path.raw`.

The original course input dataset is not redistributed.

## Skills demonstrated

C++, graph modeling, Dijkstra's algorithm, priority queues, binary file I/O, and algorithmic complexity analysis.

## Verification status

The source compiled successfully with the Visual Studio 2022 C++17 toolchain during the portfolio pass. A boundary-condition failure was also reproduced at the lower map edge; it is documented honestly rather than presenting the current implementation as production-ready.

## About the author

Built by **Ahmed Balde** as a graph-algorithm and binary-raster processing project. See more C++, Python, data, and software-engineering work on [GitHub](https://github.com/fetachino).
