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
  buffer.str().reserve(samples.size() * 80);

  for (const auto& sample : samples) {
    int pchannel{(5 * 128 - 1) -
                 (int(sample.apv_id) * 128 + (128 - 1) - int(sample.channel))};
    buffer << event_count << ", " << int(sample.apv_trigger) << ", "
           << int(sample.feb_id) << ", " << int(sample.hybrid_id) << ", "
           << int(sample.apv_id) << ", " << int(sample.channel) << ", "
           << pchannel << ", " << sample.samples[0] << ", " << sample.samples[1]
           << ", " << sample.samples[2] << ", " << int(sample.read_error)
           << ", " << int(sample.head) << ", " << int(sample.tail) << ", "
           << int(sample.filter) << std::endl;
  }

  // Write the entire buffer
  csv << buffer.str();
}
}  // namespace tracker
