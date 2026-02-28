#pragma once
#include "Utils.h"
#include "GlobalLocationManager.h"
#include "CityGraph.h"
#include <fstream> 

// PUBLIC FACILITIES MODULE
// Graph (Adjacency List) for facility connections
// Dijkstra's Algorithm for shortest path
// Hash Table for facility type search (separate chaining)
// Linked Lists for facilities
 

const int FACILITY_PARK = 0;
const int FACILITY_LIBRARY = 1;
const int FACILITY_COMMUNITY_CENTER = 2;
const int FACILITY_SPORTS_COMPLEX = 3;
const int FACILITY_MUSEUM = 4;
const int FACILITY_PLAYGROUND = 5;
const int FACILITY_SWIMMING_POOL = 6;
const int FACILITY_GYM = 7;
const int FACILITY_MOSQUE = 8;
const int FACILITY_POST_OFFICE = 9;
const int FACILITY_POLICE_STATION = 10;
const int FACILITY_FIRE_STATION = 11;
const int FACILITY_GOVERNMENT_OFFICE = 12;
const int FACILITY_OTHER = 13;

// Convert Facility Type int to string
string facilityTypeToString(int type) {
    switch (type) {
    case FACILITY_PARK: 
        return "Park";
    case FACILITY_LIBRARY: 
        return "Library";
    case FACILITY_COMMUNITY_CENTER: 
        return "Community Center";
    case FACILITY_SPORTS_COMPLEX: 
        return "Sports Complex";
    case FACILITY_MUSEUM:
        return "Museum";
    case FACILITY_PLAYGROUND: 
        return "Playground";
    case FACILITY_SWIMMING_POOL: 
        return "Swimming Pool";
    case FACILITY_GYM: 
        return "Gym";
    case FACILITY_MOSQUE: 
        return "Mosque";
    case FACILITY_POST_OFFICE: 
        return "Post Office";
    case FACILITY_POLICE_STATION: 
        return "Police Station";
    case FACILITY_FIRE_STATION: 
        return "Fire Station";
    case FACILITY_GOVERNMENT_OFFICE: 
        return "Government Office";
    default: return "Other";
    }
}

// Parse string to Facility Type (int)
int stringToFacilityType(const string& typeStr) {
    string lower = toLowerStr(trimString(typeStr));

    if (lower == "park" || lower == "1") 
        return FACILITY_PARK;
    if (lower == "library" || lower == "2") 
        return FACILITY_LIBRARY;
    if (lower == "community center" || lower == "community" || lower == "3") 
        return FACILITY_COMMUNITY_CENTER;
    if (lower == "sports complex" || lower == "sports" || lower == "4") 
        return FACILITY_SPORTS_COMPLEX;
    if (lower == "museum" || lower == "5") 
        return FACILITY_MUSEUM;
    if (lower == "playground" || lower == "6") 
        return FACILITY_PLAYGROUND;
    if (lower == "swimming pool" || lower == "pool" || lower == "7") 
        return FACILITY_SWIMMING_POOL;
    if (lower == "gym" || lower == "8") 
        return FACILITY_GYM;
    if (lower == "mosque" || lower == "9") 
        return FACILITY_MOSQUE;
    if (lower == "post office" || lower == "post" || lower == "10") 
        return FACILITY_POST_OFFICE;
    if (lower == "police station" || lower == "police" || lower == "11") 
        return FACILITY_POLICE_STATION;
    if (lower == "fire station" || lower == "fire" || lower == "12") 
        return FACILITY_FIRE_STATION;
    if (lower == "government office" || lower == "government" || lower == "13") 
        return FACILITY_GOVERNMENT_OFFICE;

    return FACILITY_OTHER;
}

// Amenity Node Services/Features of a facility
struct AmenityNode {
    string name;
    bool available;
    AmenityNode* next;

    AmenityNode(const string& n, bool avail = true)
        : name(n), available(avail), next(nullptr) {
    }
};

// Facility Edge Graph Edge for connections
struct FacilityEdge {
    string destFacilityID;
    double distance;
    string connectionType; // e.g., "Walking Path", "Road", "Bicycle Lane"
    FacilityEdge* next;

    FacilityEdge(const string& dest, double dist, const string& connType = "Road")
        : destFacilityID(dest), distance(dist), connectionType(connType), next(nullptr) {
    }
};


// Facility Adjacency List Node 
struct FacilityAdjNode {
    string facilityID;
    FacilityEdge* edgeHead;
    FacilityAdjNode* next;

    FacilityAdjNode(const string& id) : facilityID(id), edgeHead(nullptr), next(nullptr) {}
};


// Facility Node
struct FacilityNode {
    string facilityID;
    string name;
    int type; // Changed from FacilityType to int
    string sector;
    double lat, lon;
    string openingTime;
    string closingTime;
    bool isOpen24Hours;
    double rating;
    int amenityCount;
    AmenityNode* amenityHead;
    FacilityNode* next;

    FacilityNode(const string& id, const string& n, int t, const string& sec, double la, double lo, const string& open, const string& close, bool is24 = false)
        : facilityID(id), name(n), type(t), sector(sec), lat(la), lon(lo), openingTime(open), closingTime(close), isOpen24Hours(is24), rating(0.0),
        amenityCount(0), amenityHead(nullptr), next(nullptr) {
    }
};


// Hash Node for Facility Type Search 
struct FacilityHashNode {
    int type; // Changed from FacilityType to int
    FacilityNode* facilityRef;
    FacilityHashNode* next;

    FacilityHashNode(int t, FacilityNode* ref)
        : type(t), facilityRef(ref), next(nullptr) {
    }
};


// MIN HEAP FOR DIJKSTRA'S ALGORITHM
struct FacilityDijkstraNode {
    string facilityID;
    double distance;
};

class FacilityDijkstraHeap {
private:
    static const int MAX_SIZE = 500;
    FacilityDijkstraNode heap[MAX_SIZE];
    int size;

    void swap(int i, int j) {
        FacilityDijkstraNode temp = heap[i];
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
    FacilityDijkstraHeap() : size(0) {}

    bool isEmpty() { return size == 0; }

    void insert(const string& id, double dist) {
        if (size >= MAX_SIZE) return;
        heap[size].facilityID = id;
        heap[size].distance = dist;
        siftUp(size);
        size++;
    }

    FacilityDijkstraNode extractMin() {
        FacilityDijkstraNode minNode = heap[0];
        heap[0] = heap[size - 1];
        size--;
        if (size > 0)
            siftDown(0);
        return minNode;
    }

    void clear() { size = 0; }
};

// FACILITY TYPE HASH TABLE (Separate Chaining)
class FacilityTypeHashTable {
private:
    static const int TABLE_SIZE = 20;
    FacilityHashNode* table[TABLE_SIZE];

    int hashFunc(int type) {
        return type % TABLE_SIZE;
    }

public:
    FacilityTypeHashTable() {
        for (int i = 0; i < TABLE_SIZE; i++) {
            table[i] = nullptr;
        }
    }

    void insert(int type, FacilityNode* facility) {
        int idx = hashFunc(type);

        // Check for duplicate
        FacilityHashNode* current = table[idx];
        while (current != nullptr) {
            if (current->facilityRef == facility) {
                return; 
            }
            current = current->next;
        }

        FacilityHashNode* newNode = new FacilityHashNode(type, facility);
        newNode->next = table[idx];
        table[idx] = newNode;
    }

    void remove(FacilityNode* facility) {
        int idx = hashFunc(facility->type);

        if (table[idx] == nullptr) 
            return;

        if (table[idx]->facilityRef == facility) {
            FacilityHashNode* temp = table[idx];
            table[idx] = table[idx]->next;
            delete temp;
            return;
        }

        FacilityHashNode* current = table[idx];
        while (current->next != nullptr) {
            if (current->next->facilityRef == facility) {
                FacilityHashNode* temp = current->next;
                current->next = current->next->next;
                delete temp;
                return;
            }
            current = current->next;
        }
    }

    // Changed argument from FacilityType to int
    void searchByType(int type) {
        int idx = hashFunc(type);
        FacilityHashNode* current = table[idx];
        bool found = false;

        cout << "\n=== " << facilityTypeToString(type) << " Facilities ===" << endl;
        while (current != nullptr) {
            if (current->type == type && current->facilityRef != nullptr) {
                FacilityNode* f = current->facilityRef;
                cout << "- " << f->name << " (" << f->sector << ")" << endl;
                cout << "  Timings: " << (f->isOpen24Hours ? "Open 24 Hours" : f->openingTime + " - " + f->closingTime) << endl;
                if (f->rating > 0) {
                    cout << "  Rating: " << f->rating << "/5.0" << endl;
                }
                found = true;
            }
            current = current->next;
        }
        if (!found) {
            cout << "No facilities of this type found." << endl;
        }
    }

    int countByType(int type) {
        int idx = hashFunc(type);
        FacilityHashNode* current = table[idx];
        int count = 0;

        while (current != nullptr) {
            if (current->type == type) count++;
            current = current->next;
        }
        return count;
    }

    ~FacilityTypeHashTable() {
        for (int i = 0; i < TABLE_SIZE; i++) {
            while (table[i] != nullptr) {
                FacilityHashNode* temp = table[i];
                table[i] = table[i]->next;
                delete temp;
            }
        }
    }
};


// FACILITIES SYSTEM MANAGER
class FacilitiesSystem {
private:
    FacilityNode* facilitiesHead;
    FacilityAdjNode* adjListHead;
    FacilityTypeHashTable typeHash;
    int facilityCount;
    int facilityIDCounter;


    // Private helper Functions 
    FacilityNode* findFacilityByName(const string& name) {
        FacilityNode* current = facilitiesHead;
        while (current != nullptr) {
            if (stringsEqualIgnoreCase(current->name, name)) return current;
            current = current->next;
        }
        return nullptr;
    }

    FacilityNode* findFacilityByID(const string& id) {
        FacilityNode* current = facilitiesHead;
        while (current != nullptr) {
            if (stringsEqualIgnoreCase(current->facilityID, id)) return current;
            current = current->next;
        }
        return nullptr;
    }

    FacilityAdjNode* getAdjList(const string& facilityID) {
        FacilityAdjNode* current = adjListHead;
        while (current != nullptr) {
            if (stringsEqualIgnoreCase(current->facilityID, facilityID)) return current;
            current = current->next;
        }
        return nullptr;
    }

    FacilityAdjNode* getOrCreateAdjList(const string& facilityID) {
        FacilityAdjNode* existing = getAdjList(facilityID);
        if (existing != nullptr) 
            return existing;

        FacilityAdjNode* newNode = new FacilityAdjNode(facilityID);
        newNode->next = adjListHead;
        adjListHead = newNode;
        return newNode;
    }

    bool edgeExists(const string& from, const string& to) {
        FacilityAdjNode* adj = getAdjList(from);
        if (adj == nullptr) return false;

        FacilityEdge* edge = adj->edgeHead;
        while (edge != nullptr) {
            if (stringsEqualIgnoreCase(edge->destFacilityID, to)) return true;
            edge = edge->next;
        }
        return false;
    }

    void removeEdgeFromAdjList(FacilityAdjNode* adj, const string& destID) {
        if (adj == nullptr || adj->edgeHead == nullptr) return;

        while (adj->edgeHead != nullptr && stringsEqualIgnoreCase(adj->edgeHead->destFacilityID, destID)) {
            FacilityEdge* temp = adj->edgeHead;
            adj->edgeHead = adj->edgeHead->next;
            delete temp;
        }

        if (adj->edgeHead == nullptr) return;

        FacilityEdge* current = adj->edgeHead;
        while (current->next != nullptr) {
            if (stringsEqualIgnoreCase(current->next->destFacilityID, destID)) {
                FacilityEdge* temp = current->next;
                current->next = current->next->next;
                delete temp;
            }
            else {
                current = current->next;
            }
        }
    }

    void removeAdjListEntry(const string& facilityID) {
        if (adjListHead == nullptr) return;

        if (stringsEqualIgnoreCase(adjListHead->facilityID, facilityID)) {
            FacilityAdjNode* temp = adjListHead;
            while (temp->edgeHead != nullptr) {
                FacilityEdge* edgeTemp = temp->edgeHead;
                temp->edgeHead = temp->edgeHead->next;
                delete edgeTemp;
            }
            adjListHead = adjListHead->next;
            delete temp;
            return;
        }

        FacilityAdjNode* current = adjListHead;
        while (current->next != nullptr) {
            if (stringsEqualIgnoreCase(current->next->facilityID, facilityID)) {
                FacilityAdjNode* temp = current->next;
                while (temp->edgeHead != nullptr) {
                    FacilityEdge* edgeTemp = temp->edgeHead;
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

    AmenityNode* findAmenity(FacilityNode* facility, const string& amenityName) {
        if (facility == nullptr) return nullptr;
        AmenityNode* current = facility->amenityHead;
        while (current != nullptr) {
            if (stringsEqualIgnoreCase(current->name, amenityName)) return current;
            current = current->next;
        }
        return nullptr;
    }

    void deleteAllAmenities(FacilityNode* facility) {
        while (facility->amenityHead != nullptr) {
            AmenityNode* temp = facility->amenityHead;
            facility->amenityHead = facility->amenityHead->next;
            delete temp;
        }
        facility->amenityCount = 0;
    }

    FacilityNode* selectFacility() {
        string name = getStringInput("Enter Facility Name: ");
        FacilityNode* facility = findFacilityByName(name);
        if (facility == nullptr) {
            cout << "Error: Facility '" << name << "' not found!" << endl;
        }
        return facility;
    }

    void displayFacilityTypes() {
        cout << "\nFacility Types:" << endl;
        cout << "1.   Park" << endl;
        cout << "2.  Library" << endl;
        cout << "3.  Community Center" << endl;
        cout << "4.   Sports Complex" << endl;
        cout << "5.  Museum" << endl;
        cout << "6.  Playground" << endl;
        cout << "7.  Swimming Pool" << endl;
        cout << "8.   Gym" << endl;
        cout << "9.  Mosque" << endl;
        cout << "10. Post Office" << endl;
        cout << "11. Police Station" << endl;
        cout << "12. Fire Station" << endl;
        cout << "13. Government Office" << endl;
        cout << "14.  Other" << endl;
    }

    // Changed return type from FacilityType to int
    int selectFacilityType() {
        displayFacilityTypes();
        int choice = getIntInput("Enter type number: ");

        switch (choice) {
        case 1: return FACILITY_PARK;
        case 2: return FACILITY_LIBRARY;
        case 3: return FACILITY_COMMUNITY_CENTER;
        case 4: return FACILITY_SPORTS_COMPLEX;
        case 5: return FACILITY_MUSEUM;
        case 6: return FACILITY_PLAYGROUND;
        case 7: return FACILITY_SWIMMING_POOL;
        case 8: return FACILITY_GYM;
        case 9: return FACILITY_MOSQUE;
        case 10: return FACILITY_POST_OFFICE;
        case 11: return FACILITY_POLICE_STATION;
        case 12: return FACILITY_FIRE_STATION;
        case 13: return FACILITY_GOVERNMENT_OFFICE;
        default: return FACILITY_OTHER;
        }
    }

    int getConnectionCount() {
        int count = 0;
        FacilityAdjNode* adj = adjListHead;
        while (adj != nullptr) {
            FacilityEdge* edge = adj->edgeHead;
            while (edge != nullptr) {
                count++;
                edge = edge->next;
            }
            adj = adj->next;
        }
        return count / 2; // Undirected graph
    }

public:
    FacilitiesSystem() : facilitiesHead(nullptr), adjListHead(nullptr), facilityCount(0), facilityIDCounter(1) {
    }


    void registerFacility() {
        cout << "\n=== Register New Public Facility ===" << endl;

        string name = getStringInput("Enter Facility Name: ");
        if (name.empty()) {
            cout << "Error: Facility name cannot be empty." << endl;
            return;
        }

        if (findFacilityByName(name) != nullptr) {
            cout << "Error: Facility '" << name << "' already exists!" << endl;
            return;
        }

        int type = selectFacilityType();

        string sector = getStringInput("Enter Sector (e.g., F-9, G-10): ");
        if (sector.empty()) {
            cout << "Error: Sector cannot be empty." << endl;
            return;
        }

        double lat = getDoubleInput("Enter Latitude: ");
        double lon = getDoubleInput("Enter Longitude: ");

        GlobalLocationManager* locMgr = GlobalLocationManager::getInstance();
        if (!locMgr->registerLocation(lat, lon, "Facility", name)) {
            cout << "Facility not created !";
            return;
        }

        string is24HoursStr = getStringInput("Is it open 24 hours?  (yes/no): ");
        bool is24Hours = (toLowerStr(is24HoursStr) == "yes" || toLowerStr(is24HoursStr) == "y");

        string openTime = "N/A";
        string closeTime = "N/A";
        if (!is24Hours) {
            openTime = getStringInput("Enter Opening Time (e.g., 9:00 AM): ");
            closeTime = getStringInput("Enter Closing Time (e.g., 5:00 PM): ");
        }

        string facilityID = "F" + intToString(facilityIDCounter++);

        FacilityNode* newFacility = new FacilityNode(facilityID, name, type, sector,
            lat, lon, openTime, closeTime, is24Hours);
        newFacility->next = facilitiesHead;
        facilitiesHead = newFacility;
        facilityCount++;

        getOrCreateAdjList(facilityID);
        typeHash.insert(type, newFacility);

        CityGraph::getInstance()->addLocation(facilityID, name, LOC_FACILITY, lat, lon, newFacility);

        cout << "Facility '" << name << "' registered successfully with ID: " << facilityID << endl;
        cout << "Type: " << facilityTypeToString(type) << endl;
    }

    void deleteFacility() {
        cout << "\n=== Delete Facility ===" << endl;

        string name = getStringInput("Enter Facility Name to delete: ");
        FacilityNode* facility = findFacilityByName(name);

        if (facility == nullptr) {
            cout << "Error: Facility not found!" << endl;
            return;
        }

        cout << "\nFacility Details:" << endl;
        cout << "  Name: " << facility->name << endl;
        cout << "  Type: " << facilityTypeToString(facility->type) << endl;
        cout << "  Sector: " << facility->sector << endl;
        cout << "  Amenities: " << facility->amenityCount << endl;

        string confirm = getStringInput("\nType 'YES' to confirm deletion: ");
        if (confirm != "YES") {
            cout << "Deletion cancelled." << endl;
            return;
        }

        string facilityID = facility->facilityID;

        FacilityAdjNode* adj = adjListHead;
        while (adj != nullptr) {
            removeEdgeFromAdjList(adj, facilityID);
            adj = adj->next;
        }

        // Remove this facility's adjacency list
        removeAdjListEntry(facilityID);

        // Remove from type hash table
        typeHash.remove(facility);

        // Remove from linked list
        if (stringsEqualIgnoreCase(facilitiesHead->name, name)) {
            FacilityNode* temp = facilitiesHead;
            facilitiesHead = facilitiesHead->next;
            deleteAllAmenities(temp);
            GlobalLocationManager::getInstance()->removeLocation(temp->lat, temp->lon);
            CityGraph::getInstance()->removeLocation(temp->facilityID);
            delete temp;
        }
        else {
            FacilityNode* current = facilitiesHead;
            while (current->next != nullptr) {
                if (stringsEqualIgnoreCase(current->next->name, name)) {
                    FacilityNode* temp = current->next;
                    current->next = current->next->next;
                    deleteAllAmenities(temp);
                    GlobalLocationManager::getInstance()->removeLocation(temp->lat, temp->lon);
                    CityGraph::getInstance()->removeLocation(temp->facilityID);
                    delete temp;
                    break;
                }
                current = current->next;
            }
        }

        facilityCount--;
        cout << "Facility '" << name << "' deleted successfully!" << endl;
    }

    void updateFacility() {
        cout << "\n=== Update Facility ===" << endl;

        FacilityNode* facility = selectFacility();
        if (facility == nullptr) return;

        cout << "\nCurrent Details:" << endl;
        cout << "  ID: " << facility->facilityID << endl;
        cout << "  Name: " << facility->name << endl;
        cout << "  Type: " << facilityTypeToString(facility->type) << endl;
        cout << "  Sector: " << facility->sector << endl;
        cout << "  Timings: " << (facility->isOpen24Hours ? "Open 24 Hours" :
            facility->openingTime + " - " + facility->closingTime) << endl;
        cout << "  Rating: " << facility->rating << "/5.0" << endl;

        cout << "\nWhat would you like to update?" << endl;
        cout << "1. Name" << endl;
        cout << "2. Type" << endl;
        cout << "3. Timings" << endl;
        cout << "4. Rating" << endl;
        cout << "5. All of the above" << endl;
        cout << "0. Cancel" << endl;

        int choice = getIntInput("Enter choice: ");

        if (choice == 0) {
            cout << "Update cancelled." << endl;
            return;
        }

        if (choice == 1 || choice == 5) {
            string newName = getStringInput("Enter new name (or press Enter to keep current): ");
            if (!newName.empty()) {
                if (findFacilityByName(newName) != nullptr &&
                    !stringsEqualIgnoreCase(newName, facility->name)) {
                    cout << "Error: A facility with name '" << newName << "' already exists!" << endl;
                    return;
                }
                facility->name = newName;
            }
        }

        if (choice == 2 || choice == 5) {
            cout << "Update facility type?" << endl;
            string updateType = getStringInput("Enter 'yes' to change type: ");
            if (toLowerStr(updateType) == "yes" || toLowerStr(updateType) == "y") {
                typeHash.remove(facility); // Remove from old type bucket
                facility->type = selectFacilityType();
                typeHash.insert(facility->type, facility); // Add to new type bucket
            }
        }

        if (choice == 3 || choice == 5) {
            string is24HoursStr = getStringInput("Is it open 24 hours? (yes/no/skip): ");
            if (toLowerStr(is24HoursStr) == "yes" || toLowerStr(is24HoursStr) == "y") {
                facility->isOpen24Hours = true;
                facility->openingTime = "N/A";
                facility->closingTime = "N/A";
            }
            else if (toLowerStr(is24HoursStr) == "no" || toLowerStr(is24HoursStr) == "n") {
                facility->isOpen24Hours = false;
                string newOpenTime = getStringInput("Enter opening time: ");
                string newCloseTime = getStringInput("Enter closing time: ");
                if (!newOpenTime.empty()) facility->openingTime = newOpenTime;
                if (!newCloseTime.empty()) facility->closingTime = newCloseTime;
            }
        }

        if (choice == 4 || choice == 5) {
            double newRating = getDoubleInput("Enter new rating (0.0 - 5.0, -1 to skip): ");
            if (newRating >= 0.0 && newRating <= 5.0) {
                facility->rating = newRating;
            }
        }

        cout << "Facility updated successfully!" << endl;
    }

    // Facility Connection Operations 

    void connectFacilities() {
        cout << "\n=== Connect Facilities (Add Path) ===" << endl;

        if (facilityCount < 2) {
            cout << "Error: Need at least 2 facilities to create a connection." << endl;
            return;
        }

        string name1 = getStringInput("Enter First Facility Name: ");
        FacilityNode* facility1 = findFacilityByName(name1);
        if (facility1 == nullptr) {
            cout << "Error: Facility '" << name1 << "' not found!" << endl;
            return;
        }

        string name2 = getStringInput("Enter Second Facility Name: ");
        FacilityNode* facility2 = findFacilityByName(name2);
        if (facility2 == nullptr) {
            cout << "Error: Facility '" << name2 << "' not found!" << endl;
            return;
        }

        if (stringsEqualIgnoreCase(facility1->facilityID, facility2->facilityID)) {
            cout << "Error: Cannot connect a facility to itself!" << endl;
            return;
        }

        if (edgeExists(facility1->facilityID, facility2->facilityID)) {
            cout << "Error: These facilities are already connected!" << endl;
            return;
        }

        cout << "\nConnection Type:" << endl;
        cout << "1. Walking Path" << endl;
        cout << "2. Road" << endl;
        cout << "3.  Bicycle Lane" << endl;
        cout << "4. Other" << endl;

        int connChoice = getIntInput("Enter connection type: ");
        string connectionType;
        switch (connChoice) {
        case 1: connectionType = "Walking Path";
            break;
        case 2: connectionType = "Road"; 
            break;
        case 3: connectionType = "Bicycle Lane"; 
            break;
        default: connectionType = "Other"; 
            break;
        }

        double distance = calculateDistance(facility1->lat, facility1->lon,
            facility2->lat, facility2->lon);

        // Add bidirectional edges
        FacilityAdjNode* adj1 = getOrCreateAdjList(facility1->facilityID);
        FacilityEdge* edge1 = new FacilityEdge(facility2->facilityID, distance, connectionType);
        edge1->next = adj1->edgeHead;
        adj1->edgeHead = edge1;

        FacilityAdjNode* adj2 = getOrCreateAdjList(facility2->facilityID);
        FacilityEdge* edge2 = new FacilityEdge(facility1->facilityID, distance, connectionType);
        edge2->next = adj2->edgeHead;
        adj2->edgeHead = edge2;

        cout << "Connected '" << facility1->name << "' <--> '" << facility2->name << "'" << endl;
        cout << "Connection Type: " << connectionType << " | Distance: " << distance << " units" << endl;
    }

    void disconnectFacilities() {
        cout << "\n=== Disconnect Facilities (Remove Path) ===" << endl;

        string name1 = getStringInput("Enter First Facility Name: ");
        FacilityNode* facility1 = findFacilityByName(name1);
        if (facility1 == nullptr) {
            cout << "Error: Facility '" << name1 << "' not found!" << endl;
            return;
        }

        string name2 = getStringInput("Enter Second Facility Name: ");
        FacilityNode* facility2 = findFacilityByName(name2);
        if (facility2 == nullptr) {
            cout << "Error: Facility '" << name2 << "' not found!" << endl;
            return;
        }

        if (!edgeExists(facility1->facilityID, facility2->facilityID)) {
            cout << "Error: These facilities are not connected!" << endl;
            return;
        }

        FacilityAdjNode* adj1 = getAdjList(facility1->facilityID);
        if (adj1 != nullptr) {
            removeEdgeFromAdjList(adj1, facility2->facilityID);
        }

        FacilityAdjNode* adj2 = getAdjList(facility2->facilityID);
        if (adj2 != nullptr) {
            removeEdgeFromAdjList(adj2, facility1->facilityID);
        }

        cout << "Disconnected '" << facility1->name << "' from '" << facility2->name << "'!" << endl;
    }


    void addAmenity() {
        cout << "\n=== Add Amenity to Facility ===" << endl;

        FacilityNode* facility = selectFacility();
        if (facility == nullptr) return;

        string amenityName = getStringInput("Enter Amenity Name (e.g., Parking, Restroom, WiFi): ");
        if (amenityName.empty()) {
            cout << "Error: Amenity name cannot be empty." << endl;
            return;
        }

        if (findAmenity(facility, amenityName) != nullptr) {
            cout << "Error: Amenity '" << amenityName << "' already exists in this facility!" << endl;
            return;
        }

        string availStr = getStringInput("Is it currently available? (yes/no): ");
        bool available = (toLowerStr(availStr) == "yes" || toLowerStr(availStr) == "y");

        AmenityNode* newAmenity = new AmenityNode(amenityName, available);
        newAmenity->next = facility->amenityHead;
        facility->amenityHead = newAmenity;
        facility->amenityCount++;

        cout << "Amenity '" << amenityName << "' added to '" << facility->name << "'!" << endl;
    }

    void removeAmenity() {
        cout << "\n=== Remove Amenity from Facility ===" << endl;

        FacilityNode* facility = selectFacility();
        if (facility == nullptr) return;

        if (facility->amenityCount == 0) {
            cout << "Error: No amenities in this facility!" << endl;
            return;
        }

        // Display 
        cout << "\nAmenities at " << facility->name << ":" << endl;
        AmenityNode* amenity = facility->amenityHead;
        while (amenity != nullptr) {
            cout << "  - " << amenity->name << " [" << (amenity->available ? "Available" : "Unavailable") << "]" << endl;
            amenity = amenity->next;
        }

        string amenityName = getStringInput("\nEnter Amenity Name to remove: ");
        amenity = findAmenity(facility, amenityName);

        if (amenity == nullptr) {
            cout << "Error: Amenity not found!" << endl;
            return;
        }

        // Remove from linked list
        if (stringsEqualIgnoreCase(facility->amenityHead->name, amenityName)) {
            AmenityNode* temp = facility->amenityHead;
            facility->amenityHead = facility->amenityHead->next;
            cout << "Amenity '" << temp->name << "' removed successfully!" << endl;
            delete temp;
        }
        else {
            AmenityNode* current = facility->amenityHead;
            while (current->next != nullptr) {
                if (stringsEqualIgnoreCase(current->next->name, amenityName)) {
                    AmenityNode* temp = current->next;
                    current->next = current->next->next;
                    cout << "Amenity '" << temp->name << "' removed successfully!" << endl;
                    delete temp;
                    break;
                }
                current = current->next;
            }
        }

        facility->amenityCount--;
    }

    void updateAmenityStatus() {
        cout << "\n=== Update Amenity Availability ===" << endl;

        FacilityNode* facility = selectFacility();
        if (facility == nullptr) return;

        if (facility->amenityCount == 0) {
            cout << "Error: No amenities in this facility!" << endl;
            return;
        }

        // Display amenities
        cout << "\nAmenities at " << facility->name << ":" << endl;
        AmenityNode* amenity = facility->amenityHead;
        while (amenity != nullptr) {
            cout << "  - " << amenity->name << " ["
                << (amenity->available ? "Available" : "Unavailable") << "]" << endl;
            amenity = amenity->next;
        }

        string amenityName = getStringInput("\nEnter Amenity Name to update: ");
        amenity = findAmenity(facility, amenityName);

        if (amenity == nullptr) {
            cout << "Error: Amenity not found!" << endl;
            return;
        }

        cout << "Current Status: " << (amenity->available ? "Available" : "Unavailable") << endl;
        string newStatus = getStringInput("Set as available? (yes/no): ");

        amenity->available = (toLowerStr(newStatus) == "yes" || toLowerStr(newStatus) == "y");
        cout << "Amenity status updated to: " << (amenity->available ? "Available" : "Unavailable") << endl;
    }

  
    // Rating Operations 

    void rateFacility() {
        cout << "\n=== Rate Facility ===" << endl;

        FacilityNode* facility = selectFacility();
        if (facility == nullptr) return;

        cout << "Current Rating: " << facility->rating << "/5.0" << endl;
        double newRating = getDoubleInput("Enter your rating (0.0 - 5.0): ");

        if (newRating < 0.0 || newRating > 5.0) {
            cout << "Error: Rating must be between 0.0 and 5.0!" << endl;
            return;
        }

        // Simple average (in real system would track number of ratings)
        if (facility->rating == 0) {
            facility->rating = newRating;
        }
        else {
            facility->rating = (facility->rating + newRating) / 2.0;
        }

        cout << "Thank you!  New average rating: " << facility->rating << "/5.0" << endl;
    }

    // Search Functions

    void searchByType() {
        cout << "\n=== Search Facilities by Type ===" << endl;
        int type = selectFacilityType();
        typeHash.searchByType(type);
    }

    void searchByName() {
        cout << "\n=== Search Facility by Name ===" << endl;
        string query = getStringInput("Enter facility name to search: ");
        string cleanQuery = toLowerStr(trimString(query));

        bool found = false;
        cout << "\n=== Search Results ===" << endl;

        FacilityNode* facility = facilitiesHead;
        while (facility != nullptr) {
            string cleanName = toLowerStr(facility->name);
            if (cleanName.find(cleanQuery) != string::npos) {
                cout << "- " << facility->name << " [" << facilityTypeToString(facility->type) << "]" << endl;
                cout << "  Sector: " << facility->sector << endl;
                cout << "  Timings: " << (facility->isOpen24Hours ? "Open 24 Hours" : facility->openingTime + " - " + facility->closingTime) << endl;
                if (facility->rating > 0) {
                    cout << "  Rating: " << facility->rating << "/5.0" << endl;
                }
                found = true;
            }
            facility = facility->next;
        }

        if (!found) {
            cout << "No facilities found matching '" << query << "'." << endl;
        }
    }

    void findNearestFacility() {
        cout << "\n=== Find Nearest Facility ===" << endl;

        if (facilitiesHead == nullptr) {
            cout << "No facilities registered." << endl;
            return;
        }

        cout << "Search for specific type or any facility?" << endl;
        cout << "1. Any facility" << endl;
        cout << "2. Specific type" << endl;

        int choice = getIntInput("Enter choice: ");

        int searchType = FACILITY_OTHER;
        bool filterByType = false;

        if (choice == 2) {
            searchType = selectFacilityType();
            filterByType = true;
        }

        double userLat = getDoubleInput("Enter your Latitude: ");
        double userLon = getDoubleInput("Enter your Longitude: ");

        FacilityNode* nearest = nullptr;
        double minDist = DBL_MAX;

        FacilityNode* current = facilitiesHead;
        while (current != nullptr) {
            if (!filterByType || current->type == searchType) {
                double dist = calculateDistance(userLat, userLon, current->lat, current->lon);
                if (dist < minDist) {
                    minDist = dist;
                    nearest = current;
                }
            }
            current = current->next;
        }

        if (nearest != nullptr) {
            cout << "\n=== Nearest " << (filterByType ? facilityTypeToString(searchType) : "Facility") << " ===" << endl;
            cout << "Name: " << nearest->name << endl;
            cout << "Type: " << facilityTypeToString(nearest->type) << endl;
            cout << "Sector: " << nearest->sector << endl;
            cout << "Timings: " << (nearest->isOpen24Hours ? "Open 24 Hours" :
                nearest->openingTime + " - " + nearest->closingTime) << endl;
            if (nearest->rating > 0) {
                cout << "Rating: " << nearest->rating << "/5.0" << endl;
            }
            cout << "Distance: " << minDist << " units" << endl;
        }
        else {
            cout << "No " << (filterByType ? facilityTypeToString(searchType) + " " : "")
                << "facilities found." << endl;
        }
    }

    void findFacilityWithAmenity() {
        cout << "\n=== Find Facility with Specific Amenity ===" << endl;

        string amenityName = getStringInput("Enter Amenity Name (e.g., Parking, WiFi): ");
        double userLat = getDoubleInput("Enter your Latitude: ");
        double userLon = getDoubleInput("Enter your Longitude: ");

        FacilityNode* nearestFacility = nullptr;
        double minDist = DBL_MAX;

        FacilityNode* facility = facilitiesHead;
        while (facility != nullptr) {
            AmenityNode* amenity = findAmenity(facility, amenityName);
            if (amenity != nullptr && amenity->available) {
                double dist = calculateDistance(userLat, userLon, facility->lat, facility->lon);
                if (dist < minDist) {
                    minDist = dist;
                    nearestFacility = facility;
                }
            }
            facility = facility->next;
        }

        if (nearestFacility != nullptr) {
            cout << "\n=== Nearest Facility with '" << amenityName << "' ===" << endl;
            cout << "Name: " << nearestFacility->name << endl;
            cout << "Type: " << facilityTypeToString(nearestFacility->type) << endl;
            cout << "Sector: " << nearestFacility->sector << endl;
            cout << "Distance: " << minDist << " units" << endl;
        }
        else {
            cout << "No facility found with available '" << amenityName << "'." << endl;
        }
    }


    // Dijikistra Algorithm

    void findShortestPath() {
        cout << "\n=== Find Shortest Path Between Facilities (Dijkstra) ===" << endl;

        if (facilityCount < 2) {
            cout << "Error: Need at least 2 facilities." << endl;
            return;
        }

        string srcName = getStringInput("Enter Source Facility Name: ");
        FacilityNode* srcFacility = findFacilityByName(srcName);
        if (srcFacility == nullptr) {
            cout << "Error: Source facility not found!" << endl;
            return;
        }

        string destName = getStringInput("Enter Destination Facility Name: ");
        FacilityNode* destFacility = findFacilityByName(destName);
        if (destFacility == nullptr) {
            cout << "Error: Destination facility not found!" << endl;
            return;
        }

        if (stringsEqualIgnoreCase(srcFacility->facilityID, destFacility->facilityID)) {
            cout << "Source and destination are the same!" << endl;
            return;
        }

        // Initialize arrays
        const int MAX_FACILITIES = 500;
        string facilityIDs[MAX_FACILITIES];
        double distances[MAX_FACILITIES];
        string previous[MAX_FACILITIES];
        bool visited[MAX_FACILITIES];
        int numFacilities = 0;

        // Collect all facility IDs
        FacilityNode* current = facilitiesHead;
        while (current != nullptr && numFacilities < MAX_FACILITIES) {
            facilityIDs[numFacilities] = current->facilityID;
            distances[numFacilities] = DBL_MAX;
            previous[numFacilities] = "";
            visited[numFacilities] = false;
            numFacilities++;
            current = current->next;
        }

        // Find source index
        int srcIdx = -1;
        for (int i = 0; i < numFacilities; i++) {
            if (stringsEqualIgnoreCase(facilityIDs[i], srcFacility->facilityID)) {
                srcIdx = i;
                break;
            }
        }
        if (srcIdx == -1) {
            cout << "Error: Source facility not in graph!" << endl;
            return;
        }

        distances[srcIdx] = 0;

        // Dijkstra's algorithm
        FacilityDijkstraHeap minHeap;
        minHeap.insert(srcFacility->facilityID, 0);

        while (!minHeap.isEmpty()) {
            FacilityDijkstraNode minNode = minHeap.extractMin();
            string currentID = minNode.facilityID;

            int currentIdx = -1;
            for (int i = 0; i < numFacilities; i++) {
                if (stringsEqualIgnoreCase(facilityIDs[i], currentID)) {
                    currentIdx = i;
                    break;
                }
            }

            if (currentIdx == -1 || visited[currentIdx]) continue;
            visited[currentIdx] = true;

            // Process neighbors
            FacilityAdjNode* adj = getAdjList(currentID);
            if (adj != nullptr) {
                FacilityEdge* edge = adj->edgeHead;
                while (edge != nullptr) {
                    int neighborIdx = -1;
                    for (int i = 0; i < numFacilities; i++) {
                        if (stringsEqualIgnoreCase(facilityIDs[i], edge->destFacilityID)) {
                            neighborIdx = i;
                            break;
                        }
                    }

                    if (neighborIdx != -1 && !visited[neighborIdx]) {
                        double newDist = distances[currentIdx] + edge->distance;
                        if (newDist < distances[neighborIdx]) {
                            distances[neighborIdx] = newDist;
                            previous[neighborIdx] = currentID;
                            minHeap.insert(edge->destFacilityID, newDist);
                        }
                    }
                    edge = edge->next;
                }
            }
        }

        // Find destination index
        int destIdx = -1;
        for (int i = 0; i < numFacilities; i++) {
            if (stringsEqualIgnoreCase(facilityIDs[i], destFacility->facilityID)) {
                destIdx = i;
                break;
            }
        }

        if (destIdx == -1 || distances[destIdx] == DBL_MAX) {
            cout << "\nNo path exists between '" << srcFacility->name
                << "' and '" << destFacility->name << "'!" << endl;
            cout << "Tip: Make sure the facilities are connected." << endl;
            return;
        }

        // Reconstruct path
        string path[MAX_FACILITIES];
        int pathLen = 0;
        string currentID = destFacility->facilityID;

        while (!currentID.empty() && pathLen < MAX_FACILITIES) {
            path[pathLen++] = currentID;
            int idx = -1;
            for (int i = 0; i < numFacilities; i++) {
                if (stringsEqualIgnoreCase(facilityIDs[i], currentID)) {
                    idx = i;
                    break;
                }
            }
            if (idx == -1) break;
            currentID = previous[idx];
        }

        // Display path
        cout << "\n=== Shortest Path Found ===" << endl;
        cout << "From: " << srcFacility->name << endl;
        cout << "To: " << destFacility->name << endl;
        cout << "Total Distance: " << distances[destIdx] << " units" << endl;
        cout << "\nPath: ";

        for (int i = pathLen - 1; i >= 0; i--) {
            FacilityNode* facility = findFacilityByID(path[i]);
            if (facility != nullptr) {
                cout << facility->name;
                if (i > 0) cout << " -> ";
            }
        }
        cout << endl;
    }


    // Display Functions

    void displayAllFacilities() {
        cout << "\n=== All Public Facilities ===" << endl;
        FacilityNode* current = facilitiesHead;
        int count = 0;
        while (current != nullptr) {
            cout << ++count << ".  [" << current->facilityID << "] " << current->name << endl;
            cout << "   Type: " << facilityTypeToString(current->type) << " | Sector: " << current->sector << endl;
            cout << "   Timings: " << (current->isOpen24Hours ? "Open 24 Hours" : current->openingTime + " - " + current->closingTime);
            if (current->rating > 0) {
                cout << " | Rating: " << current->rating << "/5.0";
            }
            cout << endl;
            current = current->next;
        }
        if (count == 0) {
            cout << "No facilities registered." << endl;
        }
    }

    void displayFacilityDetails() {
        cout << "\n=== Facility Details ===" << endl;

        FacilityNode* facility = selectFacility();
        if (facility == nullptr) return;

        cout << "\n============================================" << endl;
        cout << "Facility: " << facility->name << endl;
        cout << "ID: " << facility->facilityID << endl;
        cout << "Type: " << facilityTypeToString(facility->type) << endl;
        cout << "Sector: " << facility->sector << endl;
        cout << "Location: (" << facility->lat << ", " << facility->lon << ")" << endl;
        cout << "Timings: " << (facility->isOpen24Hours ? "Open 24 Hours" : facility->openingTime + " - " + facility->closingTime) << endl;
        cout << "Rating: " << (facility->rating > 0 ? to_string(facility->rating) + "/5.0" : "Not rated") << endl;
        cout << "============================================" << endl;

        // Display amenities
        cout << "Amenities (" << facility->amenityCount << "):" << endl;
        if (facility->amenityHead == nullptr) {
            cout << "  (No amenities listed)" << endl;
        }
        else {
            AmenityNode* amenity = facility->amenityHead;
            while (amenity != nullptr) {
                cout << " - " << amenity->name << " ["
                    << (amenity->available ? "Available" : "Unavailable") << "]" << endl;
                amenity = amenity->next;
            }
        }

        // Display connections
        cout << "\n Connected Facilities " << endl;
        FacilityAdjNode* adj = getAdjList(facility->facilityID);
        if (adj != nullptr && adj->edgeHead != nullptr) {
            FacilityEdge* edge = adj->edgeHead;
            while (edge != nullptr) {
                FacilityNode* connectedFacility = findFacilityByID(edge->destFacilityID);
                if (connectedFacility != nullptr) {
                    cout << "  -> " << connectedFacility->name
                        << " (" << edge->connectionType << ", " << edge->distance << " units)" << endl;
                }
                edge = edge->next;
            }
        }
        else {
            cout << "  No connections " << endl;
        }
    }

    void displayFacilityNetwork() {
        cout << "\n=== Facility Network Adjacency List ===" << endl;

        FacilityAdjNode* adj = adjListHead;
        while (adj != nullptr) {
            FacilityNode* facility = findFacilityByID(adj->facilityID);
            cout << "[" << (facility ? facility->name : adj->facilityID) << "] -> ";

            FacilityEdge* edge = adj->edgeHead;
            if (edge == nullptr) {
                cout << "(no connections)";
            }
            while (edge != nullptr) {
                FacilityNode* dest = findFacilityByID(edge->destFacilityID);
                cout << (dest ? dest->name : edge->destFacilityID)
                    << "(" << edge->distance << ", " << edge->connectionType << ")";
                if (edge->next != nullptr) cout << ", ";
                edge = edge->next;
            }
            cout << endl;
            adj = adj->next;
        }

        if (adjListHead == nullptr) {
            cout << "(No facilities registered)" << endl;
        }
    }

    void displayByType() {
        cout << "\n=== Display Facilities by Type ===" << endl;
        int type = selectFacilityType();
        typeHash.searchByType(type);
    }

    void displayStatistics() {
        cout << "\n=== Facilities System Statistics ===" << endl;
        cout << "Total Facilities: " << facilityCount << endl;
        cout << "Total Connections: " << getConnectionCount() << endl;

        // Count by type
        cout << "\n--- Breakdown by Type ---" << endl;
        // Loop using int iteration
        for (int t = 0; t <= FACILITY_OTHER; t++) {
            int type = t;
            int count = typeHash.countByType(type);
            if (count > 0) {
                cout << facilityTypeToString(type) << ": " << count << endl;
            }
        }

        // Count total amenities
        int totalAmenities = 0;
        FacilityNode* facility = facilitiesHead;
        while (facility != nullptr) {
            totalAmenities += facility->amenityCount;
            facility = facility->next;
        }
        cout << "\nTotal Amenities: " << totalAmenities << endl;

        // Find highest rated facility
        FacilityNode* topRated = nullptr;
        double maxRating = 0;
        facility = facilitiesHead;
        while (facility != nullptr) {
            if (facility->rating > maxRating) {
                maxRating = facility->rating;
                topRated = facility;
            }
            facility = facility->next;
        }

        if (topRated != nullptr && maxRating > 0) {
            cout << "Highest Rated: " << topRated->name
                << " (" << maxRating << "/5.0)" << endl;
        }
    }

    void display24HourFacilities() {
        cout << "\n=== 24-Hour Facilities ===" << endl;

        bool found = false;
        FacilityNode* facility = facilitiesHead;
        while (facility != nullptr) {
            if (facility->isOpen24Hours) {
                cout << "- " << facility->name << " [" << facilityTypeToString(facility->type) << "]" << endl;
                cout << "  Sector: " << facility->sector << endl;
                found = true;
            }
            facility = facility->next;
        }

        if (!found) {
            cout << "No 24-hour facilities found." << endl;
        }
    }

    // File Loading

    void loadFromFile() {
        GlobalLocationManager* locMgr = GlobalLocationManager::getInstance();

        ifstream file("facilities.csv");
        if (!file.is_open()) {
            cout << "Warning: facilities.csv not found." << endl;
            return;
        }

        string line;
        getline(file, line); // Skip header
        int loadedCount = 0;

        while (getline(file, line)) {
            if (line.empty())
                continue;

            // Normalize separators
            for (size_t i = 0; i < line.length(); i++) {
                if (line[i] == '\t') line[i] = ',';
            }

            string parts[7];
            int count = splitString(line, ',', parts, 7);

            if (count >= 5) {
                string id = trimString(parts[0]);
                string name = trimString(parts[1]);
                string typeStr = trimString(parts[2]);
                string sector = trimString(parts[3]);
                string timings = trimString(parts[4]);

                if (findFacilityByID(id) != nullptr || findFacilityByName(name) != nullptr) {
                    continue;
                }

                int type = stringToFacilityType(typeStr);

                // Parse timings
                bool is24Hours = (toLowerStr(timings).find("24") != string::npos);
                string openTime = "9:00 AM";
                string closeTime = "5:00 PM";

                if (!is24Hours && timings.find("-") != string::npos) {
                    string timeParts[2];
                    splitString(timings, '-', timeParts, 2);
                    openTime = trimString(timeParts[0]);
                    closeTime = trimString(timeParts[1]);
                }

                // Generate coordinates based on sector
                double lat, lon;
                double facilityOffset = 0.006;

                if (sector == "F-9") { lat = 33.700 + facilityOffset; lon = 73.025 + facilityOffset; }
                else if (sector == "G-10") { lat = 33.684 + facilityOffset; lon = 73.025 + facilityOffset; }
                else if (sector == "F-6") { lat = 33.727 + facilityOffset; lon = 73.075 + facilityOffset; }
                else if (sector == "G-9") { lat = 33.700 + facilityOffset; lon = 73.030 + facilityOffset; }
                else if (sector == "E-7") { lat = 33.740 + facilityOffset; lon = 73.060 + facilityOffset; }
                else {
                    lat = 33.700 + (loadedCount * 0.004) + facilityOffset;
                    lon = 73.000 + (loadedCount * 0.004) + facilityOffset;
                }

                if (!locMgr->registerLocation(lat, lon, "Facility", name)) {
                    lat += 0.001;
                    lon += 0.001;
                    if (!locMgr->registerLocation(lat, lon, "Facility", name)) {
                        continue;
                    }
                }

                FacilityNode* newFacility = new FacilityNode(id, name, type, sector,
                    lat, lon, openTime, closeTime, is24Hours);

                // Parse rating if available
                if (count >= 6 && !parts[5].empty()) {
                    newFacility->rating = stringToDouble(trimString(parts[5]));
                }

                newFacility->next = facilitiesHead;
                facilitiesHead = newFacility;
                facilityCount++;

                getOrCreateAdjList(id);
                typeHash.insert(type, newFacility);

                // Register with CityGraph for unified city-wide graph (LOC_FACILITY is int)
                CityGraph::getInstance()->addLocation(id, name, LOC_FACILITY, lat, lon, newFacility);

                loadedCount++;
            }
        }

        file.close();
        cout << "Loaded " << loadedCount << " facilities from file." << endl;
    }

    // Getters

    int getFacilityCount() {
        return facilityCount;
    }

    // Destructors

    ~FacilitiesSystem() {
        // Delete all facilities
        while (facilitiesHead != nullptr) {
            FacilityNode* facilityTemp = facilitiesHead;
            facilitiesHead = facilitiesHead->next;

            // Delete all amenities
            while (facilityTemp->amenityHead != nullptr) {
                AmenityNode* amenityTemp = facilityTemp->amenityHead;
                facilityTemp->amenityHead = facilityTemp->amenityHead->next;
                delete amenityTemp;
            }
            delete facilityTemp;
        }

        // Delete adjacency list
        while (adjListHead != nullptr) {
            FacilityAdjNode* adjTemp = adjListHead;
            adjListHead = adjListHead->next;

            // Delete all edges
            while (adjTemp->edgeHead != nullptr) {
                FacilityEdge* edgeTemp = adjTemp->edgeHead;
                adjTemp->edgeHead = adjTemp->edgeHead->next;
                delete edgeTemp;
            }
            delete adjTemp;
        }
    }
};
