#include <string>
#include <vector>
#include <tuple>
#include <utility>
#include <numeric>
#include <stdexcept>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>
#include <fstream>

// Class representing a remote measurement device
class Machine {
public:
    Machine(const std::string& ip, int port);
    const std::string& ip() const;
    int port() const;

private:
    std::string ip_;
    int port_;
};

// Class to handle communication and data collection from Machine
class MeasurementManager {
public:
    MeasurementManager(const Machine& machine);

    // Measure a value multiple times and return the average
    double measureAverage(const std::string& command, int repeats);

    // Measure a value multiple times and return all results
    std::vector<std::string> repeatMeasurement(const std::string& command, int times);

    // Prompt user for repeat count and return average
    double askRepeatsAndAverage(const std::string& command);

    // Prompt user for repeat count and return all responses
    std::vector<std::string> askRepeatsAndMeasure(const std::string& command);

    // Send a command and return the response
    std::string sendCommand(const std::string& command);

    // Perform basic voltage/current measurement and return ID
    std::tuple<std::string, double, double> getBasicMeasurement();

    // Save collected command-response pairs to binary file
    void saveToBinaryFile(const std::string& filepath);

private:
    Machine machine_;
    std::vector<std::pair<std::string, std::string>> measurements_;
};

