/*
 * ============================================================
 *   PARKING MANAGEMENT SYSTEM
 *   CC 103 - Data Structures and Algorithms
 *   Batangas State University - Alangilan Campus
 *   2nd Semester, AY 2025-2026
 *
 *   Team Number : 3
 *   Team Name   : DoLiCol
 *   Members:
 *     - Dorado, John Leonard S.   (Programmer, Tester)
 *     - Galido, Renz Moen S.      (Team Leader, Main Programmer)
 *     - Palacol, Jana Beatrice T. (Documenter, System Architect)
 * ============================================================
 *
 *  Data Structures Used:
 *    Stack          -> Undo last parking action (LIFO)
 *    Queue          -> Regular vehicle waiting list (FIFO)
 *    Priority Queue -> PWD / Reserved vehicle waiting list
 *
 *  Concepts Demonstrated:
 *    Iterative  -> viewParkedVehicles() uses a for-loop
 *    Recursive  -> viewWaitingList() uses recursion on a copy
 *    OOP        -> Classes with private attributes & public methods
 */

#include <iostream>
#include <queue>
#include <stack>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include <limits>

using namespace std;

// ============================================================
//  CONSTANTS
// ============================================================
const int MAX_SLOTS = 10; // Fixed-size parking lot

// ============================================================
//  CLASS: Vehicle
//  Stores all info about one vehicle in the system
// ============================================================
class Vehicle {
private:
    string plateNumber;
    string ownerName;
    int    priorityLevel; // 1 = PWD/Pregnant, 2 = Reserved, 3 = Regular
    string vehicleType;

public:
    // Default constructor (needed by some STL containers)
    Vehicle() : plateNumber(""), ownerName(""), priorityLevel(3), vehicleType("Regular") {}

    Vehicle(string plate, string owner, int priority, string type)
        : plateNumber(plate), ownerName(owner),
          priorityLevel(priority), vehicleType(type) {}

    // --- Getters ---
    string getPlate()    const { return plateNumber; }
    string getOwner()    const { return ownerName; }
    int    getPriority() const { return priorityLevel; }
    string getType()     const { return vehicleType; }

    // For priority_queue: lower number = higher priority
    bool operator>(const Vehicle& other) const {
        return priorityLevel > other.priorityLevel;
    }

    // Display one vehicle's info in a table row
    void display(int slotNum = -1) const {
        if (slotNum != -1)
            cout << "  " << left << setw(6) << slotNum;
        cout << left
             << setw(14) << plateNumber
             << setw(22) << ownerName
             << setw(14) << vehicleType
             << "\n";
    }
};

// ============================================================
//  CLASS: ParkingSlot
//  Represents one physical parking space
// ============================================================
class ParkingSlot {
private:
    int    slotNumber;
    bool   isOccupied;
    string occupantPlate;

public:
    ParkingSlot(int num)
        : slotNumber(num), isOccupied(false), occupantPlate("") {}

    // --- Getters ---
    int    getSlotNumber()    const { return slotNumber; }
    bool   getIsOccupied()    const { return isOccupied; }
    string getOccupantPlate() const { return occupantPlate; }

    void occupy(const string& plate) {
        isOccupied    = true;
        occupantPlate = plate;
    }

    void vacate() {
        isOccupied    = false;
        occupantPlate = "";
    }
};

// ============================================================
//  CLASS: ParkingSystem
//  Main controller — manages all operations
// ============================================================
class ParkingSystem {
private:
    // --- Data Structures ---
    vector<ParkingSlot> slots;       // Fixed-size slot array (index = slot# - 1)
    queue<Vehicle>      regularQueue;     // FIFO: regular vehicles
    priority_queue<Vehicle,
        vector<Vehicle>,
        greater<Vehicle>> priorityQueue;  // Priority: PWD / Reserved

    // Stack stores <plateNumber, slotNumber> for undo
    stack<pair<string, int>> undoStack;

    int totalSlots;
    int availableSlots;

    // --- Private Helpers ---

    // Scan for first available slot (sequential assignment)
    int findAvailableSlot() {
        for (auto& slot : slots) {
            if (!slot.getIsOccupied())
                return slot.getSlotNumber();
        }
        return -1; // Lot is full
    }

    // Find which slot a plate number is in (-1 if not found)
    int findSlotByPlate(const string& plate) {
        string upper = plate;
        transform(upper.begin(), upper.end(), upper.begin(), ::toupper);
        for (auto& slot : slots) {
            string slotPlate = slot.getOccupantPlate();
            transform(slotPlate.begin(), slotPlate.end(), slotPlate.begin(), ::toupper);
            if (slotPlate == upper) return slot.getSlotNumber();
        }
        return -1;
    }

    // Assign a vehicle to a slot and record it on the undo stack
    void assignSlot(const Vehicle& v, int slotNum) {
        slots[slotNum - 1].occupy(v.getPlate());
        availableSlots--;
        undoStack.push({ v.getPlate(), slotNum });

        cout << "\n  [ASSIGNED] " << v.getOwner()
             << " (" << v.getPlate() << ") -> Slot #" << slotNum << "\n";
    }

    // RECURSIVE helper: display a copy of a queue without modifying the original
    void displayQueueRecursive(queue<Vehicle> q, int position) {
        if (q.empty()) return; // Base case

        Vehicle v = q.front();
        q.pop();
        cout << "  " << setw(4) << position << ". ";
        v.display();

        displayQueueRecursive(q, position + 1); // Recursive call
    }

    // Input helper: flush cin on bad input
    void clearInput() {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    // Prompt for vehicle type and return priority level
    int getVehicleType(string& typeOut) {
        int choice;
        cout << "\n  Vehicle Type:\n";
        cout << "    [1] Regular\n";
        cout << "    [2] PWD / Pregnant\n";
        cout << "    [3] Reserved\n";
        cout << "  Choice: ";
        cin >> choice;
        clearInput();

        switch (choice) {
            case 1: typeOut = "Regular";      return 3;
            case 2: typeOut = "PWD/Pregnant"; return 1;
            case 3: typeOut = "Reserved";     return 2;
            default:
                cout << "  Invalid type. Defaulting to Regular.\n";
                typeOut = "Regular"; return 3;
        }
    }

public:
    // Constructor: build the slot array
    ParkingSystem(int numSlots)
        : totalSlots(numSlots), availableSlots(numSlots) {
        for (int i = 1; i <= numSlots; i++)
            slots.push_back(ParkingSlot(i));
    }

    // ==============================================================
    //  FEATURE 1: Park Vehicle
    //  Assigns slot immediately, or adds to the appropriate queue
    // ==============================================================
    void parkVehicle() {
        cout << "\n  ---- PARK VEHICLE ----\n";

        string plate, owner, type;
        cout << "  Plate Number : "; cin >> plate;
        clearInput();

        // Check duplicate
        if (findSlotByPlate(plate) != -1) {
            cout << "  Vehicle " << plate << " is already parked.\n";
            return;
        }

        cout << "  Owner Name   : "; getline(cin, owner);
        int priority = getVehicleType(type);

        Vehicle v(plate, owner, priority, type);
        int slotNum = findAvailableSlot();

        if (slotNum != -1) {
            // Slot available — assign immediately
            assignSlot(v, slotNum);
        } else {
            // Lot full — join queue
            if (priority < 3) {
                priorityQueue.push(v);
                cout << "  Lot is full. Added to PRIORITY waiting queue.\n";
            } else {
                regularQueue.push(v);
                cout << "  Lot is full. Added to REGULAR waiting queue.\n";
            }
        }
    }

    // ==============================================================
    //  FEATURE 2: Remove Vehicle
    //  Vehicle exits; frees slot and serves next in queue
    // ==============================================================
    void removeVehicle() {
        cout << "\n  ---- REMOVE VEHICLE ----\n";

        string plate;
        cout << "  Enter Plate Number of exiting vehicle: "; cin >> plate;
        clearInput();

        int slotNum = findSlotByPlate(plate);
        if (slotNum == -1) {
            cout << "  Vehicle not found in any slot.\n";
            return;
        }

        slots[slotNum - 1].vacate();
        availableSlots++;
        cout << "  [EXIT] " << plate << " has left Slot #" << slotNum << ".\n";

        // Immediately serve next waiting vehicle if any
        if (!priorityQueue.empty()) {
            Vehicle next = priorityQueue.top();
            priorityQueue.pop();
            cout << "  Serving next from PRIORITY queue...\n";
            assignSlot(next, slotNum);
        } else if (!regularQueue.empty()) {
            Vehicle next = regularQueue.front();
            regularQueue.pop();
            cout << "  Serving next from REGULAR queue...\n";
            assignSlot(next, slotNum);
        }
    }

    // ==============================================================
    //  FEATURE 3: Undo Last Action (STACK)
    //  Pops the stack and vacates the most recently assigned slot
    // ==============================================================
    void undoLastAction() {
        cout << "\n  ---- UNDO LAST ACTION ----\n";

        if (undoStack.empty()) {
            cout << "  Nothing to undo.\n";
            return;
        }

        auto [plate, slotNum] = undoStack.top();
        undoStack.pop();

        // Only undo if the vehicle is still in that slot
        if (slots[slotNum - 1].getOccupantPlate() == plate) {
            slots[slotNum - 1].vacate();
            availableSlots++;
            cout << "  [UNDO] " << plate
                 << " removed from Slot #" << slotNum << ".\n";
        } else {
            cout << "  [UNDO] " << plate
                 << " has already exited Slot #" << slotNum
                 << ". No change made.\n";
        }
    }

    // ==============================================================
    //  FEATURE 4: View Parked Vehicles (ITERATIVE)
    //  Uses a for-loop to walk through the slots vector
    // ==============================================================
    void viewParkedVehicles() {
        cout << "\n  ---- PARKED VEHICLES (Iterative) ----\n";
        cout << "  " << string(56, '-') << "\n";
        cout << "  " << left
             << setw(6)  << "Slot"
             << setw(14) << "Plate"
             << setw(22) << "Owner"
             << setw(14) << "Type" << "\n";
        cout << "  " << string(56, '-') << "\n";

        bool anyParked = false;

        // ITERATIVE: simple for-loop over the slots vector
        for (int i = 0; i < (int)slots.size(); i++) {
            if (slots[i].getIsOccupied()) {
                cout << "  " << left << setw(6) << slots[i].getSlotNumber()
                     << setw(14) << slots[i].getOccupantPlate() << "\n";
                anyParked = true;
            }
        }

        if (!anyParked)
            cout << "  No vehicles currently parked.\n";

        cout << "  " << string(56, '-') << "\n";
        cout << "  Available Slots: " << availableSlots
             << " / " << totalSlots << "\n";
    }

    // ==============================================================
    //  FEATURE 5: View Waiting List (RECURSIVE)
    //  Calls recursive helper on a COPY of each queue
    // ==============================================================
    void viewWaitingList() {
        cout << "\n  ---- WAITING LIST ----\n";

        // --- Priority Queue ---
        cout << "\n  >> PRIORITY QUEUE (PWD / Reserved):\n";
        if (priorityQueue.empty()) {
            cout << "     (empty)\n";
        } else {
            cout << "  " << left
                 << setw(6)  << "No."
                 << setw(14) << "Plate"
                 << setw(22) << "Owner"
                 << setw(14) << "Type" << "\n";
            cout << "  " << string(56, '-') << "\n";

            // Copy so original is not modified
            priority_queue<Vehicle, vector<Vehicle>, greater<Vehicle>> pqCopy = priorityQueue;
            int pos = 1;
            while (!pqCopy.empty()) {
                Vehicle v = pqCopy.top(); pqCopy.pop();
                cout << "  " << setw(4) << pos++ << ". ";
                v.display();
            }
        }

        // --- Regular Queue (RECURSIVE display) ---
        cout << "\n  >> REGULAR QUEUE (FIFO - Recursive Display):\n";
        if (regularQueue.empty()) {
            cout << "     (empty)\n";
        } else {
            cout << "  " << left
                 << setw(6)  << "No."
                 << setw(14) << "Plate"
                 << setw(22) << "Owner"
                 << setw(14) << "Type" << "\n";
            cout << "  " << string(56, '-') << "\n";

            // Pass a COPY to recursive function — original is unchanged
            displayQueueRecursive(regularQueue, 1);
        }
    }

    // ==============================================================
    //  FEATURE 6: Search Vehicle (Linear Search)
    //  Scans all slots for a matching plate number
    // ==============================================================
    void searchVehicle() {
        cout << "\n  ---- SEARCH VEHICLE ----\n";
        string plate;
        cout << "  Enter Plate Number: "; cin >> plate;
        clearInput();

        // Normalize to uppercase for comparison
        string upper = plate;
        transform(upper.begin(), upper.end(), upper.begin(), ::toupper);

        bool found = false;

        // Linear search through all slots
        for (auto& slot : slots) {
            string slotPlate = slot.getOccupantPlate();
            transform(slotPlate.begin(), slotPlate.end(), slotPlate.begin(), ::toupper);
            if (slotPlate == upper) {
                cout << "  Found! " << plate
                     << " is parked at Slot #" << slot.getSlotNumber() << ".\n";
                found = true;
                break;
            }
        }

        if (!found)
            cout << "  Vehicle \"" << plate << "\" not found in any slot.\n";
    }

    // ==============================================================
    //  FEATURE 7: View All Slots (Sorted by Slot Number)
    //  Slots are stored in order, so no extra sorting needed
    // ==============================================================
    void viewAllSlots() {
        cout << "\n  ---- ALL PARKING SLOTS (Sorted by Slot #) ----\n";
        cout << "  " << string(38, '-') << "\n";
        cout << "  " << left
             << setw(8)  << "Slot"
             << setw(16) << "Plate"
             << setw(10) << "Status" << "\n";
        cout << "  " << string(38, '-') << "\n";

        for (auto& slot : slots) {
            cout << "  " << left
                 << setw(8)  << slot.getSlotNumber()
                 << setw(16) << (slot.getIsOccupied() ? slot.getOccupantPlate() : "---")
                 << setw(10) << (slot.getIsOccupied() ? "OCCUPIED" : "FREE")
                 << "\n";
        }
        cout << "  " << string(38, '-') << "\n";
        cout << "  Total: " << totalSlots
             << "  |  Occupied: " << (totalSlots - availableSlots)
             << "  |  Free: "    << availableSlots << "\n";
    }
};

// ============================================================
//  MAIN FUNCTION
// ============================================================
int main() {
    cout << "\n";
    cout << "  =====================================================\n";
    cout << "         PARKING MANAGEMENT SYSTEM\n";
    cout << "         CC 103 - Data Structures & Algorithms\n";
    cout << "         Batangas State University - Alangilan\n";
    cout << "         Team DoLiCol  |  Group 3\n";
    cout << "  =====================================================\n";

    cout << "\n  Enter total number of parking slots (max " << MAX_SLOTS << "): ";
    int numSlots;
    cin >> numSlots;

    if (numSlots <= 0 || numSlots > MAX_SLOTS) {
        cout << "  Invalid number of slots. Using default of " << MAX_SLOTS << ".\n";
        numSlots = MAX_SLOTS;
    }

    ParkingSystem system(numSlots);

    int choice;
    do {
        cout << "\n";
        cout << "  =====================================================\n";
        cout << "                      MAIN MENU\n";
        cout << "  =====================================================\n";
        cout << "   [1] Park Vehicle\n";
        cout << "   [2] Remove Vehicle\n";
        cout << "   [3] Undo Last Action          (Stack - LIFO)\n";
        cout << "   [4] View Parked Vehicles      (Iterative)\n";
        cout << "   [5] View Waiting List         (Recursive)\n";
        cout << "   [6] Search Vehicle\n";
        cout << "   [7] View All Slots\n";
        cout << "   [8] Exit\n";
        cout << "  =====================================================\n";
        cout << "   Enter choice: ";
        cin >> choice;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            choice = -1;
        }

        switch (choice) {
            case 1: system.parkVehicle();      break;
            case 2: system.removeVehicle();    break;
            case 3: system.undoLastAction();   break;
            case 4: system.viewParkedVehicles(); break;
            case 5: system.viewWaitingList();  break;
            case 6: system.searchVehicle();    break;
            case 7: system.viewAllSlots();     break;
            case 8: cout << "\n  Exiting system. Goodbye!\n\n"; break;
            default: cout << "\n  Invalid choice. Please enter 1-8.\n";
        }

    } while (choice != 8);

    return 0;
}