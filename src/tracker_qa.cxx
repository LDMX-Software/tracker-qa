#include <cstdlib>
#include <exception>
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>

#include "qa/CsvWriter.h"
#include "rogue/GeneralError.h"
#include "rogue/Helpers.h"
#include "rogue/utilities/fileio/StreamReader.h"

void display_usage();

// Constants
constexpr const char* APPLICATION_NAME = "tracker-qa";

int main(int argc, char** argv) {
  // Check if the correct number of arguments were passed
  if (argc != 2) {
    display_usage();
    return EXIT_FAILURE;
  }

  // Get the file anme from the command line argument
  const char* file_name = argv[1];

  // Get the file name without extension and create the output file path
  std::filesystem::path input_file_path(file_name);
  std::filesystem::path output_file_path{
      input_file_path.replace_extension(".csv")};

  // Open the file for reading
  try {
    // Instantiate the reader used to stream frames from a file created by
    // Rogue
    auto reader = std::make_unique<rogue::utilities::fileio::StreamReader>();

    // Instantiate the CSV writer used to process the frames
    // Note: These should be dymaically loaded.
    auto parser = std::make_shared<tracker::CsvWriter>();

    // Connect the two streams
    rogueStreamConnect(reader, parser);

    // Open the CSV file to write the data to
    std::cout << "Writing output to " << output_file_path << std::endl;
    parser->open(output_file_path.string());

    // Open the file for reading
    std::cout << "Processing the file " << file_name << std::endl;
    reader->open(file_name);

    reader->closeWait();

  } catch (const rogue::GeneralError& ex) {
    std::cerr << ex.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

void display_usage() {
  std::cout << "Usage: " << APPLICATION_NAME << " {raw_data_file}\n";
}
