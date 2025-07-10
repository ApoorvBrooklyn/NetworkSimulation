#include <iostream>
#include <vector>
#include <queue>
#include <chrono>
#include <random>
#include <string>

class StopAndWait {
private:
    int sequence_number;
    int ack_number;
    bool waiting_for_ack;
    std::vector<std::string> packet_history;
    std::vector<bool> ack_history;
    std::vector<int> rtt_history;
    std::chrono::steady_clock::time_point last_send_time;
    int timeout_ms;
    int total_packets_sent;
    int total_packets_acked;
    int total_timeouts;

public:
    StopAndWait() {
        sequence_number = 0;
        ack_number = 0;
        waiting_for_ack = false;
        timeout_ms = 1000;
        total_packets_sent = 0;
        total_packets_acked = 0;
        total_timeouts = 0;
    }

    bool send_packet(const std::string& data) {
        if (waiting_for_ack) {
            return false; // Still waiting for ACK
        }

        packet_history.push_back(data);
        ack_history.push_back(false);
        waiting_for_ack = true;
        last_send_time = std::chrono::steady_clock::now();
        total_packets_sent++;
        
        return true;
    }

    bool receive_ack(int received_ack) {
        if (!waiting_for_ack) {
            return false;
        }

        if (received_ack == sequence_number) {
            auto now = std::chrono::steady_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_send_time);
            rtt_history.push_back(duration.count());
            
            ack_history.back() = true;
            waiting_for_ack = false;
            sequence_number = (sequence_number + 1) % 2;
            total_packets_acked++;
            return true;
        }
        return false;
    }

    bool check_timeout() {
        if (!waiting_for_ack) {
            return false;
        }

        auto now = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_send_time);
        
        if (duration.count() > timeout_ms) {
            total_timeouts++;
            waiting_for_ack = false;
            return true;
        }
        return false;
    }

    void retransmit() {
        if (!waiting_for_ack) {
            waiting_for_ack = true;
            last_send_time = std::chrono::steady_clock::now();
        }
    }

    std::vector<std::string> get_packet_history() const { return packet_history; }
    std::vector<bool> get_ack_history() const { return ack_history; }
    std::vector<int> get_rtt_history() const { return rtt_history; }
    
    int get_total_packets_sent() const { return total_packets_sent; }
    int get_total_packets_acked() const { return total_packets_acked; }
    int get_total_timeouts() const { return total_timeouts; }
    bool is_waiting_for_ack() const { return waiting_for_ack; }
    int get_current_sequence() const { return sequence_number; }
}; 