#ifndef __PYPLOTTER_HPP__
#define __PYPLOTTER_HPP__
#include <string>

// PyPlotter class manages real-time plotting using a Python subprocess
class PyPlotter {
public:
    PyPlotter();             // Constructor
    ~PyPlotter();            // Destructor (ensures subprocess cleanup)

    void startPython();      // Launch plotting subprocess
    void sendPoint(double); // Send new data point to the plot
    void stopPlotter();      // Stop and close the plotting subprocess

private:
    pid_t pid;               // Process ID of the child process
    int plotter_stdin;       // File descriptor to send data to plotter
    bool running;            // Track if the plotter is active
};

#endif

