#ifndef TCP_TAHOE_H
#define TCP_TAHOE_H

#include <vector>
#include <string>
#include <chrono>
#include <memory>

enum class CongestionAlgorithm {
    TAHOE,
    RENO,
    CUBIC,
    BBR
};

enum class TCPState {
    SLOW_START,
    CONGESTION_AVOIDANCE,
    FAST_RECOVERY,
    TIMEOUT
};

class TCPTahoe {
private:
    int cwnd;                           // Congestion window size
    int ssthresh;                       // Slow start threshold
    int rtt;                            // Round trip time
    int timeout;                        // Timeout value
    int duplicate_ack_count;            // Count of duplicate ACKs
    bool in_slow_start;                 // Whether in slow start phase
    CongestionAlgorithm algorithm;      // Current congestion algorithm
    TCPState current_state;             // Current TCP state
    
    // History tracking
    std::vector<int> cwnd_history;
    std::vector<int> ssthresh_history;
    std::vector<std::string> state_history;
    std::vector<double> throughput_history;
    std::vector<int> rtt_history;
    
    // Advanced congestion control parameters
    double cubic_beta;                  // CUBIC beta parameter
    double cubic_c;                     // CUBIC C parameter
    uint64_t last_cwnd_reduction_time;  // Time of last cwnd reduction
    double bbr_min_rtt;                 // BBR minimum RTT
    double bbr_max_bandwidth;           // BBR maximum bandwidth
    
    // Network conditions
    double packet_loss_rate;
    double network_utilization;
    int queue_delay;

public:
    TCPTahoe(CongestionAlgorithm algo = CongestionAlgorithm::TAHOE);
    
    // Core protocol methods
    void send_packet();
    void timeout_event();
    void duplicate_ack();
    void receive_ack(int ack_num);
    
    // Congestion control algorithms
    void tahoe_congestion_control();
    void reno_congestion_control();
    void cubic_congestion_control();
    void bbr_congestion_control();
    
    // Network condition simulation
    void set_network_conditions(double loss_rate, double utilization, int delay);
    void simulate_network_congestion();
    void adaptive_congestion_response();
    
    // Getters
    int get_current_cwnd() const;
    int get_current_ssthresh() const;
    std::string get_current_state() const;
    CongestionAlgorithm get_algorithm() const;
    std::vector<int> get_cwnd_history() const;
    std::vector<int> get_ssthresh_history() const;
    std::vector<std::string> get_state_history() const;
    std::vector<double> get_throughput_history() const;
    double get_current_throughput() const;
    double get_packet_loss_rate() const;
    double get_network_utilization() const;
    
    // Setters
    void set_algorithm(CongestionAlgorithm algo);
    void reset();
};

#endif // TCP_TAHOE_H 