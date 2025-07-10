#include "tcp_tahoe.h"
#include <iostream>
#include <cmath>
#include <algorithm>
#include <random>

TCPTahoe::TCPTahoe(CongestionAlgorithm algo) {
    cwnd = 1;
    ssthresh = 65535;
    rtt = 100;
    timeout = 200;
    duplicate_ack_count = 0;
    in_slow_start = true;
    algorithm = algo;
    current_state = TCPState::SLOW_START;
    
    // CUBIC parameters
    cubic_beta = 0.7;
    cubic_c = 0.4;
    last_cwnd_reduction_time = 0;
    
    // BBR parameters
    bbr_min_rtt = 100.0;
    bbr_max_bandwidth = 10.0; // Mbps
    
    // Network conditions
    packet_loss_rate = 0.0;
    network_utilization = 0.0;
    queue_delay = 0;
}

void TCPTahoe::send_packet() {
    cwnd_history.push_back(cwnd);
    ssthresh_history.push_back(ssthresh);
    
    // Update throughput calculation
    double current_throughput = calculate_throughput();
    throughput_history.push_back(current_throughput);
    rtt_history.push_back(rtt);
    
    // Apply the selected congestion control algorithm
    switch (algorithm) {
        case CongestionAlgorithm::TAHOE:
            tahoe_congestion_control();
            break;
        case CongestionAlgorithm::RENO:
            reno_congestion_control();
            break;
        case CongestionAlgorithm::CUBIC:
            cubic_congestion_control();
            break;
        case CongestionAlgorithm::BBR:
            bbr_congestion_control();
            break;
    }
    
    // Adaptive response to network conditions
    adaptive_congestion_response();
}

void TCPTahoe::tahoe_congestion_control() {
    if (current_state == TCPState::SLOW_START) {
        state_history.push_back("Slow Start");
        cwnd *= 2;
        if (cwnd >= ssthresh) {
            current_state = TCPState::CONGESTION_AVOIDANCE;
            in_slow_start = false;
        }
    } else {
        state_history.push_back("Congestion Avoidance");
        cwnd += 1;
    }
}

void TCPTahoe::reno_congestion_control() {
    if (current_state == TCPState::SLOW_START) {
        state_history.push_back("Slow Start");
        cwnd *= 2;
        if (cwnd >= ssthresh) {
            current_state = TCPState::CONGESTION_AVOIDANCE;
            in_slow_start = false;
        }
    } else if (current_state == TCPState::CONGESTION_AVOIDANCE) {
        state_history.push_back("Congestion Avoidance");
        cwnd += 1.0 / cwnd;  // Linear increase
    } else if (current_state == TCPState::FAST_RECOVERY) {
        state_history.push_back("Fast Recovery");
        cwnd += 1;  // For each additional duplicate ACK
    }
}

void TCPTahoe::cubic_congestion_control() {
    auto current_time = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
    
    if (current_state == TCPState::SLOW_START) {
        state_history.push_back("CUBIC Slow Start");
        cwnd *= 2;
        if (cwnd >= ssthresh) {
            current_state = TCPState::CONGESTION_AVOIDANCE;
            in_slow_start = false;
        }
    } else {
        state_history.push_back("CUBIC Congestion Avoidance");
        
        // CUBIC window growth function
        double time_since_reduction = (current_time - last_cwnd_reduction_time) / 1000.0;
        double wmax = cwnd / cubic_beta;  // Window size before last reduction
        double target_cwnd = cubic_c * std::pow(time_since_reduction, 3) + wmax;
        
        if (target_cwnd > cwnd) {
            cwnd = std::min(target_cwnd, cwnd + 1.0);
        } else {
            cwnd += 1.0 / cwnd;  // TCP-friendly region
        }
    }
}

void TCPTahoe::bbr_congestion_control() {
    state_history.push_back("BBR");
    
    // Simplified BBR implementation
    // In practice, BBR has multiple phases: STARTUP, DRAIN, PROBE_BW, PROBE_RTT
    
    // Estimate bandwidth-delay product
    double bdp = bbr_max_bandwidth * bbr_min_rtt / 8.0;  // Convert to packets
    
    // Set cwnd based on bandwidth-delay product
    double target_cwnd = bdp * 2.0;  // Add some buffer
    
    if (cwnd < target_cwnd) {
        cwnd = std::min(target_cwnd, cwnd * 1.25);  // Gradual increase
    } else if (cwnd > target_cwnd * 1.25) {
        cwnd = std::max(target_cwnd, cwnd * 0.9);   // Gradual decrease
    }
    
    current_state = TCPState::CONGESTION_AVOIDANCE;
}

void TCPTahoe::timeout_event() {
    cwnd_history.push_back(cwnd);
    ssthresh_history.push_back(ssthresh);
    
    auto current_time = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
    last_cwnd_reduction_time = current_time;
    
    switch (algorithm) {
        case CongestionAlgorithm::TAHOE:
            state_history.push_back("Timeout");
            ssthresh = std::max(cwnd / 2, 1);
            cwnd = 1;
            current_state = TCPState::SLOW_START;
            in_slow_start = true;
            break;
            
        case CongestionAlgorithm::RENO:
            state_history.push_back("Timeout");
            ssthresh = std::max(cwnd / 2, 1);
            cwnd = 1;
            current_state = TCPState::SLOW_START;
            in_slow_start = true;
            break;
            
        case CongestionAlgorithm::CUBIC:
            state_history.push_back("CUBIC Timeout");
            ssthresh = std::max(static_cast<int>(cwnd * cubic_beta), 1);
            cwnd = 1;
            current_state = TCPState::SLOW_START;
            in_slow_start = true;
            break;
            
        case CongestionAlgorithm::BBR:
            state_history.push_back("BBR Timeout");
            // BBR doesn't reduce window on timeout as aggressively
            cwnd = std::max(cwnd * 0.8, 1.0);
            break;
    }
    
    duplicate_ack_count = 0;
}

void TCPTahoe::duplicate_ack() {
    duplicate_ack_count++;
    
    if (duplicate_ack_count >= 3) {  // Fast retransmit threshold
        cwnd_history.push_back(cwnd);
        ssthresh_history.push_back(ssthresh);
        
        auto current_time = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count();
        last_cwnd_reduction_time = current_time;
        
        switch (algorithm) {
            case CongestionAlgorithm::TAHOE:
                state_history.push_back("Fast Retransmit");
                ssthresh = std::max(cwnd / 2, 1);
                cwnd = 1;
                current_state = TCPState::SLOW_START;
                in_slow_start = true;
                break;
                
            case CongestionAlgorithm::RENO:
                state_history.push_back("Fast Retransmit");
                ssthresh = std::max(cwnd / 2, 1);
                cwnd = ssthresh + 3;  // Fast recovery
                current_state = TCPState::FAST_RECOVERY;
                in_slow_start = false;
                break;
                
            case CongestionAlgorithm::CUBIC:
                state_history.push_back("CUBIC Fast Retransmit");
                ssthresh = std::max(static_cast<int>(cwnd * cubic_beta), 1);
                cwnd = ssthresh;
                current_state = TCPState::CONGESTION_AVOIDANCE;
                in_slow_start = false;
                break;
                
            case CongestionAlgorithm::BBR:
                state_history.push_back("BBR Fast Retransmit");
                // BBR doesn't change window on duplicate ACKs
                break;
        }
        
        duplicate_ack_count = 0;
    }
}

void TCPTahoe::receive_ack(int ack_num) {
    if (current_state == TCPState::FAST_RECOVERY && algorithm == CongestionAlgorithm::RENO) {
        // Exit fast recovery
        current_state = TCPState::CONGESTION_AVOIDANCE;
        cwnd = ssthresh;
    }
    duplicate_ack_count = 0;
}

void TCPTahoe::set_network_conditions(double loss_rate, double utilization, int delay) {
    packet_loss_rate = loss_rate;
    network_utilization = utilization;
    queue_delay = delay;
}

void TCPTahoe::simulate_network_congestion() {
    // Simulate varying network conditions
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);
    
    // Simulate packet loss
    if (dis(gen) < packet_loss_rate) {
        timeout_event();
    }
    
    // Simulate congestion-based RTT increase
    if (network_utilization > 0.7) {
        rtt = static_cast<int>(rtt * (1.0 + network_utilization));
    }
}

void TCPTahoe::adaptive_congestion_response() {
    // Adapt algorithm parameters based on network conditions
    if (packet_loss_rate > 0.05) {  // High loss rate
        if (algorithm == CongestionAlgorithm::CUBIC) {
            cubic_beta = 0.8;  // More conservative
        }
    } else if (packet_loss_rate < 0.01) {  // Low loss rate
        if (algorithm == CongestionAlgorithm::CUBIC) {
            cubic_beta = 0.7;  // Standard
        }
    }
    
    // Adjust timeout based on RTT variation
    timeout = rtt * 2;
}

double TCPTahoe::calculate_throughput() const {
    if (rtt == 0) return 0.0;
    return (cwnd * 1500.0 * 8.0) / (rtt * 1000.0);  // Mbps (assuming 1500 byte packets)
}

// Getters
int TCPTahoe::get_current_cwnd() const { return cwnd; }
int TCPTahoe::get_current_ssthresh() const { return ssthresh; }
std::string TCPTahoe::get_current_state() const {
    switch (current_state) {
        case TCPState::SLOW_START: return "Slow Start";
        case TCPState::CONGESTION_AVOIDANCE: return "Congestion Avoidance";
        case TCPState::FAST_RECOVERY: return "Fast Recovery";
        case TCPState::TIMEOUT: return "Timeout";
        default: return "Unknown";
    }
}

CongestionAlgorithm TCPTahoe::get_algorithm() const { return algorithm; }
std::vector<int> TCPTahoe::get_cwnd_history() const { return cwnd_history; }
std::vector<int> TCPTahoe::get_ssthresh_history() const { return ssthresh_history; }
std::vector<std::string> TCPTahoe::get_state_history() const { return state_history; }
std::vector<double> TCPTahoe::get_throughput_history() const { return throughput_history; }
double TCPTahoe::get_current_throughput() const { return calculate_throughput(); }
double TCPTahoe::get_packet_loss_rate() const { return packet_loss_rate; }
double TCPTahoe::get_network_utilization() const { return network_utilization; }

// Setters
void TCPTahoe::set_algorithm(CongestionAlgorithm algo) { 
    algorithm = algo;
    reset();  // Reset state when changing algorithm
}

void TCPTahoe::reset() {
    cwnd = 1;
    ssthresh = 65535;
    duplicate_ack_count = 0;
    in_slow_start = true;
    current_state = TCPState::SLOW_START;
    cwnd_history.clear();
    ssthresh_history.clear();
    state_history.clear();
    throughput_history.clear();
    rtt_history.clear();
} 