#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <string>
#include <vector>
#include <memory>

namespace py = pybind11;

// Include all protocol implementations
#include "tcp_tahoe.h"
#include "tcp_tahoe_enhanced.cpp"
#include "cross_layer_protocol.h"
#include "cross_layer_protocol.cpp"
#include "lte_network.h"
#include "lte_network_core.cpp"
#include "validation_framework.h"
#include "network_logger.h"

PYBIND11_MODULE(network_protocols_enhanced, m) {
    m.doc() = "Enhanced Network Protocol Simulator with TCP variants, Cross-layer optimization, LTE, Validation, and Logging";
    
    // Enums
    py::enum_<CongestionAlgorithm>(m, "CongestionAlgorithm")
        .value("TAHOE", CongestionAlgorithm::TAHOE)
        .value("RENO", CongestionAlgorithm::RENO)
        .value("CUBIC", CongestionAlgorithm::CUBIC)
        .value("BBR", CongestionAlgorithm::BBR);
    
    py::enum_<TCPState>(m, "TCPState")
        .value("SLOW_START", TCPState::SLOW_START)
        .value("CONGESTION_AVOIDANCE", TCPState::CONGESTION_AVOIDANCE)
        .value("FAST_RECOVERY", TCPState::FAST_RECOVERY)
        .value("TIMEOUT", TCPState::TIMEOUT);
    
    py::enum_<LayerType>(m, "LayerType")
        .value("PHYSICAL", LayerType::PHYSICAL)
        .value("DATA_LINK", LayerType::DATA_LINK)
        .value("NETWORK", LayerType::NETWORK)
        .value("TRANSPORT", LayerType::TRANSPORT)
        .value("APPLICATION", LayerType::APPLICATION);
    
    py::enum_<CrossLayerEvent>(m, "CrossLayerEvent")
        .value("SIGNAL_STRENGTH_CHANGE", CrossLayerEvent::SIGNAL_STRENGTH_CHANGE)
        .value("HANDOVER_INITIATION", CrossLayerEvent::HANDOVER_INITIATION)
        .value("CONGESTION_DETECTED", CrossLayerEvent::CONGESTION_DETECTED)
        .value("ERROR_RATE_CHANGE", CrossLayerEvent::ERROR_RATE_CHANGE)
        .value("BANDWIDTH_CHANGE", CrossLayerEvent::BANDWIDTH_CHANGE)
        .value("LATENCY_CHANGE", CrossLayerEvent::LATENCY_CHANGE);
    
    py::enum_<LTEState>(m, "LTEState")
        .value("IDLE", LTEState::IDLE)
        .value("CONNECTED", LTEState::CONNECTED)
        .value("HANDOVER_PREPARATION", LTEState::HANDOVER_PREPARATION)
        .value("HANDOVER_EXECUTION", LTEState::HANDOVER_EXECUTION)
        .value("HANDOVER_COMPLETION", LTEState::HANDOVER_COMPLETION);
    
    py::enum_<HandoverType>(m, "HandoverType")
        .value("INTRA_LTE", HandoverType::INTRA_LTE)
        .value("INTER_LTE", HandoverType::INTER_LTE)
        .value("LTE_TO_3G", HandoverType::LTE_TO_3G)
        .value("LTE_TO_WIFI", HandoverType::LTE_TO_WIFI);
    
    py::enum_<ValidationLevel>(m, "ValidationLevel")
        .value("BASIC", ValidationLevel::BASIC)
        .value("STANDARD", ValidationLevel::STANDARD)
        .value("COMPREHENSIVE", ValidationLevel::COMPREHENSIVE)
        .value("EXHAUSTIVE", ValidationLevel::EXHAUSTIVE);
    
    py::enum_<ValidationResult>(m, "ValidationResult")
        .value("PASS", ValidationResult::PASS)
        .value("FAIL", ValidationResult::FAIL)
        .value("WARNING", ValidationResult::WARNING)
        .value("SKIPPED", ValidationResult::SKIPPED);
    
    py::enum_<LogLevel>(m, "LogLevel")
        .value("DEBUG", LogLevel::DEBUG)
        .value("INFO", LogLevel::INFO)
        .value("WARNING", LogLevel::WARNING)
        .value("ERROR", LogLevel::ERROR)
        .value("CRITICAL", LogLevel::CRITICAL);
    
    py::enum_<EventType>(m, "EventType")
        .value("PACKET_SENT", EventType::PACKET_SENT)
        .value("PACKET_RECEIVED", EventType::PACKET_RECEIVED)
        .value("PACKET_DROPPED", EventType::PACKET_DROPPED)
        .value("PACKET_CORRUPTED", EventType::PACKET_CORRUPTED)
        .value("ACK_SENT", EventType::ACK_SENT)
        .value("ACK_RECEIVED", EventType::ACK_RECEIVED)
        .value("TIMEOUT_OCCURRED", EventType::TIMEOUT_OCCURRED)
        .value("RETRANSMISSION", EventType::RETRANSMISSION)
        .value("HANDOVER_START", EventType::HANDOVER_START)
        .value("HANDOVER_COMPLETE", EventType::HANDOVER_COMPLETE)
        .value("CONGESTION_DETECTED", EventType::CONGESTION_DETECTED)
        .value("CONNECTION_ESTABLISHED", EventType::CONNECTION_ESTABLISHED)
        .value("CONNECTION_CLOSED", EventType::CONNECTION_CLOSED)
        .value("ERROR_DETECTED", EventType::ERROR_DETECTED)
        .value("PROTOCOL_STATE_CHANGE", EventType::PROTOCOL_STATE_CHANGE)
        .value("PERFORMANCE_MEASUREMENT", EventType::PERFORMANCE_MEASUREMENT);
    
    // Enhanced TCP Tahoe binding
    py::class_<TCPTahoe>(m, "TCPTahoe")
        .def(py::init<CongestionAlgorithm>(), py::arg("algorithm") = CongestionAlgorithm::TAHOE)
        .def("send_packet", &TCPTahoe::send_packet)
        .def("timeout_event", &TCPTahoe::timeout_event)
        .def("duplicate_ack", &TCPTahoe::duplicate_ack)
        .def("receive_ack", &TCPTahoe::receive_ack)
        .def("set_network_conditions", &TCPTahoe::set_network_conditions)
        .def("simulate_network_congestion", &TCPTahoe::simulate_network_congestion)
        .def("adaptive_congestion_response", &TCPTahoe::adaptive_congestion_response)
        .def("get_current_cwnd", &TCPTahoe::get_current_cwnd)
        .def("get_current_ssthresh", &TCPTahoe::get_current_ssthresh)
        .def("get_current_state", &TCPTahoe::get_current_state)
        .def("get_algorithm", &TCPTahoe::get_algorithm)
        .def("get_cwnd_history", &TCPTahoe::get_cwnd_history)
        .def("get_ssthresh_history", &TCPTahoe::get_ssthresh_history)
        .def("get_state_history", &TCPTahoe::get_state_history)
        .def("get_throughput_history", &TCPTahoe::get_throughput_history)
        .def("get_current_throughput", &TCPTahoe::get_current_throughput)
        .def("get_packet_loss_rate", &TCPTahoe::get_packet_loss_rate)
        .def("get_network_utilization", &TCPTahoe::get_network_utilization)
        .def("set_algorithm", &TCPTahoe::set_algorithm)
        .def("reset", &TCPTahoe::reset);
    
    // Structs and data classes
    py::class_<LayerInfo>(m, "LayerInfo")
        .def(py::init<>())
        .def_readwrite("layer", &LayerInfo::layer)
        .def_readwrite("status", &LayerInfo::status)
        .def_readwrite("metrics", &LayerInfo::metrics)
        .def_readwrite("timestamp", &LayerInfo::timestamp);
    
    py::class_<CrossLayerMessage>(m, "CrossLayerMessage")
        .def(py::init<>())
        .def_readwrite("source", &CrossLayerMessage::source)
        .def_readwrite("destination", &CrossLayerMessage::destination)
        .def_readwrite("event", &CrossLayerMessage::event)
        .def_readwrite("parameters", &CrossLayerMessage::parameters)
        .def_readwrite("timestamp", &CrossLayerMessage::timestamp)
        .def_readwrite("message", &CrossLayerMessage::message);
    
    py::class_<CellInfo>(m, "CellInfo")
        .def(py::init<>())
        .def_readwrite("cell_id", &CellInfo::cell_id)
        .def_readwrite("signal_strength", &CellInfo::signal_strength)
        .def_readwrite("signal_quality", &CellInfo::signal_quality)
        .def_readwrite("interference_level", &CellInfo::interference_level)
        .def_readwrite("load_percentage", &CellInfo::load_percentage)
        .def_readwrite("technology", &CellInfo::technology)
        .def_readwrite("latitude", &CellInfo::latitude)
        .def_readwrite("longitude", &CellInfo::longitude);
    
    py::class_<UserEquipment>(m, "UserEquipment")
        .def(py::init<>())
        .def_readwrite("ue_id", &UserEquipment::ue_id)
        .def_readwrite("x_position", &UserEquipment::x_position)
        .def_readwrite("y_position", &UserEquipment::y_position)
        .def_readwrite("velocity", &UserEquipment::velocity)
        .def_readwrite("direction", &UserEquipment::direction)
        .def_readwrite("serving_cell", &UserEquipment::serving_cell)
        .def_readwrite("state", &UserEquipment::state)
        .def_readwrite("current_throughput", &UserEquipment::current_throughput)
        .def_readwrite("battery_level", &UserEquipment::battery_level);
    
    py::class_<HandoverEvent>(m, "HandoverEvent")
        .def(py::init<>())
        .def_readwrite("source_cell", &HandoverEvent::source_cell)
        .def_readwrite("target_cell", &HandoverEvent::target_cell)
        .def_readwrite("type", &HandoverEvent::type)
        .def_readwrite("trigger_rsrp", &HandoverEvent::trigger_rsrp)
        .def_readwrite("target_rsrp", &HandoverEvent::target_rsrp)
        .def_readwrite("start_time", &HandoverEvent::start_time)
        .def_readwrite("completion_time", &HandoverEvent::completion_time)
        .def_readwrite("success", &HandoverEvent::success)
        .def_readwrite("failure_reason", &HandoverEvent::failure_reason);
    
    py::class_<LogEntry>(m, "LogEntry")
        .def(py::init<>())
        .def_readwrite("timestamp", &LogEntry::timestamp)
        .def_readwrite("level", &LogEntry::level)
        .def_readwrite("event_type", &LogEntry::event_type)
        .def_readwrite("protocol", &LogEntry::protocol)
        .def_readwrite("source", &LogEntry::source)
        .def_readwrite("destination", &LogEntry::destination)
        .def_readwrite("message", &LogEntry::message)
        .def_readwrite("metadata", &LogEntry::metadata)
        .def_readwrite("sequence_number", &LogEntry::sequence_number)
        .def_readwrite("packet_size", &LogEntry::packet_size)
        .def_readwrite("latency_ms", &LogEntry::latency_ms)
        .def_readwrite("packet_data", &LogEntry::packet_data);
    
    // Cross Layer Optimizer binding
    py::class_<CrossLayerOptimizer>(m, "CrossLayerOptimizer")
        .def(py::init<>())
        .def("register_layer", &CrossLayerOptimizer::register_layer)
        .def("update_layer_state", &CrossLayerOptimizer::update_layer_state)
        .def("get_layer_state", &CrossLayerOptimizer::get_layer_state)
        .def("send_cross_layer_message", &CrossLayerOptimizer::send_cross_layer_message)
        .def("register_event_handler", &CrossLayerOptimizer::register_event_handler)
        .def("get_message_history", &CrossLayerOptimizer::get_message_history)
        .def("enable_adaptive_optimization", &CrossLayerOptimizer::enable_adaptive_optimization)
        .def("set_optimization_weights", &CrossLayerOptimizer::set_optimization_weights)
        .def("optimize_network_performance", &CrossLayerOptimizer::optimize_network_performance)
        .def("adapt_to_network_conditions", &CrossLayerOptimizer::adapt_to_network_conditions)
        .def("get_current_throughput", &CrossLayerOptimizer::get_current_throughput)
        .def("get_current_latency", &CrossLayerOptimizer::get_current_latency)
        .def("get_current_energy_consumption", &CrossLayerOptimizer::get_current_energy_consumption)
        .def("get_current_packet_loss_rate", &CrossLayerOptimizer::get_current_packet_loss_rate)
        .def("get_throughput_history", &CrossLayerOptimizer::get_throughput_history)
        .def("get_latency_history", &CrossLayerOptimizer::get_latency_history)
        .def("simulate_mobility", &CrossLayerOptimizer::simulate_mobility)
        .def("simulate_interference", &CrossLayerOptimizer::simulate_interference)
        .def("simulate_traffic_variation", &CrossLayerOptimizer::simulate_traffic_variation)
        .def("reset", &CrossLayerOptimizer::reset)
        .def("clear_history", &CrossLayerOptimizer::clear_history);
    
    // LTE Network binding
    py::class_<LTENetwork>(m, "LTENetwork")
        .def(py::init<>())
        .def("initialize_network", &LTENetwork::initialize_network)
        .def("get_user_info", &LTENetwork::get_user_info)
        .def("get_cell_info", &LTENetwork::get_cell_info)
        .def("update_user_position", &LTENetwork::update_user_position)
        .def("calculate_rsrp", &LTENetwork::calculate_rsrp)
        .def("should_trigger_handover", &LTENetwork::should_trigger_handover)
        .def("initiate_handover", &LTENetwork::initiate_handover)
        .def("set_handover_parameters", &LTENetwork::set_handover_parameters)
        .def("get_network_throughput", &LTENetwork::get_network_throughput)
        .def("get_active_users_count", &LTENetwork::get_active_users_count)
        .def("get_handover_history", &LTENetwork::get_handover_history)
        .def("step_simulation", &LTENetwork::step_simulation);
    
    // Validation Framework (simplified interface)
    py::class_<ValidationFramework>(m, "ValidationFramework")
        .def(py::init<>())
        .def("set_validation_level", &ValidationFramework::set_validation_level)
        .def("run_all_tests", &ValidationFramework::run_all_tests)
        .def("validate_packet_integrity", &ValidationFramework::validate_packet_integrity)
        .def("validate_tcp_tahoe_conformance", &ValidationFramework::validate_tcp_tahoe_conformance)
        .def("validate_throughput_performance", &ValidationFramework::validate_throughput_performance)
        .def("validate_latency_performance", &ValidationFramework::validate_latency_performance)
        .def("validate_packet_loss_performance", &ValidationFramework::validate_packet_loss_performance)
        .def("get_overall_pass_rate", &ValidationFramework::get_overall_pass_rate)
        .def("generate_validation_report", &ValidationFramework::generate_validation_report)
        .def("reset_validation_framework", &ValidationFramework::reset_validation_framework);
    
    // Network Logger (simplified interface)
    py::class_<NetworkLogger>(m, "NetworkLogger")
        .def(py::init<>())
        .def("set_log_level", &NetworkLogger::set_log_level)
        .def("enable_console_logging", &NetworkLogger::enable_console_logging)
        .def("enable_file_logging", &NetworkLogger::enable_file_logging)
        .def("log_info", &NetworkLogger::log_info)
        .def("log_warning", &NetworkLogger::log_warning)
        .def("log_error", &NetworkLogger::log_error)
        .def("log_event", &NetworkLogger::log_event)
        .def("log_packet_event", &NetworkLogger::log_packet_event)
        .def("start_packet_trace", &NetworkLogger::start_packet_trace)
        .def("complete_packet_trace", &NetworkLogger::complete_packet_trace)
        .def("start_connection_log", &NetworkLogger::start_connection_log)
        .def("end_connection_log", &NetworkLogger::end_connection_log)
        .def("get_logs", &NetworkLogger::get_logs)
        .def("calculate_average_throughput", &NetworkLogger::calculate_average_throughput)
        .def("calculate_average_latency", &NetworkLogger::calculate_average_latency)
        .def("calculate_packet_loss_rate", &NetworkLogger::calculate_packet_loss_rate)
        .def("generate_summary_report", &NetworkLogger::generate_summary_report)
        .def("export_logs_to_csv", &NetworkLogger::export_logs_to_csv)
        .def("clear_logs", &NetworkLogger::clear_logs);
} 