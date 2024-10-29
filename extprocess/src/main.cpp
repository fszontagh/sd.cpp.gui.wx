#include <iostream>
#include <string>
#ifdef _WIN32
#include <windows.h>
#else
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#endif
#include <csignal>


#include "libs/SharedLibrary.h"

void signal_handler(int signum) {
    switch (signum) {
        case SIGINT:
            std::cout << "SIGINT received, exiting.\n";
            exit(0);
            break;
        case SIGTERM:
            std::cout << "SIGTERM received, exiting.\n";
            exit(0);
            break;
        default:
            std::cout << "Unknown signal " << signum << " received.\n";
            break;
    }
}
int main(int argc, char* argv[]) {
    // install signal handler
    struct sigaction action;
    action.sa_handler = signal_handler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    sigaction(SIGINT, &action, NULL);
    sigaction(SIGTERM, &action, NULL);

    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <shared_library>" << std::endl;
        return 1;
    }

    
    SharedLibrary sharedLibrary(argv[1]);

    try {
        sharedLibrary.load();
    } catch (std::runtime_error& e) {
        std::cerr << "Failed to load shared library: " << e.what() << std::endl;
        return 1;
    }

    std::string message;

    while (true) {
        std::getline(std::cin, message);  // Üzenet fogadása
        if (message == "exit") {
            break;  // Ha "exit" üzenetet kap, kilép
        }

        
    }
    sharedLibrary.unload();
    return 0;
}
