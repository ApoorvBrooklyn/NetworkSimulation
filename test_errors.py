#!/usr/bin/env python3
"""
Test script to demonstrate error simulation in the network protocols.
"""

from network_wrapper import NetworkSimulator
import random

def test_error_simulation():
    """Test error simulation with different error rates."""
    print("🔍 Testing Error Simulation...")
    
    simulator = NetworkSimulator()
    
    # Test with different error rates
    error_rates = [0, 5, 10, 15, 20]
    
    for error_rate in error_rates:
        print(f"\n📊 Testing with {error_rate}% error rate:")
        
        # Send multiple packets to get statistics
        for i in range(10):
            result = simulator.simulate_packet_transmission(
                f"Test Packet {i}", 
                {'error_rate': error_rate, 'packet_loss': False, 'timeout': False}
            )
            
            status = "✅" if result['crc_verified'] else "❌"
            corruption = "⚠️" if result.get('data_corrupted') else "✅"
            
            print(f"  Packet {i}: {status} CRC={result['crc_verified']}, Corruption={corruption}")
        
        # Get statistics
        stats = simulator.get_simulation_stats()
        print(f"  Success Rate: {stats['success_rate']:.1%}")
        
        # Reset for next test
        simulator.reset_simulation()

def test_packet_loss():
    """Test packet loss simulation."""
    print("\n🔍 Testing Packet Loss Simulation...")
    
    simulator = NetworkSimulator()
    
    # Test with packet loss enabled
    for i in range(10):
        result = simulator.simulate_packet_transmission(
            f"Test Packet {i}", 
            {'error_rate': 0, 'packet_loss': True, 'timeout': False}
        )
        
        status = "✅" if result['crc_verified'] else "❌"
        lost = "⚠️" if result.get('packet_lost') else "✅"
        
        print(f"  Packet {i}: {status} CRC={result['crc_verified']}, Lost={lost}")
    
    stats = simulator.get_simulation_stats()
    print(f"  Success Rate: {stats['success_rate']:.1%}")

def test_data_corruption():
    """Test data corruption simulation."""
    print("\n🔍 Testing Data Corruption Simulation...")
    
    simulator = NetworkSimulator()
    
    # Test with high error rate to see corruption
    for i in range(5):
        result = simulator.simulate_packet_transmission(
            "Hello World", 
            {'error_rate': 25, 'packet_loss': False, 'timeout': False}
        )
        
        print(f"  Packet {i}:")
        print(f"    Original: {result['data']}")
        print(f"    With CRC: {result['data_with_crc']}")
        if result.get('corrupted_data'):
            print(f"    Corrupted: {result['corrupted_data']}")
        print(f"    CRC Verified: {'✅' if result['crc_verified'] else '❌'}")
        print()

if __name__ == "__main__":
    print("🚀 Network Error Simulation Test\n")
    
    test_error_simulation()
    test_packet_loss()
    test_data_corruption()
    
    print("\n🎉 Error simulation tests completed!")
    print("Now you can see realistic success rates in the Streamlit app!") 