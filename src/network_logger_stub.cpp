#include "network_logger.h"
#include <iostream>
#include <sstream>

NetworkLogger::NetworkLogger() {
    min_log_level = LogLevel::INFO;
    console_logging_enabled = true;
    file_logging_enabled = false;
    real_time_logging = false;
    log_file_path = "network.log";
    performance_monitoring_enabled = false;
    performance_monitoring_interval_ms = 1000;
    async_logging_enabled = false;
    max_buffer_size = 10000;
    next_packet_id = 1;
    total_bytes_logged = 0;
    logging_start_time = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
}

NetworkLogger::~NetworkLogger() {
    if (log_file.is_open()) {
        log_file.close();
    }
}

void NetworkLogger::set_log_level(LogLevel level) {
    min_log_level = level;
}

void NetworkLogger::enable_console_logging(bool enable) {
    console_logging_enabled = enable;
}

void NetworkLogger::enable_file_logging(bool enable, const std::string& file_path) {
    file_logging_enabled = enable;
    if (!file_path.empty()) {
        log_file_path = file_path;
    }
    
    if (enable && !log_file.is_open()) {
        log_file.open(log_file_path, std::ios::app);
    } else if (!enable && log_file.is_open()) {
        log_file.close();
    }
}

void NetworkLogger::log_info(const std::string& message) {
    LogEntry entry;
    entry.timestamp = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
    entry.level = LogLevel::INFO;
    entry.message = message;
    entry.event_type = EventType::PERFORMANCE_MEASUREMENT;
    entry.protocol = "GENERAL";
    
    log_entries.push_back(entry);
    
    if (console_logging_enabled) {
        std::cout << "[INFO] " << message << std::endl;
    }
}

void NetworkLogger::log_warning(const std::string& message) {
    LogEntry entry;
    entry.timestamp = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
    entry.level = LogLevel::WARNING;
    entry.message = message;
    entry.event_type = EventType::ERROR_DETECTED;
    entry.protocol = "GENERAL";
    
    log_entries.push_back(entry);
    
    if (console_logging_enabled) {
        std::cout << "[WARNING] " << message << std::endl;
    }
}

void NetworkLogger::log_error(const std::string& message) {
    LogEntry entry;
    entry.timestamp = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
    entry.level = LogLevel::ERROR;
    entry.message = message;
    entry.event_type = EventType::ERROR_DETECTED;
    entry.protocol = "GENERAL";
    
    log_entries.push_back(entry);
    
    if (console_logging_enabled) {
        std::cout << "[ERROR] " << message << std::endl;
    }
}

void NetworkLogger::log_event(EventType event, const std::string& protocol, 
                             const std::string& message, const std::map<std::string, std::string>& metadata) {
    LogEntry entry;
    entry.timestamp = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
    entry.level = LogLevel::INFO;
    entry.event_type = event;
    entry.protocol = protocol;
    entry.message = message;
    entry.metadata = metadata;
    
    log_entries.push_back(entry);
}

void NetworkLogger::log_packet_event(EventType event, const std::string& protocol,
                                    int sequence_number, int packet_size, const std::string& data) {
    LogEntry entry;
    entry.timestamp = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
    entry.level = LogLevel::DEBUG;
    entry.event_type = event;
    entry.protocol = protocol;
    entry.sequence_number = sequence_number;
    entry.packet_size = packet_size;
    entry.packet_data = data;
    entry.message = "Packet event: " + std::to_string(sequence_number);
    
    log_entries.push_back(entry);
}

uint64_t NetworkLogger::start_packet_trace(const std::string& source_protocol, 
                                          const std::string& destination_protocol,
                                          int packet_size, const std::string& data) {
    PacketTrace trace;
    trace.packet_id = next_packet_id++;
    trace.timestamp_sent = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
    trace.source_protocol = source_protocol;
    trace.destination_protocol = destination_protocol;
    trace.packet_size = packet_size;
    trace.packet_data = data;
    trace.corrupted = false;
    trace.dropped = false;
    trace.retransmission_count = 0;
    
    packet_traces.push_back(trace);
    return trace.packet_id;
}

void NetworkLogger::complete_packet_trace(uint64_t packet_id, uint64_t received_time, double rtt_ms) {
    for (auto& trace : packet_traces) {
        if (trace.packet_id == packet_id) {
            trace.timestamp_received = received_time;
            trace.rtt_ms = rtt_ms;
            break;
        }
    }
}

std::string NetworkLogger::start_connection_log(const std::string& source, const std::string& destination, 
                                               const std::string& protocol) {
    std::string connection_id = source + "->" + destination + ":" + protocol;
    
    ConnectionLog conn_log;
    conn_log.connection_id = connection_id;
    conn_log.start_time = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
    conn_log.source_endpoint = source;
    conn_log.destination_endpoint = destination;
    conn_log.protocol = protocol;
    conn_log.total_packets_sent = 0;
    conn_log.total_packets_received = 0;
    conn_log.total_bytes_sent = 0;
    conn_log.total_bytes_received = 0;
    
    active_connections[connection_id] = conn_log;
    connection_logs.push_back(conn_log);
    
    return connection_id;
}

void NetworkLogger::end_connection_log(const std::string& connection_id) {
    auto it = active_connections.find(connection_id);
    if (it != active_connections.end()) {
        it->second.end_time = std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count();
        active_connections.erase(it);
    }
}

std::vector<LogEntry> NetworkLogger::get_logs(LogLevel min_level, uint64_t since_timestamp) const {
    std::vector<LogEntry> filtered_logs;
    for (const auto& entry : log_entries) {
        if (entry.level >= min_level && entry.timestamp >= since_timestamp) {
            filtered_logs.push_back(entry);
        }
    }
    return filtered_logs;
}

double NetworkLogger::calculate_average_throughput(const std::string& connection_id) const {
    // Simplified calculation
    if (connection_logs.empty()) return 0.0;
    
    double total_throughput = 0.0;
    int count = 0;
    
    for (const auto& conn : connection_logs) {
        if (connection_id.empty() || conn.connection_id == connection_id) {
            total_throughput += conn.throughput_mbps;
            count++;
        }
    }
    
    return count > 0 ? total_throughput / count : 0.0;
}

double NetworkLogger::calculate_average_latency(const std::string& connection_id) const {
    if (packet_traces.empty()) return 0.0;
    
    double total_latency = 0.0;
    int count = 0;
    
    for (const auto& trace : packet_traces) {
        if (connection_id.empty() || true) { // Simplified matching
            total_latency += trace.rtt_ms;
            count++;
        }
    }
    
    return count > 0 ? total_latency / count : 0.0;
}

double NetworkLogger::calculate_packet_loss_rate(const std::string& connection_id) const {
    if (packet_traces.empty()) return 0.0;
    
    int total_packets = 0;
    int dropped_packets = 0;
    
    for (const auto& trace : packet_traces) {
        if (connection_id.empty() || true) { // Simplified matching
            total_packets++;
            if (trace.dropped) {
                dropped_packets++;
            }
        }
    }
    
    return total_packets > 0 ? static_cast<double>(dropped_packets) / total_packets : 0.0;
}

std::string NetworkLogger::generate_summary_report() const {
    std::stringstream report;
    report << "Network Logging Summary Report\n";
    report << "==============================\n";
    report << "Total Log Entries: " << log_entries.size() << "\n";
    report << "Total Packet Traces: " << packet_traces.size() << "\n";
    report << "Total Connections: " << connection_logs.size() << "\n";
    report << "Average Throughput: " << calculate_average_throughput() << " Mbps\n";
    report << "Average Latency: " << calculate_average_latency() << " ms\n";
    report << "Packet Loss Rate: " << (calculate_packet_loss_rate() * 100.0) << "%\n";
    
    return report.str();
}

void NetworkLogger::export_logs_to_csv(const std::string& file_path) const {
    std::ofstream csv_file(file_path);
    if (!csv_file.is_open()) return;
    
    // CSV header
    csv_file << "Timestamp,Level,Event,Protocol,Source,Destination,Message,Sequence,Size,Latency\n";
    
    for (const auto& entry : log_entries) {
        csv_file << entry.timestamp << ","
                << static_cast<int>(entry.level) << ","
                << static_cast<int>(entry.event_type) << ","
                << entry.protocol << ","
                << entry.source << ","
                << entry.destination << ","
                << "\"" << entry.message << "\","
                << entry.sequence_number << ","
                << entry.packet_size << ","
                << entry.latency_ms << "\n";
    }
    
    csv_file.close();
}

void NetworkLogger::clear_logs() {
    log_entries.clear();
    packet_traces.clear();
    connection_logs.clear();
    active_connections.clear();
    event_counts.clear();
    protocol_statistics.clear();
    total_bytes_logged = 0;
} 