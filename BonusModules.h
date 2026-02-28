#pragma once
#include "Utils.h"
#include "CityGraph.h"
#include "GlobalLocationManager.h"
#include "Transport.h"
#include "Education.h"
#include "Population.h"
#include <ctime>

// Transport Hub Types
const int HUB_AIRPORT = 0;
const int HUB_RAILWAY = 1;
const int HUB_METRO = 2;
const int HUB_BUS_TERMINAL = 3;

string hubTypeToString(int type) {
    switch (type) {
    case HUB_AIRPORT: 
        return "Airport";
    case HUB_RAILWAY:
        return "Railway Station";
    case HUB_METRO: 
        return "Metro Station";
    case HUB_BUS_TERMINAL:
        return "Bus Terminal";
    default: return "Unknown";
    }
}

struct ScheduleEntry {
    string vehicleID;       // Flight / Train number
    string destination; 
    string departureTime;
    string arrivalTime;
    string status;         
    int platform;           // Platforms / Gate number 
    ScheduleEntry* next;

    ScheduleEntry(const string& vid, const string& dest, const string& dep,
        const string& arr, int plat)
        : vehicleID(vid), destination(dest), departureTime(dep), arrivalTime(arr),
        status("On Time"), platform(plat), next(nullptr) {
    }
};

// Transport Hub Node
struct TransportHubNode {
    string hubID;
    string name;
    int type;
    string sector;
    double lat, lon;
    int totalPlatforms;
    bool isOperational;
    ScheduleEntry* departuresHead;
    ScheduleEntry* arrivalsHead;
    TransportHubNode* next;

    TransportHubNode(const string& id, const string& n, int t, const string& sec, double la, double lo, int platforms)
        : hubID(id), name(n), type(t), sector(sec), lat(la), lon(lo), totalPlatforms(platforms), isOperational(true),
        departuresHead(nullptr), arrivalsHead(nullptr), next(nullptr) {
    }
};

// Connected Bus Stop 
struct ConnectedStopNode {
    string hubID;
    string stopID;
    double distance;
    int shuttleFrequency;  
    ConnectedStopNode* next;

    ConnectedStopNode(const string& hid, const string& sid, double dist, int freq)
        : hubID(hid), stopID(sid), distance(dist), shuttleFrequency(freq), next(nullptr) {
    }
};

// TransportHubSystem 
class TransportHubSystem {
private:
    TransportHubNode* hubsHead;
    ConnectedStopNode* connectionsHead;
    int hubCount;
    int hubIDCounter;

    TransportHubNode* findHubByID(const string& id) {
        TransportHubNode* current = hubsHead;
        while (current != nullptr) {
            if (stringsEqualIgnoreCase(current->hubID, id)) 
                return current;
            current = current->next;
        }
        return nullptr;
    }

    TransportHubNode* findHubByName(const string& name) {
        TransportHubNode* current = hubsHead;
        while (current != nullptr) {
            if (stringsEqualIgnoreCase(current->name, name))
                return current;
            current = current->next;
        }
        return nullptr;
    }

public:
    TransportHubSystem() : hubsHead(nullptr), connectionsHead(nullptr),
        hubCount(0), hubIDCounter(1) {
    }

    void registerTransportHub() {
        cout << "\n=== Register Transport Hub ===" << endl;

        string name = getStringInput("Enter Hub Name: ");
        if (name.empty()) {
            cout << "Error: Hub name cannot be empty." << endl;
            return;
        }

        if (findHubByName(name) != nullptr) {
            cout << "Error: Hub with this name already exists!" << endl;
            return;
        }

        cout << "\nHub Type:" << endl;
        cout << "1.  Airport" << endl;
        cout << "2. Railway Station" << endl;
        cout << "3.  Metro Station" << endl;
        cout << "4. Bus Terminal" << endl;
        int typeChoice = getIntInput("Enter type: ");
        int type = (typeChoice >= 1 && typeChoice <= 4) ? typeChoice - 1 : HUB_BUS_TERMINAL;

        string sector = getStringInput("Enter Sector: ");
        double lat = getDoubleInput("Enter Latitude: ");
        double lon = getDoubleInput("Enter Longitude: ");
        int platforms = getIntInput("Enter Number of Platforms/Gates: ");
        if (platforms <= 0) platforms = 1;

        GlobalLocationManager* locMgr = GlobalLocationManager::getInstance();
        if (!locMgr->registerLocation(lat, lon, hubTypeToString(type), name)) {
            return;
        }

        string hubID = "HUB" + intToString(hubIDCounter++);

        TransportHubNode* newHub = new TransportHubNode(hubID, name, type, sector,
            lat, lon, platforms);
        newHub->next = hubsHead;
        hubsHead = newHub;
        hubCount++;

        CityGraph::getInstance()->addLocation(hubID, name, LOC_BUS_STOP, lat, lon, newHub);

        cout << "\nTransport Hub registered successfully!" << endl;
        cout << "Hub ID: " << hubID << endl;
        cout << "Name: " << name << endl;
        cout << "Type: " << hubTypeToString(type) << endl;
        cout << "Platforms/Gates: " << platforms << endl;
    }

    void addDeparture() {
        cout << "\n=== Add Departure Schedule ===" << endl;

        string hubID = getStringInput("Enter Hub ID: ");
        TransportHubNode* hub = findHubByID(hubID);
        if (hub == nullptr) {
            cout << "Error: Hub not found!" << endl;
            return;
        }

        string vehicleID = getStringInput("Enter Flight/Train Number: ");
        string destination = getStringInput("Enter Destination: ");
        string depTime = getStringInput("Enter Departure Time (HH:MM): ");
        string arrTime = getStringInput("Enter Estimated Arrival Time (HH:MM): ");
        int platform = getIntInput("Enter Platform/Gate Number: ");

        if (platform < 1 || platform > hub->totalPlatforms) {
            cout << "Warning: Platform number out of range.  Setting to 1." << endl;
            platform = 1;
        }

        ScheduleEntry* newEntry = new ScheduleEntry(vehicleID, destination, depTime, arrTime, platform);

        newEntry->next = hub->departuresHead;
        hub->departuresHead = newEntry;

        cout << "\nDeparture added!" << endl;
        cout << vehicleID << " to " << destination << " at " << depTime << endl;
        cout << "Platform/Gate: " << platform << endl;
    }

    void addArrival() {
        cout << "\n=== Add Arrival Schedule ===" << endl;

        string hubID = getStringInput("Enter Hub ID: ");
        TransportHubNode* hub = findHubByID(hubID);
        if (hub == nullptr) {
            cout << "Error: Hub not found!" << endl;
            return;
        }

        string vehicleID = getStringInput("Enter Flight/Train Number: ");
        string origin = getStringInput("Enter Origin: ");
        string depTime = getStringInput("Enter Departure Time from Origin (HH:MM): ");
        string arrTime = getStringInput("Enter Arrival Time (HH:MM): ");
        int platform = getIntInput("Enter Platform/Gate Number: ");

        ScheduleEntry* newEntry = new ScheduleEntry(vehicleID, origin,
            depTime, arrTime, platform);

        newEntry->next = hub->arrivalsHead;
        hub->arrivalsHead = newEntry;

        cout << "\nArrival added!" << endl;
        cout << vehicleID << " from " << origin << " at " << arrTime << endl;
    }

    void updateScheduleStatus() {
        cout << "\n=== Update Schedule Status ===" << endl;

        string hubID = getStringInput("Enter Hub ID: ");
        TransportHubNode* hub = findHubByID(hubID);
        if (hub == nullptr) {
            cout << "Error: Hub not found!" << endl;
            return;
        }

        string vehicleID = getStringInput("Enter Flight/Train Number: ");

        cout << "\nNew Status:" << endl;
        cout << "1. On Time" << endl;
        cout << "2.  Delayed" << endl;
        cout << "3. Cancelled" << endl;
        cout << "4.  Boarding" << endl;
        cout << "5. Departed" << endl;
        cout << "6. Arrived" << endl;
        int statusChoice = getIntInput("Enter status: ");

        string newStatus;
        switch (statusChoice) {
        case 1: newStatus = "On Time"; 
            break;
        case 2: newStatus = "Delayed";
            break;
        case 3: newStatus = "Cancelled"; 
            break;
        case 4: newStatus = "Boarding"; 
            break;
        case 5: newStatus = "Departed";
            break;
        case 6: newStatus = "Arrived"; 
            break;
        default: newStatus = "On Time";
        }

        // Search in departures
        ScheduleEntry* entry = hub->departuresHead;
        while (entry != nullptr) {
            if (stringsEqualIgnoreCase(entry->vehicleID, vehicleID)) {
                entry->status = newStatus;
                cout << "Status updated to: " << newStatus << endl;
                return;
            }
            entry = entry->next;
        }

        // Search in arrivals
        entry = hub->arrivalsHead;
        while (entry != nullptr) {
            if (stringsEqualIgnoreCase(entry->vehicleID, vehicleID)) {
                entry->status = newStatus;
                cout << "Status updated to: " << newStatus << endl;
                return;
            }
            entry = entry->next;
        }

        cout << "Error: Vehicle not found in schedule!" << endl;
    }

    void connectToBusNetwork() {
        cout << "\n=== Connect Hub to Bus Network ===" << endl;

        string hubID = getStringInput("Enter Hub ID: ");
        TransportHubNode* hub = findHubByID(hubID);
        if (hub == nullptr) {
            cout << "Error: Hub not found!" << endl;
            return;
        }

        string stopID = getStringInput("Enter Bus Stop ID to connect: ");
        int shuttleFreq = getIntInput("Enter Shuttle Frequency (minutes): ");
        if (shuttleFreq <= 0) shuttleFreq = 15;

        double distance = 0;
        CityLocation* stopLoc = CityGraph::getInstance()->findLocationByID(stopID);
        if (stopLoc != nullptr) {
            distance = calculateDistance(hub->lat, hub->lon, stopLoc->lat, stopLoc->lon);
        }

        ConnectedStopNode* newConn = new ConnectedStopNode(hubID, stopID, distance, shuttleFreq);
        newConn->next = connectionsHead;
        connectionsHead = newConn;

        // Also add edge in city graph
        CityGraph::getInstance()->connectLocations(hubID, stopID, distance);

        cout << "Hub connected to bus stop!" << endl;
        cout << "Distance: " << distance << " units" << endl;
        cout << "Shuttle every " << shuttleFreq << " minutes" << endl;
    }

    void viewHubSchedule() {
        cout << "\n=== View Hub Schedule ===" << endl;

        string hubID = getStringInput("Enter Hub ID: ");
        TransportHubNode* hub = findHubByID(hubID);
        if (hub == nullptr) {
            cout << "Error: Hub not found!" << endl;
            return;
        }

        cout << "\n============================================" << endl;
        cout << hub->name << " (" << hubTypeToString(hub->type) << ")" << endl;
        cout << "Status: " << (hub->isOperational ? "OPERATIONAL" : "CLOSED") << endl;
        cout << "Platforms/Gates: " << hub->totalPlatforms << endl;
        cout << "============================================" << endl;

        cout << "\n--- DEPARTURES ---" << endl;
        cout << "+----------+------------------+-------+------+------------+" << endl;
        cout << "| Number   | Destination      | Time  | Gate | Status     |" << endl;
        cout << "+----------+------------------+-------+------+------------+" << endl;

        ScheduleEntry* dep = hub->departuresHead;
        if (dep == nullptr) {
            cout << "| No departures scheduled                              |" << endl;
        }
        while (dep != nullptr) {
            cout << "| " << left << setw(8) << dep->vehicleID << " | ";
            cout << left << setw(16) << dep->destination << " | ";
            cout << left << setw(5) << dep->departureTime << " | ";
            cout << right << setw(4) << dep->platform << " | ";
            cout << left << setw(10) << dep->status << " |" << endl;
            dep = dep->next;
        }
        cout << "+----------+------------------+-------+------+------------+" << endl;

        cout << "\n--- ARRIVALS ---" << endl;
        cout << "+----------+------------------+-------+------+------------+" << endl;
        cout << "| Number   | Origin           | Time  | Gate | Status     |" << endl;
        cout << "+----------+------------------+-------+------+------------+" << endl;

        ScheduleEntry* arr = hub->arrivalsHead;
        if (arr == nullptr) {
            cout << "| No arrivals scheduled                                |" << endl;
        }
        while (arr != nullptr) {
            cout << "| " << left << setw(8) << arr->vehicleID << " | ";
            cout << left << setw(16) << arr->destination << " | ";
            cout << left << setw(5) << arr->arrivalTime << " | ";
            cout << right << setw(4) << arr->platform << " | ";
            cout << left << setw(10) << arr->status << " |" << endl;
            arr = arr->next;
        }
        cout << "+----------+------------------+-------+------+------------+" << endl;
    }

    void displayAllHubs() {
        cout << "\n=== All Transport Hubs ===" << endl;

        if (hubsHead == nullptr) {
            cout << "No transport hubs registered." << endl;
            return;
        }

        TransportHubNode* hub = hubsHead;
        int count = 0;
        while (hub != nullptr) {
            cout << ++count << ". [" << hub->hubID << "] " << hub->name << endl;
            cout << "   Type: " << hubTypeToString(hub->type) << endl;
            cout << "   Sector: " << hub->sector << " | Platforms: " << hub->totalPlatforms << endl;
            cout << "   Status: " << (hub->isOperational ? "Operational" : "Closed") << endl;
            hub = hub->next;
        }

        cout << "\nTotal Hubs: " << hubCount << endl;
    }

    void findNearestHub() {
        cout << "\n=== Find Nearest Transport Hub ===" << endl;

        double userLat = getDoubleInput("Enter your Latitude: ");
        double userLon = getDoubleInput("Enter your Longitude: ");

        cout << "\nSearch for specific type?" << endl;
        cout << "0. Any hub" << endl;
        cout << "1. Airport" << endl;
        cout << "2. Railway Station" << endl;
        cout << "3. Metro Station" << endl;
        cout << "4.  Bus Terminal" << endl;
        int typeChoice = getIntInput("Enter choice: ");
        bool filterType = (typeChoice >= 1 && typeChoice <= 4);
        int searchType = filterType ? typeChoice - 1 : -1;

        TransportHubNode* nearest = nullptr;
        double minDist = DBL_MAX;

        TransportHubNode* hub = hubsHead;
        while (hub != nullptr) {
            if (!filterType || hub->type == searchType) {
                double dist = calculateDistance(userLat, userLon, hub->lat, hub->lon);
                if (dist < minDist) {
                    minDist = dist;
                    nearest = hub;
                }
            }
            hub = hub->next;
        }

        if (nearest != nullptr) {
            cout << "\n=== Nearest Hub Found ===" << endl;
            cout << "Name: " << nearest->name << endl;
            cout << "Type: " << hubTypeToString(nearest->type) << endl;
            cout << "Sector: " << nearest->sector << endl;
            cout << "Distance: " << minDist << " units" << endl;
        }
        else {
            cout << "No hubs found." << endl;
        }
    }

    void loadDefaultHubs() {
        cout << "\nLoading default transport hubs..." << endl;

        GlobalLocationManager* locMgr = GlobalLocationManager::getInstance();

        // Islamabad International Airport
        if (locMgr->registerLocation(33.549, 72.824, "Airport", "Islamabad International Airport")) {
            TransportHubNode* airport = new TransportHubNode("HUB1", "Islamabad International Airport",
                HUB_AIRPORT, "Fateh Jang", 33.549, 72.824, 15);
            airport->next = hubsHead;
            hubsHead = airport;
            hubCount++;
            hubIDCounter = 2;

            // Add sample departures
            ScheduleEntry* dep1 = new ScheduleEntry("PK301", "Karachi", "08:00", "09:30", 1);
            ScheduleEntry* dep2 = new ScheduleEntry("PK205", "Lahore", "10:30", "11:15", 3);
            dep1->next = dep2;
            airport->departuresHead = dep1;

            // Add sample arrivals
            ScheduleEntry* arr1 = new ScheduleEntry("PK302", "Karachi", "06:00", "07:30", 2);
            airport->arrivalsHead = arr1;

            CityGraph::getInstance()->addLocation("HUB1", "Islamabad International Airport",
                LOC_BUS_STOP, 33.549, 72.824, airport);
        }

        // Rawalpindi Railway Station
        if (locMgr->registerLocation(33.597, 73.047, "Railway Station", "Rawalpindi Railway Station")) {
            TransportHubNode* railway = new TransportHubNode("HUB2", "Rawalpindi Railway Station",
                HUB_RAILWAY, "Saddar", 33.597, 73.047, 8);
            railway->next = hubsHead;
            hubsHead = railway;
            hubCount++;
            hubIDCounter = 3;

            ScheduleEntry* dep = new ScheduleEntry("Green Line", "Lahore", "14:00", "18:30", 1);
            railway->departuresHead = dep;

            CityGraph::getInstance()->addLocation("HUB2", "Rawalpindi Railway Station",
                LOC_BUS_STOP, 33.597, 73.047, railway);
        }

        cout << "Loaded " << hubCount << " transport hubs." << endl;
    }

    int getHubCount() { 
        return hubCount;
    }

    // Export transport hubs for visualization
    int exportHubs(string hubIDs[], string names[], double lats[], double lons[],
        int types[], bool operational[], int maxHubs) {
        int count = 0;
        TransportHubNode* hub = hubsHead;
        while (hub != nullptr && count < maxHubs) {
            hubIDs[count] = hub->hubID;
            names[count] = hub->name;
            lats[count] = hub->lat;
            lons[count] = hub->lon;
            types[count] = hub->type;
            operational[count] = hub->isOperational;
            count++;
            hub = hub->next;
        }
        return count;
    }

    ~TransportHubSystem() {
        while (hubsHead != nullptr) {
            TransportHubNode* temp = hubsHead;
            hubsHead = hubsHead->next;

            while (temp->departuresHead != nullptr) {
                ScheduleEntry* depTemp = temp->departuresHead;
                temp->departuresHead = temp->departuresHead->next;
                delete depTemp;
            }

            while (temp->arrivalsHead != nullptr) {
                ScheduleEntry* arrTemp = temp->arrivalsHead;
                temp->arrivalsHead = temp->arrivalsHead->next;
                delete arrTemp;
            }

            delete temp;
        }

        while (connectionsHead != nullptr) {
            ConnectedStopNode* temp = connectionsHead;
            connectionsHead = connectionsHead->next;
            delete temp;
        }
    }
};

// BONUS MODULES MENU SYSTEM GENERIC

class BonusModulesMenu {
private:
    TransportHubSystem transportHubSystem;

public:
    void displayMainMenu() {
        cout << "\n";
        cout << "+================================================================+" << endl;
        cout << "|             BONUS MODULES - EXTENDED FEATURES                  |" << endl;
        cout << "+================================================================+" << endl;
        cout << "|  1. Airport/Rail Integration                                   |" << endl;
        cout << "|  0. Return to Main Menu                                        |" << endl;
        cout << "+================================================================+" << endl;
    }

    void transportHubMenu() {
        int choice;
        do {
            cout << "\n=== Airport/Rail Integration ===" << endl;
            cout << "1. Register Transport Hub" << endl;
            cout << "2. Add Departure Schedule" << endl;
            cout << "3. Add Arrival Schedule" << endl;
            cout << "4. Update Schedule Status" << endl;
            cout << "5. Connect Hub to Bus Network" << endl;
            cout << "6. View Hub Schedule" << endl;
            cout << "7. Display All Hubs" << endl;
            cout << "8. Find Nearest Hub" << endl;
            cout << "0. Back" << endl;

            choice = getIntInput("Enter choice: ");

            switch (choice) {
            case 1: transportHubSystem.registerTransportHub(); break;
            case 2: transportHubSystem.addDeparture(); break;
            case 3: transportHubSystem.addArrival(); break;
            case 4: transportHubSystem.updateScheduleStatus(); break;
            case 5: transportHubSystem.connectToBusNetwork(); break;
            case 6: transportHubSystem.viewHubSchedule(); break;
            case 7: transportHubSystem.displayAllHubs(); break;
            case 8: transportHubSystem.findNearestHub(); break;
            case 0: break;
            default: cout << "Invalid choice!" << endl;
            }
        } while (choice != 0);
    }

    void run() {
        int choice;
        do {
            displayMainMenu();
            choice = getIntInput("Enter choice: ");

            switch (choice) {
            case 1: transportHubMenu(); break;
            case 0: cout << "Returning to main menu..." << endl; break;
            default: cout << "Invalid choice!" << endl;
            }
        } while (choice != 0);
    }

    TransportHubSystem& getTransportHubSystem() { return transportHubSystem; }
};
