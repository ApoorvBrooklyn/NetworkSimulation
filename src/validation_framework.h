#ifndef VALIDATION_FRAMEWORK_H
#define VALIDATION_FRAMEWORK_H

#include <vector>
#include <string>
#include <memory>
#include <map>
#include <functional>
#include <chrono>

enum class ValidationLevel {
    BASIC,
    STANDARD,
    COMPREHENSIVE,
    EXHAUSTIVE
};

enum class ValidationResult {
    PASS,
    FAIL,
    WARNING,
    SKIPPED
};

enum class ProtocolType {
    TCP_TAHOE,
    STOP_AND_WAIT,
    CRC,
    CROSS_LAYER,
    LTE
};

struct ValidationTest {
    std::string test_id;
    std::string test_name;
    std::string description;
    ProtocolType protocol;
    ValidationLevel level;
    std::function<ValidationResult()> test_function;
    double execution_time_ms;
    ValidationResult result;
    std::string error_message;
    std::map<std::string, double> metrics;
};

struct PerformanceMetrics {
    double throughput_mbps;
    double latency_ms;
    double packet_loss_rate;
    double jitter_ms;
    double cpu_usage_percent;
    double memory_usage_mb;
    double energy_consumption_mw;
    uint64_t timestamp;
};

struct ConformanceCheck {
    std::string check_id;
    std::string standard_reference;  // RFC number or standard
    std::string requirement;
    ValidationResult result;
    std::string details;
};

class ValidationFramework {
private:
    std::vector<ValidationTest> test_suite;
    std::vector<PerformanceMetrics> performance_history;
    std::vector<ConformanceCheck> conformance_checks;
    
    ValidationLevel current_level;
    bool continuous_validation_enabled;
    double performance_threshold_throughput;
    double performance_threshold_latency;
    double performance_threshold_packet_loss;
    
    // Protocol instances for testing
    std::shared_ptr<void> tcp_instance;
    std::shared_ptr<void> stop_wait_instance;
    std::shared_ptr<void> crc_instance;
    std::shared_ptr<void> cross_layer_instance;
    std::shared_ptr<void> lte_instance;
    
    // Test statistics
    int tests_passed;
    int tests_failed;
    int tests_warnings;
    int tests_skipped;

public:
    ValidationFramework();
    
    // Test suite management
    void add_test(const ValidationTest& test);
    void remove_test(const std::string& test_id);
    void set_validation_level(ValidationLevel level);
    std::vector<ValidationTest> get_test_suite() const;
    
    // Protocol instance registration
    void register_protocol_instance(ProtocolType protocol, std::shared_ptr<void> instance);
    
    // Test execution
    ValidationResult run_all_tests();
    ValidationResult run_test(const std::string& test_id);
    ValidationResult run_tests_for_protocol(ProtocolType protocol);
    void run_continuous_validation();
    void stop_continuous_validation();
    
    // Packet integrity validation
    ValidationResult validate_packet_integrity(const std::string& data, const std::string& received_data);
    ValidationResult validate_crc_integrity(const std::string& data, const std::string& crc);
    ValidationResult validate_tcp_sequence_numbers(const std::vector<int>& sequence_numbers);
    ValidationResult validate_stop_and_wait_acks(const std::vector<int>& ack_numbers);
    
    // Protocol conformance validation
    ValidationResult validate_tcp_tahoe_conformance();
    ValidationResult validate_stop_and_wait_conformance();
    ValidationResult validate_crc_conformance();
    ValidationResult validate_cross_layer_conformance();
    ValidationResult validate_lte_conformance();
    
    // Performance validation
    ValidationResult validate_throughput_performance(double measured_throughput);
    ValidationResult validate_latency_performance(double measured_latency);
    ValidationResult validate_packet_loss_performance(double measured_loss_rate);
    ValidationResult validate_energy_efficiency(double energy_consumption);
    
    // Performance metrics collection
    void collect_performance_metrics();
    PerformanceMetrics get_current_performance_metrics() const;
    std::vector<PerformanceMetrics> get_performance_history() const;
    void set_performance_thresholds(double throughput, double latency, double packet_loss);
    
    // Conformance checking
    void add_conformance_check(const ConformanceCheck& check);
    std::vector<ConformanceCheck> run_conformance_checks();
    ValidationResult check_rfc_compliance(const std::string& rfc_number);
    
    // Stress testing
    ValidationResult run_stress_test(int num_packets, double load_factor);
    ValidationResult run_endurance_test(int duration_minutes);
    ValidationResult run_boundary_test();
    ValidationResult run_fault_injection_test();
    
    // Network condition testing
    ValidationResult test_under_high_latency(int latency_ms);
    ValidationResult test_under_packet_loss(double loss_rate);
    ValidationResult test_under_jitter(double jitter_ms);
    ValidationResult test_under_congestion();
    
    // Security validation
    ValidationResult validate_data_integrity();
    ValidationResult validate_error_detection_capability();
    ValidationResult validate_protocol_robustness();
    
    // Reporting and statistics
    std::map<std::string, int> get_test_statistics() const;
    std::string generate_validation_report() const;
    std::string generate_performance_report() const;
    std::string generate_conformance_report() const;
    double get_overall_pass_rate() const;
    
    // Test result analysis
    std::vector<ValidationTest> get_failed_tests() const;
    std::vector<ValidationTest> get_warning_tests() const;
    std::map<ProtocolType, double> get_protocol_pass_rates() const;
    
    // Configuration
    void load_test_configuration(const std::string& config_file);
    void save_test_results(const std::string& output_file);
    void enable_detailed_logging(bool enable);
    
    // Reset and cleanup
    void reset_test_results();
    void clear_performance_history();
    void reset_validation_framework();
};

// Specific test implementations
class TCPTahoeValidation {
public:
    static ValidationResult test_slow_start_behavior();
    static ValidationResult test_congestion_avoidance();
    static ValidationResult test_fast_retransmit();
    static ValidationResult test_timeout_handling();
    static ValidationResult test_window_size_limits();
};

class StopAndWaitValidation {
public:
    static ValidationResult test_sequence_number_alternation();
    static ValidationResult test_ack_handling();
    static ValidationResult test_timeout_retransmission();
    static ValidationResult test_duplicate_detection();
};

class CRCValidation {
public:
    static ValidationResult test_error_detection();
    static ValidationResult test_polynomial_calculation();
    static ValidationResult test_false_positive_rate();
    static ValidationResult test_burst_error_detection();
};

class LTEValidation {
public:
    static ValidationResult test_handover_procedures();
    static ValidationResult test_resource_allocation();
    static ValidationResult test_signal_measurements();
    static ValidationResult test_mobility_handling();
};

#endif // VALIDATION_FRAMEWORK_H 