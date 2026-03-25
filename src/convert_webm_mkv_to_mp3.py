'''20260325 convert webm and mkv files to mp3'''
import os
import subprocess
from concurrent.futures import ThreadPoolExecutor, as_completed
from pathlib import Path
import threading

# File extensions to process
EXTENSIONS = {".webm", ".mkv"}

# Thread-safe error collection
errors = []
errors_lock = threading.Lock()

def run_command(cmd):
    """Run a shell command and return (success, stdout, stderr)."""
    try:
        result = subprocess.run(
            cmd,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True
        )
        return result.returncode == 0, result.stdout, result.stderr
    except Exception as e:
        return False, "", str(e)


def convert_to_mp3(input_path):
    """Convert a single file to MP3 and validate it."""
    input_path = Path(input_path)
    output_path = input_path.with_suffix(".mp3")

    # Skip if already exists
    if output_path.exists():
        return f"SKIPPED (exists): {input_path.name}"

    # Convert to MP3
    convert_cmd = [
        "ffmpeg",
        "-y",  # overwrite if needed
        "-i", str(input_path),
        "-vn",  # no video
        "-acodec", "libmp3lame",
        "-ab", "192k",
        str(output_path)
    ]

    success, _, stderr = run_command(convert_cmd)

    if not success or not output_path.exists():
        with errors_lock:
            errors.append((input_path.name, "Conversion failed", stderr))
        return f"FAILED: {input_path.name}"

    # Validate MP3 using ffmpeg
    validate_cmd = [
        "ffmpeg",
        "-v", "error",
        "-i", str(output_path),
        "-f", "null",
        "-"
    ]

    valid, _, val_err = run_command(validate_cmd)

    # Check file size (basic sanity check)
    size_ok = output_path.stat().st_size > 1024 * 50  # >50 KB

    if not valid or not size_ok:
        with errors_lock:
            errors.append((input_path.name, "Validation failed", val_err))
        return f"INVALID: {input_path.name}"

    # Delete original if everything is good
    try:
        input_path.unlink()
    except Exception as e:
        with errors_lock:
            errors.append((input_path.name, "Delete failed", str(e)))
        return f"WARNING (converted but not deleted): {input_path.name}"

    return f"SUCCESS: {input_path.name}"


def main():
    folder = Path(".")

    files = [f for f in folder.iterdir() if f.suffix.lower() in EXTENSIONS]

    if not files:
        print("No matching files found.")
        return

    print(f"Found {len(files)} files. Processing...\n")

    max_workers = os.cpu_count() or 4

    with ThreadPoolExecutor(max_workers=max_workers) as executor:
        futures = {executor.submit(convert_to_mp3, f): f for f in files}

        for future in as_completed(futures):
            print(future.result())

    print("\n--- SUMMARY ---")

    if errors:
        print(f"\nErrors ({len(errors)}):\n")
        for filename, reason, detail in errors:
            print(f"{filename}: {reason}")
            if detail:
                print(f"  Detail: {detail.strip()[:300]}")
    else:
        print("All files processed successfully.")


if __name__ == "__main__":
    main()


