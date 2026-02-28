#pragma once

#include "Utils.h"
#include "GlobalLocationManager.h"
#include "CityGraph.h"

struct Citizen {
    string cnic;
    string name;
    int age;
    string gender;
    string sector;
    string streetNumber;
    string houseNumber;
    string occupation;
    string phoneNumber;

    Citizen() : age(0) {}

    Citizen(const string& c, const string& n, int a, const string& g,
        const string& sec, const string& street, const string& house,
        const string& occ, const string& phone = "")
        : cnic(c), name(n), age(a), gender(g), sector(sec),
        streetNumber(street), houseNumber(house), occupation(occ), phoneNumber(phone) {
    }

    bool isEmpty() const {
        return cnic.empty();
    }

    void display() const {
        cout << "  CNIC: " << cnic << endl;
        cout << "  Name: " << name << endl;
        cout << "  Age: " << age << " | Gender: " << gender << endl;
        cout << "  Address: House " << houseNumber << ", Street " << streetNumber
            << ", " << sector << endl;
        cout << "  Occupation: " << occupation << endl;
        if (!phoneNumber.empty()) {
            cout << "  Phone: " << phoneNumber << endl;
        }
    }
};

// CNIC HASH TABLE FOR O(1) LOOKUP
struct CNICHashEntry {
    string cnic;
    Citizen* citizenPtr;
    string sector;
    string street;
    string house;
    CNICHashEntry* next;

    CNICHashEntry(const string& c, Citizen* ptr, const string& sec,
        const string& str, const string& h)
        : cnic(c), citizenPtr(ptr), sector(sec), street(str), house(h), next(nullptr) {
    }
};

class CNICHashTable {
private:
    static const int TABLE_SIZE = 500;
    CNICHashEntry* table[TABLE_SIZE];
    int count;

    int hashFunc(const string& cnic) {
        return polynomialHash(cnic, TABLE_SIZE);
    }

public:
    CNICHashTable() : count(0) {
        for (int i = 0; i < TABLE_SIZE; i++) {
            table[i] = nullptr;
        }
    }

    void insert(const string& cnic, Citizen* citizen, const string& sector,
        const string& street, const string& house) {
        int idx = hashFunc(cnic);

        CNICHashEntry* current = table[idx];
        while (current != nullptr) {
            if (current->cnic == cnic) {
                current->citizenPtr = citizen;
                current->sector = sector;
                current->street = street;
                current->house = house;
                return;
            }
            current = current->next;
        }

        CNICHashEntry* newEntry = new CNICHashEntry(cnic, citizen, sector, street, house);
        newEntry->next = table[idx];
        table[idx] = newEntry;
        count++;
    }

    CNICHashEntry* search(const string& cnic) {
        int idx = hashFunc(cnic);
        CNICHashEntry* current = table[idx];
        while (current != nullptr) {
            if (current->cnic == cnic)
                return current;
            current = current->next;
        }
        return nullptr;
    }

    bool remove(const string& cnic) {
        int idx = hashFunc(cnic);

        if (table[idx] == nullptr)
            return false;

        if (table[idx]->cnic == cnic) {
            CNICHashEntry* temp = table[idx];
            table[idx] = table[idx]->next;
            delete temp;
            count--;
            return true;
        }

        CNICHashEntry* current = table[idx];
        while (current->next != nullptr) {
            if (current->next->cnic == cnic) {
                CNICHashEntry* temp = current->next;
                current->next = current->next->next;
                delete temp;
                count--;
                return true;
            }
            current = current->next;
        }
        return false;
    }

    bool exists(const string& cnic) {
        return search(cnic) != nullptr;
    }

    int getCount() {
        return count;
    }

    void getCitizensBySector(const string& sector, Citizen** results, int& resultCount, int maxResults) {
        resultCount = 0;
        for (int i = 0; i < TABLE_SIZE && resultCount < maxResults; i++) {
            CNICHashEntry* current = table[i];
            while (current != nullptr && resultCount < maxResults) {
                if (stringsEqualIgnoreCase(current->sector, sector) && current->citizenPtr != nullptr) {
                    results[resultCount++] = current->citizenPtr;
                }
                current = current->next;
            }
        }
    }

    void getCitizensByStreet(const string& sector, const string& street,
        Citizen** results, int& resultCount, int maxResults) {
        resultCount = 0;
        for (int i = 0; i < TABLE_SIZE && resultCount < maxResults; i++) {
            CNICHashEntry* current = table[i];
            while (current != nullptr && resultCount < maxResults) {
                if (stringsEqualIgnoreCase(current->sector, sector) &&
                    stringsEqualIgnoreCase(current->street, street) &&
                    current->citizenPtr != nullptr) {
                    results[resultCount++] = current->citizenPtr;
                }
                current = current->next;
            }
        }
    }

    void getCitizensByHouse(const string& sector, const string& street, const string& house,
        Citizen** results, int& resultCount, int maxResults) {
        resultCount = 0;
        for (int i = 0; i < TABLE_SIZE && resultCount < maxResults; i++) {
            CNICHashEntry* current = table[i];
            while (current != nullptr && resultCount < maxResults) {
                if (stringsEqualIgnoreCase(current->sector, sector) &&
                    stringsEqualIgnoreCase(current->street, street) &&
                    stringsEqualIgnoreCase(current->house, house) &&
                    current->citizenPtr != nullptr) {
                    results[resultCount++] = current->citizenPtr;
                }
                current = current->next;
            }
        }
    }

    ~CNICHashTable() {
        for (int i = 0; i < TABLE_SIZE; i++) {
            while (table[i] != nullptr) {
                CNICHashEntry* temp = table[i];
                table[i] = table[i]->next;
                delete temp;
            }
        }
    }
};

// N-ARY HIERARCHY TREE STRUCTURE

const int NODE_SECTOR = 0;
const int NODE_STREET = 1;
const int NODE_HOUSE = 2;
const int NODE_FAMILY = 3;

struct FamilyMember {
    Citizen* citizen;
    FamilyMember* next;

    FamilyMember(Citizen* c) : citizen(c), next(nullptr) {}
};

struct HierarchyNode {
    string id;
    string name;
    int type;
    HierarchyNode* firstChild;
    HierarchyNode* nextSibling;
    FamilyMember* familyMembers;

    HierarchyNode(const string& i, const string& n, int t)
        : id(i), name(n), type(t), firstChild(nullptr), nextSibling(nullptr), familyMembers(nullptr) {
    }
};

class HierarchyTree {
private:
    HierarchyNode* root;

    HierarchyNode* findOrCreateSector(const string& sectorID, const string& sectorName) {
        if (root == nullptr) {
            root = new HierarchyNode(sectorID, sectorName, NODE_SECTOR);
            return root;
        }

        HierarchyNode* current = root;
        while (current != nullptr) {
            if (current->type == NODE_SECTOR && stringsEqualIgnoreCase(current->id, sectorID)) {
                return current;
            }
            current = current->nextSibling;
        }

        HierarchyNode* newSector = new HierarchyNode(sectorID, sectorName, NODE_SECTOR);
        newSector->nextSibling = root;
        root = newSector;
        return newSector;
    }

    HierarchyNode* findOrCreateStreet(HierarchyNode* sector, const string& streetID, const string& streetName) {
        HierarchyNode* current = sector->firstChild;
        while (current != nullptr) {
            if (current->type == NODE_STREET && stringsEqualIgnoreCase(current->id, streetID)) {
                return current;
            }
            current = current->nextSibling;
        }

        HierarchyNode* newStreet = new HierarchyNode(streetID, streetName, NODE_STREET);
        newStreet->nextSibling = sector->firstChild;
        sector->firstChild = newStreet;
        return newStreet;
    }

    HierarchyNode* findOrCreateHouse(HierarchyNode* street, const string& houseID, const string& houseName) {
        HierarchyNode* current = street->firstChild;
        while (current != nullptr) {
            if (current->type == NODE_HOUSE && stringsEqualIgnoreCase(current->id, houseID)) {
                return current;
            }
            current = current->nextSibling;
        }

        HierarchyNode* newHouse = new HierarchyNode(houseID, houseName, NODE_HOUSE);
        newHouse->nextSibling = street->firstChild;
        street->firstChild = newHouse;
        return newHouse;
    }

    HierarchyNode* findOrCreateFamily(HierarchyNode* house, const string& familyID) {
        HierarchyNode* current = house->firstChild;
        while (current != nullptr) {
            if (current->type == NODE_FAMILY && stringsEqualIgnoreCase(current->id, familyID)) {
                return current;
            }
            current = current->nextSibling;
        }

        HierarchyNode* newFamily = new HierarchyNode(familyID, "Family", NODE_FAMILY);
        newFamily->nextSibling = house->firstChild;
        house->firstChild = newFamily;
        return newFamily;
    }

    void addCitizenToFamily(HierarchyNode* family, Citizen* citizen) {
        FamilyMember* newMember = new FamilyMember(citizen);
        newMember->next = family->familyMembers;
        family->familyMembers = newMember;
    }

    void removeCitizenFromFamily(HierarchyNode* family, const string& cnic) {
        if (family->familyMembers == nullptr) return;

        if (stringsEqualIgnoreCase(family->familyMembers->citizen->cnic, cnic)) {
            FamilyMember* temp = family->familyMembers;
            family->familyMembers = family->familyMembers->next;
            delete temp;
            return;
        }

        FamilyMember* current = family->familyMembers;
        while (current->next != nullptr) {
            if (stringsEqualIgnoreCase(current->next->citizen->cnic, cnic)) {
                FamilyMember* temp = current->next;
                current->next = current->next->next;
                delete temp;
                return;
            }
            current = current->next;
        }
    }

    HierarchyNode* findFamilyWithCitizen(HierarchyNode* node, const string& cnic) {
        if (node == nullptr) return nullptr;

        if (node->type == NODE_FAMILY) {
            FamilyMember* member = node->familyMembers;
            while (member != nullptr) {
                if (stringsEqualIgnoreCase(member->citizen->cnic, cnic)) {
                    return node;
                }
                member = member->next;
            }
        }

        HierarchyNode* result = findFamilyWithCitizen(node->firstChild, cnic);
        if (result != nullptr) return result;

        return findFamilyWithCitizen(node->nextSibling, cnic);
    }

    void deleteTree(HierarchyNode* node) {
        if (node == nullptr) return;

        deleteTree(node->firstChild);
        deleteTree(node->nextSibling);

        while (node->familyMembers != nullptr) {
            FamilyMember* temp = node->familyMembers;
            node->familyMembers = node->familyMembers->next;
            delete temp;
        }

        delete node;
    }

    void displayTreeRecursive(HierarchyNode* node, int level, const string& prefix) {
        if (node == nullptr) return;

        for (int i = 0; i < level; i++) cout << "  ";
        cout << prefix;

        if (node->type == NODE_SECTOR) {
            cout << "Sector: " << node->name << " (" << node->id << ")" << endl;
        }
        else if (node->type == NODE_STREET) {
            cout << "Street: " << node->name << " (" << node->id << ")" << endl;
        }
        else if (node->type == NODE_HOUSE) {
            cout << "House: " << node->name << " (" << node->id << ")" << endl;
        }
        else if (node->type == NODE_FAMILY) {
            cout << "Family: " << node->id << endl;
            FamilyMember* member = node->familyMembers;
            int memberCount = 0;
            while (member != nullptr) {
                for (int i = 0; i <= level; i++) cout << "  ";
                cout << "  - " << member->citizen->name << " (CNIC: " << member->citizen->cnic << ")" << endl;
                member = member->next;
                memberCount++;
            }
            if (memberCount == 0) {
                for (int i = 0; i <= level; i++) cout << "  ";
                cout << "  (No members)" << endl;
            }
        }

        if (node->firstChild != nullptr) {
            displayTreeRecursive(node->firstChild, level + 1, " |- ");
        }
    }

public:
    HierarchyTree() : root(nullptr) {}

    ~HierarchyTree() {
        deleteTree(root);
    }

    void insertCitizen(Citizen* citizen) {
        if (citizen == nullptr) return;

        string sectorID = citizen->sector;
        string streetID = sectorID + "-ST" + citizen->streetNumber;
        string houseID = streetID + "-H" + citizen->houseNumber;
        string familyID = houseID + "-FAM";

        HierarchyNode* sector = findOrCreateSector(sectorID, citizen->sector);
        HierarchyNode* street = findOrCreateStreet(sector, streetID, "Street " + citizen->streetNumber);
        HierarchyNode* house = findOrCreateHouse(street, houseID, "House " + citizen->houseNumber);
        HierarchyNode* family = findOrCreateFamily(house, familyID);

        addCitizenToFamily(family, citizen);
    }

    void removeCitizen(const string& cnic) {
        if (root == nullptr)
            return;

        HierarchyNode* family = findFamilyWithCitizen(root, cnic);
        if (family != nullptr) {
            removeCitizenFromFamily(family, cnic);
        }
    }

    void updateCitizenLocation(Citizen* citizen, const string& oldSector, const string& oldStreet, const string& oldHouse) {
        HierarchyNode* oldFamily = findFamilyWithCitizen(root, citizen->cnic);
        if (oldFamily != nullptr) {
            removeCitizenFromFamily(oldFamily, citizen->cnic);
        }

        insertCitizen(citizen);
    }

    void displayTree() {
        if (root == nullptr) {
            cout << "\n=== Population Hierarchy ===" << endl;
            cout << "(No data)" << endl;
            return;
        }

        cout << "\n=== Population Hierarchy (Sector -> Street -> House -> Family) ===" << endl;
        HierarchyNode* current = root;
        while (current != nullptr) {
            displayTreeRecursive(current, 0, "");
            current = current->nextSibling;
            if (current != nullptr) cout << endl;
        }
    }

    int countNodes(HierarchyNode* node) {
        if (node == nullptr) return 0;
        int count = 1;
        count += countNodes(node->firstChild);
        count += countNodes(node->nextSibling);
        return count;
    }

    int getNodeCount() {
        return countNodes(root);
    }
};

// B-TREE CONSTANTS

const int B_TREE_ORDER = 6;
const int MAX_KEYS = B_TREE_ORDER - 1;       // 5
const int MIN_KEYS = (B_TREE_ORDER - 1) / 2; // 2

struct BTreeNode {
    Citizen keys[MAX_KEYS];
    BTreeNode* children[B_TREE_ORDER];
    int keyCount;
    bool isLeaf;

    BTreeNode(bool leaf = true) : keyCount(0), isLeaf(leaf) {
        for (int i = 0; i < B_TREE_ORDER; i++) {
            children[i] = nullptr;
        }
    }
};

// B-TREE CLASS 

class CitizenBTree {
private:
    BTreeNode* root;
    int nodeCount;
    int citizenCount;


    int compareCNIC(const string& cnic1, const string& cnic2) {
        if (cnic1 < cnic2)
            return -1;
        if (cnic1 > cnic2)
            return 1;
        return 0;
    }

    int findKeyIndex(BTreeNode* node, const string& cnic) {
        int idx = 0;
        while (idx < node->keyCount && compareCNIC(node->keys[idx].cnic, cnic) < 0) {
            idx++;
        }
        return idx;
    }

    // INSERTION FUNCTIONS

    void splitChildWithKey(BTreeNode* parent, int childIndex, const Citizen& newKey, bool& keyAlreadyInserted) {
        BTreeNode* fullChild = parent->children[childIndex];
        keyAlreadyInserted = true;


        Citizen allKeys[MAX_KEYS + 1];
        BTreeNode* allChildren[B_TREE_ORDER + 1];


        for (int i = 0; i <= B_TREE_ORDER; i++) {
            allChildren[i] = nullptr;
        }


        int newKeyPos = 0;
        while (newKeyPos < fullChild->keyCount &&
            compareCNIC(fullChild->keys[newKeyPos].cnic, newKey.cnic) < 0) {
            newKeyPos++;
        }


        for (int i = 0; i < newKeyPos; i++) {
            allKeys[i] = fullChild->keys[i];
        }
        allKeys[newKeyPos] = newKey;
        for (int i = newKeyPos; i < fullChild->keyCount; i++) {
            allKeys[i + 1] = fullChild->keys[i];
        }


        if (!fullChild->isLeaf) {
            for (int i = 0; i <= fullChild->keyCount; i++) {
                allChildren[i] = fullChild->children[i];
            }
        }


        int midIndex = (MAX_KEYS + 1) / 2;
        Citizen middleKey = allKeys[midIndex];


        BTreeNode* rightNode = new BTreeNode(fullChild->isLeaf);
        nodeCount++;


        fullChild->keyCount = midIndex;
        for (int i = 0; i < midIndex; i++) {
            fullChild->keys[i] = allKeys[i];
        }

        for (int i = midIndex; i < MAX_KEYS; i++) {
            fullChild->keys[i] = Citizen();
        }


        rightNode->keyCount = MAX_KEYS - midIndex;
        for (int i = 0; i < rightNode->keyCount; i++) {
            rightNode->keys[i] = allKeys[midIndex + 1 + i];
        }


        if (!fullChild->isLeaf) {

            for (int i = midIndex + 1; i < B_TREE_ORDER; i++) {
                fullChild->children[i] = nullptr;
            }


            for (int i = 0; i <= rightNode->keyCount; i++) {
                rightNode->children[i] = allChildren[midIndex + 1 + i];
            }
        }


        for (int i = parent->keyCount; i > childIndex; i--) {
            parent->keys[i] = parent->keys[i - 1];
            parent->children[i + 1] = parent->children[i];
        }


        parent->keys[childIndex] = middleKey;
        parent->children[childIndex + 1] = rightNode;
        parent->keyCount++;
    }

    void splitChild(BTreeNode* parent, int i) {
        BTreeNode* y = parent->children[i];
        BTreeNode* z = new BTreeNode(y->isLeaf);
        nodeCount++;

        z->keyCount = MIN_KEYS;

        // Copy the last MIN_KEYS keys of y to z
        for (int j = 0; j < MIN_KEYS; j++) {
            z->keys[j] = y->keys[j + MIN_KEYS + 1];
        }

        // If not leaf, copy the last MIN_KEYS + 1 children of y to z
        if (!y->isLeaf) {
            for (int j = 0; j <= MIN_KEYS; j++) {
                z->children[j] = y->children[j + MIN_KEYS + 1];
            }
        }

        y->keyCount = MIN_KEYS;

        // Shift children of parent to make room for z
        for (int j = parent->keyCount; j >= i + 1; j--) {
            parent->children[j + 1] = parent->children[j];
        }

        // Link z to parent
        parent->children[i + 1] = z;

        // Shift keys of parent to make room for median of y
        for (int j = parent->keyCount - 1; j >= i; j--) {
            parent->keys[j + 1] = parent->keys[j];
        }

        // Move median key from y to parent
        parent->keys[i] = y->keys[MIN_KEYS];
        parent->keyCount++;
    }
    void insertNonFull(BTreeNode* node, const Citizen& citizen) {
        int i = node->keyCount - 1;

        if (node->isLeaf) {
            // Leaf node: insert in sorted position
            while (i >= 0 && compareCNIC(node->keys[i].cnic, citizen.cnic) > 0) {
                node->keys[i + 1] = node->keys[i];
                i--;
            }
            node->keys[i + 1] = citizen;
            node->keyCount++;
        }
        else {
            // Find which child to descend into
            while (i >= 0 && compareCNIC(node->keys[i].cnic, citizen.cnic) > 0) {
                i--;
            }
            i++;

            // If child is full, split it FIRST
            if (node->children[i]->keyCount == MAX_KEYS) {
                splitChild(node, i);


                if (compareCNIC(node->keys[i].cnic, citizen.cnic) < 0) {
                    i++;
                }
            }

            insertNonFull(node->children[i], citizen);
        }
    }

    // SEARCH FUNCTION

    Citizen* searchNode(BTreeNode* node, const string& cnic) {
        if (node == nullptr)
            return nullptr;

        int i = 0;
        while (i < node->keyCount && compareCNIC(node->keys[i].cnic, cnic) < 0) {
            i++;
        }

        if (i < node->keyCount && compareCNIC(node->keys[i].cnic, cnic) == 0) {
            return &(node->keys[i]);
        }

        if (node->isLeaf) {
            return nullptr;
        }

        return searchNode(node->children[i], cnic);
    }


    Citizen getPredecessor(BTreeNode* node, int idx) {
        BTreeNode* current = node->children[idx];
        while (!current->isLeaf) {
            current = current->children[current->keyCount];
        }
        return current->keys[current->keyCount - 1];
    }


    Citizen getSuccessor(BTreeNode* node, int idx) {
        BTreeNode* current = node->children[idx + 1];
        while (!current->isLeaf) {
            current = current->children[0];
        }
        return current->keys[0];
    }


    void borrowFromLeft(BTreeNode* node, int idx) {
        BTreeNode* child = node->children[idx];
        BTreeNode* leftSibling = node->children[idx - 1];


        for (int i = child->keyCount - 1; i >= 0; i--) {
            child->keys[i + 1] = child->keys[i];
        }


        if (!child->isLeaf) {
            for (int i = child->keyCount; i >= 0; i--) {
                child->children[i + 1] = child->children[i];
            }

            child->children[0] = leftSibling->children[leftSibling->keyCount];
            leftSibling->children[leftSibling->keyCount] = nullptr;
        }


        child->keys[0] = node->keys[idx - 1];
        child->keyCount++;


        node->keys[idx - 1] = leftSibling->keys[leftSibling->keyCount - 1];
        leftSibling->keyCount--;
    }

    // Borrow a key from the right sibling
    void borrowFromRight(BTreeNode* node, int idx) {
        BTreeNode* child = node->children[idx];
        BTreeNode* rightSibling = node->children[idx + 1];


        child->keys[child->keyCount] = node->keys[idx];
        child->keyCount++;

        // If child is not a leaf, move first child of right sibling to child
        if (!child->isLeaf) {
            child->children[child->keyCount] = rightSibling->children[0];
        }


        node->keys[idx] = rightSibling->keys[0];


        for (int i = 1; i < rightSibling->keyCount; i++) {
            rightSibling->keys[i - 1] = rightSibling->keys[i];
        }


        if (!rightSibling->isLeaf) {
            for (int i = 1; i <= rightSibling->keyCount; i++) {
                rightSibling->children[i - 1] = rightSibling->children[i];
            }
            rightSibling->children[rightSibling->keyCount] = nullptr;
        }

        rightSibling->keyCount--;
    }


    void merge(BTreeNode* node, int idx) {
        BTreeNode* leftChild = node->children[idx];
        BTreeNode* rightChild = node->children[idx + 1];

        // Pull down key from parent to left child
        leftChild->keys[leftChild->keyCount] = node->keys[idx];
        leftChild->keyCount++;

        // Copy all keys from right child to left child
        for (int i = 0; i < rightChild->keyCount; i++) {
            leftChild->keys[leftChild->keyCount] = rightChild->keys[i];
            leftChild->keyCount++;
        }

        // If not leaf, copy all children from right child to left child
        if (!leftChild->isLeaf) {
            for (int i = 0; i <= rightChild->keyCount; i++) {
                leftChild->children[leftChild->keyCount - rightChild->keyCount + i] = rightChild->children[i];
            }
        }

        // Shift keys in parent to fill the gap
        for (int i = idx; i < node->keyCount - 1; i++) {
            node->keys[i] = node->keys[i + 1];
        }

        // Shift children in parent to fill the gap
        for (int i = idx + 1; i < node->keyCount; i++) {
            node->children[i] = node->children[i + 1];
        }
        node->children[node->keyCount] = nullptr;

        node->keyCount--;


        delete rightChild;
        nodeCount--;
    }


    void fill(BTreeNode* node, int idx) {
        // Try to borrow from left sibling
        if (idx > 0 && node->children[idx - 1] != nullptr &&
            node->children[idx - 1]->keyCount > MIN_KEYS) {
            borrowFromLeft(node, idx);
        }
        // Try to borrow from right sibling
        else if (idx < node->keyCount && node->children[idx + 1] != nullptr &&
            node->children[idx + 1]->keyCount > MIN_KEYS) {
            borrowFromRight(node, idx);
        }

        else {
            if (idx < node->keyCount) {
                // Merge with right sibling
                merge(node, idx);
            }
            else {
                // Merge with left sibling
                merge(node, idx - 1);
            }
        }
    }


    void removeFromLeaf(BTreeNode* node, int idx) {
        // Shift all keys after idx one position left
        for (int i = idx; i < node->keyCount - 1; i++) {
            node->keys[i] = node->keys[i + 1];
        }
        node->keyCount--;
    }

    // Remove from a non-leaf node
    void removeFromNonLeaf(BTreeNode* node, int idx) {
        Citizen key = node->keys[idx];


        if (node->children[idx]->keyCount > MIN_KEYS) {
            Citizen predecessor = getPredecessor(node, idx);
            node->keys[idx] = predecessor;
            removeKey(node->children[idx], predecessor.cnic);
        }

        else if (node->children[idx + 1]->keyCount > MIN_KEYS) {
            Citizen successor = getSuccessor(node, idx);
            node->keys[idx] = successor;
            removeKey(node->children[idx + 1], successor.cnic);
        }

        else {
            merge(node, idx);
            removeKey(node->children[idx], key.cnic);
        }
    }

    // Main recursive delete function
    void removeKey(BTreeNode* node, const string& cnic) {
        if (node == nullptr) return;

        int idx = findKeyIndex(node, cnic);

        // Case 1: Key is present in this node
        if (idx < node->keyCount && compareCNIC(node->keys[idx].cnic, cnic) == 0) {
            if (node->isLeaf) {
                removeFromLeaf(node, idx);
            }
            else {
                removeFromNonLeaf(node, idx);
            }
            return;
        }

        // Case 2: Key is not in this node
        if (node->isLeaf) {

            return;
        }


        bool isLastChild = (idx == node->keyCount);


        if (node->children[idx]->keyCount == MIN_KEYS) {
            fill(node, idx);
        }


        if (isLastChild && idx > node->keyCount) {
            removeKey(node->children[idx - 1], cnic);
        }
        else {
            removeKey(node->children[idx], cnic);
        }
    }

    // TRAVERSAL AND DISPLAY

    void inorderTraversal(BTreeNode* node, Citizen* results, int& count, int maxCount) {
        if (node == nullptr || count >= maxCount)
            return;

        for (int i = 0; i < node->keyCount && count < maxCount; i++) {
            if (!node->isLeaf) {
                inorderTraversal(node->children[i], results, count, maxCount);
            }
            if (count < maxCount) {
                results[count++] = node->keys[i];
            }
        }

        if (!node->isLeaf) {
            inorderTraversal(node->children[node->keyCount], results, count, maxCount);
        }
    }

    void displayNode(BTreeNode* node, int level) {
        if (node == nullptr)
            return;

        string indent = "";
        for (int i = 0; i < level; i++) indent += "    ";

        cout << indent << "[";
        for (int i = 0; i < node->keyCount; i++) {
            cout << node->keys[i].cnic;
            if (i < node->keyCount - 1) cout << ", ";
        }
        cout << "]" << (node->isLeaf ? " (Leaf)" : "") << endl;

        if (!node->isLeaf) {
            for (int i = 0; i <= node->keyCount; i++) {
                displayNode(node->children[i], level + 1);
            }
        }
    }

    void deleteTree(BTreeNode* node) {
        if (node == nullptr)
            return;

        if (!node->isLeaf) {
            for (int i = 0; i <= node->keyCount; i++) {
                deleteTree(node->children[i]);
            }
        }
        delete node;
    }

    int countNodes(BTreeNode* node) {
        if (node == nullptr)
            return 0;
        int count = 1;
        if (!node->isLeaf) {
            for (int i = 0; i <= node->keyCount; i++) {
                count += countNodes(node->children[i]);
            }
        }
        return count;
    }

    int getHeight(BTreeNode* node) {
        if (node == nullptr)
            return 0;
        if (node->isLeaf)
            return 1;
        return 1 + getHeight(node->children[0]);
    }

    // VALIDATION FUNCTION 

    bool validateNode(BTreeNode* node, bool isRoot, string& error) {
        if (node == nullptr)
            return true;

        // Check key count bounds
        if (!isRoot && node->keyCount < MIN_KEYS) {
            error = "Node has fewer than MIN_KEYS (" + intToString(node->keyCount) + " < " + intToString(MIN_KEYS) + ")";
            return false;
        }
        if (node->keyCount > MAX_KEYS) {
            error = "Node has more than MAX_KEYS (" + intToString(node->keyCount) + " > " + intToString(MAX_KEYS) + ")";
            return false;
        }

        // Check keys are sorted
        for (int i = 0; i < node->keyCount - 1; i++) {
            if (compareCNIC(node->keys[i].cnic, node->keys[i + 1].cnic) >= 0) {
                error = "Keys not in sorted order";
                return false;
            }
        }

        // Recursively validate children
        if (!node->isLeaf) {
            for (int i = 0; i <= node->keyCount; i++) {
                if (node->children[i] == nullptr) {
                    error = "Non-leaf node has null child";
                    return false;
                }
                if (!validateNode(node->children[i], false, error)) {
                    return false;
                }
            }
        }

        return true;
    }

public:
    CitizenBTree() : root(nullptr), nodeCount(0), citizenCount(0) {}



    void insert(const Citizen& citizen) {
        if (root == nullptr) {
            root = new BTreeNode(true);
            root->keys[0] = citizen;
            root->keyCount = 1;
            nodeCount = 1;
            citizenCount++;
        }
        else {
            // If root is full, tree grows in height
            if (root->keyCount == MAX_KEYS) {
                BTreeNode* newRoot = new BTreeNode(false);
                newRoot->children[0] = root;
                nodeCount++;

                // Split the old root
                splitChild(newRoot, 0);

                // New root has 2 children now. Decide which one to insert into.
                int i = 0;
                if (compareCNIC(newRoot->keys[0].cnic, citizen.cnic) < 0) {
                    i++;
                }

                insertNonFull(newRoot->children[i], citizen);
                root = newRoot;
            }
            else {
                insertNonFull(root, citizen);
            }
            citizenCount++;
        }
    }



    bool remove(const string& cnic) {
        if (root == nullptr)
            return false;

        // First check if the key exists
        Citizen* found = search(cnic);
        if (found == nullptr) return false;

        // Remove the key
        removeKey(root, cnic);

        // If root has no keys left after deletion
        if (root->keyCount == 0) {
            BTreeNode* oldRoot = root;
            if (root->isLeaf) {
                root = nullptr;
            }
            else {
                root = root->children[0];
            }
            delete oldRoot;
            nodeCount--;
        }

        citizenCount--;
        return true;
    }



    Citizen* search(const string& cnic) {
        return searchNode(root, cnic);
    }



    bool update(const string& cnic, const Citizen& updatedCitizen) {
        Citizen* existing = search(cnic);
        if (existing == nullptr)
            return false;

        existing->name = updatedCitizen.name;
        existing->age = updatedCitizen.age;
        existing->gender = updatedCitizen.gender;
        existing->sector = updatedCitizen.sector;
        existing->streetNumber = updatedCitizen.streetNumber;
        existing->houseNumber = updatedCitizen.houseNumber;
        existing->occupation = updatedCitizen.occupation;
        existing->phoneNumber = updatedCitizen.phoneNumber;

        return true;
    }

    // PUBLIC GETTERS

    void getAllCitizens(Citizen* results, int& count, int maxCount) {
        count = 0;
        inorderTraversal(root, results, count, maxCount);
    }

    void displayTree() {
        cout << "\n=== B-Tree Structure ===" << endl;
        if (root == nullptr) {
            cout << "(Empty tree)" << endl;
            return;
        }
        displayNode(root, 0);
    }

    bool validate() {
        if (root == nullptr)
            return true;
        string error;
        bool isValid = validateNode(root, true, error);
        if (!isValid) {
            cout << "B-Tree INVALID: " << error << endl;
        }
        return isValid;
    }

    int getCitizenCount() {
        return citizenCount;
    }
    int getNodeCount() {
        return countNodes(root);
    }
    int getTreeHeight() {
        return getHeight(root);
    }

    ~CitizenBTree() {
        deleteTree(root);
    }
};

// POPULATION SYSTEM MANAGER

class PopulationSystem {
private:
    CitizenBTree bTree;
    CNICHashTable hashTable;
    HierarchyTree hierarchyTree;

    bool isValidCNICFormat(const string& cnic) {
        if (cnic.length() != 15)
            return false;
        if (cnic[5] != '-' || cnic[13] != '-')
            return false;

        for (int i = 0; i < 15; i++) {
            if (i == 5 || i == 13)
                continue;
            if (cnic[i] < '0' || cnic[i] > '9')
                return false;
        }
        return true;
    }

    string getGenderFromCNIC(const string& cnic) {
        if (cnic.length() < 15)
            return "Unknown";
        char lastDigit = cnic[14];
        int digit = lastDigit - '0';
        return (digit % 2 == 0) ? "Female" : "Male";
    }

    string formatCNIC(const string& input) {
        string digits = "";
        for (size_t i = 0; i < input.length(); i++) {
            if (input[i] >= '0' && input[i] <= '9') {
                digits += input[i];
            }
        }

        if (digits.length() != 13) {
            return input;
        }

        return digits.substr(0, 5) + "-" + digits.substr(5, 7) + "-" + digits.substr(12, 1);
    }


public:
    PopulationSystem() {}


    int exportSectorPopulation(string sectorNames[], int populations[], int maxSectors) {
        const int MAX_RESULTS = 500;
        Citizen results[MAX_RESULTS];
        int count = 0;
        bTree.getAllCitizens(results, count, MAX_RESULTS);

        if (count == 0)
            return 0;

        // Count population by sector
        string sectors[100];
        int sectorPops[100];
        int sectorCount = 0;

        for (int i = 0; i < count; i++) {
            bool found = false;
            for (int j = 0; j < sectorCount; j++) {
                if (stringsEqualIgnoreCase(sectors[j], results[i].sector)) {
                    sectorPops[j]++;
                    found = true;
                    break;
                }
            }
            if (!found && sectorCount < 100) {
                sectors[sectorCount] = results[i].sector;
                sectorPops[sectorCount] = 1;
                sectorCount++;
            }
        }

        // Copy to output arrays
        int outputCount = (sectorCount < maxSectors) ? sectorCount : maxSectors;
        for (int i = 0; i < outputCount; i++) {
            sectorNames[i] = sectors[i];
            populations[i] = sectorPops[i];
        }

        return outputCount;
    }


    void registerCitizen() {
        cout << "\n=== Register New Citizen ===" << endl;

        string cnicInput = getStringInput("Enter CNIC (XXXXX-XXXXXXX-X or 13 digits): ");
        string cnic = formatCNIC(cnicInput);

        if (!isValidCNICFormat(cnic)) {
            cout << "Error: Invalid CNIC format!" << endl;
            cout << "Expected format: XXXXX-XXXXXXX-X (e.g., 61101-1234567-1)" << endl;
            return;
        }

        if (hashTable.exists(cnic)) {
            cout << "Error: Citizen with CNIC '" << cnic << "' already exists!" << endl;
            return;
        }

        string name = getStringInput("Enter Full Name: ");
        if (name.empty()) {
            cout << "Error: Name cannot be empty." << endl;
            return;
        }

        int age = getIntInput("Enter Age: ");
        if (age < 0 || age > 150) {
            cout << "Error: Invalid age!" << endl;
            return;
        }

        string gender = getGenderFromCNIC(cnic);
        cout << "Gender (from CNIC): " << gender << endl;

        string sector = getStringInput("Enter Sector (e.g., G-10, F-8): ");
        if (sector.empty()) {
            cout << "Error: Sector cannot be empty." << endl;
            return;
        }

        string street = getStringInput("Enter Street Number: ");
        if (street.empty()) {
            cout << "Error: Street number cannot be empty." << endl;
            return;
        }

        string house = getStringInput("Enter House Number: ");
        if (house.empty()) {
            cout << "Error: House number cannot be empty." << endl;
            return;
        }

        string occupation = getStringInput("Enter Occupation: ");
        string phone = getStringInput("Enter Phone Number (optional): ");

        Citizen newCitizen(cnic, name, age, gender, sector, street, house, occupation, phone);

        bTree.insert(newCitizen);

        Citizen* citizenPtr = bTree.search(cnic);
        hashTable.insert(cnic, citizenPtr, sector, street, house);

        hierarchyTree.insertCitizen(citizenPtr);

        cout << "\nCitizen registered successfully!" << endl;
        cout << "CNIC: " << cnic << endl;
        cout << "Name: " << name << endl;
        cout << "Gender: " << gender << endl;
    }

    void deleteCitizen() {
        cout << "\n=== Delete Citizen ===" << endl;

        string cnicInput = getStringInput("Enter CNIC of citizen to delete: ");
        string cnic = formatCNIC(cnicInput);

        if (!isValidCNICFormat(cnic)) {
            cout << "Invalid CNIC format!" << endl;
            return;
        }

        Citizen* citizen = bTree.search(cnic);
        if (citizen == nullptr) {
            cout << "Citizen with CNIC '" << cnic << "' not found!" << endl;
            return;
        }

        cout << "\nCitizen Details:" << endl;
        citizen->display();

        string confirm = getStringInput("\nType 'YES' to confirm deletion: ");
        if (confirm != "YES") {
            cout << "Deletion cancelled." << endl;
            return;
        }

        hashTable.remove(cnic);
        hierarchyTree.removeCitizen(cnic);
        bTree.remove(cnic);

        cout << "Citizen deleted successfully!" << endl;
    }

    void updateCitizen() {
        cout << "\n=== Update Citizen ===" << endl;

        string cnicInput = getStringInput("Enter CNIC of citizen to update: ");
        string cnic = formatCNIC(cnicInput);

        if (!isValidCNICFormat(cnic)) {
            cout << "Invalid CNIC format!" << endl;
            return;
        }

        Citizen* citizen = bTree.search(cnic);
        if (citizen == nullptr) {
            cout << "Citizen with CNIC '" << cnic << "' not found!" << endl;
            return;
        }

        cout << "\nCurrent Details:" << endl;
        citizen->display();

        cout << "\nWhat would you like to update?" << endl;
        cout << "1. Name" << endl;
        cout << "2. Age" << endl;
        cout << "3. Address (Sector, Street, House)" << endl;
        cout << "4. Occupation" << endl;
        cout << "5. Phone Number" << endl;
        cout << "6. All of the above" << endl;
        cout << "0. Cancel" << endl;

        int choice = getIntInput("Enter choice: ");

        if (choice == 0) {
            cout << "Update cancelled." << endl;
            return;
        }

        string oldSector = citizen->sector;
        string oldStreet = citizen->streetNumber;
        string oldHouse = citizen->houseNumber;
        bool addressChanged = false;

        if (choice == 1 || choice == 6) {
            string newName = getStringInput("Enter new name (or press Enter to keep current): ");
            if (!newName.empty()) {
                citizen->name = newName;
            }
        }

        if (choice == 2 || choice == 6) {
            int newAge = getIntInput("Enter new age (-1 to keep current): ");
            if (newAge >= 0 && newAge <= 150) {
                citizen->age = newAge;
            }
        }

        if (choice == 3 || choice == 6) {
            string newSector = getStringInput("Enter new sector (or press Enter to keep current): ");
            if (!newSector.empty()) {
                citizen->sector = newSector;
                addressChanged = true;
            }

            string newStreet = getStringInput("Enter new street number (or press Enter to keep current): ");
            if (!newStreet.empty()) {
                citizen->streetNumber = newStreet;
                addressChanged = true;
            }

            string newHouse = getStringInput("Enter new house number (or press Enter to keep current): ");
            if (!newHouse.empty()) {
                citizen->houseNumber = newHouse;
                addressChanged = true;
            }
        }

        if (choice == 4 || choice == 6) {
            string newOccupation = getStringInput("Enter new occupation (or press Enter to keep current): ");
            if (!newOccupation.empty()) {
                citizen->occupation = newOccupation;
            }
        }

        if (choice == 5 || choice == 6) {
            string newPhone = getStringInput("Enter new phone number (or press Enter to keep current): ");
            if (!newPhone.empty()) {
                citizen->phoneNumber = newPhone;
            }
        }

        if (addressChanged) {


            hashTable.remove(cnic);
            hashTable.insert(cnic, citizen, citizen->sector, citizen->streetNumber, citizen->houseNumber);

            hierarchyTree.updateCitizenLocation(citizen, oldSector, oldStreet, oldHouse);
        }

        cout << "Citizen updated successfully!" << endl;
    }

    void searchCitizen() {
        cout << "\n=== Search Citizen ===" << endl;

        cout << "Search by:" << endl;
        cout << "1. CNIC" << endl;
        cout << "2. Name" << endl;

        int choice = getIntInput("Enter choice: ");

        if (choice == 1) {
            string cnicInput = getStringInput("Enter CNIC: ");
            string cnic = formatCNIC(cnicInput);


            Citizen* citizen = bTree.search(cnic);

            if (citizen != nullptr) {
                cout << "\n=== Citizen Found ===" << endl;
                citizen->display();
            }
            else {
                if (hashTable.exists(cnic)) {
                    cout << "Error: Index mismatch. Citizen exists in registry but record is inaccessible." << endl;
                }
                else {
                    cout << "Citizen with CNIC '" << cnic << "' not found." << endl;
                }
            }
        }
        else if (choice == 2) {
            string nameQuery = getStringInput("Enter name to search: ");
            string lowerQuery = toLowerStr(trimString(nameQuery));

            const int MAX_RESULTS = 500;
            Citizen results[MAX_RESULTS];
            int count = 0;
            bTree.getAllCitizens(results, count, MAX_RESULTS);

            bool found = false;
            cout << "\n=== Search Results ===" << endl;

            for (int i = 0; i < count; i++) {
                string lowerName = toLowerStr(results[i].name);
                if (lowerName.find(lowerQuery) != string::npos) {
                    cout << "\n--- Citizen " << (found ? "" : "#1") << " ---" << endl;
                    results[i].display();
                    found = true;
                }
            }

            if (!found) {
                cout << "No citizens found matching '" << nameQuery << "'." << endl;
            }
        }
    }

    void transferCitizen() {
        cout << "\n=== Transfer Citizen to New Address ===" << endl;

        string cnicInput = getStringInput("Enter CNIC: ");
        string cnic = formatCNIC(cnicInput);

        Citizen* citizen = bTree.search(cnic);
        if (citizen == nullptr) {
            cout << "Citizen not found!" << endl;
            return;
        }

        cout << "\nCurrent Address:" << endl;
        cout << "  House " << citizen->houseNumber << ", Street " << citizen->streetNumber
            << ", " << citizen->sector << endl;

        cout << "\nEnter New Address:" << endl;
        string newSector = getStringInput("New Sector: ");
        string newStreet = getStringInput("New Street Number: ");
        string newHouse = getStringInput("New House Number: ");

        if (newSector.empty() || newStreet.empty() || newHouse.empty()) {
            cout << "All address fields are required!" << endl;
            return;
        }

        citizen->sector = newSector;
        citizen->streetNumber = newStreet;
        citizen->houseNumber = newHouse;

        hashTable.insert(cnic, citizen, newSector, newStreet, newHouse);

        cout << "\nCitizen transferred successfully!" << endl;
        cout << "New Address: House " << newHouse << ", Street " << newStreet
            << ", " << newSector << endl;
    }

    void displayAllCitizens() {
        cout << "\n=== All Registered Citizens ===" << endl;

        const int MAX_RESULTS = 500;
        Citizen results[MAX_RESULTS];
        int count = 0;
        bTree.getAllCitizens(results, count, MAX_RESULTS);

        if (count == 0) {
            cout << "No citizens registered." << endl;
            return;
        }

        for (int i = 0; i < count; i++) {
            cout << "\n--- Citizen #" << (i + 1) << " ---" << endl;
            results[i].display();
        }

        cout << "\nTotal Citizens: " << count << endl;
    }

    void displayCitizensBySector() {
        cout << "\n=== Display Citizens by Sector ===" << endl;

        string sector = getStringInput("Enter Sector: ");

        const int MAX_RESULTS = 500;
        Citizen* results[MAX_RESULTS];
        int count = 0;
        hashTable.getCitizensBySector(sector, results, count, MAX_RESULTS);

        if (count == 0) {
            cout << "No citizens found in sector '" << sector << "'." << endl;
            return;
        }

        cout << "\n=== Citizens in " << sector << " ===" << endl;
        for (int i = 0; i < count; i++) {
            cout << "\n--- Citizen #" << (i + 1) << " ---" << endl;
            results[i]->display();
        }

        cout << "\nTotal: " << count << " citizens" << endl;
    }

    void displayCitizensByStreet() {
        cout << "\n=== Display Citizens by Street ===" << endl;

        string sector = getStringInput("Enter Sector: ");
        string street = getStringInput("Enter Street Number: ");

        const int MAX_RESULTS = 500;
        Citizen* results[MAX_RESULTS];
        int count = 0;
        hashTable.getCitizensByStreet(sector, street, results, count, MAX_RESULTS);

        if (count == 0) {
            cout << "No citizens found on Street " << street << " in " << sector << "." << endl;
            return;
        }

        cout << "\n=== Citizens on Street " << street << ", " << sector << " ===" << endl;
        for (int i = 0; i < count; i++) {
            cout << "\n--- Citizen #" << (i + 1) << " ---" << endl;
            results[i]->display();
        }

        cout << "\nTotal: " << count << " citizens" << endl;
    }

    void displayCitizensByHouse() {
        cout << "\n=== Display Citizens by House ===" << endl;

        string sector = getStringInput("Enter Sector: ");
        string street = getStringInput("Enter Street Number: ");
        string house = getStringInput("Enter House Number: ");

        const int MAX_RESULTS = 500;
        Citizen* results[MAX_RESULTS];
        int count = 0;
        hashTable.getCitizensByHouse(sector, street, house, results, count, MAX_RESULTS);

        if (count == 0) {
            cout << "No citizens found in House " << house << ", Street " << street
                << ", " << sector << "." << endl;
            return;
        }

        cout << "\n=== Residents of House " << house << ", Street " << street
            << ", " << sector << " ===" << endl;
        for (int i = 0; i < count; i++) {
            cout << "\n--- Resident #" << (i + 1) << " ---" << endl;
            results[i]->display();
        }

        cout << "\nTotal Residents: " << count << endl;
    }

    void displayBTreeStructure() {
        bTree.displayTree();
    }

    void displayStatistics() {
        cout << "\n=== Population System Statistics ===" << endl;
        cout << "Total Citizens: " << bTree.getCitizenCount() << endl;
        cout << "B-Tree Nodes: " << bTree.getNodeCount() << endl;
        cout << "B-Tree Height: " << bTree.getTreeHeight() << endl;
        cout << "B-Tree Order: " << B_TREE_ORDER << endl;
        cout << "Max Keys per Node: " << MAX_KEYS << endl;
        cout << "Min Keys per Node: " << MIN_KEYS << endl;

        const int MAX_RESULTS = 500;
        Citizen results[MAX_RESULTS];
        int count = 0;
        bTree.getAllCitizens(results, count, MAX_RESULTS);

        int maleCount = 0, femaleCount = 0;
        int ageSum = 0;
        int children = 0, adults = 0, seniors = 0;

        for (int i = 0; i < count; i++) {
            if (results[i].gender == "Male")
                maleCount++;
            else
                femaleCount++;

            ageSum += results[i].age;

            if (results[i].age < 18) children++;
            else if (results[i].age < 60) adults++;
            else seniors++;
        }

        if (count > 0) {
            cout << "\n--- Demographics ---" << endl;
            cout << "Male: " << maleCount << " (" << (maleCount * 100 / count) << "%)" << endl;
            cout << "Female: " << femaleCount << " (" << (femaleCount * 100 / count) << "%)" << endl;
            cout << "Average Age: " << (ageSum / count) << " years" << endl;
            cout << "\n--- Age Groups ---" << endl;
            cout << "Children (0-17): " << children << endl;
            cout << "Adults (18-59): " << adults << endl;
            cout << "Seniors (60+): " << seniors << endl;
        }
    }

    void displayHierarchy() {
        // Use the N-ary hierarchy tree to display the structure
        hierarchyTree.displayTree();
    }

    void generateOccupationReport() {
        cout << "\n=== Occupation Report ===" << endl;

        const int MAX_RESULTS = 500;
        Citizen results[MAX_RESULTS];
        int count = 0;
        bTree.getAllCitizens(results, count, MAX_RESULTS);

        if (count == 0) {
            cout << "No data available." << endl;
            return;
        }

        string occupations[100];
        int occCounts[100];
        int occCount = 0;

        for (int i = 0; i < count; i++) {
            string occ = results[i].occupation;
            bool found = false;

            for (int j = 0; j < occCount; j++) {
                if (stringsEqualIgnoreCase(occupations[j], occ)) {
                    occCounts[j]++;
                    found = true;
                    break;
                }
            }

            if (!found && occCount < 100) {
                occupations[occCount] = occ;
                occCounts[occCount] = 1;
                occCount++;
            }
        }

        for (int i = 0; i < occCount - 1; i++) {
            for (int j = 0; j < occCount - i - 1; j++) {
                if (occCounts[j] < occCounts[j + 1]) {
                    int tempCount = occCounts[j];
                    occCounts[j] = occCounts[j + 1];
                    occCounts[j + 1] = tempCount;
                    string tempName = occupations[j];
                    occupations[j] = occupations[j + 1];
                    occupations[j + 1] = tempName;
                }
            }
        }

        for (int i = 0; i < occCount; i++) {
            cout << (i + 1) << ". " << occupations[i] << ": " << occCounts[i]
                << " (" << (occCounts[i] * 100 / count) << "%)" << endl;
        }
    }

    void generateAgeReport() {
        cout << "\n=== Age Distribution Report ===" << endl;

        const int MAX_RESULTS = 500;
        Citizen results[MAX_RESULTS];
        int count = 0;
        bTree.getAllCitizens(results, count, MAX_RESULTS);

        if (count == 0) {
            cout << "No data available." << endl;
            return;
        }

        int age0_5 = 0, age6_12 = 0, age13_17 = 0;
        int age18_25 = 0, age26_35 = 0, age36_45 = 0;
        int age46_55 = 0, age56_65 = 0, age66_plus = 0;

        for (int i = 0; i < count; i++) {
            int age = results[i].age;
            if (age <= 5)
                age0_5++;
            else if (age <= 12)
                age6_12++;
            else if (age <= 17)
                age13_17++;
            else if (age <= 25)
                age18_25++;
            else if (age <= 35)
                age26_35++;
            else if (age <= 45)
                age36_45++;
            else if (age <= 55)
                age46_55++;
            else if (age <= 65)
                age56_65++;
            else age66_plus++;
        }

        cout << "0-5 years:   " << age0_5 << " (" << (age0_5 * 100 / count) << "%)" << endl;
        cout << "6-12 years:  " << age6_12 << " (" << (age6_12 * 100 / count) << "%)" << endl;
        cout << "13-17 years: " << age13_17 << " (" << (age13_17 * 100 / count) << "%)" << endl;
        cout << "18-25 years: " << age18_25 << " (" << (age18_25 * 100 / count) << "%)" << endl;
        cout << "26-35 years: " << age26_35 << " (" << (age26_35 * 100 / count) << "%)" << endl;
        cout << "36-45 years: " << age36_45 << " (" << (age36_45 * 100 / count) << "%)" << endl;
        cout << "46-55 years: " << age46_55 << " (" << (age46_55 * 100 / count) << "%)" << endl;
        cout << "56-65 years: " << age56_65 << " (" << (age56_65 * 100 / count) << "%)" << endl;
        cout << "66+ years:   " << age66_plus << " (" << (age66_plus * 100 / count) << "%)" << endl;
    }

    void generateSectorReport() {
        cout << "\n=== Sector Population Report ===" << endl;

        const int MAX_RESULTS = 500;
        Citizen results[MAX_RESULTS];
        int count = 0;
        bTree.getAllCitizens(results, count, MAX_RESULTS);

        if (count == 0) {
            cout << "No data available." << endl;
            return;
        }

        string sectors[50];
        int sectorCounts[50];
        int sectorCount = 0;

        for (int i = 0; i < count; i++) {
            bool found = false;
            for (int j = 0; j < sectorCount; j++) {
                if (stringsEqualIgnoreCase(sectors[j], results[i].sector)) {
                    sectorCounts[j]++;
                    found = true;
                    break;
                }
            }
            if (!found && sectorCount < 50) {
                sectors[sectorCount] = results[i].sector;
                sectorCounts[sectorCount] = 1;
                sectorCount++;
            }
        }

        for (int i = 0; i < sectorCount - 1; i++) {
            for (int j = 0; j < sectorCount - i - 1; j++) {
                if (sectorCounts[j] < sectorCounts[j + 1]) {
                    int tempCount = sectorCounts[j];
                    sectorCounts[j] = sectorCounts[j + 1];
                    sectorCounts[j + 1] = tempCount;
                    string tempName = sectors[j];
                    sectors[j] = sectors[j + 1];
                    sectors[j + 1] = tempName;
                }
            }
        }

        for (int i = 0; i < sectorCount; i++) {
            cout << (i + 1) << ". " << sectors[i] << ": " << sectorCounts[i]
                << " residents (" << (sectorCounts[i] * 100 / count) << "%)" << endl;
        }
    }

    void loadFromFile() {
        ifstream file("population.csv");
        if (!file.is_open()) {
            cout << "Warning: population.csv not found." << endl;
            return;
        }

        string line;
        getline(file, line);
        int loadedCount = 0;
        int duplicateCount = 0;
        int errorCount = 0;

        while (getline(file, line)) {
            if (line.empty())
                continue;

            for (size_t i = 0; i < line.length(); i++) {
                if (line[i] == '\t') line[i] = ',';
            }

            string parts[8];
            int count = splitString(line, ',', parts, 8);

            if (count >= 7) {
                string cnic = formatCNIC(trimString(parts[0]));
                string name = trimString(parts[1]);
                int age = stringToInt(trimString(parts[2]));
                string sector = trimString(parts[3]);
                string street = trimString(parts[4]);
                string house = trimString(parts[5]);
                string occupation = trimString(parts[6]);
                string phone = (count >= 8) ? trimString(parts[7]) : "";

                if (!isValidCNICFormat(cnic)) {
                    errorCount++;
                    continue;
                }

                if (hashTable.exists(cnic)) {
                    duplicateCount++;
                    continue;
                }

                string gender = getGenderFromCNIC(cnic);

                Citizen newCitizen(cnic, name, age, gender, sector, street, house, occupation, phone);
                bTree.insert(newCitizen);

                Citizen* citizenPtr = bTree.search(cnic);
                hashTable.insert(cnic, citizenPtr, sector, street, house);

                hierarchyTree.insertCitizen(citizenPtr);

                loadedCount++;
            }
        }

        file.close();
        cout << "Loaded " << loadedCount << " citizens from file." << endl;
        if (duplicateCount > 0) {
            cout << "Skipped " << duplicateCount << " duplicate CNICs." << endl;
        }
        if (errorCount > 0) {
            cout << "Skipped " << errorCount << " invalid entries." << endl;
        }
    }

    int getCitizenCount() {

        return bTree.getCitizenCount();

    }
};