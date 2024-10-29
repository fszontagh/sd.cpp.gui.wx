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

    while (true) {
#ifdef _WIN32
        Sleep(1000);
#else
        sleep(1);
#endif
    }

    return 0;
}
