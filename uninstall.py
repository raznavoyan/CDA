import os
from pathlib import Path
import shutil

# Define important file paths
HOME = Path.home()
APP_DIR = HOME / ".local/share/keysight"                   # Application files directory
DESKTOP_FILE = HOME / ".local/share/applications/keysight.desktop"  # Application menu entry
DESKTOP_ICON = HOME / "Desktop/Keysight.desktop"           # Desktop shortcut

# Function to delete a file or directory
def delete(path):
    if path.exists():
        if path.is_dir():
            shutil.rmtree(path)  # Recursively remove directory
            print(f"[✓] Removed directory: {path}")
        else:
            path.unlink()        # Remove file
            print(f"[✓] Removed file: {path}")
    else:
        print(f"[ ] Not found: {path}")  # Nothing to delete

# Main uninstall procedure
def uninstall():
    print("[*] Uninstalling Keysight Controller...")
    delete(APP_DIR)
    delete(DESKTOP_FILE)
    delete(DESKTOP_ICON)
    print("[✓] Uninstall complete.")

# Run uninstall when script is executed directly
if __name__ == "__main__":
    uninstall()

