#ifndef CROSS_LAYER_PROTOCOL_H
#define CROSS_LAYER_PROTOCOL_H

#include <vector>
#include <string>
#include <memory>
#include <map>
#include <functional>
#include <chrono>

// Forward declarations
class TCPTahoe;
class LTENetwork;

enum class LayerType {
    PHYSICAL,
    DATA_LINK,
    NETWORK,
    TRANSPORT,
    APPLICATION
};

enum class CrossLayerEvent {
    SIGNAL_STRENGTH_CHANGE,
    HANDOVER_INITIATION,
    CONGESTION_DETECTED,
    ERROR_RATE_CHANGE,
    BANDWIDTH_CHANGE,
    LATENCY_CHANGE
};

struct LayerInfo {
    LayerType layer;
    std::string status;
    std::map<std::string, double> metrics;
    uint64_t timestamp;
};

struct CrossLayerMessage {
    LayerType source;
    LayerType destination;
    CrossLayerEvent event;
    std::map<std::string, double> parameters;
    uint64_t timestamp;
    std::string message;
};

class CrossLayerOptimizer {
private:
    std::map<LayerType, LayerInfo> layer_states;
    std::vector<CrossLayerMessage> message_history;
    std::vector<std::function<void(const CrossLayerMessage&)>> event_handlers;
    
    // Layer references
    std::shared_ptr<TCPTahoe> tcp_layer;
    std::shared_ptr<LTENetwork> lte_network;
    
    // Optimization parameters
    bool adaptive_optimization_enabled;
    double optimization_weight_throughput;
    double optimization_weight_latency;
    double optimization_weight_energy;
    
    // Performance metrics
    std::vector<double> throughput_history;
    std::vector<double> latency_history;
    std::vector<double> energy_consumption_history;
    std::vector<double> packet_loss_history;

public:
    CrossLayerOptimizer();
    
    // Layer management
    void register_layer(LayerType layer, std::shared_ptr<void> layer_instance);
    void update_layer_state(LayerType layer, const LayerInfo& info);
    LayerInfo get_layer_state(LayerType layer) const;
    
    // Cross-layer communication
    void send_cross_layer_message(const CrossLayerMessage& message);
    void register_event_handler(std::function<void(const CrossLayerMessage&)> handler);
    std::vector<CrossLayerMessage> get_message_history() const;
    
    // Optimization strategies
    void enable_adaptive_optimization(bool enable);
    void set_optimization_weights(double throughput, double latency, double energy);
    void optimize_network_performance();
    void adapt_to_network_conditions();
    
    // Specific optimization functions
    void optimize_tcp_congestion_control();
    void optimize_error_correction();
    void optimize_handover_decisions();
    void optimize_power_consumption();
    
    // Performance monitoring
    double get_current_throughput() const;
    double get_current_latency() const;
    double get_current_energy_consumption() const;
    double get_current_packet_loss_rate() const;
    std::vector<double> get_throughput_history() const;
    std::vector<double> get_latency_history() const;
    
    // Network condition simulation
    void simulate_mobility();
    void simulate_interference();
    void simulate_traffic_variation();
    
    // Event handling
    void handle_signal_strength_change(double new_strength);
    void handle_handover_event(const std::string& target_cell);
    void handle_congestion_event(double congestion_level);
    void handle_error_rate_change(double new_error_rate);
    
    // Reset and cleanup
    void reset();
    void clear_history();
};

#endif // CROSS_LAYER_PROTOCOL_H 