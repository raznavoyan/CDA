#include "data_manager.hpp"
#include <fstream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <filesystem>
#include <iostream> 

// Add one measurement to the internal data storage
void DataManager::addMeasurement(const std::string& idn, double voltage, double current) {
    std::stringstream ss(idn);
    std::string vendor, model, serial, firmware;

    // Parse instrument identification string
    if (!std::getline(ss, vendor, ',')) vendor = "N/A";
    if (!std::getline(ss, model, ',')) model = "N/A";
    if (!std::getline(ss, serial, ',')) serial = "N/A";
    if (!std::getline(ss, firmware)) firmware = "N/A";

    MeasurementData m;
    m.timestamp = getTimestamp();
    m.vendor = vendor;
    m.model = model;
    m.serial = serial;
    m.firmware = firmware;
    m.voltage = voltage;
    m.current = current;
    m.resistance = (current != 0.0) ? voltage / current : 0.0;  // Avoid division by zero
    data.push_back(m);
}

// Save collected measurements as CSV file
void DataManager::saveCSV(const std::string& full_path) const {
    std::filesystem::create_directories(std::filesystem::path(full_path).parent_path());

    std::ofstream out(full_path);
    if (!out) {
        std::cerr << "[!] Failed to open file for writing: " << full_path << std::endl;
        return;
    }

    // Write header row
    out << "Timestamp,Vendor,Model,Serial,Firmware,Voltage,Current,Resistance\n";
    for (const auto& m : data) {
        out << m.timestamp << "," << m.vendor << "," << m.model << "," << m.serial << "," << m.firmware
            << "," << m.voltage << "," << m.current << "," << m.resistance << "\n";
    }

    std::cout << "[✓] Saved to " << full_path << std::endl;
}

// Return const reference to stored data
const std::vector<MeasurementData>& DataManager::getData() const {
    return data;
}

// Get current system timestamp in human-readable format
std::string DataManager::getTimestamp() const {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

