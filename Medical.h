#pragma once
#include "Utils.h"
#include "GlobalLocationManager.h"
#include "CityGraph.h"

// MEDICAL MODULE  DYNAMIC MEMORY VERSION
// Dynamic Hash Table for medicines with resizing
// Dynamic Max Heap for emergency beds with resizing
// Linked list for hospitals and pharmacies


// Medicine Node (Hash Chain) 
struct MedicineNode {
    string name;
    string formula;
    int price;
    MedicineNode* next;

    MedicineNode(const string& n, const string& f, int p)
        : name(n), formula(f), price(p), next(nullptr) {
    }
};


// Doctor Node 
struct DoctorNode {
    string name;
    string specialization;
    int empID;
    DoctorNode* next;

    DoctorNode(const string& n, const string& spec, int id)
        : name(n), specialization(spec), empID(id), next(nullptr) {
    }
};


// Hospital Node 
struct HospitalNode {
    string hospitalID;
    string name;
    string sector;
    double lat, lon;
    int emergencyBeds;
    string specialization;
    DoctorNode* doctorHead;
    HospitalNode* next;

    HospitalNode(const string& id, const string& n, const string& sec, double la, double lo, int beds, const string& spec)
        : hospitalID(id), name(n), sector(sec), lat(la), lon(lo), emergencyBeds(beds), specialization(spec), doctorHead(nullptr), next(nullptr) {
    }
};

// Pharmacy Node with DYNAMIC Hash Table 
struct PharmacyNode {
    string pharmacyID;
    string name;
    string sector;
    double lat, lon;

    // Dynamic Hash Table
    MedicineNode** medTable;
    int medTableSize;
    int medicineCount;

    PharmacyNode* next;

    PharmacyNode(const string& id, const string& n, const string& sec, double la, double lo)
        : pharmacyID(id), name(n), sector(sec), lat(la), lon(lo), next(nullptr), medTableSize(10), medicineCount(0) {  // Start with size 10

        medTable = new MedicineNode * [medTableSize];
        for (int i = 0; i < medTableSize; i++) {
            medTable[i] = nullptr;
        }
    }

    // Destructor to clean up dynamic hash table
    ~PharmacyNode() {
        for (int i = 0; i < medTableSize; i++) {
            while (medTable[i] != nullptr) {
                MedicineNode* temp = medTable[i];
                medTable[i] = medTable[i]->next;
                delete temp;
            }
        }
        delete[] medTable;
    }

    // Resize hash table when load factor exceeds 0.75
    void resizeHashTable() {
        int oldSize = medTableSize;
        MedicineNode** oldTable = medTable;

        // Double the size
        medTableSize = oldSize * 2;
        medTable = new MedicineNode * [medTableSize];
        for (int i = 0; i < medTableSize; i++) {
            medTable[i] = nullptr;
        }

        // Rehash all medicines
        medicineCount = 0;
        for (int i = 0; i < oldSize; i++) {
            MedicineNode* current = oldTable[i];
            while (current != nullptr) {
                MedicineNode* next = current->next;

                // Re insert into new table
                int newIdx = polynomialHash(current->name, medTableSize);
                current->next = medTable[newIdx];
                medTable[newIdx] = current;
                medicineCount++;

                current = next;
            }
        }

        delete[] oldTable;
        cout << "  [Hash table resized: " << oldSize << " -> " << medTableSize << "]" << endl;
    }

    // Check if resize is needed
    bool needsResize() {
        double loadFactor = (double)medicineCount / medTableSize;
        return loadFactor > 0.75;
    }
};

// Dynamic max heap for Emergency Bed 

class EmergencyBedHeap {
private:
    HospitalNode** heap;  
    int size;
    int capacity;

    void swap(int i, int j) {
        HospitalNode* temp = heap[i];
        heap[i] = heap[j];
        heap[j] = temp;
    }

    void siftUp(int i) {
        while (i > 0) {
            int parent = (i - 1) / 2;
            if (heap[i]->emergencyBeds > heap[parent]->emergencyBeds) {
                swap(i, parent);
                i = parent;
            }
            else break;
        }
    }

    void siftDown(int i) {
        while (true) {
            int largest = i;
            int left = 2 * i + 1;
            int right = 2 * i + 2;

            if (left < size && heap[left]->emergencyBeds > heap[largest]->emergencyBeds)
                largest = left;
            if (right < size && heap[right]->emergencyBeds > heap[largest]->emergencyBeds)
                largest = right;

            if (largest != i) {
                swap(i, largest);
                i = largest;
            }
            else break;
        }
    }

    int findIndex(HospitalNode* hospital) {
        for (int i = 0; i < size; i++) {
            if (heap[i] == hospital) return i;
        }
        return -1;
    }

    // Resize HEAP when capacity is reached
    void resize() {
        int oldCapacity = capacity;
        capacity = capacity * 2;

        HospitalNode** newHeap = new HospitalNode * [capacity];
        for (int i = 0; i < size; i++) {
            newHeap[i] = heap[i];
        }

        delete[] heap;
        heap = newHeap;

        cout << "  [Heap resized: " << oldCapacity << " -> " << capacity << "]" << endl;
    }

public:
    bool contains(const string& hospitalName) {
        for (int i = 0; i < size; i++) {
            if (heap[i]->name == hospitalName) return true;
        }
        return false;
    }

    EmergencyBedHeap() : size(0), capacity(10) {  // Start with capacity 10
        heap = new HospitalNode * [capacity];
    }

    ~EmergencyBedHeap() {
        delete[] heap;
    }

    void insert(HospitalNode* hospital) {
        if (size >= capacity) {
            resize();  // Automatically resize when full
        }

        heap[size] = hospital;
        siftUp(size);
        size++;
    }

    void remove(HospitalNode* hospital) {
        int idx = findIndex(hospital);
        if (idx == -1) return;

        heap[idx] = heap[size - 1];
        size--;

        if (size > 0 && idx < size) {
            siftDown(idx);
            siftUp(idx);
        }
    }

    void rebuildHeap() {
        for (int i = size / 2 - 1; i >= 0; i--) {
            siftDown(i);
        }
    }

    HospitalNode* getMax() {
        if (size == 0) return nullptr;
        return heap[0];
    }

    HospitalNode* findWithMinBeds(int requiredBeds) {
        HospitalNode* best = nullptr;
        for (int i = 0; i < size; i++) {
            if (heap[i]->emergencyBeds >= requiredBeds) {
                if (best == nullptr || heap[i]->emergencyBeds < best->emergencyBeds) {
                    best = heap[i];
                }
            }
        }
        return best;
    }

    void displayAvailability() {
        if (size == 0) {
            cout << "No hospitals registered." << endl;
            return;
        }

        // Create temporary array for sorting
        HospitalNode** tempArr = new HospitalNode * [size];
        for (int i = 0; i < size; i++) {
            tempArr[i] = heap[i];
        }

        // Bubble sort
        for (int i = 0; i < size - 1; i++) {
            for (int j = 0; j < size - i - 1; j++) {
                if (tempArr[j]->emergencyBeds < tempArr[j + 1]->emergencyBeds) {
                    HospitalNode* temp = tempArr[j];
                    tempArr[j] = tempArr[j + 1];
                    tempArr[j + 1] = temp;
                }
            }
        }

        cout << "\n=== Emergency Bed Availability (Ranked by Beds) ===" << endl;
        int displayCount = (size < 10) ? size : 10;
        for (int i = 0; i < displayCount; i++) {
            cout << (i + 1) << ".  " << tempArr[i]->name << " [" << tempArr[i]->emergencyBeds
                << " beds] - " << tempArr[i]->specialization << endl;
        }

        delete[] tempArr;
    }

    int getSize() { return size; }
    int getCapacity() { return capacity; }
};

// Medical system manager 

class MedicalSystem {
private:
    HospitalNode* hospitalsHead;
    PharmacyNode* pharmaciesHead;
    EmergencyBedHeap bedHeap;
    int hospitalCount;
    int pharmacyCount;
    int hospitalIDCounter;
    int pharmacyIDCounter;

    int medHash(const string& key, int tableSize) {
        return polynomialHash(key, tableSize);
    }

    HospitalNode* findHospitalByName(const string& name) {
        HospitalNode* current = hospitalsHead;
        while (current != nullptr) {
            if (stringsEqualIgnoreCase(current->name, name)) return current;
            current = current->next;
        }
        return nullptr;
    }

    HospitalNode* findHospitalByID(const string& id) {
        HospitalNode* current = hospitalsHead;
        while (current != nullptr) {
            if (stringsEqualIgnoreCase(current->hospitalID, id)) return current;
            current = current->next;
        }
        return nullptr;
    }

    PharmacyNode* findPharmacyByName(const string& name) {
        PharmacyNode* current = pharmaciesHead;
        while (current != nullptr) {
            if (stringsEqualIgnoreCase(current->name, name)) return current;
            current = current->next;
        }
        return nullptr;
    }

    PharmacyNode* findPharmacyByID(const string& id) {
        PharmacyNode* current = pharmaciesHead;
        while (current != nullptr) {
            if (stringsEqualIgnoreCase(current->pharmacyID, id)) return current;
            current = current->next;
        }
        return nullptr;
    }

    bool isDoctorIDTaken(HospitalNode* hospital, int empID) {
        DoctorNode* doc = hospital->doctorHead;
        while (doc != nullptr) {
            if (doc->empID == empID) return true;
            doc = doc->next;
        }
        return false;
    }

    DoctorNode* findDoctor(HospitalNode* hospital, int empID) {
        DoctorNode* doc = hospital->doctorHead;
        while (doc != nullptr) {
            if (doc->empID == empID) return doc;
            doc = doc->next;
        }
        return nullptr;
    }

public:

    MedicalSystem() : hospitalsHead(nullptr), pharmaciesHead(nullptr), hospitalCount(0), pharmacyCount(0), hospitalIDCounter(1), pharmacyIDCounter(1) {
    }


    EmergencyBedHeap* getHeap() { return &bedHeap; }

    void registerHospital() {
        cout << "\n=== Register Hospital ===" << endl;

        string name = getStringInput("Enter Hospital Name: ");
        if (name.empty()) {
            cout << "Error: Hospital name cannot be empty." << endl;
            return;
        }

        if (findHospitalByName(name) != nullptr) {
            cout << "Error: Hospital '" << name << "' already exists!" << endl;
            return;
        }

        string sector = getStringInput("Enter Sector: ");
        double lat = getDoubleInput("Enter Latitude: ");
        double lon = getDoubleInput("Enter Longitude: ");

        GlobalLocationManager* locMgr = GlobalLocationManager::getInstance();
        if (!locMgr->registerLocation(lat, lon, "Hospital", name)) {
            return;
        }

        int beds = getIntInput("Enter Number of Emergency Beds: ");
        if (beds < 0) beds = 0;

        string specialization = getStringInput("Enter Specialization (e.g., General, Cardiology): ");

        string hospitalID = "H" + intToString(hospitalIDCounter++);

        HospitalNode* newHospital = new HospitalNode(hospitalID, name, sector, lat, lon, beds, specialization);
        newHospital->next = hospitalsHead;
        hospitalsHead = newHospital;
        hospitalCount++;

        bedHeap.insert(newHospital);

        CityGraph::getInstance()->addLocation(hospitalID, name, LOC_HOSPITAL, lat, lon, newHospital);

        cout << "Hospital '" << name << "' registered with ID: " << hospitalID << endl;
    }

    void deleteHospital() {
        cout << "\n=== Delete Hospital ===" << endl;

        string name = getStringInput("Enter Hospital Name to delete: ");
        HospitalNode* hospital = findHospitalByName(name);

        if (hospital == nullptr) {
            cout << "Error: Hospital not found!" << endl;
            return;
        }

        string confirm = getStringInput("Type 'YES' to confirm deletion of '" + name + "': ");
        if (confirm != "YES") {
            cout << "Deletion cancelled." << endl;
            return;
        }

        bedHeap.remove(hospital);

        if (stringsEqualIgnoreCase(hospitalsHead->name, name)) {
            HospitalNode* temp = hospitalsHead;
            hospitalsHead = hospitalsHead->next;

            while (temp->doctorHead != nullptr) {
                DoctorNode* docTemp = temp->doctorHead;
                temp->doctorHead = temp->doctorHead->next;
                delete docTemp;
            }

            GlobalLocationManager::getInstance()->removeLocation(temp->lat, temp->lon);
            CityGraph::getInstance()->removeLocation(temp->hospitalID);
            delete temp;
        }
        else {
            HospitalNode* current = hospitalsHead;
            while (current->next != nullptr) {
                if (stringsEqualIgnoreCase(current->next->name, name)) {
                    HospitalNode* temp = current->next;
                    current->next = current->next->next;

                    while (temp->doctorHead != nullptr) {
                        DoctorNode* docTemp = temp->doctorHead;
                        temp->doctorHead = temp->doctorHead->next;
                        delete docTemp;
                    }

                    GlobalLocationManager::getInstance()->removeLocation(temp->lat, temp->lon);
                    CityGraph::getInstance()->removeLocation(temp->hospitalID);
                    delete temp;
                    break;
                }
                current = current->next;
            }
        }

        hospitalCount--;
        cout << "Hospital deleted successfully!" << endl;
    }

    void updateHospital() {
        cout << "\n=== Update Hospital ===" << endl;

        string name = getStringInput("Enter Hospital Name to update: ");
        HospitalNode* hospital = findHospitalByName(name);

        if (hospital == nullptr) {
            cout << "Error: Hospital not found!" << endl;
            return;
        }

        cout << "Current Details:" << endl;
        cout << "  Name: " << hospital->name << endl;
        cout << "  Sector: " << hospital->sector << endl;
        cout << "  Specialization: " << hospital->specialization << endl;
        cout << "  Emergency Beds: " << hospital->emergencyBeds << endl;

        cout << "\nWhat would you like to update?" << endl;
        cout << "1.  Specialization" << endl;
        cout << "2. Emergency Beds" << endl;
        cout << "3. Both" << endl;
        cout << "0. Cancel" << endl;

        int choice = getIntInput("Enter choice: ");

        if (choice == 1 || choice == 3) {
            string newSpec = getStringInput("Enter new specialization: ");
            if (!newSpec.empty()) {
                hospital->specialization = newSpec;
            }
        }

        if (choice == 2 || choice == 3) {
            int newBeds = getIntInput("Enter new bed count: ");
            if (newBeds >= 0) {
                hospital->emergencyBeds = newBeds;
                bedHeap.rebuildHeap();
            }
        }

        if (choice != 0) {
            cout << "Hospital updated successfully!" << endl;
        }
    }

  // Doctor operations

    void addDoctor() {
        cout << "\n=== Add Doctor to Hospital ===" << endl;

        string hospitalName = getStringInput("Enter Hospital Name: ");
        HospitalNode* hospital = findHospitalByName(hospitalName);
        if (hospital == nullptr) {
            cout << "Error: Hospital not found!" << endl;
            return;
        }

        string docName = getStringInput("Enter Doctor Name: ");
        if (docName.empty()) {
            cout << "Error: Doctor name cannot be empty." << endl;
            return;
        }

        int empID = getIntInput("Enter Employee ID: ");
        if (isDoctorIDTaken(hospital, empID)) {
            cout << "Error: Employee ID " << empID << " already exists in this hospital!" << endl;
            return;
        }

        string specialization = getStringInput("Enter Specialization: ");

        DoctorNode* newDoctor = new DoctorNode(docName, specialization, empID);
        newDoctor->next = hospital->doctorHead;
        hospital->doctorHead = newDoctor;

        cout << "Doctor '" << docName << "' added to '" << hospital->name << "'!" << endl;
    }

    void removeDoctor() {
        cout << "\n=== Remove Doctor from Hospital ===" << endl;

        string hospitalName = getStringInput("Enter Hospital Name: ");
        HospitalNode* hospital = findHospitalByName(hospitalName);
        if (hospital == nullptr) {
            cout << "Error: Hospital not found!" << endl;
            return;
        }

        if (hospital->doctorHead == nullptr) {
            cout << "Error: No doctors in this hospital!" << endl;
            return;
        }

        cout << "Doctors in " << hospital->name << ":" << endl;
        DoctorNode* doc = hospital->doctorHead;
        while (doc != nullptr) {
            cout << "  - " << doc->name << " (ID: " << doc->empID << ") - " << doc->specialization << endl;
            doc = doc->next;
        }

        int empID = getIntInput("Enter Employee ID of doctor to remove: ");

        if (hospital->doctorHead->empID == empID) {
            DoctorNode* temp = hospital->doctorHead;
            hospital->doctorHead = hospital->doctorHead->next;
            cout << "Doctor '" << temp->name << "' removed successfully!" << endl;
            delete temp;
            return;
        }

        DoctorNode* current = hospital->doctorHead;
        while (current->next != nullptr) {
            if (current->next->empID == empID) {
                DoctorNode* temp = current->next;
                current->next = current->next->next;
                cout << "Doctor '" << temp->name << "' removed successfully!" << endl;
                delete temp;
                return;
            }
            current = current->next;
        }

        cout << "Error: Doctor with ID " << empID << " not found!" << endl;
    }

    void updateDoctor() {
        cout << "\n=== Update Doctor ===" << endl;

        string hospitalName = getStringInput("Enter Hospital Name: ");
        HospitalNode* hospital = findHospitalByName(hospitalName);
        if (hospital == nullptr) {
            cout << "Error: Hospital not found!" << endl;
            return;
        }

        int empID = getIntInput("Enter Doctor Employee ID: ");
        DoctorNode* doctor = findDoctor(hospital, empID);

        if (doctor == nullptr) {
            cout << "Error: Doctor not found!" << endl;
            return;
        }

        cout << "Current Details:" << endl;
        cout << "  Name: " << doctor->name << endl;
        cout << "  Specialization: " << doctor->specialization << endl;

        string newSpec = getStringInput("Enter new specialization (or press Enter to keep current): ");
        if (!newSpec.empty()) {
            doctor->specialization = newSpec;
            cout << "Doctor updated successfully!" << endl;
        }
        else {
            cout << "No changes made." << endl;
        }
    }

    void updateBedCount() {
        cout << "\n=== Update Emergency Bed Count ===" << endl;

        string hospitalName = getStringInput("Enter Hospital Name: ");
        HospitalNode* hospital = findHospitalByName(hospitalName);
        if (hospital == nullptr) {
            cout << "Error: Hospital not found!" << endl;
            return;
        }

        cout << "Current beds: " << hospital->emergencyBeds << endl;
        int newBeds = getIntInput("Enter new bed count: ");
        if (newBeds < 0) newBeds = 0;

        hospital->emergencyBeds = newBeds;
        bedHeap.rebuildHeap();

        cout << "Bed count updated to " << newBeds << "!" << endl;
    }

    void viewBedAvailability() {
        bedHeap.displayAvailability();
    }

    void findHospitalWithBeds() {
        cout << "\n=== Find Hospital with Required Beds ===" << endl;

        int requiredBeds = getIntInput("Enter number of beds required: ");
        HospitalNode* hospital = bedHeap.findWithMinBeds(requiredBeds);

        if (hospital != nullptr) {
            cout << "\nRecommended Hospital:" << endl;
            cout << "  Name: " << hospital->name << endl;
            cout << "  Sector: " << hospital->sector << endl;
            cout << "  Available Beds: " << hospital->emergencyBeds << endl;
            cout << "  Specialization: " << hospital->specialization << endl;
        }
        else {
            cout << "No hospital found with " << requiredBeds << " or more beds available." << endl;
        }
    }

    void findNearestHospital() {
        cout << "\n=== Find Nearest Hospital ===" << endl;

        if (hospitalsHead == nullptr) {
            cout << "No hospitals registered." << endl;
            return;
        }

        double userLat = getDoubleInput("Enter your Latitude: ");
        double userLon = getDoubleInput("Enter your Longitude: ");

        HospitalNode* nearest = nullptr;
        double minDist = DBL_MAX;

        HospitalNode* current = hospitalsHead;
        while (current != nullptr) {
            double dist = calculateDistance(userLat, userLon, current->lat, current->lon);
            if (dist < minDist) {
                minDist = dist;
                nearest = current;
            }
            current = current->next;
        }

        if (nearest != nullptr) {
            cout << "\n=== Nearest Hospital ===" << endl;
            cout << "Name: " << nearest->name << endl;
            cout << "Sector: " << nearest->sector << endl;
            cout << "Specialization: " << nearest->specialization << endl;
            cout << "Emergency Beds: " << nearest->emergencyBeds << endl;
            cout << "Distance: " << minDist << " units" << endl;
        }
    }

// Pharmacy Operations 

    void registerPharmacy() {
        cout << "\n=== Register Pharmacy ===" << endl;

        string name = getStringInput("Enter Pharmacy Name: ");
        if (name.empty()) {
            cout << "Error: Pharmacy name cannot be empty." << endl;
            return;
        }

        if (findPharmacyByName(name) != nullptr) {
            cout << "Error: Pharmacy '" << name << "' already exists!" << endl;
            return;
        }

        string sector = getStringInput("Enter Sector: ");
        double lat = getDoubleInput("Enter Latitude: ");
        double lon = getDoubleInput("Enter Longitude: ");

        GlobalLocationManager* locMgr = GlobalLocationManager::getInstance();
        if (!locMgr->registerLocation(lat, lon, "Pharmacy", name)) {
            return;
        }

        string pharmacyID = "P" + intToString(pharmacyIDCounter++);

        PharmacyNode* newPharmacy = new PharmacyNode(pharmacyID, name, sector, lat, lon);
        newPharmacy->next = pharmaciesHead;
        pharmaciesHead = newPharmacy;
        pharmacyCount++;

        CityGraph::getInstance()->addLocation(pharmacyID, name, LOC_PHARMACY, lat, lon, newPharmacy);

        cout << "Pharmacy '" << name << "' registered with ID: " << pharmacyID << endl;
    }

    void deletePharmacy() {
        cout << "\n=== Delete Pharmacy ===" << endl;

        string name = getStringInput("Enter Pharmacy Name to delete: ");
        PharmacyNode* pharmacy = findPharmacyByName(name);

        if (pharmacy == nullptr) {
            cout << "Error: Pharmacy not found!" << endl;
            return;
        }

        string confirm = getStringInput("Type 'YES' to confirm deletion: ");
        if (confirm != "YES") {
            cout << "Deletion cancelled." << endl;
            return;
        }

        if (stringsEqualIgnoreCase(pharmaciesHead->name, name)) {
            PharmacyNode* temp = pharmaciesHead;
            pharmaciesHead = pharmaciesHead->next;

            GlobalLocationManager::getInstance()->removeLocation(temp->lat, temp->lon);
            CityGraph::getInstance()->removeLocation(temp->pharmacyID);
            delete temp;  // Destructor will clean up hash table
        }
        else {
            PharmacyNode* current = pharmaciesHead;
            while (current->next != nullptr) {
                if (stringsEqualIgnoreCase(current->next->name, name)) {
                    PharmacyNode* temp = current->next;
                    current->next = current->next->next;

                    GlobalLocationManager::getInstance()->removeLocation(temp->lat, temp->lon);
                    CityGraph::getInstance()->removeLocation(temp->pharmacyID);
                    delete temp;  // Destructor will clean up hash table
                    break;
                }
                current = current->next;
            }
        }

        pharmacyCount--;
        cout << "Pharmacy deleted successfully!" << endl;
    }

    void updatePharmacy() {
        cout << "\n=== Update Pharmacy ===" << endl;

        string name = getStringInput("Enter Pharmacy Name to update: ");
        PharmacyNode* pharmacy = findPharmacyByName(name);

        if (pharmacy == nullptr) {
            cout << "Error: Pharmacy not found!" << endl;
            return;
        }

        cout << "Current Sector: " << pharmacy->sector << endl;

        string newSector = getStringInput("Enter new sector (or press Enter to keep current): ");
        if (!newSector.empty()) {
            pharmacy->sector = newSector;
            cout << "Pharmacy updated successfully!" << endl;
        }
        else {
            cout << "No changes made." << endl;
        }
    }

// Medicine Operations 

    void addMedicine() {
        cout << "\n=== Add Medicine to Pharmacy ===" << endl;

        if (pharmaciesHead == nullptr) {
            cout << "No pharmacies registered.  Please register a pharmacy first." << endl;
            return;
        }

        string pharmacyName = getStringInput("Enter Pharmacy Name: ");
        PharmacyNode* pharmacy = findPharmacyByName(pharmacyName);
        if (pharmacy == nullptr) {
            cout << "Error: Pharmacy not found!" << endl;
            return;
        }

        string medName = getStringInput("Enter Medicine Name: ");
        if (medName.empty()) {
            cout << "Error: Medicine name cannot be empty." << endl;
            return;
        }

        string formula = getStringInput("Enter Formula: ");
        int price = getIntInput("Enter Price: ");
        if (price < 0) price = 0;

        // Check if resize is needed BEFORE insertion
        if (pharmacy->needsResize()) {
            pharmacy->resizeHashTable();
        }

        int idx = medHash(medName, pharmacy->medTableSize);
        MedicineNode* newMed = new MedicineNode(medName, formula, price);
        newMed->next = pharmacy->medTable[idx];
        pharmacy->medTable[idx] = newMed;
        pharmacy->medicineCount++;

        cout << "Medicine '" << medName << "' added to '" << pharmacy->name << "'!" << endl;
    }

    void removeMedicine() {
        cout << "\n=== Remove Medicine from Pharmacy ===" << endl;

        string pharmacyName = getStringInput("Enter Pharmacy Name: ");
        PharmacyNode* pharmacy = findPharmacyByName(pharmacyName);
        if (pharmacy == nullptr) {
            cout << "Error: Pharmacy not found!" << endl;
            return;
        }

        string medName = getStringInput("Enter Medicine Name to remove: ");
        int idx = medHash(medName, pharmacy->medTableSize);

        if (pharmacy->medTable[idx] == nullptr) {
            cout << "Medicine '" << medName << "' not found." << endl;
            return;
        }

        if (stringsEqualIgnoreCase(pharmacy->medTable[idx]->name, medName)) {
            MedicineNode* temp = pharmacy->medTable[idx];
            pharmacy->medTable[idx] = pharmacy->medTable[idx]->next;
            cout << "Medicine '" << temp->name << "' removed successfully!" << endl;
            delete temp;
            pharmacy->medicineCount--;
            return;
        }

        MedicineNode* current = pharmacy->medTable[idx];
        while (current->next != nullptr) {
            if (stringsEqualIgnoreCase(current->next->name, medName)) {
                MedicineNode* temp = current->next;
                current->next = current->next->next;
                cout << "Medicine '" << temp->name << "' removed successfully!" << endl;
                delete temp;
                pharmacy->medicineCount--;
                return;
            }
            current = current->next;
        }

        cout << "Medicine '" << medName << "' not found." << endl;
    }

    void updateMedicine() {
        cout << "\n=== Update Medicine ===" << endl;

        string pharmacyName = getStringInput("Enter Pharmacy Name: ");
        PharmacyNode* pharmacy = findPharmacyByName(pharmacyName);
        if (pharmacy == nullptr) {
            cout << "Error: Pharmacy not found!" << endl;
            return;
        }

        string medName = getStringInput("Enter Medicine Name: ");
        int idx = medHash(medName, pharmacy->medTableSize);

        MedicineNode* current = pharmacy->medTable[idx];
        while (current != nullptr) {
            if (stringsEqualIgnoreCase(current->name, medName)) {
                cout << "Current Details:" << endl;
                cout << "  Name: " << current->name << endl;
                cout << "  Formula: " << current->formula << endl;
                cout << "  Price: Rs.  " << current->price << endl;

                cout << "\nWhat would you like to update?" << endl;
                cout << "1.  Formula" << endl;
                cout << "2. Price" << endl;
                cout << "3. Both" << endl;
                cout << "0. Cancel" << endl;

                int choice = getIntInput("Enter choice: ");

                if (choice == 1 || choice == 3) {
                    string newFormula = getStringInput("Enter new formula: ");
                    if (!newFormula.empty()) {
                        current->formula = newFormula;
                    }
                }

                if (choice == 2 || choice == 3) {
                    int newPrice = getIntInput("Enter new price: ");
                    if (newPrice >= 0) {
                        current->price = newPrice;
                    }
                }

                if (choice != 0) {
                    cout << "Medicine updated successfully!" << endl;
                }
                return;
            }
            current = current->next;
        }

        cout << "Medicine not found." << endl;
    }

    void searchMedicine() {
        cout << "\n=== Search Medicine ===" << endl;

        string query = getStringInput("Enter Medicine Name or Formula: ");
        string cleanQuery = toLowerStr(trimString(query));

        bool found = false;
        cout << "\n--- Search Results ---" << endl;

        PharmacyNode* pharmacy = pharmaciesHead;
        while (pharmacy != nullptr) {
            for (int i = 0; i < pharmacy->medTableSize; i++) {
                MedicineNode* med = pharmacy->medTable[i];
                while (med != nullptr) {
                    string cleanName = toLowerStr(med->name);
                    string cleanFormula = toLowerStr(med->formula);

                    if (cleanName.find(cleanQuery) != string::npos ||
                        cleanFormula.find(cleanQuery) != string::npos) {
                        cout << "Found at " << pharmacy->name << " (" << pharmacy->sector << "):" << endl;
                        cout << "  Medicine: " << med->name << endl;
                        cout << "  Formula: " << med->formula << endl;
                        cout << "  Price: Rs. " << med->price << endl;
                        cout << endl;
                        found = true;
                    }
                    med = med->next;
                }
            }
            pharmacy = pharmacy->next;
        }

        if (!found) {
            cout << "No medicine found matching '" << query << "'." << endl;
        }
    }

    // Display Function

    void displayAllHospitals() {
        cout << "\n=== All Hospitals ===" << endl;
        HospitalNode* current = hospitalsHead;
        int count = 0;
        while (current != nullptr) {
            cout << ++count << ". [" << current->hospitalID << "] " << current->name
                << " (" << current->sector << ") - Beds: " << current->emergencyBeds
                << " - " << current->specialization << endl;

            DoctorNode* doc = current->doctorHead;
            if (doc != nullptr) {
                cout << "   Doctors: ";
                while (doc != nullptr) {
                    cout << doc->name << " (" << doc->specialization << ")";
                    if (doc->next != nullptr) cout << ", ";
                    doc = doc->next;
                }
                cout << endl;
            }
            current = current->next;
        }
        if (count == 0) cout << "No hospitals registered." << endl;
    }

    void displayAllPharmacies() {
        cout << "\n=== All Pharmacies ===" << endl;
        PharmacyNode* current = pharmaciesHead;
        int count = 0;
        while (current != nullptr) {
            cout << ++count << ". [" << current->pharmacyID << "] " << current->name << " (" << current->sector << ")" << endl;
            cout << "   Medicines in stock: " << current->medicineCount << " | Hash table size: " << current->medTableSize << endl;

            current = current->next;
        }
        if (count == 0) cout << "No pharmacies registered." << endl;
    }

    void displayPharmacyMedicines() {
        cout << "\n=== Display Pharmacy Medicines ===" << endl;

        string pharmacyName = getStringInput("Enter Pharmacy Name: ");
        PharmacyNode* pharmacy = findPharmacyByName(pharmacyName);
        if (pharmacy == nullptr) {
            cout << "Error: Pharmacy not found!" << endl;
            return;
        }

        cout << "\n Medicines at " << pharmacy->name << " ---" << endl;
        cout << "(Hash table size: " << pharmacy->medTableSize << ", Load factor: " << (double)pharmacy->medicineCount / pharmacy->medTableSize << ")" << endl;

        bool hasMedicines = false;

        for (int i = 0; i < pharmacy->medTableSize; i++) {
            MedicineNode* med = pharmacy->medTable[i];
            while (med != nullptr) {
                cout << "  - " << med->name << " (" << med->formula << ") - Rs. " << med->price << endl;
                hasMedicines = true;
                med = med->next;
            }
        }

        if (!hasMedicines) {
            cout << "(No medicines in stock)" << endl;
        }
    }

    void displayStatistics() {
        cout << "\n=== Medical System Statistics ===" << endl;
        cout << "Total Hospitals: " << hospitalCount << endl;
        cout << "Total Pharmacies: " << pharmacyCount << endl;
        cout << "Heap Capacity: " << bedHeap.getCapacity() << " (Used: " << bedHeap.getSize() << ")" << endl;

        int totalDoctors = 0;
        int totalMedicines = 0;
        int totalBeds = 0;

        HospitalNode* hosp = hospitalsHead;
        while (hosp != nullptr) {
            totalBeds += hosp->emergencyBeds;
            DoctorNode* doc = hosp->doctorHead;
            while (doc != nullptr) {
                totalDoctors++;
                doc = doc->next;
            }
            hosp = hosp->next;
        }

        PharmacyNode* pharm = pharmaciesHead;
        while (pharm != nullptr) {
            totalMedicines += pharm->medicineCount;
            pharm = pharm->next;
        }

        cout << "Total Doctors: " << totalDoctors << endl;
        cout << "Total Emergency Beds: " << totalBeds << endl;
        cout << "Total Medicine Types: " << totalMedicines << endl;
    }

    // File Loading 

    void loadFromFiles() {
        GlobalLocationManager* locMgr = GlobalLocationManager::getInstance();

        auto normalizeSeparators = [](string& s) {
            for (size_t i = 0; i < s.length(); i++) {
                if (s[i] == '\t') s[i] = ',';
            }
            };

        auto stripQuotes = [](string& s) {
            s = trimString(s);
            if (!s.empty() && s[0] == '"') s = s.substr(1);
            if (!s.empty() && s[s.length() - 1] == '"') s = s.substr(0, s.length() - 1);
            };

        // Load hospitals
        ifstream hospFile("hospitals.csv");
        if (hospFile.is_open()) {
            string line;
            getline(hospFile, line);
            int loadedCount = 0;

            while (getline(hospFile, line)) {
                if (line.empty()) continue;
                normalizeSeparators(line);

                string parts[6];
                int count = splitString(line, ',', parts, 6);

                if (count >= 5) {
                    string id = trimString(parts[0]);
                    string name = trimString(parts[1]);
                    string sector = trimString(parts[2]);
                    int beds = stringToInt(trimString(parts[3]));

                    string spec = trimString(parts[4]);
                    if (count == 6 && !parts[5].empty()) {
                        string part4 = trimString(parts[4]);
                        string part5 = trimString(parts[5]);

                        if (!part4.empty() && part4[0] == '"') {
                            part4 = part4.substr(1);
                        }
                        if (!part5.empty() && part5[part5.length() - 1] == '"') {
                            part5 = part5.substr(0, part5.length() - 1);
                        }

                        spec = part4 + ", " + part5;
                    }
                    else {
                        stripQuotes(spec);
                    }

                    if (findHospitalByID(id) != nullptr || findHospitalByName(name) != nullptr) {
                        continue;
                    }

                    double lat, lon;
                    double hospitalOffset = 0.002;

                    if (sector == "G-8") { 
                        lat = 33.695 + hospitalOffset; lon = 73.045 + hospitalOffset;
                    }
                    else if (sector == "H-8") {
                        lat = 33.690 + hospitalOffset; lon = 73.040 + hospitalOffset;
                    }
                    else if (sector == "F-10") {
                        lat = 33.691 + hospitalOffset; lon = 73.019 + hospitalOffset;
                    }
                    else if (sector == "G-6") { 
                        lat = 33.710 + hospitalOffset; lon = 73.060 + hospitalOffset; 
                    }
                    else if (sector == "I-8") { 
                        lat = 33.680 + hospitalOffset; lon = 73.050 + hospitalOffset; 
                    }
                    else if (sector == "G-10") { 
                        lat = 33.684 + hospitalOffset; lon = 73.025 + hospitalOffset;
                    }
                    else if (sector == "G-9") {
                        lat = 33.693 + hospitalOffset; lon = 73.028 + hospitalOffset; 
                    }
                    else if (sector == "F-8") { 
                        lat = 33.709 + hospitalOffset; lon = 73.037 + hospitalOffset;
                    }
                    else if (sector == "G-11") { 
                        lat = 33.675 + hospitalOffset; lon = 73.032 + hospitalOffset;
                    }
                    else {
                        lat = 33.700 + (loadedCount * 0.005) + hospitalOffset;
                        lon = 73.050 + (loadedCount * 0.005) + hospitalOffset;
                    }

                    if (!locMgr->registerLocation(lat, lon, "Hospital", name)) {
                        lat += 0.001;
                        lon += 0.001;
                        if (!locMgr->registerLocation(lat, lon, "Hospital", name)) {
                            continue;
                        }
                    }

                    HospitalNode* newHospital = new HospitalNode(id, name, sector, lat, lon, beds, spec);
                    newHospital->next = hospitalsHead;
                    hospitalsHead = newHospital;
                    hospitalCount++;
                    bedHeap.insert(newHospital);

                    CityGraph::getInstance()->addLocation(id, name, LOC_HOSPITAL, lat, lon, newHospital);

                    loadedCount++;
                }
            }
            hospFile.close();
            cout << "Loaded " << loadedCount << " hospitals from file." << endl;
        }
        else {
            cout << "Warning: hospitals.csv not found." << endl;
        }

        // Load pharmacies
        ifstream pharmFile("pharmacies.csv");
        if (pharmFile.is_open()) {
            string line;
            getline(pharmFile, line);
            int loadedCount = 0;

            while (getline(pharmFile, line)) {
                if (line.empty()) continue;
                normalizeSeparators(line);

                string parts[6];
                int count = splitString(line, ',', parts, 6);

                if (count >= 6) {
                    string id = trimString(parts[0]);
                    string name = trimString(parts[1]);
                    string sector = trimString(parts[2]);
                    string medName = trimString(parts[3]);
                    string formula = trimString(parts[4]);
                    int price = stringToInt(trimString(parts[5]));

                    PharmacyNode* pharmacy = findPharmacyByID(id);
                    if (pharmacy == nullptr) {
                        double lat, lon;
                        double pharmacyOffset = 0.003;

                        if (sector == "F-8") { 
                            lat = 33.709 + pharmacyOffset; lon = 73.037 + pharmacyOffset;
                        }
                        else if (sector == "G-10") {
                            lat = 33.684 + pharmacyOffset; lon = 73.025 + pharmacyOffset;
                        }
                        else if (sector == "F-6") {
                            lat = 33.727 + pharmacyOffset; lon = 73.075 + pharmacyOffset;
                        }
                        else if (sector == "I-8") {
                            lat = 33.680 + pharmacyOffset; lon = 73.050 + pharmacyOffset; 
                        }
                        else if (sector == "Blue Area") { 
                            lat = 33.720 + pharmacyOffset; lon = 73.065 + pharmacyOffset;
                        }
                        else {
                            lat = 33.700 + (pharmacyCount * 0.005) + pharmacyOffset;
                            lon = 73.030 + (pharmacyCount * 0.005) + pharmacyOffset;
                        }

                        if (!locMgr->registerLocation(lat, lon, "Pharmacy", name)) {
                            lat += 0.001;
                            lon += 0.001;
                            if (!locMgr->registerLocation(lat, lon, "Pharmacy", name)) {
                                continue;
                            }
                        }

                        pharmacy = new PharmacyNode(id, name, sector, lat, lon);
                        pharmacy->next = pharmaciesHead;
                        pharmaciesHead = pharmacy;
                        pharmacyCount++;

                        CityGraph::getInstance()->addLocation(id, name, LOC_PHARMACY, lat, lon, pharmacy);

                        loadedCount++;
                    }

                    // Add medicine with auto-resize
                    if (pharmacy->needsResize()) {
                        pharmacy->resizeHashTable();
                    }

                    int idx = medHash(medName, pharmacy->medTableSize);
                    MedicineNode* newMed = new MedicineNode(medName, formula, price);
                    newMed->next = pharmacy->medTable[idx];
                    pharmacy->medTable[idx] = newMed;
                    pharmacy->medicineCount++;
                }
            }
            pharmFile.close();
            cout << "Loaded " << loadedCount << " pharmacies from file." << endl;
        }
        else {
            cout << "Warning: pharmacies.csv not found." << endl;
        }
    }

    // Getters

    int getHospitalCount() {
        return hospitalCount;
    }
    int getPharmacyCount() { 
        return pharmacyCount;
    }

    // Destructor 

    ~MedicalSystem() {
        while (hospitalsHead != nullptr) {
            HospitalNode* hospTemp = hospitalsHead;
            hospitalsHead = hospitalsHead->next;

            while (hospTemp->doctorHead != nullptr) {
                DoctorNode* docTemp = hospTemp->doctorHead;
                hospTemp->doctorHead = hospTemp->doctorHead->next;
                delete docTemp;
            }
            delete hospTemp;
        }

        while (pharmaciesHead != nullptr) {
            PharmacyNode* pharmTemp = pharmaciesHead;
            pharmaciesHead = pharmaciesHead->next;
            delete pharmTemp;  
        }
    }
};
