# Music Playlist Manager
**A Data Structures and Algorithms (DSA) Group Capstone Project in C (C11)**  
*Developed for B.Tech Computer Science and Engineering*

> **About This Project:**  
> This **Music Playlist Manager** was developed as our group academic capstone project for the **Data Structures and Algorithms (DSA)** subject in our B.Tech curriculum. The objective of this project is to implement, integrate, and practically demonstrate all major data structures—Dynamic Arrays, Singly Linked Lists, Doubly Linked Lists, Circular Linked Lists, Stacks, Queues, Binary Max-Heap Priority Queues, Hash Tables with Separate Chaining, Binary Search Trees, and Graphs—entirely in pure C without relying on built-in or external DSA libraries. Every structure and algorithm has been built manually from scratch to demonstrate practical software engineering, pointer mechanics, dynamic memory allocation, and algorithmic time complexities.

---

## 👥 Project Team Members

| S.No. | Roll Number | Key Module & DSA Responsibilities |
|:---:|:---:|:---|
| **1** | `2025DCSBT10650` | Dynamic Array (Library Catalog), Binary Search & Linear Search Engine |
| **2** | `2025DCSBT10651` | Singly Linked List (Listening History), Head Prepend & Bounded Tail Pruning |
| **3** | `2025DCSBT10652` | Doubly & Circular Linked Lists (Bidirectional Playlist Navigation & Repeat Looping) |
| **4** | `2025DCSBT10653` | LIFO Stack (Undo System) & FIFO Queue (Upcoming Song Staging) |
| **5** | `2025DCSBT10654` | Binary Max-Heap (VIP Priority Queue) & Separate Chaining Hash Table |
| **6** | `2025DCSBT10655` | Binary Search Tree (Alphabetical Title Index) & Graph (BFS/DFS Recommendations) |

---

## 1. Project Overview

The **Music Playlist Manager** is a complete, modular, menu-driven console application written entirely in pure **C11** using only standard C libraries (`stdio.h`, `stdlib.h`, `string.h`, `time.h`, `ctype.h`). 

The application demonstrates how fundamental data structures solve concrete, practical engineering challenges in audio playback, playlist management, listening history, search indexing, and musical recommendations.

Every data structure has been **implemented manually from scratch** using C `struct` types, pointers, arrays, and explicit dynamic memory management (`malloc`, `realloc`, `free`). No external or built-in DSA libraries are used.

---

## 2. The 10 Integrated Data Structures

| # | Data Structure | Application Role | Operational Role & Rationale | Viva Time Complexity |
|---|---|---|---|---|
| **1** | **Dynamic Array** | Main Music Library | Stores canonical `Song` records. Supports $O(1)$ random access, dynamic resizing with `realloc()`, linear search, and binary search on sorted IDs. | Search: $O(N)$ / $O(\log N)$<br>Insert: $O(1)$ amortized<br>Delete: $O(N)$ |
| **2** | **Singly Linked List** | Listening History | Prepends newly played songs at the head ($O(1)$ insertion) so the most recently played song appears first. Bounded to a fixed maximum capacity (20 songs); oldest nodes are pruned from the tail. | Insert (Head): $O(1)$<br>Display: $O(K)$ |
| **3** | **Doubly Linked List** | Active Playlist Navigation | Manages the active playlist tracks. Each node links `prev` and `next`, maintaining a `current_song` cursor. Supports instantaneous forward (`Next`) and backward (`Prev`) track navigation. | Navigation: $O(1)$<br>Insert/Delete: $O(1)$ with pointer |
| **4** | **Circular Linked List** | Playlist Repeat Mode | Tail's `next` loops back to `head`, and head's `prev` loops back to `tail`. Smoothly loops playback back to the very first song after reaching the end without special edge-case branching. | Loop Step: $O(1)$ |
| **5** | **LIFO Stack** | Undo Playlist Changes | Whenever a track is added, removed, or reordered in a playlist, an `UndoAction` token is pushed. "Undo Last Action" pops the action and inverts the operation (restores deleted songs, reverses additions). | Push: $O(1)$<br>Pop: $O(1)$ |
| **6** | **FIFO Queue** | Upcoming Song Queue | Circular array implementation (`(rear + 1) % capacity`). Songs queued by the user wait in a FIFO line. When a track ends, the player dequeues and plays from this queue first before resuming normal playlist order. | Enqueue: $O(1)$<br>Dequeue: $O(1)$ |
| **7** | **Priority Queue (Max-Heap)** | Priority Playback | Users assign priority scores (1 to 10). Implemented manually as a complete binary max-heap stored in an array. Supports $O(\log N)$ insertion and extraction of the highest-priority song. | Insert: $O(\log N)$<br>Extract-Max: $O(\log N)$<br>Peek: $O(1)$ |
| **8** | **Hash Table** | Fast Song Lookup by ID | Employs an array of 31 buckets with **Separate Chaining** (linked list buckets) using a modulo hash function `hash(id) = id % 31`. Enables $O(1)$ average time lookup of any song by its unique ID. | Search: $O(1)$ average, $O(N)$ worst |
| **9** | **Binary Search Tree (BST)** | Alphabetical Song Index | Self-contained BST where nodes are ordered lexicographically by song `title`. Provides alphabetical catalog browsing via **In-order traversal**, tree structure view via **Pre-order**, and clean deallocation via **Post-order**. | Search/Insert: $O(\log N)$ avg<br>Traversal: $O(N)$ |
| **10** | **Graph (Adjacency List)** | Song Recommendations | Songs are vertices; undirected edges connect songs that share the same artist or musical genre. Features **Breadth-First Search (BFS)** for immediate neighbors and **Depth-First Search (DFS)** for deep musical lineage exploration. | Traversal: $O(V + E)$ |

---

## 3. Project Directory Structure

```
MusicPlaylistManager/
│
├── Makefile                    # Standard GCC build script
├── build.bat                   # 1-click Windows compilation & run script
├── README.md                   # Project overview & instructions
├── music_manager.exe           # Compiled executable
│
├── data/                       # Persistent CSV data storage
│   ├── songs.csv               # Main music library database
│   ├── playlists.csv           # Playlists and their song ID sequences
│   └── history.csv             # Saved listening history
│
├── docs/                       # Academic submission documentation
│   ├── PROJECT_REPORT.md       # Complete B.Tech academic report
│   ├── TIME_COMPLEXITY.md      # Theoretical & empirical complexity analysis
│   └── VIVA_QUESTIONS.md       # Top 30 viva questions with model answers
│
├── include/                    # Header files
│   ├── song.h                  # Song struct & display helpers
│   ├── library.h               # Dynamic array & catalog functions
│   ├── singly_linked_list.h    # History list (SLL)
│   ├── doubly_linked_list.h    # Playlist navigation (DLL)
│   ├── circular_linked_list.h  # Repeat mode playlist (CLL)
│   ├── stack.h                 # Undo operations stack
│   ├── queue.h                 # Upcoming playback FIFO queue
│   ├── priority_queue.h        # Max-heap priority playback
│   ├── hash_table.h            # Separate chaining hash table
│   ├── bst.h                   # Binary search tree for titles
│   ├── graph.h                 # Adjacency list recommendation graph
│   ├── file_handler.h          # CSV file persistence routines
│   └── interface.h             # Console menus & interactive UI
│
└── src/                        # Complete C11 source implementations
    ├── main.c                  # Main entry point & central loop
    ├── song.c                  # Song utilities (duration formatting, cards)
    ├── library.c               # Dynamic array, sorting, linear/binary search
    ├── singly_linked_list.c    # History insertion, pruning, search
    ├── doubly_linked_list.c    # Playlist forward/backward traversal & cursor
    ├── circular_linked_list.c  # Seamless looping repeat playback
    ├── stack.c                 # Push, pop, undo record dispatcher
    ├── queue.c                 # Enqueue, dequeue, upcoming song display
    ├── priority_queue.c        # Binary max-heap operations (heapify up/down)
    ├── hash_table.c            # Modulo hash, chaining, fast ID search
    ├── bst.c                   # Alphabetical in/pre/post order traversals
    ├── graph.c                 # Adjacency list creation, BFS and DFS
    ├── file_handler.c          # CSV file parser and serializer
    └── interface.c             # Formatted menus, simulated player, DSA demo
```

---

## 4. How to Compile and Run

### Option A: Using GCC directly in terminal (Recommended)
```bash
# Navigate to project folder
cd d:\dsaproject

# Compile all modules with strict C11 standard
gcc -std=c11 -Wall -Wextra -pedantic src/*.c -Iinclude -o music_manager.exe

# Run application
./music_manager.exe
```

### Option B: Using the Windows Batch Script
Simply double-click `build.bat` or run:
```cmd
build.bat
```

### Option C: Using Make
```bash
make
make run
```

---

## 5. Quick Viva Demonstration Flow

When demonstrating this project to an external examiner, follow this 5-minute showcase:

1. **Option 1 (Music Library):** Show all 12 pre-loaded songs in the dynamic array table.
2. **Option 7 (Search & Sort):**
   - Run **Binary Search by ID** (requires sorted order, demonstrates $O(\log N)$).
   - Run **Alphabetical In-order Traversal** (demonstrates BST property).
3. **Option 3 (Music Player Simulator):**
   - Press `1` to Play. Show current track metadata and duration.
   - Press `4` for Next Track (Doubly Linked List pointer forward movement).
   - Press `5` for Previous Track (Doubly Linked List pointer backward movement).
   - Press `6` to toggle Repeat Mode (transitions to Circular Linked List).
4. **Option 4 (Upcoming Queue):**
   - Enqueue a song (e.g., ID 1008). Return to Player and press Next; verify it plays the queued song first (FIFO principle)!
5. **Option 5 (Recently Played):** Show the history list (Singly Linked List head-insertion, newest played song at the top).
6. **Option 6 (Priority Playback):** Extract and play the highest-priority song (Max-Heap $O(\log N)$ extraction).
7. **Option 8 (Recommendations):** Pick a song (e.g., ID 1001 Queen) and run **BFS** and **DFS** to explore musical connections via the Graph Adjacency List.
8. **Option 9 (Undo Last Action):** Add a song to playlist, then immediately select Undo; verify the stack inverts and removes the track.
9. **Option 10 (DSA Demonstration):** Open the interactive visualizer to show collision chains in the Hash Table, Heap structure, and BST tree levels.
