# Music Playlist Manager
**A Data Structures and Algorithms (DSA) Group Capstone Project in C (C11)**  
*Developed for B.Tech Computer Science and Engineering*

> **About This Project:**  
> This **Music Playlist Manager** was developed as our group academic capstone project for the **Data Structures and Algorithms (DSA)** subject in our B.Tech curriculum. The entire project is implemented in a single, clean, concise C file (**`main.c`**, ~360 lines / 5-6 printed pages) demonstrating **all 10 fundamental data structures** built from scratch without any external or built-in DSA libraries.

---

## 👥 Project Team Members

| S.No. | Roll Number | Key Module & DSA Responsibility |
|:---:|:---:|:---|
| **1** | `2025DCSBT10650` | Dynamic Array (Library Catalog), Linear Search & Display |
| **2** | `2025DCSBT10651` | Singly Linked List (Listening History), O(1) Head Prepend |
| **3** | `2025DCSBT10652` | Doubly & Circular Linked Lists (Bidirectional Playlist Navigation & Repeat Looping) |
| **4** | `2025DCSBT10653` | LIFO Stack (Reversible Undo Engine) & Circular FIFO Queue (Upcoming Song Staging) |
| **5** | `2025DCSBT10654` | Binary Max-Heap (VIP Priority Queue) & Separate Chaining Hash Table (O(1) Search) |
| **6** | `2025DCSBT10655` | Binary Search Tree (Alphabetical Titles) & Graph Adjacency List (BFS Recommendations) |

---

## 1. The 10 Integrated Data Structures

All 10 data structures are implemented directly within **`main.c`**:

| # | Data Structure | Application Role | Operational Role & Rationale | Viva Time Complexity |
|---|---|---|---|---|
| **1** | **Dynamic Array** | Main Music Library | Stores canonical `Song` records. Supports $O(1)$ indexing, linear search, and dynamic catalog storage. | Search: $O(N)$<br>Insert: $O(1)$ |
| **2** | **Singly Linked List** | Listening History | Prepends newly played songs at the head ($O(1)$ insertion) so the most recently played song appears first. | Insert (Head): $O(1)$<br>Display: $O(K)$ |
| **3** | **Doubly Linked List** | Active Playlist Navigation | Manages active tracks. Each node links `prev` and `next`, maintaining a `current_song` cursor for Next/Prev track playback. | Navigation: $O(1)$<br>Insert: $O(1)$ |
| **4** | **Circular Linked List** | Playlist Repeat Mode | Tail's `next` loops to `head`, and head's `prev` loops to `tail`. Smoothly loops playback back to the first song when repeat is ON. | Loop Step: $O(1)$ |
| **5** | **LIFO Stack** | Undo Playlist Changes | Pushes added song IDs. Selecting "Undo Last Action" pops the last action and removes the song from the playlist. | Push: $O(1)$<br>Pop: $O(1)$ |
| **6** | **FIFO Queue** | Upcoming Song Queue | Circular array implementation (`(rear + 1) % capacity`). Songs wait in a FIFO line; player dequeues from the queue before normal playlist order. | Enqueue: $O(1)$<br>Dequeue: $O(1)$ |
| **7** | **Priority Queue (Max-Heap)** | VIP Playback | Custom binary max-heap stored in an array. Supports $O(\log N)$ insertion and extraction of the highest-priority song. | Insert: $O(\log N)$<br>Extract-Max: $O(\log N)$ |
| **8** | **Hash Table** | Fast Song Lookup by ID | Employs an array of 11 buckets with **Separate Chaining** (linked list buckets) using `hash(id) = id % 11` for fast $O(1)$ search. | Search: $O(1)$ avg, $O(N)$ worst |
| **9** | **Binary Search Tree (BST)** | Alphabetical Song Index | Nodes are ordered lexicographically by song `title`. Provides alphabetical browsing via **In-order traversal** ($A \rightarrow Z$). | Search/Insert: $O(\log N)$ avg<br>Traversal: $O(N)$ |
| **10** | **Graph (Adjacency List)** | Song Recommendations | Songs are vertices; undirected edges connect songs sharing an artist or genre. Traversed using **Breadth-First Search (BFS)**. | Traversal: $O(V + E)$ |

---

## 2. Project Directory Structure

```
MusicPlaylistManager/
├── main.c           # Complete, self-contained C source code (5-6 pages / ~360 lines)
├── Makefile         # Standard GCC build script
├── build.bat        # 1-click Windows compilation & run script
├── README.md        # Project guide & team overview
└── data/
    └── songs.csv    # Sample music database
```

---

## 3. How to Compile and Run

### Option A: Using GCC directly in your terminal
```bash
gcc -std=c11 -Wall -Wextra main.c -o music_manager.exe
./music_manager.exe
```

### Option B: Using the Windows batch script
Double-click `build.bat` or run:
```cmd
build.bat
```

### Option C: Using Make
```bash
make
make run
```
