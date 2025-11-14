
#include <cstdlib>
#include <iostream>

void display_usage();

// Constants
constexpr const char* APPLICATION_NAME = "tracker-qa";

int main(int argc, char** argv) {
  if (argc < 2) {
    return EXIT_FAILURE;
  }
}

void display_usage() {
  std::cout << "Usage: " << APPLICATION_NAME
            << " [application arguments] {configuration_script.py} [arguments "
               "to configuration script]\n";
}
