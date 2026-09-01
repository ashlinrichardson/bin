#!/usr/bin/env python3
"""Convert all OPUS files in the current directory to MP3 in parallel, deleting originals on success."""

import os
import shutil
import subprocess
import sys
from concurrent.futures import ThreadPoolExecutor, as_completed


def get_cpu_count():
    try:
        return len(os.sched_getaffinity(0))
    except AttributeError:
        return os.cpu_count() or 1


def convert(opus):
    mp3 = os.path.splitext(opus)[0] + ".mp3"
    existed = os.path.exists(mp3)

    try:
        result = subprocess.run(
            [
                "ffmpeg",
                "-nostdin",            # don't read the terminal (essential when running in parallel)
                "-n",                  # never overwrite an existing output; fail instead
                "-hide_banner",
                "-loglevel", "error",  # only real errors on stderr
                "-i", opus,
                "-vn",                 # drop embedded cover art / video streams
                "-acodec", "libmp3lame",
                "-q:a", "2",           # VBR, ~190 kbps
                mp3,
            ],
            stdout=subprocess.DEVNULL,
            stderr=subprocess.PIPE,
        )
    except BaseException:
        # Ctrl-C or similar: don't leave a truncated file behind
        if not existed and os.path.exists(mp3):
            os.remove(mp3)
        raise

    if result.returncode == 0:
        os.remove(opus)
        return opus, True, ""

    if not existed and os.path.exists(mp3):
        os.remove(mp3)
    return opus, False, result.stderr.decode(errors="replace").strip()


def main():
    if shutil.which("ffmpeg") is None:
        print("ffmpeg not found in PATH.", file=sys.stderr)
        sys.exit(1)

    opus_files = [f for f in os.listdir(".") if f.lower().endswith(".opus")]

    if not opus_files:
        print("No OPUS files found in the current directory.")
        sys.exit(0)

    workers = get_cpu_count()
    print(f"Found {len(opus_files)} file(s). Converting with {workers} worker(s).\n")

    failures = 0
    with ThreadPoolExecutor(max_workers=workers) as pool:
        futures = {pool.submit(convert, f): f for f in opus_files}

        for future in as_completed(futures):
            name, ok, err = future.result()
            if ok:
                print(f"  OK   {name}")
            else:
                failures += 1
                print(f"  FAIL {name}", file=sys.stderr)
                for line in err.splitlines():
                    print(f"       {line}", file=sys.stderr)

    print(f"\nDone. {len(opus_files) - failures} converted, {failures} failed.")
    sys.exit(1 if failures else 0)


if __name__ == "__main__":
    main()
