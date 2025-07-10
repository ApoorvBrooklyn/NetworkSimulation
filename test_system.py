#!/usr/bin/env python3
"""
Test script to verify all network protocol components work correctly.
"""

import sys
import os

def test_imports():
    """Test if all required modules can be imported."""
    print("🔍 Testing imports...")
    
    try:
        import streamlit
        print("✅ Streamlit imported successfully")
    except ImportError as e:
        print(f"❌ Streamlit import failed: {e}")
        return False
    
    try:
        import pandas
        print("✅ Pandas imported successfully")
    except ImportError as e:
        print(f"❌ Pandas import failed: {e}")
        return False
    
    try:
        import plotly
        print("✅ Plotly imported successfully")
    except ImportError as e:
        print(f"❌ Plotly import failed: {e}")
        return False
    
    try:
        import numpy
        print("✅ NumPy imported successfully")
    except ImportError as e:
        print(f"❌ NumPy import failed: {e}")
        return False
    
    try:
        import networkx
        print("✅ NetworkX imported successfully")
    except ImportError as e:
        print(f"❌ NetworkX import failed: {e}")
        return False
    
    try:
        import matplotlib
        print("✅ Matplotlib imported successfully")
    except ImportError as e:
        print(f"❌ Matplotlib import failed: {e}")
        return False
    
    return True

def test_cpp_library():
    """Test if the C++ library can be loaded and used."""
    print("\n🔍 Testing C++ library...")
    
    try:
        from network_protocols import NetworkSimulator, TCPTahoeWrapper, StopAndWaitWrapper, PrimsAlgorithmWrapper, CRCWrapper
        print("✅ Network protocols imported successfully")
    except Exception as e:
        print(f"❌ Network protocols import failed: {e}")
        return False
    
    try:
        # Test TCP Tahoe
        tcp = TCPTahoeWrapper()
        tcp.send_packet()
        cwnd = tcp.get_current_cwnd()
        print(f"✅ TCP Tahoe test passed (CWND: {cwnd})")
    except Exception as e:
        print(f"❌ TCP Tahoe test failed: {e}")
        return False
    
    try:
        # Test Stop and Wait
        saw = StopAndWaitWrapper()
        result = saw.send_packet("test")
        print(f"✅ Stop and Wait test passed (Packet sent: {result})")
    except Exception as e:
        print(f"❌ Stop and Wait test failed: {e}")
        return False
    
    try:
        # Test CRC
        crc = CRCWrapper("1011")
        crc_value = crc.calculate_crc("test")
        print(f"✅ CRC test passed (CRC: {crc_value})")
    except Exception as e:
        print(f"❌ CRC test failed: {e}")
        return False
    
    try:
        # Test Prims Algorithm
        prims = PrimsAlgorithmWrapper(4)
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
        from network_protocols import NetworkSimulator
        
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

def test_streamlit_app():
    """Test if the Streamlit app can be imported."""
    print("\n🔍 Testing Streamlit app...")
    
    try:
        # Check if app.py exists
        if not os.path.exists('app.py'):
            print("❌ app.py not found")
            return False
        
        # Try to import the main function
        import importlib.util
        spec = importlib.util.spec_from_file_location("app", "app.py")
        app_module = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(app_module)
        
        print("✅ Streamlit app imported successfully")
        return True
    except Exception as e:
        print(f"❌ Streamlit app test failed: {e}")
        return False

def main():
    """Run all tests."""
    print("🚀 Starting Network Protocol Simulator Tests\n")
    
    tests = [
        ("Python Dependencies", test_imports),
        ("C++ Library", test_cpp_library),
        ("Network Simulator", test_network_simulator),
        ("Streamlit App", test_streamlit_app)
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
        print("🎉 All tests passed! The system is ready to run.")
        print("\n🚀 To start the application, run:")
        print("   streamlit run app.py")
    else:
        print("⚠️  Some tests failed. Please check the errors above.")
        return 1
    
    return 0

if __name__ == "__main__":
    sys.exit(main()) 