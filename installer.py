import os
import shutil
import subprocess
from pathlib import Path
import sys

# === Step 1: Patch + Build ===

# Run patch script to fix Python interpreter in PyPlotter.cpp
subprocess.run(["python3", "patch_python_in_pyplotter.py"], check=True)

# Clean previous builds
subprocess.run(["make", "clean"], check=True)

# Build the project
subprocess.run(["make"], check=True)

# === Step 2: Installer ===

HOME = Path.home()
APP_ID = "keysight"
APP_NAME = "Keysight Controller"
APP_DIR = HOME / f".local/share/{APP_ID}"                  # Main install dir
WRAPPER = APP_DIR / "run_interface.sh"                     # Launcher wrapper script
DESKTOP_FILE = HOME / ".local/share/applications/keysight.desktop"  # Menu entry
DESKTOP_ICON = HOME / "Desktop/Keysight.desktop"           # Shortcut icon
PYFILE = Path("PyPlotter.cpp")                             # C++ file to patch

# Files and folders to copy into the install directory
FILES = ["test_interface", "plotter.py", "icon.png"]
DIRS = ["data", "plots"]

# Ensure local folders exist before install
for d in DIRS:
    Path(d).mkdir(exist_ok=True)

# Return path to python3 and verify matplotlib is installed
def get_python_with_matplotlib():
    python_path = shutil.which("python3")
    print(f"[*] Checking interpreter: {python_path}")
    try:
        subprocess.run([python_path, "-c", "import matplotlib.pyplot as plt"], check=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        print(f"[✓] matplotlib found in {python_path}")
        return python_path
    except subprocess.CalledProcessError:
        print("[!] matplotlib NOT found in default python3. Try: pip3 install --user matplotlib")
        sys.exit(1)

# Modify PyPlotter.cpp to use absolute path to python3
def patch_pyplotter(python_path):
    if not PYFILE.exists():
        print("[!] PyPlotter.cpp not found for patching")
        sys.exit(1)

    code = PYFILE.read_text()
    original = 'execlp("python3", "python3", script.c_str(), (char*)NULL);'
    replacement = f'execl("{python_path}", "python3", script.c_str(), (char*)NULL);'

    if replacement in code:
        print("[=] PyPlotter.cpp already patched.")
        return

    if original not in code:
        print("[!] Could not find original exec line to patch.")
        sys.exit(1)

    PYFILE.write_text(code.replace(original, replacement))
    print(f"[✓] Patched PyPlotter.cpp to use: {python_path}")

# Skip rebuilding because it was already done above
def build_binary():
    print("[*] Skipping: build already done above")

# Copy all required files to ~/.local/share/keysight
def copy_files():
    print(f"[*] Installing to {APP_DIR}")
    APP_DIR.mkdir(parents=True, exist_ok=True)
    for f in FILES:
        shutil.copy2(f, APP_DIR / f)
    for d in DIRS:
        src = Path(d)
        dest = APP_DIR / d
        if dest.exists():
            shutil.rmtree(dest)
        shutil.copytree(src, dest)
    os.chmod(APP_DIR / "test_interface", 0o755)

# Create a bash wrapper script to run the interface binary
def create_wrapper(env_activate_line=None):
    print(f"[*] Creating wrapper: {WRAPPER}")
    script = "#!/bin/bash\n"
    if env_activate_line:
        script += f"{env_activate_line}\n"
    script += f'"{APP_DIR}/test_interface"\n'
    script += 'read -n 1 -s -r -p "Press any key to exit..."\n'
    WRAPPER.write_text(script)
    os.chmod(WRAPPER, 0o755)

# Generate .desktop file for GUI launcher
def create_launcher(path):
    print(f"[*] Writing launcher: {path}")
    content = f"""[Desktop Entry]
Version=1.0
Name={APP_NAME}
Comment=Lab measurement control interface
Exec=gnome-terminal -- {WRAPPER}
Icon={APP_DIR}/icon.png
Terminal=false
Type=Application
Categories=Science;Utility;
"""
    with open(path, "w") as f:
        f.write(content)
    os.chmod(path, 0o755)

# Detect if python is in virtualenv and return activate command
def detect_env_activation(python_path):
    if "xraylarch" in python_path:
        env_path = Path(python_path).parent.parent
        activate = env_path / "bin" / "activate"
        if activate.exists():
            return f'source "{activate}"'
    return None

# Full installation procedure
def install():
    python_path = get_python_with_matplotlib()
    patch_pyplotter(python_path)
    build_binary()
    copy_files()
    activation_line = detect_env_activation(python_path)
    create_wrapper(activation_line)
    create_launcher(DESKTOP_FILE)
    create_launcher(DESKTOP_ICON)
    print("[✓] Installed successfully.")

# Run installation when script is executed
if __name__ == "__main__":
    install()

