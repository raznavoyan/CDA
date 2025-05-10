#pragma once

// Standard includes
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <regex>
#include <thread>
#include <chrono>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include <stdexcept>

// System includes
#include <ncurses.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

// Local components
#include "interface.hpp"
#include "measurement_manager.hpp"
#include "data_manager.hpp"
#include "PyPlotter.hpp"

