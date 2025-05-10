#pragma once
#include <string>
#include <vector>
#include <map>

// Interface class handles user interaction using ncurses-based menu and parameter editing
class Interface {
public:
    Interface();   // Constructor sets up measurement types and their parameter fields
    ~Interface();  // Destructor (nothing to clean up)

    void Run();
    // Returns selected measurement type and its configured parameters
    std::pair<std::string, std::map<std::string, std::string>> getMeasurementParams() const;

private:
    // Mapping of measurement type to list of required parameters
    std::map<std::string, std::vector<std::string>> parameters;

    // User-selected measurement type
    std::string selectedType;

    // User-filled parameter values (field -> value)
    std::map<std::string, std::string> selectedValues;

    // ncurses menu to select a type or action
    int menu(const std::vector<std::string>& items, const std::string& title);

    // ncurses editor to fill in parameter values for a given type
    void parameterEditor(const std::string& type);
};
