#pragma once
#include "Utils.h"
#include <iostream>
#include <string>
#include <cfloat> 

using namespace std;


const int LOC_BUS_STOP = 0;
const int LOC_HOSPITAL = 1;
const int LOC_PHARMACY = 2;
const int LOC_SCHOOL = 3;
const int LOC_MALL = 4;
const int LOC_FACILITY = 5;
const int LOC_HOUSE = 6;
const int LOC_OTHER = 7;

// Convert int type to string
string locationTypeToString(int type) {
    switch (type) {
    case LOC_BUS_STOP:
        return "BusStop";
    case LOC_HOSPITAL: 
        return "Hospital";
    case LOC_PHARMACY: 
        return "Pharmacy";
    case LOC_SCHOOL: 
        return "School";
    case LOC_MALL: 
        return "Mall";
    case LOC_FACILITY: 
        return "Facility";
    case LOC_HOUSE: 
        return "House";
    default:
        return "Other";
    }
}


// Structs 
struct CityLocation {
    string id;
    string name;
    int type;       
    double lat, lon;
    void* entityPtr;
    CityLocation* next;

    CityLocation(const string& i, const string& n, int t, double la, double lo, void* ptr = nullptr)
        : id(i), name(n), type(t), lat(la), lon(lo), entityPtr(ptr), next(nullptr) {
    }
};


struct CityRoad {
    string destID;
    double distance;
    CityRoad* next;

    CityRoad(const string& dest, double dist) : destID(dest), distance(dist), next(nullptr) {}
};

// Adjacency list node for city graph
struct CityAdjNode {
    string locationID;
    CityRoad* roadHead;
    CityAdjNode* next;

    CityAdjNode(const string& id) : locationID(id), roadHead(nullptr), next(nullptr) {}
};

// Min-heap node for Dijkstra
struct CityDijkstraNode {
    string locationID;
    double distance;
};

// Min-heap for Dijkstra in city graph
class CityDijkstraHeap {
private:
    static const int MAX_SIZE = 500;
    CityDijkstraNode heap[MAX_SIZE];
    int size;

    void swap(int i, int j) {
        CityDijkstraNode temp = heap[i];
        heap[i] = heap[j];
        heap[j] = temp;
    }

    void siftUp(int i) {
        while (i > 0) {
            int parent = (i - 1) / 2;
            if (heap[i].distance < heap[parent].distance) {
                swap(i, parent);
                i = parent;
            }
            else break;
        }
    }

    void siftDown(int i) {
        while (true) {
            int smallest = i;
            int left = 2 * i + 1;
            int right = 2 * i + 2;

            if (left < size && heap[left].distance < heap[smallest].distance)
                smallest = left;
            if (right < size && heap[right].distance < heap[smallest].distance)
                smallest = right;

            if (smallest != i) {
                swap(i, smallest);
                i = smallest;
            }
            else break;
        }
    }

public:
    CityDijkstraHeap() : size(0) {}

    bool isEmpty() { return size == 0; }

    void insert(const string& id, double dist) {
        if (size >= MAX_SIZE) return;
        heap[size].locationID = id;
        heap[size].distance = dist;
        siftUp(size);
        size++;
    }

    CityDijkstraNode extractMin() {
        CityDijkstraNode minNode = heap[0];
        heap[0] = heap[size - 1];
        size--;
        if (size > 0) siftDown(0);
        return minNode;
    }

    void clear() { size = 0; }
};

//City Graph class 
// Singleton Class
class CityGraph {
private:
    CityLocation* locationsHead;
    CityAdjNode* adjListHead;
    int locationCount;

    static CityGraph* instance;

    CityGraph() : locationsHead(nullptr), adjListHead(nullptr), locationCount(0) {}

    // Get adjacency node
    CityAdjNode* getAdjNode(const string& id) {
        CityAdjNode* current = adjListHead;
        while (current != nullptr) {
            if (stringsEqualIgnoreCase(current->locationID, id)) return current;
            current = current->next;
        }
        return nullptr;
    }

    // Get or create adjacency node
    CityAdjNode* getOrCreateAdjNode(const string& id) {
        CityAdjNode* existing = getAdjNode(id);
        if (existing != nullptr) return existing;

        CityAdjNode* newNode = new CityAdjNode(id);
        newNode->next = adjListHead;
        adjListHead = newNode;
        return newNode;
    }

    // Check if edge exists
    bool edgeExists(const string& from, const string& to) {
        CityAdjNode* adj = getAdjNode(from);
        if (adj == nullptr) return false;

        CityRoad* road = adj->roadHead;
        while (road != nullptr) {
            if (stringsEqualIgnoreCase(road->destID, to)) return true;
            road = road->next;
        }
        return false;
    }

public:

    static CityGraph* getInstance() {
        if (instance == nullptr) {
            instance = new CityGraph();
        }
        return instance;
    }

    // Export all locations for SFML visualization
    int exportAllLocations(string names[], double lats[], double lons[],
        int types[], int maxCount) {
        int count = 0;
        CityLocation* current = locationsHead;

        while (current != nullptr && count < maxCount) {
            names[count] = current->name;
            lats[count] = current->lat;
            lons[count] = current->lon;
            types[count] = current->type;
            count++;
            current = current->next;
        }

        return count;
    }

    // Export locations of a specific type
    int exportLocationsByType(int targetType, string names[], double lats[],
        double lons[], int maxCount) {
        int count = 0;
        CityLocation* current = locationsHead;

        while (current != nullptr && count < maxCount) {
            if (current->type == targetType) {
                names[count] = current->name;
                lats[count] = current->lat;
                lons[count] = current->lon;
                count++;
            }
            current = current->next;
        }

        return count;
    }

    // ==========================================
    // LOCATION OPERATIONS
    // ==========================================

    // CHANGED: type is now int
    bool addLocation(const string& id, const string& name, int type,
        double lat, double lon, void* entityPtr = nullptr) {

        if (findLocationByID(id) != nullptr) {
            return false;  // Already exists
        }

        CityLocation* newLoc = new CityLocation(id, name, type, lat, lon, entityPtr);
        newLoc->next = locationsHead;
        locationsHead = newLoc;
        locationCount++;

        // Create adjacency list entry
        getOrCreateAdjNode(id);

        return true;
    }

    bool removeLocation(const string& id) {
        if (locationsHead == nullptr) return false;

        // Remove all edges TO this location
        CityAdjNode* adj = adjListHead;
        while (adj != nullptr) {
            removeRoad(adj->locationID, id);
            adj = adj->next;
        }

        // Remove this location's adjacency list
        removeAdjNode(id);

        // Remove from locations list
        if (stringsEqualIgnoreCase(locationsHead->id, id)) {
            CityLocation* temp = locationsHead;
            locationsHead = locationsHead->next;
            delete temp;
            locationCount--;
            return true;
        }

        CityLocation* current = locationsHead;
        while (current->next != nullptr) {
            if (stringsEqualIgnoreCase(current->next->id, id)) {
                CityLocation* temp = current->next;
                current->next = current->next->next;
                delete temp;
                locationCount--;
                return true;
            }
            current = current->next;
        }
        return false;
    }

    void removeAdjNode(const string& id) {
        if (adjListHead == nullptr) return;

        if (stringsEqualIgnoreCase(adjListHead->locationID, id)) {
            CityAdjNode* temp = adjListHead;
            // Delete all roads first
            while (temp->roadHead != nullptr) {
                CityRoad* roadTemp = temp->roadHead;
                temp->roadHead = temp->roadHead->next;
                delete roadTemp;
            }
            adjListHead = adjListHead->next;
            delete temp;
            return;
        }

        CityAdjNode* current = adjListHead;
        while (current->next != nullptr) {
            if (stringsEqualIgnoreCase(current->next->locationID, id)) {
                CityAdjNode* temp = current->next;
                while (temp->roadHead != nullptr) {
                    CityRoad* roadTemp = temp->roadHead;
                    temp->roadHead = temp->roadHead->next;
                    delete roadTemp;
                }
                current->next = current->next->next;
                delete temp;
                return;
            }
            current = current->next;
        }
    }

    CityLocation* findLocationByID(const string& id) {
        CityLocation* current = locationsHead;
        while (current != nullptr) {
            if (stringsEqualIgnoreCase(current->id, id)) 
                return current;
            current = current->next;
        }
        return nullptr;
    }

    CityLocation* findLocationByName(const string& name) {
        CityLocation* current = locationsHead;
        while (current != nullptr) {
            if (stringsEqualIgnoreCase(current->name, name)) 
                return current;
            current = current->next;
        }
        return nullptr;
    }

    // Road Operations

    bool connectLocations(const string& id1, const string& id2, double distance = -1) {
        CityLocation* loc1 = findLocationByID(id1);
        CityLocation* loc2 = findLocationByID(id2);

        if (loc1 == nullptr || loc2 == nullptr) {
            return false;
        }

        if (edgeExists(id1, id2)) {
            return false;  // Already connected
        }

        // Calculate distance if not provided assumes calculateDistance is in Utils.h
        if (distance < 0) {
            distance = calculateDistance(loc1->lat, loc1->lon, loc2->lat, loc2->lon);
        }

        // Add bidirectional edges
        CityAdjNode* adj1 = getOrCreateAdjNode(id1);
        CityRoad* road1 = new CityRoad(id2, distance);
        road1->next = adj1->roadHead;
        adj1->roadHead = road1;

        CityAdjNode* adj2 = getOrCreateAdjNode(id2);
        CityRoad* road2 = new CityRoad(id1, distance);
        road2->next = adj2->roadHead;
        adj2->roadHead = road2;

        return true;
    }

    bool removeRoad(const string& id1, const string& id2) {
        bool removed = false;

        // Remove from id1's adjacency list
        CityAdjNode* adj1 = getAdjNode(id1);
        if (adj1 != nullptr && adj1->roadHead != nullptr) {
            if (stringsEqualIgnoreCase(adj1->roadHead->destID, id2)) {
                CityRoad* temp = adj1->roadHead;
                adj1->roadHead = adj1->roadHead->next;
                delete temp;
                removed = true;
            }
            else {
                CityRoad* current = adj1->roadHead;
                while (current->next != nullptr) {
                    if (stringsEqualIgnoreCase(current->next->destID, id2)) {
                        CityRoad* temp = current->next;
                        current->next = current->next->next;
                        delete temp;
                        removed = true;
                        break;
                    }
                    current = current->next;
                }
            }
        }

        // Remove from id2's adjacency list undirected
        CityAdjNode* adj2 = getAdjNode(id2);
        if (adj2 != nullptr && adj2->roadHead != nullptr) {
            if (stringsEqualIgnoreCase(adj2->roadHead->destID, id1)) {
                CityRoad* temp = adj2->roadHead;
                adj2->roadHead = adj2->roadHead->next;
                delete temp;
                removed = true;
            }
            else {
                CityRoad* current = adj2->roadHead;
                while (current->next != nullptr) {
                    if (stringsEqualIgnoreCase(current->next->destID, id1)) {
                        CityRoad* temp = current->next;
                        current->next = current->next->next;
                        delete temp;
                        removed = true;
                        break;
                    }
                    current = current->next;
                }
            }
        }

        return removed;
    }


    // Subgraph Extraction
    // Changed: type is now int
    int getLocationsOfType(int type, CityLocation** result, int maxCount) {
        int count = 0;
        CityLocation* current = locationsHead;
        while (current != nullptr && count < maxCount) {
            if (current->type == type) {
                result[count++] = current;
            }
            current = current->next;
        }
        return count;
    }


    // Nearest Locations Queries
    CityLocation* findNearestOfType(double userLat, double userLon, int type) {
        CityLocation* nearest = nullptr;
        double minDist = DBL_MAX;

        CityLocation* current = locationsHead;
        while (current != nullptr) {
            if (current->type == type) {
                double dist = calculateDistance(userLat, userLon, current->lat, current->lon);
                if (dist < minDist) {
                    minDist = dist;
                    nearest = current;
                }
            }
            current = current->next;
        }
        return nearest;
    }

    CityLocation* findNearestOfTypeFromLocation(const string& srcID, int destType, double& outDistance) {
        outDistance = DBL_MAX;
        CityLocation* srcLoc = findLocationByID(srcID);
        if (srcLoc == nullptr) return nullptr;

        return findNearestOfType(srcLoc->lat, srcLoc->lon, destType);
    }


    // Dijkstra's Algorithm
    double findShortestPath(const string& srcID, const string& destID,
        string path[], int& pathLen, int maxPathLen) {
        pathLen = 0;

        CityLocation* srcLoc = findLocationByID(srcID);
        CityLocation* destLoc = findLocationByID(destID);

        if (srcLoc == nullptr || destLoc == nullptr) {
            return DBL_MAX;
        }

        // Initialize arrays
        const int MAX_LOCATIONS = 500;
        string locIDs[MAX_LOCATIONS];
        double distances[MAX_LOCATIONS];
        string previous[MAX_LOCATIONS];
        bool visited[MAX_LOCATIONS];
        int numLocs = 0;

        // Collect all location IDs
        CityLocation* current = locationsHead;
        while (current != nullptr && numLocs < MAX_LOCATIONS) {
            locIDs[numLocs] = current->id;
            distances[numLocs] = DBL_MAX;
            previous[numLocs] = "";
            visited[numLocs] = false;
            numLocs++;
            current = current->next;
        }

        // Find source index
        int srcIdx = -1;
        for (int i = 0; i < numLocs; i++) {
            if (stringsEqualIgnoreCase(locIDs[i], srcID)) {
                srcIdx = i;
                break;
            }
        }
        if (srcIdx == -1) return DBL_MAX;

        distances[srcIdx] = 0;

        // Dijkstra's algorithm
        CityDijkstraHeap minHeap;
        minHeap.insert(srcID, 0);

        while (!minHeap.isEmpty()) {
            CityDijkstraNode minNode = minHeap.extractMin();
            string currentID = minNode.locationID;

            int currentIdx = -1;
            for (int i = 0; i < numLocs; i++) {
                if (stringsEqualIgnoreCase(locIDs[i], currentID)) {
                    currentIdx = i;
                    break;
                }
            }

            if (currentIdx == -1 || visited[currentIdx]) continue;
            visited[currentIdx] = true;

            // Process neighbors
            CityAdjNode* adj = getAdjNode(currentID);
            if (adj != nullptr) {
                CityRoad* road = adj->roadHead;
                while (road != nullptr) {
                    int neighborIdx = -1;
                    for (int i = 0; i < numLocs; i++) {
                        if (stringsEqualIgnoreCase(locIDs[i], road->destID)) {
                            neighborIdx = i;
                            break;
                        }
                    }

                    if (neighborIdx != -1 && !visited[neighborIdx]) {
                        double newDist = distances[currentIdx] + road->distance;
                        if (newDist < distances[neighborIdx]) {
                            distances[neighborIdx] = newDist;
                            previous[neighborIdx] = currentID;
                            minHeap.insert(road->destID, newDist);
                        }
                    }
                    road = road->next;
                }
            }
        }

        // Find destination index
        int destIdx = -1;
        for (int i = 0; i < numLocs; i++) {
            if (stringsEqualIgnoreCase(locIDs[i], destID)) {
                destIdx = i;
                break;
            }
        }

        if (destIdx == -1 || distances[destIdx] == DBL_MAX) {
            return DBL_MAX;
        }

        // Reconstruct path use temporary array as stack
        string tempPath[MAX_LOCATIONS];
        int tempLen = 0;
        string currentID = destID;

        while (!currentID.empty() && tempLen < MAX_LOCATIONS) {
            tempPath[tempLen++] = currentID;
            int idx = -1;
            for (int i = 0; i < numLocs; i++) {
                if (stringsEqualIgnoreCase(locIDs[i], currentID)) {
                    idx = i;
                    break;
                }
            }
            if (idx == -1) break;
            currentID = previous[idx];
        }

        // Reverse path
        pathLen = 0;
        for (int i = tempLen - 1; i >= 0 && pathLen < maxPathLen; i--) {
            path[pathLen++] = tempPath[i];
        }

        return distances[destIdx];
    }

    // Find shortest path to nearest location of a specific type
    double findShortestPathToType(const string& srcID, int destType,
        string& destID, string path[], int& pathLen, int maxPathLen) {
        double minTotalDist = DBL_MAX;
        string bestDestID = "";
        string bestPath[100];
        int bestPathLen = 0;

        // Try all locations of the target type
        CityLocation* current = locationsHead;
        while (current != nullptr) {
            if (current->type == destType) {
                string tempPath[100];
                int tempPathLen = 0;
                double dist = findShortestPath(srcID, current->id, tempPath, tempPathLen, 100);

                if (dist < minTotalDist) {
                    minTotalDist = dist;
                    bestDestID = current->id;
                    bestPathLen = tempPathLen;
                    for (int i = 0; i < tempPathLen && i < 100; i++) {
                        bestPath[i] = tempPath[i];
                    }
                }
            }
            current = current->next;
        }

        destID = bestDestID;
        pathLen = 0;
        for (int i = 0; i < bestPathLen && i < maxPathLen; i++) {
            path[pathLen++] = bestPath[i];
        }

        return minTotalDist;
    }


    // Display functions 

    void displayAllLocations() {
        cout << "\n=== City Graph - All Locations ===" << endl;
        CityLocation* current = locationsHead;
        int count = 0;
        while (current != nullptr) {
            cout << ++count << ".  [" << locationTypeToString(current->type) << "] "
                << current->name << " (ID: " << current->id << ") at ("
                << current->lat << ", " << current->lon << ")" << endl;
            current = current->next;
        }
        if (count == 0) cout << "No locations in city graph." << endl;
        cout << "Total locations: " << locationCount << endl;
    }

    void displayGraph() {
        cout << "\n=== City Graph - Adjacency List ===" << endl;
        CityAdjNode* adj = adjListHead;
        while (adj != nullptr) {
            CityLocation* loc = findLocationByID(adj->locationID);
            cout << "[" << (loc ? loc->name : adj->locationID) << "] -> ";

            CityRoad* road = adj->roadHead;
            if (road == nullptr) {
                cout << "(no connections)";
            }
            while (road != nullptr) {
                CityLocation* dest = findLocationByID(road->destID);
                cout << (dest ? dest->name : road->destID) << "(" << road->distance << ")";
                if (road->next != nullptr) cout << ", ";
                road = road->next;
            }
            cout << endl;
            adj = adj->next;
        }
    }

    void displayLocationsByType(int type) {
        cout << "\n=== " << locationTypeToString(type) << " Locations ===" << endl;
        CityLocation* current = locationsHead;
        int count = 0;
        while (current != nullptr) {
            if (current->type == type) {
                cout << ++count << ".  " << current->name << " (ID: " << current->id << ")" << endl;
            }
            current = current->next;
        }
        if (count == 0) cout << "No locations of this type." << endl;
    }

    int getLocationCount() { return locationCount; }

    // Destructor
    ~CityGraph() {
        // Clean up locations
        while (locationsHead != nullptr) {
            CityLocation* temp = locationsHead;
            locationsHead = locationsHead->next;
            delete temp;
        }

        // Clean up adjacency list
        while (adjListHead != nullptr) {
            CityAdjNode* adjTemp = adjListHead;
            adjListHead = adjListHead->next;
            while (adjTemp->roadHead != nullptr) {
                CityRoad* roadTemp = adjTemp->roadHead;
                adjTemp->roadHead = adjTemp->roadHead->next;
                delete roadTemp;
            }
            delete adjTemp;
        }
    }
};

// Initialize static instance as it is a singleton class
CityGraph* CityGraph::instance = nullptr;

