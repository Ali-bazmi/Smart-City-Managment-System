#pragma once
#include "Utils.h"

// GLOBAL LOCATION MANAGER

struct LocationNode {
    double lat;
    double lon;
    string entityType;  // "Hospital", "School", "Mall", "Facility", "BusStop"
    string entityName;
    LocationNode* next;

    LocationNode(double la, double lo, const string& type, const string& name)
        : lat(la), lon(lo), entityType(type), entityName(name), next(nullptr) {
    }
};

class GlobalLocationManager {
private:
    LocationNode* head;
    static GlobalLocationManager* instance;

    // Singleton Class
    GlobalLocationManager() : head(nullptr) {}

public:
   
    // Resets all the locations 
    void reset() {
        LocationNode* current = head;
        while (current != nullptr) {
            LocationNode* temp = current;
            current = current->next;
            delete temp;
        }
        head = nullptr;
    }
    static GlobalLocationManager* getInstance() {
        if (instance == nullptr) {
            instance = new GlobalLocationManager();
        }
        return instance;
    }

    // Checks that location occupied or not 
    bool isLocationOccupied(double lat, double lon, string& occupiedBy) {
        LocationNode* current = head;
        while (current != nullptr) {
            if (coordinatesMatch(current->lat, current->lon, lat, lon)) {
                occupiedBy = current->entityType + ": " + current->entityName;
                return true;
            }
            current = current->next;
        }
        return false;
    }

    // Register new Location
    bool registerLocation(double lat, double lon, const string& entityType, const string& entityName) {
        string occupiedBy;
        if (isLocationOccupied(lat, lon, occupiedBy)) {
            cout << "Error: Location (" << lat << ", " << lon << ") is already occupied by " << occupiedBy << endl;
            return false;
        }

        LocationNode* newNode = new LocationNode(lat, lon, entityType, entityName);
        newNode->next = head;
        head = newNode;
        return true;
    }

    // Removes the Location
    bool removeLocation(double lat, double lon) {
        if (head == nullptr) return false;

        if (coordinatesMatch(head->lat, head->lon, lat, lon)) {
            LocationNode* temp = head;
            head = head->next;
            delete temp;
            return true;
        }

        LocationNode* current = head;
        while (current->next != nullptr) {
            if (coordinatesMatch(current->next->lat, current->next->lon, lat, lon)) {
                LocationNode* temp = current->next;
                current->next = current->next->next;
                delete temp;
                return true;
            }
            current = current->next;
        }
        return false;
    }

    // Tells how many locations are there
    int getLocationCount() {
        int count = 0;
        LocationNode* current = head;
        while (current != nullptr) {
            count++;
            current = current->next;
        }
        return count;
    }

    // Displays all the locations 
    void displayAllLocations() {
        cout << "\n=== All Registered Locations ===" << endl;
        LocationNode* current = head;
        int count = 0;
        while (current != nullptr) {
            cout << ++count << ".  [" << current->entityType << "] " << current->entityName << " at (" << current->lat << ", " << current->lon << ")" << endl;
            current = current->next;
        }
        if (count == 0) {
            cout << "No locations registered yet." << endl;
        }
    }

    ~GlobalLocationManager() {
        while (head != nullptr) {
            LocationNode* temp = head;
            head = head->next;
            delete temp;
        }
    }
};

GlobalLocationManager* GlobalLocationManager::instance = nullptr;
