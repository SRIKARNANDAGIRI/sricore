#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "srichakra/complexity.hpp"
#include "srichakra/engine.hpp"
#include "srichakra/schema.hpp"

namespace py = pybind11;

PYBIND11_MODULE(_srichakra, m) {
    m.doc() = "Python bindings for srichakra";

    py::enum_<srichakra::State>(m, "State")
        .value("Idle", srichakra::State::Idle)
        .value("Running", srichakra::State::Running)
        .value("Paused", srichakra::State::Paused)
        .value("Completed", srichakra::State::Completed)
        .value("Error", srichakra::State::Error);

    py::class_<srichakra::GraphNode>(m, "GraphNode")
        .def(py::init<>())
        .def_readwrite("id", &srichakra::GraphNode::id)
        .def_readwrite("label", &srichakra::GraphNode::label);

    py::class_<srichakra::GraphEdge>(m, "GraphEdge")
        .def(py::init<>())
        .def_readwrite("id", &srichakra::GraphEdge::id)
        .def_readwrite("from", &srichakra::GraphEdge::from)
        .def_readwrite("to", &srichakra::GraphEdge::to)
        .def_readwrite("weight", &srichakra::GraphEdge::weight);

    py::class_<srichakra::Graph>(m, "Graph")
        .def(py::init<>())
        .def_readwrite("nodes", &srichakra::Graph::nodes)
        .def_readwrite("edges", &srichakra::Graph::edges);

    py::class_<srichakra::ComplexityMetrics>(m, "ComplexityMetrics")
        .def_readonly("node_count", &srichakra::ComplexityMetrics::node_count)
        .def_readonly("edge_count", &srichakra::ComplexityMetrics::edge_count)
        .def_readonly("density", &srichakra::ComplexityMetrics::density);

    py::class_<srichakra::Engine>(m, "Engine")
        .def(py::init<>())
        .def("load", &srichakra::Engine::load)
        .def("complexity", &srichakra::Engine::complexity);

    m.def("analyze", &srichakra::analyze);
    m.def("graph_to_json", &srichakra::graph_to_json);
    m.def("graph_from_json", &srichakra::graph_from_json);
}
