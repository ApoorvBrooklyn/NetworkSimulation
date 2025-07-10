import numpy as np
import random
from typing import List, Tuple, Optional

# Import the pybind11 module
try:
    import network_protocols
except ImportError:
    raise ImportError("Network protocols module not found. Please run 'python setup.py build_ext --inplace' first.")

# Network simulation class that combines all protocols
class NetworkSimulator:
    def __init__(self):
        self.tcp_tahoe = network_protocols.TCPTahoe()
        self.stop_and_wait = network_protocols.StopAndWait()
        self.crc = network_protocols.CRC("1011")  # CRC-3 polynomial
        self.simulation_history = []
        
    def simulate_packet_transmission(self, data: str, network_conditions: dict) -> dict:
        """
        Simulate packet transmission with all protocols
        """
        # Add CRC to data
        data_with_crc = self.crc.add_crc_to_data(data)
        
        # Simulate TCP Tahoe behavior
        if network_conditions.get('congestion', False):
            self.tcp_tahoe.timeout_event()
        elif network_conditions.get('duplicate_ack', False):
            self.tcp_tahoe.duplicate_ack()
        else:
            self.tcp_tahoe.send_packet()
        
        # Simulate Stop and Wait
        packet_sent = self.stop_and_wait.send_packet(data_with_crc)
        
        # Simulate network errors and corruption
        error_rate = network_conditions.get('error_rate', 0) / 100.0
        packet_loss = network_conditions.get('packet_loss', False)
        timeout = network_conditions.get('timeout', False)
        
        # Simulate data corruption based on error rate
        corrupted_data = data_with_crc
        data_corrupted = False
        
        if error_rate > 0 and random.random() < error_rate:
            # Introduce bit errors in the data
            corrupted_data = self._introduce_bit_errors(data_with_crc, error_rate)
            data_corrupted = True
        
        # Simulate network delay and potential errors
        if timeout or packet_loss or data_corrupted:
            if timeout:
                self.stop_and_wait.check_timeout()
                self.stop_and_wait.retransmit()
            elif packet_loss:
                # Simulate packet loss - don't receive ACK
                pass
            else:
                # Data corruption - packet arrives but with errors
                self.stop_and_wait.receive_ack(self.stop_and_wait.get_total_packets_sent() - 1)
        else:
            # Simulate successful transmission
            self.stop_and_wait.receive_ack(self.stop_and_wait.get_total_packets_sent() - 1)
        
        # Verify CRC with potentially corrupted data
        crc_verified = self.crc.verify_crc(data, corrupted_data[-3:])  # Last 3 bits are CRC
        
        result = {
            'data': data,
            'data_with_crc': data_with_crc,
            'corrupted_data': corrupted_data if data_corrupted else None,
            'packet_sent': packet_sent,
            'crc_verified': crc_verified,
            'data_corrupted': data_corrupted,
            'packet_lost': packet_loss and random.random() < 0.7,  # 70% chance of actual loss
            'tcp_cwnd': self.tcp_tahoe.get_current_cwnd(),
            'tcp_ssthresh': self.tcp_tahoe.get_current_ssthresh(),
            'total_packets_sent': self.stop_and_wait.get_total_packets_sent(),
            'total_packets_acked': self.stop_and_wait.get_total_packets_acked(),
            'total_timeouts': self.stop_and_wait.get_total_timeouts(),
            'network_conditions': network_conditions
        }
        
        self.simulation_history.append(result)
        return result
    
    def _introduce_bit_errors(self, data: str, error_rate: float) -> str:
        """
        Introduce random bit errors in the data to simulate transmission errors
        """
        import random
        
        # Convert string to list of characters for manipulation
        data_list = list(data)
        
        # Calculate number of bits to flip based on error rate
        total_bits = len(data) * 8
        num_errors = max(1, int(total_bits * error_rate * 0.1))  # Scale down for realistic errors
        
        for _ in range(num_errors):
            if len(data_list) > 0:
                # Pick a random character position
                char_pos = random.randint(0, len(data_list) - 1)
                char = data_list[char_pos]
                
                # Pick a random bit position (0-7)
                bit_pos = random.randint(0, 7)
                
                # Flip the bit
                char_code = ord(char)
                char_code ^= (1 << bit_pos)
                data_list[char_pos] = chr(char_code)
        
        return ''.join(data_list)
    
    def get_simulation_stats(self) -> dict:
        """
        Get overall simulation statistics
        """
        if not self.simulation_history:
            return {}
        
        total_packets = len(self.simulation_history)
        successful_transmissions = sum(1 for r in self.simulation_history if r['crc_verified'])
        total_timeouts = self.stop_and_wait.get_total_timeouts()
        
        return {
            'total_packets': total_packets,
            'successful_transmissions': successful_transmissions,
            'success_rate': successful_transmissions / total_packets if total_packets > 0 else 0,
            'total_timeouts': total_timeouts,
            'current_tcp_cwnd': self.tcp_tahoe.get_current_cwnd(),
            'current_tcp_ssthresh': self.tcp_tahoe.get_current_ssthresh()
        }
    
    def reset_simulation(self):
        """
        Reset all protocols to initial state
        """
        self.tcp_tahoe = network_protocols.TCPTahoe()
        self.stop_and_wait = network_protocols.StopAndWait()
        self.simulation_history = []

# Convenience aliases for direct access to protocol classes
TCPTahoe = network_protocols.TCPTahoe
StopAndWait = network_protocols.StopAndWait
PrimsAlgorithm = network_protocols.PrimsAlgorithm
CRC = network_protocols.CRC
Edge = network_protocols.Edge 