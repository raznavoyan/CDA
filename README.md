Keysight Controller (Beta Version)
==================================

This is a Linux-based application for controlling Keysight measurement instruments over a network using SCPI commands. It features a user-friendly terminal interface and supports real-time data plotting and automatic file saving.

Features
--------
- Simple ncurses interface for entering measurement parameters
- Fully working Resistance measurement mode
- Capacitance mode is available in the menu, but not yet implemented or tested
- Live plotting with Python and matplotlib
- Automatic saving of measurement data (CSV) and plots (PNG)
- Creates desktop icon and launcher for easy access

Installation
------------
Requirements:
- Ubuntu or another Linux distro
- Python 3 with matplotlib:
  pip3 install --user matplotlib

To install:
Run:
  python3 installer.py

This will:
- Patch and compile the source code
- Install the program into ~/.local/share/keysight
- Add shortcuts to the desktop and application menu

Usage
-----
After installation, you can launch the program using:
- The desktop icon ("Keysight Controller"), or
- The terminal:
  ~/.local/share/keysight/test_interface

Then:
1. Choose "Resistance" mode from the menu
2. Enter the measurement parameters:
   - V start
   - V end
   - Points number
   - Plot (y/n)
   - Save data table (y/n)
3. Start the measurement
4. If enabled, a live plot window will appear
5. Data and plots are saved to ~/Desktop/data and ~/Desktop/plots

Note: The "Capacitance" mode is not implemented. The interface is present, but the feature was not tested due to time constraints.

Uninstall
---------
To remove the program and all associated files, run:
  python3 uninstall.py

This will delete:
- All files in ~/.local/share/keysight
- Application menu launcher
- Desktop shortcut

Files Overview
--------------
- test_interface : Main executable program
- plotter.py     : Python script for real-time plotting
- installer.py   : Installs and configures the application
- uninstall.py   : Removes all files and shortcuts
- icon.png       : Program icon

Author
------
Razmik Navoyan  
University of Nova Gorica, School of Science

Notes
-----
- Make sure the instrument IP (default: 192.168.1.120, port 5025) is reachable
- Basic SCPI commands are pre-programmed for typical operation
- The Python path for plotting is patched automatically by the installer
