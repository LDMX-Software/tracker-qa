#pragma once

#include <fstream>
#include <memory>
#include <string>

#include "qa/FrameParser.h"
#include "rogue/interfaces/stream/Slave.h"

namespace rogue::interfaces::stream {
class Frame;
}  // namespace rogue::interfaces::stream

namespace tracker {

/**
 * @brief A class for writing tracker data to a CSV file.
 */
class CsvWriter : public rogue::interfaces::stream::Slave {
 public:
  /**
   * @brief Default constructor
   */
  CsvWriter() = default;

  /**
   * @brief Destructor
   *
   * Ensures that if the file is open, it will be closed properly when the
   * object is destroyed.
   */
  ~CsvWriter();

  /**
   * @brief Disable copy constructor.
   *
   * Prevents copying of the CsvWriter object to avoid accidental duplication
   * of the file stream.
   */
  CsvWriter(const CsvWriter&) = delete;

  /**
   * @brief Disable copy assigment operator.
   *
   * Prevents assignment of CsVWriter objects to avoid accidental duplication
   * of the file stream.
   */
  CsvWriter& operator=(const CsvWriter&) = delete;

  /**
   * @brief Opens the CSV file for writing.
   *
   * This method opens the CSV file at the specified path for writing.
   * If the file cannot be opened, an exception is thrown.
   *
   * @param file_path The path to the CSV file to open.
   * @throws std::runtime_error If the file cannot be opened or is already
   *   open.
   */
  void open(const std::string& file_path);

  /**
   * @brief Close the CSV file.
   *
   * This method ensures that the CSV file is closed properly. It is called
   * automatically in the destructor if the file is open.
   */
  void close();

  /**
   * @brief Method called when a frame is received.
   *
   * This method is called once for every frame streamed from a file. Prior
   * to extracting the raw data for writing, the frame undergoes integrity
   * checks. If the integrity checks fail, a warning is issued and parsing
   * of the frame is skipped. If the raw data is extracted successfully, it
   * persisted as a row in the CSV file.
   *
   * @param frame A shared pointer to the frame to write to the CSV file.
   */
  void acceptFrame(std::shared_ptr<rogue::interfaces::stream::Frame> frame);

 private:
  // Parser used to extract the raw data
  std::unique_ptr<FrameParser> parser;

  // Output file stream used to write to a CSV file
  std::ofstream csv;

  int event_count{0};
};
}  // namespace tracker
