#!/usr/bin/env python3
"""Convert all MP4 files in the current directory to MP3 in parallel, deleting originals on success."""

import os
import subprocess
import sys
from concurrent.futures import ThreadPoolExecutor, as_completed


def get_cpu_count():
    try:
        return len(os.sched_getaffinity(0))
    except AttributeError:
        return os.cpu_count() or 1


def convert(mp4):
    mp3 = os.path.splitext(mp4)[0] + ".mp3"
    result = subprocess.run(
        ["ffmpeg", "-i", mp4, "-vn", "-acodec", "libmp3lame", "-q:a", "2", mp3],
        stdout=subprocess.DEVNULL,
        stderr=subprocess.PIPE,
    )
    if result.returncode == 0:
        os.remove(mp4)
        return mp4, True, ""
    return mp4, False, result.stderr.decode()


def main():
    mp4_files = [f for f in os.listdir(".") if f.lower().endswith(".mp4")]

    if not mp4_files:
        print("No MP4 files found in the current directory.")
        sys.exit(0)

    workers = get_cpu_count()
    print(f"Found {len(mp4_files)} file(s). Converting with {workers} worker(s).\n")

    with ThreadPoolExecutor(max_workers=workers) as pool:
        futures = {pool.submit(convert, f): f for f in mp4_files}

        for future in as_completed(futures):
            name, ok, err = future.result()
            if ok:
                print(f"  OK   {name}")
            else:
                print(f"  FAIL {name}", file=sys.stderr)
                print(f"       {err}", file=sys.stderr)

    print("\nDone.")


if __name__ == "__main__":
    main()


