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

  parser = std::make_unique<FrameParser>();
}

// Close the file
void CsvWriter::close() {
  if (csv.is_open()) {
    csv.close();
  }
}

void CsvWriter::acceptFrame(
    std::shared_ptr<rogue::interfaces::stream::Frame> frame) {
  // Skip the frame if an error is found.
  if (frame->getError()) return;

  // Extract the samples from the frame.
  std::vector<Sample> samples = parser->parse(frame);
  if (samples.empty()) return;

  // Due to varying readout modes (e.g. 3-sample, 27-sample), the CSV header
  // cannot be written when the file is first opened. The readout mode can be
  // determined by checking the size of the samples vector after processing of
  // the first frame. Once the readout mode is identified, the appropriate CSV
  // header wil be generated and written once.

  // This checks if the file is empty
  if (csv.tellp() == 0) {
    csv << "ROR_trigger, FEB, Hybrid, APV, channel, pchannel, ";
    for (int sample_count{0}; sample_count < samples[0].samples.size();
         ++sample_count) {
      csv << ("sample" + std::to_string(sample_count) + ", ");
    }
    csv << "error, head, tail, filter\n";
  }

  // Buffer to hold CSV rows before writing
  std::stringstream buffer;

  // To improve performance, pre-allocate space in the vector. This
  // assumes that each line will contain ~80 characters.
  buffer.str().reserve(samples.size() * 80);

  for (const auto& sample : samples) {
    int pchannel{(5 * 128 - 1) -
                 (int(sample.apv_id) * 128 + (128 - 1) - int(sample.channel))};
    buffer << int(sample.ror_trigger) << ", " << int(sample.feb_id) << ", "
           << int(sample.hybrid_id) << ", " << int(sample.apv_id) << ", "
           << int(sample.channel) << ", " << pchannel << ", ";
    for (int sample_count{0}; sample_count < samples[0].samples.size();
         ++sample_count) {
      buffer << int(sample.samples[sample_count]) << ", ";
    }
    buffer << int(sample.read_error) << ", " << int(sample.head) << ", "
           << int(sample.tail) << ", " << int(sample.filter) << std::endl;
  }

  // Write the entire buffer
  csv << buffer.str();
}
}  // namespace tracker
