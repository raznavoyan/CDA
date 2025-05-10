#include "PyPlotter.hpp"
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdlib.h>

// Constructor initializes the plotter state
PyPlotter::PyPlotter() : running(false) {}

// Start Python-based plotting script as a child process
void PyPlotter::startPython() {
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        std::cerr << "Pipe creation failed\n";
        exit(1);
    }

    pid = fork();
    if (pid == -1) {
        std::cerr << "Fork failed\n";
        exit(1);
    }

    if (pid == 0) {
        // Child process
        const char* home = getenv("HOME");
        std::string script = std::string(home) + "/.local/share/keysight/plotter.py";

        // Redirect stdin to read end of the pipe
        dup2(pipefd[0], STDIN_FILENO);
        close(pipefd[0]);
        close(pipefd[1]);

        // Start the Python script using a patched path
        execl("/home/razmik/xraylarch/bin/python3", "python3", script.c_str(), (char*)NULL);
        std::cerr << "Exec failed\n";
        exit(1);
    } else {
        // Parent process
        close(pipefd[0]);
        plotter_stdin = pipefd[1];
        running = true;
    }
}

// Send a single numeric data point to the Python plotter
void PyPlotter::sendPoint(double value) {
    if (!running) return;
    std::string data = std::to_string(value) + "\n";
    (void)write(plotter_stdin, data.c_str(), data.size());
    fsync(plotter_stdin);
}

// Stop the plotting process cleanly
void PyPlotter::stopPlotter() {
    if (!running) return;
    (void)write(plotter_stdin, "STOP\n", 5);
    close(plotter_stdin);
    waitpid(pid, NULL, 0);
    running = false;
}

// Destructor ensures plotter process is terminated
PyPlotter::~PyPlotter() {
    if (running) stopPlotter();
}

