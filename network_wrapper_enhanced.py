import numpy as np
import random
from typing import List, Tuple, Optional, Dict, Any
import time

# Import the enhanced pybind11 module
try:
    import network_protocols_enhanced as np_enhanced
except ImportError:
    raise ImportError("Enhanced network protocols module not found. Please run 'python setup_enhanced.py build_ext --inplace' first.")

class EnhancedNetworkSimulator:
    """
    Enhanced Network Simulator that integrates all protocols with advanced features:
    - Multiple TCP congestion control algorithms (Tahoe, Reno, CUBIC, BBR)
    - Cross-layer optimization
    - LTE network simulation with handovers
    - Comprehensive validation framework
    - Detailed network logging and analytics
    """
    
    def __init__(self):
        # Initialize all protocol instances
        self.tcp_tahoe = np_enhanced.TCPTahoe(np_enhanced.CongestionAlgorithm.TAHOE)
        self.cross_layer_optimizer = np_enhanced.CrossLayerOptimizer()
        self.lte_network = np_enhanced.LTENetwork()
        self.validation_framework = np_enhanced.ValidationFramework()
        self.network_logger = np_enhanced.NetworkLogger()
        
        # Register protocols with cross-layer optimizer
        self.cross_layer_optimizer.register_layer(np_enhanced.LayerType.TRANSPORT, self.tcp_tahoe)
        self.cross_layer_optimizer.register_layer(np_enhanced.LayerType.PHYSICAL, self.lte_network)
        
        # Initialize simulation state
        self.simulation_history = []
        self.current_simulation_step = 0
        
        # Configure logging
        self.network_logger.set_log_level(np_enhanced.LogLevel.INFO)
        self.network_logger.enable_console_logging(True)
        
        # Initialize LTE network
        self.lte_network.initialize_network(num_cells=9, num_users=20)
        
    def set_tcp_algorithm(self, algorithm: str):
        """Set the TCP congestion control algorithm"""
        algorithm_map = {
            'tahoe': np_enhanced.CongestionAlgorithm.TAHOE,
            'reno': np_enhanced.CongestionAlgorithm.RENO,
            'cubic': np_enhanced.CongestionAlgorithm.CUBIC,
            'bbr': np_enhanced.CongestionAlgorithm.BBR
        }
        
        if algorithm.lower() in algorithm_map:
            self.tcp_tahoe.set_algorithm(algorithm_map[algorithm.lower()])
            self.network_logger.log_info(f"TCP algorithm changed to {algorithm.upper()}")
        else:
            raise ValueError(f"Unknown algorithm: {algorithm}")
    
    def simulate_enhanced_packet_transmission(self, data: str, network_conditions: dict) -> dict:
        """
        Enhanced packet transmission simulation with all protocols integrated
        """
        self.current_simulation_step += 1
        
        # Start packet trace
        packet_id = self.network_logger.start_packet_trace(
            "APPLICATION", "NETWORK", len(data), data
        )
        
        # Log packet transmission start
        self.network_logger.log_packet_event(
            np_enhanced.EventType.PACKET_SENT, "TCP", 
            self.current_simulation_step, len(data), data
        )
        
        # Set network conditions in TCP layer
        loss_rate = network_conditions.get('error_rate', 0) / 100.0
        utilization = network_conditions.get('network_utilization', 50) / 100.0
        delay = network_conditions.get('network_delay', 100)
        
        self.tcp_tahoe.set_network_conditions(loss_rate, utilization, delay)
        
        # Update cross-layer information
        physical_info = np_enhanced.LayerInfo()
        physical_info.layer = np_enhanced.LayerType.PHYSICAL
        physical_info.status = "active"
        physical_info.metrics = {
            "signal_strength": -75.0 + random.uniform(-10, 10),
            "interference": random.uniform(0.01, 0.1),
            "error_rate": loss_rate,
            "utilization": utilization,
            "delay": float(delay)
        }
        
        self.cross_layer_optimizer.update_layer_state(np_enhanced.LayerType.PHYSICAL, physical_info)
        
        # Simulate network conditions
        packet_success = True
        data_corrupted = False
        packet_lost = False
        handover_occurred = False
        
        # Simulate various network events
        if network_conditions.get('congestion', False):
            self.tcp_tahoe.timeout_event()
            self.network_logger.log_event(
                np_enhanced.EventType.TIMEOUT_OCCURRED, "TCP", 
                "Network congestion caused timeout"
            )
        elif network_conditions.get('duplicate_ack', False):
            self.tcp_tahoe.duplicate_ack()
            self.network_logger.log_event(
                np_enhanced.EventType.ACK_RECEIVED, "TCP", 
                "Duplicate ACK received"
            )
        else:
            self.tcp_tahoe.send_packet()
        
        # Simulate packet corruption
        if random.random() < loss_rate:
            data_corrupted = True
            packet_success = False
            self.network_logger.log_event(
                np_enhanced.EventType.PACKET_CORRUPTED, "PHYSICAL", 
                "Packet corrupted due to channel errors"
            )
        
        # Simulate packet loss
        if network_conditions.get('packet_loss', False) or random.random() < loss_rate:
            packet_lost = True
            packet_success = False
            self.network_logger.log_event(
                np_enhanced.EventType.PACKET_DROPPED, "NETWORK", 
                "Packet lost in network"
            )
        
        # Simulate LTE handover
        if random.random() < 0.1:  # 10% chance of handover
            self.lte_network.step_simulation()
            handover_events = self.lte_network.get_handover_history()
            if handover_events and handover_events[-1].success:
                handover_occurred = True
                self.network_logger.log_event(
                    np_enhanced.EventType.HANDOVER_COMPLETE, "LTE", 
                    f"Handover from cell {handover_events[-1].source_cell} to {handover_events[-1].target_cell}"
                )
        
        # Cross-layer optimization
        self.cross_layer_optimizer.optimize_network_performance()
        
        # Complete packet trace
        rtt = random.uniform(50, 200)
        self.network_logger.complete_packet_trace(packet_id, 
                                                 time.time_ns() // 1000, rtt)
        
        # Validate packet transmission
        validation_result = self.validation_framework.validate_packet_integrity(data, data)
        throughput_validation = self.validation_framework.validate_throughput_performance(
            self.tcp_tahoe.get_current_throughput()
        )
        
        # Compile results
        result = {
            'data': data,
            'packet_id': packet_id,
            'packet_success': packet_success,
            'data_corrupted': data_corrupted,
            'packet_lost': packet_lost,
            'handover_occurred': handover_occurred,
            'tcp_algorithm': self.tcp_tahoe.get_algorithm().name,
            'tcp_state': self.tcp_tahoe.get_current_state(),
            'tcp_cwnd': self.tcp_tahoe.get_current_cwnd(),
            'tcp_ssthresh': self.tcp_tahoe.get_current_ssthresh(),
            'current_throughput': self.tcp_tahoe.get_current_throughput(),
            'network_conditions': network_conditions,
            'cross_layer_throughput': self.cross_layer_optimizer.get_current_throughput(),
            'cross_layer_latency': self.cross_layer_optimizer.get_current_latency(),
            'lte_active_users': self.lte_network.get_active_users_count(),
            'lte_network_throughput': self.lte_network.get_network_throughput(),
            'validation_result': validation_result.name,
            'throughput_validation': throughput_validation.name,
            'rtt': rtt,
            'simulation_step': self.current_simulation_step
        }
        
        self.simulation_history.append(result)
        
        # Log successful transmission
        if packet_success:
            self.network_logger.log_event(
                np_enhanced.EventType.PACKET_RECEIVED, "APPLICATION", 
                "Packet successfully transmitted"
            )
        
        return result
    
    def run_validation_suite(self) -> dict:
        """Run comprehensive validation tests"""
        self.network_logger.log_info("Starting validation suite")
        
        # Set validation level
        self.validation_framework.set_validation_level(np_enhanced.ValidationLevel.COMPREHENSIVE)
        
        # Run all validation tests
        overall_result = self.validation_framework.run_all_tests()
        
        # Get validation statistics
        pass_rate = self.validation_framework.get_overall_pass_rate()
        validation_report = self.validation_framework.generate_validation_report()
        
        self.network_logger.log_info(f"Validation completed with {pass_rate:.2%} pass rate")
        
        return {
            'overall_result': overall_result.name,
            'pass_rate': pass_rate,
            'report': validation_report
        }
    
    def get_network_analytics(self) -> dict:
        """Get comprehensive network analytics"""
        # Get logging analytics
        avg_throughput = self.network_logger.calculate_average_throughput()
        avg_latency = self.network_logger.calculate_average_latency()
        packet_loss_rate = self.network_logger.calculate_packet_loss_rate()
        
        # Get cross-layer metrics
        cl_throughput_history = self.cross_layer_optimizer.get_throughput_history()
        cl_latency_history = self.cross_layer_optimizer.get_latency_history()
        
        # Get TCP metrics
        tcp_cwnd_history = self.tcp_tahoe.get_cwnd_history()
        tcp_throughput_history = self.tcp_tahoe.get_throughput_history()
        tcp_state_history = self.tcp_tahoe.get_state_history()
        
        # Get LTE metrics
        lte_handover_history = self.lte_network.get_handover_history()
        
        return {
            'network_performance': {
                'average_throughput': avg_throughput,
                'average_latency': avg_latency,
                'packet_loss_rate': packet_loss_rate
            },
            'tcp_metrics': {
                'current_algorithm': self.tcp_tahoe.get_algorithm().name,
                'current_state': self.tcp_tahoe.get_current_state(),
                'current_cwnd': self.tcp_tahoe.get_current_cwnd(),
                'current_ssthresh': self.tcp_tahoe.get_current_ssthresh(),
                'cwnd_history': tcp_cwnd_history,
                'throughput_history': tcp_throughput_history,
                'state_history': tcp_state_history
            },
            'cross_layer_metrics': {
                'current_throughput': self.cross_layer_optimizer.get_current_throughput(),
                'current_latency': self.cross_layer_optimizer.get_current_latency(),
                'current_energy': self.cross_layer_optimizer.get_current_energy_consumption(),
                'throughput_history': cl_throughput_history,
                'latency_history': cl_latency_history
            },
            'lte_metrics': {
                'active_users': self.lte_network.get_active_users_count(),
                'network_throughput': self.lte_network.get_network_throughput(),
                'handover_count': len(lte_handover_history),
                'handover_history': [
                    {
                        'source_cell': h.source_cell,
                        'target_cell': h.target_cell,
                        'success': h.success,
                        'trigger_rsrp': h.trigger_rsrp,
                        'target_rsrp': h.target_rsrp
                    } for h in lte_handover_history
                ]
            }
        }
    
    def export_logs(self, filename: str = "network_simulation.csv"):
        """Export network logs to CSV file"""
        self.network_logger.export_logs_to_csv(filename)
        self.network_logger.log_info(f"Logs exported to {filename}")
    
    def generate_summary_report(self) -> str:
        """Generate comprehensive simulation summary report"""
        analytics = self.get_network_analytics()
        validation_stats = self.run_validation_suite()
        log_summary = self.network_logger.generate_summary_report()
        
        report = f"""
ENHANCED NETWORK PROTOCOL SIMULATION REPORT
==========================================

Simulation Overview:
- Total Simulation Steps: {self.current_simulation_step}
- TCP Algorithm: {analytics['tcp_metrics']['current_algorithm']}
- Current TCP State: {analytics['tcp_metrics']['current_state']}

Network Performance:
- Average Throughput: {analytics['network_performance']['average_throughput']:.2f} Mbps
- Average Latency: {analytics['network_performance']['average_latency']:.2f} ms
- Packet Loss Rate: {analytics['network_performance']['packet_loss_rate']:.2%}

TCP Metrics:
- Current CWND: {analytics['tcp_metrics']['current_cwnd']}
- Current SSTHRESH: {analytics['tcp_metrics']['current_ssthresh']}

Cross-Layer Optimization:
- Optimized Throughput: {analytics['cross_layer_metrics']['current_throughput']:.2f} Mbps
- Optimized Latency: {analytics['cross_layer_metrics']['current_latency']:.2f} ms
- Energy Consumption: {analytics['cross_layer_metrics']['current_energy']:.2f} mW

LTE Network:
- Active Users: {analytics['lte_metrics']['active_users']}
- Network Throughput: {analytics['lte_metrics']['network_throughput']:.2f} Mbps
- Handovers Performed: {analytics['lte_metrics']['handover_count']}

Validation Results:
- Overall Result: {validation_stats['overall_result']}
- Pass Rate: {validation_stats['pass_rate']:.2%}

{log_summary}
        """
        
        return report
    
    def reset_simulation(self):
        """Reset all simulation components"""
        self.tcp_tahoe.reset()
        self.cross_layer_optimizer.reset()
        self.lte_network.initialize_network(num_cells=9, num_users=20)
        self.validation_framework.reset_validation_framework()
        self.network_logger.clear_logs()
        self.simulation_history.clear()
        self.current_simulation_step = 0
        
        self.network_logger.log_info("Simulation reset completed")
    
    def get_simulation_stats(self) -> dict:
        """Get overall simulation statistics"""
        if not self.simulation_history:
            return {}
        
        total_packets = len(self.simulation_history)
        successful_packets = sum(1 for r in self.simulation_history if r['packet_success'])
        corrupted_packets = sum(1 for r in self.simulation_history if r['data_corrupted'])
        lost_packets = sum(1 for r in self.simulation_history if r['packet_lost'])
        handovers = sum(1 for r in self.simulation_history if r['handover_occurred'])
        
        return {
            'total_packets': total_packets,
            'successful_packets': successful_packets,
            'success_rate': successful_packets / total_packets if total_packets > 0 else 0,
            'corruption_rate': corrupted_packets / total_packets if total_packets > 0 else 0,
            'loss_rate': lost_packets / total_packets if total_packets > 0 else 0,
            'handover_count': handovers,
            'avg_throughput': np.mean([r['current_throughput'] for r in self.simulation_history]),
            'avg_rtt': np.mean([r['rtt'] for r in self.simulation_history]),
            'current_tcp_algorithm': self.tcp_tahoe.get_algorithm().name,
            'current_tcp_cwnd': self.tcp_tahoe.get_current_cwnd(),
            'current_tcp_ssthresh': self.tcp_tahoe.get_current_ssthresh()
        }

# Convenience aliases for backward compatibility and direct access
TCPTahoe = np_enhanced.TCPTahoe
CrossLayerOptimizer = np_enhanced.CrossLayerOptimizer
LTENetwork = np_enhanced.LTENetwork
ValidationFramework = np_enhanced.ValidationFramework
NetworkLogger = np_enhanced.NetworkLogger

# Enums for easy access
CongestionAlgorithm = np_enhanced.CongestionAlgorithm
LayerType = np_enhanced.LayerType
CrossLayerEvent = np_enhanced.CrossLayerEvent
LTEState = np_enhanced.LTEState
ValidationLevel = np_enhanced.ValidationLevel
ValidationResult = np_enhanced.ValidationResult
LogLevel = np_enhanced.LogLevel
EventType = np_enhanced.EventType 