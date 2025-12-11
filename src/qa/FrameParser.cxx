#include "qa/FrameParser.h"

#include <iomanip>
#include <iostream>

#include "rogue/interfaces/stream/Frame.h"
#include "rogue/interfaces/stream/FrameIterator.h"

namespace tracker {

std::vector<Sample> FrameParser::parse(
    std::shared_ptr<rogue::interfaces::stream::Frame> frame) {
  // Container for all samples
  std::vector<Sample> samples;

  //  Get an iterator to the data in the frame. The iterator is advanced by
  //  a bytes where the event header begins.
  auto it{frame->begin() + 1};

  //
  // Extract the event header
  //

  EventHeader event_header;
  rogue::interfaces::stream::fromFrame(it, 1, &event_header.subsystem_id);
  // If the Subsystem ID isn't equal to 4, then the frame contains non-tracker
  // data.
  if (event_header.subsystem_id != 4) return samples;

  rogue::interfaces::stream::fromFrame(it, 1, &event_header.contributor_id);
  rogue::interfaces::stream::fromFrame(it, 1, &event_header.burn_count);
  it = it + 4;
  rogue::interfaces::stream::fromFrame(it, 8, &event_header.timestamp);

  // std::cout << "Subsystem ID: " << unsigned(event_header.subsystem_id)
  //           << std::endl;
  // std::cout << "Contributor ID: " << unsigned(event_header.contributor_id)
  //           << std::endl;
  // std::cout << "Burn Count: " << unsigned(event_header.burn_count) <<
  // std::endl; std::cout << "Timestamp: " << unsigned(event_header.timestamp)
  // << std::endl;

  //
  // Event Count
  //

  uint32_t event_count;
  rogue::interfaces::stream::fromFrame(it, 10, &event_count);
  // std::cout << "Event count: " << unsigned(event_count) << std::endl;

  //
  // Calculate the total number of multisamples and process them
  //
  size_t total_multisamples = std::distance(it, frame->end()) / 10;
  samples.reserve(total_multisamples * 12);
  //  std::cout << "Total multisamples: " << total_multisamples << std::endl;
  Sample sample;
  for (int multisample{0}; multisample < total_multisamples - 1;
       ++multisample) {
    rogue::interfaces::stream::fromFrame(it, 2, &sample.samples[0]);
    rogue::interfaces::stream::fromFrame(it, 2, &sample.samples[1]);
    rogue::interfaces::stream::fromFrame(it, 2, &sample.samples[2]);
    rogue::interfaces::stream::fromFrame(it, 1, &sample.channel);
    uint8_t apv_hybrid_id;
    rogue::interfaces::stream::fromFrame(it, 1, &apv_hybrid_id);
    sample.apv_id = apv_hybrid_id & 0x07;
    sample.hybrid_id = (apv_hybrid_id >> 4) & 0x07;
    rogue::interfaces::stream::fromFrame(it, 1, &sample.feb_id);
    sample.feb_id &= 0x0F;
    --it;
    rogue::interfaces::stream::fromFrame(it, 2, &sample.apv_trigger);
    sample.apv_trigger = (sample.apv_trigger >> 4) & 0x3F;
    --it;
    uint8_t tail;
    rogue::interfaces::stream::fromFrame(it, 1, &tail);
    sample.read_error = (tail >> 2) & 1;
    sample.tail = (tail >> 3) & 1;
    sample.head = (tail >> 4) & 1;
    sample.filter = (tail >> 5) & 1;

    /*
    std::cout << "Hex value: 0x" << std::hex << std::uppercase << std::setw(2)
          << std::setfill('0') << (int)*it << std::endl;
    std::cout << "Channel: " << std::dec << unsigned(sample.channel)
          << std::endl;
    std::cout << "FEB: " << unsigned(sample.feb_id) << std::endl;
    std::cout << "Hybrid: " << unsigned(sample.hybrid_id) << std::endl;
    std::cout << "APV: " << unsigned(sample.apv_id) << std::endl;
    std::cout << "Trigger: " << unsigned(sample.apv_trigger) << std::endl;
    std::cout << "Read Error: " << unsigned(sample.read_error) << std::endl;
    std::cout << "Tail: " << unsigned(sample.tail) << std::endl;
    std::cout << "Head: " << unsigned(sample.head) << std::endl;
    std::cout << "Filter: " << unsigned(sample.filter) << std::endl;
    std::cout << "Sample 0: " << unsigned(sample.samples[0]) << std::endl;
    std::cout << "Sample 1: " << unsigned(sample.samples[1]) << std::endl;
    std::cout << "Sample 2: " << unsigned(sample.samples[2]) << std::endl;
    */

    samples.push_back(sample);
  }

  return samples;
}
}  // namespace tracker
