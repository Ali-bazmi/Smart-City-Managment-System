#pragma once


#include "Utils.h"
#include "GlobalLocationManager.h"
#include "CityGraph.h"


struct StudentNode {
    string name;
    int rollNumber;
    string contactNumber;
    StudentNode* next;

    StudentNode(const string& n, int r, const string& contact = "")
        : name(n), rollNumber(r), contactNumber(contact), next(nullptr) {
    }
};


struct SectionNode {
    string sectionName;
    int studentCount;
    StudentNode* studentHead;
    SectionNode* next;

    SectionNode(const string& n) : sectionName(n), studentCount(0), studentHead(nullptr), next(nullptr) {}
};


struct TeacherNode {
    string name;
    int empID;
    string designation;
    string subject;
    TeacherNode* next;

    TeacherNode(const string& n, int id, const string& des, const string& sub = "")
        : name(n), empID(id), designation(des), subject(sub), next(nullptr) {
    }
};


struct DeptNode {
    string deptName;
    int sectionCount;
    int teacherCount;
    SectionNode* sectionHead;
    TeacherNode* teacherHead;
    DeptNode* next;

    DeptNode(const string& n) : deptName(n), sectionCount(0), teacherCount(0),
        sectionHead(nullptr), teacherHead(nullptr), next(nullptr) {
    }
};


struct SchoolNode {
    string schoolID;
    string name;
    string sector;
    double lat, lon;
    double rating;
    string subjects;
    int deptCount;
    DeptNode* deptHead;
    SchoolNode* next;

    SchoolNode(const string& id, const string& n, const string& sec, double la, double lo, double r, const string& subs)
        : schoolID(id), name(n), sector(sec), lat(la), lon(lo), rating(r), subjects(subs),
        deptCount(0), deptHead(nullptr), next(nullptr) {
    }
};


struct SubjectHashNode {
    string subject;
    SchoolNode* schoolRef;
    SubjectHashNode* next;

    SubjectHashNode(const string& sub, SchoolNode* s) : subject(sub), schoolRef(s), next(nullptr) {}
};


class SubjectHashTable {
private:
    static const int TABLE_SIZE = 50;
    SubjectHashNode* table[TABLE_SIZE];

    int hashFunc(const string& key) {
        return polynomialHash(key, TABLE_SIZE);
    }

public:
    SubjectHashTable() {
        for (int i = 0; i < TABLE_SIZE; i++) {
            table[i] = nullptr;
        }
    }

    void insert(const string& subject, SchoolNode* school) {
        string cleanSubject = trimString(toLowerStr(subject));
        if (cleanSubject.empty())
            return;

        // Check if already exists for this school
        int idx = hashFunc(cleanSubject);
        SubjectHashNode* current = table[idx];
        while (current != nullptr) {
            if (current->subject == cleanSubject && current->schoolRef == school) {
                return;
            }
            current = current->next;
        }

        SubjectHashNode* newNode = new SubjectHashNode(cleanSubject, school);
        newNode->next = table[idx];
        table[idx] = newNode;
    }

    void remove(const string& subject, SchoolNode* school) {
        string cleanSubject = trimString(toLowerStr(subject));
        int idx = hashFunc(cleanSubject);

        if (table[idx] == nullptr)
            return;

        // Remove all entries for this school and subject
        while (table[idx] != nullptr &&
            table[idx]->subject == cleanSubject &&
            table[idx]->schoolRef == school) {
            SubjectHashNode* temp = table[idx];
            table[idx] = table[idx]->next;
            delete temp;
        }

        if (table[idx] == nullptr)
            return;

        SubjectHashNode* current = table[idx];
        while (current->next != nullptr) {
            if (current->next->subject == cleanSubject && current->next->schoolRef == school) {
                SubjectHashNode* temp = current->next;
                current->next = current->next->next;
                delete temp;
            }
            else {
                current = current->next;
            }
        }
    }

    void removeAllForSchool(SchoolNode* school) {
        for (int i = 0; i < TABLE_SIZE; i++) {
            while (table[i] != nullptr && table[i]->schoolRef == school) {
                SubjectHashNode* temp = table[i];
                table[i] = table[i]->next;
                delete temp;
            }

            if (table[i] == nullptr)
                continue;

            SubjectHashNode* current = table[i];
            while (current->next != nullptr) {
                if (current->next->schoolRef == school) {
                    SubjectHashNode* temp = current->next;
                    current->next = current->next->next;
                    delete temp;
                }
                else {
                    current = current->next;
                }
            }
        }
    }

    void search(const string& subject) {
        string cleanSubject = trimString(toLowerStr(subject));
        int idx = hashFunc(cleanSubject);
        SubjectHashNode* current = table[idx];
        bool found = false;

        cout << "\n=== Schools offering '" << subject << "' ===" << endl;
        while (current != nullptr) {
            if (current->subject == cleanSubject) {
                cout << "- " << current->schoolRef->name << " (" << current->schoolRef->sector
                    << ") [Rating: " << current->schoolRef->rating << "]" << endl;
                found = true;
            }
            current = current->next;
        }
        if (!found) {
            cout << "No schools found offering this subject." << endl;
        }
    }

    void displayAllSubjects() {
        cout << "\n=== All Subjects in System ===" << endl;

        // Collect unique subjects
        string uniqueSubjects[200];
        int uniqueCount = 0;

        for (int i = 0; i < TABLE_SIZE; i++) {
            SubjectHashNode* current = table[i];
            while (current != nullptr) {
                bool exists = false;
                for (int j = 0; j < uniqueCount; j++) {
                    if (uniqueSubjects[j] == current->subject) {
                        exists = true;
                        break;
                    }
                }
                if (!exists && uniqueCount < 200) {
                    uniqueSubjects[uniqueCount++] = current->subject;
                }
                current = current->next;
            }
        }

        if (uniqueCount == 0) {
            cout << "No subjects registered." << endl;
            return;
        }

        for (int i = 0; i < uniqueCount; i++) {
            cout << (i + 1) << ". " << uniqueSubjects[i] << endl;
        }
    }

    ~SubjectHashTable() {
        for (int i = 0; i < TABLE_SIZE; i++) {
            while (table[i] != nullptr) {
                SubjectHashNode* temp = table[i];
                table[i] = table[i]->next;
                delete temp;
            }
        }
    }
};



class SchoolRankingHeap {
private:
    static const int MAX_SIZE = 100;
    SchoolNode* heap[MAX_SIZE];
    int size;

    void swap(int i, int j) {
        SchoolNode* temp = heap[i];
        heap[i] = heap[j];
        heap[j] = temp;
    }

    void siftUp(int i) {
        while (i > 0) {
            int parent = (i - 1) / 2;
            if (heap[i]->rating > heap[parent]->rating) {
                swap(i, parent);
                i = parent;
            }
            else
                break;
        }
    }

    void siftDown(int i) {
        while (true) {
            int largest = i;
            int left = 2 * i + 1;
            int right = 2 * i + 2;

            if (left < size && heap[left]->rating > heap[largest]->rating)
                largest = left;
            if (right < size && heap[right]->rating > heap[largest]->rating)
                largest = right;

            if (largest != i) {
                swap(i, largest);
                i = largest;
            }
            else break;
        }
    }

    int findIndex(SchoolNode* school) {
        for (int i = 0; i < size; i++) {
            if (heap[i] == school)
                return i;
        }
        return -1;
    }

public:
    SchoolRankingHeap() : size(0) {}

    void insert(SchoolNode* school) {
        if (size >= MAX_SIZE) {
            cout << "Warning: Ranking heap is full." << endl;
            return;
        }
        heap[size] = school;
        siftUp(size);
        size++;
    }

    void remove(SchoolNode* school) {
        int idx = findIndex(school);
        if (idx == -1)
            return;

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

    void updateRating(SchoolNode* school) {
        int idx = findIndex(school);
        if (idx == -1)
            return;

        siftUp(idx);
        siftDown(idx);
    }

    SchoolNode* getTop() {
        if (size == 0)
            return nullptr;
        return heap[0];
    }

    void displayRankings(int topN = -1) {
        if (size == 0) {
            cout << "No schools to rank." << endl;
            return;
        }

        SchoolNode* tempArr[MAX_SIZE];
        for (int i = 0; i < size; i++) {
            tempArr[i] = heap[i];
        }


        for (int i = 0; i < size - 1; i++) {
            for (int j = 0; j < size - i - 1; j++) {
                if (tempArr[j]->rating < tempArr[j + 1]->rating) {
                    SchoolNode* temp = tempArr[j];
                    tempArr[j] = tempArr[j + 1];
                    tempArr[j + 1] = temp;
                }
            }
        }

        cout << "\n=== School Rankings ===" << endl;
        int displayCount = (topN > 0 && topN < size) ? topN : size;
        for (int i = 0; i < displayCount; i++) {
            cout << (i + 1) << ". " << tempArr[i]->name << " (" << tempArr[i]->sector
                << ") - Rating: " << tempArr[i]->rating << "/5. 0" << endl;
        }
    }

    int getSize() { return size; }
};


class EducationSystem {
private:
    SchoolNode* schoolsHead;
    SubjectHashTable subjectHash;
    SchoolRankingHeap rankingHeap;
    int schoolCount;
    int manualIDCounter;



    SchoolNode* findSchoolByName(const string& name) {
        SchoolNode* current = schoolsHead;
        while (current != nullptr) {
            if (stringsEqualIgnoreCase(current->name, name)) return current;
            current = current->next;
        }
        return nullptr;
    }

    SchoolNode* findSchoolByID(const string& id) {
        SchoolNode* current = schoolsHead;
        while (current != nullptr) {
            if (stringsEqualIgnoreCase(current->schoolID, id))
                return current;
            current = current->next;
        }
        return nullptr;
    }

    DeptNode* findDepartment(SchoolNode* school, const string& deptName) {
        if (school == nullptr)
            return nullptr;
        DeptNode* dept = school->deptHead;
        while (dept != nullptr) {
            if (stringsEqualIgnoreCase(dept->deptName, deptName))
                return dept;
            dept = dept->next;
        }
        return nullptr;
    }

    SectionNode* findSection(DeptNode* dept, const string& secName) {
        if (dept == nullptr)
            return nullptr;
        SectionNode* sec = dept->sectionHead;
        while (sec != nullptr) {
            if (stringsEqualIgnoreCase(sec->sectionName, secName))
                return sec;
            sec = sec->next;
        }
        return nullptr;
    }

    StudentNode* findStudent(SectionNode* section, int rollNumber) {
        if (section == nullptr)
            return nullptr;
        StudentNode* stu = section->studentHead;
        while (stu != nullptr) {
            if (stu->rollNumber == rollNumber)
                return stu;
            stu = stu->next;
        }
        return nullptr;
    }

    StudentNode* findStudentByName(SectionNode* section, const string& name) {
        if (section == nullptr)
            return nullptr;
        StudentNode* stu = section->studentHead;
        while (stu != nullptr) {
            if (stringsEqualIgnoreCase(stu->name, name))
                return stu;
            stu = stu->next;
        }
        return nullptr;
    }

    TeacherNode* findTeacher(DeptNode* dept, int empID) {
        if (dept == nullptr)
            return nullptr;
        TeacherNode* teacher = dept->teacherHead;
        while (teacher != nullptr) {
            if (teacher->empID == empID)
                return teacher;
            teacher = teacher->next;
        }
        return nullptr;
    }

    TeacherNode* findTeacherByName(DeptNode* dept, const string& name) {
        if (dept == nullptr)
            return nullptr;
        TeacherNode* teacher = dept->teacherHead;
        while (teacher != nullptr) {
            if (stringsEqualIgnoreCase(teacher->name, name))
                return teacher;
            teacher = teacher->next;
        }
        return nullptr;
    }

    void processSubjects(const string& subjects, SchoolNode* school) {
        string subjectArray[20];
        int count = splitString(subjects, ',', subjectArray, 20);
        for (int i = 0; i < count; i++) {
            string sub = trimString(subjectArray[i]);
            if (!sub.empty()) {
                subjectHash.insert(sub, school);
            }
        }
    }

    bool isRollTaken(SchoolNode* school, int roll) {
        DeptNode* dept = school->deptHead;
        while (dept != nullptr) {
            SectionNode* sec = dept->sectionHead;
            while (sec != nullptr) {
                StudentNode* stu = sec->studentHead;
                while (stu != nullptr) {
                    if (stu->rollNumber == roll)
                        return true;
                    stu = stu->next;
                }
                sec = sec->next;
            }
            dept = dept->next;
        }
        return false;
    }

    bool isTeacherIDTaken(SchoolNode* school, int empID) {
        DeptNode* dept = school->deptHead;
        while (dept != nullptr) {
            TeacherNode* teacher = dept->teacherHead;
            while (teacher != nullptr) {
                if (teacher->empID == empID)
                    return true;
                teacher = teacher->next;
            }
            dept = dept->next;
        }
        return false;
    }

    SchoolNode* selectSchool() {
        string name = getStringInput("Enter School Name: ");
        SchoolNode* school = findSchoolByName(name);
        if (school == nullptr) {
            cout << " School '" << name << "' not found!" << endl;
        }
        return school;
    }

    int getTotalStudents(SchoolNode* school) {
        int count = 0;
        DeptNode* dept = school->deptHead;
        while (dept != nullptr) {
            SectionNode* sec = dept->sectionHead;
            while (sec != nullptr) {
                count += sec->studentCount;
                sec = sec->next;
            }
            dept = dept->next;
        }
        return count;
    }

    int getTotalTeachers(SchoolNode* school) {
        int count = 0;
        DeptNode* dept = school->deptHead;
        while (dept != nullptr) {
            count += dept->teacherCount;
            dept = dept->next;
        }
        return count;
    }

    // Delete all students in a section
    void deleteAllStudents(SectionNode* section) {
        while (section->studentHead != nullptr) {
            StudentNode* temp = section->studentHead;
            section->studentHead = section->studentHead->next;
            delete temp;
        }
        section->studentCount = 0;
    }

    // Delete all teachers in a department
    void deleteAllTeachers(DeptNode* dept) {
        while (dept->teacherHead != nullptr) {
            TeacherNode* temp = dept->teacherHead;
            dept->teacherHead = dept->teacherHead->next;
            delete temp;
        }
        dept->teacherCount = 0;
    }

    // Delete all sections in a department
    void deleteAllSections(DeptNode* dept) {
        while (dept->sectionHead != nullptr) {
            SectionNode* temp = dept->sectionHead;
            dept->sectionHead = dept->sectionHead->next;
            deleteAllStudents(temp);
            delete temp;
        }
        dept->sectionCount = 0;
    }

    // Delete all departments in a school
    void deleteAllDepartments(SchoolNode* school) {
        while (school->deptHead != nullptr) {
            DeptNode* temp = school->deptHead;
            school->deptHead = school->deptHead->next;
            deleteAllSections(temp);
            deleteAllTeachers(temp);
            delete temp;
        }
        school->deptCount = 0;
    }

public:
    EducationSystem() : schoolsHead(nullptr), schoolCount(0), manualIDCounter(100) {}


    void registerSchool() {
        cout << "\n=== Register New School ===" << endl;

        string name = getStringInput("Enter School Name: ");
        if (name.empty()) {
            cout << " School name cannot be empty." << endl;
            return;
        }

        if (findSchoolByName(name) != nullptr) {
            cout << " School '" << name << "' already exists!" << endl;
            return;
        }

        string sector = getStringInput("Enter Sector (e.g., G-10, F-8): ");
        if (sector.empty()) {
            cout << " Sector cannot be empty." << endl;
            return;
        }

        double lat = getDoubleInput("Enter Latitude: ");
        double lon = getDoubleInput("Enter Longitude: ");

        GlobalLocationManager* locMgr = GlobalLocationManager::getInstance();
        if (!locMgr->registerLocation(lat, lon, "School", name)) {
            return;
        }

        double rating = getDoubleInput("Enter Rating (0.0 - 5.0): ");
        if (rating < 0.0) rating = 0.0;
        if (rating > 5.0) rating = 5.0;

        string subjects = getStringInput("Enter Subjects (comma-separated, e.g., Math, Physics, English): ");

        string schoolID = "S" + intToString(manualIDCounter++);

        SchoolNode* newSchool = new SchoolNode(schoolID, name, sector, lat, lon, rating, subjects);
        newSchool->next = schoolsHead;
        schoolsHead = newSchool;
        schoolCount++;

        rankingHeap.insert(newSchool);
        processSubjects(subjects, newSchool);

        // Register with CityGraph for unified city-wide graph
        CityGraph::getInstance()->addLocation(schoolID, name, LOC_SCHOOL, lat, lon, newSchool);

        cout << "School '" << name << "' registered successfully with ID: " << schoolID << endl;
    }

    void deleteSchool() {
        cout << "\n=== Delete School ===" << endl;

        string name = getStringInput("Enter School Name to delete: ");
        SchoolNode* school = findSchoolByName(name);

        if (school == nullptr) {
            cout << " School not found!" << endl;
            return;
        }


        cout << "\nSchool Statistics:" << endl;
        cout << "  Departments: " << school->deptCount << endl;
        cout << "  Total Teachers: " << getTotalTeachers(school) << endl;
        cout << "  Total Students: " << getTotalStudents(school) << endl;

        string confirm = getStringInput("\nType 'YES' to confirm deletion of '" + name + "' and ALL its data: ");
        if (confirm != "YES") {
            cout << "Deletion cancelled." << endl;
            return;
        }

        // Remove from ranking heap
        rankingHeap.remove(school);

        // Remove from subject hash table
        subjectHash.removeAllForSchool(school);

        // Remove from linked list
        if (stringsEqualIgnoreCase(schoolsHead->name, name)) {
            SchoolNode* temp = schoolsHead;
            schoolsHead = schoolsHead->next;

            deleteAllDepartments(temp);
            GlobalLocationManager::getInstance()->removeLocation(temp->lat, temp->lon);
            CityGraph::getInstance()->removeLocation(temp->schoolID);
            delete temp;
        }
        else {
            SchoolNode* current = schoolsHead;
            while (current->next != nullptr) {
                if (stringsEqualIgnoreCase(current->next->name, name)) {
                    SchoolNode* temp = current->next;
                    current->next = current->next->next;

                    deleteAllDepartments(temp);
                    GlobalLocationManager::getInstance()->removeLocation(temp->lat, temp->lon);
                    CityGraph::getInstance()->removeLocation(temp->schoolID);
                    delete temp;
                    break;
                }
                current = current->next;
            }
        }

        schoolCount--;
        cout << "School '" << name << "' and all its data deleted successfully!" << endl;
    }

    void updateSchool() {
        cout << "\n=== Update School ===" << endl;

        SchoolNode* school = selectSchool();
        if (school == nullptr)
            return;

        cout << "\nCurrent Details:" << endl;
        cout << "  ID: " << school->schoolID << endl;
        cout << "  Name: " << school->name << endl;
        cout << "  Sector: " << school->sector << endl;
        cout << "  Rating: " << school->rating << "/5.0" << endl;
        cout << "  Subjects: " << school->subjects << endl;

        cout << "\nWhat would you like to update?" << endl;
        cout << "1. Name" << endl;
        cout << "2. Rating" << endl;
        cout << "3. Subjects" << endl;
        cout << "4. All of the above" << endl;
        cout << "0. Cancel" << endl;

        int choice = getIntInput("Enter choice: ");

        if (choice == 0) {
            cout << "Update cancelled." << endl;
            return;
        }

        if (choice == 1 || choice == 4) {
            string newName = getStringInput("Enter new name (or press Enter to keep current): ");
            if (!newName.empty()) {
                if (findSchoolByName(newName) != nullptr && !stringsEqualIgnoreCase(newName, school->name)) {
                    cout << " A school with name '" << newName << "' already exists!" << endl;
                    return;
                }
                school->name = newName;
            }
        }

        if (choice == 2 || choice == 4) {
            double newRating = getDoubleInput("Enter new rating (0. 0 - 5.0): ");
            if (newRating >= 0.0 && newRating <= 5.0) {
                school->rating = newRating;
                rankingHeap.updateRating(school);
            }
        }

        if (choice == 3 || choice == 4) {
            string newSubjects = getStringInput("Enter new subjects (comma-separated): ");
            if (!newSubjects.empty()) {
                // Remove old subjects from hash
                subjectHash.removeAllForSchool(school);
                // Add new subjects
                school->subjects = newSubjects;
                processSubjects(newSubjects, school);
            }
        }

        cout << "School updated successfully!" << endl;
    }



    void addDepartment() {
        cout << "\n=== Add Department ===" << endl;

        SchoolNode* school = selectSchool();
        if (school == nullptr)
            return;

        string deptName = getStringInput("Enter Department Name: ");
        if (deptName.empty()) {
            cout << " Department name cannot be empty." << endl;
            return;
        }

        if (findDepartment(school, deptName) != nullptr) {
            cout << " Department '" << deptName << "' already exists in this school!" << endl;
            return;
        }

        DeptNode* newDept = new DeptNode(deptName);
        newDept->next = school->deptHead;
        school->deptHead = newDept;
        school->deptCount++;

        cout << "Department '" << deptName << "' added to '" << school->name << "'!" << endl;
    }

    void deleteDepartment() {
        cout << "\n=== Delete Department ===" << endl;

        SchoolNode* school = selectSchool();
        if (school == nullptr)
            return;

        if (school->deptHead == nullptr) {
            cout << " No departments in this school!" << endl;
            return;
        }

        // Display departments
        cout << "\nDepartments in " << school->name << ":" << endl;
        DeptNode* dept = school->deptHead;
        int count = 0;
        while (dept != nullptr) {
            cout << (++count) << ". " << dept->deptName
                << " (Sections: " << dept->sectionCount
                << ", Teachers: " << dept->teacherCount << ")" << endl;
            dept = dept->next;
        }

        string deptName = getStringInput("\nEnter Department Name to delete: ");
        dept = findDepartment(school, deptName);

        if (dept == nullptr) {
            cout << " Department not found!" << endl;
            return;
        }

        // Count students
        int studentCount = 0;
        SectionNode* sec = dept->sectionHead;
        while (sec != nullptr) {
            studentCount += sec->studentCount;
            sec = sec->next;
        }

        cout << "\nThis department has:" << endl;
        cout << "  Sections: " << dept->sectionCount << endl;
        cout << "  Teachers: " << dept->teacherCount << endl;
        cout << "  Students: " << studentCount << endl;

        string confirm = getStringInput("Type 'YES' to confirm deletion: ");
        if (confirm != "YES") {
            cout << "Deletion cancelled." << endl;
            return;
        }

        // Remove from linked list
        if (stringsEqualIgnoreCase(school->deptHead->deptName, deptName)) {
            DeptNode* temp = school->deptHead;
            school->deptHead = school->deptHead->next;
            deleteAllSections(temp);
            deleteAllTeachers(temp);
            delete temp;
        }
        else {
            DeptNode* current = school->deptHead;
            while (current->next != nullptr) {
                if (stringsEqualIgnoreCase(current->next->deptName, deptName)) {
                    DeptNode* temp = current->next;
                    current->next = current->next->next;
                    deleteAllSections(temp);
                    deleteAllTeachers(temp);
                    delete temp;
                    break;
                }
                current = current->next;
            }
        }

        school->deptCount--;
        cout << "Department deleted successfully!" << endl;
    }

    void updateDepartment() {
        cout << "\n=== Update Department ===" << endl;

        SchoolNode* school = selectSchool();
        if (school == nullptr)
            return;

        string deptName = getStringInput("Enter Department Name to update: ");
        DeptNode* dept = findDepartment(school, deptName);

        if (dept == nullptr) {
            cout << " Department not found!" << endl;
            return;
        }

        string newName = getStringInput("Enter new department name (or press Enter to keep current): ");
        if (!newName.empty()) {
            if (findDepartment(school, newName) != nullptr && !stringsEqualIgnoreCase(newName, deptName)) {
                cout << " A department with name '" << newName << "' already exists!" << endl;
                return;
            }
            dept->deptName = newName;
            cout << "Department renamed to '" << newName << "'!" << endl;
        }
        else {
            cout << "No changes made." << endl;
        }
    }


    void addSection() {
        cout << "\n=== Add Section ===" << endl;

        SchoolNode* school = selectSchool();
        if (school == nullptr)
            return;

        string deptName = getStringInput("Enter Department Name: ");
        DeptNode* dept = findDepartment(school, deptName);
        if (dept == nullptr) {
            cout << " Department '" << deptName << "' not found!" << endl;
            return;
        }

        string secName = getStringInput("Enter Section Name (e.g., 9-A, 10-B): ");
        if (secName.empty()) {
            cout << " Section name cannot be empty." << endl;
            return;
        }

        if (findSection(dept, secName) != nullptr) {
            cout << " Section '" << secName << "' already exists in this department!" << endl;
            return;
        }

        SectionNode* newSec = new SectionNode(secName);
        newSec->next = dept->sectionHead;
        dept->sectionHead = newSec;
        dept->sectionCount++;

        cout << "Section '" << secName << "' added to department '" << deptName << "'!" << endl;
    }

    void deleteSection() {
        cout << "\n=== Delete Section ===" << endl;

        SchoolNode* school = selectSchool();
        if (school == nullptr)
            return;

        string deptName = getStringInput("Enter Department Name: ");
        DeptNode* dept = findDepartment(school, deptName);
        if (dept == nullptr) {
            cout << " Department not found!" << endl;
            return;
        }

        if (dept->sectionHead == nullptr) {
            cout << " No sections in this department!" << endl;
            return;
        }

        // Display sections
        cout << "\nSections in " << deptName << ":" << endl;
        SectionNode* sec = dept->sectionHead;
        while (sec != nullptr) {
            cout << "  - " << sec->sectionName << " (Students: " << sec->studentCount << ")" << endl;
            sec = sec->next;
        }

        string secName = getStringInput("\nEnter Section Name to delete: ");
        sec = findSection(dept, secName);

        if (sec == nullptr) {
            cout << " Section not found!" << endl;
            return;
        }

        if (sec->studentCount > 0) {
            cout << "Warning: This section has " << sec->studentCount << " students!" << endl;
            string confirm = getStringInput("Type 'YES' to confirm deletion: ");
            if (confirm != "YES") {
                cout << "Deletion cancelled." << endl;
                return;
            }
        }

        // Remove from linked list
        if (stringsEqualIgnoreCase(dept->sectionHead->sectionName, secName)) {
            SectionNode* temp = dept->sectionHead;
            dept->sectionHead = dept->sectionHead->next;
            deleteAllStudents(temp);
            delete temp;
        }
        else {
            SectionNode* current = dept->sectionHead;
            while (current->next != nullptr) {
                if (stringsEqualIgnoreCase(current->next->sectionName, secName)) {
                    SectionNode* temp = current->next;
                    current->next = current->next->next;
                    deleteAllStudents(temp);
                    delete temp;
                    break;
                }
                current = current->next;
            }
        }

        dept->sectionCount--;
        cout << "Section deleted successfully!" << endl;
    }

    void updateSection() {
        cout << "\n=== Update Section ===" << endl;

        SchoolNode* school = selectSchool();
        if (school == nullptr)
            return;

        string deptName = getStringInput("Enter Department Name: ");
        DeptNode* dept = findDepartment(school, deptName);
        if (dept == nullptr) {
            cout << " Department not found!" << endl;
            return;
        }

        string secName = getStringInput("Enter Section Name to update: ");
        SectionNode* sec = findSection(dept, secName);

        if (sec == nullptr) {
            cout << " Section not found!" << endl;
            return;
        }

        string newName = getStringInput("Enter new section name (or press Enter to keep current): ");
        if (!newName.empty()) {
            if (findSection(dept, newName) != nullptr && !stringsEqualIgnoreCase(newName, secName)) {
                cout << " A section with name '" << newName << "' already exists!" << endl;
                return;
            }
            sec->sectionName = newName;
            cout << "Section renamed to '" << newName << "'!" << endl;
        }
        else {
            cout << "No changes made." << endl;
        }
    }

    // Teachers CRUD

    void addTeacher() {
        cout << "\n=== Add Teacher ===" << endl;

        SchoolNode* school = selectSchool();
        if (school == nullptr)
            return;

        string deptName = getStringInput("Enter Department Name: ");
        DeptNode* dept = findDepartment(school, deptName);
        if (dept == nullptr) {
            cout << " Department '" << deptName << "' not found!" << endl;
            return;
        }

        string teacherName = getStringInput("Enter Teacher Name: ");
        if (teacherName.empty()) {
            cout << " Teacher name cannot be empty." << endl;
            return;
        }

        int empID = getIntInput("Enter Employee ID: ");
        if (isTeacherIDTaken(school, empID)) {
            cout << " Employee ID " << empID << " is already taken in this school!" << endl;
            return;
        }

        string designation = getStringInput("Enter Designation (e.g., Professor, Lecturer): ");
        string subject = getStringInput("Enter Subject (optional): ");

        TeacherNode* newTeacher = new TeacherNode(teacherName, empID, designation, subject);
        newTeacher->next = dept->teacherHead;
        dept->teacherHead = newTeacher;
        dept->teacherCount++;

        cout << "Teacher '" << teacherName << "' added to department '" << deptName << "'!" << endl;
    }

    void removeTeacher() {
        cout << "\n=== Remove Teacher ===" << endl;

        SchoolNode* school = selectSchool();
        if (school == nullptr)
            return;

        string deptName = getStringInput("Enter Department Name: ");
        DeptNode* dept = findDepartment(school, deptName);
        if (dept == nullptr) {
            cout << " Department not found!" << endl;
            return;
        }

        if (dept->teacherHead == nullptr) {
            cout << " No teachers in this department!" << endl;
            return;
        }

        // Display teachers
        cout << "\nTeachers in " << deptName << ":" << endl;
        TeacherNode* teacher = dept->teacherHead;
        while (teacher != nullptr) {
            cout << "  - " << teacher->name << " (ID: " << teacher->empID
                << ") - " << teacher->designation << endl;
            teacher = teacher->next;
        }

        int empID = getIntInput("\nEnter Employee ID of teacher to remove: ");
        teacher = findTeacher(dept, empID);

        if (teacher == nullptr) {
            cout << " Teacher with ID " << empID << " not found!" << endl;
            return;
        }

        // Remove from linked list
        if (dept->teacherHead->empID == empID) {
            TeacherNode* temp = dept->teacherHead;
            dept->teacherHead = dept->teacherHead->next;
            cout << "Teacher '" << temp->name << "' removed successfully!" << endl;
            delete temp;
        }
        else {
            TeacherNode* current = dept->teacherHead;
            while (current->next != nullptr) {
                if (current->next->empID == empID) {
                    TeacherNode* temp = current->next;
                    current->next = current->next->next;
                    cout << "Teacher '" << temp->name << "' removed successfully!" << endl;
                    delete temp;
                    break;
                }
                current = current->next;
            }
        }

        dept->teacherCount--;
    }

    void updateTeacher() {
        cout << "\n=== Update Teacher ===" << endl;

        SchoolNode* school = selectSchool();
        if (school == nullptr)
            return;

        string deptName = getStringInput("Enter Department Name: ");
        DeptNode* dept = findDepartment(school, deptName);
        if (dept == nullptr) {
            cout << " Department not found!" << endl;
            return;
        }

        int empID = getIntInput("Enter Teacher Employee ID: ");
        TeacherNode* teacher = findTeacher(dept, empID);

        if (teacher == nullptr) {
            cout << " Teacher not found!" << endl;
            return;
        }

        cout << "\nCurrent Details:" << endl;
        cout << "  Name: " << teacher->name << endl;
        cout << "  Designation: " << teacher->designation << endl;
        cout << "  Subject: " << (teacher->subject.empty() ? "Not assigned" : teacher->subject) << endl;

        cout << "\nWhat would you like to update?" << endl;
        cout << "1. Name" << endl;
        cout << "2. Designation" << endl;
        cout << "3. Subject" << endl;
        cout << "4. All of the above" << endl;
        cout << "0. Cancel" << endl;

        int choice = getIntInput("Enter choice: ");

        if (choice == 0) {
            cout << "Update cancelled." << endl;
            return;
        }

        if (choice == 1 || choice == 4) {
            string newName = getStringInput("Enter new name (or press Enter to keep current): ");
            if (!newName.empty()) {
                teacher->name = newName;
            }
        }

        if (choice == 2 || choice == 4) {
            string newDesignation = getStringInput("Enter new designation (or press Enter to keep current): ");
            if (!newDesignation.empty()) {
                teacher->designation = newDesignation;
            }
        }

        if (choice == 3 || choice == 4) {
            string newSubject = getStringInput("Enter new subject (or press Enter to keep current): ");
            if (!newSubject.empty()) {
                teacher->subject = newSubject;
            }
        }

        cout << "Teacher updated successfully!" << endl;
    }

    // students CRUD

    void registerStudent() {
        cout << "\n=== Register Student ===" << endl;

        SchoolNode* school = selectSchool();
        if (school == nullptr)
            return;

        string deptName = getStringInput("Enter Department Name: ");
        DeptNode* dept = findDepartment(school, deptName);
        if (dept == nullptr) {
            cout << " Department '" << deptName << "' not found!" << endl;
            return;
        }

        string secName = getStringInput("Enter Section Name: ");
        SectionNode* sec = findSection(dept, secName);
        if (sec == nullptr) {
            cout << " Section '" << secName << "' not found!" << endl;
            return;
        }

        string studentName = getStringInput("Enter Student Name: ");
        if (studentName.empty()) {
            cout << " Student name cannot be empty." << endl;
            return;
        }

        int rollNumber = getIntInput("Enter Roll Number: ");
        if (isRollTaken(school, rollNumber)) {
            cout << " Roll number " << rollNumber << " is already taken in this school!" << endl;
            return;
        }

        string contact = getStringInput("Enter Contact Number (optional): ");

        StudentNode* newStudent = new StudentNode(studentName, rollNumber, contact);
        newStudent->next = sec->studentHead;
        sec->studentHead = newStudent;
        sec->studentCount++;

        cout << "Student '" << studentName << "' registered in section '" << secName << "'!" << endl;
    }

    void removeStudent() {
        cout << "\n=== Remove Student ===" << endl;

        SchoolNode* school = selectSchool();
        if (school == nullptr)
            return;

        string deptName = getStringInput("Enter Department Name: ");
        DeptNode* dept = findDepartment(school, deptName);
        if (dept == nullptr) {
            cout << " Department not found!" << endl;
            return;
        }

        string secName = getStringInput("Enter Section Name: ");
        SectionNode* sec = findSection(dept, secName);
        if (sec == nullptr) {
            cout << " Section not found!" << endl;
            return;
        }

        if (sec->studentHead == nullptr) {
            cout << " No students in this section!" << endl;
            return;
        }

        // Display students
        cout << "\nStudents in " << secName << ":" << endl;
        StudentNode* stu = sec->studentHead;
        while (stu != nullptr) {
            cout << "  - " << stu->name << " (Roll: " << stu->rollNumber << ")" << endl;
            stu = stu->next;
        }

        int rollNumber = getIntInput("\nEnter Roll Number of student to remove: ");
        stu = findStudent(sec, rollNumber);

        if (stu == nullptr) {
            cout << " Student with roll number " << rollNumber << " not found!" << endl;
            return;
        }

        // Remove from linked list
        if (sec->studentHead->rollNumber == rollNumber) {
            StudentNode* temp = sec->studentHead;
            sec->studentHead = sec->studentHead->next;
            cout << "Student '" << temp->name << "' removed successfully!" << endl;
            delete temp;
        }
        else {
            StudentNode* current = sec->studentHead;
            while (current->next != nullptr) {
                if (current->next->rollNumber == rollNumber) {
                    StudentNode* temp = current->next;
                    current->next = current->next->next;
                    cout << "Student '" << temp->name << "' removed successfully!" << endl;
                    delete temp;
                    break;
                }
                current = current->next;
            }
        }

        sec->studentCount--;
    }

    void updateStudent() {
        cout << "\n=== Update Student ===" << endl;

        SchoolNode* school = selectSchool();
        if (school == nullptr)
            return;

        string deptName = getStringInput("Enter Department Name: ");
        DeptNode* dept = findDepartment(school, deptName);
        if (dept == nullptr) {
            cout << " Department not found!" << endl;
            return;
        }

        string secName = getStringInput("Enter Section Name: ");
        SectionNode* sec = findSection(dept, secName);
        if (sec == nullptr) {
            cout << " Section not found!" << endl;
            return;
        }

        int rollNumber = getIntInput("Enter Student Roll Number: ");
        StudentNode* student = findStudent(sec, rollNumber);

        if (student == nullptr) {
            cout << " Student not found!" << endl;
            return;
        }

        cout << "\nCurrent Details:" << endl;
        cout << "  Name: " << student->name << endl;
        cout << "  Roll Number: " << student->rollNumber << endl;
        cout << "  Contact: " << (student->contactNumber.empty() ? "Not provided" : student->contactNumber) << endl;

        cout << "\nWhat would you like to update?" << endl;
        cout << "1. Name" << endl;
        cout << "2.  Contact Number" << endl;
        cout << "3. Both" << endl;
        cout << "0. Cancel" << endl;

        int choice = getIntInput("Enter choice: ");

        if (choice == 0) {
            cout << "Update cancelled." << endl;
            return;
        }

        if (choice == 1 || choice == 3) {
            string newName = getStringInput("Enter new name (or press Enter to keep current): ");
            if (!newName.empty()) {
                student->name = newName;
            }
        }

        if (choice == 2 || choice == 3) {
            string newContact = getStringInput("Enter new contact number (or press Enter to keep current): ");
            if (!newContact.empty()) {
                student->contactNumber = newContact;
            }
        }

        cout << "Student updated successfully!" << endl;
    }

    void transferStudent() {
        cout << "\n=== Transfer Student to Another Section ===" << endl;

        SchoolNode* school = selectSchool();
        if (school == nullptr)
            return;

        // Source section
        cout << "\n--- Source Section ---" << endl;
        string srcDeptName = getStringInput("Enter Source Department Name: ");
        DeptNode* srcDept = findDepartment(school, srcDeptName);
        if (srcDept == nullptr) {
            cout << " Source department not found!" << endl;
            return;
        }

        string srcSecName = getStringInput("Enter Source Section Name: ");
        SectionNode* srcSec = findSection(srcDept, srcSecName);
        if (srcSec == nullptr) {
            cout << " Source section not found!" << endl;
            return;
        }

        int rollNumber = getIntInput("Enter Student Roll Number: ");
        StudentNode* student = findStudent(srcSec, rollNumber);

        if (student == nullptr) {
            cout << " Student not found in source section!" << endl;
            return;
        }

        // Destination section
        cout << "\n--- Destination Section ---" << endl;
        string destDeptName = getStringInput("Enter Destination Department Name: ");
        DeptNode* destDept = findDepartment(school, destDeptName);
        if (destDept == nullptr) {
            cout << " Destination department not found!" << endl;
            return;
        }

        string destSecName = getStringInput("Enter Destination Section Name: ");
        SectionNode* destSec = findSection(destDept, destSecName);
        if (destSec == nullptr) {
            cout << " Destination section not found!" << endl;
            return;
        }

        if (srcSec == destSec) {
            cout << " Source and destination sections are the same!" << endl;
            return;
        }

        // Remove from source section
        if (srcSec->studentHead->rollNumber == rollNumber) {
            srcSec->studentHead = srcSec->studentHead->next;
        }
        else {
            StudentNode* current = srcSec->studentHead;
            while (current->next != nullptr && current->next->rollNumber != rollNumber) {
                current = current->next;
            }
            if (current->next != nullptr) {
                current->next = current->next->next;
            }
        }
        srcSec->studentCount--;

        // Add to destination section
        student->next = destSec->studentHead;
        destSec->studentHead = student;
        destSec->studentCount++;

        cout << "Student '" << student->name << "' transferred from "
            << srcSecName << " to " << destSecName << " successfully!" << endl;
    }


    void searchBySubject() {
        cout << "\n=== Search Schools by Subject ===" << endl;
        string subject = getStringInput("Enter Subject to search: ");
        subjectHash.search(subject);
    }

    void showRankings() {
        int topN = getIntInput("Enter number of top schools to show (0 for all): ");
        rankingHeap.displayRankings(topN);
    }

    void findNearestSchool() {
        cout << "\n=== Find Nearest School ===" << endl;

        if (schoolsHead == nullptr) {
            cout << "No schools registered." << endl;
            return;
        }

        double userLat = getDoubleInput("Enter your Latitude: ");
        double userLon = getDoubleInput("Enter your Longitude: ");

        SchoolNode* nearest = nullptr;
        double minDist = DBL_MAX;

        SchoolNode* current = schoolsHead;
        while (current != nullptr) {
            double dist = calculateDistance(userLat, userLon, current->lat, current->lon);
            if (dist < minDist) {
                minDist = dist;
                nearest = current;
            }
            current = current->next;
        }

        if (nearest != nullptr) {
            cout << "\n=== Nearest School ===" << endl;
            cout << "Name: " << nearest->name << endl;
            cout << "Sector: " << nearest->sector << endl;
            cout << "Rating: " << nearest->rating << "/5. 0" << endl;
            cout << "Subjects: " << nearest->subjects << endl;
            cout << "Distance: " << minDist << " units" << endl;
        }
    }

    void searchStudent() {
        cout << "\n=== Search Student ===" << endl;

        cout << "Search by:" << endl;
        cout << "1.  Roll Number (across all schools)" << endl;
        cout << "2. Name (across all schools)" << endl;

        int choice = getIntInput("Enter choice: ");

        if (choice == 1) {
            int rollNumber = getIntInput("Enter Roll Number: ");
            bool foundAny = false;

            SchoolNode* school = schoolsHead;
            while (school != nullptr) {
                DeptNode* dept = school->deptHead;
                while (dept != nullptr) {
                    SectionNode* sec = dept->sectionHead;
                    while (sec != nullptr) {
                        StudentNode* stu = findStudent(sec, rollNumber);
                        if (stu != nullptr) {
                            cout << "\n=== Student Found ===" << endl;
                            cout << "Name: " << stu->name << endl;
                            cout << "Roll Number: " << stu->rollNumber << endl;
                            cout << "Contact: " << (stu->contactNumber.empty() ? "Not provided" : stu->contactNumber) << endl;
                            cout << "School: " << school->name << endl;
                            cout << "Department: " << dept->deptName << endl;
                            cout << "Section: " << sec->sectionName << endl;
                            foundAny = true;
                        }
                        sec = sec->next;
                    }
                    dept = dept->next;
                }
                school = school->next;
            }


            if (!foundAny) {
                cout << "Student with roll number " << rollNumber << " not found." << endl;
            }
        }
        else if (choice == 2) {
            string studentName = getStringInput("Enter Student Name: ");
            bool found = false;

            cout << "\n=== Search Results ===" << endl;

            SchoolNode* school = schoolsHead;
            while (school != nullptr) {
                DeptNode* dept = school->deptHead;
                while (dept != nullptr) {
                    SectionNode* sec = dept->sectionHead;
                    while (sec != nullptr) {
                        StudentNode* stu = sec->studentHead;
                        while (stu != nullptr) {
                            if (toLowerStr(stu->name).find(toLowerStr(studentName)) != string::npos) {
                                cout << "- " << stu->name << " (Roll: " << stu->rollNumber << ")" << endl;
                                cout << "  School: " << school->name << ", Dept: " << dept->deptName
                                    << ", Section: " << sec->sectionName << endl;
                                found = true;
                            }
                            stu = stu->next;
                        }
                        sec = sec->next;
                    }
                    dept = dept->next;
                }
                school = school->next;
            }

            if (!found) {
                cout << "No students found matching '" << studentName << "'." << endl;
            }
        }
    }

    void searchTeacher() {
        cout << "\n=== Search Teacher ===" << endl;

        cout << "Search by:" << endl;
        cout << "1.  Employee ID (across all schools)" << endl;
        cout << "2. Name (across all schools)" << endl;

        int choice = getIntInput("Enter choice: ");

        if (choice == 1) {
            int empID = getIntInput("Enter Employee ID: ");
            bool foundAny = false;

            SchoolNode* school = schoolsHead;
            while (school != nullptr) {
                DeptNode* dept = school->deptHead;
                while (dept != nullptr) {
                    TeacherNode* teacher = findTeacher(dept, empID);
                    if (teacher != nullptr) {
                        cout << "\n=== Teacher Found ===" << endl;
                        cout << "Name: " << teacher->name << endl;
                        cout << "Employee ID: " << teacher->empID << endl;
                        cout << "Designation: " << teacher->designation << endl;
                        cout << "Subject: " << (teacher->subject.empty() ? "Not assigned" : teacher->subject) << endl;
                        cout << "School: " << school->name << endl;
                        cout << "Department: " << dept->deptName << endl;
                        foundAny = true;
                    }
                    dept = dept->next;
                }
                school = school->next;
            }

            if (!foundAny) {
                cout << "Teacher with ID " << empID << " not found." << endl;
            }
        }
        else if (choice == 2) {
            string teacherName = getStringInput("Enter Teacher Name: ");
            bool found = false;

            cout << "\n=== Search Results ===" << endl;

            SchoolNode* school = schoolsHead;
            while (school != nullptr) {
                DeptNode* dept = school->deptHead;
                while (dept != nullptr) {
                    TeacherNode* teacher = dept->teacherHead;
                    while (teacher != nullptr) {
                        if (toLowerStr(teacher->name).find(toLowerStr(teacherName)) != string::npos) {
                            cout << "- " << teacher->name << " (ID: " << teacher->empID << ")" << endl;
                            cout << "  School: " << school->name << ", Dept: " << dept->deptName
                                << ", Designation: " << teacher->designation << endl;
                            found = true;
                        }
                        teacher = teacher->next;
                    }
                    dept = dept->next;
                }
                school = school->next;
            }

            if (!found) {
                cout << "No teachers found matching '" << teacherName << "'." << endl;
            }
        }
    }

    void viewOrganogram() {
        cout << "\n=== View School Organogram ===" << endl;

        SchoolNode* school = selectSchool();
        if (school == nullptr)
            return;

        cout << "\n============================================" << endl;
        cout << " ORGANOGRAM: " << school->name << " (" << school->sector << ")" << endl;
        cout << " ID: " << school->schoolID << " | Rating: " << school->rating << "/5. 0" << endl;
        cout << " Subjects: " << school->subjects << endl;
        cout << "============================================" << endl;

        DeptNode* dept = school->deptHead;
        if (dept == nullptr) {
            cout << "  [No Departments]" << endl;
        }

        while (dept != nullptr) {
            cout << "  |" << endl;
            cout << "  +-- [DEPARTMENT] " << dept->deptName << endl;

            // Display Teachers
            cout << "  |    |" << endl;
            cout << "  |    +-- Faculty (" << dept->teacherCount << " teachers):" << endl;
            TeacherNode* teacher = dept->teacherHead;
            if (teacher == nullptr) {
                cout << "  |    |    (No teachers)" << endl;
            }
            while (teacher != nullptr) {
                cout << "  |    |    * " << teacher->name << " (" << teacher->designation << ")";
                if (!teacher->subject.empty()) {
                    cout << " [" << teacher->subject << "]";
                }
                cout << " ID: " << teacher->empID << endl;
                teacher = teacher->next;
            }

            // Display Sections and Students
            SectionNode* sec = dept->sectionHead;
            while (sec != nullptr) {
                cout << "  |    |" << endl;
                cout << "  |    +-- [SECTION] " << sec->sectionName
                    << " (" << sec->studentCount << " students)" << endl;

                StudentNode* stu = sec->studentHead;
                if (stu == nullptr) {
                    cout << "  |    |    (No students)" << endl;
                }
                while (stu != nullptr) {
                    cout << "  |    |    - " << stu->name << " (Roll: " << stu->rollNumber << ")";
                    if (!stu->contactNumber.empty()) {
                        cout << " [" << stu->contactNumber << "]";
                    }
                    cout << endl;
                    stu = stu->next;
                }
                sec = sec->next;
            }

            dept = dept->next;
        }
    }

    void displayAllSchools() {
        cout << "\n=== All Schools ===" << endl;
        SchoolNode* current = schoolsHead;
        int count = 0;
        while (current != nullptr) {
            cout << ++count << ".  [" << current->schoolID << "] " << current->name
                << " (" << current->sector << ") - Rating: " << current->rating << "/5.0" << endl;
            cout << "   Departments: " << current->deptCount
                << " | Teachers: " << getTotalTeachers(current)
                << " | Students: " << getTotalStudents(current) << endl;
            current = current->next;
        }
        if (count == 0) {
            cout << "No schools registered." << endl;
        }
    }

    void displaySchoolDetails() {
        cout << "\n=== School Details ===" << endl;

        SchoolNode* school = selectSchool();
        if (school == nullptr)
            return;

        cout << "\n========================================" << endl;
        cout << "School: " << school->name << endl;
        cout << "ID: " << school->schoolID << endl;
        cout << "Sector: " << school->sector << endl;
        cout << "Location: (" << school->lat << ", " << school->lon << ")" << endl;
        cout << "Rating: " << school->rating << "/5.0" << endl;
        cout << "Subjects: " << school->subjects << endl;
        cout << "========================================" << endl;
        cout << "Departments: " << school->deptCount << endl;
        cout << "Total Teachers: " << getTotalTeachers(school) << endl;
        cout << "Total Students: " << getTotalStudents(school) << endl;
        cout << "========================================" << endl;

        DeptNode* dept = school->deptHead;
        while (dept != nullptr) {
            int deptStudents = 0;
            SectionNode* sec = dept->sectionHead;
            while (sec != nullptr) {
                deptStudents += sec->studentCount;
                sec = sec->next;
            }

            cout << "  [" << dept->deptName << "] - Sections: " << dept->sectionCount
                << ", Teachers: " << dept->teacherCount << ", Students: " << deptStudents << endl;
            dept = dept->next;
        }
    }

    void displayStatistics() {
        cout << "\n=== Education System Statistics ===" << endl;
        cout << "Total Schools: " << schoolCount << endl;

        int totalDepts = 0;
        int totalTeachers = 0;
        int totalStudents = 0;
        int totalSections = 0;

        SchoolNode* school = schoolsHead;
        while (school != nullptr) {
            totalDepts += school->deptCount;
            DeptNode* dept = school->deptHead;
            while (dept != nullptr) {
                totalTeachers += dept->teacherCount;
                totalSections += dept->sectionCount;
                SectionNode* sec = dept->sectionHead;
                while (sec != nullptr) {
                    totalStudents += sec->studentCount;
                    sec = sec->next;
                }
                dept = dept->next;
            }
            school = school->next;
        }

        cout << "Total Departments: " << totalDepts << endl;
        cout << "Total Sections: " << totalSections << endl;
        cout << "Total Teachers: " << totalTeachers << endl;
        cout << "Total Students: " << totalStudents << endl;

        if (schoolCount > 0) {
            cout << "Average Departments per School: " << (double)totalDepts / schoolCount << endl;
            cout << "Average Students per School: " << (double)totalStudents / schoolCount << endl;
        }
    }

    void displayAllSubjects() {
        subjectHash.displayAllSubjects();
    }


    void loadFromFile() {
        ifstream file("schools.csv");
        if (!file.is_open()) {
            cout << "Warning: 'schools.csv' not found. Starting with empty database." << endl;
            return;
        }

        string line;
        getline(file, line);

        GlobalLocationManager* locMgr = GlobalLocationManager::getInstance();
        int loadedCount = 0;

        while (getline(file, line)) {
            if (line.empty())
                continue;

            // Normalize separators
            for (size_t i = 0; i < line.length(); i++) {
                if (line[i] == '\t') line[i] = ',';
            }

            string parts[6];
            int count = splitString(line, ',', parts, 5);

            if (count >= 5) {
                string id = trimString(parts[0]);
                string name = trimString(parts[1]);
                string sector = trimString(parts[2]);
                double rating = stringToDouble(trimString(parts[3]));

                // Remaining part is subjects 
                string subjects = "";
                size_t pos = 0;
                int commaCount = 0;
                for (size_t i = 0; i < line.length(); i++) {
                    if (line[i] == ',') {
                        commaCount++;
                        if (commaCount == 4) {
                            subjects = line.substr(i + 1);
                            break;
                        }
                    }
                }
                subjects = trimString(subjects);

                // Remove surrounding quotes from subjects
                if (!subjects.empty() && subjects[0] == '"') {
                    subjects = subjects.substr(1);
                }
                if (!subjects.empty() && subjects[subjects.length() - 1] == '"') {
                    subjects = subjects.substr(0, subjects.length() - 1);
                }

                if (findSchoolByID(id) != nullptr || findSchoolByName(name) != nullptr) {
                    continue;

                }

                // Generate coordinates based on sector
                double lat, lon;
                double schoolOffset = 0.004;

                if (sector == "G-10") {
                    lat = 33.684 + schoolOffset; lon = 73.025 + schoolOffset;
                }
                else if (sector == "F-8") {
                    lat = 33.709 + schoolOffset; lon = 73.037 + schoolOffset;
                }
                else if (sector == "F-6") {
                    lat = 33.727 + schoolOffset; lon = 73.075 + schoolOffset;
                }
                else if (sector == "G-9") {
                    lat = 33.700 + schoolOffset; lon = 73.030 + schoolOffset;
                }
                else if (sector == "F-7") {
                    lat = 33.721 + schoolOffset; lon = 73.052 + schoolOffset;
                }
                else if (sector == "H-8") {
                    lat = 33.690 + schoolOffset; lon = 73.040 + schoolOffset;
                }
                else if (sector == "I-8") {
                    lat = 33.680 + schoolOffset; lon = 73.050 + schoolOffset;
                }
                else {
                    lat = 33.700 + (loadedCount * 0.002) + schoolOffset;
                    lon = 73.000 + (loadedCount * 0.002) + schoolOffset;
                }

                if (!locMgr->registerLocation(lat, lon, "School", name)) {
                    // Adjust coordinates slightly if occupied
                    lat += 0.001;
                    lon += 0.001;
                    if (!locMgr->registerLocation(lat, lon, "School", name)) {
                        continue;
                    }
                }

                SchoolNode* newSchool = new SchoolNode(id, name, sector, lat, lon, rating, subjects);
                newSchool->next = schoolsHead;
                schoolsHead = newSchool;
                schoolCount++;

                rankingHeap.insert(newSchool);
                processSubjects(subjects, newSchool);

                // Register with CityGraph for unified city-wide graph
                CityGraph::getInstance()->addLocation(id, name, LOC_SCHOOL, lat, lon, newSchool);

                loadedCount++;
            }
        }

        file.close();
        cout << "Loaded " << loadedCount << " schools from file." << endl;
    }



    int getSchoolCount() {
        return schoolCount;
    }

    SchoolNode* getSchoolsHead() {
        return schoolsHead;
    }


    ~EducationSystem() {
        while (schoolsHead != nullptr) {
            SchoolNode* schoolTemp = schoolsHead;
            schoolsHead = schoolsHead->next;

            // Delete all departments
            while (schoolTemp->deptHead != nullptr) {
                DeptNode* deptTemp = schoolTemp->deptHead;
                schoolTemp->deptHead = schoolTemp->deptHead->next;

                // Delete all teachers in department
                while (deptTemp->teacherHead != nullptr) {
                    TeacherNode* teacherTemp = deptTemp->teacherHead;
                    deptTemp->teacherHead = deptTemp->teacherHead->next;
                    delete teacherTemp;
                }

                // Delete all sections in department
                while (deptTemp->sectionHead != nullptr) {
                    SectionNode* secTemp = deptTemp->sectionHead;
                    deptTemp->sectionHead = deptTemp->sectionHead->next;

                    // Delete all students in section
                    while (secTemp->studentHead != nullptr) {
                        StudentNode* stuTemp = secTemp->studentHead;
                        secTemp->studentHead = secTemp->studentHead->next;
                        delete stuTemp;
                    }
                    delete secTemp;
                }
                delete deptTemp;
            }
            delete schoolTemp;
        }
    }
};
