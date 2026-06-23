'''20260623 freespace.py: df -h is confusing on mac. 

How much free space do I actually have? On a mac.
'''
#!/usr/bin/env python3

import subprocess
import re
import sys


def run(cmd):
    return subprocess.check_output(cmd, text=True)


def human(n):
    units = ["B", "KB", "MB", "GB", "TB"]
    n = float(n)

    for unit in units:
        if n < 1024 or unit == units[-1]:
            return f"{n:.1f} {unit}"
        n /= 1024


def get_data_volume_stats():
    df = run(["df", "-k", "/System/Volumes/Data"])

    lines = df.strip().splitlines()
    cols = lines[1].split()

    total_k = int(cols[1])
    used_k = int(cols[2])
    avail_k = int(cols[3])

    return (
        total_k * 1024,
        used_k * 1024,
        avail_k * 1024,
    )


def get_apfs_container_info():
    text = run(["diskutil", "apfs", "list"])

    container_size = None
    container_free = None

    for line in text.splitlines():

        m = re.search(
            r"Capacity Ceiling.*:\s*([\d,]+)\s*B",
            line,
        )
        if m:
            container_size = int(m.group(1).replace(",", ""))

        m = re.search(
            r"Capacity Available.*:\s*([\d,]+)\s*B",
            line,
        )
        if m:
            container_free = int(m.group(1).replace(",", ""))

    return container_size, container_free


def main():

    total, used, avail = get_data_volume_stats()

    print("PRIMARY STORAGE")
    print("----------------")
    print(f"Total : {human(total)}")
    print(f"Used  : {human(used)}")
    print(f"Free  : {human(avail)}")

    try:
        container_size, container_free = get_apfs_container_info()

        if container_size and container_free:
            container_used = container_size - container_free

            print()
            print("APFS CONTAINER")
            print("----------------")
            print(f"Size  : {human(container_size)}")
            print(f"Used  : {human(container_used)}")
            print(f"Free  : {human(container_free)}")

    except Exception:
        pass


if __name__ == "__main__":
    main()
