#ifndef LTE_NETWORK_H
#define LTE_NETWORK_H

#include <vector>
#include <string>
#include <memory>
#include <map>
#include <chrono>

enum class LTEState {
    IDLE,
    CONNECTED,
    HANDOVER_PREPARATION,
    HANDOVER_EXECUTION,
    HANDOVER_COMPLETION
};

enum class HandoverType {
    INTRA_LTE,
    INTER_LTE,
    LTE_TO_3G,
    LTE_TO_WIFI
};

enum class ResourceBlockType {
    UPLINK,
    DOWNLINK
};

struct CellInfo {
    int cell_id;
    double signal_strength;     // RSRP (dBm)
    double signal_quality;      // RSRQ (dB)
    double interference_level;
    int load_percentage;
    std::string technology;     // "LTE", "3G", "WiFi"
    double latitude;
    double longitude;
};

struct ResourceBlock {
    int rb_id;
    ResourceBlockType type;
    bool allocated;
    int user_id;
    double frequency;
    int bandwidth;
    uint64_t allocation_time;
};

struct HandoverEvent {
    int source_cell;
    int target_cell;
    HandoverType type;
    double trigger_rsrp;
    double target_rsrp;
    uint64_t start_time;
    uint64_t completion_time;
    bool success;
    std::string failure_reason;
};

struct UserEquipment {
    int ue_id;
    double x_position;
    double y_position;
    double velocity;
    double direction;
    int serving_cell;
    LTEState state;
    std::vector<ResourceBlock> allocated_rbs;
    double current_throughput;
    double battery_level;
    std::vector<CellInfo> neighbor_cells;
};

class LTENetwork {
private:
    std::vector<CellInfo> cells;
    std::vector<UserEquipment> users;
    std::vector<ResourceBlock> resource_blocks;
    std::vector<HandoverEvent> handover_history;
    
    // Network parameters
    double handover_margin;
    double handover_hysteresis;
    int handover_time_to_trigger;
    double interference_threshold;
    int max_users_per_cell;
    
    // Scheduling and resource management
    std::string scheduling_algorithm;  // "Round Robin", "Proportional Fair", "Max C/I"
    std::map<int, std::vector<int>> user_rb_allocation;
    std::map<int, double> user_cqi_values;  // Channel Quality Indicator
    
    // Performance metrics
    std::vector<double> network_throughput_history;
    std::vector<double> handover_success_rate_history;
    std::vector<double> network_latency_history;
    std::vector<int> active_users_history;
    
    // Mobility model parameters
    bool mobility_enabled;
    double mobility_speed_min;
    double mobility_speed_max;
    std::string mobility_model;  // "Random Walk", "Manhattan", "Highway"

public:
    LTENetwork();
    
    // Network initialization
    void initialize_network(int num_cells, int num_users);
    void add_cell(const CellInfo& cell);
    void add_user(const UserEquipment& user);
    
    // Cell management
    std::vector<CellInfo> get_cells() const;
    CellInfo get_cell_info(int cell_id) const;
    void update_cell_load(int cell_id, int load_percentage);
    void update_cell_interference(int cell_id, double interference);
    
    // User equipment management
    std::vector<UserEquipment> get_users() const;
    UserEquipment get_user_info(int ue_id) const;
    void update_user_position(int ue_id, double x, double y);
    void update_user_state(int ue_id, LTEState state);
    
    // Resource block management
    std::vector<ResourceBlock> allocate_resource_blocks(int ue_id, int num_rbs);
    void deallocate_resource_blocks(int ue_id);
    void update_resource_allocation();
    double calculate_user_throughput(int ue_id);
    
    // Handover management
    bool should_trigger_handover(int ue_id);
    HandoverEvent initiate_handover(int ue_id, int target_cell);
    void execute_handover(int ue_id, int target_cell);
    void complete_handover(int ue_id);
    std::vector<HandoverEvent> get_handover_history() const;
    
    // Signal strength and quality
    double calculate_rsrp(int ue_id, int cell_id);
    double calculate_rsrq(int ue_id, int cell_id);
    double calculate_sinr(int ue_id, int cell_id);
    std::vector<CellInfo> get_neighbor_cells(int ue_id);
    
    // Scheduling algorithms
    void set_scheduling_algorithm(const std::string& algorithm);
    void round_robin_scheduler();
    void proportional_fair_scheduler();
    void max_ci_scheduler();
    
    // Mobility simulation
    void enable_mobility(bool enable);
    void set_mobility_model(const std::string& model);
    void update_user_mobility();
    void simulate_random_walk_mobility();
    void simulate_manhattan_mobility();
    void simulate_highway_mobility();
    
    // Performance monitoring
    double get_network_throughput() const;
    double get_handover_success_rate() const;
    double get_network_latency() const;
    int get_active_users_count() const;
    std::vector<double> get_throughput_history() const;
    std::vector<double> get_handover_success_rate_history() const;
    
    // Network optimization
    void optimize_handover_parameters();
    void optimize_resource_allocation();
    void load_balancing();
    void interference_coordination();
    
    // Configuration
    void set_handover_parameters(double margin, double hysteresis, int time_to_trigger);
    void set_network_parameters(double interference_threshold, int max_users);
    
    // Simulation control
    void step_simulation();
    void reset_network();
    void generate_network_events();
    
    // Statistics and reporting
    std::map<std::string, double> get_network_statistics() const;
    std::string generate_performance_report() const;
};

#endif // LTE_NETWORK_H 