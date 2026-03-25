#include <Python.h>
#include <vector>
#include <algorithm> 

struct GraphData {
    bool vertices[16];
    std::vector<int> adj[16];
};

typedef struct {
    PyObject_HEAD
    GraphData g;
} AdjacencyListObject;

static int AdjacencyList_init(AdjacencyListObject* self, PyObject* args, PyObject* kwds)
{
    const char* text = "?";

    if (!PyArg_ParseTuple(args, "|s", &text))
        return -1;

    for (int i = 0; i < 16; i++) {
        self->g.vertices[i] = false;
        self->g.adj[i].clear();
    }

    int n = text[0] - 63;

    for (int i = 0; i < n; i++) {
        self->g.vertices[i] = true;
    }

    int i = 1;
    int k = 0;
    int c = 0;

    for (int v = 1; v < n; v++) {
        for (int u = 0; u < v; u++) {

            if (k == 0) {
                c = text[i++] - 63;
                k = 6;
            }

            k--;

            if (c & (1 << k)) {
                self->g.adj[u].push_back(v);
                self->g.adj[v].push_back(u);
            }
        }
    }

    return 0;
}

static PyObject* AdjacencyList_number_of_vertices(AdjacencyListObject* self, PyObject* Py_UNUSED(ignored))
{
    int count = 0;

    for (int i = 0; i < 16; i++) {
        if (self->g.vertices[i])
            count++;
    }

    return PyLong_FromLong(count);
}

static PyObject* AdjacencyList_vertices(AdjacencyListObject* self, PyObject* Py_UNUSED(ignored))
{
    PyObject* result = PySet_New(NULL);

    for (int i = 0; i < 16; i++) {
        if (self->g.vertices[i]) {
            PyObject* val = PyLong_FromLong(i);
            PySet_Add(result, val);
            Py_DECREF(val);
        }
    }

    return result;

}

static PyObject* AdjacencyList_vertex_degree(AdjacencyListObject* self, PyObject* args)
{
    int vertex;

    if (!PyArg_ParseTuple(args, "i", &vertex))
        return NULL;

    if (vertex < 0 || vertex >= 16) {
        PyErr_SetString(PyExc_ValueError, "Vertex out of range");
        return NULL;
    }

    if (!self->g.vertices[vertex]) {
        PyErr_SetString(PyExc_ValueError, "Vertex is not in graph");
        return NULL;
    }

    int degree = self->g.adj[vertex].size();

    return PyLong_FromLong(degree);

}

static PyObject* AdjacencyList_vertex_neighbors(AdjacencyListObject* self, PyObject* args)
{
    int vertex;

    if (!PyArg_ParseTuple(args, "i", &vertex))
        return NULL;

    if (vertex < 0 || vertex >= 16) {
        PyErr_SetString(PyExc_ValueError, "Vertex out of range");
        return NULL;
    }

    if (!self->g.vertices[vertex]) {
        PyErr_SetString(PyExc_ValueError, "Vertex is not in graph");
        return NULL;
    }

    PyObject* result = PySet_New(NULL);

    for (int neighbor : self->g.adj[vertex]) {
        PyObject* val = PyLong_FromLong(neighbor);
        PySet_Add(result, val);
        Py_DECREF(val);
    }

    return result;

}


static PyObject* AdjacencyList_add_vertex(AdjacencyListObject* self, PyObject* args)
{
    int vertex;

    if (!PyArg_ParseTuple(args, "i", &vertex))
        return NULL;

    if (vertex < 0 || vertex >= 16) {
        PyErr_SetString(PyExc_ValueError, "Vertex out of range");
        return NULL;
    }

    self->g.vertices[vertex] = true;

    Py_RETURN_NONE;
}

static PyObject* AdjacencyList_delete_vertex(AdjacencyListObject* self, PyObject* args)
{
    int vertex;

    if (!PyArg_ParseTuple(args, "i", &vertex))
        return NULL;

    if (vertex < 0 || vertex >= 16) {
        PyErr_SetString(PyExc_ValueError, "Vertex out of range");
        return NULL;
    }

    if (!self->g.vertices[vertex]) {
        Py_RETURN_NONE;
    }

    for (int i = 0; i < 16; i++) {
        auto& neighbors = self->g.adj[i];

        neighbors.erase(
            std::remove(neighbors.begin(), neighbors.end(), vertex),
            neighbors.end()
        );
    }

    self->g.adj[vertex].clear();

    self->g.vertices[vertex] = false;

    Py_RETURN_NONE;
}

static PyObject* AdjacencyList_number_of_edges(AdjacencyListObject* self, PyObject* Py_UNUSED(ignored))
{
    int count = 0;

    for (int i = 0; i < 16; i++) {
        count += self->g.adj[i].size();
    }

    count /= 2;

    return PyLong_FromLong(count);
}

static PyObject* AdjacencyList_edges(AdjacencyListObject* self, PyObject* Py_UNUSED(ignored))
{
    PyObject* result = PySet_New(NULL);

    for (int u = 0; u < 16; u++) {
        if (!self->g.vertices[u]) continue;

        for (int v : self->g.adj[u]) {
            if (u < v) {
                PyObject* tuple = PyTuple_New(2);

                PyTuple_SetItem(tuple, 0, PyLong_FromLong(u));
                PyTuple_SetItem(tuple, 1, PyLong_FromLong(v));

                PySet_Add(result, tuple);
                Py_DECREF(tuple);
            }
        }
    }

    return result;

}

static PyObject* AdjacencyList_is_edge(AdjacencyListObject* self, PyObject* args)
{
    int u, v;

    if (!PyArg_ParseTuple(args, "ii", &u, &v))
        return NULL;

    if (u < 0 || u >= 16 || v < 0 || v >= 16) {
        PyErr_SetString(PyExc_ValueError, "Vertex out of range");
        return NULL;
    }

    for (int neighbor : self->g.adj[u]) {
        if (neighbor == v) {
            Py_RETURN_TRUE;
        }
    }

    Py_RETURN_FALSE;
}

static PyObject* AdjacencyList_add_edge(AdjacencyListObject* self, PyObject* args)
{
    int u, v;

    if (!PyArg_ParseTuple(args, "ii", &u, &v))
        return NULL;

    if (u < 0 || u >= 16 || v < 0 || v >= 16) {
        PyErr_SetString(PyExc_ValueError, "Vertex out of range");
        return NULL;
    }

    if (!self->g.vertices[u] || !self->g.vertices[v]) {
        PyErr_SetString(PyExc_ValueError, "Vertex not in graph");
        return NULL;
    }

    for (int x : self->g.adj[u]) {
        if (x == v)
            Py_RETURN_NONE;
    }

    self->g.adj[u].push_back(v);
    self->g.adj[v].push_back(u);

    Py_RETURN_NONE;
}

static PyObject* AdjacencyList_delete_edge(AdjacencyListObject* self, PyObject* args)
{
    int u, v;

    if (!PyArg_ParseTuple(args, "ii", &u, &v))
        return NULL;

    if (u < 0 || u >= 16 || v < 0 || v >= 16) {
        PyErr_SetString(PyExc_ValueError, "Vertex out of range");
        return NULL;
    }

    auto& nu = self->g.adj[u];
    nu.erase(std::remove(nu.begin(), nu.end(), v), nu.end());

    auto& nv = self->g.adj[v];
    nv.erase(std::remove(nv.begin(), nv.end(), u), nv.end());

    Py_RETURN_NONE;
}


static PyTypeObject AdjacencyListType = {
    PyVarObject_HEAD_INIT(NULL, 0)
};

//additional method
static PyObject* AdjacencyList_create_path(PyObject* self, PyObject* args)
{
    int n;

    if (!PyArg_ParseTuple(args, "i", &n))
        return NULL;

    PyObject* obj = PyObject_CallObject((PyObject*)&AdjacencyListType, NULL);
    if (!obj) return NULL;

    AdjacencyListObject* g = (AdjacencyListObject*)obj;

    for (int i = 0; i < n; i++) {
        g->g.vertices[i] = true;
    }

    for (int i = 1; i < n; i++) {
        g->g.adj[i - 1].push_back(i);
        g->g.adj[i].push_back(i - 1);
    }

    return obj;
}

static PyModuleDef simple_graphs_module = {
    PyModuleDef_HEAD_INIT,
    "simple_graphs",
    "Module for simple graphs",
    -1,  //stateless
    NULL, NULL, NULL, NULL, NULL
};

static PyMethodDef AdjacencyList_methods[] = {
    {"add_vertex", (PyCFunction)AdjacencyList_add_vertex, METH_VARARGS, "Add vertex"},
    {"add_edge", (PyCFunction)AdjacencyList_add_edge, METH_VARARGS, "Add edge"},
    {"number_of_vertices", (PyCFunction)AdjacencyList_number_of_vertices, METH_NOARGS, "Number of vertices"},
    {"number_of_edges", (PyCFunction)AdjacencyList_number_of_edges, METH_NOARGS, "Number of edges"},
    {"is_edge", (PyCFunction)AdjacencyList_is_edge, METH_VARARGS, "Check edge"},
    {"vertex_neighbors", (PyCFunction)AdjacencyList_vertex_neighbors, METH_VARARGS, "Neighbors"},
    {"vertex_degree", (PyCFunction)AdjacencyList_vertex_degree, METH_VARARGS, "Degree"},
    {"delete_edge", (PyCFunction)AdjacencyList_delete_edge, METH_VARARGS, "Delete edge"},
    {"delete_vertex", (PyCFunction)AdjacencyList_delete_vertex, METH_VARARGS, "Delete vertex"},
    {"vertices", (PyCFunction)AdjacencyList_vertices, METH_NOARGS, "Vertices"},
    {"edges", (PyCFunction)AdjacencyList_edges, METH_NOARGS, "Edges"},
    {"create_path", (PyCFunction)AdjacencyList_create_path, METH_VARARGS | METH_STATIC, "Create path"},
    {NULL, NULL, 0, NULL}
};

PyMODINIT_FUNC PyInit_simple_graphs(void)
{
    PyObject* m;

    AdjacencyListType.tp_name = "simple_graphs.AdjacencyList";
    AdjacencyListType.tp_basicsize = sizeof(AdjacencyListObject);
    AdjacencyListType.tp_flags = Py_TPFLAGS_DEFAULT;
    AdjacencyListType.tp_doc = "Adjacency list graph";
    AdjacencyListType.tp_init = (initproc)AdjacencyList_init;
    AdjacencyListType.tp_new = PyType_GenericNew;
    AdjacencyListType.tp_methods = AdjacencyList_methods;

    if (PyType_Ready(&AdjacencyListType) < 0)
        return NULL;

    m = PyModule_Create(&simple_graphs_module);

    Py_INCREF(&AdjacencyListType);
    PyModule_AddObject(m, "AdjacencyList", (PyObject*)&AdjacencyListType);

    return m;
}

