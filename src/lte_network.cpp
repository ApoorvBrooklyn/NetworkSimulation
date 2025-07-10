#include "lte_network.h"
#include <cmath>
#include <algorithm>
#include <random>
#include <sstream>

LTENetwork::LTENetwork() {
    // Initialize handover parameters
    handover_margin = 3.0;           // dB
    handover_hysteresis = 1.0;       // dB
    handover_time_to_trigger = 320;  // ms
    interference_threshold = 0.1;
    max_users_per_cell = 100;
    
    // Initialize scheduling
    scheduling_algorithm = "Proportional Fair";
    
    // Initialize mobility
    mobility_enabled = false;
    mobility_speed_min = 5.0;   // km/h
    mobility_speed_max = 120.0; // km/h
    mobility_model = "Random Walk";
}

void LTENetwork::initialize_network(int num_cells, int num_users) {
    cells.clear();
    users.clear();
    resource_blocks.clear();
    handover_history.clear();
    
    // Create cells in a hexagonal layout
    for (int i = 0; i < num_cells; i++) {
        CellInfo cell;
        cell.cell_id = i;
        cell.signal_strength = -70.0; // Default RSRP
        cell.signal_quality = -10.0;  // Default RSRQ
        cell.interference_level = 0.05;
        cell.load_percentage = 0;
        cell.technology = "LTE";
        
        // Position cells in a grid pattern
        int cols = static_cast<int>(std::sqrt(num_cells));
        cell.latitude = (i / cols) * 1000.0;  // 1km spacing
        cell.longitude = (i % cols) * 1000.0;
        
        cells.push_back(cell);
    }
    
    // Create users with random positions
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> pos_dis(0.0, std::sqrt(num_cells) * 1000.0);
    std::uniform_real_distribution<> speed_dis(mobility_speed_min, mobility_speed_max);
    std::uniform_real_distribution<> dir_dis(0.0, 2 * M_PI);
    
    for (int i = 0; i < num_users; i++) {
        UserEquipment ue;
        ue.ue_id = i;
        ue.x_position = pos_dis(gen);
        ue.y_position = pos_dis(gen);
        ue.velocity = speed_dis(gen);
        ue.direction = dir_dis(gen);
        ue.serving_cell = find_best_serving_cell(ue.x_position, ue.y_position);
        ue.state = LTEState::IDLE;
        ue.current_throughput = 0.0;
        ue.battery_level = 1.0;
        
        users.push_back(ue);
    }
    
    // Initialize resource blocks (simplified - 100 RBs per cell)
    for (int cell_id = 0; cell_id < num_cells; cell_id++) {
        for (int rb_id = 0; rb_id < 100; rb_id++) {
            ResourceBlock rb;
            rb.rb_id = rb_id + cell_id * 100;
            rb.type = (rb_id < 50) ? ResourceBlockType::DOWNLINK : ResourceBlockType::UPLINK;
            rb.allocated = false;
            rb.user_id = -1;
            rb.frequency = 2100.0 + rb_id * 0.18; // MHz
            rb.bandwidth = 180; // kHz
            rb.allocation_time = 0;
            
            resource_blocks.push_back(rb);
        }
    }
}

int LTENetwork::find_best_serving_cell(double x, double y) {
    int best_cell = 0;
    double best_rsrp = -200.0;
    
    for (size_t i = 0; i < cells.size(); i++) {
        double distance = std::sqrt(std::pow(x - cells[i].longitude, 2) + 
                                   std::pow(y - cells[i].latitude, 2));
        
        // Simplified path loss model: RSRP = -70 - 20*log10(distance_km)
        double rsrp = -70.0 - 20.0 * std::log10(std::max(distance / 1000.0, 0.001));
        
        if (rsrp > best_rsrp) {
            best_rsrp = rsrp;
            best_cell = static_cast<int>(i);
        }
    }
    
    return best_cell;
}

void LTENetwork::add_cell(const CellInfo& cell) {
    cells.push_back(cell);
}

void LTENetwork::add_user(const UserEquipment& user) {
    users.push_back(user);
}

std::vector<CellInfo> LTENetwork::get_cells() const {
    return cells;
}

CellInfo LTENetwork::get_cell_info(int cell_id) const {
    for (const auto& cell : cells) {
        if (cell.cell_id == cell_id) {
            return cell;
        }
    }
    return CellInfo{};
}

void LTENetwork::update_cell_load(int cell_id, int load_percentage) {
    for (auto& cell : cells) {
        if (cell.cell_id == cell_id) {
            cell.load_percentage = load_percentage;
            break;
        }
    }
}

void LTENetwork::update_cell_interference(int cell_id, double interference) {
    for (auto& cell : cells) {
        if (cell.cell_id == cell_id) {
            cell.interference_level = interference;
            break;
        }
    }
}

std::vector<UserEquipment> LTENetwork::get_users() const {
    return users;
}

UserEquipment LTENetwork::get_user_info(int ue_id) const {
    for (const auto& user : users) {
        if (user.ue_id == ue_id) {
            return user;
        }
    }
    return UserEquipment{};
}

void LTENetwork::update_user_position(int ue_id, double x, double y) {
    for (auto& user : users) {
        if (user.ue_id == ue_id) {
            user.x_position = x;
            user.y_position = y;
            
            // Check if handover is needed due to position change
            if (should_trigger_handover(ue_id)) {
                int new_cell = find_best_serving_cell(x, y);
                if (new_cell != user.serving_cell) {
                    initiate_handover(ue_id, new_cell);
                }
            }
            break;
        }
    }
}

void LTENetwork::update_user_state(int ue_id, LTEState state) {
    for (auto& user : users) {
        if (user.ue_id == ue_id) {
            user.state = state;
            break;
        }
    }
}

std::vector<ResourceBlock> LTENetwork::allocate_resource_blocks(int ue_id, int num_rbs) {
    std::vector<ResourceBlock> allocated_rbs;
    
    // Find the user's serving cell
    int serving_cell = -1;
    for (const auto& user : users) {
        if (user.ue_id == ue_id) {
            serving_cell = user.serving_cell;
            break;
        }
    }
    
    if (serving_cell == -1) return allocated_rbs;
    
    // Find available RBs in the serving cell
    int allocated_count = 0;
    for (auto& rb : resource_blocks) {
        if (rb.rb_id >= serving_cell * 100 && rb.rb_id < (serving_cell + 1) * 100) {
            if (!rb.allocated && allocated_count < num_rbs) {
                rb.allocated = true;
                rb.user_id = ue_id;
                rb.allocation_time = std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::steady_clock::now().time_since_epoch()).count();
                allocated_rbs.push_back(rb);
                allocated_count++;
            }
        }
    }
    
    // Update user's allocated RBs
    for (auto& user : users) {
        if (user.ue_id == ue_id) {
            user.allocated_rbs = allocated_rbs;
            break;
        }
    }
    
    return allocated_rbs;
}

void LTENetwork::deallocate_resource_blocks(int ue_id) {
    for (auto& rb : resource_blocks) {
        if (rb.user_id == ue_id) {
            rb.allocated = false;
            rb.user_id = -1;
            rb.allocation_time = 0;
        }
    }
    
    // Clear user's allocated RBs
    for (auto& user : users) {
        if (user.ue_id == ue_id) {
            user.allocated_rbs.clear();
            break;
        }
    }
}

double LTENetwork::calculate_user_throughput(int ue_id) {
    UserEquipment user = get_user_info(ue_id);
    if (user.allocated_rbs.empty()) return 0.0;
    
    // Calculate SINR for the user
    double sinr = calculate_sinr(ue_id, user.serving_cell);
    
    // Convert SINR to spectral efficiency (Shannon's formula, simplified)
    double spectral_efficiency = std::log2(1.0 + std::pow(10.0, sinr / 10.0));
    
    // Calculate throughput: spectral_efficiency * bandwidth * num_rbs
    double total_bandwidth = user.allocated_rbs.size() * 180.0; // kHz
    double throughput_kbps = spectral_efficiency * total_bandwidth;
    
    return throughput_kbps / 1000.0; // Convert to Mbps
}

bool LTENetwork::should_trigger_handover(int ue_id) {
    UserEquipment user = get_user_info(ue_id);
    double serving_rsrp = calculate_rsrp(ue_id, user.serving_cell);
    
    // Find neighbor cells and check if any has better signal
    std::vector<CellInfo> neighbors = get_neighbor_cells(ue_id);
    
    for (const auto& neighbor : neighbors) {
        double neighbor_rsrp = calculate_rsrp(ue_id, neighbor.cell_id);
        
        // Handover condition: neighbor_rsrp > serving_rsrp + margin + hysteresis
        if (neighbor_rsrp > serving_rsrp + handover_margin + handover_hysteresis) {
            return true;
        }
    }
    
    return false;
}

HandoverEvent LTENetwork::initiate_handover(int ue_id, int target_cell) {
    UserEquipment user = get_user_info(ue_id);
    
    HandoverEvent handover;
    handover.source_cell = user.serving_cell;
    handover.target_cell = target_cell;
    handover.type = HandoverType::INTRA_LTE;
    handover.trigger_rsrp = calculate_rsrp(ue_id, user.serving_cell);
    handover.target_rsrp = calculate_rsrp(ue_id, target_cell);
    handover.start_time = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
    handover.completion_time = 0;
    handover.success = false;
    
    // Update user state
    update_user_state(ue_id, LTEState::HANDOVER_PREPARATION);
    
    // Simulate handover execution
    execute_handover(ue_id, target_cell);
    
    return handover;
}

void LTENetwork::execute_handover(int ue_id, int target_cell) {
    // Update user state
    update_user_state(ue_id, LTEState::HANDOVER_EXECUTION);
    
    // Deallocate resources from source cell
    deallocate_resource_blocks(ue_id);
    
    // Update serving cell
    for (auto& user : users) {
        if (user.ue_id == ue_id) {
            user.serving_cell = target_cell;
            break;
        }
    }
    
    // Complete handover
    complete_handover(ue_id);
}

void LTENetwork::complete_handover(int ue_id) {
    update_user_state(ue_id, LTEState::CONNECTED);
    
    // Record successful handover
    if (!handover_history.empty()) {
        auto& last_handover = handover_history.back();
        if (last_handover.completion_time == 0) {  // If it's the most recent handover
            last_handover.completion_time = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now().time_since_epoch()).count();
            last_handover.success = true;
        }
    }
}

double LTENetwork::calculate_rsrp(int ue_id, int cell_id) {
    UserEquipment user = get_user_info(ue_id);
    CellInfo cell = get_cell_info(cell_id);
    
    // Calculate distance
    double distance = std::sqrt(std::pow(user.x_position - cell.longitude, 2) + 
                               std::pow(user.y_position - cell.latitude, 2));
    
    // Path loss model: PL = 128.1 + 37.6*log10(distance_km)
    double path_loss = 128.1 + 37.6 * std::log10(std::max(distance / 1000.0, 0.001));
    
    // RSRP = Tx_Power - Path_Loss + Antenna_Gain - Shadowing
    double tx_power = 46.0;  // dBm (typical for macro cell)
    double antenna_gain = 15.0; // dBi
    double shadowing = 0.0;  // Simplified - no shadowing
    
    double rsrp = tx_power - path_loss + antenna_gain - shadowing;
    
    return rsrp;
}

double LTENetwork::calculate_rsrq(int ue_id, int cell_id) {
    double rsrp = calculate_rsrp(ue_id, cell_id);
    
    // Calculate interference from other cells
    double total_interference = 0.0;
    for (const auto& cell : cells) {
        if (cell.cell_id != cell_id) {
            double interference_rsrp = calculate_rsrp(ue_id, cell.cell_id);
            total_interference += std::pow(10.0, interference_rsrp / 10.0);
        }
    }
    
    // RSRQ = RSRP / (RSSI), where RSSI includes signal + interference + noise
    double noise_power = -104.0; // dBm (thermal noise)
    double rssi = 10.0 * std::log10(std::pow(10.0, rsrp / 10.0) + total_interference + 
                                   std::pow(10.0, noise_power / 10.0));
    
    double rsrq = rsrp - rssi;
    
    return rsrq;
}

double LTENetwork::calculate_sinr(int ue_id, int cell_id) {
    double rsrp = calculate_rsrp(ue_id, cell_id);
    
    // Calculate interference + noise
    double total_interference_noise = 0.0;
    
    // Interference from other cells
    for (const auto& cell : cells) {
        if (cell.cell_id != cell_id) {
            double interference_rsrp = calculate_rsrp(ue_id, cell.cell_id);
            total_interference_noise += std::pow(10.0, interference_rsrp / 10.0);
        }
    }
    
    // Add thermal noise
    double noise_power = -104.0; // dBm
    total_interference_noise += std::pow(10.0, noise_power / 10.0);
    
    // SINR = Signal / (Interference + Noise)
    double sinr = rsrp - 10.0 * std::log10(total_interference_noise);
    
    return sinr;
}

std::vector<CellInfo> LTENetwork::get_neighbor_cells(int ue_id) {
    std::vector<CellInfo> neighbors;
    UserEquipment user = get_user_info(ue_id);
    
    // Find cells within a certain range (simplified neighbor detection)
    for (const auto& cell : cells) {
        if (cell.cell_id != user.serving_cell) {
            double distance = std::sqrt(std::pow(user.x_position - cell.longitude, 2) + 
                                       std::pow(user.y_position - cell.latitude, 2));
            
            if (distance < 3000.0) {  // 3km range
                neighbors.push_back(cell);
            }
        }
    }
    
    return neighbors;
}

void LTENetwork::set_scheduling_algorithm(const std::string& algorithm) {
    scheduling_algorithm = algorithm;
}

void LTENetwork::update_resource_allocation() {
    if (scheduling_algorithm == "Round Robin") {
        round_robin_scheduler();
    } else if (scheduling_algorithm == "Proportional Fair") {
        proportional_fair_scheduler();
    } else if (scheduling_algorithm == "Max C/I") {
        max_ci_scheduler();
    }
}

void LTENetwork::round_robin_scheduler() {
    // Simple round-robin allocation
    static int next_user_index = 0;
    
    for (auto& user : users) {
        if (user.state == LTEState::CONNECTED) {
            deallocate_resource_blocks(user.ue_id);
            allocate_resource_blocks(user.ue_id, 10); // Allocate 10 RBs per user
            user.current_throughput = calculate_user_throughput(user.ue_id);
        }
    }
}

void LTENetwork::proportional_fair_scheduler() {
    // Proportional fair scheduling based on channel quality and past throughput
    for (auto& user : users) {
        if (user.state == LTEState::CONNECTED) {
            double sinr = calculate_sinr(user.ue_id, user.serving_cell);
            double channel_rate = std::log2(1.0 + std::pow(10.0, sinr / 10.0));
            
            // Simplified proportional fair metric
            double metric = channel_rate / std::max(user.current_throughput, 0.1);
            
            // Allocate RBs based on metric (simplified)
            int num_rbs = static_cast<int>(std::min(metric * 5.0, 20.0));
            
            deallocate_resource_blocks(user.ue_id);
            allocate_resource_blocks(user.ue_id, num_rbs);
            user.current_throughput = calculate_user_throughput(user.ue_id);
        }
    }
}

void LTENetwork::max_ci_scheduler() {
    // Max C/I (Carrier to Interference) scheduling
    std::vector<std::pair<double, int>> user_metrics;
    
    for (const auto& user : users) {
        if (user.state == LTEState::CONNECTED) {
            double sinr = calculate_sinr(user.ue_id, user.serving_cell);
            user_metrics.push_back({sinr, user.ue_id});
        }
    }
    
    // Sort by SINR (descending)
    std::sort(user_metrics.begin(), user_metrics.end(), std::greater<std::pair<double, int>>());
    
    // Allocate more RBs to users with better channel conditions
    for (size_t i = 0; i < user_metrics.size(); i++) {
        int ue_id = user_metrics[i].second;
        int num_rbs = static_cast<int>(20 - i * 2);  // Decreasing allocation
        num_rbs = std::max(num_rbs, 2);  // Minimum 2 RBs
        
        deallocate_resource_blocks(ue_id);
        allocate_resource_blocks(ue_id, num_rbs);
        
        // Update user throughput
        for (auto& user : users) {
            if (user.ue_id == ue_id) {
                user.current_throughput = calculate_user_throughput(ue_id);
                break;
            }
        }
    }
}

// Additional methods for mobility, performance monitoring, etc.
void LTENetwork::enable_mobility(bool enable) {
    mobility_enabled = enable;
}

void LTENetwork::set_mobility_model(const std::string& model) {
    mobility_model = model;
}

void LTENetwork::update_user_mobility() {
    if (!mobility_enabled) return;
    
    if (mobility_model == "Random Walk") {
        simulate_random_walk_mobility();
    } else if (mobility_model == "Manhattan") {
        simulate_manhattan_mobility();
    } else if (mobility_model == "Highway") {
        simulate_highway_mobility();
    }
}

void LTENetwork::simulate_random_walk_mobility() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dir_change(-0.1, 0.1);
    
    for (auto& user : users) {
        // Update direction with some randomness
        user.direction += dir_change(gen);
        
        // Update position based on velocity and direction
        double time_step = 0.1; // seconds
        user.x_position += user.velocity * std::cos(user.direction) * time_step / 3.6; // Convert km/h to m/s
        user.y_position += user.velocity * std::sin(user.direction) * time_step / 3.6;
        
        // Keep within bounds
        user.x_position = std::max(0.0, std::min(user.x_position, 10000.0));
        user.y_position = std::max(0.0, std::min(user.y_position, 10000.0));
    }
}

void LTENetwork::simulate_manhattan_mobility() {
    // Simplified Manhattan mobility - users move along grid lines
    for (auto& user : users) {
        // Move in current direction for some time, then potentially turn 90 degrees
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_real_distribution<> turn_prob(0.0, 1.0);
        
        if (turn_prob(gen) < 0.05) {  // 5% chance to turn
            user.direction = std::round(user.direction / (M_PI/2)) * (M_PI/2); // Snap to 90-degree angles
        }
        
        double time_step = 0.1;
        user.x_position += user.velocity * std::cos(user.direction) * time_step / 3.6;
        user.y_position += user.velocity * std::sin(user.direction) * time_step / 3.6;
        
        user.x_position = std::max(0.0, std::min(user.x_position, 10000.0));
        user.y_position = std::max(0.0, std::min(user.y_position, 10000.0));
    }
}

void LTENetwork::simulate_highway_mobility() {
    // Highway mobility - users move at high speed in relatively straight lines
    for (auto& user : users) {
        // Higher velocity for highway scenario
        user.velocity = std::max(user.velocity, 60.0); // Minimum 60 km/h
        
        double time_step = 0.1;
        user.x_position += user.velocity * std::cos(user.direction) * time_step / 3.6;
        user.y_position += user.velocity * std::sin(user.direction) * time_step / 3.6;
        
        user.x_position = std::max(0.0, std::min(user.x_position, 10000.0));
        user.y_position = std::max(0.0, std::min(user.y_position, 10000.0));
    }
}

double LTENetwork::get_network_throughput() const {
    double total_throughput = 0.0;
    for (const auto& user : users) {
        total_throughput += user.current_throughput;
    }
    return total_throughput;
}

double LTENetwork::get_handover_success_rate() const {
    if (handover_history.empty()) return 1.0;
    
    int successful_handovers = 0;
    for (const auto& handover : handover_history) {
        if (handover.success) {
            successful_handovers++;
        }
    }
    
    return static_cast<double>(successful_handovers) / handover_history.size();
}

void LTENetwork::set_handover_parameters(double margin, double hysteresis, int time_to_trigger) {
    handover_margin = margin;
    handover_hysteresis = hysteresis;
    handover_time_to_trigger = time_to_trigger;
}

void LTENetwork::step_simulation() {
    // Update user mobility
    update_user_mobility();
    
    // Update resource allocation
    update_resource_allocation();
    
    // Check for handovers
    for (auto& user : users) {
        if (user.state == LTEState::CONNECTED && should_trigger_handover(user.ue_id)) {
            int new_cell = find_best_serving_cell(user.x_position, user.y_position);
            if (new_cell != user.serving_cell) {
                HandoverEvent handover = initiate_handover(user.ue_id, new_cell);
                handover_history.push_back(handover);
            }
        }
    }
    
    // Update performance metrics
    network_throughput_history.push_back(get_network_throughput());
    handover_success_rate_history.push_back(get_handover_success_rate());
    active_users_history.push_back(get_active_users_count());
}

int LTENetwork::get_active_users_count() const {
    int count = 0;
    for (const auto& user : users) {
        if (user.state == LTEState::CONNECTED) {
            count++;
        }
    }
    return count;
}

std::vector<HandoverEvent> LTENetwork::get_handover_history() const {
    return handover_history;
}

void LTENetwork::reset_network() {
    for (auto& user : users) {
        user.state = LTEState::IDLE;
        user.current_throughput = 0.0;
        user.allocated_rbs.clear();
    }
    
    for (auto& rb : resource_blocks) {
        rb.allocated = false;
        rb.user_id = -1;
        rb.allocation_time = 0;
    }
    
    handover_history.clear();
    network_throughput_history.clear();
    handover_success_rate_history.clear();
    active_users_history.clear();
} 