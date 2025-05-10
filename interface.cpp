#include "interface.hpp"
#include <ncurses.h>
#include <iostream>
#include <regex>

// Constructor defines available parameter fields for each measurement type
Interface::Interface() {
    parameters["Resistance"] = {
        "Vstart",
        "Vend",
        "Points number",
        "Plot (y/n)",
        "Save data table (y/n)"
    };

    parameters["Capacitance"] = {
        "Vstart",
        "Vend",
        "Frequency",
        "AC level",
        "Points number",
        "Plot (y/n)",
        "Save data table (y/n)"
    };
}

Interface::~Interface() {}

// Display ncurses-based selection menu
int Interface::menu(const std::vector<std::string>& items, const std::string& title) {
    int highlight = 0;
    int choice = -1;
    while (true) {
        clear();
        mvprintw(0, 2, "%s", title.c_str());
        for (size_t i = 0; i < items.size(); ++i) {
            if (i == static_cast<size_t>(highlight)) attron(A_REVERSE);
            mvprintw(i + 2, 4, "%zu. %s", i + 1, items[i].c_str());
            if (i == static_cast<size_t>(highlight)) attroff(A_REVERSE);
        }
        int c = getch();
        if (c == KEY_UP) highlight = (highlight - 1 + items.size()) % items.size();
        else if (c == KEY_DOWN) highlight = (highlight + 1) % items.size();
        else if (c == 10) {
            choice = highlight;
            break;
        }
    }
    return choice;
}

// Launch the selection and parameter entry interface
void Interface::Run() {
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);

    std::vector<std::string> types = {"Resistance", "Capacitance"};
    int choice = menu(types, "Select Measurement Type:");
    parameterEditor(types[choice]);
}

// Validate field input format (float, integer, yes/no)
bool validateInput(const std::string& field, const std::string& value) {
    if (field == "Vstart" || field == "Vend" || field == "Frequency" || field == "AC level") {
        return std::regex_match(value, std::regex(R"(^-?\d+(\.\d+)?$)"));
    } else if (field == "Points number") {
        return std::regex_match(value, std::regex(R"(^\d+$)"));
    } else if (field == "Plot (y/n)" || field == "Save data table (y/n)") {
        return std::regex_match(value, std::regex(R"(^(y|n|Y|N)$)"));
    }
    return true;
}

// Input and edit field values for selected type
void Interface::parameterEditor(const std::string& type) {
    selectedType = type;
    std::vector<std::string>& fields = parameters[type];
    std::vector<std::string> values(fields.size(), "");
    int pos = 0;
    int total = static_cast<int>(fields.size()) + 1;

    while (true) {
        clear();
        mvprintw(0, 2, "Configure parameters for %s (ENTER to edit, TAB to move, ENTER on START)", type.c_str());
        for (size_t i = 0; i < fields.size(); ++i) {
            if (static_cast<int>(i) == pos) attron(A_REVERSE);
            mvprintw(static_cast<int>(i) + 2, 4, "%s: %s", fields[i].c_str(), values[i].c_str());
            if (static_cast<int>(i) == pos) attroff(A_REVERSE);
        }
        if (pos == static_cast<int>(fields.size())) attron(A_REVERSE);
        mvprintw(static_cast<int>(fields.size()) + 3, 4, "[ START MEASUREMENT ]");
        if (pos == static_cast<int>(fields.size())) attroff(A_REVERSE);

        int c = getch();
        if (c == KEY_UP) pos = (pos - 1 + total) % total;
        else if (c == KEY_DOWN || c == '\t') pos = (pos + 1) % total;
        else if (c == 10) {
            if (pos == static_cast<int>(fields.size())) break;
            else {
                echo();
                char buf[128];
                while (true) {
                    move(pos + 2, 6 + fields[pos].size());
                    clrtoeol();
                    getstr(buf);
                    std::string value = buf;
                    if (validateInput(fields[pos], value)) {
                        values[pos] = value;
                        break;
                    } else {
                        mvprintw(static_cast<int>(fields.size()) + 5, 2, "Invalid input for %s. Try again.", fields[pos].c_str());
                    }
                }
                noecho();
            }
        }
    }

    selectedValues.clear();
    for (size_t i = 0; i < fields.size(); ++i) {
        selectedValues[fields[i]] = values[i];
    }
}

// Provide the result (type + parameters) to caller
std::pair<std::string, std::map<std::string, std::string>> Interface::getMeasurementParams() const {
    return {selectedType, selectedValues};
}

