import streamlit as st
import pandas as pd
import plotly.graph_objects as go
import plotly.express as px
import numpy as np
import time
import random
from network_wrapper import NetworkSimulator, PrimsAlgorithm, CRC
import networkx as nx
import matplotlib.pyplot as plt

# Page configuration
st.set_page_config(
    page_title="Network Protocol Simulator",
    page_icon="🌐",
    layout="wide",
    initial_sidebar_state="expanded"
)

# Custom CSS for better styling
st.markdown("""
<style>
    .main-header {
        font-size: 3rem;
        color: #1f77b4;
        text-align: center;
        margin-bottom: 2rem;
    }
    .protocol-card {
        background-color: #f0f2f6;
        padding: 1rem;
        border-radius: 0.5rem;
        margin: 1rem 0;
    }
    .metric-card {
        background-color: #e8f4fd;
        padding: 1rem;
        border-radius: 0.5rem;
        margin: 0.5rem 0;
    }
</style>
""", unsafe_allow_html=True)

# Initialize session state
if 'simulator' not in st.session_state:
    st.session_state.simulator = NetworkSimulator()
if 'simulation_running' not in st.session_state:
    st.session_state.simulation_running = False
if 'prims_graph' not in st.session_state:
    st.session_state.prims_graph = None

def main():
    st.markdown('<h1 class="main-header">🌐 Network Protocol Simulator</h1>', unsafe_allow_html=True)
    st.markdown("### TCP Tahoe + Stop-and-Wait + Prim's Algorithm + CRC Simulation")
    
    # Sidebar for navigation
    st.sidebar.title("Navigation")
    page = st.sidebar.selectbox(
        "Choose a simulation:",
        ["Network Simulation", "TCP Tahoe Analysis", "Stop-and-Wait Protocol", "Prim's Algorithm", "CRC Error Detection", "About"]
    )
    
    if page == "Network Simulation":
        network_simulation_page()
    elif page == "TCP Tahoe Analysis":
        tcp_tahoe_page()
    elif page == "Stop-and-Wait Protocol":
        stop_and_wait_page()
    elif page == "Prim's Algorithm":
        prims_algorithm_page()
    elif page == "CRC Error Detection":
        crc_page()
    elif page == "About":
        about_page()

def network_simulation_page():
    st.header("🌐 Integrated Network Simulation")
    st.markdown("Simulate packet transmission using all protocols together")
    
    col1, col2 = st.columns([2, 1])
    
    with col1:
        st.subheader("Simulation Parameters")
        
        # Packet data input
        packet_data = st.text_area("Packet Data:", value="Hello, Network World!", height=100)
        
        # Network conditions
        st.subheader("Network Conditions")
        col_a, col_b, col_c = st.columns(3)
        
        with col_a:
            congestion = st.checkbox("Network Congestion")
            timeout = st.checkbox("Packet Timeout")
        
        with col_b:
            duplicate_ack = st.checkbox("Duplicate ACK")
            packet_loss = st.checkbox("Packet Loss")
        
        with col_c:
            network_delay = st.slider("Network Delay (ms)", 0, 1000, 100)
            error_rate = st.slider("Error Rate (%)", 0, 20, 5)
        
        # Simulation controls
        st.subheader("Simulation Controls")
        col_x, col_y, col_z = st.columns(3)
        
        with col_x:
            if st.button("🚀 Send Single Packet", use_container_width=True):
                network_conditions = {
                    'congestion': congestion,
                    'timeout': timeout,
                    'duplicate_ack': duplicate_ack,
                    'packet_loss': packet_loss,
                    'delay': network_delay,
                    'error_rate': error_rate
                }
                
                result = st.session_state.simulator.simulate_packet_transmission(packet_data, network_conditions)
                
                # Show detailed result
                if result['crc_verified']:
                    st.success(f"✅ Packet sent successfully! CRC Verified: {result['crc_verified']}")
                else:
                    st.error(f"❌ Packet transmission failed! CRC Verified: {result['crc_verified']}")
                
                # Show error details if any
                if result.get('data_corrupted'):
                    st.warning(f"⚠️ Data was corrupted during transmission")
                if result.get('packet_lost'):
                    st.warning(f"⚠️ Packet was lost during transmission")
        
        with col_y:
            if st.button("🔄 Reset Simulation", use_container_width=True):
                st.session_state.simulator.reset_simulation()
                st.success("Simulation reset!")
        
        with col_z:
            if st.button("📊 Show Statistics", use_container_width=True):
                stats = st.session_state.simulator.get_simulation_stats()
                if stats:
                    st.json(stats)
    
    with col2:
        st.subheader("Real-time Metrics")
        
        # Display current metrics
        if st.session_state.simulator.simulation_history:
            latest = st.session_state.simulator.simulation_history[-1]
            
            st.metric("TCP CWND", latest['tcp_cwnd'])
            st.metric("TCP SSTHRESH", latest['tcp_ssthresh'])
            st.metric("Packets Sent", latest['total_packets_sent'])
            st.metric("Packets ACKed", latest['total_packets_acked'])
            st.metric("Timeouts", latest['total_timeouts'])
            
            # Success rate
            stats = st.session_state.simulator.get_simulation_stats()
            if stats:
                st.metric("Success Rate", f"{stats['success_rate']:.2%}")
                
                # Show error breakdown
                if st.session_state.simulator.simulation_history:
                    total_packets = len(st.session_state.simulator.simulation_history)
                    corrupted_packets = sum(1 for r in st.session_state.simulator.simulation_history if r.get('data_corrupted'))
                    lost_packets = sum(1 for r in st.session_state.simulator.simulation_history if r.get('packet_lost'))
                    
                    if total_packets > 0:
                        st.metric("Data Corruption Rate", f"{corrupted_packets/total_packets:.1%}")
                        st.metric("Packet Loss Rate", f"{lost_packets/total_packets:.1%}")
    
    # Latest packet details
    if st.session_state.simulator.simulation_history:
        latest = st.session_state.simulator.simulation_history[-1]
        st.subheader("📦 Latest Packet Details")
        
        col_a, col_b = st.columns(2)
        with col_a:
            st.write(f"**Original Data:** {latest['data']}")
            st.write(f"**Data with CRC:** {latest['data_with_crc']}")
            if latest.get('corrupted_data'):
                st.write(f"**Corrupted Data:** {latest['corrupted_data']}")
        
        with col_b:
            st.write(f"**CRC Verified:** {'✅ Yes' if latest['crc_verified'] else '❌ No'}")
            st.write(f"**Data Corrupted:** {'⚠️ Yes' if latest.get('data_corrupted') else '✅ No'}")
            st.write(f"**Packet Lost:** {'⚠️ Yes' if latest.get('packet_lost') else '✅ No'}")
    
    # Simulation history visualization
    if st.session_state.simulator.simulation_history:
        st.subheader("📈 Simulation History")
        
        # Create DataFrame for visualization
        df = pd.DataFrame(st.session_state.simulator.simulation_history)
        
        # TCP metrics over time
        fig1 = go.Figure()
        fig1.add_trace(go.Scatter(y=df['tcp_cwnd'], name='Congestion Window', line=dict(color='blue')))
        fig1.add_trace(go.Scatter(y=df['tcp_ssthresh'], name='Slow Start Threshold', line=dict(color='red')))
        fig1.update_layout(title="TCP Tahoe Metrics Over Time", xaxis_title="Packet Number", yaxis_title="Window Size")
        st.plotly_chart(fig1, use_container_width=True)
        
        # Success rate over time
        success_rates = []
        for i in range(len(df)):
            success_rates.append(sum(df.iloc[:i+1]['crc_verified']) / (i + 1))
        
        fig2 = go.Figure()
        fig2.add_trace(go.Scatter(y=success_rates, name='Success Rate', line=dict(color='green')))
        fig2.update_layout(title="Success Rate Over Time", xaxis_title="Packet Number", yaxis_title="Success Rate")
        st.plotly_chart(fig2, use_container_width=True)

def tcp_tahoe_page():
    st.header("📊 TCP Tahoe Congestion Control Analysis")
    
    # Create a new TCP Tahoe instance for analysis
    from network_wrapper import TCPTahoe
    
    st.subheader("TCP Tahoe States")
    st.markdown("""
    - **Slow Start**: Window size doubles every RTT until ssthresh
    - **Congestion Avoidance**: Window size increases linearly (cwnd += 1)
    - **Fast Retransmit**: Triggered by 3 duplicate ACKs, cwnd drops to 1
    - **Timeout**: Triggered by packet loss, cwnd drops to 1
    """)
    
    # Simulation parameters
    col1, col2 = st.columns(2)
    
    with col1:
        st.subheader("Simulation Parameters")
        num_packets = st.slider("Number of Packets", 20, 200, 100)
        
        # Improved simulation modes
        simulation_mode = st.selectbox(
            "Simulation Mode",
            ["Realistic Network", "High Loss Network", "Periodic Loss", "Custom Parameters"]
        )
        
        if simulation_mode == "Realistic Network":
            timeout_prob = 5  # 5% timeout probability
            dup_ack_prob = 3  # 3% duplicate ACK probability
            loss_interval = 0  # No fixed interval
        elif simulation_mode == "High Loss Network":
            timeout_prob = 15  # 15% timeout probability
            dup_ack_prob = 10  # 10% duplicate ACK probability
            loss_interval = 0
        elif simulation_mode == "Periodic Loss":
            timeout_prob = 0
            dup_ack_prob = 0
            loss_interval = st.slider("Loss Interval (packets)", 10, 50, 20)
        else:  # Custom Parameters
            timeout_prob = st.slider("Timeout Probability (%)", 0, 50, 10)
            dup_ack_prob = st.slider("Duplicate ACK Probability (%)", 0, 30, 5)
            loss_interval = st.slider("Fixed Loss Interval (0 for random)", 0, 100, 0)
    
    with col2:
        st.subheader("TCP Tahoe Parameters")
        initial_cwnd = st.number_input("Initial CWND", value=1, min_value=1)
        initial_ssthresh = st.number_input("Initial SSTHRESH", value=64, min_value=1)
        
        # Note about TCP parameters
        st.info("💡 A new TCP instance will be created for each simulation with fresh state.")
    
    if st.button("🚀 Run TCP Tahoe Simulation"):
        # Create a fresh TCP instance for the simulation
        tcp = TCPTahoe()
        
        cwnd_history = []
        ssthresh_history = []
        state_history = []
        event_history = []
        
        for i in range(num_packets):
            # Determine what event occurs
            event_type = "normal"
            
            if simulation_mode == "Periodic Loss" and loss_interval > 0:
                # Periodic packet loss
                if (i + 1) % loss_interval == 0:
                    tcp.timeout_event()
                    event_type = "timeout"
                else:
                    tcp.send_packet()
                    event_type = "normal"
            else:
                # Random events based on probabilities
                rand_val = random.random() * 100
                if rand_val < timeout_prob:
                    tcp.timeout_event()
                    event_type = "timeout"
                elif rand_val < timeout_prob + dup_ack_prob:
                    tcp.duplicate_ack()
                    event_type = "duplicate_ack"
                else:
                    tcp.send_packet()
                    event_type = "normal"
            
            # Record state
            cwnd_history.append(tcp.get_current_cwnd())
            ssthresh_history.append(tcp.get_current_ssthresh())
            state_history.append(tcp.get_current_state())
            event_history.append(event_type)
        
        # Enhanced Visualization
        fig = go.Figure()
        
        # Add CWND trace
        fig.add_trace(go.Scatter(
            x=list(range(len(cwnd_history))),
            y=cwnd_history, 
            name='Congestion Window (CWND)', 
            line=dict(color='blue', width=2),
            mode='lines+markers',
            marker=dict(size=4)
        ))
        
        # Add SSTHRESH trace
        fig.add_trace(go.Scatter(
            x=list(range(len(ssthresh_history))),
            y=ssthresh_history, 
            name='Slow Start Threshold (SSTHRESH)', 
            line=dict(color='red', width=2, dash='dash'),
            mode='lines'
        ))
        
        # Highlight packet loss events
        loss_events_x = []
        loss_events_y = []
        loss_annotations = []
        
        for i, event in enumerate(event_history):
            if event in ["timeout", "duplicate_ack"]:
                loss_events_x.append(i)
                loss_events_y.append(cwnd_history[i])
                loss_annotations.append(dict(
                    x=i, y=cwnd_history[i],
                    text="📉 Loss" if event == "timeout" else "📉 Dup ACK",
                    showarrow=True,
                    arrowhead=2,
                    arrowsize=1,
                    arrowwidth=2,
                    arrowcolor="red"
                ))
        
        # Add loss event markers
        if loss_events_x:
            fig.add_trace(go.Scatter(
                x=loss_events_x,
                y=loss_events_y,
                mode='markers',
                marker=dict(color='red', size=8, symbol='x'),
                name='Packet Loss Events',
                showlegend=True
            ))
        
        fig.update_layout(
            title="TCP Tahoe Congestion Control - Sawtooth Pattern",
            xaxis_title="Transmission Round / Packet Number",
            yaxis_title="Window Size (packets)",
            yaxis=dict(type='log'),  # Log scale to better show the pattern
            hovermode='x unified',
            annotations=loss_annotations
        )
        
        st.plotly_chart(fig, use_container_width=True)
        
        # Statistics and Analysis
        col_a, col_b, col_c = st.columns(3)
        
        with col_a:
            st.subheader("📈 Statistics")
            max_cwnd = max(cwnd_history)
            min_cwnd = min(cwnd_history)
            avg_cwnd = sum(cwnd_history) / len(cwnd_history)
            loss_events = sum(1 for e in event_history if e in ["timeout", "duplicate_ack"])
            
            st.metric("Max CWND", max_cwnd)
            st.metric("Min CWND", min_cwnd)
            st.metric("Avg CWND", f"{avg_cwnd:.1f}")
            st.metric("Loss Events", loss_events)
        
        with col_b:
            st.subheader("🔄 State Transitions")
            state_df = pd.DataFrame({'State': state_history})
            state_counts = state_df['State'].value_counts()
            st.bar_chart(state_counts)
        
        with col_c:
            st.subheader("📊 Event Distribution")
            event_df = pd.DataFrame({'Event': event_history})
            event_counts = event_df['Event'].value_counts()
            
            # Create a pie chart for events
            fig_pie = go.Figure(data=[go.Pie(
                labels=event_counts.index,
                values=event_counts.values,
                hole=0.3
            )])
            fig_pie.update_layout(title="Event Distribution")
            st.plotly_chart(fig_pie, use_container_width=True)
        
        # Detailed Analysis
        st.subheader("🔍 Detailed Analysis")
        
        # Show sawtooth pattern characteristics
        if loss_events > 0:
            st.success(f"✅ Sawtooth pattern detected! {loss_events} packet loss events triggered cwnd reductions.")
            
            # Calculate average cycle length
            loss_indices = [i for i, e in enumerate(event_history) if e in ["timeout", "duplicate_ack"]]
            if len(loss_indices) > 1:
                cycle_lengths = [loss_indices[i+1] - loss_indices[i] for i in range(len(loss_indices)-1)]
                avg_cycle = sum(cycle_lengths) / len(cycle_lengths)
                st.info(f"📊 Average cycle length: {avg_cycle:.1f} packets")
        else:
            st.warning("⚠️ No packet loss events occurred. Increase loss probability to see sawtooth pattern.")
        
        # Show the characteristic TCP Tahoe behavior
        st.markdown("### 🎯 TCP Tahoe Characteristics Observed:")
        st.markdown(f"""
        - **Exponential Growth**: In slow start, CWND doubles each RTT
        - **Linear Growth**: In congestion avoidance, CWND increases by 1 each RTT
        - **Multiplicative Decrease**: On loss, CWND drops to 1 (classic Tahoe behavior)
        - **Threshold Update**: SSTHRESH becomes CWND/2 after loss
        """)
        
        # Export data option
        if st.button("📥 Export Simulation Data"):
            export_df = pd.DataFrame({
                'Packet': range(len(cwnd_history)),
                'CWND': cwnd_history,
                'SSTHRESH': ssthresh_history,
                'State': state_history,
                'Event': event_history
            })
            
            csv = export_df.to_csv(index=False)
            st.download_button(
                label="Download CSV",
                data=csv,
                file_name="tcp_tahoe_simulation.csv",
                mime="text/csv"
            )

def stop_and_wait_page():
    st.header("⏳ Stop-and-Wait Protocol Analysis")
    
    from network_wrapper import StopAndWait
    saw = StopAndWait()
    
    st.subheader("Protocol Overview")
    st.markdown("""
    - **Sender**: Sends packet and waits for ACK
    - **Receiver**: Sends ACK upon receiving packet
    - **Timeout**: Retransmits if ACK not received
    - **Sequence Numbers**: Alternates between 0 and 1
    """)
    
    col1, col2 = st.columns(2)
    
    with col1:
        st.subheader("Simulation Parameters")
        num_packets = st.slider("Number of Packets", 5, 50, 20)
        timeout_prob = st.slider("Timeout Probability (%)", 0, 50, 15)
        packet_data = st.text_input("Packet Data", "Hello")
    
    with col2:
        st.subheader("Protocol Statistics")
        if st.button("🚀 Run Stop-and-Wait Simulation"):
            results = []
            
            for i in range(num_packets):
                # Send packet
                sent = saw.send_packet(f"{packet_data}_{i}")
                
                # Simulate network conditions
                if random.random() < timeout_prob / 100:
                    # Timeout occurs
                    saw.check_timeout()
                    saw.retransmit()
                    results.append({"Packet": i, "Status": "Timeout", "RTT": 0})
                else:
                    # Successful transmission
                    saw.receive_ack(saw.get_total_packets_sent() - 1)
                    results.append({"Packet": i, "Status": "Success", "RTT": random.randint(50, 200)})
            
            # Display results
            results_df = pd.DataFrame(results)
            st.dataframe(results_df)
            
            # Statistics
            st.metric("Total Packets Sent", saw.get_total_packets_sent())
            st.metric("Total Packets ACKed", saw.get_total_packets_acked())
            st.metric("Total Timeouts", saw.get_total_timeouts())
            st.metric("Success Rate", f"{saw.get_total_packets_acked() / saw.get_total_packets_sent():.2%}")

def prims_algorithm_page():
    st.header("🌳 Prim's Algorithm - Minimum Spanning Tree")
    
    st.subheader("Algorithm Overview")
    st.markdown("""
    - **Purpose**: Find minimum spanning tree of a weighted graph
    - **Method**: Greedy algorithm starting from any vertex
    - **Complexity**: O(V²) for adjacency matrix representation
    """)
    
    col1, col2 = st.columns(2)
    
    with col1:
        st.subheader("Graph Configuration")
        num_vertices = st.slider("Number of Vertices", 3, 10, 5)
        
        # Generate random graph
        if st.button("🎲 Generate Random Graph"):
            prims = PrimsAlgorithm(num_vertices)
            
            # Add random edges
            edges_added = 0
            max_edges = num_vertices * (num_vertices - 1) // 2
            
            for i in range(num_vertices):
                for j in range(i + 1, num_vertices):
                    if random.random() < 0.7 and edges_added < max_edges:  # 70% chance of edge
                        weight = random.randint(1, 20)
                        prims.add_edge(i, j, weight)
                        edges_added += 1
            
            st.session_state.prims_graph = prims
            st.success(f"Generated graph with {edges_added} edges!")
    
    with col2:
        st.subheader("MST Computation")
        if st.session_state.prims_graph and st.button("🌳 Compute MST"):
            prims = st.session_state.prims_graph
            prims.find_mst()
            
            total_weight = prims.get_total_weight()
            st.metric("MST Total Weight", total_weight)
            st.metric("Vertices", prims.get_vertex_count())
            
            # Visualize the graph (simplified)
            st.subheader("Graph Visualization")
            st.markdown("**Note**: This is a simplified representation. The actual MST computation is performed in C++.")
            
            # Create a simple network graph visualization
            G = nx.Graph()
            for i in range(prims.get_vertex_count()):
                G.add_node(i)
            
            # Add some sample edges for visualization
            for i in range(prims.get_vertex_count() - 1):
                G.add_edge(i, i + 1, weight=random.randint(1, 10))
            
            fig, ax = plt.subplots(figsize=(8, 6))
            pos = nx.spring_layout(G)
            nx.draw(G, pos, with_labels=True, node_color='lightblue', 
                   node_size=500, font_size=16, font_weight='bold')
            nx.draw_networkx_edge_labels(G, pos)
            st.pyplot(fig)

def crc_page():
    st.header("🔍 CRC Error Detection Analysis")
    
    st.subheader("CRC Overview")
    st.markdown("""
    - **Purpose**: Detect errors in data transmission
    - **Method**: Polynomial division with remainder
    - **Polynomial**: x³ + x + 1 (1011 in binary)
    """)
    
    col1, col2 = st.columns(2)
    
    with col1:
        st.subheader("CRC Parameters")
        polynomial = st.selectbox("CRC Polynomial", ["1011", "1101", "10011"], index=0)
        crc = CRC(polynomial)
        
        data_input = st.text_area("Input Data", "Hello World")
        
        if st.button("🔍 Calculate CRC"):
            crc_value = crc.calculate_crc(data_input)
            data_with_crc = crc.add_crc_to_data(data_input)
            
            st.metric("CRC Value", crc_value)
            st.metric("Data with CRC", data_with_crc)
            
            # Verify CRC
            is_valid = crc.verify_crc(data_input, crc_value)
            st.metric("CRC Verification", "✅ Valid" if is_valid else "❌ Invalid")
    
    with col2:
        st.subheader("Error Simulation")
        test_data = st.text_input("Test Data", "Test Message")
        error_position = st.slider("Error Position", 0, len(test_data) - 1, 0)
        
        if st.button("🚨 Simulate Error"):
            crc = CRC("1011")
            
            # Calculate original CRC
            original_crc = crc.calculate_crc(test_data)
            data_with_crc = crc.add_crc_to_data(test_data)
            
            # Introduce error
            error_data = list(test_data)
            error_data[error_position] = chr(ord(error_data[error_position]) ^ 1)  # Flip one bit
            error_data = ''.join(error_data)
            
            # Check if error is detected
            error_detected = not crc.verify_crc(error_data, original_crc)
            
            st.metric("Original Data", test_data)
            st.metric("Corrupted Data", error_data)
            st.metric("Error Detected", "✅ Yes" if error_detected else "❌ No")
            
            if error_detected:
                st.success("CRC successfully detected the error!")
            else:
                st.error("CRC failed to detect the error!")

def about_page():
    st.header("📚 About This Network Simulator")
    
    st.markdown("""
    ## 🌐 Network Protocol Simulator
    
    This application simulates various network protocols to study network reliability and performance.
    
    ### 🔧 Technologies Used
    - **Backend**: C++ for high-performance protocol implementations
    - **Frontend**: Python Streamlit for interactive visualization
    - **Integration**: ctypes for C++/Python communication
    
    ### 📊 Protocols Implemented
    
    #### 1. TCP Tahoe Congestion Control
    - Slow Start phase
    - Congestion Avoidance phase
    - Fast Retransmit on duplicate ACKs
    - Timeout handling
    
    #### 2. Stop-and-Wait Protocol
    - Reliable packet transmission
    - ACK-based flow control
    - Timeout and retransmission
    - Sequence number management
    
    #### 3. Prim's Algorithm
    - Minimum Spanning Tree computation
    - Greedy approach
    - Graph optimization
    
    #### 4. CRC Error Detection
    - Cyclic Redundancy Check
    - Error detection capabilities
    - Polynomial division method
    
    ### 🎯 Learning Objectives
    - Understand network protocol behavior
    - Study congestion control mechanisms
    - Analyze error detection methods
    - Visualize network performance metrics
    
    ### 🚀 Getting Started
    1. Compile the C++ library: `make`
    2. Install Python dependencies: `pip install -r requirements.txt`
    3. Run the application: `streamlit run app.py`
    
    ### 📈 Features
    - Real-time simulation
    - Interactive visualizations
    - Performance metrics
    - Error analysis
    - Network condition simulation
    """)

if __name__ == "__main__":
    main() 