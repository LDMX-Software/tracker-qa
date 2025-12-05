#include "qa/CsvWriter.h"

#include <stdint.h>

#include <iostream>
#include <stdexcept>

#include "rogue/interfaces/stream/Frame.h"

namespace tracker {

// Destructor
CsvWriter::~CsvWriter() {
  if (csv.is_open()) {
    close();
  }
}

// Open the file
void CsvWriter::open(const std::string& file_path) {
  if (csv.is_open()) {
    throw std::runtime_error("File is already open.");
  }

  csv.open(file_path, std::ios::out);
  if (!csv.is_open()) {
    throw std::runtime_error("Failed to open file: " + file_path);
  }
  csv << "ROR_trigger,FEB,Hybrid,APV,channel,pchannel,sample0,sample1,sample2,"
         "error,head,tail,filter";

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
  // if (frame->getError()) return;

  // std::cout << "Event number: " << int(event_count) << std::endl;
  //  frame->getNumpy(0, frame->getPayload());

  std::vector<Sample> samples = parser->parse(frame);
  if (samples.empty()) return;

  for (auto& sample : samples) {
    int pchannel = (5 * 128 - 1) -
                   (int(sample.apv_id) * 128 + (128 - 1) - int(sample.channel));
    csv << int(sample.ror_trigger) << "," << int(sample.feb_id) << ","
        << int(sample.hybrid_id) << "," << int(sample.apv_id) << ","
        << int(sample.channel) << "," << pchannel << ","
        << int(sample.samples[0]) << "," << int(sample.samples[1]) << ","
        << int(sample.samples[2]) << "," << int(sample.read_error) << ","
        << int(sample.head) << "," << int(sample.tail) << ","
        << int(sample.filter) << std::endl;
  }
  // event_count++;
}
}  // namespace tracker
