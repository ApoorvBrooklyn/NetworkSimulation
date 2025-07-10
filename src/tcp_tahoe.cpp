#include <iostream>
#include <vector>
#include <queue>
#include <chrono>
#include <random>
#include <cmath>

class TCPTahoe {
private:
    int cwnd;           // Congestion window size
    int ssthresh;       // Slow start threshold
    int rtt;            // Round trip time
    int timeout;        // Timeout value
    bool in_slow_start; // Whether in slow start phase
    std::vector<int> cwnd_history;
    std::vector<int> ssthresh_history;
    std::vector<std::string> state_history;

public:
    TCPTahoe() {
        cwnd = 1;
        ssthresh = 65535;
        rtt = 100;
        timeout = 200;
        in_slow_start = true;
    }

    void send_packet() {
        cwnd_history.push_back(cwnd);
        ssthresh_history.push_back(ssthresh);
        
        if (in_slow_start) {
            state_history.push_back("Slow Start");
            cwnd *= 2;
            if (cwnd >= ssthresh) {
                in_slow_start = false;
                state_history.back() = "Congestion Avoidance";
            }
        } else {
            state_history.push_back("Congestion Avoidance");
            cwnd += 1;
        }
    }

    void timeout_event() {
        cwnd_history.push_back(cwnd);
        ssthresh_history.push_back(ssthresh);
        state_history.push_back("Timeout");
        
        ssthresh = cwnd / 2;
        cwnd = 1;
        in_slow_start = true;
    }

    void duplicate_ack() {
        cwnd_history.push_back(cwnd);
        ssthresh_history.push_back(ssthresh);
        state_history.push_back("Fast Retransmit");
        
        ssthresh = cwnd / 2;
        cwnd = 1;  // TCP Tahoe behavior: cwnd drops to 1 on fast retransmit
        in_slow_start = true;
    }

    std::vector<int> get_cwnd_history() const { return cwnd_history; }
    std::vector<int> get_ssthresh_history() const { return ssthresh_history; }
    std::vector<std::string> get_state_history() const { return state_history; }
    
    int get_current_cwnd() const { return cwnd; }
    int get_current_ssthresh() const { return ssthresh; }
    std::string get_current_state() const { 
        return in_slow_start ? "Slow Start" : "Congestion Avoidance"; 
    }
    
    void reset() {
        cwnd = 1;
        ssthresh = 65535;
        rtt = 100;
        timeout = 200;
        in_slow_start = true;
        cwnd_history.clear();
        ssthresh_history.clear();
        state_history.clear();
    }
}; 