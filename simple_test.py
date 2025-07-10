#!/usr/bin/env python3
"""
Simple test to isolate the segmentation fault issue.
"""

import ctypes
import os

# Load the library
lib_path = os.path.join(os.path.dirname(__file__), 'libnetwork_protocols.so')
lib = ctypes.CDLL(lib_path)

print("Library loaded successfully")

# Test TCP Tahoe
print("Testing TCP Tahoe...")
tcp_obj = lib.create_tcp_tahoe()
print("TCP object created")
lib.tcp_send_packet(tcp_obj)
print("TCP packet sent")
cwnd = lib.get_current_cwnd(tcp_obj)
print(f"CWND: {cwnd}")
lib.destroy_tcp_tahoe(tcp_obj)
print("TCP object destroyed")

print("All tests passed!") 