
#include <cstdlib>
#include <exception>
#include <iostream>
#include <memory>

#include "rogue/GeneralError.h"
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

  // Open the file for reading
  try {
    // Instantiate the reader used to stream frames from a file created by
    // Rogue
    auto reader = std::make_unique<rogue::utilities::fileio::StreamReader>();

    // Open the file for reading
    reader->open(file_name);

    std::cout << "Processing the file " << file_name << std::endl;

  } catch (const rogue::GeneralError& ex) {
    std::cerr << ex.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

void display_usage() {
  std::cout << "Usage: " << APPLICATION_NAME << " {raw_data_file}\n";
}
