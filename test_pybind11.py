#!/usr/bin/env python3
"""
Test script for the pybind11 network protocols module.
"""

import sys

def test_pybind11_module():
    """Test if the pybind11 module works correctly."""
    print("🔍 Testing pybind11 module...")
    
    try:
        import network_protocols
        print("✅ Network protocols module imported successfully")
    except ImportError as e:
        print(f"❌ Network protocols import failed: {e}")
        return False
    
    try:
        # Test TCP Tahoe
        tcp = network_protocols.TCPTahoe()
        tcp.send_packet()
        cwnd = tcp.get_current_cwnd()
        print(f"✅ TCP Tahoe test passed (CWND: {cwnd})")
    except Exception as e:
        print(f"❌ TCP Tahoe test failed: {e}")
        return False
    
    try:
        # Test Stop and Wait
        saw = network_protocols.StopAndWait()
        result = saw.send_packet("test")
        print(f"✅ Stop and Wait test passed (Packet sent: {result})")
    except Exception as e:
        print(f"❌ Stop and Wait test failed: {e}")
        return False
    
    try:
        # Test CRC
        crc = network_protocols.CRC("1011")
        crc_value = crc.calculate_crc("test")
        print(f"✅ CRC test passed (CRC: {crc_value})")
    except Exception as e:
        print(f"❌ CRC test failed: {e}")
        return False
    
    try:
        # Test Prims Algorithm
        prims = network_protocols.PrimsAlgorithm(4)
        prims.add_edge(0, 1, 5)
        prims.add_edge(1, 2, 3)
        prims.add_edge(2, 3, 4)
        prims.find_mst()
        weight = prims.get_total_weight()
        print(f"✅ Prims Algorithm test passed (MST weight: {weight})")
    except Exception as e:
        print(f"❌ Prims Algorithm test failed: {e}")
        return False
    
    return True

def test_network_simulator():
    """Test the integrated network simulator."""
    print("\n🔍 Testing network simulator...")
    
    try:
        # Import from the Python wrapper file
        from network_wrapper import NetworkSimulator
        
        simulator = NetworkSimulator()
        
        # Test basic simulation
        result = simulator.simulate_packet_transmission(
            "Hello World", 
            {'congestion': False, 'timeout': False}
        )
        
        print(f"✅ Network simulator test passed")
        print(f"   - Data: {result['data']}")
        print(f"   - CRC Verified: {result['crc_verified']}")
        print(f"   - TCP CWND: {result['tcp_cwnd']}")
        print(f"   - Packets Sent: {result['total_packets_sent']}")
        
        # Test statistics
        stats = simulator.get_simulation_stats()
        print(f"   - Success Rate: {stats['success_rate']:.2%}")
        
        return True
    except Exception as e:
        print(f"❌ Network simulator test failed: {e}")
        return False

def main():
    """Run all tests."""
    print("🚀 Starting pybind11 Network Protocol Tests\n")
    
    tests = [
        ("pybind11 Module", test_pybind11_module),
        ("Network Simulator", test_network_simulator)
    ]
    
    passed = 0
    total = len(tests)
    
    for test_name, test_func in tests:
        print(f"📋 Running {test_name} test...")
        if test_func():
            passed += 1
            print(f"✅ {test_name} test PASSED\n")
        else:
            print(f"❌ {test_name} test FAILED\n")
    
    print("=" * 50)
    print(f"📊 Test Results: {passed}/{total} tests passed")
    
    if passed == total:
        print("🎉 All tests passed! The pybind11 system is working correctly.")
        print("\n🚀 To start the Streamlit application, run:")
        print("   streamlit run app.py")
    else:
        print("⚠️  Some tests failed. Please check the errors above.")
        return 1
    
    return 0

if __name__ == "__main__":
    sys.exit(main()) 