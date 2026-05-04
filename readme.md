# 🚗 Parking Management System
 
**CC 103 – Data Structures and Algorithms**
Batangas State University – Alangilan Campus
College of Informatics and Computing Sciences
2nd Semester, AY 2025–2026
 
---
 
## 📌 Project Title
 
**Parking Management System**
 
---
 
## 👥 Team Details
 
| Field | Details |
|---|---|
| Team Number | 3 |
| Team Name | DoLiCol |
 
| SR-Code | Name | Duties / Responsibilities |
|---|---|---|
| 25-05950 | Dorado, John Leonard S. | Programmer, Tester |
| 25-05093 | Galido, Renz Moen S. | Team Leader, Main Programmer |
| 25-09379 | Palacol, Jana Beatrice T. | Documenter, System Architect |
 
---
 
## 🧩 Problem Description
 
Managing a parking lot involves challenges such as tracking available slots, fairly queuing vehicles that are waiting, prioritizing special vehicles (PWD, pregnant women, reserved), and allowing attendants to undo incorrect entries. This system solves these problems using appropriate data structures to simulate a real-world parking lot in a C++ console application.
 
---
 
## 💡 Proposed Solution / Program Description
 
### What the program does (Features)
- Park a vehicle into an available slot, or add it to a waiting queue if the lot is full
- Assign slots automatically using FIFO (Queue) for regular vehicles
- Prioritize PWD, pregnant, and reserved vehicles using a Priority Queue
- Allow parking attendants to undo the last parking action using a Stack
- Display all currently parked vehicles using an **iterative** approach
- Display the waiting list using a **recursive** approach
- Search for a vehicle by plate number
- View all slots sorted by slot number

### Who will use it
Parking lot attendants and administrators.
 
### How users interact with it
Through a numbered menu-driven console interface (options 1–8).
 
---
 
## 🗂️ Data Structures Used
 
### 1. Stack — Undo Last Action
**What it is:** A Stack follows the **LIFO** (Last In, First Out) principle. The most recently added item is the first to be removed — like a stack of plates.
 
**Why we used it:** When a parking attendant makes an incorrect entry (wrong plate, wrong slot), they need to reverse the most recent action. The Stack records every parking assignment, and popping it undoes the most recent one — exactly what LIFO is designed for.
 
**In the code:** `stack<pair<string, int>> undoStack` — stores `<plateNumber, slotNumber>` for every assignment.
 
---
 
### 2. Queue — Regular Vehicle Waiting List
**What it is:** A Queue follows the **FIFO** (First In, First Out) principle — the first vehicle to arrive is the first to be served, just like a physical line.
 
**Why we used it:** When the lot is full and a regular vehicle arrives, it joins the end of the waiting line. When a slot opens, the car that has been waiting the longest gets it first. This is fair and mirrors how real parking lots operate.
 
**In the code:** `queue<Vehicle> regularQueue` — stores regular vehicles waiting for a slot.
 
---
 
### 3. Priority Queue — PWD / Reserved Waiting List
**What it is:** A Priority Queue is similar to a Queue, but elements are served based on their **priority level** rather than arrival order. Higher-priority items are dequeued first.
 
**Why we used it:** PWD vehicles, pregnant women, and reserved slot holders should not have to wait behind regular vehicles even if they arrived later. The Priority Queue ensures they are always served ahead of regular cars.
 
**In the code:** `priority_queue<Vehicle, vector<Vehicle>, greater<Vehicle>> priorityQueue`
 
| Priority Level | Type |
|---|---|
| 1 (Highest) | PWD / Pregnant |
| 2 | Reserved |
| 3 (Lowest) | Regular |
 
---
 
## ⚙️ Algorithm Explanation
 
### Parking a Vehicle
1. User enters plate number, owner name, and vehicle type.
2. System checks if the vehicle is already parked (duplicate check).
3. If a slot is available → assign immediately and push the action to the undo Stack.
4. If no slot is available → add to the Priority Queue (PWD/Reserved) or Regular Queue (Regular).
### Removing a Vehicle
1. User enters the plate number of the exiting vehicle.
2. System searches all slots for a match.
3. Slot is vacated and available count is updated.
4. System immediately serves the next waiting vehicle — checking the Priority Queue first, then the Regular Queue.
### Undo Last Action
1. Pop the top of the undo Stack (most recent assignment).
2. Check if the slot still holds the same plate number.
3. If yes → vacate the slot (undo successful).
4. If no → vehicle has already exited; notify the user.
### Search Vehicle
1. User enters a plate number.
2. System performs a **linear search** through all slots.
3. Reports the slot number if found.
---
 
## 🔁 Iterative vs Recursive Comparison
 
### Iterative — `viewParkedVehicles()`
Uses a `for` loop to walk through every `ParkingSlot` in the `vector` and prints the occupied ones.
 
```cpp
for (int i = 0; i < (int)slots.size(); i++) {
    if (slots[i].getIsOccupied()) {
        // print slot, plate, owner, type
    }
}
```
 
**Pros:** Faster in practice, no extra memory usage, easy to trace and debug.
**Cons:** More manual control required.
 
---
 
### Recursive — `viewWaitingList()` via `displayQueueRecursive()`
Passes a **copy** of the queue to the recursive function. Each call prints the front vehicle, pops it, and calls itself with the remaining queue until empty.
 
```cpp
void displayQueueRecursive(queue<Vehicle> q, int position) {
    if (q.empty()) return;              // Base case
    Vehicle v = q.front(); q.pop();
    v.display();
    displayQueueRecursive(q, position + 1); // Recursive call
}
```
 
**Pros:** Elegant and concise; naturally mirrors the structure of the queue.
**Cons:** Uses extra call stack memory per recursive call; slower for very large queues.
 
| | Iterative | Recursive |
|---|---|---|
| Speed | ✅ Faster | ⚠️ Slower for large data |
| Memory | ✅ No extra memory | ⚠️ Uses call stack |
| Readability | ✅ Easy to follow | ✅ Clean and concise |
| Used for | Parked vehicles display | Waiting list display |
 
---
 
## 🏗️ OOP Design
 
The program uses three classes following OOP principles — all attributes are `private`, and all operations are exposed through `public` methods.
 
```
ParkingSystem
├── Vehicle
│   ├── private: plateNumber, ownerName, priorityLevel, vehicleType
│   └── public:  getPlate(), getOwner(), getPriority(), getType(),
│                display(), operator>()
│
├── ParkingSlot
│   ├── private: slotNumber, isOccupied, occupant (Vehicle)
│   └── public:  getSlotNumber(), getIsOccupied(), getOccupantPlate(),
│                getOccupant(), occupy(Vehicle), vacate()
│
└── ParkingSystem
    ├── private: slots (vector), regularQueue, priorityQueue, undoStack
    └── public:  parkVehicle(), removeVehicle(), undoLastAction(),
                 viewParkedVehicles(), viewWaitingList(),
                 searchVehicle(), viewAllSlots()
```
 
---
 
## 🧠 Design Decisions & Trade-offs
 
| Decision | Why | Trade-off |
|---|---|---|
| Queue for regular vehicles | FIFO is the fairest method — first to arrive, first served | Cannot serve by priority |
| Priority Queue for PWD/Reserved | Urgent vehicles skip the regular line automatically | Order within the same priority level is not guaranteed |
| Stack for undo | LIFO is perfect for reversing the most recent action | Can only undo one action at a time |
| Two separate queues | Cleaner logic; mixing vehicles in one queue complicates priority checks | Two queues to maintain |
| Vector for slots | Index-based access; slot number maps directly to index; already sorted | Fixed size defined at startup |
| Store full Vehicle in ParkingSlot | Keeps owner name and type accessible for display | Slightly more memory per slot |
 
**Why not Stack for the waiting list?**
A Stack would serve the most recently arrived car first (LIFO), which is unfair to earlier arrivals.
 
**Why not Queue for undo?**
An undo should reverse the most recent action. A Queue would reverse the oldest action first — the opposite of what is needed.
 
---
 
## 🖥️ How to Compile and Run
 
```bash
# Compile (requires C++17)
g++ -std=c++17 -o parking_management_system parking_management_system.cpp
 
# Run
./parking_management_system
```
 
> **Windows:** Use `parking_management_system.exe` after compiling with MinGW or MSVC.
 
---
 
## ⚠️ Expected Challenges
 
- Coordinating two separate queues without mixing them, especially when a slot opens and both queues are non-empty.
- Ensuring the undo Stack only undoes valid current assignments (vehicle may have already exited before undo is called).
- Implementing recursive display without modifying the original queue — requires passing a copy to the recursive function.
- Handling edge cases such as duplicate plate entries, invalid inputs, and an empty lot.
---
 
## 📚 References / Resources
 
- Cormen, T. H., Leiserson, C. E., Rivest, R. L., & Stein, C. (2022). *Introduction to algorithms* (4th ed.). MIT Press.
- cppreference.com. (2024). *std::queue*. https://en.cppreference.com/w/cpp/container/queue
- cppreference.com. (2024). *std::priority_queue*. https://en.cppreference.com/w/cpp/container/priority_queue
- cppreference.com. (2024). *std::stack*. https://en.cppreference.com/w/cpp/container/stack
- GeeksforGeeks. (2024). *Stack data structure*. https://www.geeksforgeeks.org/stack-data-structure/
---
 
## 🙏 Acknowledgement
 
We would like to express our sincere gratitude to our instructor for the clear and detailed project guidelines provided for CC 103 – Data Structures and Algorithms. This project helped us understand the practical applications of Stack, Queue, and Priority Queue in solving real-world problems.
 
We also thank Batangas State University – Alangilan Campus and the College of Informatics and Computing Sciences for the knowledge and resources provided throughout this course.
 
---
 
*Batangas State University – The National Engineering University*
*College of Informatics and Computing Sciences | Computer Science Department*