import shutil
from pathlib import Path
import subprocess
import sys

# Path to the C++ source file we want to patch
PYFILE = Path("PyPlotter.cpp")

# Detect system Python interpreter and check matplotlib availability
def get_python_with_matplotlib():
    python_path = shutil.which("python3")  # Locate python3 in PATH
    print(f"[*] Checking: {python_path}")
    try:
        # Try importing matplotlib to ensure it is installed
        subprocess.run([python_path, "-c", "import matplotlib.pyplot as plt"], check=True)
        print("[✓] matplotlib is available")
    except subprocess.CalledProcessError:
        print("[!] matplotlib is NOT available in that Python")
        sys.exit(1)
    return python_path

# Patch PyPlotter.cpp to call Python with absolute path using bash
def patch_pyplotter(python_path):
    if not PYFILE.exists():
        print("[!] PyPlotter.cpp not found")
        sys.exit(1)

    code = PYFILE.read_text()

    # Look for the exec call pattern used to launch the Python plot script
    if 'execlp("python3", "python3", script.c_str(),' in code or 'execl(' in code:
        print("[*] Patching PyPlotter.cpp to use bash -c with full python path...")

        # Construct new command using bash -c and full Python path
        patch_code = (
            'std::string full_cmd = std::string("' + python_path +
            ' ") + script;\n'
            'execl("/bin/bash", "bash", "-c", full_cmd.c_str(), (char*)NULL);'
        )

        # Replace old exec lines with the new patched version
        code = code.replace(
            'execlp("python3", "python3", script.c_str(), (char*)NULL);',
            patch_code
        )
        code = code.replace(
            'execl("python3", "python3", script.c_str(), (char*)NULL);',
            patch_code
        )

        # Write the patched code back to file
        PYFILE.write_text(code)
        print("[✓] Patched PyPlotter.cpp with full bash-based command.")
    else:
        print("[=] Already patched or unexpected format.")

# Main entry point
if __name__ == "__main__":
    python_path = get_python_with_matplotlib()
    patch_pyplotter(python_path)

