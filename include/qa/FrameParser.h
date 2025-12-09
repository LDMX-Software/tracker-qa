#pragma once

#include <cstdint>
#include <memory>
#include <vector>

// Forward declarations
namespace rogue::interfaces::stream {
class Frame;
}  // namespace rogue::interfaces::stream

namespace tracker {

struct EventHeader {
  uint8_t subsystem_id{99};
  uint8_t contributor_id{99};
  uint8_t burn_count{99};
  uint64_t timestamp{999999};
};

struct Sample {
  uint8_t channel;
  uint8_t apv_id;
  uint8_t hybrid_id;
  uint8_t feb_id;
  uint8_t read_error;
  uint8_t tail;
  uint8_t head;
  uint8_t filter;
  uint16_t apv_trigger;
  std::vector<uint16_t> samples{0, 0, 0};
};

class FrameParser {
 public:
  /// Constructor
  FrameParser() = default;

  /// Destructor
  ~FrameParser() = default;

  /**
   */
  std::vector<Sample> parse(
      std::shared_ptr<rogue::interfaces::stream::Frame> frame);
};
}  // namespace tracker
