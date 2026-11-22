# TOP 30 VIVA QUESTIONS AND MODEL ANSWERS
## MUSIC PLAYLIST MANAGER (DATA STRUCTURES & ALGORITHMS IN C)

---

### Q1. Why did you choose C language for this project instead of Python or Java?
**Model Answer:**  
"In Python or Java, data structures are abstracted away by built-in standard library collections like `ArrayList`, `LinkedList`, `HashMap`, or `PriorityQueue`. In C, there are no built-in collection classes. Implementing this project in C required me to manually declare structs, manipulate pointers, manage memory using `malloc()`, `realloc()`, and `free()`, and implement algorithms like heapify, binary search, and graph traversals from scratch. This demonstrates a deep, fundamental understanding of how computer memory and data structures work at the machine level."

---

### Q2. How are the 10 data structures coordinated in your application?
**Model Answer:**  
"We utilize the **Single Source of Truth** design pattern. The main music library is stored in a **Dynamic Array** containing the complete `Song` records. All other structures—such as the **Doubly Linked List** (playlists), **Singly Linked List** (history), **FIFO Queue** (upcoming songs), **Priority Queue** (VIP songs), **Hash Table** (ID lookup), **BST** (alphabetical titles), and **Graph** (recommendations)—only store integer `song_id` references. This eliminates data redundancy, reduces memory consumption, and ensures that updating or deleting a song in the library can be cascaded across all auxiliary structures without leaving dangling pointers."

---

### Q3. Why use an Array for the main music library instead of a Linked List?
**Model Answer:**  
"An array provides $O(1)$ constant-time random access by index, which is ideal for a library catalog where songs are enumerated. Furthermore, array elements are stored in contiguous memory blocks, which maximizes CPU cache locality. Most importantly, an array allows us to perform **Binary Search in $O(\log N)$ time** once sorted, which is impossible on a standard linked list because linked lists do not support random indexing."

---

### Q4. How does your Dynamic Array handle resizing when it runs out of capacity?
**Model Answer:**  
"When `lib->count == lib->capacity`, our `add_song()` function doubles the capacity (`new_capacity = capacity * 2`) and calls `realloc(lib->songs, sizeof(Song) * new_capacity)`. `realloc()` attempts to expand the existing memory block in place; if that is not possible, it allocates a new larger block elsewhere, copies the existing records over, frees the old block, and returns the new pointer. This geometric doubling strategy ensures an **amortized $O(1)$ insertion time complexity**."

---

### Q5. Why is a Doubly Linked List preferred over a Singly Linked List for the active playlist?
**Model Answer:**  
"A music player requires instantaneous bidirectional navigation: jumping to the next track or moving back to the previous track. In a **Doubly Linked List (DLL)**, every node contains both a `next` and a `prev` pointer. Thus, navigating to `current->next` or `current->prev` is an $O(1)$ operation. In a Singly Linked List, moving to the previous song would require traversing the list from `head` all the way to the node preceding `current`, which would take $O(N)$ time."

---

### Q6. Why did you use a Singly Linked List for Listening History?
**Model Answer:**  
"Listening history only requires chronological prepend operations. When a song finishes playing, we insert it at the head of the Singly Linked List in $O(1)$ time. Because users read history from newest to oldest, traversing from `head` naturally displays songs in reverse chronological order. When the history exceeds our fixed limit (20 songs), we prune the oldest song from the tail."

---

### Q7. How does the Circular Linked List implement Repeat Mode?
**Model Answer:**  
"In our Circular Doubly Linked List, the `next` pointer of the final node (tail) points directly back to the `head`, and the `prev` pointer of the `head` points to the tail. When repeat mode is active, advancing past the last song seamlessly brings the playback cursor back to the first song in $O(1)$ time without requiring conditional boundary checks."

---

### Q8. What is the role of the Stack in your project?
**Model Answer:**  
"The Stack implements our **Undo Last Action** feature. A stack operates on the **LIFO (Last-In-First-Out)** principle, which matches operational undo semantics—the most recently performed modification must be the first one reversed. When a track is added, removed, or moved in a playlist, an `UndoAction` token containing the operation type, song ID, and original position is pushed onto the stack. Selecting 'Undo' pops this token and executes the inverse operation in $O(1)$ time."

---

### Q9. What happens if a user triggers Undo when no actions have occurred?
**Model Answer:**  
"This tests for **Stack Underflow**. Our `pop_action()` function first checks `is_stack_empty(s)`. If `top == -1`, it returns 0 and displays a polite message stating that no undo actions are available, preventing invalid memory access or segmentation faults."

---

### Q10. How does your Upcoming Queue work and why is it implemented as a circular array?
**Model Answer:**  
"The upcoming queue operates on the **FIFO (First-In-First-Out)** principle, ensuring songs manually queued by the user play in the exact sequence they were added. We implemented it as a circular array using `front` and `rear` indices with modular arithmetic: `rear = (rear + 1) % capacity` and `front = (front + 1) % capacity`. This prevents the 'drifting queue' problem of linear array queues, allowing enqueue and dequeue operations to run in $O(1)$ time without shifting elements."

---

### Q11. Why did you implement a Priority Queue using a Binary Max-Heap instead of a sorted array?
**Model Answer:**  
"In an unsorted array, finding or extracting the highest priority song takes $O(N)$ time. In a sorted array, extraction takes $O(1)$, but inserting a new song requires shifting elements, which takes $O(N)$ time. A **Binary Max-Heap** provides an optimal logarithmic balance:
- Insertion takes $O(\log N)$ via `heapify_up`.
- Extraction of the maximum priority song takes $O(\log N)$ via `heapify_down`.
- Peeking at the root takes $O(1)$."

---

### Q12. How is the Binary Heap stored in memory without child pointers?
**Model Answer:**  
"A binary heap is a complete binary tree, which allows it to be mapped directly into a flat contiguous 1D array without child or parent pointers. For any element at index $i$:
- Its parent is at index `(i - 1) / 2`.
- Its left child is at index `2 * i + 1`.
- Its right child is at index `2 * i + 2`.
This eliminates pointer overhead and ensures fast array indexing."

---

### Q13. How do you resolve ties when two songs in the Priority Queue have the same priority level?
**Model Answer:**  
"We maintain a global monotonic sequence counter. When a song is inserted into the heap, it receives the current counter value. When comparing two elements with equal priority scores, our comparator prioritizes the smaller sequence number. This guarantees **FIFO fairness** among songs with identical priorities."

---

### Q14. What is the Hash Table used for and how does its hash function work?
**Model Answer:**  
"The Hash Table provides **$O(1)$ average time lookup** for songs using their unique numeric ID. We use a modulo division hash function:
$$\text{hash}(id) = |id| \pmod{\text{TABLE\_SIZE}}$$
We chose `TABLE_SIZE = 31`, which is a prime number. Prime moduli distribute keys more uniformly across buckets and reduce clustering."

---

### Q15. How do you handle hash collisions?
**Model Answer:**  
"We handle collisions using **Separate Chaining**. Each bucket in the hash table array contains a pointer to the head of a linked list of `HashNode` structures. When two song IDs hash to the same bucket index, the new node is prepended to that bucket's linked list in $O(1)$ time."

---

### Q16. What is the worst-case time complexity of your Hash Table, and when does it occur?
**Model Answer:**  
"The worst-case search complexity is $O(N)$. This occurs if the hash function maps every single song ID into the exact same bucket, causing the separate chain to degenerate into a linear linked list of length $N$. However, with a good prime table size and uniformly distributed IDs, the average lookup time remains $O(1)$."

---

### Q17. Why do you use a Binary Search Tree (BST) for song titles?
**Model Answer:**  
"A Binary Search Tree maintains titles in lexicographical order. The BST property states that for any node, all titles in its left subtree are alphabetically smaller, and all titles in its right subtree are alphabetically larger. Performing an **In-order traversal (Left $\rightarrow$ Root $\rightarrow$ Right)** visits the nodes in strictly sorted alphabetical order ($A \rightarrow Z$) in $O(N)$ time without having to sort the catalog repeatedly."

---

### Q18. How do you handle duplicate song titles in your BST?
**Model Answer:**  
"If two songs have identical titles (for example, cover versions by different artists), our `insert_recursive()` function uses the unique `song_id` as a secondary comparison key. If the titles match, it compares the integer IDs to determine whether the node branches to the left or right subtree."

---

### Q19. How does node deletion work in your BST?
**Model Answer:**  
"BST deletion handles three distinct structural cases:
1. **Node is a leaf (0 children):** The node is freed and its parent pointer is set to `NULL`.
2. **Node has 1 child:** The node is freed and its child is spliced directly into its parent.
3. **Node has 2 children:** We find the node's **in-order successor** (the minimum node in its right subtree), copy the successor's data into the current node, and recursively delete the successor from the right subtree."

---

### Q20. What is the role of the Graph in your application?
**Model Answer:**  
"The Graph models musical relationships and powers our **Song Recommendation Engine**. Every song in the library is a vertex. An undirected edge is created between two songs if they share the same artist or musical genre. Each edge stores a descriptive reason, such as 'Same Artist' or 'Same Genre: Rock'."

---

### Q21. How is your Graph represented in memory?
**Model Answer:**  
"We use an **Adjacency List** representation. The graph consists of an array of vertices, where each vertex stores a `song_id` and a linked list pointer `head`. Each node in the list (`AdjListNode`) represents an outgoing edge to an adjacent song. An adjacency list is space-efficient ($O(V + E)$) compared to an adjacency matrix ($O(V^2)$), which would waste significant memory because music similarity graphs are relatively sparse."

---

### Q22. Explain the difference between your Graph's BFS and DFS implementations.
**Model Answer:**  
- **BFS (Breadth-First Search):** Uses a FIFO queue and a `visited` boolean array. It explores songs level by level, discovering immediate 1-hop similar tracks first before moving to 2-hop neighbors. This is ideal for finding the closest musical matches.
- **DFS (Depth-First Search):** Uses recursion (call stack) and a `visited` boolean array. It follows a musical path as deeply as possible along shared genres and artists before backtracking. Both algorithms run in $O(V + E)$ time."

---

### Q23. Why is a `visited` array necessary in BFS and DFS?
**Model Answer:**  
"Because our recommendation graph is undirected, edges exist in both directions ($u \rightarrow v$ and $v \rightarrow u$), creating cycles. Without a `visited` array to mark vertices that have already been processed, the traversal would loop infinitely between adjacent songs and cause a stack overflow or infinite loop."

---

### Q24. What is 'Cascade Deletion' and why is it critical in this project?
**Model Answer:**  
"Cascade deletion ensures **referential integrity**. Because auxiliary structures (playlists, history, queue, priority queue, BST, hash table, and graph) store `song_id` references, deleting a song from the library without notifying these structures would leave invalid IDs. Our `cascade_delete_song()` function systematically deletes the song from the library array, unlinks it from all playlists, removes it from the history list and upcoming queue, prunes it from the priority queue, deletes it from the hash table and BST, and removes its vertex and edges from the graph."

---

### Q25. What sorting algorithms did you implement and where are they used?
**Model Answer:**  
"We implemented three classic sorting algorithms to showcase different techniques:
1. **Bubble Sort:** Used for sorting by Song ID and Track Duration ($O(N^2)$ worst-case, adjacent swaps).
2. **Selection Sort:** Used for sorting by Title ($O(N^2)$ comparisons, minimum swaps).
3. **Insertion Sort:** Used for sorting by Artist ($O(N^2)$ worst-case, efficient $O(N)$ on nearly sorted data)."

---

### Q26. How does your Binary Search function work, and what is its precondition?
**Model Answer:**  
"Our `binary_search_by_id()` function operates on the library array using `low`, `high`, and `mid = low + (high - low) / 2`. At each step, it compares the target ID with `songs[mid].id`. If equal, it returns the index; if target is smaller, `high = mid - 1`; if larger, `low = mid + 1`. The essential precondition is that the array **must be sorted by Song ID**. Binary search runs in $O(\log N)$ time."

---

### Q27. How did you prevent buffer overflows when reading user input from the console?
**Model Answer:**  
"We strictly avoided unsafe functions like `gets()` or raw `scanf("%s")`. Instead, we implemented `safe_read_string()` and `safe_read_int()` using `fgets()`, which takes the explicit buffer size as a parameter. We then strip the trailing newline character `\n` and parse values with `strtol()`, safely discarding any leftover characters from the input stream."

---

### Q28. How is data persisted between application sessions?
**Model Answer:**  
"We use standard C file I/O (`fopen`, `fprintf`, `fgets`, `fclose`) with formatted **CSV (Comma-Separated Values)** files stored in the `data/` directory:
- `data/songs.csv`: Stores the complete song catalog.
- `data/playlists.csv`: Stores playlist names and comma-separated sequences of track IDs.
- `data/history.csv`: Stores recently played track IDs and timestamps.
When the program launches, `load_all_data()` parses these files line-by-line; on exit, `save_all_data()` serializes the state to disk."

---

### Q29. How do you prevent memory leaks in your program?
**Model Answer:**  
"Every data structure that dynamically allocates memory has a dedicated cleanup function (`free_library`, `free_history`, `free_playlist`, `free_stack`, `free_queue`, `free_priority_queue`, `free_hash_table`, `free_bst`, `free_graph`). When the user selects Option 11 ('Save Catalog & Exit'), `free_app_context()` is invoked, traversing all linked lists, tree nodes, adjacency lists, and dynamic arrays to call `free()` on every allocated block before the process terminates."

---

### Q30. If you had one more month to work on this project, what major feature would you add?
**Model Answer:**  
"I would implement two enhancements:
1. **Audio Decoding:** Integrate a lightweight single-header C audio library like `miniaudio.h` to play actual `.mp3` or `.wav` sound buffers through the computer's sound card.
2. **AVL Tree:** Upgrade our Binary Search Tree to a self-balancing **AVL Tree** using rotations (LL, RR, LR, RL) to guarantee strict $O(\log N)$ search time even if songs are inserted in pre-sorted alphabetical order."
