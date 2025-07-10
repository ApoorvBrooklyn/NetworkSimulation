#include "lte_network.h"
#include <cmath>
#include <algorithm>
#include <random>

LTENetwork::LTENetwork() {
    handover_margin = 3.0;
    handover_hysteresis = 1.0;
    handover_time_to_trigger = 320;
    interference_threshold = 0.1;
    max_users_per_cell = 100;
    scheduling_algorithm = "Proportional Fair";
    mobility_enabled = false;
    mobility_speed_min = 5.0;
    mobility_speed_max = 120.0;
    mobility_model = "Random Walk";
}

void LTENetwork::initialize_network(int num_cells, int num_users) {
    cells.clear();
    users.clear();
    handover_history.clear();
    
    // Create cells
    for (int i = 0; i < num_cells; i++) {
        CellInfo cell;
        cell.cell_id = i;
        cell.signal_strength = -70.0;
        cell.signal_quality = -10.0;
        cell.interference_level = 0.05;
        cell.load_percentage = 0;
        cell.technology = "LTE";
        cell.latitude = (i / 3) * 1000.0;
        cell.longitude = (i % 3) * 1000.0;
        cells.push_back(cell);
    }
    
    // Create users
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> pos_dis(0.0, 3000.0);
    
    for (int i = 0; i < num_users; i++) {
        UserEquipment ue;
        ue.ue_id = i;
        ue.x_position = pos_dis(gen);
        ue.y_position = pos_dis(gen);
        ue.velocity = 30.0;
        ue.direction = 0.0;
        ue.serving_cell = 0;
        ue.state = LTEState::CONNECTED;
        ue.current_throughput = 1.0;
        ue.battery_level = 1.0;
        users.push_back(ue);
    }
}

double LTENetwork::calculate_rsrp(int ue_id, int cell_id) {
    UserEquipment user = get_user_info(ue_id);
    CellInfo cell = get_cell_info(cell_id);
    
    double distance = std::sqrt(std::pow(user.x_position - cell.longitude, 2) + 
                               std::pow(user.y_position - cell.latitude, 2));
    
    double path_loss = 128.1 + 37.6 * std::log10(std::max(distance / 1000.0, 0.001));
    double rsrp = 46.0 - path_loss + 15.0;
    
    return rsrp;
}

bool LTENetwork::should_trigger_handover(int ue_id) {
    UserEquipment user = get_user_info(ue_id);
    double serving_rsrp = calculate_rsrp(ue_id, user.serving_cell);
    
    for (const auto& cell : cells) {
        if (cell.cell_id != user.serving_cell) {
            double neighbor_rsrp = calculate_rsrp(ue_id, cell.cell_id);
            if (neighbor_rsrp > serving_rsrp + handover_margin + handover_hysteresis) {
                return true;
            }
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
    handover.success = true;
    handover.completion_time = handover.start_time + 100; // 100ms handover
    
    // Update user's serving cell
    for (auto& u : users) {
        if (u.ue_id == ue_id) {
            u.serving_cell = target_cell;
            u.state = LTEState::CONNECTED;
            break;
        }
    }
    
    return handover;
}

UserEquipment LTENetwork::get_user_info(int ue_id) const {
    for (const auto& user : users) {
        if (user.ue_id == ue_id) return user;
    }
    return UserEquipment{};
}

CellInfo LTENetwork::get_cell_info(int cell_id) const {
    for (const auto& cell : cells) {
        if (cell.cell_id == cell_id) return cell;
    }
    return CellInfo{};
}

void LTENetwork::update_user_position(int ue_id, double x, double y) {
    for (auto& user : users) {
        if (user.ue_id == ue_id) {
            user.x_position = x;
            user.y_position = y;
            break;
        }
    }
}

void LTENetwork::set_handover_parameters(double margin, double hysteresis, int time_to_trigger) {
    handover_margin = margin;
    handover_hysteresis = hysteresis;
    handover_time_to_trigger = time_to_trigger;
}

double LTENetwork::get_network_throughput() const {
    double total = 0.0;
    for (const auto& user : users) {
        total += user.current_throughput;
    }
    return total;
}

int LTENetwork::get_active_users_count() const {
    int count = 0;
    for (const auto& user : users) {
        if (user.state == LTEState::CONNECTED) count++;
    }
    return count;
}

std::vector<HandoverEvent> LTENetwork::get_handover_history() const {
    return handover_history;
}

void LTENetwork::step_simulation() {
    // Simplified simulation step
    for (auto& user : users) {
        if (should_trigger_handover(user.ue_id)) {
            // Find best cell
            int best_cell = 0;
            double best_rsrp = -200.0;
            for (const auto& cell : cells) {
                double rsrp = calculate_rsrp(user.ue_id, cell.cell_id);
                if (rsrp > best_rsrp) {
                    best_rsrp = rsrp;
                    best_cell = cell.cell_id;
                }
            }
            
            if (best_cell != user.serving_cell) {
                HandoverEvent handover = initiate_handover(user.ue_id, best_cell);
                handover_history.push_back(handover);
            }
        }
    }
} 