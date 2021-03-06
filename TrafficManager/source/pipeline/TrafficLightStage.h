#pragma once

#include <chrono>
#include <memory>
#include <mutex>
#include <unordered_map>

#include "carla/client/Vehicle.h"
#include "carla/client/TrafficLight.h"
#include "carla/Memory.h"
#include "carla/rpc/TrafficLightState.h"

#include "MessengerAndDataTypes.h"
#include "PipelineStage.h"

namespace traffic_manager {

namespace chr = std::chrono;
namespace cc = carla::client;
namespace cg = carla::geom;

  using ActorId = carla::ActorId;
  using Actor = carla::SharedPtr<cc::Actor>;
  using JunctionID = carla::road::JuncId;
  using SimpleWaypointPtr = std::shared_ptr<SimpleWaypoint>;
  using TrafficLight = carla::SharedPtr<cc::TrafficLight>;
  using TLS = carla::rpc::TrafficLightState;
  using TimeInstance = chr::time_point<chr::_V2::system_clock, chr::nanoseconds>;

  /// This class provides the information about the Traffic Lights at the
  /// junctions.
  class TrafficLightStage : public PipelineStage {

  private:

    cc::DebugHelper &debug_helper;
    /// Variables to remember messenger states.
    int localization_messenger_state;
    int planner_messenger_state;
    /// Selection key to switch between output frames.
    bool frame_selector;
    /// Pointer data frame received from localization stage.
    std::shared_ptr<LocalizationToTrafficLightFrame> localization_frame;
    /// Pointers to data frames to be shared with motion planner stage.
    std::shared_ptr<TrafficLightToPlannerFrame> planner_frame_a;
    std::shared_ptr<TrafficLightToPlannerFrame> planner_frame_b;
    /// Pointers to messenger objects.
    std::shared_ptr<LocalizationToTrafficLightMessenger> localization_messenger;
    std::shared_ptr<TrafficLightToPlannerMessenger> planner_messenger;
    /// Map containing the time ticket issued for vehicles.
    std::unordered_map<ActorId, TimeInstance> vehicle_last_ticket;
    /// Map containing the previous time ticket issued for junctions.
    std::unordered_map<JunctionID, TimeInstance> junction_last_ticket;
    /// Map containing the previous junction visited by a vehicle.
    std::unordered_map<ActorId, JunctionID> vehicle_last_junction;
    /// No signal negotiation mutex.
    std::mutex no_signal_negotiation_mutex;

  public:

    TrafficLightStage(
        std::shared_ptr<LocalizationToTrafficLightMessenger> localization_messenger,
        std::shared_ptr<TrafficLightToPlannerMessenger> planner_messenger,
        uint number_of_vehicle,
        uint pool_size,
        cc::DebugHelper &debug_helper);
    ~TrafficLightStage();

    void DataReceiver() override;

    void Action(const uint start_index, const uint end_index) override;

    void DataSender() override;

  };

}
