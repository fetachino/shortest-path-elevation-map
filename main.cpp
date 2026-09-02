#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <cmath>
#include <limits>
#include <string>

// Constants for map dimensions
const int WIDTH = 250;
const int HEIGHT = 200;
const int START_X = 0;     // Start from top row
const int START_Y = 249;   // Start from rightmost column
const int END_X = 199;     // End at bottom row
const int END_Y = 0;       // End at leftmost column

// Direction codes for the output map
const unsigned char NORTH = 50;  // ↑
const unsigned char EAST = 100;  // →
const unsigned char SOUTH = 150; // ↓
const unsigned char WEST = 200;  // ←
const unsigned char UNKNOWN = 0; // Not yet determined

// Node for the priority queue
struct Node {
    int x, y;           // Coordinates in the map
    double distance;    // Distance from start

    // Overload operator for priority queue (min-heap)
    bool operator>(const Node& other) const {
        return distance > other.distance;
    }
};

// Function to calculate cost between two adjacent points
double calculateCost(unsigned char h1, unsigned char h2) {
    double heightDiff = static_cast<double>(h1) - static_cast<double>(h2);
    return sqrt(1.0 + heightDiff * heightDiff);
}

// Function to check if coordinates are valid
bool isValid(int x, int y) {
    return x >= 0 && x < HEIGHT && y >= 0 && y < WIDTH;
}

// Function to get index from coordinates
int getIndex(int x, int y) {
    if (isValid(x, y)) {
        return x * WIDTH + y;
    }
    return -1; // Invalid index
}

int main() {
    // Set the path for input and output files
    std::string inputPath = "map1.raw";
    std::string directionPath = "direction.raw";
    std::string pathOutputPath = "path.raw";

    // Open input file
    std::ifstream inputFile(inputPath, std::ios::binary);
    if (!inputFile) {
        std::cerr << "Could not open input file " << inputPath << std::endl;
        std::cout << "Make sure the file is in the correct directory." << std::endl;
        std::cout << "Press Enter to exit...";
        std::cin.get();
        return 1;
    }

    // Read elevation map
    std::vector<unsigned char> elevationMap(WIDTH * HEIGHT);
    inputFile.read(reinterpret_cast<char*>(elevationMap.data()), WIDTH * HEIGHT);
    inputFile.close();

    std::cout << "Successfully read elevation map" << std::endl;

    // Show some stats about the elevation map
    unsigned char min = 255, max = 0;
    double sum = 0;
    for (auto h : elevationMap) {
        min = std::min(min, h);
        max = std::max(max, h);
        sum += h;
    }
    std::cout << "Elevation map statistics:" << std::endl;
    std::cout << "  Min height: " << static_cast<int>(min) << std::endl;
    std::cout << "  Max height: " << static_cast<int>(max) << std::endl;
    std::cout << "  Average height: " << sum / elevationMap.size() << std::endl;

    // Initialize data structures
    std::vector<double> distances(WIDTH * HEIGHT, std::numeric_limits<double>::infinity());
    std::vector<unsigned char> directions(WIDTH * HEIGHT, UNKNOWN);
    std::vector<bool> visited(WIDTH * HEIGHT, false);

    // Priority queue for Dijkstra's algorithm
    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> pq;

    // Initialize starting point
    int startIdx = getIndex(START_X, START_Y);
    if (startIdx == -1) {
        std::cerr << "Error: Start coordinates (" << START_X << ", " << START_Y << ") are invalid!" << std::endl;
        std::cout << "Press Enter to exit...";
        std::cin.get();
        return 1;
    }

    distances[startIdx] = 0.0;
    pq.push({ START_X, START_Y, 0.0 });

    std::cout << "Starting Dijkstra's algorithm from point (" << START_X << ", " << START_Y << ")..." << std::endl;

    // Direction deltas for north, east, south, west
    const int dx[4] = { -1, 0, 1, 0 };  // Change in x (row)
    const int dy[4] = { 0, 1, 0, -1 };  // Change in y (column)
    const unsigned char dirCodes[4] = { NORTH, EAST, SOUTH, WEST };

    // Progress tracking
    int processedNodes = 0;
    const int totalNodes = WIDTH * HEIGHT;
    const int reportInterval = totalNodes / 10; // Report progress every 10%

    // Run Dijkstra's algorithm
    while (!pq.empty()) {
        Node current = pq.top();
        pq.pop();

        int currentIdx = getIndex(current.x, current.y);
        if (currentIdx == -1) continue; // Skip invalid indices

        // Skip if we've already processed this node
        if (visited[currentIdx]) continue;

        // Mark as visited
        visited[currentIdx] = true;
        processedNodes++;

        // Report progress
        if (processedNodes % reportInterval == 0 || processedNodes == totalNodes) {
            int percentage = (processedNodes * 100) / totalNodes;
            std::cout << "  Progress: " << percentage << "% (" << processedNodes << "/" << totalNodes << " nodes)" << std::endl;
        }

        // Check all four adjacent nodes
        for (int dir = 0; dir < 4; ++dir) {
            int nx = current.x + dx[dir];
            int ny = current.y + dy[dir];

            // Skip if out of bounds
            if (!isValid(nx, ny)) continue;

            int nextIdx = getIndex(nx, ny);
            if (nextIdx == -1) continue; // Skip invalid indices

            // Skip if already visited
            if (visited[nextIdx]) continue;

            // Calculate new distance
            double cost = calculateCost(elevationMap[currentIdx], elevationMap[nextIdx]);
            double newDist = distances[currentIdx] + cost;

            // Update if new path is shorter
            if (newDist < distances[nextIdx]) {
                distances[nextIdx] = newDist;
                directions[nextIdx] = dirCodes[(dir + 2) % 4]; // Store opposite direction (from where we came)
                pq.push({ nx, ny, newDist });
            }
        }
    }

    std::cout << "Dijkstra's algorithm completed" << std::endl;

    // Output direction map
    std::ofstream directionFile(directionPath, std::ios::binary);
    if (!directionFile) {
        std::cerr << "Could not create " << directionPath << std::endl;
        std::cout << "Press Enter to exit...";
        std::cin.get();
        return 1;
    }
    directionFile.write(reinterpret_cast<const char*>(directions.data()), WIDTH * HEIGHT);
    directionFile.close();
    std::cout << "Direction map saved to " << directionPath << std::endl;

    // Create path map (from start to end point)
    std::vector<unsigned char> pathMap(WIDTH * HEIGHT, 0);

    // Trace path from end to start
    int x = END_X;
    int y = END_Y;
    int pathLength = 0;

    std::cout << "Tracing path from (" << END_X << ", " << END_Y << ") to (" << START_X << ", " << START_Y << ")..." << std::endl;

    // Validate end coordinates
    if (!isValid(END_X, END_Y)) {
        std::cerr << "Error: End coordinates (" << END_X << ", " << END_Y << ") are invalid!" << std::endl;
        std::cout << "Press Enter to exit...";
        std::cin.get();
        return 1;
    }

    // Check if a path exists
    int endIdx = getIndex(END_X, END_Y);
    if (distances[endIdx] == std::numeric_limits<double>::infinity()) {
        std::cerr << "Error: No path exists from start to end point!" << std::endl;
        std::cout << "Press Enter to exit...";
        std::cin.get();
        return 1;
    }

    // Safely trace the path
    while (!(x == START_X && y == START_Y)) {
        int idx = getIndex(x, y);
        if (idx == -1) {
            std::cerr << "Error: Invalid coordinates (" << x << ", " << y << ") encountered while tracing path!" << std::endl;
            break;
        }

        pathMap[idx] = 255; // Mark path
        pathLength++;

        // Follow direction backward
        unsigned char dir = directions[idx];
        int nx = x, ny = y;

        if (dir == NORTH) nx++;       // Move south
        else if (dir == EAST) ny--;   // Move west
        else if (dir == SOUTH) nx--;  // Move north
        else if (dir == WEST) ny++;   // Move east
        else {
            std::cerr << "Error: No valid direction at (" << x << ", " << y << ")" << std::endl;
            break;
        }

        // Validate next coordinates
        if (!isValid(nx, ny)) {
            std::cerr << "Error: Path leads out of bounds at (" << x << ", " << y << ") -> (" << nx << ", " << ny << ")" << std::endl;
            break;
        }

        // Move to next position
        x = nx;
        y = ny;

        // Safety check to prevent infinite loops
        if (pathLength > WIDTH * HEIGHT) {
            std::cerr << "Error: Path tracing exceeded maximum length. Possible loop detected." << std::endl;
            break;
        }
    }

    // Mark the start point
    int startIndex = getIndex(START_X, START_Y);
    if (startIndex != -1) {
        pathMap[startIndex] = 255;
        pathLength++;
    }

    // Output path map
    std::ofstream pathFile(pathOutputPath, std::ios::binary);
    if (!pathFile) {
        std::cerr << "Could not create " << pathOutputPath << std::endl;
        std::cout << "Press Enter to exit...";
        std::cin.get();
        return 1;
    }
    pathFile.write(reinterpret_cast<const char*>(pathMap.data()), WIDTH * HEIGHT);
    pathFile.close();
    std::cout << "Path map saved to " << pathOutputPath << std::endl;

    // Output some statistics
    if (endIdx != -1) {
        std::cout << "Shortest path distance to end point: " << distances[endIdx] << std::endl;
    }
    std::cout << "Path length (in pixels): " << pathLength << std::endl;

    std::cout << "\nProgram completed successfully. Press Enter to exit...";
    std::cin.get();
    return 0;
}