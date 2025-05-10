#ifndef __DATA__MENEGER__
#define __DATA__MENEGER__

#include <string>
#include <vector>

// Structure to store one measurement record
struct MeasurementData {
    std::string timestamp;
    std::string vendor;
    std::string model;
    std::string serial;
    std::string firmware;
    double voltage;
    double current;
    double resistance;
};

class DataManager {
public:
    // Add a new measurement
    void addMeasurement(const std::string& idn, double voltage, double current);

    // Save all measurements to a CSV file
    void saveCSV(const std::string& full_path) const;

    // Get access to stored measurement data
    const std::vector<MeasurementData>& getData() const;

private:
    // Get current timestamp in formatted string
    std::string getTimestamp() const;

    std::vector<MeasurementData> data;
};

#endif
