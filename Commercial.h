#pragma once

#include "Utils.h"
#include "GlobalLocationManager.h"
#include "CityGraph.h"


struct ProductNode {
    string productID;
    string name;
    string category;
    int price;
    int quantity;
    ProductNode* next;

    ProductNode(const string& id, const string& n, const string& cat, int p, int qty)
        : productID(id), name(n), category(cat), price(p), quantity(qty), next(nullptr) {
    }
};


struct ProductHashNode {
    string productName;
    string mallID;
    ProductNode* productRef;
    ProductHashNode* next;

    ProductHashNode(const string& name, const string& mID, ProductNode* ref)
        : productName(name), mallID(mID), productRef(ref), next(nullptr) {
    }
};


struct MallEdge {
    string destMallID;
    double distance;
    MallEdge* next;

    MallEdge(const string& dest, double dist) : destMallID(dest), distance(dist), next(nullptr) {}
};


struct MallAdjNode {
    string mallID;
    MallEdge* edgeHead;
    MallAdjNode* next;

    MallAdjNode(const string& id) : mallID(id), edgeHead(nullptr), next(nullptr) {}
};


struct MallNode {
    string mallID;
    string name;
    string sector;
    double lat, lon;
    string openingTime;
    string closingTime;
    int productCount;
    static const int PRODUCT_TABLE_SIZE = 30;
    ProductNode* productTable[PRODUCT_TABLE_SIZE];
    MallNode* next;

    MallNode(const string& id, const string& n, const string& sec, double la, double lo,
        const string& open, const string& close)
        : mallID(id), name(n), sector(sec), lat(la), lon(lo),
        openingTime(open), closingTime(close), productCount(0), next(nullptr) {
        for (int i = 0; i < PRODUCT_TABLE_SIZE; i++) {
            productTable[i] = nullptr;
        }
    }
};



struct CommercialDijkstraNode {
    string mallID;
    double distance;
};

class CommercialDijkstraHeap {
private:
    static const int MAX_SIZE = 500;
    CommercialDijkstraNode heap[MAX_SIZE];
    int size;

    void swap(int i, int j) {
        CommercialDijkstraNode temp = heap[i];
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
    CommercialDijkstraHeap() : size(0) {}

    bool isEmpty() { 
        return size == 0; 
    }

    void insert(const string& id, double dist) {
        if (size >= MAX_SIZE)
            return;
        heap[size].mallID = id;
        heap[size].distance = dist;
        siftUp(size);
        size++;
    }

    CommercialDijkstraNode extractMin() {
        CommercialDijkstraNode minNode = heap[0];
        heap[0] = heap[size - 1];
        size--;
        if (size > 0) siftDown(0);
        return minNode;
    }

    void clear() { size = 0; }
};


class ProductHashTable {
private:
    static const int TABLE_SIZE = 100;
    ProductHashNode* table[TABLE_SIZE];

    int hashFunc(const string& key) {
        return polynomialHash(key, TABLE_SIZE);
    }

public:
    ProductHashTable() {
        for (int i = 0; i < TABLE_SIZE; i++) {
            table[i] = nullptr;
        }
    }

    void insert(const string& productName, const string& mallID, ProductNode* product) {
        string cleanName = toLowerStr(trimString(productName));
        if (cleanName.empty())
            return;

        int idx = hashFunc(cleanName);

        // Check for duplicate
        ProductHashNode* current = table[idx];
        while (current != nullptr) {
            if (current->productName == cleanName && current->mallID == mallID) {
                current->productRef = product;
                return;
            }
            current = current->next;
        }

        ProductHashNode* newNode = new ProductHashNode(cleanName, mallID, product);
        newNode->next = table[idx];
        table[idx] = newNode;
    }

    void remove(const string& productName, const string& mallID) {
        string cleanName = toLowerStr(trimString(productName));
        int idx = hashFunc(cleanName);

        if (table[idx] == nullptr)
            return;

        if (table[idx]->productName == cleanName && table[idx]->mallID == mallID) {
            ProductHashNode* temp = table[idx];
            table[idx] = table[idx]->next;
            delete temp;
            return;
        }

        ProductHashNode* current = table[idx];
        while (current->next != nullptr) {
            if (current->next->productName == cleanName && current->next->mallID == mallID) {
                ProductHashNode* temp = current->next;
                current->next = current->next->next;
                delete temp;
                return;
            }
            current = current->next;
        }
    }

    void removeAllForMall(const string& mallID) {
        for (int i = 0; i < TABLE_SIZE; i++) {
            while (table[i] != nullptr && table[i]->mallID == mallID) {
                ProductHashNode* temp = table[i];
                table[i] = table[i]->next;
                delete temp;
            }

            if (table[i] == nullptr)
                continue;

            ProductHashNode* current = table[i];
            while (current->next != nullptr) {
                if (current->next->mallID == mallID) {
                    ProductHashNode* temp = current->next;
                    current->next = current->next->next;
                    delete temp;
                }
                else {
                    current = current->next;
                }
            }
        }
    }

    void search(const string& query) {
        string cleanQuery = toLowerStr(trimString(query));
        bool found = false;

        cout << "\n=== Product Search Results for '" << query << "' ===" << endl;

        for (int i = 0; i < TABLE_SIZE; i++) {
            ProductHashNode* current = table[i];
            while (current != nullptr) {
                if (current->productName.find(cleanQuery) != string::npos) {
                    if (current->productRef != nullptr) {
                        cout << "- " << current->productRef->name
                            << " [" << current->productRef->category << "]" << endl;
                        cout << "  Mall ID: " << current->mallID
                            << " | Price: Rs.  " << current->productRef->price
                            << " | Stock: " << current->productRef->quantity << endl;
                        found = true;
                    }
                }
                current = current->next;
            }
        }

        if (!found) {
            cout << "No products found matching '" << query << "'." << endl;
        }
    }

    void searchByCategory(const string& category) {
        string cleanCategory = toLowerStr(trimString(category));
        bool found = false;

        cout << "\n=== Products in Category '" << category << "' ===" << endl;

        for (int i = 0; i < TABLE_SIZE; i++) {
            ProductHashNode* current = table[i];
            while (current != nullptr) {
                if (current->productRef != nullptr) {
                    string prodCategory = toLowerStr(current->productRef->category);
                    if (prodCategory.find(cleanCategory) != string::npos) {
                        cout << "- " << current->productRef->name
                            << " | Price: Rs. " << current->productRef->price
                            << " | Stock: " << current->productRef->quantity << endl;
                        cout << "  Mall ID: " << current->mallID << endl;
                        found = true;
                    }
                }
                current = current->next;
            }
        }

        if (!found) {
            cout << "No products found in category '" << category << "'." << endl;
        }
    }

    ~ProductHashTable() {
        for (int i = 0; i < TABLE_SIZE; i++) {
            while (table[i] != nullptr) {
                ProductHashNode* temp = table[i];
                table[i] = table[i]->next;
                delete temp;
            }
        }
    }
};


class CommercialSystem {
private:
    MallNode* mallsHead;
    MallAdjNode* adjListHead;
    ProductHashTable productHash;
    int mallCount;
    int mallIDCounter;
    int productIDCounter;



    int productTableHash(const string& name) {
        return polynomialHash(name, MallNode::PRODUCT_TABLE_SIZE);
    }

    MallNode* findMallByName(const string& name) {
        MallNode* current = mallsHead;
        while (current != nullptr) {
            if (stringsEqualIgnoreCase(current->name, name))
                return current;
            current = current->next;
        }
        return nullptr;
    }

    MallNode* findMallByID(const string& id) {
        MallNode* current = mallsHead;
        while (current != nullptr) {
            if (stringsEqualIgnoreCase(current->mallID, id))
                return current;
            current = current->next;
        }
        return nullptr;
    }

    MallAdjNode* getAdjList(const string& mallID) {
        MallAdjNode* current = adjListHead;
        while (current != nullptr) {
            if (stringsEqualIgnoreCase(current->mallID, mallID))
                return current;
            current = current->next;
        }
        return nullptr;
    }

    MallAdjNode* getOrCreateAdjList(const string& mallID) {
        MallAdjNode* existing = getAdjList(mallID);
        if (existing != nullptr)
            return existing;

        MallAdjNode* newNode = new MallAdjNode(mallID);
        newNode->next = adjListHead;
        adjListHead = newNode;
        return newNode;
    }

    bool edgeExists(const string& from, const string& to) {
        MallAdjNode* adj = getAdjList(from);
        if (adj == nullptr)
            return false;

        MallEdge* edge = adj->edgeHead;
        while (edge != nullptr) {
            if (stringsEqualIgnoreCase(edge->destMallID, to)) return true;
            edge = edge->next;
        }
        return false;
    }

    void removeEdgeFromAdjList(MallAdjNode* adj, const string& destID) {
        if (adj == nullptr || adj->edgeHead == nullptr)
            return;

        while (adj->edgeHead != nullptr &&
            stringsEqualIgnoreCase(adj->edgeHead->destMallID, destID)) {
            MallEdge* temp = adj->edgeHead;
            adj->edgeHead = adj->edgeHead->next;
            delete temp;
        }

        if (adj->edgeHead == nullptr)
            return;

        MallEdge* current = adj->edgeHead;
        while (current->next != nullptr) {
            if (stringsEqualIgnoreCase(current->next->destMallID, destID)) {
                MallEdge* temp = current->next;
                current->next = current->next->next;
                delete temp;
            }
            else {
                current = current->next;
            }
        }
    }

    void removeAdjListEntry(const string& mallID) {
        if (adjListHead == nullptr)
            return;

        if (stringsEqualIgnoreCase(adjListHead->mallID, mallID)) {
            MallAdjNode* temp = adjListHead;
            while (temp->edgeHead != nullptr) {
                MallEdge* edgeTemp = temp->edgeHead;
                temp->edgeHead = temp->edgeHead->next;
                delete edgeTemp;
            }
            adjListHead = adjListHead->next;
            delete temp;
            return;
        }

        MallAdjNode* current = adjListHead;
        while (current->next != nullptr) {
            if (stringsEqualIgnoreCase(current->next->mallID, mallID)) {
                MallAdjNode* temp = current->next;
                while (temp->edgeHead != nullptr) {
                    MallEdge* edgeTemp = temp->edgeHead;
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

    ProductNode* findProduct(MallNode* mall, const string& productName) {
        if (mall == nullptr)
            return nullptr;

        int idx = productTableHash(productName);
        ProductNode* current = mall->productTable[idx];
        while (current != nullptr) {
            if (stringsEqualIgnoreCase(current->name, productName))
                return current;
            current = current->next;
        }
        return nullptr;
    }

    ProductNode* findProductByID(MallNode* mall, const string& productID) {
        if (mall == nullptr)
            return nullptr;

        for (int i = 0; i < MallNode::PRODUCT_TABLE_SIZE; i++) {
            ProductNode* current = mall->productTable[i];
            while (current != nullptr) {
                if (stringsEqualIgnoreCase(current->productID, productID))
                    return current;
                current = current->next;
            }
        }
        return nullptr;
    }

    void deleteAllProducts(MallNode* mall) {
        for (int i = 0; i < MallNode::PRODUCT_TABLE_SIZE; i++) {
            while (mall->productTable[i] != nullptr) {
                ProductNode* temp = mall->productTable[i];
                mall->productTable[i] = mall->productTable[i]->next;
                delete temp;
            }
        }
        mall->productCount = 0;
    }

    MallNode* selectMall() {
        string name = getStringInput("Enter Mall Name: ");
        MallNode* mall = findMallByName(name);
        if (mall == nullptr) {
            cout << " Mall '" << name << "' not found!" << endl;
        }
        return mall;
    }

    int getTotalProducts() {
        int total = 0;
        MallNode* mall = mallsHead;
        while (mall != nullptr) {
            total += mall->productCount;
            mall = mall->next;
        }
        return total;
    }

    int getConnectionCount() {
        int count = 0;
        MallAdjNode* adj = adjListHead;
        while (adj != nullptr) {
            MallEdge* edge = adj->edgeHead;
            while (edge != nullptr) {
                count++;
                edge = edge->next;
            }
            adj = adj->next;
        }
        return count / 2; // Undirected graph so each edge counted twice
    }

public:
    CommercialSystem() : mallsHead(nullptr), adjListHead(nullptr), mallCount(0),
        mallIDCounter(1), productIDCounter(1) {
    }
    void registerMall() {
        cout << "\n=== Register New Mall ===" << endl;

        string name = getStringInput("Enter Mall Name: ");
        if (name.empty()) {
            cout << "Mall name cannot be empty." << endl;
            return;
        }

        if (findMallByName(name) != nullptr) {
            cout << " Mall '" << name << "' already exists!" << endl;
            return;
        }

        string sector = getStringInput("Enter Sector (e.g., F-10, Blue Area): ");
        if (sector.empty()) {
            cout << " Sector cannot be empty." << endl;
            return;
        }

        double lat = getDoubleInput("Enter Latitude: ");
        double lon = getDoubleInput("Enter Longitude: ");

        GlobalLocationManager* locMgr = GlobalLocationManager::getInstance();
        if (!locMgr->registerLocation(lat, lon, "Mall", name)) {
            return;
        }

        string openTime = getStringInput("Enter Opening Time (e.g., 10:00 AM): ");
        string closeTime = getStringInput("Enter Closing Time (e.g., 10:00 PM): ");

        string mallID = "M" + intToString(mallIDCounter++);

        MallNode* newMall = new MallNode(mallID, name, sector, lat, lon, openTime, closeTime);
        newMall->next = mallsHead;
        mallsHead = newMall;
        mallCount++;

        getOrCreateAdjList(mallID);

        // Register with CityGraph for unified city-wide graph
        CityGraph::getInstance()->addLocation(mallID, name, LOC_MALL, lat, lon, newMall);

        cout << "Mall '" << name << "' registered successfully with ID: " << mallID << endl;
    }

    void deleteMall() {
        cout << "\n=== Delete Mall ===" << endl;

        string name = getStringInput("Enter Mall Name to delete: ");
        MallNode* mall = findMallByName(name);

        if (mall == nullptr) {
            cout << " Mall not found!" << endl;
            return;
        }

        cout << "\nMall Details:" << endl;
        cout << "  Name: " << mall->name << endl;
        cout << "  Sector: " << mall->sector << endl;
        cout << "  Products: " << mall->productCount << endl;

        string confirm = getStringInput("\nType 'YES' to confirm deletion of '" + name + "' and ALL its products: ");
        if (confirm != "YES") {
            cout << "Deletion cancelled." << endl;
            return;
        }

        string mallID = mall->mallID;

        // Remove all edges TO this mall
        MallAdjNode* adj = adjListHead;
        while (adj != nullptr) {
            removeEdgeFromAdjList(adj, mallID);
            adj = adj->next;
        }

        // Remove this mall's adjacency list
        removeAdjListEntry(mallID);

        // Remove products from hash table
        productHash.removeAllForMall(mallID);

        // Remove from linked list
        if (stringsEqualIgnoreCase(mallsHead->name, name)) {
            MallNode* temp = mallsHead;
            mallsHead = mallsHead->next;
            deleteAllProducts(temp);
            GlobalLocationManager::getInstance()->removeLocation(temp->lat, temp->lon);
            CityGraph::getInstance()->removeLocation(temp->mallID);
            delete temp;
        }
        else {
            MallNode* current = mallsHead;
            while (current->next != nullptr) {
                if (stringsEqualIgnoreCase(current->next->name, name)) {
                    MallNode* temp = current->next;
                    current->next = current->next->next;
                    deleteAllProducts(temp);
                    GlobalLocationManager::getInstance()->removeLocation(temp->lat, temp->lon);
                    CityGraph::getInstance()->removeLocation(temp->mallID);
                    delete temp;
                    break;
                }
                current = current->next;
            }
        }

        mallCount--;
        cout << "Mall '" << name << "' deleted successfully!" << endl;
    }

    void updateMall() {
        cout << "\n=== Update Mall ===" << endl;

        MallNode* mall = selectMall();
        if (mall == nullptr)
            return;

        cout << "\nCurrent Details:" << endl;
        cout << "  ID: " << mall->mallID << endl;
        cout << "  Name: " << mall->name << endl;
        cout << "  Sector: " << mall->sector << endl;
        cout << "  Opening Time: " << mall->openingTime << endl;
        cout << "  Closing Time: " << mall->closingTime << endl;

        cout << "\nWhat would you like to update?" << endl;
        cout << "1. Name" << endl;
        cout << "2. Opening Time" << endl;
        cout << "3. Closing Time" << endl;
        cout << "4. All Timings" << endl;
        cout << "5. Everything" << endl;
        cout << "0. Cancel" << endl;

        int choice = getIntInput("Enter choice: ");

        if (choice == 0) {
            cout << "Update cancelled." << endl;
            return;
        }

        if (choice == 1 || choice == 5) {
            string newName = getStringInput("Enter new name (or press Enter to keep current): ");
            if (!newName.empty()) {
                if (findMallByName(newName) != nullptr && !stringsEqualIgnoreCase(newName, mall->name)) {
                    cout << " A mall with name '" << newName << "' already exists!" << endl;
                    return;
                }
                mall->name = newName;
            }
        }

        if (choice == 2 || choice == 4 || choice == 5) {
            string newOpenTime = getStringInput("Enter new opening time (or press Enter to keep current): ");
            if (!newOpenTime.empty()) {
                mall->openingTime = newOpenTime;
            }
        }

        if (choice == 3 || choice == 4 || choice == 5) {
            string newCloseTime = getStringInput("Enter new closing time (or press Enter to keep current): ");
            if (!newCloseTime.empty()) {
                mall->closingTime = newCloseTime;
            }
        }

        cout << "Mall updated successfully!" << endl;
    }



    void connectMalls() {
        cout << "\n=== Connect Malls (Add Delivery Route) ===" << endl;

        if (mallCount < 2) {
            cout << " Need at least 2 malls to create a connection." << endl;
            return;
        }

        string name1 = getStringInput("Enter First Mall Name: ");
        MallNode* mall1 = findMallByName(name1);
        if (mall1 == nullptr) {
            cout << " Mall '" << name1 << "' not found!" << endl;
            return;
        }

        string name2 = getStringInput("Enter Second Mall Name: ");
        MallNode* mall2 = findMallByName(name2);
        if (mall2 == nullptr) {
            cout << " Mall '" << name2 << "' not found!" << endl;
            return;
        }

        if (stringsEqualIgnoreCase(mall1->mallID, mall2->mallID)) {
            cout << " Cannot connect a mall to itself!" << endl;
            return;
        }

        if (edgeExists(mall1->mallID, mall2->mallID)) {
            cout << " These malls are already connected!" << endl;
            return;
        }

        double distance = calculateDistance(mall1->lat, mall1->lon, mall2->lat, mall2->lon);


        MallAdjNode* adj1 = getOrCreateAdjList(mall1->mallID);
        MallEdge* edge1 = new MallEdge(mall2->mallID, distance);
        edge1->next = adj1->edgeHead;
        adj1->edgeHead = edge1;

        MallAdjNode* adj2 = getOrCreateAdjList(mall2->mallID);
        MallEdge* edge2 = new MallEdge(mall1->mallID, distance);
        edge2->next = adj2->edgeHead;
        adj2->edgeHead = edge2;

        cout << "Connected '" << mall1->name << "' <--> '" << mall2->name
            << "' (Distance: " << distance << " units)" << endl;
    }

    void disconnectMalls() {
        cout << "\n=== Disconnect Malls (Remove Delivery Route) ===" << endl;

        string name1 = getStringInput("Enter First Mall Name: ");
        MallNode* mall1 = findMallByName(name1);
        if (mall1 == nullptr) {
            cout << " Mall '" << name1 << "' not found!" << endl;
            return;
        }

        string name2 = getStringInput("Enter Second Mall Name: ");
        MallNode* mall2 = findMallByName(name2);
        if (mall2 == nullptr) {
            cout << " Mall '" << name2 << "' not found!" << endl;
            return;
        }

        if (!edgeExists(mall1->mallID, mall2->mallID)) {
            cout << " These malls are not connected!" << endl;
            return;
        }

        MallAdjNode* adj1 = getAdjList(mall1->mallID);
        if (adj1 != nullptr) {
            removeEdgeFromAdjList(adj1, mall2->mallID);
        }

        MallAdjNode* adj2 = getAdjList(mall2->mallID);
        if (adj2 != nullptr) {
            removeEdgeFromAdjList(adj2, mall1->mallID);
        }

        cout << "Disconnected '" << mall1->name << "' from '" << mall2->name << "'!" << endl;
    }


    void addProduct() {
        cout << "\n=== Add Product to Mall ===" << endl;

        MallNode* mall = selectMall();
        if (mall == nullptr)
            return;

        string productName = getStringInput("Enter Product Name: ");
        if (productName.empty()) {
            cout << " Product name cannot be empty." << endl;
            return;
        }

        if (findProduct(mall, productName) != nullptr) {
            cout << " Product '" << productName << "' already exists in this mall!" << endl;
            cout << "Use 'Update Product' to modify existing products." << endl;
            return;
        }

        string category = getStringInput("Enter Category (e.g., Electronics, Clothing, Food): ");
        int price = getIntInput("Enter Price (Rs. ): ");
        if (price < 0) price = 0;

        int quantity = getIntInput("Enter Quantity in Stock: ");
        if (quantity < 0) quantity = 0;

        string productID = "P" + intToString(productIDCounter++);

        int idx = productTableHash(productName);
        ProductNode* newProduct = new ProductNode(productID, productName, category, price, quantity);
        newProduct->next = mall->productTable[idx];
        mall->productTable[idx] = newProduct;
        mall->productCount++;

        // Add to hash table for search
        productHash.insert(productName, mall->mallID, newProduct);

        cout << "Product '" << productName << "' added to '" << mall->name
            << "' with ID: " << productID << endl;
    }

    void removeProduct() {
        cout << "\n=== Remove Product from Mall ===" << endl;

        MallNode* mall = selectMall();
        if (mall == nullptr)
            return;

        if (mall->productCount == 0) {
            cout << " No products in this mall!" << endl;
            return;
        }

        // Display products
        cout << "\nProducts in " << mall->name << ":" << endl;
        for (int i = 0; i < MallNode::PRODUCT_TABLE_SIZE; i++) {
            ProductNode* prod = mall->productTable[i];
            while (prod != nullptr) {
                cout << "  - [" << prod->productID << "] " << prod->name
                    << " (" << prod->category << ") - Rs. " << prod->price
                    << " | Stock: " << prod->quantity << endl;
                prod = prod->next;
            }
        }

        string productName = getStringInput("\nEnter Product Name to remove: ");
        ProductNode* product = findProduct(mall, productName);

        if (product == nullptr) {
            cout << " Product not found!" << endl;
            return;
        }

        int idx = productTableHash(productName);


        productHash.remove(productName, mall->mallID);


        if (stringsEqualIgnoreCase(mall->productTable[idx]->name, productName)) {
            ProductNode* temp = mall->productTable[idx];
            mall->productTable[idx] = mall->productTable[idx]->next;
            cout << "Product '" << temp->name << "' removed successfully!" << endl;
            delete temp;
        }
        else {
            ProductNode* current = mall->productTable[idx];
            while (current->next != nullptr) {
                if (stringsEqualIgnoreCase(current->next->name, productName)) {
                    ProductNode* temp = current->next;
                    current->next = current->next->next;
                    cout << "Product '" << temp->name << "' removed successfully!" << endl;
                    delete temp;
                    break;
                }
                current = current->next;
            }
        }

        mall->productCount--;
    }

    void updateProduct() {
        cout << "\n=== Update Product ===" << endl;

        MallNode* mall = selectMall();
        if (mall == nullptr)
            return;

        string productName = getStringInput("Enter Product Name: ");
        ProductNode* product = findProduct(mall, productName);

        if (product == nullptr) {
            cout << " Product not found!" << endl;
            return;
        }

        cout << "\nCurrent Details:" << endl;
        cout << "  ID: " << product->productID << endl;
        cout << "  Name: " << product->name << endl;
        cout << "  Category: " << product->category << endl;
        cout << "  Price: Rs. " << product->price << endl;
        cout << "  Quantity: " << product->quantity << endl;

        cout << "\nWhat would you like to update?" << endl;
        cout << "1. Category" << endl;
        cout << "2. Price" << endl;
        cout << "3. Quantity (Stock)" << endl;
        cout << "4. All of the above" << endl;
        cout << "0. Cancel" << endl;

        int choice = getIntInput("Enter choice: ");

        if (choice == 0) {
            cout << "Update cancelled." << endl;
            return;
        }

        if (choice == 1 || choice == 4) {
            string newCategory = getStringInput("Enter new category (or press Enter to keep current): ");
            if (!newCategory.empty()) {
                product->category = newCategory;
            }
        }

        if (choice == 2 || choice == 4) {
            int newPrice = getIntInput("Enter new price (-1 to keep current): ");
            if (newPrice >= 0) {
                product->price = newPrice;
            }
        }

        if (choice == 3 || choice == 4) {
            int newQuantity = getIntInput("Enter new quantity (-1 to keep current): ");
            if (newQuantity >= 0) {
                product->quantity = newQuantity;
            }
        }

        cout << "Product updated successfully!" << endl;
    }

    void updateStock() {
        cout << "\n=== Update Product Stock ===" << endl;

        MallNode* mall = selectMall();
        if (mall == nullptr)
            return;

        string productName = getStringInput("Enter Product Name: ");
        ProductNode* product = findProduct(mall, productName);

        if (product == nullptr) {
            cout << " Product not found!" << endl;
            return;
        }

        cout << "Current Stock: " << product->quantity << endl;
        cout << "\n1. Set new quantity" << endl;
        cout << "2. Add to stock" << endl;
        cout << "3. Remove from stock" << endl;

        int choice = getIntInput("Enter choice: ");

        if (choice == 1) {
            int newQty = getIntInput("Enter new quantity: ");
            if (newQty >= 0) {
                product->quantity = newQty;
                cout << "Stock updated to " << newQty << "!" << endl;
            }
        }
        else if (choice == 2) {
            int addQty = getIntInput("Enter quantity to add: ");
            if (addQty > 0) {
                product->quantity += addQty;
                cout << "Added " << addQty << " units.  New stock: " << product->quantity << endl;
            }
        }
        else if (choice == 3) {
            int removeQty = getIntInput("Enter quantity to remove: ");
            if (removeQty > 0 && removeQty <= product->quantity) {
                product->quantity -= removeQty;
                cout << "Removed " << removeQty << " units. New stock: " << product->quantity << endl;
            }
            else if (removeQty > product->quantity) {
                cout << " Cannot remove more than available stock!" << endl;
            }
        }
    }


    void searchProduct() {
        cout << "\n=== Search Product ===" << endl;
        string query = getStringInput("Enter product name to search: ");
        productHash.search(query);
    }

    void searchByCategory() {
        cout << "\n=== Search by Category ===" << endl;
        string category = getStringInput("Enter category to search: ");
        productHash.searchByCategory(category);
    }

    void findNearestMall() {
        cout << "\n=== Find Nearest Mall ===" << endl;

        if (mallsHead == nullptr) {
            cout << "No malls registered." << endl;
            return;
        }

        double userLat = getDoubleInput("Enter your Latitude: ");
        double userLon = getDoubleInput("Enter your Longitude: ");

        MallNode* nearest = nullptr;
        double minDist = DBL_MAX;

        MallNode* current = mallsHead;
        while (current != nullptr) {
            double dist = calculateDistance(userLat, userLon, current->lat, current->lon);
            if (dist < minDist) {
                minDist = dist;
                nearest = current;
            }
            current = current->next;
        }

        if (nearest != nullptr) {
            cout << "\n=== Nearest Mall ===" << endl;
            cout << "Name: " << nearest->name << endl;
            cout << "Sector: " << nearest->sector << endl;
            cout << "Timings: " << nearest->openingTime << " - " << nearest->closingTime << endl;
            cout << "Products: " << nearest->productCount << endl;
            cout << "Distance: " << minDist << " units" << endl;
        }
    }

    void findMallWithProduct() {
        cout << "\n=== Find Mall with Product ===" << endl;

        string productName = getStringInput("Enter Product Name: ");
        double userLat = getDoubleInput("Enter your Latitude: ");
        double userLon = getDoubleInput("Enter your Longitude: ");

        MallNode* nearestMall = nullptr;
        ProductNode* foundProduct = nullptr;
        double minDist = DBL_MAX;

        MallNode* mall = mallsHead;
        while (mall != nullptr) {
            ProductNode* product = findProduct(mall, productName);
            if (product != nullptr && product->quantity > 0) {
                double dist = calculateDistance(userLat, userLon, mall->lat, mall->lon);
                if (dist < minDist) {
                    minDist = dist;
                    nearestMall = mall;
                    foundProduct = product;
                }
            }
            mall = mall->next;
        }

        if (nearestMall != nullptr && foundProduct != nullptr) {
            cout << "\n=== Nearest Mall with '" << productName << "' ===" << endl;
            cout << "Mall: " << nearestMall->name << " (" << nearestMall->sector << ")" << endl;
            cout << "Distance: " << minDist << " units" << endl;
            cout << "\nProduct Details:" << endl;
            cout << "  Name: " << foundProduct->name << endl;
            cout << "  Category: " << foundProduct->category << endl;
            cout << "  Price: Rs. " << foundProduct->price << endl;
            cout << "  In Stock: " << foundProduct->quantity << endl;
        }
        else {
            cout << "Product '" << productName << "' not found in any mall or out of stock." << endl;
        }
    }


    void findShortestDeliveryPath() {
        cout << "\n=== Find Shortest Delivery Path (Dijkstra's Algorithm) ===" << endl;

        if (mallCount < 2) {
            cout << " Need at least 2 malls." << endl;
            return;
        }

        string srcName = getStringInput("Enter Source Mall Name: ");
        MallNode* srcMall = findMallByName(srcName);
        if (srcMall == nullptr) {
            cout << " Source mall not found!" << endl;
            return;
        }

        string destName = getStringInput("Enter Destination Mall Name: ");
        MallNode* destMall = findMallByName(destName);
        if (destMall == nullptr) {
            cout << " Destination mall not found!" << endl;
            return;
        }

        if (stringsEqualIgnoreCase(srcMall->mallID, destMall->mallID)) {
            cout << "Source and destination are the same!" << endl;
            return;
        }


        const int MAX_MALLS = 500;
        string mallIDs[MAX_MALLS];
        double distances[MAX_MALLS];
        string previous[MAX_MALLS];
        bool visited[MAX_MALLS];
        int numMalls = 0;

        // Collect all mall IDs
        MallNode* current = mallsHead;
        while (current != nullptr && numMalls < MAX_MALLS) {
            mallIDs[numMalls] = current->mallID;
            distances[numMalls] = DBL_MAX;
            previous[numMalls] = "";
            visited[numMalls] = false;
            numMalls++;
            current = current->next;
        }


        int srcIdx = -1;
        for (int i = 0; i < numMalls; i++) {
            if (stringsEqualIgnoreCase(mallIDs[i], srcMall->mallID)) {
                srcIdx = i;
                break;
            }
        }
        if (srcIdx == -1) {
            cout << " Source mall not in graph!" << endl;
            return;
        }

        distances[srcIdx] = 0;

        // Dijkstras algaram
        CommercialDijkstraHeap minHeap;
        minHeap.insert(srcMall->mallID, 0);

        while (!minHeap.isEmpty()) {
            CommercialDijkstraNode minNode = minHeap.extractMin();
            string currentID = minNode.mallID;

            int currentIdx = -1;
            for (int i = 0; i < numMalls; i++) {
                if (stringsEqualIgnoreCase(mallIDs[i], currentID)) {
                    currentIdx = i;
                    break;
                }
            }

            if (currentIdx == -1 || visited[currentIdx])
                continue;
            visited[currentIdx] = true;

            // Process neighbors
            MallAdjNode* adj = getAdjList(currentID);
            if (adj != nullptr) {
                MallEdge* edge = adj->edgeHead;
                while (edge != nullptr) {
                    int neighborIdx = -1;
                    for (int i = 0; i < numMalls; i++) {
                        if (stringsEqualIgnoreCase(mallIDs[i], edge->destMallID)) {
                            neighborIdx = i;
                            break;
                        }
                    }

                    if (neighborIdx != -1 && !visited[neighborIdx]) {
                        double newDist = distances[currentIdx] + edge->distance;
                        if (newDist < distances[neighborIdx]) {
                            distances[neighborIdx] = newDist;
                            previous[neighborIdx] = currentID;
                            minHeap.insert(edge->destMallID, newDist);
                        }
                    }
                    edge = edge->next;
                }
            }
        }

        // Find destination index
        int destIdx = -1;
        for (int i = 0; i < numMalls; i++) {
            if (stringsEqualIgnoreCase(mallIDs[i], destMall->mallID)) {
                destIdx = i;
                break;
            }
        }

        if (destIdx == -1 || distances[destIdx] == DBL_MAX) {
            cout << "\nNo delivery path exists between '" << srcMall->name
                << "' and '" << destMall->name << "'!" << endl;
            cout << "Tip: Make sure the malls are connected via 'Connect Malls' option." << endl;
            return;
        }

        // Reconstruct path
        string path[MAX_MALLS];
        int pathLen = 0;
        string currentID = destMall->mallID;

        while (!currentID.empty() && pathLen < MAX_MALLS) {
            path[pathLen++] = currentID;
            int idx = -1;
            for (int i = 0; i < numMalls; i++) {
                if (stringsEqualIgnoreCase(mallIDs[i], currentID)) {
                    idx = i;
                    break;
                }
            }
            if (idx == -1) break;
            currentID = previous[idx];
        }

        // Display path
        cout << "\n=== Shortest Delivery Path Found ===" << endl;
        cout << "From: " << srcMall->name << endl;
        cout << "To: " << destMall->name << endl;
        cout << "Total Distance: " << distances[destIdx] << " units" << endl;
        cout << "\nPath: ";

        for (int i = pathLen - 1; i >= 0; i--) {
            MallNode* mall = findMallByID(path[i]);
            if (mall != nullptr) {
                cout << mall->name;
                if (i > 0) cout << " -> ";
            }
        }
        cout << endl;
    }



    void displayAllMalls() {
        cout << "\n=== All Malls ===" << endl;
        MallNode* current = mallsHead;
        int count = 0;
        while (current != nullptr) {
            cout << ++count << ". [" << current->mallID << "] " << current->name
                << " (" << current->sector << ")" << endl;
            cout << "   Timings: " << current->openingTime << " - " << current->closingTime
                << " | Products: " << current->productCount << endl;
            current = current->next;
        }
        if (count == 0) {
            cout << "No malls registered." << endl;
        }
    }

    void displayMallDetails() {
        cout << "\n=== Mall Details ===" << endl;

        MallNode* mall = selectMall();
        if (mall == nullptr)
            return;

        cout << "\n========================================" << endl;
        cout << "Mall: " << mall->name << endl;
        cout << "ID: " << mall->mallID << endl;
        cout << "Sector: " << mall->sector << endl;
        cout << "Location: (" << mall->lat << ", " << mall->lon << ")" << endl;
        cout << "Timings: " << mall->openingTime << " - " << mall->closingTime << endl;
        cout << "========================================" << endl;
        cout << "Products (" << mall->productCount << "):" << endl;

        bool hasProducts = false;
        for (int i = 0; i < MallNode::PRODUCT_TABLE_SIZE; i++) {
            ProductNode* prod = mall->productTable[i];
            while (prod != nullptr) {
                cout << "  - [" << prod->productID << "] " << prod->name
                    << " (" << prod->category << ")" << endl;
                cout << "    Price: Rs. " << prod->price
                    << " | Stock: " << prod->quantity << endl;
                hasProducts = true;
                prod = prod->next;
            }
        }

        if (!hasProducts) {
            cout << "  (No products)" << endl;
        }

        // Display connections
        cout << "\n--- Connected Malls ---" << endl;
        MallAdjNode* adj = getAdjList(mall->mallID);
        if (adj != nullptr && adj->edgeHead != nullptr) {
            MallEdge* edge = adj->edgeHead;
            while (edge != nullptr) {
                MallNode* connectedMall = findMallByID(edge->destMallID);
                if (connectedMall != nullptr) {
                    cout << "  -> " << connectedMall->name
                        << " (Distance: " << edge->distance << ")" << endl;
                }
                edge = edge->next;
            }
        }
        else {
            cout << "  (No connections)" << endl;
        }
    }

    void displayMallProducts() {
        cout << "\n=== Display Mall Products ===" << endl;

        MallNode* mall = selectMall();
        if (mall == nullptr)
            return;

        cout << "\n--- Products at " << mall->name << " ---" << endl;

        if (mall->productCount == 0) {
            cout << "(No products)" << endl;
            return;
        }

        // Group by category
        string categories[50];
        int categoryCount = 0;

        for (int i = 0; i < MallNode::PRODUCT_TABLE_SIZE; i++) {
            ProductNode* prod = mall->productTable[i];
            while (prod != nullptr) {
                bool exists = false;
                for (int j = 0; j < categoryCount; j++) {
                    if (stringsEqualIgnoreCase(categories[j], prod->category)) {
                        exists = true;
                        break;
                    }
                }
                if (!exists && categoryCount < 50) {
                    categories[categoryCount++] = prod->category;
                }
                prod = prod->next;
            }
        }

        for (int c = 0; c < categoryCount; c++) {
            cout << "\n[" << categories[c] << "]" << endl;
            for (int i = 0; i < MallNode::PRODUCT_TABLE_SIZE; i++) {
                ProductNode* prod = mall->productTable[i];
                while (prod != nullptr) {
                    if (stringsEqualIgnoreCase(prod->category, categories[c])) {
                        cout << "  - " << prod->name << " | Rs. " << prod->price
                            << " | Stock: " << prod->quantity << endl;
                    }
                    prod = prod->next;
                }
            }
        }
    }

    void displayMallNetwork() {
        cout << "\n=== Mall Network (Adjacency List) ===" << endl;

        MallAdjNode* adj = adjListHead;
        while (adj != nullptr) {
            MallNode* mall = findMallByID(adj->mallID);
            cout << "[" << (mall ? mall->name : adj->mallID) << "] -> ";

            MallEdge* edge = adj->edgeHead;
            if (edge == nullptr) {
                cout << "(no connections)";
            }
            while (edge != nullptr) {
                MallNode* dest = findMallByID(edge->destMallID);
                cout << (dest ? dest->name : edge->destMallID)
                    << "(" << edge->distance << ")";
                if (edge->next != nullptr) cout << ", ";
                edge = edge->next;
            }
            cout << endl;
            adj = adj->next;
        }

        if (adjListHead == nullptr) {
            cout << "(No malls registered)" << endl;
        }
    }

    void displayStatistics() {
        cout << "\n=== Commercial System Statistics ===" << endl;
        cout << "Total Malls: " << mallCount << endl;
        cout << "Total Products: " << getTotalProducts() << endl;
        cout << "Total Connections: " << getConnectionCount() << endl;

        if (mallCount > 0) {
            cout << "Average Products per Mall: " << (double)getTotalProducts() / mallCount << endl;
        }

        // Find mall with most products
        MallNode* topMall = nullptr;
        int maxProducts = 0;
        MallNode* mall = mallsHead;
        while (mall != nullptr) {
            if (mall->productCount > maxProducts) {
                maxProducts = mall->productCount;
                topMall = mall;
            }
            mall = mall->next;
        }

        if (topMall != nullptr) {
            cout << "Mall with Most Products: " << topMall->name
                << " (" << maxProducts << " products)" << endl;
        }
    }

    void displayLowStockProducts() {
        cout << "\n=== Low Stock Products (< 10 units) ===" << endl;

        bool found = false;
        MallNode* mall = mallsHead;
        while (mall != nullptr) {
            for (int i = 0; i < MallNode::PRODUCT_TABLE_SIZE; i++) {
                ProductNode* prod = mall->productTable[i];
                while (prod != nullptr) {
                    if (prod->quantity < 10) {
                        cout << "- " << prod->name << " at " << mall->name
                            << " | Stock: " << prod->quantity << endl;
                        found = true;
                    }
                    prod = prod->next;
                }
            }
            mall = mall->next;
        }

        if (!found) {
            cout << "No low stock products found." << endl;
        }
    }


    void loadFromFiles() {
        GlobalLocationManager* locMgr = GlobalLocationManager::getInstance();

        auto normalizeSeparators = [](string& s) {
            for (size_t i = 0; i < s.length(); i++) {
                if (s[i] == '\t') s[i] = ',';
            }
            };

        // Load malls
        ifstream mallFile("malls.csv");
        if (mallFile.is_open()) {
            string line;
            getline(mallFile, line); // Skip header
            int loadedCount = 0;

            while (getline(mallFile, line)) {
                if (line.empty()) continue;
                normalizeSeparators(line);

                string parts[6];
                int count = splitString(line, ',', parts, 6);

                if (count >= 5) {
                    string id = trimString(parts[0]);
                    string name = trimString(parts[1]);
                    string sector = trimString(parts[2]);
                    string openTime = trimString(parts[3]);
                    string closeTime = trimString(parts[4]);

                    if (findMallByID(id) != nullptr || findMallByName(name) != nullptr) {
                        continue;
                    }

                    // Generate coordinates based on sector
                    double lat, lon;
                    double mallOffset = 0.005;

                    if (sector == "F-10") { lat = 33.691 + mallOffset; lon = 73.019 + mallOffset; }
                    else if (sector == "Blue Area") { lat = 33.720 + mallOffset; lon = 73.065 + mallOffset; }
                    else if (sector == "F-6") { lat = 33.727 + mallOffset; lon = 73.075 + mallOffset; }
                    else if (sector == "G-9") { lat = 33.700 + mallOffset; lon = 73.030 + mallOffset; }
                    else if (sector == "I-8") { lat = 33.680 + mallOffset; lon = 73.050 + mallOffset; }
                    else {
                        lat = 33.700 + (loadedCount * 0.003) + mallOffset;
                        lon = 73.000 + (loadedCount * 0.003) + mallOffset;
                    }

                    if (!locMgr->registerLocation(lat, lon, "Mall", name)) {
                        lat += 0.001;
                        lon += 0.001;
                        if (!locMgr->registerLocation(lat, lon, "Mall", name)) {
                            continue;
                        }
                    }

                    MallNode* newMall = new MallNode(id, name, sector, lat, lon, openTime, closeTime);
                    newMall->next = mallsHead;
                    mallsHead = newMall;
                    mallCount++;

                    getOrCreateAdjList(id);

                    // Register with CityGraph for unified city-wide graph
                    CityGraph::getInstance()->addLocation(id, name, LOC_MALL, lat, lon, newMall);

                    loadedCount++;
                }
            }
            mallFile.close();
            cout << "Loaded " << loadedCount << " malls from file." << endl;
        }
        else {
            cout << "Warning: malls.csv not found." << endl;
        }

        // Load products
        ifstream prodFile("products.csv");
        if (prodFile.is_open()) {
            string line;
            getline(prodFile, line);
            int loadedCount = 0;

            while (getline(prodFile, line)) {
                if (line.empty())
                    continue;
                normalizeSeparators(line);

                string parts[6];
                int count = splitString(line, ',', parts, 6);

                if (count >= 6) {
                    string mallID = trimString(parts[0]);
                    string prodID = trimString(parts[1]);
                    string prodName = trimString(parts[2]);
                    string category = trimString(parts[3]);
                    int price = stringToInt(trimString(parts[4]));
                    int quantity = stringToInt(trimString(parts[5]));

                    MallNode* mall = findMallByID(mallID);
                    if (mall == nullptr) continue;

                    if (findProduct(mall, prodName) != nullptr) continue;

                    int idx = productTableHash(prodName);
                    ProductNode* newProduct = new ProductNode(prodID, prodName, category, price, quantity);
                    newProduct->next = mall->productTable[idx];
                    mall->productTable[idx] = newProduct;
                    mall->productCount++;

                    productHash.insert(prodName, mallID, newProduct);
                    loadedCount++;
                }
            }
            prodFile.close();
            cout << "Loaded " << loadedCount << " products from file." << endl;
        }
        else {
            cout << "Warning: products.csv not found." << endl;
        }
    }


    int getMallCount() {
        return mallCount;
    }


    ~CommercialSystem() {
        // Delete all malls
        while (mallsHead != nullptr) {
            MallNode* mallTemp = mallsHead;
            mallsHead = mallsHead->next;

            // Delete all products
            for (int i = 0; i < MallNode::PRODUCT_TABLE_SIZE; i++) {
                while (mallTemp->productTable[i] != nullptr) {
                    ProductNode* prodTemp = mallTemp->productTable[i];
                    mallTemp->productTable[i] = mallTemp->productTable[i]->next;
                    delete prodTemp;
                }
            }
            delete mallTemp;
        }

        // Delete adjacency list
        while (adjListHead != nullptr) {
            MallAdjNode* adjTemp = adjListHead;
            adjListHead = adjListHead->next;

            while (adjTemp->edgeHead != nullptr) {
                MallEdge* edgeTemp = adjTemp->edgeHead;
                adjTemp->edgeHead = adjTemp->edgeHead->next;
                delete edgeTemp;
            }
            delete adjTemp;
        }
    }
};