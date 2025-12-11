#include "qa/CsvWriter.h"

#include <iostream>
#include <sstream>
#include <stdexcept>

#include "rogue/interfaces/stream/Frame.h"

namespace tracker {

// Destructor
CsvWriter::~CsvWriter() { close(); }

// Open the file
void CsvWriter::open(const std::string& file_path) {
  if (csv.is_open()) {
    throw std::runtime_error("File is already open.");
  }

  csv.open(file_path, std::ios::out);
  if (!csv.is_open()) {
    throw std::runtime_error("Failed to open file: " + file_path);
  }

  // Write the header out
  csv << "Event, APV trigger, FEB, Hybrid, APV, channel, pchannel, sample0, "
         "sample1, sample2, error, head, tail, filter\n";

  parser = std::make_unique<FrameParser>();
}

// Close the file
void CsvWriter::close() {
  if (csv.is_open()) {
    csv.close();
  }

  std::cout << "Total events processed " << event_count << std::endl;
}

void CsvWriter::acceptFrame(
    std::shared_ptr<rogue::interfaces::stream::Frame> frame) {
  // Skip the frame if an error is found.
  if (frame->getError()) return;

  if (frame->getChannel() != 0) return;

  // Extract the samples from the frame.
  std::vector<Sample> samples = parser->parse(frame);
  if (samples.empty()) return;

  event_count++;

  // Buffer to hold CSV rows before writing
  std::stringstream buffer;

  // To improve performance, pre-allocate space in the vector. This
  // assumes that each line will contain ~80 characters.
  buffer.str().reserve(samples.size() * 100);

  for (const auto& sample : samples) {
    auto apv_id{static_cast<int>(sample.apv_id)};
    auto channel{static_cast<int>(sample.channel)};
    int pchannel{(5 * 128 - 1) - (apv_id * 128 + (128 - 1) - channel)};

    buffer << event_count << ", " << static_cast<int>(sample.apv_trigger)
           << ", " << static_cast<int>(sample.feb_id) << ", "
           << static_cast<int>(sample.hybrid_id) << ", " << apv_id << ", "
           << channel << ", " << pchannel << ", "
           << static_cast<int>(sample.samples[0]) << ", "
           << static_cast<int>(sample.samples[1]) << ", "
           << static_cast<int>(sample.samples[2]) << ", "
           << static_cast<int>(sample.read_error) << ", "
           << static_cast<int>(sample.head) << ", "
           << static_cast<int>(sample.tail) << ", "
           << static_cast<int>(sample.filter) << std::endl;
  }

  // Write the entire buffer
  csv << buffer.str();
}
}  // namespace tracker
