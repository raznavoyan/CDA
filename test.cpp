#include "machineKeysid.hpp"
#include <ncurses.h>
#include <cstdlib>
#include <string>
#include <filesystem>

int main() {
    // Initialize ncurses-based user interface
    Interface iface;
    iface.Run();

    // Cleanly terminate ncurses UI after parameter input
    endwin();

    // Retrieve user-selected measurement type and parameters
    auto [type, params] = iface.getMeasurementParams();

    // Create connection to measurement instrument (localhost for test)
    Machine machine("127.0.0.1", 5025);  // Replace with actual IP for real usage
    MeasurementManager meas(machine);
    DataManager data;
    PyPlotter plot;

    // Identify instrument via SCPI *IDN? command
    std::string idn = std::get<0>(meas.getBasicMeasurement());

    // Parse user input parameters
    double vstart = std::stod(params["Vstart"]);
    double vend = std::stod(params["Vend"]);
    int points = std::stoi(params["Points number"]);
    bool doPlot = (params["Plot (y/n)"] == "y" || params["Plot (y/n)"] == "Y");
    bool doSave = (params["Save data table (y/n)"] == "y" || params["Save data table (y/n)"] == "Y");

    // Start Python-based live plotter if selected
    if (doPlot) plot.startPython();

    // Main measurement loop
    for (int i = 0; i < points; ++i) {
        double v = vstart + i * (vend - vstart) / (points - 1);  // Linearly spaced voltages

        // Set voltage and enable output on the instrument
        meas.sendCommand(":SOUR:VOLT " + std::to_string(v));
        meas.sendCommand(":OUTP ON");
        std::this_thread::sleep_for(std::chrono::milliseconds(200));  // Wait for stability

        // Measure voltage and current, calculate resistance, and store
        auto [_, volt, curr] = meas.getBasicMeasurement();
        data.addMeasurement(idn, volt, curr);

        // Send current to plotter in real time
        if (doPlot) plot.sendPoint(curr);
    }

    // Terminate plotting process
    if (doPlot) plot.stopPlotter();

    // Prepare save directories
    const char* home = std::getenv("HOME");
    std::string saveDataDir = std::string(home) + "/Desktop/data";
    std::string savePlotDir = std::string(home) + "/Desktop/plots";
    std::filesystem::create_directories(saveDataDir);
    std::filesystem::create_directories(savePlotDir);

    // Define output file paths
    std::string dataFile = saveDataDir + "/measurement.csv";
    std::string plotFile = savePlotDir + "/plot.png";

    // Save measurements to CSV if requested
    if (doSave) {
        data.saveCSV(dataFile);
    }

    // Show final ncurses-based result screen
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);

    clear();
    mvprintw(2, 4, "✓ Measurement completed successfully.");
    mvprintw(4, 4, "Saved to:");
    mvprintw(5, 6, "• %s", plotFile.c_str());
    mvprintw(6, 6, "• %s", dataFile.c_str());
    mvprintw(8, 4, "Press any key to exit...");
    refresh();
    getch();

    // Cleanup ncurses
    endwin();
    return 0;
}

