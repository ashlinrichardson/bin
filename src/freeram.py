'''20220413 free ram on ubuntu
20251020 added support for mac os'''
import os
import platform

def run(cmd):
    print(f"> {cmd}")
    return os.system(cmd)

def free_ram_linux():
    run("free -m")
    run("sudo sync")
    run("sudo sysctl -w vm.drop_caches=3")
    run("free -m")

def free_ram_macos():
    run("vm_stat")
    run("sync")
    run("sudo purge")  # May not work on modern macOS versions
    run("vm_stat")

def main():
    system = platform.system()
    print(f"Detected OS: {system}")

    if system == "Linux":
        free_ram_linux()
    elif system == "Darwin":
        free_ram_macos()
    else:
        print("Unsupported OS. This script supports Linux and macOS only.")

if __name__ == "__main__":
    main()

