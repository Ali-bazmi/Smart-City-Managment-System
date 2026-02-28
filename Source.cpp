#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <cfloat>

using namespace std;


const string RESET = "\033[0m";
const string BLACK = "\033[30m";
const string RED = "\033[31m";
const string GREEN = "\033[32m";
const string YELLOW = "\033[33m";
const string BLUE = "\033[34m";
const string MAGENTA = "\033[35m";
const string CYAN = "\033[36m";
const string WHITE = "\033[37m";
const string BOLD = "\033[1m";

#include "Utils.h"
#include "GlobalLocationManager.h"
#include "Transport.h"
#include "Medical.h"
#include "Education.h"
#include "Commercial.h"
#include "Facilities.h"
#include "Population.h"
#include "BonusModules.h" 
#include "../Project1/Sfmlvisualizer.h"


TransportSystem transportSystem;
MedicalSystem medicalSystem;
EducationSystem educationSystem;
CommercialSystem commercialSystem;
FacilitiesSystem facilitiesSystem;
PopulationSystem populationSystem;
BonusModulesMenu bonusModulesMenu;



void displayMainHeader() {
    cout << "\n";
    cout << BOLD << CYAN << "================================================================" << endl;
    cout << "=         " << YELLOW << "ISLAMABAD CITY MANAGEMENT SYSTEM" << CYAN << "                     =" << endl;
    cout << "=         " << WHITE << "Smart City Infrastructure Manager" << CYAN << "                    =" << endl;
    cout << "================================================================" << RESET << endl;
}

void displayMainMenu() {
    cout << "\n" << BOLD << BLUE << "==================== MAIN MENU ====================" << RESET << endl;
    cout << GREEN << "1.  " << RESET << "Transport Management" << endl;
    cout << GREEN << "2.  " << RESET << "Medical Services" << endl;
    cout << GREEN << "3.  " << RESET << "Education System" << endl;
    cout << GREEN << "4.  " << RESET << "Commercial (Malls & Shopping)" << endl;
    cout << GREEN << "5.  " << RESET << "Public Facilities" << endl;
    cout << GREEN << "6.  " << RESET << "Population & Housing" << endl;
    cout << GREEN << "7.  " << RESET << "System Statistics" << endl;
    cout << GREEN << "8.  " << RESET << "Load All Data from Files" << endl;
    cout << MAGENTA << "9.  BONUS MODULES (Extended Features)" << RESET << endl;
    cout << MAGENTA << "10. SFML GRAPHICAL VISUALIZATIONS" << RESET << endl;  // NEW!
    cout << RED << "0.  Exit" << RESET << endl;
    cout << BLUE << "===================================================" << RESET << endl;
}



void displayTransportMenu() {
    cout << "\n" << BOLD << CYAN << "=============== TRANSPORT MANAGEMENT ===============" << RESET << endl;
    cout << YELLOW << "--- Bus Stop Operations ---" << RESET << endl;
    cout << "1.  Add Bus Stop" << endl;
    cout << "2.  Delete Bus Stop" << endl;
    cout << "3.  Update Bus Stop" << endl;
    cout << "4.  Connect Stops (Add Road)" << endl;
    cout << "5.   Disconnect Stops (Remove Road)" << endl;
    cout << "6.  Display All Stops" << endl;
    cout << "7.  Display Road Network" << endl;
    cout << endl;
    cout << YELLOW << "--- Bus Operations ---" << RESET << endl;
    cout << "8.  Register Bus" << endl;
    cout << "9.  Delete Bus" << endl;
    cout << "10. Update Bus" << endl;
    cout << "11.  Search Bus" << endl;
    cout << "12. Add Stop to Bus Route" << endl;
    cout << "13. Remove Stop from Bus Route" << endl;
    cout << "14. Update Bus Location" << endl;
    cout << "15. Find Nearest Bus" << endl;
    cout << "16.  Display All Buses" << endl;
    cout << endl;
    cout << YELLOW << "--- Passenger Operations ---" << RESET << endl;
    cout << "17. Add Passenger to Queue" << endl;
    cout << "18. Remove Passenger from Queue" << endl;
    cout << "19.  Process Next Passenger" << endl;
    cout << "20. View Passenger Queue" << endl;
    cout << "21. Clear Passenger Queue" << endl;
    cout << endl;
    cout << YELLOW << "--- Navigation ---" << RESET << endl;
    cout << "22. Find Shortest Path (Dijkstra)" << endl;
    cout << "23. View Route History" << endl;
    cout << "24. Clear Route History" << endl;
    cout << endl;
    cout << YELLOW << "--- School Bus Operations ---" << RESET << endl;
    cout << "25. Register School Bus" << endl;
    cout << "26. Simulate School Bus Movement" << endl;
    cout << "27. Display All School Buses" << endl;
    cout << endl;
    cout << YELLOW << "--- Statistics ---" << RESET << endl;
    cout << "28. Display Statistics" << endl;
    cout << endl;
    cout << RED << "0.  Back to Main Menu" << RESET << endl;
    cout << CYAN << "=====================================================" << RESET << endl;
}

void handleTransportMenu() {
    int choice;
    do {
        displayTransportMenu();
        choice = getIntInput(YELLOW + "Enter choice: " + RESET);

        switch (choice) {
        case 0:
            cout << BLUE << "Returning to Main Menu..." << RESET << endl;
            break;
            // Bus Stop Operations
        case 1:
            transportSystem.addBusStop();
            break;
        case 2:
            transportSystem.deleteBusStop();
            break;
        case 3:
            transportSystem.updateBusStop();
            break;
        case 4:
            transportSystem.connectStops();
            break;
        case 5:
            transportSystem.disconnectStops();
            break;
        case 6:
            transportSystem.displayAllStops();
            break;
        case 7:
            transportSystem.displayGraph();
            break;

            // Bus Operations
        case 8:
            transportSystem.registerBus();
            break;
        case 9:
            transportSystem.deleteBus();
            break;
        case 10:
            transportSystem.updateBus();
            break;
        case 11:
            transportSystem.searchBus();
            break;
        case 12:
            transportSystem.addStopToBusRoute();
            break;
        case 13:
            transportSystem.removeStopFromRoute();
            break;
        case 14:
            transportSystem.updateBusLocation();
            break;
        case 15:
            transportSystem.findNearestBus();
            break;
        case 16:
            transportSystem.displayAllBuses();
            break;

            // Passenger Operations
        case 17:
            transportSystem.addPassenger();
            break;
        case 18:
            transportSystem.removePassengerFromQueue();
            break;
        case 19:
            transportSystem.processNextPassenger();
            break;
        case 20:
            transportSystem.viewPassengerQueue();
            break;
        case 21:
            transportSystem.clearPassengerQueue();
            break;

            // Navigation
        case 22:
            transportSystem.findShortestPath();
            break;
        case 23:
            transportSystem.viewRouteHistory();
            break;
        case 24:
            transportSystem.clearRouteHistory();
            break;

            // School Bus Operations
        case 25:
            transportSystem.registerSchoolBus();
            break;
        case 26:
            transportSystem.simulateSchoolBus();
            break;
        case 27:
            transportSystem.displaySchoolBuses();
            break;

            // Statistics
        case 28:
            transportSystem.displayStatistics();
            break;


        default: cout << RED << "Invalid choice!  Please try again." << RESET << endl;
        }

        if (choice != 0) {
            cout << "\nPress Enter to continue... ";
            cin.get();
        }
    } while (choice != 0);
}

// MEDICAL MENU

void displayMedicalMenu() {
    cout << "\n" << BOLD << CYAN << "================ MEDICAL SERVICES =================" << RESET << endl;
    cout << YELLOW << "--- Hospital Operations ---" << RESET << endl;
    cout << "1.  Register Hospital" << endl;
    cout << "2.  Delete Hospital" << endl;
    cout << "3.  Update Hospital" << endl;
    cout << "4.   Add Doctor to Hospital" << endl;
    cout << "5.  Remove Doctor from Hospital" << endl;
    cout << "6.  Update Doctor" << endl;
    cout << "7.   Update Bed Count" << endl;
    cout << "8.  View Bed Availability" << endl;
    cout << "9.  Find Hospital with Required Beds" << endl;
    cout << "10. Find Nearest Hospital" << endl;
    cout << "11. Display All Hospitals" << endl;
    cout << endl;
    cout << YELLOW << "--- Pharmacy Operations ---" << RESET << endl;
    cout << "12. Register Pharmacy" << endl;
    cout << "13. Delete Pharmacy" << endl;
    cout << "14. Update Pharmacy" << endl;
    cout << "15. Add Medicine to Pharmacy" << endl;
    cout << "16.  Remove Medicine from Pharmacy" << endl;
    cout << "17. Update Medicine" << endl;
    cout << "18.  Search Medicine" << endl;
    cout << "19. Display All Pharmacies" << endl;
    cout << "20. Display Pharmacy Medicines" << endl;
    cout << endl;
    cout << YELLOW << "--- Statistics ---" << RESET << endl;
    cout << "21. Display Statistics" << endl;
    cout << endl;
    cout << RED << "0.   Back to Main Menu" << RESET << endl;
    cout << CYAN << "=====================================================" << RESET << endl;
}

void handleMedicalMenu() {
    int choice;
    do {
        displayMedicalMenu();
        choice = getIntInput(YELLOW + "Enter choice: " + RESET);

        switch (choice) {
        case 0:
            cout << BLUE << "Returning to Main Menu..." << RESET << endl;
            break;
            // Hospital Operations
        case 1:
            medicalSystem.registerHospital();
            break;
        case 2:
            medicalSystem.deleteHospital();
            break;
        case 3:
            medicalSystem.updateHospital();
            break;
        case 4:
            medicalSystem.addDoctor();
            break;
        case 5:
            medicalSystem.removeDoctor();
            break;
        case 6:
            medicalSystem.updateDoctor();
            break;
        case 7:
            medicalSystem.updateBedCount();
            break;
        case 8:
            medicalSystem.viewBedAvailability();
            break;
        case 9:
            medicalSystem.findHospitalWithBeds();
            break;
        case 10:
            medicalSystem.findNearestHospital();
            break;
        case 11:
            medicalSystem.displayAllHospitals();
            break;

            // Pharmacy Operations
        case 12:
            medicalSystem.registerPharmacy();
            break;
        case 13:
            medicalSystem.deletePharmacy();
            break;
        case 14:
            medicalSystem.updatePharmacy();
            break;
        case 15:
            medicalSystem.addMedicine();
            break;
        case 16:
            medicalSystem.removeMedicine();
            break;
        case 17:
            medicalSystem.updateMedicine();
            break;
        case 18:
            medicalSystem.searchMedicine();
            break;
        case 19:
            medicalSystem.displayAllPharmacies();
            break;
        case 20:
            medicalSystem.displayPharmacyMedicines();
            break;

            // Statistics
        case 21:
            medicalSystem.displayStatistics();
            break;


        default: cout << RED << "Invalid choice!  Please try again." << RESET << endl;
        }

        if (choice != 0) {
            cout << "\nPress Enter to continue...";
            cin.get();
        }
    } while (choice != 0);
}

// EDUCATION MENU

void displayEducationMenu() {
    cout << "\n" << BOLD << CYAN << "================ EDUCATION SYSTEM =================" << RESET << endl;
    cout << YELLOW << "--- School Operations ---" << RESET << endl;
    cout << "1.  Register School" << endl;
    cout << "2.   Delete School" << endl;
    cout << "3.  Update School" << endl;
    cout << "4.  Display All Schools" << endl;
    cout << "5.  Display School Details" << endl;
    cout << "6.  View School Organogram" << endl;
    cout << "7.  Show School Rankings" << endl;
    cout << "8.  Find Nearest School" << endl;
    cout << "9.  Search by Subject" << endl;
    cout << "10. Display All Subjects" << endl;
    cout << endl;
    cout << YELLOW << "--- Department Operations ---" << RESET << endl;
    cout << "11. Add Department" << endl;
    cout << "12. Delete Department" << endl;
    cout << "13.  Update Department" << endl;
    cout << endl;
    cout << YELLOW << "--- Section Operations ---" << RESET << endl;
    cout << "14. Add Section" << endl;
    cout << "15. Delete Section" << endl;
    cout << "16.  Update Section" << endl;
    cout << endl;
    cout << YELLOW << "--- Teacher Operations ---" << RESET << endl;
    cout << "17. Add Teacher" << endl;
    cout << "18. Remove Teacher" << endl;
    cout << "19. Update Teacher" << endl;
    cout << "20.  Search Teacher" << endl;
    cout << endl;
    cout << YELLOW << "--- Student Operations ---" << RESET << endl;
    cout << "21. Register Student" << endl;
    cout << "22. Remove Student" << endl;
    cout << "23. Update Student" << endl;
    cout << "24.  Transfer Student" << endl;
    cout << "25. Search Student" << endl;
    cout << endl;
    cout << YELLOW << "--- Statistics ---" << RESET << endl;
    cout << "26. Display Statistics" << endl;
    cout << endl;
    cout << RED << "0.  Back to Main Menu" << RESET << endl;
    cout << CYAN << "=====================================================" << RESET << endl;
}

void handleEducationMenu() {
    int choice;
    do {
        displayEducationMenu();
        choice = getIntInput(YELLOW + "Enter choice: " + RESET);

        switch (choice) {
        case 0:
            cout << BLUE << "Returning to Main Menu..." << RESET << endl; break;
            // School Operations
        case 1:
            educationSystem.registerSchool();
            break;
        case 2:
            educationSystem.deleteSchool();
            break;
        case 3:
            educationSystem.updateSchool();
            break;
        case 4:
            educationSystem.displayAllSchools();
            break;
        case 5:
            educationSystem.displaySchoolDetails();
            break;
        case 6:
            educationSystem.viewOrganogram();
            break;
        case 7:
            educationSystem.showRankings();
            break;
        case 8:
            educationSystem.findNearestSchool();
            break;
        case 9:
            educationSystem.searchBySubject();
            break;
        case 10:
            educationSystem.displayAllSubjects();
            break;

            // Department Operations
        case 11:
            educationSystem.addDepartment();
            break;
        case 12:
            educationSystem.deleteDepartment();
            break;
        case 13:
            educationSystem.updateDepartment();
            break;

            // Section Operations
        case 14:
            educationSystem.addSection();
            break;
        case 15:
            educationSystem.deleteSection();
            break;
        case 16:
            educationSystem.updateSection();
            break;

            // Teacher Operations
        case 17:
            educationSystem.addTeacher();
            break;
        case 18:
            educationSystem.removeTeacher();
            break;
        case 19:
            educationSystem.updateTeacher();
            break;
        case 20:
            educationSystem.searchTeacher();
            break;

            // Student Operations
        case 21:
            educationSystem.registerStudent();
            break;
        case 22:
            educationSystem.removeStudent();
            break;
        case 23:
            educationSystem.updateStudent();
            break;
        case 24:
            educationSystem.transferStudent();
            break;
        case 25:
            educationSystem.searchStudent();
            break;

            // Statistics
        case 26:
            educationSystem.displayStatistics();
            break;


        default: cout << RED << "Invalid choice! Please try again." << RESET << endl;
        }

        if (choice != 0) {
            cout << "\nPress Enter to continue... ";
            cin.get();
        }
    } while (choice != 0);
}

// COMMERCIAL MENU


void displayCommercialMenu() {
    cout << "\n" << BOLD << CYAN << "============= COMMERCIAL (MALLS & SHOPPING) =============" << RESET << endl;
    cout << YELLOW << "--- Mall Operations ---" << RESET << endl;
    cout << "1.  Register Mall" << endl;
    cout << "2.  Delete Mall" << endl;
    cout << "3.  Update Mall" << endl;
    cout << "4.   Connect Malls (Delivery Route)" << endl;
    cout << "5.  Disconnect Malls" << endl;
    cout << "6.   Display All Malls" << endl;
    cout << "7.  Display Mall Details" << endl;
    cout << "8.  Display Mall Network" << endl;
    cout << "9.  Find Nearest Mall" << endl;
    cout << endl;
    cout << YELLOW << "--- Product Operations ---" << RESET << endl;
    cout << "10. Add Product" << endl;
    cout << "11. Remove Product" << endl;
    cout << "12. Update Product" << endl;
    cout << "13.  Update Stock" << endl;
    cout << "14. Display Mall Products" << endl;
    cout << "15. Display Low Stock Products" << endl;
    cout << endl;
    cout << YELLOW << "--- Search Operations ---" << RESET << endl;
    cout << "16. Search Product by Name" << endl;
    cout << "17. Search by Category" << endl;
    cout << "18. Find Mall with Product" << endl;
    cout << endl;
    cout << YELLOW << "--- Navigation ---" << RESET << endl;
    cout << "19. Find Shortest Delivery Path" << endl;
    cout << endl;
    cout << YELLOW << "--- Statistics ---" << RESET << endl;
    cout << "20.  Display Statistics" << endl;
    cout << endl;
    cout << RED << "0.  Back to Main Menu" << RESET << endl;
    cout << CYAN << "==========================================================" << RESET << endl;
}

void handleCommercialMenu() {
    int choice;
    do {
        displayCommercialMenu();
        choice = getIntInput(YELLOW + "Enter choice: " + RESET);

        switch (choice) {

        case 0:
            cout << BLUE << "Returning to Main Menu..." << RESET << endl;
            break;
            // Mall Operations
        case 1:
            commercialSystem.registerMall();
            break;
        case 2:
            commercialSystem.deleteMall();
            break;
        case 3:
            commercialSystem.updateMall();
            break;
        case 4:
            commercialSystem.connectMalls();
            break;
        case 5:
            commercialSystem.disconnectMalls();
            break;
        case 6:
            commercialSystem.displayAllMalls();
            break;
        case 7:
            commercialSystem.displayMallDetails();
            break;
        case 8:
            commercialSystem.displayMallNetwork();
            break;
        case 9:
            commercialSystem.findNearestMall();
            break;

            // Product Operations
        case 10:
            commercialSystem.addProduct();
            break;
        case 11:
            commercialSystem.removeProduct();
            break;
        case 12:
            commercialSystem.updateProduct();
            break;
        case 13:
            commercialSystem.updateStock();
            break;
        case 14:
            commercialSystem.displayMallProducts();
            break;
        case 15:
            commercialSystem.displayLowStockProducts();
            break;

            // Search Operations
        case 16:
            commercialSystem.searchProduct();
            break;
        case 17:
            commercialSystem.searchByCategory();
            break;
        case 18:
            commercialSystem.findMallWithProduct();
            break;

            // Navigation
        case 19:
            commercialSystem.findShortestDeliveryPath();
            break;

            // Statistics
        case 20:
            commercialSystem.displayStatistics();
            break;


        default: cout << RED << "Invalid choice! Please try again." << RESET << endl;
        }

        if (choice != 0) {
            cout << "\nPress Enter to continue... ";
            cin.get();
        }
    } while (choice != 0);
}

// FACILITIES MENU

void displayFacilitiesMenu() {
    cout << "\n" << BOLD << CYAN << "================ PUBLIC FACILITIES =================" << RESET << endl;
    cout << YELLOW << "--- Facility Operations ---" << RESET << endl;
    cout << "1.  Register Facility" << endl;
    cout << "2.  Delete Facility" << endl;
    cout << "3.  Update Facility" << endl;
    cout << "4.  Connect Facilities (Add Path)" << endl;
    cout << "5.  Disconnect Facilities" << endl;
    cout << "6.  Display All Facilities" << endl;
    cout << "7.  Display Facility Details" << endl;
    cout << "8.  Display Facility Network" << endl;
    cout << "9.   Display by Type" << endl;
    cout << "10. Display 24-Hour Facilities" << endl;
    cout << endl;
    cout << YELLOW << "--- Amenity Operations ---" << RESET << endl;
    cout << "11. Add Amenity" << endl;
    cout << "12. Remove Amenity" << endl;
    cout << "13. Update Amenity Status" << endl;
    cout << endl;
    cout << YELLOW << "--- Rating ---" << RESET << endl;
    cout << "14. Rate Facility" << endl;
    cout << endl;
    cout << YELLOW << "--- Search Operations ---" << RESET << endl;
    cout << "15. Search by Name" << endl;
    cout << "16. Search by Type" << endl;
    cout << "17. Find Nearest Facility" << endl;
    cout << "18. Find Facility with Amenity" << endl;
    cout << endl;
    cout << YELLOW << "--- Navigation ---" << RESET << endl;
    cout << "19. Find Shortest Path" << endl;
    cout << endl;
    cout << YELLOW << "--- Statistics ---" << RESET << endl;
    cout << "20.  Display Statistics" << endl;
    cout << endl;
    cout << RED << "0.  Back to Main Menu" << RESET << endl;
    cout << CYAN << "=====================================================" << RESET << endl;
}

void handleFacilitiesMenu() {
    int choice;
    do {
        displayFacilitiesMenu();
        choice = getIntInput(YELLOW + "Enter choice: " + RESET);

        switch (choice) {

        case 0:
            cout << BLUE << "Returning to Main Menu..." << RESET << endl;
            break;
            // Facility Operations
        case 1:
            facilitiesSystem.registerFacility();
            break;
        case 2:
            facilitiesSystem.deleteFacility();
            break;
        case 3:
            facilitiesSystem.updateFacility();
            break;
        case 4:
            facilitiesSystem.connectFacilities();
            break;
        case 5:
            facilitiesSystem.disconnectFacilities();
            break;
        case 6:
            facilitiesSystem.displayAllFacilities();
            break;
        case 7:
            facilitiesSystem.displayFacilityDetails();
            break;
        case 8:
            facilitiesSystem.displayFacilityNetwork();
            break;
        case 9:
            facilitiesSystem.displayByType();
            break;
        case 10:
            facilitiesSystem.display24HourFacilities();
            break;

            // Amenity Operations
        case 11:
            facilitiesSystem.addAmenity();
            break;
        case 12:
            facilitiesSystem.removeAmenity();
            break;
        case 13:
            facilitiesSystem.updateAmenityStatus();
            break;

            // Rating

        case 14:
            facilitiesSystem.rateFacility();
            break;

            // Search Operations
        case 15:
            facilitiesSystem.searchByName();
            break;
        case 16:
            facilitiesSystem.searchByType();
            break;
        case 17:
            facilitiesSystem.findNearestFacility();
            break;
        case 18:
            facilitiesSystem.findFacilityWithAmenity();
            break;

            // Navigation
        case 19:
            facilitiesSystem.findShortestPath();
            break;

            // Statistics
        case 20:
            facilitiesSystem.displayStatistics();
            break;


        default: cout << RED << "Invalid choice!  Please try again." << RESET << endl;
        }

        if (choice != 0) {
            cout << "\nPress Enter to continue...";
            cin.get();
        }
    } while (choice != 0);
}

// POPULATION MENU


void displayPopulationMenu() {
    cout << "\n" << BOLD << CYAN << "============== POPULATION & HOUSING ==============" << RESET << endl;
    cout << YELLOW << "--- Citizen Operations ---" << RESET << endl;
    cout << "1.  Register Citizen" << endl;
    cout << "2.  Delete Citizen" << endl;
    cout << "3.  Update Citizen" << endl;
    cout << "4.  Search Citizen" << endl;
    cout << "5.  Transfer Citizen (Change Address)" << endl;
    cout << endl;
    cout << YELLOW << "--- Display Operations ---" << RESET << endl;
    cout << "6. Display All Citizens" << endl;
    cout << "7. Display by Sector" << endl;
    cout << "8. Display by Street" << endl;
    cout << "9. Display by House (Family)" << endl;
    cout << "10. Display Hierarchy (Tree View)" << endl;
    cout << "11. Display B-Tree Structure" << endl;
    cout << endl;
    cout << YELLOW << "--- Reports ---" << RESET << endl;
    cout << "12.  Occupation Report" << endl;
    cout << "13. Age Distribution Report" << endl;
    cout << "14.  Sector Population Report" << endl;
    cout << endl;
    cout << YELLOW << "--- Statistics ---" << RESET << endl;
    cout << "15. Display Statistics" << endl;
    cout << endl;
    cout << RED << "0.  Back to Main Menu" << RESET << endl;
    cout << CYAN << "===================================================" << RESET << endl;
}

void handlePopulationMenu() {
    int choice;
    do {
        displayPopulationMenu();
        choice = getIntInput(YELLOW + "Enter choice: " + RESET);

        switch (choice) {
        case 0:
            cout << BLUE << "Returning to Main Menu..." << RESET << endl;
            break;
            // Citizen Operations

        case 1:
            populationSystem.registerCitizen();
            break;
        case 2:
            populationSystem.deleteCitizen();
            break;
        case 3:
            populationSystem.updateCitizen();
            break;
        case 4:
            populationSystem.searchCitizen();
            break;
        case 5:
            populationSystem.transferCitizen();
            break;

            // Display Operations
        case 6:
            populationSystem.displayAllCitizens();
            break;
        case 7:
            populationSystem.displayCitizensBySector();
            break;
        case 8:
            populationSystem.displayCitizensByStreet();
            break;
        case 9:
            populationSystem.displayCitizensByHouse();
            break;
        case 10:
            populationSystem.displayHierarchy();
            break;
        case 11:
            populationSystem.displayBTreeStructure();
            break;

            // Reports
        case 12:
            populationSystem.generateOccupationReport();
            break;
        case 13:
            populationSystem.generateAgeReport();
            break;
        case 14:
            populationSystem.generateSectorReport();
            break;

            // Statistics
        case 15:
            populationSystem.displayStatistics();
            break;


        default: cout << RED << "Invalid choice! Please try again." << RESET << endl;
        }

        if (choice != 0) {
            cout << "\nPress Enter to continue...";
            cin.get();
        }
    } while (choice != 0);
}

// SYSTEM STATISTICS


void displaySystemStatistics() {
    cout << "\n" << BOLD << CYAN << "===============================================================" << endl;
    cout << "=              SYSTEM-WIDE STATISTICS                          =" << endl;
    cout << "================================================================" << RESET << endl;

    cout << "\n" << YELLOW << "=== Transport System ===" << RESET << endl;
    cout << "Bus Stops: " << transportSystem.getStopCount() << endl;
    cout << "Buses: " << transportSystem.getBusCount() << endl;

    cout << "\n" << YELLOW << "=== Medical System ===" << RESET << endl;
    cout << "Hospitals: " << medicalSystem.getHospitalCount() << endl;
    cout << "Pharmacies: " << medicalSystem.getPharmacyCount() << endl;

    cout << "\n" << YELLOW << "=== Education System ===" << RESET << endl;
    cout << "Schools: " << educationSystem.getSchoolCount() << endl;

    cout << "\n" << YELLOW << "=== Commercial System ===" << RESET << endl;
    cout << "Malls: " << commercialSystem.getMallCount() << endl;

    cout << "\n" << YELLOW << "=== Public Facilities ===" << RESET << endl;
    cout << "Facilities: " << facilitiesSystem.getFacilityCount() << endl;

    cout << "\n" << YELLOW << "=== Population ===" << RESET << endl;
    cout << "Registered Citizens: " << populationSystem.getCitizenCount() << endl;

    cout << "\n" << YELLOW << "=== Global Location Manager ===" << RESET << endl;
    GlobalLocationManager* locMgr = GlobalLocationManager::getInstance();
    cout << "Total Registered Locations: " << locMgr->getLocationCount() << endl;

    cout << "\n" << CYAN << "============================================" << RESET << endl;
}

// LOAD ALL DATA

void loadAllData() {
    cout << "\n" << BOLD << GREEN << "=== Loading All Data from Files ===" << RESET << endl;
    cout << "-----------------------------------" << endl;

    cout << "\n" << CYAN << "[1/6] Loading Transport Data..." << RESET << endl;
    transportSystem.loadFromFiles();

    cout << "\n" << CYAN << "[2/6] Loading Medical Data..." << RESET << endl;
    medicalSystem.loadFromFiles();

    cout << "\n" << CYAN << "[3/6] Loading Education Data..." << RESET << endl;
    educationSystem.loadFromFile();

    cout << "\n" << CYAN << "[4/6] Loading Commercial Data..." << RESET << endl;
    commercialSystem.loadFromFiles();

    cout << "\n" << CYAN << "[5/6] Loading Facilities Data..." << RESET << endl;
    facilitiesSystem.loadFromFile();

    cout << "\n" << CYAN << "[6/6] Loading Population Data..." << RESET << endl;
    populationSystem.loadFromFile();

    cout << "\n-----------------------------------" << endl;
    cout << BOLD << GREEN << "All data loaded successfully!" << RESET << endl;
    cout << "-----------------------------------" << endl;

    // Display summary
    cout << "\n" << YELLOW << "=== Data Loading Summary ===" << RESET << endl;
    cout << "Bus Stops: " << transportSystem.getStopCount() << endl;
    cout << "Buses: " << transportSystem.getBusCount() << endl;
    cout << "Hospitals: " << medicalSystem.getHospitalCount() << endl;
    cout << "Pharmacies: " << medicalSystem.getPharmacyCount() << endl;
    cout << "Schools: " << educationSystem.getSchoolCount() << endl;
    cout << "Malls: " << commercialSystem.getMallCount() << endl;
    cout << "Facilities: " << facilitiesSystem.getFacilityCount() << endl;
    cout << "Citizens: " << populationSystem.getCitizenCount() << endl;
}


void displaySFMLVisualizationMenu() {
    cout << "\n" << BOLD << MAGENTA << "================ SFML VISUALIZATIONS ================" << RESET << endl;
    cout << YELLOW << "--- City Visualizations ---" << RESET << endl;
    cout << "1.  City Map (All Locations)" << endl;
    cout << "2.  Graph Network (Roads & Connections)" << endl;
    cout << "3.  Bus Route Viewer" << endl;
    cout << "4.  Population Heatmap" << endl;
    cout << "5.  Statistics Dashboard" << endl;
    cout << endl;
    cout << YELLOW << "--- Quick Views ---" << RESET << endl;
    cout << "6.  Transport Network" << endl;
    cout << "7.  Hospital Locations" << endl;
    cout << "8.  School Locations" << endl;
    cout << "9.  Mall Locations" << endl;
    cout << "10. All Public Facilities" << endl;
    cout << endl;
    cout << RED << "0.  Back to Main Menu" << RESET << endl;
    cout << MAGENTA << "=====================================================" << RESET << endl;
}

// Collect all city locations for visualization

void visualizeCityMapSFML() {
    const int MAX_LOCS = 500;
    string locations[MAX_LOCS];
    double lats[MAX_LOCS];
    double lons[MAX_LOCS];
    int types[MAX_LOCS];

    cout << "\n" << CYAN << "Preparing city map visualization..." << RESET << endl;

    CityGraph* cityGraph = CityGraph::getInstance();
    int count = cityGraph->exportAllLocations(locations, lats, lons, types, MAX_LOCS);

    cout << "Total locations: " << count << endl;

    if (count == 0) {
        cout << RED << "No locations to display. Please load data first (Main Menu -> 8)" << RESET << endl;
        return;
    }

    cout << GREEN << "Opening SFML window..." << RESET << endl;
    cout << "Note: Make sure 'arial.ttf' is in the same folder as the executable!" << endl;
    SFMLVisualizer::visualizeCityMap(locations, lats, lons, types, count);
}

void visualizeTransportNetworkSFML() {
    const int MAX_STOPS = 200;
    string stops[MAX_STOPS];
    double lats[MAX_STOPS];
    double lons[MAX_STOPS];
    bool adjacency[MAX_STOPS][50];
    double weights[MAX_STOPS][50];

    cout << "\n" << CYAN << "Preparing transport network visualization..." << RESET << endl;

    int count = transportSystem.exportRoadNetwork(stops, lats, lons,
        adjacency, weights, MAX_STOPS);

    cout << "Total stops: " << count << endl;

    if (count < 2) {
        cout << RED << "Not enough bus stops for network visualization." << RESET << endl;
        cout << "Please add bus stops first (Transport Menu -> 1)" << endl;
        return;
    }

    cout << GREEN << "Opening SFML window..." << RESET << endl;
    SFMLVisualizer::visualizeGraphNetwork(stops, lats, lons, count, adjacency, weights);
}

void visualizeBusRouteSFML() {
    string busNumber = getStringInput(YELLOW + "\nEnter Bus Number to visualize: " + RESET);

    const int MAX_STOPS = 50;
    string company;
    string stops[MAX_STOPS];
    double lats[MAX_STOPS];
    double lons[MAX_STOPS];
    int stopCount = 0;
    int currentStopIndex = 0;

    cout << "\n" << CYAN << "Preparing bus route visualization..." << RESET << endl;

    bool success = transportSystem.exportBusRoute(busNumber, company, stops, lats, lons,
        stopCount, currentStopIndex, MAX_STOPS);

    if (!success || stopCount == 0) {
        cout << RED << "Bus '" << busNumber << "' not found or has no route!" << RESET << endl;
        return;
    }

    cout << "Bus: " << busNumber << " (" << company << ")" << endl;
    cout << "Route stops: " << stopCount << endl;
    cout << GREEN << "Opening SFML window..." << RESET << endl;

    SFMLVisualizer::visualizeBusRoute(busNumber, stops, lats, lons,
        stopCount, currentStopIndex);
}


void visualizeLocationTypeSFML(int locationType, const string& typeName) {
    const int MAX_LOCS = 200;
    string locations[MAX_LOCS];
    double lats[MAX_LOCS];
    double lons[MAX_LOCS];
    int types[MAX_LOCS];

    cout << "\n" << CYAN << "Preparing " << typeName << " visualization..." << RESET << endl;

    CityGraph* cityGraph = CityGraph::getInstance();
    int count = cityGraph->exportLocationsByType(locationType, locations,
        lats, lons, MAX_LOCS);

    cout << "Total " << typeName << ": " << count << endl;

    if (count == 0) {
        cout << RED << "No " << typeName << " locations found." << RESET << endl;
        return;
    }

    // Fill types array
    for (int i = 0; i < count; i++) {
        types[i] = locationType;
    }

    cout << GREEN << "Opening SFML window..." << RESET << endl;
    SFMLVisualizer::visualizeCityMap(locations, lats, lons, types, count);
}

void visualizeStatsDashboardSFML() {
    cout << "\n" << CYAN << "Preparing statistics dashboard..." << RESET << endl;

    int totalPop = populationSystem.getCitizenCount();
    int buses = transportSystem.getBusCount();
    int hospitals = medicalSystem.getHospitalCount();
    int schools = educationSystem.getSchoolCount();
    int malls = commercialSystem.getMallCount();
    int facilities = facilitiesSystem.getFacilityCount();

    cout << GREEN << "Opening SFML window..." << RESET << endl;
    SFMLVisualizer::visualizeStatsDashboard(totalPop, buses, hospitals,
        schools, malls, facilities);
}

void visualizePopulationHeatmapSFML() {
    const int MAX_SECTORS = 50;
    string sectors[MAX_SECTORS];
    int populations[MAX_SECTORS];

    cout << "\n" << CYAN << "Preparing population heatmap..." << RESET << endl;

    int sectorCount = populationSystem.exportSectorPopulation(sectors, populations, MAX_SECTORS);

    cout << "Total sectors: " << sectorCount << endl;

    if (sectorCount == 0) {
        cout << RED << "No population data available." << RESET << endl;
        cout << "Please register citizens first (Population Menu)" << endl;
        return;
    }

    cout << GREEN << "Opening SFML window..." << RESET << endl;
    SFMLVisualizer::visualizePopulationHeatmap(sectors, populations, sectorCount);
}

// Handle SFML visualization menu
void handleSFMLVisualizationMenu() {
    int choice;
    do {
        displaySFMLVisualizationMenu();
        choice = getIntInput(YELLOW + "Enter choice: " + RESET);

        switch (choice) {
        case 1:
            visualizeCityMapSFML();
            break;

        case 2:
            visualizeTransportNetworkSFML();
            break;

        case 3:
            visualizeBusRouteSFML();
            break;

        case 4:
            visualizePopulationHeatmapSFML();
            break;

        case 5:
            visualizeStatsDashboardSFML();
            break;

        case 6:
            visualizeLocationTypeSFML(0, "Transport Stops");
            break;

        case 7:
            visualizeLocationTypeSFML(1, "Hospitals");
            break;

        case 8:
            visualizeLocationTypeSFML(3, "Schools");
            break;

        case 9:
            visualizeLocationTypeSFML(4, "Malls");
            break;

        case 10:
            visualizeLocationTypeSFML(5, "Public Facilities");
            break;

        case 0:
            cout << BLUE << "Returning to Main Menu..." << RESET << endl;
            break;

        default:
            cout << RED << "Invalid choice!" << RESET << endl;
        }

        if (choice != 0 && choice >= 1 && choice <= 10) {
            cout << "\nPress Enter to continue... ";
            cin.get();
        }
    } while (choice != 0);
}

int main() {
    displayMainHeader();

    cout << "\n" << GREEN << "Welcome to the Islamabad City Management System!" << RESET << endl;
    cout << "This system manages Transport, Medical, Education," << endl;
    cout << "Commercial, Public Facilities, and Population data." << endl;

    // Ask if user wants to load data from files
    cout << "\n" << YELLOW << "Would you like to load existing data from files? (yes/no): " << RESET;
    string loadChoice;
    getline(cin, loadChoice);

    if (toLowerStr(trimString(loadChoice)) == "yes" ||
        toLowerStr(trimString(loadChoice)) == "y") {
        loadAllData();
        cout << "\nPress Enter to continue to Main Menu... ";
        cin.get();
    }

    int choice;
    do {
        displayMainMenu();
        choice = getIntInput(YELLOW + "Enter choice: " + RESET);

        switch (choice) {
        case 1:
            handleTransportMenu();
            break;

        case 2:
            handleMedicalMenu();
            break;

        case 3:
            handleEducationMenu();
            break;

        case 4:
            handleCommercialMenu();
            break;

        case 5:
            handleFacilitiesMenu();
            break;

        case 6:
            handlePopulationMenu();
            break;

        case 7:
            displaySystemStatistics();
            cout << "\nPress Enter to continue...";
            cin.get();
            break;

        case 8:
            loadAllData();
            cout << "\nPress Enter to continue... ";
            cin.get();
            break;

        case 9:
            bonusModulesMenu.run();
            break;

        case 10:
            handleSFMLVisualizationMenu();
            break;

        case 0:
            cout << "\n" << endl;
            cout << BOLD << CYAN << " Thank you for using Islamabad City Management System!       " << RESET << endl;
            cout << BOLD << CYAN << " Allah Hafiz                                                 " << RESET << endl;
            cout << "" << endl;
            break;

        default:
            cout << RED << "Invalid choice! Please try again." << RESET << endl;
            cout << "\nPress Enter to continue...";
            cin.get();
        }

    } while (choice != 0);

    return 0;
}