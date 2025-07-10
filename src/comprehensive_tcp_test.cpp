#include "tcp_tahoe.cpp"
#include <iostream>
#include <iomanip>

int main() {
    std::cout << "=== TCP Tahoe Comprehensive Test ===" << std::endl;
    std::cout << std::left << std::setw(8) << "Step" 
              << std::setw(12) << "Action" 
              << std::setw(8) << "CWND" 
              << std::setw(12) << "SSTHRESH" 
              << std::setw(15) << "State" << std::endl;
    std::cout << std::string(55, '-') << std::endl;
    
    TCPTahoe tcp;
    
    // Initial state
    std::cout << std::left << std::setw(8) << "0" 
              << std::setw(12) << "Initial" 
              << std::setw(8) << tcp.get_current_cwnd() 
              << std::setw(12) << tcp.get_current_ssthresh() 
              << std::setw(15) << tcp.get_current_state() << std::endl;
    
    // Send packets in slow start phase
    std::cout << "\n--- Slow Start Phase ---" << std::endl;
    for (int i = 1; i <= 10; i++) {
        tcp.send_packet();
        std::cout << std::left << std::setw(8) << i 
                  << std::setw(12) << "Send Packet" 
                  << std::setw(8) << tcp.get_current_cwnd() 
                  << std::setw(12) << tcp.get_current_ssthresh() 
                  << std::setw(15) << tcp.get_current_state() << std::endl;
    }
    
    // Simulate timeout (packet loss)
    std::cout << "\n--- Timeout Event (Packet Loss) ---" << std::endl;
    tcp.timeout_event();
    std::cout << std::left << std::setw(8) << "T1" 
              << std::setw(12) << "Timeout" 
              << std::setw(8) << tcp.get_current_cwnd() 
              << std::setw(12) << tcp.get_current_ssthresh() 
              << std::setw(15) << tcp.get_current_state() << std::endl;
    
    // Continue sending packets after timeout
    std::cout << "\n--- Recovery Phase ---" << std::endl;
    for (int i = 1; i <= 15; i++) {
        tcp.send_packet();
        std::cout << std::left << std::setw(8) << i 
                  << std::setw(12) << "Send Packet" 
                  << std::setw(8) << tcp.get_current_cwnd() 
                  << std::setw(12) << tcp.get_current_ssthresh() 
                  << std::setw(15) << tcp.get_current_state() << std::endl;
    }
    
    // Another timeout to show sawtooth pattern
    std::cout << "\n--- Second Timeout Event ---" << std::endl;
    tcp.timeout_event();
    std::cout << std::left << std::setw(8) << "T2" 
              << std::setw(12) << "Timeout" 
              << std::setw(8) << tcp.get_current_cwnd() 
              << std::setw(12) << tcp.get_current_ssthresh() 
              << std::setw(15) << tcp.get_current_state() << std::endl;
    
    // Continue again
    std::cout << "\n--- Second Recovery Phase ---" << std::endl;
    for (int i = 1; i <= 10; i++) {
        tcp.send_packet();
        std::cout << std::left << std::setw(8) << i 
                  << std::setw(12) << "Send Packet" 
                  << std::setw(8) << tcp.get_current_cwnd() 
                  << std::setw(12) << tcp.get_current_ssthresh() 
                  << std::setw(15) << tcp.get_current_state() << std::endl;
    }
    
    // Test duplicate ACK (fast retransmit)
    std::cout << "\n--- Duplicate ACK Test ---" << std::endl;
    tcp.duplicate_ack();
    std::cout << std::left << std::setw(8) << "D1" 
              << std::setw(12) << "Dup ACK" 
              << std::setw(8) << tcp.get_current_cwnd() 
              << std::setw(12) << tcp.get_current_ssthresh() 
              << std::setw(15) << tcp.get_current_state() << std::endl;
    
    // Show final history
    std::cout << "\n=== CWND History ===" << std::endl;
    auto cwnd_history = tcp.get_cwnd_history();
    for (size_t i = 0; i < cwnd_history.size(); i++) {
        std::cout << "Step " << i << ": " << cwnd_history[i] << std::endl;
    }
    
    return 0;
} 