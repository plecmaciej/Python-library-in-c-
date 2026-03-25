# Simple Graphs – Python C++ Extension

## 📌 Project Overview

This project implements a graph data structure as a **Python extension module written in C++**.

The goal was to create a fully functional module (`simple_graphs`) that can be imported in Python and behaves like a native Python class, while internally using efficient C++ structures.

The implemented class:
```
simple_graphs.AdjacencyList
```
represents an **undirected graph using an adjacency list**.

---

## ⚙️ Features

The module provides:

### ✅ Basic graph operations

All methods defined in `graphs.py` have been implemented, including:

- `add_vertex(v)`
- `add_edge(u, v)`
- `delete_vertex(v)`
- `delete_edge(u, v)`
- `number_of_vertices()`
- `number_of_edges()`
- `vertices()`
- `edges()`
- `vertex_degree(v)`
- `vertex_neighbors(v)`
- `is_edge(u, v)`

---

### ⭐ Additional operation

This project also implements the required additional operation:
```
create_path(n)
```
This function creates a graph consisting of a simple path:  
```
0 — 1 — 2 — ... — (n-1)
```
---

## 🧠 Internal Representation

The graph is implemented in C++ using:

```cpp
bool vertices[16];
std::vector<int> adj[16];
vertices[i] → indicates whether vertex i exists
adj[i] → stores neighbors of vertex i
```
## 🔍 Graph6 (g6) Format

The constructor of the class supports loading graphs encoded in **graph6 (g6)** format.

### How it works in this project

1. The first character encodes the number of vertices:
```cpp
n = text[0] - 63
```
2. The remaining characters encode edges as a sequence of bits.

3. Each bit represents whether an edge exists between a pair of vertices.

4. The graph is undirected, so edges are added in both directions.

---

### 📚 Learn more about graph6

For a detailed explanation of the format, see:

- https://networkx.org/documentation/stable/reference/readwrite/graph6.html

---

## 🧪 Testing

The repository includes a test script:
```python
test.py
```
This script:

- generates graphs in g6 format  
- loads them into your implementation  
- validates correctness of all operations  

---

### ▶️ Run tests
```python
python test.py -t AdjacencyList
```
To test the additional operation:
```python
python test.py -t AdjacencyList create_path
```
---

## 🛠️ Building the Module

The project includes a `setup.py` script for building the extension.

### 🔧 Build and install

Run:
```python
python setup.py build
python setup.py install
```
or (recommended):
```python
python -m pip install .
```

---

## 📦 Result

After installation, the module can be used directly in Python:

```python
import simple_graphs

g = simple_graphs.AdjacencyList()
g.add_vertex(0)
g.add_vertex(1)
g.add_edge(0, 1)

print(g.vertices())
print(g.edges())
```
