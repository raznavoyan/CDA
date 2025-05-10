#include "measurement_manager.hpp"
#include <iostream>


// Constructor to set IP and port of target instrument
Machine::Machine(const std::string& ip, int port) : ip_(ip), port_(port) {}

// Accessor for IP address
const std::string& Machine::ip() const {
    return ip_;
}

// Accessor for port
int Machine::port() const {
    return port_;
}

// Constructor that accepts a Machine object (defines connection target)
MeasurementManager::MeasurementManager(const Machine& machine) : machine_(machine) {}

// Repeat a command multiple times, calculate and return average of results
double MeasurementManager::measureAverage(const std::string& command, int repeats) {
    std::vector<double> values;
    for (int i = 0; i < repeats; ++i) {
        std::string response = sendCommand(command);
        try {
            double val = std::stod(response);
            values.push_back(val);
        } catch (...) {}
    }
    if (values.empty()) return 0.0;
    double sum = std::accumulate(values.begin(), values.end(), 0.0);
    double avg = sum / values.size();
    measurements_.emplace_back(command, std::to_string(avg));
    return avg;
}

// Repeat a command multiple times and return all results
std::vector<std::string> MeasurementManager::repeatMeasurement(const std::string& command, int times) {
    std::vector<std::string> results;
    for (int i = 0; i < times; ++i) {
        std::string res = sendCommand(command);
        results.push_back(res);
        measurements_.emplace_back(command, res);
    }
    return results;
}

// Save all command-response pairs to a binary file
void MeasurementManager::saveToBinaryFile(const std::string& filepath) {
    std::ofstream out(filepath, std::ios::binary);
    for (const auto& [cmd, res] : measurements_) {
        uint32_t cmd_len = cmd.size();
        uint32_t res_len = res.size();
        out.write(reinterpret_cast<const char*>(&cmd_len), sizeof(cmd_len));
        out.write(cmd.c_str(), cmd_len);
        out.write(reinterpret_cast<const char*>(&res_len), sizeof(res_len));
        out.write(res.c_str(), res_len);
    }
}

// Ask user for number of repetitions, then measure average
double MeasurementManager::askRepeatsAndAverage(const std::string& command) {
    int times;
    std::cout << "How many times to repeat '" << command << "' for average? ";
    std::cin >> times;
    return measureAverage(command, times);
}

// Ask user for number of repetitions, then collect individual measurements
std::vector<std::string> MeasurementManager::askRepeatsAndMeasure(const std::string& command) {
    int times;
    std::cout << "How many times to repeat '" << command << "'? ";
    std::cin >> times;
    return repeatMeasurement(command, times);
}

// Send SCPI command via socket, receive response
std::string MeasurementManager::sendCommand(const std::string& command) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) throw std::runtime_error("Socket creation failed");

    sockaddr_in serv_addr{};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(machine_.port());
    inet_pton(AF_INET, machine_.ip().c_str(), &serv_addr.sin_addr);

    if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
        throw std::runtime_error("Connect failed");

    send(sockfd, command.c_str(), command.length(), 0);
    send(sockfd, "\n", 1, 0);  // SCPI commands are newline-terminated
    char buffer[1024] = {0};
    recv(sockfd, buffer, sizeof(buffer), 0);
    close(sockfd);
    return std::string(buffer);
}

// Get basic ID, voltage, and current readings from instrument
std::tuple<std::string, double, double> MeasurementManager::getBasicMeasurement() {
    std::string name = sendCommand("*IDN?");
    std::string v_str = sendCommand(":MEAS:VOLT?");
    std::string c_str = sendCommand(":MEAS:CURR?");

    double voltage = 0.0, current = 0.0;
    try { voltage = std::stod(v_str); } catch (...) {}
    try { current = std::stod(c_str); } catch (...) {}

    return {name, voltage, current};
}

