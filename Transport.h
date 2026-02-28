#pragma once

#include "Utils.h"
#include "GlobalLocationManager.h"
#include "CityGraph.h"
#include <iomanip>


struct BusStop {
    string stopID;
    string name;
    double lat, lon;
    BusStop* next;

    BusStop(const string& id, const string& n, double la, double lo) : stopID(id), name(n), lat(la), lon(lo), next(nullptr) {
    }


};




struct RoadEdge {

    string destStopID;
    double distance;
    RoadEdge* next;

    RoadEdge(const string& dest, double dist) : destStopID(dest), distance(dist), next(nullptr) {

    }

};



struct AdjListNode {

    string stopID;
    RoadEdge* edgeHead;
    AdjListNode* next;

    AdjListNode(const string& id) : stopID(id), edgeHead(nullptr), next(nullptr) {


    }


};

// Route Stop Node (Singly Linked List for Bus Route) 

struct RouteStopNode {

    string stopID;

    RouteStopNode* next;


    RouteStopNode(const string& id) : stopID(id), next(nullptr) {


    }

};


struct BusNode {


    string busNumber;

    string company;

    string currentStopID;

    RouteStopNode* routeHead;

    bool isSchoolBus;          // Flag to mark school bus

    string schoolName;          // Destination school name

    string schoolID;            // Destination school ID (if available)

    BusNode* next;

    BusNode(const string& num, const string& comp) : busNumber(num), company(comp), currentStopID(""), routeHead(nullptr), isSchoolBus(false), schoolName(""), schoolID(""), next(nullptr) {
    }

};



// Bus Hash Node 
struct BusHashNode {
    string key;

    BusNode* busRef;

    BusHashNode* next;


    BusHashNode(const string& k, BusNode* b) : key(k), busRef(b), next(nullptr) {

    }

};

struct PassengerNode {

    string name;

    string sourceStop;

    string destStop;

};


class PassengerQueue {

private:

    static const int MAX_SIZE = 500;
    PassengerNode passengers[MAX_SIZE];
    int front;
    int rear;
    int count;

public:
    PassengerQueue() : front(0), rear(-1), count(0) {

    }

    bool isEmpty()
    {
        return count == 0;
    }
    bool isFull()
    {
        return count == MAX_SIZE;

    }

    bool enqueue(const string& name, const string& src, const string& dest) {
        if (isFull()) {
            //error msg
            cout << "Error: Passenger queue is full!" << endl;

            return false;

        }
        rear = (rear + 1) % MAX_SIZE;

        passengers[rear].name = name;

        passengers[rear].sourceStop = src;

        passengers[rear].destStop = dest;

        count++;

        return true;

    }

    bool dequeue(PassengerNode& p) {

        if (isEmpty()) {

            cout << "Error: Passenger queue is empty!" << endl;

            return false;

        }
        p = passengers[front];

        front = (front + 1) % MAX_SIZE;

        count--;


        return true;
    }


    PassengerNode peek() {

        PassengerNode empty;

        empty.name = "";

        if (isEmpty())
            return empty;

        return passengers[front];
    }

    int size() {
        return count;
    }

    void display() {


        if (isEmpty()) {



            cout << "No passengers in queue." << endl;
            return;
        }
        cout << "\n=== Passenger Queue (" << count << " passengers) ===" << endl;
        int idx = front;
        for (int i = 0; i < count; i++) {
            cout << (i + 1) << ". " << passengers[idx].name
                << " | From: " << passengers[idx].sourceStop
                << " -> To: " << passengers[idx].destStop << endl;
            idx = (idx + 1) % MAX_SIZE;
        }


    }

    void clear() {

        front = 0;
        rear = -1;
        count = 0;

    }
};

class RouteHistoryStack {

private:
    static const int MAX_SIZE = 500;
    string history[MAX_SIZE];
    int top;

public:
    RouteHistoryStack() : top(-1) {}

    bool isEmpty() {
        return top == -1;

    }
    bool isFull() {
        return top == MAX_SIZE - 1;

    }

    bool push(const string& stopID) {
        if (isFull()) {

            cout << "Error: Route history stack is full!" << endl;
            return false;

        }
        history[++top] = stopID;

        return true;
    }

    bool pop(string& stopID) {

        if (isEmpty()) {

            cout << "Error: Route history stack is empty!" << endl;

            return false;
        }

        stopID = history[top--];
        return true;

    }

    string peek() {

        if (isEmpty())
            return "";

        return history[top];

    }

    void display() {

        if (isEmpty()) {
            cout << "No route history." << endl;

            return;
        }
        cout << "\n=== Route History (most recent first) ===" << endl;

        for (int i = top; i >= 0; i--) {


            cout << (top - i + 1) << ". ";
            cout << history[i];
            cout << endl;
        }
    }

    void clear() {
        top = -1;
    }
};


struct DijkstraNode {

    string stopID;
    double distance;

};
// Applying dijstra for finding the min value for min heap 

class DijkstraMinHeap {
private:
    static const int MAX_SIZE = 500;

    DijkstraNode heap[MAX_SIZE];
    int size;

    void swap(int i, int j) {
        DijkstraNode temp = heap[i];
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
    DijkstraMinHeap() : size(0) {}

    bool isEmpty() { return size == 0; }

    void insert(const string& id, double dist) {
        if (size >= MAX_SIZE) return;
        heap[size].stopID = id;
        heap[size].distance = dist;
        siftUp(size);
        size++;
    }



    DijkstraNode extractMin() {
        DijkstraNode minNode = heap[0];
        heap[0] = heap[size - 1];
        size--;
        if (size > 0) siftDown(0);
        return minNode;
    }

    void clear() { size = 0; }
};



// TRANSPORT SYSTEM MANAGER

class TransportSystem {
private:
    BusStop* stopsHead;
    AdjListNode* adjListHead;
    BusNode* busesHead;
    PassengerQueue passengerQueue;
    RouteHistoryStack routeHistory;

    static const int BUS_TABLE_SIZE = 50;
    BusHashNode* busHashTable[BUS_TABLE_SIZE];

    int stopCount;

    int busCount;

    // PRIVATE HELPER FUNCTIONS

    int busHash(const string& busNumber) {
        return polynomialHash(busNumber, BUS_TABLE_SIZE);
    }

    void addBusToHash(BusNode* bus) {
        int idx = busHash(bus->busNumber);
        BusHashNode* newNode = new BusHashNode(bus->busNumber, bus);
        newNode->next = busHashTable[idx];
        busHashTable[idx] = newNode;
    }

    void removeBusFromHash(const string& busNumber) {
        int idx = busHash(busNumber);
        if (busHashTable[idx] == nullptr)
            return;

        if (stringsEqualIgnoreCase(busHashTable[idx]->key, busNumber)) {
            BusHashNode* temp = busHashTable[idx];
            busHashTable[idx] = busHashTable[idx]->next;
            delete temp;
            return;
        }

        BusHashNode* current = busHashTable[idx];
        while (current->next != nullptr) {
            if (stringsEqualIgnoreCase(current->next->key, busNumber)) {
                BusHashNode* temp = current->next;
                current->next = current->next->next;
                delete temp;
                return;
            }
            current = current->next;
        }
    }

    BusStop* findStopByID(const string& id) {
        BusStop* current = stopsHead;
        while (current != nullptr) {
            if (stringsEqualIgnoreCase(current->stopID, id))
                return current;
            current = current->next;
        }
        return nullptr;
    }

    BusStop* findStopByName(const string& name) {
        BusStop* current = stopsHead;
        while (current != nullptr) {
            if (stringsEqualIgnoreCase(current->name, name))
                return current;
            current = current->next;
        }
        return nullptr;
    }


    AdjListNode* getAdjList(const string& stopID) {
        AdjListNode* current = adjListHead;
        while (current != nullptr) {
            if (stringsEqualIgnoreCase(current->stopID, stopID)) return current;
            current = current->next;
        }
        return nullptr;
    }


    AdjListNode* getOrCreateAdjList(const string& stopID) {
        AdjListNode* existing = getAdjList(stopID);
        if (existing != nullptr) return existing;

        AdjListNode* newNode = new AdjListNode(stopID);
        newNode->next = adjListHead;
        adjListHead = newNode;
        return newNode;
    }

    bool edgeExists(const string& from, const string& to) {
        AdjListNode* adj = getAdjList(from);
        if (adj == nullptr) return false;

        RoadEdge* edge = adj->edgeHead;
        while (edge != nullptr) {
            if (stringsEqualIgnoreCase(edge->destStopID, to)) return true;
            edge = edge->next;
        }
        return false;
    }

    void removeEdgeFromAdjList(AdjListNode* adj, const string& destID) {
        if (adj == nullptr || adj->edgeHead == nullptr) return;

        while (adj->edgeHead != nullptr &&
            stringsEqualIgnoreCase(adj->edgeHead->destStopID, destID)) {
            RoadEdge* temp = adj->edgeHead;
            adj->edgeHead = adj->edgeHead->next;
            delete temp;
        }


        RoadEdge* current = adj->edgeHead;
        while (current != nullptr && current->next != nullptr) {
            if (stringsEqualIgnoreCase(current->next->destStopID, destID)) {
                RoadEdge* temp = current->next;
                current->next = current->next->next;
                delete temp;
            }
            else {
                current = current->next;
            }
        }
    }



    void removeAdjListEntry(const string& stopID) {
        if (adjListHead == nullptr) return;

        if (stringsEqualIgnoreCase(adjListHead->stopID, stopID)) {
            AdjListNode* temp = adjListHead;
            while (temp->edgeHead != nullptr) {
                RoadEdge* edgeTemp = temp->edgeHead;
                temp->edgeHead = temp->edgeHead->next;
                delete edgeTemp;
            }
            adjListHead = adjListHead->next;
            delete temp;
            return;
        }


        AdjListNode* current = adjListHead;
        while (current->next != nullptr) {
            if (stringsEqualIgnoreCase(current->next->stopID, stopID)) {
                AdjListNode* temp = current->next;
                while (temp->edgeHead != nullptr) {
                    RoadEdge* edgeTemp = temp->edgeHead;
                    temp->edgeHead = temp->edgeHead->next;
                    delete edgeTemp;
                }
                current->next = current->next->next;
                delete temp;
                return;
            }
            current = current->next;
        }
    }


    void removeStopFromBusRoute(BusNode* bus, const string& stopID) {
        if (bus->routeHead == nullptr)
            return;

        while (bus->routeHead != nullptr &&
            stringsEqualIgnoreCase(bus->routeHead->stopID, stopID)) {
            RouteStopNode* temp = bus->routeHead;
            bus->routeHead = bus->routeHead->next;
            delete temp;
        }

        RouteStopNode* current = bus->routeHead;
        while (current != nullptr && current->next != nullptr) {
            if (stringsEqualIgnoreCase(current->next->stopID, stopID)) {
                RouteStopNode* temp = current->next;
                current->next = current->next->next;
                delete temp;
            }
            else {
                current = current->next;
            }
        }
    }

    // Helper: Normalize separators 

    void normalizeSeparatorsHelper(string& s) {
        for (size_t i = 0; i < s.length(); i++) {
            if (s[i] == '\t') s[i] = ',';
        }
    }

    // Helper: Strip quotes from string
    void stripQuotesHelper(string& s) {
        s = trimString(s);
        if (!s.empty() && s[0] == '"') s = s.substr(1);
        if (!s.empty() && s[s.length() - 1] == '"')
            s = s.substr(0, s.length() - 1);
    }

    // Generate roads from bus routes automatically
    void generateRoadsFromBusRoutes() {
        cout << "Generating road network from bus routes..." << endl;
        int roadsCreated = 0;

        BusNode* bus = busesHead;
        while (bus != nullptr) {
            RouteStopNode* current = bus->routeHead;


            while (current != nullptr && current->next != nullptr) {
                string fromStop = current->stopID;
                string toStop = current->next->stopID;

                BusStop* stop1 = findStopByID(fromStop);

                BusStop* stop2 = findStopByID(toStop);


                if (stop1 != nullptr && stop2 != nullptr && !edgeExists(fromStop, toStop)) {
                    double distance = calculateDistance(stop1->lat, stop1->lon,
                        stop2->lat, stop2->lon);


                    AdjListNode* adj1 = getOrCreateAdjList(fromStop);
                    RoadEdge* edge1 = new RoadEdge(toStop, distance);
                    edge1->next = adj1->edgeHead;
                    adj1->edgeHead = edge1;

                    AdjListNode* adj2 = getOrCreateAdjList(toStop);
                    RoadEdge* edge2 = new RoadEdge(fromStop, distance);
                    edge2->next = adj2->edgeHead;
                    adj2->edgeHead = edge2;


                    CityGraph::getInstance()->connectLocations(fromStop, toStop, distance);

                    roadsCreated++;
                }

                current = current->next;
            }

            bus = bus->next;
        }

        if (roadsCreated > 0) {
            cout << "  Created " << roadsCreated << " road connections from bus routes." << endl;
        }
    }

public:
    TransportSystem() : stopsHead(nullptr), adjListHead(nullptr), busesHead(nullptr),
        stopCount(0), busCount(0) {
        for (int i = 0; i < BUS_TABLE_SIZE; i++) {
            busHashTable[i] = nullptr;
        }
    }

    // BUS STOP OPERATIONS 



    // Export bus stops and road network for visualization

    int exportRoadNetwork(string stopNames[], double lats[], double lons[],
        bool adjacencyMatrix[][50], double weights[][50],

        int maxStops) {
        int count = 0;

        // Build stop ID to index mapping
        string stopIDs[200];
        BusStop* stop = stopsHead;



        while (stop != nullptr && count < maxStops) {
            stopIDs[count] = stop->stopID;
            stopNames[count] = stop->name;

            lats[count] = stop->lat;

            lons[count] = stop->lon;
            count++;
            stop = stop->next;
        }

        // Initialize adjacency matrix
        for (int i = 0; i < count; i++) {


            for (int j = 0; j < 50; j++) {

                adjacencyMatrix[i][j] = false;
                weights[i][j] = 0.0;
            }
        }

        // Fill adjacency matrix
        for (int i = 0; i < count; i++) {
            AdjListNode* adj = getAdjList(stopIDs[i]);

            if (adj != nullptr) {
                RoadEdge* edge = adj->edgeHead;
                while (edge != nullptr) {

                    // Find destination index
                    for (int j = 0; j < count; j++) {
                        if (stringsEqualIgnoreCase(stopIDs[j], edge->destStopID)) {
                            adjacencyMatrix[i][j] = true;
                            weights[i][j] = edge->distance;
                            break;

                        }
                    }
                    edge = edge->next;
                }

            }
        }

        return count;
    }

    // Export specific bus route
    bool exportBusRoute(const string& busNumber, string& company,
        string stopNames[], double lats[], double lons[],
        int& stopCount, int& currentStopIdx, int maxStops) {
        BusNode* bus = searchBusByNumber(busNumber);
        if (bus == nullptr)
            return false;

        company = bus->company;
        stopCount = 0;
        currentStopIdx = -1;

        RouteStopNode* route = bus->routeHead;

        while (route != nullptr && stopCount < maxStops) {
            BusStop* stop = findStopByID(route->stopID);
            if (stop != nullptr) {
                stopNames[stopCount] = stop->name;

                lats[stopCount] = stop->lat;

                lons[stopCount] = stop->lon;

                // Check if this is current stop
                if (stringsEqualIgnoreCase(route->stopID, bus->currentStopID)) {

                    currentStopIdx = stopCount;

                }

                stopCount++;
            }
            route = route->next;
        }

        return stopCount > 0;
    }

    void addBusStop() {
        cout << "\n=== Add Bus Stop ===" << endl;

        string id = getStringInput("Enter Stop ID (e.g., Stop1): ");
        if (id.empty()) {
            cout << "Error: Stop ID cannot be empty." << endl;
            return;
        }

        if (findStopByID(id) != nullptr) {
            cout << "Error: Stop with ID '" << id << "' already exists!" << endl;
            return;
        }

        string name = getStringInput("Enter Stop Name: ");
        if (name.empty()) {
            cout << "Error: Stop name cannot be empty." << endl;
            return;
        }

        if (findStopByName(name) != nullptr) {
            cout << "Error: Stop with name '" << name << "' already exists!" << endl;
            return;
        }

        double lat = getDoubleInput("Enter Latitude: ");
        double lon = getDoubleInput("Enter Longitude: ");

        GlobalLocationManager* locMgr = GlobalLocationManager::getInstance();
        if (!locMgr->registerLocation(lat, lon, "BusStop", name)) {
            return;
        }

        BusStop* newStop = new BusStop(id, name, lat, lon);
        newStop->next = stopsHead;
        stopsHead = newStop;
        stopCount++;

        getOrCreateAdjList(id);

        // Add to city graph
        CityGraph::getInstance()->addLocation(id, name, LOC_BUS_STOP, lat, lon, newStop);

        cout << "Bus stop '" << name << "' added successfully!" << endl;
    }

    void deleteBusStop() {
        cout << "\n=== Delete Bus Stop ===" << endl;

        string id = getStringInput("Enter Stop ID to delete: ");
        BusStop* stopToDelete = findStopByID(id);

        if (stopToDelete == nullptr) {
            cout << "Error: Stop '" << id << "' not found!" << endl;
            return;
        }



        // Check if any bus is currently at this stop
        BusNode* bus = busesHead;
        while (bus != nullptr) {
            if (stringsEqualIgnoreCase(bus->currentStopID, id)) {
                cout << "Error: Bus '" << bus->busNumber << "' is currently at this stop.  "
                    << "Please move the bus first." << endl;
                return;
            }
            bus = bus->next;
        }



        // Confirmation
        cout << "Warning: This will remove stop '" << stopToDelete->name << "' and all its connections." << endl;
        string confirm = getStringInput("Type 'YES' to confirm: ");
        if (confirm != "YES") {
            cout << "Deletion cancelled." << endl;
            return;
        }



        // Remove from all bus routes
        bus = busesHead;
        while (bus != nullptr) {
            removeStopFromBusRoute(bus, id);
            bus = bus->next;
        }



        // Remove all edges TO this stop from other stops
        AdjListNode* adj = adjListHead;
        while (adj != nullptr) {
            removeEdgeFromAdjList(adj, id);
            adj = adj->next;
        }




        removeAdjListEntry(id);



        if (stringsEqualIgnoreCase(stopsHead->stopID, id)) {
            BusStop* temp = stopsHead;
            stopsHead = stopsHead->next;
            GlobalLocationManager::getInstance()->removeLocation(temp->lat, temp->lon);
            CityGraph::getInstance()->removeLocation(id);
            delete temp;
        }
        else {
            BusStop* current = stopsHead;
            while (current->next != nullptr) {

                if (stringsEqualIgnoreCase(current->next->stopID, id)) {

                    BusStop* temp = current->next;
                    current->next = current->next->next;

                    GlobalLocationManager::getInstance()->removeLocation(temp->lat, temp->lon);

                    CityGraph::getInstance()->removeLocation(id);
                    delete temp;
                    break;
                }
                current = current->next;
            }
        }

        stopCount--;
        cout << "Bus stop deleted successfully!" << endl;
    }


    void updateBusStop() {


        cout << "\n=== Update Bus Stop ===" << endl;


        string id = getStringInput("Enter Stop ID to update: ");
        BusStop* stop = findStopByID(id);

        if (stop == nullptr) {
            cout << "Error: Stop '" << id << "' not found!" << endl;

            return;
        }

        cout << "Current Name: " << stop->name << endl;

        cout << "Current Location: (" << stop->lat << ", " << stop->lon << ")" << endl;

        cout << "\nWhat would you like to update?" << endl;

        cout << "1. Name only" << endl;

        cout << "2. Location only" << endl;

        cout << "3.  Both" << endl;

        cout << "0. Cancel" << endl;

        int choice = getIntInput("Enter choice: ");

        if (choice == 0) {
            cout << "Update cancelled." << endl;
            return;
        }

        if (choice == 1 || choice == 3) {

            string newName = getStringInput("Enter new name (or press Enter to keep current): ");
            if (!newName.empty()) {

                if (findStopByName(newName) != nullptr && !stringsEqualIgnoreCase(newName, stop->name)) {
                    cout << "Error: A stop with name '" << newName << "' already exists!" << endl;
                    return;
                }

                stop->name = newName;
            }
        }

        if (choice == 2 || choice == 3) {
            double newLat = getDoubleInput("Enter new latitude: ");

            double newLon = getDoubleInput("Enter new longitude: ");

            GlobalLocationManager* locMgr = GlobalLocationManager::getInstance();
            locMgr->removeLocation(stop->lat, stop->lon);

            if (!locMgr->registerLocation(newLat, newLon, "BusStop", stop->name)) {

                // Re-register old location
                locMgr->registerLocation(stop->lat, stop->lon, "BusStop", stop->name);

                cout << "Error: New location is occupied.  Update cancelled." << endl;

                return;
            }

            stop->lat = newLat;

            stop->lon = newLon;

            // Update distances to connected stops

            AdjListNode* adj = getAdjList(id);
            if (adj != nullptr) {
                RoadEdge* edge = adj->edgeHead;
                while (edge != nullptr) {
                    BusStop* destStop = findStopByID(edge->destStopID);

                    if (destStop != nullptr) {
                        edge->distance = calculateDistance(stop->lat, stop->lon,
                            destStop->lat, destStop->lon);
                        // Update reverse edge

                        AdjListNode* destAdj = getAdjList(edge->destStopID);
                        if (destAdj != nullptr) {
                            RoadEdge* revEdge = destAdj->edgeHead;
                            while (revEdge != nullptr) {
                                if (stringsEqualIgnoreCase(revEdge->destStopID, id)) {
                                    revEdge->distance = edge->distance;

                                    break;
                                }
                                revEdge = revEdge->next;
                            }
                        }
                    }
                    edge = edge->next;

                }
            }
        }

        cout << "Bus stop updated successfully!" << endl;

    }

    void connectStops() {
        cout << "\n=== Connect Bus Stops (Add Road) ===" << endl;

        if (stopCount < 2) {
            cout << "Error: Need at least 2 stops to create a connection." << endl;
            return;
        }



        string id1 = getStringInput("Enter First Stop ID: ");
        BusStop* stop1 = findStopByID(id1);
        if (stop1 == nullptr) {
            cout << "Error: Stop '" << id1 << "' not found!" << endl;
            return;
        }

        string id2 = getStringInput("Enter Second Stop ID: ");
        BusStop* stop2 = findStopByID(id2);
        if (stop2 == nullptr) {
            cout << "Error: Stop '" << id2 << "' not found!" << endl;
            return;
        }



        if (stringsEqualIgnoreCase(id1, id2)) {
            cout << "Error: Cannot connect a stop to itself!" << endl;
            return;
        }


        if (edgeExists(id1, id2)) {
            cout << "Error: These stops are already connected!" << endl;
            return;
        }



        double distance = calculateDistance(stop1->lat, stop1->lon, stop2->lat, stop2->lon);


        AdjListNode* adj1 = getOrCreateAdjList(id1);
        RoadEdge* edge1 = new RoadEdge(id2, distance);
        edge1->next = adj1->edgeHead;
        adj1->edgeHead = edge1;


        AdjListNode* adj2 = getOrCreateAdjList(id2);
        RoadEdge* edge2 = new RoadEdge(id1, distance);
        edge2->next = adj2->edgeHead;
        adj2->edgeHead = edge2;



        // Connect in city graph too
        CityGraph::getInstance()->connectLocations(id1, id2, distance);

        cout << "Connected '" << stop1->name << "' <--> '" << stop2->name

            << "' (Distance: " << distance << " units)" << endl;

    }

    void disconnectStops() {
        cout << "\n=== Disconnect Stops (Remove Road) ===" << endl;




        string id1 = getStringInput("Enter First Stop ID: ");
        if (findStopByID(id1) == nullptr) {
            cout << "Error: Stop '" << id1 << "' not found!" << endl;
            return;
        }



        string id2 = getStringInput("Enter Second Stop ID: ");
        if (findStopByID(id2) == nullptr) {
            cout << "Error: Stop '" << id2 << "' not found!" << endl;
            return;
        }



        if (!edgeExists(id1, id2)) {
            cout << "Error: These stops are not connected!" << endl;
            return;
        }


        AdjListNode* adj1 = getAdjList(id1);
        if (adj1 != nullptr) {
            removeEdgeFromAdjList(adj1, id2);
        }



        AdjListNode* adj2 = getAdjList(id2);
        if (adj2 != nullptr) {
            removeEdgeFromAdjList(adj2, id1);
        }



        CityGraph::getInstance()->removeRoad(id1, id2);

        BusStop* stop1 = findStopByID(id1);
        BusStop* stop2 = findStopByID(id2);
        cout << "Disconnected '" << (stop1 ? stop1->name : id1)
            << "' from '" << (stop2 ? stop2->name : id2) << "'!" << endl;

    }

    // BUS OPERATIONS 

    void registerBus() {
        cout << "\n=== Register New Bus ===" << endl;

        string busNum = getStringInput("Enter Bus Number: ");
        if (busNum.empty()) {
            cout << "Error: Bus number cannot be empty." << endl;
            return;
        }

        if (searchBusByNumber(busNum) != nullptr) {
            cout << "Error: Bus '" << busNum << "' already exists!" << endl;
            return;
        }

        string company = getStringInput("Enter Company Name: ");
        if (company.empty()) {
            cout << "Error: Company name cannot be empty." << endl;
            return;
        }



        BusNode* newBus = new BusNode(busNum, company);
        newBus->next = busesHead;
        busesHead = newBus;
        busCount++;

        addBusToHash(newBus);

        cout << "Bus '" << busNum << "' registered successfully!" << endl;
    }



    void deleteBus() {
        cout << "\n=== Delete Bus ===" << endl;

        string busNum = getStringInput("Enter Bus Number to delete: ");
        BusNode* bus = searchBusByNumber(busNum);

        if (bus == nullptr) {
            cout << "Error: Bus '" << busNum << "' not found!" << endl;
            return;
        }



        string confirm = getStringInput("Type 'YES' to confirm deletion of bus '" + busNum + "': ");
        if (confirm != "YES") {
            cout << "Deletion cancelled." << endl;
            return;
        }


        removeBusFromHash(busNum);



        // Remove from buses linked list
        if (stringsEqualIgnoreCase(busesHead->busNumber, busNum)) {
            BusNode* temp = busesHead;
            busesHead = busesHead->next;
            while (temp->routeHead != nullptr) {
                RouteStopNode* routeTemp = temp->routeHead;
                temp->routeHead = temp->routeHead->next;
                delete routeTemp;
            }
            delete temp;


        }
        else {
            BusNode* current = busesHead;
            while (current->next != nullptr) {
                if (stringsEqualIgnoreCase(current->next->busNumber, busNum)) {
                    BusNode* temp = current->next;
                    current->next = current->next->next;
                    while (temp->routeHead != nullptr) {
                        RouteStopNode* routeTemp = temp->routeHead;
                        temp->routeHead = temp->routeHead->next;
                        delete routeTemp;

                    }
                    delete temp;
                    break;

                }
                current = current->next;
            }
        }


        busCount--;
        cout << "Bus '" << busNum << "' deleted successfully!" << endl;
    }


    void updateBus() {
        cout << "\n=== Update Bus ===" << endl;

        string busNum = getStringInput("Enter Bus Number to update: ");
        BusNode* bus = searchBusByNumber(busNum);

        if (bus == nullptr) {
            cout << "Error: Bus '" << busNum << "' not found!" << endl;
            return;
        }


        cout << "Current Company: " << bus->company << endl;

        string newCompany = getStringInput("Enter new company name (or press Enter to keep current): ");
        if (!newCompany.empty()) {
            bus->company = newCompany;
            cout << "Bus company updated successfully!" << endl;
        }
        else {
            cout << "No changes made." << endl;
        }
    }

    void addStopToBusRoute() {
        cout << "\n=== Add Stop to Bus Route ===" << endl;

        string busNum = getStringInput("Enter Bus Number: ");
        BusNode* bus = searchBusByNumber(busNum);
        if (bus == nullptr) {
            cout << "Error: Bus not found!" << endl;
            return;
        }

        string stopID = getStringInput("Enter Stop ID to add: ");
        BusStop* stop = findStopByID(stopID);
        if (stop == nullptr) {
            cout << "Error: Stop not found!" << endl;
            return;
        }

        RouteStopNode* check = bus->routeHead;
        while (check != nullptr) {
            if (stringsEqualIgnoreCase(check->stopID, stopID)) {
                cout << "Error: Stop already in this bus's route!" << endl;
                return;
            }
            check = check->next;
        }

        RouteStopNode* newRouteStop = new RouteStopNode(stopID);
        if (bus->routeHead == nullptr) {
            bus->routeHead = newRouteStop;
            bus->currentStopID = stopID;
        }
        else {
            RouteStopNode* temp = bus->routeHead;
            while (temp->next != nullptr) {
                temp = temp->next;
            }
            temp->next = newRouteStop;
        }

        cout << "Stop '" << stop->name << "' added to bus '" << busNum << "' route!" << endl;
    }

    void removeStopFromRoute() {
        cout << "\n=== Remove Stop from Bus Route ===" << endl;

        string busNum = getStringInput("Enter Bus Number: ");
        BusNode* bus = searchBusByNumber(busNum);
        if (bus == nullptr) {
            cout << "Error: Bus not found!" << endl;
            return;
        }

        if (bus->routeHead == nullptr) {
            cout << "Error: Bus has no route!" << endl;
            return;
        }

        // Display current route
        cout << "Current Route: ";
        RouteStopNode* temp = bus->routeHead;
        while (temp != nullptr) {
            BusStop* s = findStopByID(temp->stopID);
            cout << (s ? s->name : temp->stopID);
            if (temp->next != nullptr) cout << " -> ";
            temp = temp->next;
        }
        cout << endl;

        string stopID = getStringInput("Enter Stop ID to remove: ");

        bool found = false;
        temp = bus->routeHead;
        while (temp != nullptr) {
            if (stringsEqualIgnoreCase(temp->stopID, stopID)) {
                found = true;
                break;
            }
            temp = temp->next;
        }

        if (!found) {
            cout << "Error: Stop not in this bus's route!" << endl;
            return;
        }

        removeStopFromBusRoute(bus, stopID);

        // Update current stop if it was the removed stop
        if (stringsEqualIgnoreCase(bus->currentStopID, stopID)) {
            if (bus->routeHead != nullptr) {
                bus->currentStopID = bus->routeHead->stopID;
            }
            else {
                bus->currentStopID = "";
            }
        }

        cout << "Stop removed from route successfully!" << endl;
    }

    void updateBusLocation() {
        cout << "\n=== Update Bus Location ===" << endl;

        string busNum = getStringInput("Enter Bus Number: ");
        BusNode* bus = searchBusByNumber(busNum);
        if (bus == nullptr) {
            cout << "Error: Bus not found!" << endl;
            return;
        }

        string stopID = getStringInput("Enter Current Stop ID: ");
        BusStop* stop = findStopByID(stopID);
        if (stop == nullptr) {
            cout << "Error: Stop not found!" << endl;
            return;
        }

        RouteStopNode* check = bus->routeHead;
        bool inRoute = false;
        while (check != nullptr) {
            if (stringsEqualIgnoreCase(check->stopID, stopID)) {
                inRoute = true;
                break;
            }
            check = check->next;
        }

        if (!inRoute) {
            cout << "Warning: This stop is not in the bus's registered route." << endl;
        }

        bus->currentStopID = stopID;
        cout << "Bus '" << busNum << "' location updated to '" << stop->name << "'!" << endl;
    }

    BusNode* searchBusByNumber(const string& busNum) {
        int idx = busHash(busNum);
        BusHashNode* current = busHashTable[idx];
        while (current != nullptr) {
            if (stringsEqualIgnoreCase(current->key, busNum)) {
                return current->busRef;
            }
            current = current->next;
        }
        return nullptr;
    }

    void searchBus() {
        cout << "\n=== Search Bus by Number ===" << endl;
        string busNum = getStringInput("Enter Bus Number: ");

        BusNode* bus = searchBusByNumber(busNum);
        if (bus == nullptr) {
            cout << "Bus not found!" << endl;
            return;
        }

        cout << "\n--- Bus Details ---" << endl;
        cout << "Bus Number: " << bus->busNumber << endl;
        cout << "Company: " << bus->company << endl;

        if (!bus->currentStopID.empty()) {
            BusStop* currentStop = findStopByID(bus->currentStopID);
            cout << "Current Location: " << (currentStop ? currentStop->name : bus->currentStopID) << endl;
        }
        else {
            cout << "Current Location: Not set" << endl;
        }

        cout << "Route: ";
        RouteStopNode* route = bus->routeHead;
        if (route == nullptr) {
            cout << "No route assigned" << endl;
        }
        else {
            bool first = true;
            while (route != nullptr) {
                if (!first) cout << " -> ";
                BusStop* s = findStopByID(route->stopID);
                cout << (s ? s->name : route->stopID);
                first = false;
                route = route->next;
            }
            cout << endl;
        }
    }

    void findNearestBus() {
        cout << "\n=== Find Nearest Bus ===" << endl;

        if (busesHead == nullptr) {
            cout << "No buses registered." << endl;
            return;
        }

        double userLat = getDoubleInput("Enter your Latitude: ");
        double userLon = getDoubleInput("Enter your Longitude: ");

        BusNode* nearestBus = nullptr;
        double minDist = DBL_MAX;

        BusNode* bus = busesHead;
        while (bus != nullptr) {
            if (!bus->currentStopID.empty()) {
                BusStop* stop = findStopByID(bus->currentStopID);
                if (stop != nullptr) {
                    double dist = calculateDistance(userLat, userLon, stop->lat, stop->lon);
                    if (dist < minDist) {
                        minDist = dist;
                        nearestBus = bus;
                    }
                }
            }
            bus = bus->next;
        }

        if (nearestBus != nullptr) {
            BusStop* stop = findStopByID(nearestBus->currentStopID);
            cout << "\nNearest Bus: " << nearestBus->busNumber << endl;
            cout << "Company: " << nearestBus->company << endl;
            cout << "Current Location: " << (stop ? stop->name : nearestBus->currentStopID) << endl;
            cout << "Distance: " << minDist << " units" << endl;
        }
        else {
            cout << "No buses with location information available." << endl;
        }
    }

    // DIJKSTRA'S ALGORITHM - SHORTEST PATH

    void findShortestPath() {
        cout << "\n=== Find Shortest Path (Dijkstra's Algorithm) ===" << endl;

        if (stopCount < 2) {
            cout << "Error: Need at least 2 stops." << endl;
            return;
        }

        string srcID = getStringInput("Enter Source Stop ID: ");
        BusStop* srcStop = findStopByID(srcID);
        if (srcStop == nullptr) {
            cout << "Error: Source stop not found!" << endl;
            return;
        }

        string destID = getStringInput("Enter Destination Stop ID: ");
        BusStop* destStop = findStopByID(destID);
        if (destStop == nullptr) {
            cout << "Error: Destination stop not found!" << endl;
            return;
        }

        if (stringsEqualIgnoreCase(srcID, destID)) {
            cout << "You are already at the destination!" << endl;
            return;
        }

        // Initialize distances and visited arrays
        const int MAX_STOPS = 500;
        string stopIDs[MAX_STOPS];
        double distances[MAX_STOPS];
        string previous[MAX_STOPS];
        bool visited[MAX_STOPS];
        int numStops = 0;

        // Collect all stop IDs
        BusStop* current = stopsHead;
        while (current != nullptr && numStops < MAX_STOPS) {
            stopIDs[numStops] = current->stopID;
            distances[numStops] = DBL_MAX;
            previous[numStops] = "";
            visited[numStops] = false;
            numStops++;
            current = current->next;
        }

        // Find source index
        int srcIdx = -1;
        for (int i = 0; i < numStops; i++) {
            if (stringsEqualIgnoreCase(stopIDs[i], srcID)) {
                srcIdx = i;
                break;
            }
        }
        if (srcIdx == -1) {
            cout << "Error: Source stop not in graph!" << endl;
            return;
        }

        distances[srcIdx] = 0;

        // Dijkstra's algorithm using min-heap
        DijkstraMinHeap minHeap;
        minHeap.insert(srcID, 0);

        while (!minHeap.isEmpty()) {
            DijkstraNode minNode = minHeap.extractMin();
            string currentID = minNode.stopID;

            // Find index of current stop
            int currentIdx = -1;
            for (int i = 0; i < numStops; i++) {
                if (stringsEqualIgnoreCase(stopIDs[i], currentID)) {
                    currentIdx = i;
                    break;
                }
            }

            if (currentIdx == -1 || visited[currentIdx]) continue;
            visited[currentIdx] = true;

            // Process all neighbors
            AdjListNode* adj = getAdjList(currentID);
            if (adj != nullptr) {
                RoadEdge* edge = adj->edgeHead;
                while (edge != nullptr) {
                    // Find neighbor index
                    int neighborIdx = -1;
                    for (int i = 0; i < numStops; i++) {
                        if (stringsEqualIgnoreCase(stopIDs[i], edge->destStopID)) {
                            neighborIdx = i;
                            break;
                        }
                    }

                    if (neighborIdx != -1 && !visited[neighborIdx]) {
                        double newDist = distances[currentIdx] + edge->distance;
                        if (newDist < distances[neighborIdx]) {
                            distances[neighborIdx] = newDist;
                            previous[neighborIdx] = currentID;
                            minHeap.insert(edge->destStopID, newDist);
                        }
                    }
                    edge = edge->next;
                }
            }
        }

        // Find destination index
        int destIdx = -1;
        for (int i = 0; i < numStops; i++) {
            if (stringsEqualIgnoreCase(stopIDs[i], destID)) {
                destIdx = i;
                break;
            }
        }

        if (destIdx == -1 || distances[destIdx] == DBL_MAX) {
            cout << "\nNo path exists between '" << srcStop->name << "' and '" << destStop->name << "'!" << endl;
            return;
        }

        // Reconstruct path using stack
        RouteHistoryStack pathStack;
        string currentID = destID;
        while (!currentID.empty()) {
            pathStack.push(currentID);
            int idx = -1;
            for (int i = 0; i < numStops; i++) {
                if (stringsEqualIgnoreCase(stopIDs[i], currentID)) {
                    idx = i;
                    break;
                }
            }
            if (idx == -1) break;
            currentID = previous[idx];
        }

        // Display path
        cout << "\n=== Shortest Path Found ===" << endl;
        cout << "From: " << srcStop->name << endl;
        cout << "To: " << destStop->name << endl;
        cout << "Total Distance: " << distances[destIdx] << " units" << endl;
        cout << "\nPath: ";

        string stopOnPath;
        bool first = true;
        while (!pathStack.isEmpty()) {
            pathStack.pop(stopOnPath);
            BusStop* s = findStopByID(stopOnPath);
            if (s != nullptr) {
                if (!first) cout << " -> ";
                cout << s->name;
                first = false;

                // Add to route history
                routeHistory.push(stopOnPath);
            }
        }
        cout << endl;
    }


    // PASSENGER QUEUE OPERATIONS


    void addPassenger() {
        cout << "\n=== Add Passenger to Queue ===" << endl;

        string name = getStringInput("Enter Passenger Name: ");
        if (name.empty()) {
            cout << "Error: Name cannot be empty." << endl;
            return;
        }

        string srcStop = getStringInput("Enter Source Stop ID: ");
        if (findStopByID(srcStop) == nullptr) {
            cout << "Error: Source stop not found!" << endl;
            return;
        }

        string destStop = getStringInput("Enter Destination Stop ID: ");
        if (findStopByID(destStop) == nullptr) {
            cout << "Error: Destination stop not found!" << endl;
            return;
        }

        if (passengerQueue.enqueue(name, srcStop, destStop)) {
            cout << "Passenger '" << name << "' added to queue!" << endl;
        }
    }

    void removePassengerFromQueue() {
        cout << "\n=== Remove Passenger from Queue ===" << endl;

        if (passengerQueue.isEmpty()) {
            cout << "Error: Queue is empty!" << endl;
            return;
        }

        string name = getStringInput("Enter Passenger Name to remove: ");

        PassengerQueue tempQueue;
        bool found = false;
        int originalSize = passengerQueue.size();

        for (int i = 0; i < originalSize; i++) {
            PassengerNode p;
            passengerQueue.dequeue(p);
            if (stringsEqualIgnoreCase(p.name, name) && !found) {
                found = true;
                cout << "Passenger '" << name << "' removed from queue!" << endl;
            }
            else {
                tempQueue.enqueue(p.name, p.sourceStop, p.destStop);
            }
        }

        // Restore queue
        int tempSize = tempQueue.size();
        for (int i = 0; i < tempSize; i++) {
            PassengerNode p;
            tempQueue.dequeue(p);
            passengerQueue.enqueue(p.name, p.sourceStop, p.destStop);
        }

        if (!found) {
            cout << "Passenger '" << name << "' not found in queue." << endl;
        }
    }

    void processNextPassenger() {
        cout << "\n=== Process Next Passenger ===" << endl;

        PassengerNode p;
        if (passengerQueue.dequeue(p)) {
            BusStop* src = findStopByID(p.sourceStop);
            BusStop* dest = findStopByID(p.destStop);

            cout << "Processing: " << p.name << endl;
            cout << "From: " << (src ? src->name : p.sourceStop) << endl;
            cout << "To: " << (dest ? dest->name : p.destStop) << endl;
        }
    }

    void viewPassengerQueue() {
        passengerQueue.display();
    }

    void clearPassengerQueue() {
        passengerQueue.clear();
        cout << "Passenger queue cleared!" << endl;
    }

    // ROUTE HISTORY STACK

    void viewRouteHistory() {
        routeHistory.display();
    }

    void clearRouteHistory() {
        routeHistory.clear();
        cout << "Route history cleared!" << endl;
    }

    // DISPLAY FUNCTIONS

    void displayAllStops() {
        cout << "\n=== All Bus Stops ===" << endl;
        BusStop* current = stopsHead;
        int count = 0;
        while (current != nullptr) {
            cout << ++count << ". [" << current->stopID << "] " << current->name
                << " at (" << current->lat << ", " << current->lon << ")" << endl;
            current = current->next;
        }
        if (count == 0) cout << "No bus stops registered." << endl;
    }

    void displayGraph() {
        cout << "\n=== Road Network (Adjacency List) ===" << endl;
        AdjListNode* adj = adjListHead;
        while (adj != nullptr) {
            BusStop* stop = findStopByID(adj->stopID);
            cout << "[" << (stop ? stop->name : adj->stopID) << "] -> ";

            RoadEdge* edge = adj->edgeHead;
            if (edge == nullptr) {
                cout << "(no connections)";
            }
            while (edge != nullptr) {
                BusStop* dest = findStopByID(edge->destStopID);
                cout << (dest ? dest->name : edge->destStopID) << "(" << edge->distance << ")";
                if (edge->next != nullptr) cout << ", ";
                edge = edge->next;
            }
            cout << endl;
            adj = adj->next;
        }
    }

    void displayAllBuses() {
        cout << "\n=== All Buses ===" << endl;
        BusNode* bus = busesHead;
        int count = 0;
        while (bus != nullptr) {
            cout << ++count << ". " << bus->busNumber << " [" << bus->company << "]";
            if (bus->isSchoolBus) {
                cout << " [SCHOOL BUS -> " << bus->schoolName << "]";
            }
            if (!bus->currentStopID.empty()) {
                BusStop* stop = findStopByID(bus->currentStopID);
                cout << " - Currently at: " << (stop ? stop->name : bus->currentStopID);
            }
            cout << endl;

            // Display route
            cout << "   Route: ";
            RouteStopNode* route = bus->routeHead;
            if (route == nullptr) {
                cout << "(No route)";
            }
            else {
                bool first = true;
                while (route != nullptr) {
                    if (!first) cout << " -> ";
                    BusStop* s = findStopByID(route->stopID);
                    cout << (s ? s->name : route->stopID);
                    first = false;
                    route = route->next;
                }
                if (bus->isSchoolBus) {
                    cout << " -> " << bus->schoolName << " (School)";
                }
            }
            cout << endl;

            bus = bus->next;
        }
        if (count == 0) cout << "No buses registered." << endl;
    }

    void displayStatistics() {
        cout << "\n=== Transport System Statistics ===" << endl;
        cout << "Total Bus Stops: " << stopCount << endl;
        cout << "Total Buses: " << busCount << endl;
        cout << "Passengers in Queue: " << passengerQueue.size() << endl;

        // Count school buses
        int schoolBusCount = 0;
        BusNode* bus = busesHead;
        while (bus != nullptr) {
            if (bus->isSchoolBus) schoolBusCount++;
            bus = bus->next;
        }
        cout << "School Buses: " << schoolBusCount << endl;

        // Count roads
        int roadCount = 0;
        AdjListNode* adj = adjListHead;
        while (adj != nullptr) {
            RoadEdge* edge = adj->edgeHead;
            while (edge != nullptr) {
                roadCount++;
                edge = edge->next;
            }
            adj = adj->next;
        }
        cout << "Total Road Connections: " << (roadCount / 2) << " (bidirectional)" << endl;
    }

    //school bus 

    void registerSchoolBus() {
        cout << "\n=== Register School Bus ===" << endl;

        // First, check if bus exists
        string busNum = getStringInput("Enter Bus Number: ");
        BusNode* bus = searchBusByNumber(busNum);
        if (bus == nullptr) {
            cout << "Error: Bus '" << busNum << "' not found!" << endl;
            cout << "Please register the bus first (Menu -> 10: Register Bus)" << endl;
            return;
        }


        if (bus->routeHead == nullptr) {
            cout << "Error: Bus has no route assigned!" << endl;
            cout << "Please add stops to bus route first (Menu -> 14: Add Stop to Bus Route)" << endl;
            return;
        }

        // Get school name

        string schoolName = getStringInput("Enter School Name (destination): ");
        if (schoolName.empty()) {
            cout << "Error: School name cannot be empty." << endl;
            return;
        }

        // Mark as school bus
        bus->isSchoolBus = true;
        bus->schoolName = schoolName;




        cout << "\n========================================" << endl;
        cout << "  SCHOOL BUS REGISTERED SUCCESSFULLY" << endl;
        cout << "========================================" << endl;
        cout << "Bus Number: " << bus->busNumber << endl;
        cout << "Company: " << bus->company << endl;
        cout << "Destination School: " << schoolName << endl;
        cout << "Route: ";
        RouteStopNode* route = bus->routeHead;
        bool first = true;
        int routeLength = 0;
        while (route != nullptr) {
            if (!first) cout << " -> ";
            BusStop* s = findStopByID(route->stopID);
            cout << (s ? s->name : route->stopID);
            first = false;
            route = route->next;
            routeLength++;
        }
        cout << endl;
        cout << "Total Stops: " << routeLength << endl;
        cout << "========================================" << endl;
        cout << "\nNote: The last stop in the route should be near the school." << endl;
        cout << "Use 'Simulate School Bus' to move the bus along its route." << endl;
    }

    void simulateSchoolBus() {
        cout << "\n=== Simulate School Bus Movement ===" << endl;

        string busNum = getStringInput("Enter School Bus Number: ");
        BusNode* bus = searchBusByNumber(busNum);
        if (bus == nullptr) {
            cout << "Error: Bus not found!" << endl;
            return;
        }

        if (!bus->isSchoolBus) {
            cout << "Error: This bus is not registered as a school bus!" << endl;
            cout << "Please register it as a school bus first (Menu -> 35: Register School Bus)" << endl;
            return;
        }

        if (bus->routeHead == nullptr) {
            cout << "Error: Bus has no route!" << endl;
            return;
        }

        // Find current position in route


        RouteStopNode* currentRouteNode = nullptr;
        RouteStopNode* route = bus->routeHead;
        bool foundCurrent = false;

        if (bus->currentStopID.empty()) {
            // Bus hasn't started - start at first stop


            currentRouteNode = bus->routeHead;
            bus->currentStopID = currentRouteNode->stopID;
            foundCurrent = true;
        }
        else {
            // Find current stop in route
            while (route != nullptr) {
                if (stringsEqualIgnoreCase(route->stopID, bus->currentStopID)) {
                    currentRouteNode = route;
                    foundCurrent = true;
                    break;
                }
                route = route->next;
            }
        }

        if (!foundCurrent) {
            cout << "Warning: Current stop not found in route. Resetting to first stop." << endl;
            currentRouteNode = bus->routeHead;
            bus->currentStopID = currentRouteNode->stopID;
        }

        // Move to next stop
        RouteStopNode* nextStop = currentRouteNode->next;

        if (nextStop == nullptr) {
            // Reached end of route - bus arrived at school
            BusStop* currentStop = findStopByID(bus->currentStopID);
            string currentStopName = currentStop ? currentStop->name : bus->currentStopID;

            cout << "\n========================================" << endl;
            cout << "  SCHOOL BUS REACHED DESTINATION!" << endl;
            cout << "========================================" << endl;
            cout << "Bus Number: " << bus->busNumber << endl;
            cout << "Current Location: " << currentStopName << endl;
            cout << "Destination School: " << bus->schoolName << endl;
            cout << "Status: ARRIVED AT SCHOOL" << endl;
            cout << "========================================" << endl;
            cout << "\nThe bus has completed its route and reached the school." << endl;
            cout << "To restart the route, update bus location to first stop." << endl;
        }
        else {
            // Move to next stop
            bus->currentStopID = nextStop->stopID;
            BusStop* currentStop = findStopByID(bus->currentStopID);
            BusStop* prevStop = findStopByID(currentRouteNode->stopID);
            string currentStopName = currentStop ? currentStop->name : bus->currentStopID;
            string prevStopName = prevStop ? prevStop->name : currentRouteNode->stopID;

            // Calculate progress
            int totalStops = 0;
            int currentPosition = 0;
            route = bus->routeHead;
            while (route != nullptr) {
                totalStops++;
                if (stringsEqualIgnoreCase(route->stopID, bus->currentStopID)) {
                    currentPosition = totalStops;
                }
                route = route->next;
            }

            double progress = totalStops > 0 ? (currentPosition * 100.0 / totalStops) : 0.0;

            cout << "\n========================================" << endl;
            cout << "  SCHOOL BUS MOVED TO NEXT STOP" << endl;
            cout << "========================================" << endl;
            cout << "Bus Number: " << bus->busNumber << endl;
            cout << "From: " << prevStopName << endl;
            cout << "To: " << currentStopName << endl;
            cout << "Destination School: " << bus->schoolName << endl;
            cout << "Progress: " << fixed << setprecision(1) << progress << "% (" << currentPosition << "/" << totalStops << " stops)" << endl;
            cout << "========================================" << endl;



            // Show remaining route
            if (nextStop->next != nullptr) {
                cout << "\nRemaining Route: ";
                RouteStopNode* remaining = nextStop->next;
                bool first = true;
                while (remaining != nullptr) {


                    if (!first) cout << " -> ";
                    BusStop* s = findStopByID(remaining->stopID);
                    cout << (s ? s->name : remaining->stopID);
                    first = false;
                    remaining = remaining->next;
                }
                cout << " -> " << bus->schoolName << " (Final Destination)" << endl;
            }
            else {
                cout << "\nNext Stop: " << bus->schoolName << " (Final Destination - School)" << endl;
            }
        }
    }


    void displaySchoolBuses() {
        cout << "\n=== School Buses ===" << endl;

        BusNode* bus = busesHead;
        int count = 0;
        bool foundAny = false;

        while (bus != nullptr) {
            if (bus->isSchoolBus) {
                foundAny = true;
                count++;
                cout << "\n--- School Bus #" << count << " ---" << endl;
                cout << "Bus Number: " << bus->busNumber << endl;
                cout << "Company: " << bus->company << endl;
                cout << "Destination School: " << bus->schoolName << endl;

                if (bus->currentStopID.empty()) {
                    cout << "Status: NOT STARTED" << endl;
                }
                else {
                    BusStop* currentStop = findStopByID(bus->currentStopID);
                    string currentStopName = currentStop ? currentStop->name : bus->currentStopID;
                    cout << "Current Location: " << currentStopName << endl;

                    // Calculate progress
                    int totalStops = 0;
                    int currentPosition = 0;
                    RouteStopNode* route = bus->routeHead;
                    while (route != nullptr) {
                        totalStops++;
                        if (stringsEqualIgnoreCase(route->stopID, bus->currentStopID)) {
                            currentPosition = totalStops;
                        }
                        route = route->next;
                    }

                    if (currentPosition == totalStops && totalStops > 0) {
                        cout << "Status: ARRIVED AT SCHOOL" << endl;
                    }
                    else {
                        double progress = totalStops > 0 ? (currentPosition * 100.0 / totalStops) : 0.0;
                        cout << "Status: IN TRANSIT" << endl;
                        cout << "Progress: " << fixed << setprecision(1) << progress << "%" << endl;
                    }
                }


                // Show route
                cout << "Route: ";
                RouteStopNode* route = bus->routeHead;
                bool first = true;
                while (route != nullptr) {
                    if (!first) cout << " -> ";

                    BusStop* s = findStopByID(route->stopID);
                    bool isCurrent = stringsEqualIgnoreCase(route->stopID, bus->currentStopID);
                    if (isCurrent) {
                        cout << "[" << (s ? s->name : route->stopID) << "]";
                    }
                    else {
                        cout << (s ? s->name : route->stopID);
                    }
                    first = false;

                    route = route->next;
                }
                cout << " -> " << bus->schoolName << " (School)" << endl;
            }
            bus = bus->next;
        }


        if (!foundAny) {
            cout << "No school buses registered." << endl;
            cout << "Use 'Register School Bus' to convert a regular bus to a school bus." << endl;
        }
        else {
            cout << "\nTotal School Buses: " << count << endl;
        }
    }

    void loadFromFiles() {
        bool loadedStops = false;


        {
            ifstream stopsFile("stops.csv");
            if (stopsFile.is_open()) {
                string line;
                getline(stopsFile, line); // Skip header

                while (getline(stopsFile, line)) {
                    if (line.empty()) continue;
                    normalizeSeparatorsHelper(line);

                    string parts[4];
                    int count = splitString(line, ',', parts, 4);

                    string id, name;
                    double lat = 0, lon = 0;
                    bool valid = false;

                    if (count == 3) {
                        id = trimString(parts[0]);
                        name = trimString(parts[1]);
                        string coordStr = parts[2];
                        stripQuotesHelper(coordStr);

                        string coordParts[2];
                        int coordCount = splitString(coordStr, ',', coordParts, 2);
                        if (coordCount >= 2) {
                            lat = stringToDouble(trimString(coordParts[0]));
                            lon = stringToDouble(trimString(coordParts[1]));
                            valid = true;
                        }
                    }
                    else if (count == 4) {
                        id = trimString(parts[0]);
                        name = trimString(parts[1]);

                        string latStr = parts[2];
                        string lonStr = parts[3];

                        stripQuotesHelper(latStr);
                        stripQuotesHelper(lonStr);

                        lat = stringToDouble(latStr);
                        lon = stringToDouble(lonStr);
                        valid = true;
                    }

                    if (valid && !id.empty() && !name.empty()) {
                        if (findStopByID(id) == nullptr) {
                            GlobalLocationManager* locMgr = GlobalLocationManager::getInstance();
                            if (locMgr->registerLocation(lat, lon, "BusStop", name)) {
                                BusStop* newStop = new BusStop(id, name, lat, lon);
                                newStop->next = stopsHead;
                                stopsHead = newStop;
                                stopCount++;
                                getOrCreateAdjList(id);

                                CityGraph::getInstance()->addLocation(id, name, LOC_BUS_STOP, lat, lon, newStop);
                                loadedStops = true;
                            }
                        }
                    }
                }
                stopsFile.close();

                if (loadedStops) {
                    cout << "Bus stops loaded from file (stops.csv)." << endl;
                }
            }
        }


        if (!loadedStops) {
            ifstream stopsFile2("busstops.csv");
            if (stopsFile2.is_open()) {
                string line;
                getline(stopsFile2, line);

                while (getline(stopsFile2, line)) {
                    if (line.empty()) continue;
                    normalizeSeparatorsHelper(line);

                    string parts[4];
                    int count = splitString(line, ',', parts, 4);
                    if (count >= 4) {
                        string id = trimString(parts[0]);
                        string name = trimString(parts[1]);
                        double lat = stringToDouble(trimString(parts[2]));
                        double lon = stringToDouble(trimString(parts[3]));

                        if (findStopByID(id) == nullptr) {
                            GlobalLocationManager* locMgr = GlobalLocationManager::getInstance();
                            if (locMgr->registerLocation(lat, lon, "BusStop", name)) {
                                BusStop* newStop = new BusStop(id, name, lat, lon);
                                newStop->next = stopsHead;
                                stopsHead = newStop;
                                stopCount++;
                                getOrCreateAdjList(id);

                                CityGraph::getInstance()->addLocation(id, name, LOC_BUS_STOP, lat, lon, newStop);
                                loadedStops = true;
                            }
                        }
                    }
                }
                stopsFile2.close();

                if (loadedStops) {
                    cout << "Bus stops loaded from file (busstops.csv)." << endl;
                }
            }
        }

        if (!loadedStops) {
            cout << "Warning: No bus stops file found or parsed." << endl;
        }

        // ---------- Load roads ----------
        int roadsLoadedFromFile = 0;
        {
            ifstream roadsFile("roads.csv");
            if (roadsFile.is_open()) {
                string line;
                getline(roadsFile, line); // Skip header

                while (getline(roadsFile, line)) {
                    if (line.empty()) continue;
                    normalizeSeparatorsHelper(line);

                    string parts[3];
                    int count = splitString(line, ',', parts, 3);
                    if (count >= 2) {
                        string id1 = trimString(parts[0]);
                        string id2 = trimString(parts[1]);

                        BusStop* stop1 = findStopByID(id1);
                        BusStop* stop2 = findStopByID(id2);

                        if (stop1 != nullptr && stop2 != nullptr && !edgeExists(id1, id2)) {
                            double distance = calculateDistance(stop1->lat, stop1->lon, stop2->lat, stop2->lon);

                            AdjListNode* adj1 = getOrCreateAdjList(id1);
                            RoadEdge* edge1 = new RoadEdge(id2, distance);
                            edge1->next = adj1->edgeHead;
                            adj1->edgeHead = edge1;

                            AdjListNode* adj2 = getOrCreateAdjList(id2);
                            RoadEdge* edge2 = new RoadEdge(id1, distance);
                            edge2->next = adj2->edgeHead;
                            adj2->edgeHead = edge2;

                            CityGraph::getInstance()->connectLocations(id1, id2, distance);

                            roadsLoadedFromFile++;
                        }
                    }
                }
                roadsFile.close();
                cout << "Loaded " << roadsLoadedFromFile << " roads from file." << endl;
            }
            else {
                cout << "Warning: roads.csv not found." << endl;
            }
        }


        int busesLoaded = 0;
        {
            ifstream busesFile("buses.csv");
            if (busesFile.is_open()) {
                string line;
                getline(busesFile, line); // Skip header

                while (getline(busesFile, line)) {
                    if (line.empty()) continue;
                    normalizeSeparatorsHelper(line);

                    string parts[4];
                    int count = splitString(line, ',', parts, 4);
                    if (count >= 3) {
                        string busNum = trimString(parts[0]);
                        string company = trimString(parts[1]);
                        string currentStop = trimString(parts[2]);

                        if (searchBusByNumber(busNum) == nullptr) {
                            BusNode* newBus = new BusNode(busNum, company);
                            newBus->currentStopID = currentStop;
                            newBus->next = busesHead;
                            busesHead = newBus;
                            busCount++;
                            addBusToHash(newBus);


                            if (count >= 4 && !parts[3].empty()) {
                                string routeRaw = parts[3];


                                for (size_t i = 0; i < routeRaw.length(); i++) {
                                    if (routeRaw[i] == '>') routeRaw[i] = '-';
                                }

                                string routeStops[200];
                                int routeCount = splitString(routeRaw, '-', routeStops, 50);
                                for (int i = 0; i < routeCount; i++) {
                                    string stopID = trimString(routeStops[i]);
                                    if (!stopID.empty() && findStopByID(stopID) != nullptr) {
                                        RouteStopNode* newRouteStop = new RouteStopNode(stopID);
                                        if (newBus->routeHead == nullptr) {
                                            newBus->routeHead = newRouteStop;
                                        }
                                        else {
                                            RouteStopNode* temp = newBus->routeHead;
                                            while (temp->next != nullptr) temp = temp->next;
                                            temp->next = newRouteStop;
                                        }
                                    }
                                }
                            }
                            busesLoaded++;
                        }
                    }
                }
                busesFile.close();
                cout << "Loaded " << busesLoaded << " buses from file." << endl;
            }
            else {
                cout << "Warning: buses.csv not found." << endl;
            }
        }


        if (roadsLoadedFromFile == 0 && busesLoaded > 0) {
            cout << "\nWarning: No roads. csv found or no roads loaded." << endl;
            generateRoadsFromBusRoutes();
        }
    }



    int getStopCount() {
        return stopCount;
    }
    int getBusCount() {
        return busCount;
    }


    ~TransportSystem() {

        while (stopsHead != nullptr) {
            BusStop* temp = stopsHead;
            stopsHead = stopsHead->next;
            delete temp;
        }

        while (adjListHead != nullptr) {
            AdjListNode* adjTemp = adjListHead;
            adjListHead = adjListHead->next;


            while (adjTemp->edgeHead != nullptr) {
                RoadEdge* edgeTemp = adjTemp->edgeHead;
                adjTemp->edgeHead = adjTemp->edgeHead->next;
                delete edgeTemp;
            }
            delete adjTemp;
        }




        while (busesHead != nullptr) {
            BusNode* busTemp = busesHead;
            busesHead = busesHead->next;


            while (busTemp->routeHead != nullptr) {
                RouteStopNode* routeTemp = busTemp->routeHead;
                busTemp->routeHead = busTemp->routeHead->next;
                delete routeTemp;
            }
            delete busTemp;
        }


        for (int i = 0; i < BUS_TABLE_SIZE; i++) {
            while (busHashTable[i] != nullptr) {
                BusHashNode* temp = busHashTable[i];
                busHashTable[i] = busHashTable[i]->next;
                delete temp;
            }
        }
    }
};

