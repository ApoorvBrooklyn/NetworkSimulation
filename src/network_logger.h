#ifndef NETWORK_LOGGER_H
#define NETWORK_LOGGER_H

#include <vector>
#include <string>
#include <memory>
#include <map>
#include <fstream>
#include <chrono>
#include <queue>
#include <mutex>

enum class LogLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    CRITICAL
};

enum class EventType {
    PACKET_SENT,
    PACKET_RECEIVED,
    PACKET_DROPPED,
    PACKET_CORRUPTED,
    ACK_SENT,
    ACK_RECEIVED,
    TIMEOUT_OCCURRED,
    RETRANSMISSION,
    HANDOVER_START,
    HANDOVER_COMPLETE,
    CONGESTION_DETECTED,
    CONNECTION_ESTABLISHED,
    CONNECTION_CLOSED,
    ERROR_DETECTED,
    PROTOCOL_STATE_CHANGE,
    PERFORMANCE_MEASUREMENT
};

struct LogEntry {
    uint64_t timestamp;
    LogLevel level;
    EventType event_type;
    std::string protocol;
    std::string source;
    std::string destination;
    std::string message;
    std::map<std::string, std::string> metadata;
    int sequence_number;
    int packet_size;
    double latency_ms;
    std::string packet_data;
};

struct PacketTrace {
    uint64_t packet_id;
    uint64_t timestamp_sent;
    uint64_t timestamp_received;
    std::string source_protocol;
    std::string destination_protocol;
    int packet_size;
    std::string packet_data;
    std::string checksum;
    bool corrupted;
    bool dropped;
    int retransmission_count;
    double rtt_ms;
    std::vector<std::string> path;  // Network path taken
};

struct ConnectionLog {
    std::string connection_id;
    uint64_t start_time;
    uint64_t end_time;
    std::string source_endpoint;
    std::string destination_endpoint;
    std::string protocol;
    int total_packets_sent;
    int total_packets_received;
    int total_bytes_sent;
    int total_bytes_received;
    double average_rtt_ms;
    double throughput_mbps;
    double packet_loss_rate;
    std::vector<LogEntry> events;
    std::vector<PacketTrace> packet_traces;
};

struct PerformanceAnalytics {
    uint64_t measurement_time;
    double instantaneous_throughput;
    double average_throughput;
    double instantaneous_latency;
    double average_latency;
    double jitter;
    double packet_loss_rate;
    double error_rate;
    int active_connections;
    double cpu_usage;
    double memory_usage;
    std::map<std::string, double> protocol_specific_metrics;
};

class NetworkLogger {
private:
    std::vector<LogEntry> log_entries;
    std::vector<PacketTrace> packet_traces;
    std::vector<ConnectionLog> connection_logs;
    std::vector<PerformanceAnalytics> performance_analytics;
    
    // Logging configuration
    LogLevel min_log_level;
    bool console_logging_enabled;
    bool file_logging_enabled;
    bool real_time_logging;
    std::string log_file_path;
    std::ofstream log_file;
    
    // Performance monitoring
    bool performance_monitoring_enabled;
    int performance_monitoring_interval_ms;
    std::chrono::steady_clock::time_point last_performance_measurement;
    
    // Buffering and threading
    std::queue<LogEntry> log_buffer;
    std::mutex log_mutex;
    bool async_logging_enabled;
    size_t max_buffer_size;
    
    // Active connections tracking
    std::map<std::string, ConnectionLog> active_connections;
    uint64_t next_packet_id;
    
    // Analytics
    std::map<std::string, int> event_counts;
    std::map<std::string, double> protocol_statistics;
    uint64_t total_bytes_logged;
    uint64_t logging_start_time;

public:
    NetworkLogger();
    ~NetworkLogger();
    
    // Configuration
    void set_log_level(LogLevel level);
    void enable_console_logging(bool enable);
    void enable_file_logging(bool enable, const std::string& file_path = "");
    void enable_real_time_logging(bool enable);
    void enable_async_logging(bool enable, size_t buffer_size = 10000);
    void enable_performance_monitoring(bool enable, int interval_ms = 1000);
    
    // Basic logging
    void log(LogLevel level, const std::string& message);
    void log_debug(const std::string& message);
    void log_info(const std::string& message);
    void log_warning(const std::string& message);
    void log_error(const std::string& message);
    void log_critical(const std::string& message);
    
    // Event logging
    void log_event(EventType event, const std::string& protocol, 
                   const std::string& message, const std::map<std::string, std::string>& metadata = {});
    void log_packet_event(EventType event, const std::string& protocol,
                         int sequence_number, int packet_size, const std::string& data = "");
    void log_connection_event(const std::string& connection_id, EventType event, const std::string& details);
    
    // Packet tracing
    uint64_t start_packet_trace(const std::string& source_protocol, 
                               const std::string& destination_protocol,
                               int packet_size, const std::string& data);
    void update_packet_trace(uint64_t packet_id, bool corrupted, bool dropped, int retransmissions = 0);
    void complete_packet_trace(uint64_t packet_id, uint64_t received_time, double rtt_ms);
    std::vector<PacketTrace> get_packet_traces(uint64_t since_timestamp = 0) const;
    
    // Connection logging
    std::string start_connection_log(const std::string& source, const std::string& destination, 
                                    const std::string& protocol);
    void update_connection_stats(const std::string& connection_id, int packets_sent, 
                               int packets_received, int bytes_sent, int bytes_received);
    void end_connection_log(const std::string& connection_id);
    std::vector<ConnectionLog> get_connection_logs() const;
    ConnectionLog get_connection_log(const std::string& connection_id) const;
    
    // Performance analytics
    void record_performance_metrics(const PerformanceAnalytics& metrics);
    void measure_current_performance();
    std::vector<PerformanceAnalytics> get_performance_analytics(uint64_t since_timestamp = 0) const;
    PerformanceAnalytics get_latest_performance_metrics() const;
    
    // Protocol-specific logging
    void log_tcp_state_change(const std::string& connection_id, const std::string& old_state, 
                             const std::string& new_state, int cwnd, int ssthresh);
    void log_handover_event(const std::string& ue_id, int source_cell, int target_cell, 
                           bool success, const std::string& reason = "");
    void log_crc_verification(const std::string& data, const std::string& crc, bool verified);
    void log_cross_layer_message(const std::string& source_layer, const std::string& dest_layer, 
                                const std::string& message);
    
    // Query and analysis
    std::vector<LogEntry> get_logs(LogLevel min_level = LogLevel::DEBUG, 
                                  uint64_t since_timestamp = 0) const;
    std::vector<LogEntry> get_logs_by_protocol(const std::string& protocol) const;
    std::vector<LogEntry> get_logs_by_event_type(EventType event_type) const;
    std::map<std::string, int> get_event_statistics() const;
    std::map<std::string, double> get_protocol_statistics() const;
    
    // Network analysis
    double calculate_average_throughput(const std::string& connection_id = "") const;
    double calculate_average_latency(const std::string& connection_id = "") const;
    double calculate_packet_loss_rate(const std::string& connection_id = "") const;
    double calculate_error_rate() const;
    std::map<std::string, int> analyze_error_patterns() const;
    std::vector<std::string> detect_anomalies() const;
    
    // Export and reporting
    void export_logs_to_csv(const std::string& file_path) const;
    void export_logs_to_json(const std::string& file_path) const;
    void export_packet_traces_to_pcap(const std::string& file_path) const;
    std::string generate_summary_report() const;
    std::string generate_performance_report() const;
    std::string generate_error_analysis_report() const;
    
    // Filtering and search
    std::vector<LogEntry> filter_logs(const std::function<bool(const LogEntry&)>& filter) const;
    std::vector<PacketTrace> filter_packet_traces(const std::function<bool(const PacketTrace&)>& filter) const;
    std::vector<LogEntry> search_logs(const std::string& search_term) const;
    
    // Real-time monitoring
    void set_real_time_callback(std::function<void(const LogEntry&)> callback);
    void set_performance_callback(std::function<void(const PerformanceAnalytics&)> callback);
    void set_anomaly_callback(std::function<void(const std::string&)> callback);
    
    // Maintenance and cleanup
    void flush_logs();
    void clear_logs();
    void clear_packet_traces();
    void clear_connection_logs();
    void rotate_log_file();
    void compress_old_logs();
    
    // Statistics
    size_t get_total_log_entries() const;
    size_t get_total_packet_traces() const;
    size_t get_total_connections() const;
    uint64_t get_total_bytes_logged() const;
    double get_logging_duration_seconds() const;
    
private:
    void write_to_file(const LogEntry& entry);
    void write_to_console(const LogEntry& entry);
    std::string format_log_entry(const LogEntry& entry) const;
    std::string get_timestamp_string(uint64_t timestamp) const;
    void process_log_buffer();
    void update_statistics(const LogEntry& entry);
};

// Utility functions for network logging
class NetworkLoggerUtils {
public:
    static std::string format_packet_data(const std::string& data, bool hex_format = false);
    static std::string calculate_packet_checksum(const std::string& data);
    static std::string get_network_path_string(const std::vector<std::string>& path);
    static uint64_t get_current_timestamp_us();
    static std::string bytes_to_human_readable(uint64_t bytes);
    static std::string duration_to_human_readable(uint64_t duration_us);
};

#endif // NETWORK_LOGGER_H 