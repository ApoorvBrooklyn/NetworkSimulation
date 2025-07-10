#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <string>
#include <vector>

namespace py = pybind11;

// Include all the protocol implementations
#include "tcp_tahoe.cpp"
#include "stop_and_wait.cpp"
#include "prims_algorithm.cpp"
#include "crc.cpp"

PYBIND11_MODULE(network_protocols, m) {
    m.doc() = "Network Protocol Simulator with TCP Tahoe, Stop-and-Wait, Prim's Algorithm, and CRC";
    
    // TCP Tahoe binding
    py::class_<TCPTahoe>(m, "TCPTahoe")
        .def(py::init<>())
        .def("send_packet", &TCPTahoe::send_packet)
        .def("timeout_event", &TCPTahoe::timeout_event)
        .def("duplicate_ack", &TCPTahoe::duplicate_ack)
        .def("get_current_cwnd", &TCPTahoe::get_current_cwnd)
        .def("get_current_ssthresh", &TCPTahoe::get_current_ssthresh)
        .def("get_cwnd_history", &TCPTahoe::get_cwnd_history)
        .def("get_ssthresh_history", &TCPTahoe::get_ssthresh_history)
        .def("get_state_history", &TCPTahoe::get_state_history)
        .def("get_current_state", &TCPTahoe::get_current_state);
    
    // Stop and Wait binding
    py::class_<StopAndWait>(m, "StopAndWait")
        .def(py::init<>())
        .def("send_packet", &StopAndWait::send_packet)
        .def("receive_ack", &StopAndWait::receive_ack)
        .def("check_timeout", &StopAndWait::check_timeout)
        .def("retransmit", &StopAndWait::retransmit)
        .def("get_total_packets_sent", &StopAndWait::get_total_packets_sent)
        .def("get_total_packets_acked", &StopAndWait::get_total_packets_acked)
        .def("get_total_timeouts", &StopAndWait::get_total_timeouts)
        .def("is_waiting_for_ack", &StopAndWait::is_waiting_for_ack)
        .def("get_current_sequence", &StopAndWait::get_current_sequence)
        .def("get_packet_history", &StopAndWait::get_packet_history)
        .def("get_ack_history", &StopAndWait::get_ack_history)
        .def("get_rtt_history", &StopAndWait::get_rtt_history);
    
    // Prim's Algorithm binding
    py::class_<PrimsAlgorithm>(m, "PrimsAlgorithm")
        .def(py::init<int>())
        .def("add_edge", &PrimsAlgorithm::add_edge)
        .def("find_mst", &PrimsAlgorithm::find_mst)
        .def("get_total_weight", &PrimsAlgorithm::get_total_weight)
        .def("get_vertex_count", &PrimsAlgorithm::get_vertex_count)
        .def("get_mst_edges", &PrimsAlgorithm::get_mst_edges)
        .def("get_graph", &PrimsAlgorithm::get_graph);
    
    // CRC binding
    py::class_<CRC>(m, "CRC")
        .def(py::init<const std::string&>())
        .def("calculate_crc", &CRC::calculate_crc)
        .def("verify_crc", &CRC::verify_crc)
        .def("add_crc_to_data", &CRC::add_crc_to_data)
        .def("remove_crc_from_data", &CRC::remove_crc_from_data);
    
    // Edge struct binding
    py::class_<Edge>(m, "Edge")
        .def(py::init<int, int, int>())
        .def_readwrite("src", &Edge::src)
        .def_readwrite("dest", &Edge::dest)
        .def_readwrite("weight", &Edge::weight);
} 