/**********************************************
 * Self-Driving Car Nano-degree - Udacity
 *  Created on: September 20, 2020
 *      Author: Munir Jojo-Verge
 **********************************************/

/**
 * @file behavior_planner_FSM.cpp
 **/

#include "behavior_planner_FSM.h"

State BehaviorPlannerFSM::get_closest_waypoint_goal(
    const State& ego_state, const SharedPtr<cc::Map>& map,
    const float& lookahead_distance, bool& is_goal_junction) {
  // Nearest waypoint on the center of a Driving Lane.
  auto waypoint_0 = map->GetWaypoint(ego_state.location);

  if (_active_maneuver == DECEL_TO_STOP || _active_maneuver == STOPPED) {
    State waypoint;
    auto wp_transform = waypoint_0->GetTransform();
    waypoint.location = wp_transform.location;
    waypoint.rotation.yaw = utils::deg2rad(wp_transform.rotation.yaw);
    waypoint.rotation.pitch = utils::deg2rad(wp_transform.rotation.pitch);
    waypoint.rotation.roll = utils::deg2rad(wp_transform.rotation.roll);
    return waypoint;
  }

  // Waypoints at a lookahead distance
  // NOTE: "GetNext(d)" creates a list of waypoints at an approximate distance
  // "d" in the direction of the lane. The list contains one waypoint for each
  // deviation possible.

  // NOTE 2: GetNextUntilLaneEnd(d) returns a list of waypoints a distance "d"
  // apart. The list goes from the current waypoint to the end of its
  // lane.

  auto lookahead_waypoints = waypoint_0->GetNext(lookahead_distance);
  auto n_wp = lookahead_waypoints.size();
  if (n_wp == 0) {
    LOG(INFO) << "Goal wp is a nullptr";
    State waypoint;
    return waypoint;
  }
  LOG(INFO) << "BP - Num of Lookahead waypoints: " << n_wp;

  waypoint_0 = lookahead_waypoints[lookahead_waypoints.size() - 1];

  is_goal_junction = waypoint_0->IsJunction();
  LOG(INFO) << "BP - Is Last wp in junction? (0/1): " << is_goal_junction;
  auto cur_junction_id = waypoint_0->GetJunctionId();
  if (is_goal_junction) {
    if (cur_junction_id == _prev_junction_id) {
      LOG(INFO) << "BP - Last wp is in same junction as ego. Junction ID: "
      //          << _prev_junction_id;
      is_goal_junction = false;
    } else {
      LOG(INFO) << "BP - Last wp is in different junction than ego. Junction
      // ID: "
      //          << cur_junction_id;
      _prev_junction_id = cur_junction_id;
    }
  }
  State waypoint;
  auto wp_transform = waypoint_0->GetTransform();
  waypoint.location = wp_transform.location;
  waypoint.rotation.yaw = utils::deg2rad(wp_transform.rotation.yaw);
  waypoint.rotation.pitch = utils::deg2rad(wp_transform.rotation.pitch);
  waypoint.rotation.roll = utils::deg2rad(wp_transform.rotation.roll);
  return waypoint;
}

double BehaviorPlannerFSM::get_look_ahead_distance(const State& ego_state) {
  auto velocity_mag = utils::magnitude(ego_state.velocity);
  auto accel_mag = utils::magnitude(ego_state.acceleration);

  // DONE-Lookahead: One way to find a reasonable lookahead distance is to find
  // the distance you will need to come to a stop while traveling at speed V and
  // using a comfortable deceleration.
  auto look_ahead_distance = velocity_mag + pow(accel_mag, 2);   

  LOG(INFO) << "Calculated look_ahead_distance: " << look_ahead_distance;

  look_ahead_distance =
      std::min(std::max(look_ahead_distance, _lookahead_distance_min),
               _lookahead_distance_max);

  LOG(INFO) << "Final look_ahead_distance: " << look_ahead_distance;

  return look_ahead_distance;
}

State BehaviorPlannerFSM::get_goal(const State& ego_state,
                                   SharedPtr<cc::Map> map) {
  // Get look-ahead distance based on Ego speed

  auto look_ahead_distance = get_look_ahead_distance(ego_state);

  // Nearest waypoint on the center of a Driving Lane.
  bool is_goal_in_junction{false};
  auto goal_wp = get_closest_waypoint_goal(ego_state, map, look_ahead_distance,
                                           is_goal_in_junction);

  LOG(INFO) << "Is the FINAL goal on a junction: " << is_goal_in_junction;
  string tl_state = "none";
  State goal =
      state_transition(ego_state, goal_wp, is_goal_in_junction, tl_state);

  return goal;
}

State BehaviorPlannerFSM::state_transition(const State& ego_state, State goal,
                                           bool& is_goal_in_junction,
                                           string tl_state) {
  // Check with the Behavior Planner to see what we are going to do and
  // where our next goal is
  goal.acceleration.x = 0;
  goal.acceleration.y = 0;
  goal.acceleration.z = 0;

  if (_active_maneuver == FOLLOW_LANE) {
    LOG(INFO) << "BP- IN FOLLOW_LANE STATE";
    if (is_goal_in_junction) {
      LOG(INFO) << "BP - goal in junction";

      _active_maneuver = DECEL_TO_STOP;
      LOG(INFO) << "BP - changing to DECEL_TO_STOP";

      // Let's backup a "buffer" distance behind the "STOP" point
      LOG(INFO) << "BP- original STOP goal at: " << goal.location.x << ", " << goal.location.y;

      // DONE: goal behind the stopping point
      auto ang = goal.rotation.yaw + M_PI;
      goal.location.x += _stop_line_buffer * std::cos(ang);  
      goal.location.y += _stop_line_buffer * std::sin(ang);  

      LOG(INFO) << "BP- new STOP goal at: " << goal.location.x << ", " << goal.location.y;

      // DONE: goal speed at stopping point
      goal.velocity.x = 0.0;  
      goal.velocity.y = 0.0;  
      goal.velocity.z = 0.0;  

    } else {
      // DONE: goal speed in nominal state
      goal.velocity.x = _speed_limit * std::cos(goal.rotation.yaw);  
      goal.velocity.y = _speed_limit * std::sin(goal.rotation.yaw);  
      goal.velocity.z = 0;
    }

  } else if (_active_maneuver == DECEL_TO_STOP) {
    LOG(INFO) << "BP- IN DECEL_TO_STOP STATE";
    // DONE: maintain the same goal when in DECEL_TO_STOP state
       goal = _goal;  

    // DONE: It turns out that when we teleport, the car is always at speed
    // zero. In this the case, as soon as we enter the DECEL_TO_STOP state,
    // the condition that we are <= _stop_threshold_speed is ALWAYS true and we
    // move straight to "STOPPED" state. 
    // Make sure the distance to the stopping point is <= P_STOP_THRESHOLD_DISTANCE
    auto distance_to_stop_sign =
        utils::magnitude(goal.location - ego_state.location);
    LOG(INFO) << "Ego distance to stop line: " << distance_to_stop_sign;

    // DONE: use distance rather than speed
    if (distance_to_stop_sign <= P_STOP_THRESHOLD_DISTANCE) {
      // DONE: move to STOPPED state
      _active_maneuver = STOPPED;  
      _start_stop_time = std::chrono::high_resolution_clock::now();
      LOG(INFO) << "BP - changing to STOPPED";
    }
  } else if (_active_maneuver == STOPPED) {
    LOG(INFO) << "BP- IN STOPPED STATE";
    // DONE-maintain the same goal when in STOPPED state (we keep the goal at the stop line).
       goal = _goal;  // Keep previous goal. Stay where you are. 

    long long stopped_secs =
        std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::high_resolution_clock::now() - _start_stop_time)
            .count();
    LOG(INFO) << "BP- Stopped for " << stopped_secs << " secs";

    if (stopped_secs >= _req_stop_time && tl_state.compare("Red") != 0) {
      // DONE: move to FOLLOW_LANE state
      _active_maneuver = FOLLOW_LANE;  
      LOG(INFO) << "BP - changing to FOLLOW_LANE";
    }
  }
  _goal = goal;
  return goal;
}
