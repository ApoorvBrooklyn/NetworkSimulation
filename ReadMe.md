# 🌐 Enhanced Network Protocol Simulator

A comprehensive network protocol simulator featuring advanced TCP algorithms, cross-layer optimization, LTE network simulation, validation framework, and detailed network logging.

## ✨ Features

### 🧠 Multiple TCP Congestion Control Algorithms
- **TCP Tahoe**: Classic slow-start and congestion avoidance
- **TCP Reno**: Fast recovery with duplicate ACK handling
- **TCP CUBIC**: High-speed networks with cubic window growth
- **TCP BBR**: Bandwidth-delay product optimization

### 🔄 Cross-Layer Protocol Optimization
- Intelligent coordination between network layers
- Adaptive algorithm selection based on network conditions
- Performance optimization for throughput, latency, and energy

### 📱 LTE Network Simulation
- Complete LTE network with multiple cells and users
- Handover management and mobility simulation
- Resource allocation and scheduling

### ✅ Comprehensive Validation Framework
- Protocol conformance testing
- Performance validation
- Stress testing with detailed reports

### 📊 Advanced Network Analytics & Logging
- Real-time performance monitoring
- Detailed packet tracing with timestamps
- CSV export for data analysis
- Performance dashboard with visualizations

## 🚀 Quick Start

### 1. Build the Backend
```bash
# Build the basic C++ library
make clean && make
```

### 2. Install Dependencies
```bash
# Install Python dependencies
pip install -r requirements.txt
```

### 3. Run the Enhanced Application
```bash
# Run the enhanced Streamlit app
streamlit run app_enhanced.py
```

### 4. Access the Application
Open your browser to: `http://localhost:8501`

## 📖 Usage Guide

### 🏠 Overview Page
- Introduction to all enhanced features
- System architecture overview
- Quick demonstration of capabilities

### 🚀 Enhanced Network Simulation
- **TCP Algorithm Selection**: Choose from Tahoe, Reno, CUBIC, or BBR
- **Network Conditions**: Configure congestion, packet loss, delay, and error rates
- **Advanced Options**: Enable cross-layer optimization and LTE handovers
- **Real-time Metrics**: Monitor TCP metrics, cross-layer performance, and LTE statistics

### 🧠 TCP Algorithm Comparison
- Compare multiple TCP algorithms side-by-side
- Analyze performance under different network conditions
- Visualize congestion window evolution and throughput

### 🔄 Cross-Layer Optimization
- Configure optimization weights for throughput, latency, and energy
- Simulate network conditions with mobility and interference
- See adaptive protocol selection in action

### 📱 LTE Network Simulation
- Configure LTE networks with multiple cells and users
- Set handover parameters (margin, hysteresis, delay)
- Visualize network topology and handover events

### ✅ Validation & Testing
- Run comprehensive test suites
- Protocol conformance testing
- Performance validation with detailed reports

### 📊 Network Analytics & Logging
- View real-time performance trends
- Export detailed simulation logs
- Generate comprehensive reports

### 📈 Performance Dashboard
- Key performance indicators (KPIs)
- Real-time network performance trends
- Algorithm performance comparison
- System status monitoring

## 🛠️ Technical Architecture

### Backend (C++)
- High-performance protocol implementations
- Advanced TCP congestion control algorithms
- Cross-layer optimization engine
- LTE network simulation core

### Frontend (Python/Streamlit)
- Interactive web interface
- Real-time visualizations with Plotly
- Comprehensive analytics dashboard
- Export capabilities

### Integration
- Enhanced Python wrapper with simulation capabilities
- Fallback support for basic features
- Modular design for easy extension

## 📊 Example Use Cases

### 1. TCP Algorithm Analysis
```python
# Compare TCP algorithms under high congestion
simulator.set_tcp_algorithm('cubic')
conditions = {'congestion': True, 'error_rate': 10}
result = simulator.simulate_enhanced_transmission(data, conditions)
```

### 2. LTE Network Planning
```python
# Simulate LTE network with 12 cells and 30 users
simulator.lte_network.initialize_network(12, 30)
simulator.lte_network.set_handover_parameters(3.0, 1.0, 320)
```

### 3. Performance Validation
```python
# Run comprehensive validation suite
validation = simulator.run_validation()
print(f"Tests passed: {validation['pass_rate']:.1%}")
```

## 🔧 Configuration Options

### TCP Algorithms
- `tahoe`: Conservative, reliable for lossy networks
- `reno`: Balanced performance with fast recovery
- `cubic`: Optimized for high-speed networks
- `bbr`: Best for variable bandwidth conditions

### Network Conditions
- **Congestion**: Simulate network congestion
- **Packet Loss**: Configure packet loss rates
- **Delay**: Set network propagation delay
- **Error Rate**: Configure bit error rates

### LTE Parameters
- **Cells**: Number of base stations
- **Users**: Number of mobile users
- **Handover Margin**: Signal strength threshold
- **Hysteresis**: Handover stability margin

## 📈 Performance Metrics

### TCP Metrics
- **CWND**: Congestion window size
- **SSTHRESH**: Slow start threshold
- **Throughput**: Data transmission rate
- **RTT**: Round-trip time

### Cross-Layer Metrics
- **Optimized Throughput**: Enhanced data rate
- **Latency**: End-to-end delay
- **Energy**: Power consumption

### LTE Metrics
- **Active Users**: Currently connected users
- **Network Throughput**: Total system capacity
- **Handovers**: Mobility management events

## 🎯 Educational Benefits

### For Students
- Understand TCP congestion control mechanisms
- Learn about cross-layer protocol optimization
- Explore LTE network architecture and operations
- Analyze network performance under various conditions

### For Researchers
- Test new protocol algorithms
- Validate theoretical models
- Compare protocol performance
- Generate data for research papers

### For Network Engineers
- Simulate network deployments
- Test network configurations
- Validate network designs
- Troubleshoot performance issues

## 🔍 Troubleshooting

### Common Issues

1. **"Enhanced mode not available"**
   - Ensure `make` completed successfully
   - Check that `libnetwork_protocols.so` exists
   - Verify Python dependencies are installed

2. **Import errors**
   - Activate virtual environment: `source myenv/bin/activate`
   - Install dependencies: `pip install -r requirements.txt`

3. **Performance issues**
   - Reduce number of simulation steps
   - Disable detailed logging for large simulations
   - Use basic mode for simple tests

## 📚 References

- TCP Congestion Control: RFC 5681
- TCP CUBIC: RFC 8312
- TCP BBR: IETF Draft
- LTE Standards: 3GPP TS 36.300

## 🤝 Contributing

1. Fork the repository
2. Create feature branch: `git checkout -b feature/new-feature`
3. Commit changes: `git commit -m 'Add new feature'`
4. Push to branch: `git push origin feature/new-feature`
5. Submit pull request

## 📄 License

This project is licensed under the MIT License - see the LICENSE file for details.

---

**🎉 Happy Network Simulation!** 🌐 