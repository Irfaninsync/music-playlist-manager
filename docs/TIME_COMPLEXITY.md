# TIME AND SPACE COMPLEXITY ANALYSIS
## MUSIC PLAYLIST MANAGER (DATA STRUCTURES & ALGORITHMS IN C)

---

## 1. Asymptotic Complexity Summary Table

| Data Structure / Operation | Function Name | Best Case | Average Case | Worst Case | Auxiliary Space | Academic Rationale |
|---|---|---|---|---|---|---|
| **Dynamic Array — Append** | `add_song()` | $\mathcal{O}(1)$ | $\mathcal{O}(1)$ amortized | $\mathcal{O}(N)$ | $\mathcal{O}(1)$ | Resizing doubles capacity; individual inserts take $\mathcal{O}(1)$ amortized. |
| **Dynamic Array — Delete** | `delete_song()` | $\mathcal{O}(1)$ | $\mathcal{O}(N)$ | $\mathcal{O}(N)$ | $\mathcal{O}(1)$ | Requires shifting subsequent elements left to maintain contiguous order. |
| **Dynamic Array — Random Access** | Indexing `songs[i]` | $\mathcal{O}(1)$ | $\mathcal{O}(1)$ | $\mathcal{O}(1)$ | $\mathcal{O}(1)$ | Pointer arithmetic: $\text{Base} + i \times \text{sizeof(Song)}$. |
| **Linear Search** | `linear_search_by_title()` | $\mathcal{O}(1)$ | $\mathcal{O}(N)$ | $\mathcal{O}(N)$ | $\mathcal{O}(1)$ | Scans sequentially; best case occurs if target is at index 0. |
| **Binary Search** | `binary_search_by_id()` | $\mathcal{O}(1)$ | $\mathcal{O}(\log N)$ | $\mathcal{O}(\log N)$ | $\mathcal{O}(1)$ | Halves the search interval on each comparison. Precondition: sorted array. |
| **Bubble Sort** | `sort_songs_by_id()` | $\mathcal{O}(N)$ | $\mathcal{O}(N^2)$ | $\mathcal{O}(N^2)$ | $\mathcal{O}(1)$ | Compares adjacent pairs; nested iterations. In-place sorting. |
| **Selection Sort** | `sort_songs_by_title()` | $\mathcal{O}(N^2)$ | $\mathcal{O}(N^2)$ | $\mathcal{O}(N^2)$ | $\mathcal{O}(1)$ | Always scans unsorted portion for minimum element; $\mathcal{O}(N)$ swaps. |
| **Insertion Sort** | `sort_songs_by_artist()` | $\mathcal{O}(N)$ | $\mathcal{O}(N^2)$ | $\mathcal{O}(N^2)$ | $\mathcal{O}(1)$ | Shifts elements into sorted prefix. Efficient $\mathcal{O}(N)$ on nearly sorted data. |
| **SLL — Insert at Head** | `add_to_history()` | $\mathcal{O}(1)$ | $\mathcal{O}(1)$ | $\mathcal{O}(1)$ | $\mathcal{O}(1)$ | Adjusts `new_node->next = head; head = new_node;`. |
| **SLL — Tail Pruning** | Bounded history trimming | $\mathcal{O}(N)$ | $\mathcal{O}(N)$ | $\mathcal{O}(N)$ | $\mathcal{O}(1)$ | Traverses to penultimate node to delete tail node. |
| **DLL — Insert End / Start** | `add_to_playlist_end()` | $\mathcal{O}(1)$ | $\mathcal{O}(1)$ | $\mathcal{O}(1)$ | $\mathcal{O}(1)$ | Maintains `tail` and `head` pointers; direct splice. |
| **DLL — Navigation** | `playlist_next()`, `prev()`| $\mathcal{O}(1)$ | $\mathcal{O}(1)$ | $\mathcal{O}(1)$ | $\mathcal{O}(1)$ | Direct pointer traversal via `current->next` or `current->prev`. |
| **CLL — Repeat Step** | `circular_next()` | $\mathcal{O}(1)$ | $\mathcal{O}(1)$ | $\mathcal{O}(1)$ | $\mathcal{O}(1)$ | Circular loop from tail to head requires no boundary re-allocation. |
| **Stack — Push / Pop / Peek** | `push_action()`, `pop()` | $\mathcal{O}(1)$ | $\mathcal{O}(1)$ | $\mathcal{O}(1)$ | $\mathcal{O}(1)$ | Operates directly on `items[top]` with integer index arithmetic. |
| **FIFO Queue — Enqueue / Dequeue**| `enqueue()`, `dequeue()` | $\mathcal{O}(1)$ | $\mathcal{O}(1)$ | $\mathcal{O}(1)$ | $\mathcal{O}(1)$ | Circular array with modulo arithmetic `(rear + 1) % capacity`. |
| **Max-Heap — Insertion** | `pq_insert()` | $\mathcal{O}(1)$ | $\mathcal{O}(\log N)$ | $\mathcal{O}(\log N)$ | $\mathcal{O}(1)$ | Element placed at heap end and bubbled up tree height ($\log_2 N$). |
| **Max-Heap — Extract Max** | `pq_extract_max()` | $\mathcal{O}(1)$ | $\mathcal{O}(\log N)$ | $\mathcal{O}(\log N)$ | $\mathcal{O}(1)$ | Root swapped with last leaf and sunk down tree height. |
| **Max-Heap — Peek Max** | `pq_peek()` | $\mathcal{O}(1)$ | $\mathcal{O}(1)$ | $\mathcal{O}(1)$ | $\mathcal{O}(1)$ | Root element is always at array index 0. |
| **Hash Table — Search by ID** | `hash_search()` | $\mathcal{O}(1)$ | $\mathcal{O}(1)$ | $\mathcal{O}(N)$ | $\mathcal{O}(1)$ | Modulo bucket indexing; worst case occurs under total hash collision. |
| **BST — Search by Title** | `bst_search()` | $\mathcal{O}(1)$ | $\mathcal{O}(\log N)$ | $\mathcal{O}(N)$ | $\mathcal{O}(H)$ call stack | Worst case $O(N)$ if tree degenerates into a linear chain (skewed tree). |
| **BST — In-order Traversal** | `bst_inorder()` | $\mathcal{O}(N)$ | $\mathcal{O}(N)$ | $\mathcal{O}(N)$ | $\mathcal{O}(H)$ call stack | Visits every node exactly once, producing alphabetical sorting. |
| **Graph — BFS Traversal** | `graph_bfs()` | $\mathcal{O}(V + E)$ | $\mathcal{O}(V + E)$ | $\mathcal{O}(V + E)$ | $\mathcal{O}(V)$ | Visits every reachable vertex and explores every edge via queue. |
| **Graph — DFS Traversal** | `graph_dfs()` | $\mathcal{O}(V + E)$ | $\mathcal{O}(V + E)$ | $\mathcal{O}(V + E)$ | $\mathcal{O}(V)$ call stack | Explores deeply along branch edges before backtracking. |

---

## 2. Theoretical vs Empirical Analysis

### 2.1 Why Binary Search Outperforms Linear Search
- **Linear Search:** For a catalog of $N$ songs, the number of comparisons is:
  $$C_{\text{avg}} = \frac{N + 1}{2} \approx \frac{N}{2}$$
  For $N = 1,000$ songs, average comparisons $\approx 500$.
- **Binary Search:** On a sorted array, the problem size is halved at each step:
  $$N \rightarrow \frac{N}{2} \rightarrow \frac{N}{4} \rightarrow \dots \rightarrow 1 \implies k = \lceil \log_2 N \rceil$$
  For $N = 1,000$ songs, maximum comparisons $= \lceil \log_2 1000 \rceil = 10$.  
  **Performance gain:** Binary Search is **50x faster** on average for $N = 1,000$.

---

### 2.2 Heap Operations vs Linear Priority Scans
- **Unsorted Array Priority Queue:**
  - Insertion: $\mathcal{O}(1)$
  - Extract Maximum: $\mathcal{O}(N)$ (Requires scanning all elements)
- **Sorted Array Priority Queue:**
  - Insertion: $\mathcal{O}(N)$ (Requires linear shifting to maintain order)
  - Extract Maximum: $\mathcal{O}(1)$
- **Binary Max-Heap (Our Implementation):**
  - Insertion: $\mathcal{O}(\log N)$
  - Extract Maximum: $\mathcal{O}(\log N)$  
  **Conclusion:** The binary heap strikes the optimal logarithmic balance for dynamic workloads where songs are continuously prioritized and extracted.

---

### 2.3 Hash Table Separate Chaining Load Factor Analysis
Let $N$ be the number of stored song records and $M = 31$ be the number of buckets:
$$\alpha = \frac{N}{M} \quad (\text{Load Factor})$$
- When $\alpha \le 1.0$, the average length of each collision chain is $\le 1$.
- Search time is modeled as:
  $$T_{\text{successful search}} = 1 + \frac{\alpha}{2} \in \mathcal{O}(1)$$
- For our default catalog ($N = 12, M = 31$), $\alpha = 12 / 31 \approx 0.387$, guaranteeing near-instantaneous $O(1)$ lookups with minimal collisions.
