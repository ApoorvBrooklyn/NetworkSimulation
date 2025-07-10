#include "cross_layer_protocol.h"
#include "tcp_tahoe.h"
#include "lte_network.h"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <random>

CrossLayerOptimizer::CrossLayerOptimizer() {
    adaptive_optimization_enabled = true;
    optimization_weight_throughput = 0.4;
    optimization_weight_latency = 0.3;
    optimization_weight_energy = 0.3;
    
    // Initialize layer states
    for (auto layer : {LayerType::PHYSICAL, LayerType::DATA_LINK, LayerType::NETWORK, 
                       LayerType::TRANSPORT, LayerType::APPLICATION}) {
        LayerInfo info;
        info.layer = layer;
        info.status = "idle";
        info.timestamp = std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count();
        layer_states[layer] = info;
    }
}

void CrossLayerOptimizer::register_layer(LayerType layer, std::shared_ptr<void> layer_instance) {
    switch (layer) {
        case LayerType::TRANSPORT:
            tcp_layer = std::static_pointer_cast<TCPTahoe>(layer_instance);
            break;
        case LayerType::DATA_LINK:
            // data_link_layer = std::static_pointer_cast<StopAndWait>(layer_instance);
            break;
        case LayerType::PHYSICAL:
            lte_network = std::static_pointer_cast<LTENetwork>(layer_instance);
            break;
        default:
            break;
    }
}

void CrossLayerOptimizer::update_layer_state(LayerType layer, const LayerInfo& info) {
    layer_states[layer] = info;
    
    // Trigger optimization when layer states change
    if (adaptive_optimization_enabled) {
        adapt_to_network_conditions();
    }
}

LayerInfo CrossLayerOptimizer::get_layer_state(LayerType layer) const {
    auto it = layer_states.find(layer);
    return (it != layer_states.end()) ? it->second : LayerInfo{};
}

void CrossLayerOptimizer::send_cross_layer_message(const CrossLayerMessage& message) {
    message_history.push_back(message);
    
    // Handle the message based on its type
    switch (message.event) {
        case CrossLayerEvent::SIGNAL_STRENGTH_CHANGE:
            handle_signal_strength_change(message.parameters.at("signal_strength"));
            break;
        case CrossLayerEvent::HANDOVER_INITIATION:
            handle_handover_event(message.message);
            break;
        case CrossLayerEvent::CONGESTION_DETECTED:
            handle_congestion_event(message.parameters.at("congestion_level"));
            break;
        case CrossLayerEvent::ERROR_RATE_CHANGE:
            handle_error_rate_change(message.parameters.at("error_rate"));
            break;
        case CrossLayerEvent::BANDWIDTH_CHANGE:
        case CrossLayerEvent::LATENCY_CHANGE:
            adapt_to_network_conditions();
            break;
    }
    
    // Notify registered event handlers
    for (auto& handler : event_handlers) {
        handler(message);
    }
}

void CrossLayerOptimizer::register_event_handler(std::function<void(const CrossLayerMessage&)> handler) {
    event_handlers.push_back(handler);
}

std::vector<CrossLayerMessage> CrossLayerOptimizer::get_message_history() const {
    return message_history;
}

void CrossLayerOptimizer::enable_adaptive_optimization(bool enable) {
    adaptive_optimization_enabled = enable;
}

void CrossLayerOptimizer::set_optimization_weights(double throughput, double latency, double energy) {
    double total = throughput + latency + energy;
    optimization_weight_throughput = throughput / total;
    optimization_weight_latency = latency / total;
    optimization_weight_energy = energy / total;
}

void CrossLayerOptimizer::optimize_network_performance() {
    // Collect current performance metrics
    double current_throughput = get_current_throughput();
    double current_latency = get_current_latency();
    double current_energy = get_current_energy_consumption();
    
    throughput_history.push_back(current_throughput);
    latency_history.push_back(current_latency);
    energy_consumption_history.push_back(current_energy);
    
    // Optimize each layer based on current conditions
    optimize_tcp_congestion_control();
    optimize_error_correction();
    optimize_handover_decisions();
    optimize_power_consumption();
}

void CrossLayerOptimizer::adapt_to_network_conditions() {
    if (!adaptive_optimization_enabled) return;
    
    // Analyze current network conditions across all layers
    LayerInfo physical_info = get_layer_state(LayerType::PHYSICAL);
    LayerInfo transport_info = get_layer_state(LayerType::TRANSPORT);
    
    // Get signal strength and interference from physical layer
    double signal_strength = physical_info.metrics.count("signal_strength") ? 
                             physical_info.metrics.at("signal_strength") : -80.0;
    double interference = physical_info.metrics.count("interference") ? 
                         physical_info.metrics.at("interference") : 0.1;
    
    // Get congestion info from transport layer
    double congestion_level = transport_info.metrics.count("congestion") ? 
                             transport_info.metrics.at("congestion") : 0.0;
    
    // Adaptive strategies based on conditions
    if (signal_strength < -90.0) {  // Poor signal
        // Increase error correction strength
        optimize_error_correction();
        // Reduce TCP aggression
        if (tcp_layer) {
            tcp_layer->set_algorithm(CongestionAlgorithm::TAHOE);
        }
    } else if (signal_strength > -70.0 && interference < 0.05) {  // Good conditions
        // Use more aggressive algorithms
        if (tcp_layer) {
            tcp_layer->set_algorithm(CongestionAlgorithm::BBR);
        }
    }
    
    if (congestion_level > 0.7) {  // High congestion
        // Trigger congestion management
        CrossLayerMessage msg;
        msg.source = LayerType::NETWORK;
        msg.destination = LayerType::TRANSPORT;
        msg.event = CrossLayerEvent::CONGESTION_DETECTED;
        msg.parameters["congestion_level"] = congestion_level;
        msg.timestamp = std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count();
        send_cross_layer_message(msg);
    }
}

void CrossLayerOptimizer::optimize_tcp_congestion_control() {
    if (!tcp_layer) return;
    
    // Get current network conditions
    double packet_loss_rate = get_current_packet_loss_rate();
    double throughput = get_current_throughput();
    
    // Choose optimal congestion control algorithm based on conditions
    if (packet_loss_rate > 0.05) {
        // High loss rate - use conservative algorithm
        tcp_layer->set_algorithm(CongestionAlgorithm::TAHOE);
    } else if (packet_loss_rate < 0.01 && throughput < 5.0) {
        // Low loss, low throughput - use aggressive algorithm
        tcp_layer->set_algorithm(CongestionAlgorithm::BBR);
    } else {
        // Moderate conditions - use balanced algorithm
        tcp_layer->set_algorithm(CongestionAlgorithm::CUBIC);
    }
    
    // Set network conditions in TCP layer
    LayerInfo physical_info = get_layer_state(LayerType::PHYSICAL);
    double utilization = physical_info.metrics.count("utilization") ? 
                        physical_info.metrics.at("utilization") : 0.5;
    int delay = physical_info.metrics.count("delay") ? 
               static_cast<int>(physical_info.metrics.at("delay")) : 50;
    
    tcp_layer->set_network_conditions(packet_loss_rate, utilization, delay);
}

void CrossLayerOptimizer::optimize_error_correction() {
    // Adjust error correction based on channel conditions
    LayerInfo physical_info = get_layer_state(LayerType::PHYSICAL);
    double error_rate = physical_info.metrics.count("error_rate") ? 
                       physical_info.metrics.at("error_rate") : 0.01;
    
    if (error_rate > 0.05) {
        // High error rate - use stronger error correction
        // This would involve configuring the CRC or other error correction schemes
        CrossLayerMessage msg;
        msg.source = LayerType::NETWORK;
        msg.destination = LayerType::DATA_LINK;
        msg.event = CrossLayerEvent::ERROR_RATE_CHANGE;
        msg.parameters["error_rate"] = error_rate;
        msg.parameters["correction_strength"] = 1.5;
        msg.message = "Increase error correction strength";
        msg.timestamp = std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count();
        send_cross_layer_message(msg);
    }
}

void CrossLayerOptimizer::optimize_handover_decisions() {
    if (!lte_network) return;
    
    // Get mobility and signal information
    LayerInfo physical_info = get_layer_state(LayerType::PHYSICAL);
    double mobility_speed = physical_info.metrics.count("mobility_speed") ? 
                           physical_info.metrics.at("mobility_speed") : 0.0;
    
    if (mobility_speed > 50.0) {  // High mobility (km/h)
        // Adjust handover parameters for high mobility
        lte_network->set_handover_parameters(3.0, 2.0, 160);  // More aggressive
    } else if (mobility_speed < 5.0) {  // Low mobility
        // Conservative handover parameters
        lte_network->set_handover_parameters(6.0, 1.0, 320);
    }
}

void CrossLayerOptimizer::optimize_power_consumption() {
    // Power optimization based on battery level and network conditions
    LayerInfo application_info = get_layer_state(LayerType::APPLICATION);
    double battery_level = application_info.metrics.count("battery_level") ? 
                          application_info.metrics.at("battery_level") : 1.0;
    
    if (battery_level < 0.2) {  // Low battery
        // Implement power-saving strategies
        CrossLayerMessage msg;
        msg.source = LayerType::APPLICATION;
        msg.destination = LayerType::PHYSICAL;
        msg.event = CrossLayerEvent::BANDWIDTH_CHANGE;
        msg.parameters["power_save_mode"] = 1.0;
        msg.parameters["reduce_transmission_power"] = 0.7;
        msg.message = "Enable power saving mode";
        msg.timestamp = std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count();
        send_cross_layer_message(msg);
    }
}

// Performance monitoring methods
double CrossLayerOptimizer::get_current_throughput() const {
    if (tcp_layer) {
        return tcp_layer->get_current_throughput();
    }
    return 0.0;
}

double CrossLayerOptimizer::get_current_latency() const {
    LayerInfo network_info = get_layer_state(LayerType::NETWORK);
    return network_info.metrics.count("latency") ? 
           network_info.metrics.at("latency") : 50.0;
}

double CrossLayerOptimizer::get_current_energy_consumption() const {
    LayerInfo physical_info = get_layer_state(LayerType::PHYSICAL);
    return physical_info.metrics.count("energy_consumption") ? 
           physical_info.metrics.at("energy_consumption") : 100.0;
}

double CrossLayerOptimizer::get_current_packet_loss_rate() const {
    if (tcp_layer) {
        return tcp_layer->get_packet_loss_rate();
    }
    return 0.01;
}

std::vector<double> CrossLayerOptimizer::get_throughput_history() const {
    return throughput_history;
}

std::vector<double> CrossLayerOptimizer::get_latency_history() const {
    return latency_history;
}

// Network condition simulation
void CrossLayerOptimizer::simulate_mobility() {
    // Simulate user movement and its impact on signal strength
    static double position = 0.0;
    position += 1.0;  // Move 1 unit per simulation step
    
    // Calculate signal strength based on position (simplified model)
    double distance_to_base_station = std::abs(std::fmod(position, 1000.0) - 500.0);
    double signal_strength = -70.0 - (distance_to_base_station / 10.0);
    
    // Update physical layer state
    LayerInfo physical_info = get_layer_state(LayerType::PHYSICAL);
    physical_info.metrics["signal_strength"] = signal_strength;
    physical_info.metrics["mobility_speed"] = 30.0;  // 30 km/h
    update_layer_state(LayerType::PHYSICAL, physical_info);
    
    // Send mobility event
    CrossLayerMessage msg;
    msg.source = LayerType::PHYSICAL;
    msg.destination = LayerType::NETWORK;
    msg.event = CrossLayerEvent::SIGNAL_STRENGTH_CHANGE;
    msg.parameters["signal_strength"] = signal_strength;
    msg.parameters["mobility_speed"] = 30.0;
    msg.timestamp = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
    send_cross_layer_message(msg);
}

void CrossLayerOptimizer::simulate_interference() {
    // Simulate random interference
    static std::mt19937 gen(std::random_device{}());
    std::uniform_real_distribution<> dis(0.0, 0.2);
    
    double interference_level = dis(gen);
    
    LayerInfo physical_info = get_layer_state(LayerType::PHYSICAL);
    physical_info.metrics["interference"] = interference_level;
    physical_info.metrics["error_rate"] = interference_level * 0.1;
    update_layer_state(LayerType::PHYSICAL, physical_info);
}

void CrossLayerOptimizer::simulate_traffic_variation() {
    // Simulate varying network traffic
    static double time = 0.0;
    time += 0.1;
    
    // Sinusoidal traffic pattern
    double traffic_load = 0.5 + 0.4 * std::sin(time);
    
    LayerInfo network_info = get_layer_state(LayerType::NETWORK);
    network_info.metrics["traffic_load"] = traffic_load;
    network_info.metrics["congestion"] = traffic_load > 0.8 ? traffic_load : 0.0;
    update_layer_state(LayerType::NETWORK, network_info);
}

// Event handlers
void CrossLayerOptimizer::handle_signal_strength_change(double new_strength) {
    if (new_strength < -90.0) {
        // Poor signal - trigger handover if available
        if (lte_network) {
            CrossLayerMessage msg;
            msg.source = LayerType::PHYSICAL;
            msg.destination = LayerType::NETWORK;
            msg.event = CrossLayerEvent::HANDOVER_INITIATION;
            msg.parameters["trigger_rsrp"] = new_strength;
            msg.message = "Handover required due to poor signal";
            msg.timestamp = std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::steady_clock::now().time_since_epoch()).count();
            send_cross_layer_message(msg);
        }
    }
}

void CrossLayerOptimizer::handle_handover_event(const std::string& target_cell) {
    // Coordinate handover across layers
    if (tcp_layer) {
        // Temporarily reduce TCP aggressiveness during handover
        tcp_layer->set_algorithm(CongestionAlgorithm::TAHOE);
    }
}

void CrossLayerOptimizer::handle_congestion_event(double congestion_level) {
    if (tcp_layer && congestion_level > 0.5) {
        // Reduce network load by adjusting TCP behavior
        tcp_layer->set_network_conditions(0.05, congestion_level, 100);
    }
}

void CrossLayerOptimizer::handle_error_rate_change(double new_error_rate) {
    if (new_error_rate > 0.1) {
        // High error rate - enable more robust protocols
        CrossLayerMessage msg;
        msg.source = LayerType::DATA_LINK;
        msg.destination = LayerType::TRANSPORT;
        msg.event = CrossLayerEvent::ERROR_RATE_CHANGE;
        msg.parameters["error_rate"] = new_error_rate;
        msg.message = "Enable robust error handling";
        msg.timestamp = std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count();
        send_cross_layer_message(msg);
    }
}

void CrossLayerOptimizer::reset() {
    message_history.clear();
    throughput_history.clear();
    latency_history.clear();
    energy_consumption_history.clear();
    packet_loss_history.clear();
    
    // Reset all layer states to idle
    for (auto& [layer, info] : layer_states) {
        info.status = "idle";
        info.metrics.clear();
        info.timestamp = std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count();
    }
}

void CrossLayerOptimizer::clear_history() {
    message_history.clear();
    throughput_history.clear();
    latency_history.clear();
    energy_consumption_history.clear();
    packet_loss_history.clear();
} 