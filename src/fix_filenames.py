'''20220423 fix filenames by removing brackets,
and replacing space w underscore

revised 20260116'''

#!/usr/bin/env python3
"""
fix_filenames.py - Make filesystem tree Unix-friendly

Recursively processes the current directory to:
- Remove brackets: [ ] { } ( )
- Replace spaces with underscores

Processes directories bottom-up to handle nested renames correctly.
"""

import os
import sys
import argparse

BRACKETS = ['[', ']', '{', '}', '(', ')']


def sanitize_name(name):
    """Remove brackets and replace spaces with underscores."""
    for char in BRACKETS:
        name = name.replace(char, '')
    name = name.replace(' ', '_')
    # Collapse multiple underscores
    while '__' in name:
        name = name.replace('__', '_')
    return name


def rename_item(old_path, new_path, dry_run=False):
    """Rename a file or directory, with optional dry-run mode."""
    if old_path == new_path:
        return False

    if os.path.exists(new_path):
        print(f"Warning: Cannot rename, target exists:\n  {old_path}\n  -> {new_path}")
        return False

    action = "Would rename" if dry_run else "Renaming"
    print(f'{action}:\n  "{old_path}"\n  -> "{new_path}"')

    if not dry_run:
        os.rename(old_path, new_path)
    return True


def process_directory(base_dir, dry_run=False, verbose=False):
    """
    Process all files and directories under base_dir.

    Uses bottom-up traversal to handle directory renames correctly.
    """
    base_dir = os.path.abspath(base_dir)
    rename_count = 0

    # Collect all paths first, then process bottom-up
    all_dirs = []
    all_files = []

    for root, dirs, files in os.walk(base_dir):
        all_dirs.append(root)
        for f in files:
            all_files.append(os.path.join(root, f))

    # Process files first
    for filepath in all_files:
        dirname = os.path.dirname(filepath)
        filename = os.path.basename(filepath)
        new_filename = sanitize_name(filename)

        if new_filename != filename:
            new_filepath = os.path.join(dirname, new_filename)
            if rename_item(filepath, new_filepath, dry_run):
                rename_count += 1

    # Process directories bottom-up (deepest first)
    all_dirs.sort(key=lambda x: x.count(os.sep), reverse=True)

    for dirpath in all_dirs:
        if dirpath == base_dir:
            continue  # Don't rename the base directory

        parent = os.path.dirname(dirpath)
        dirname = os.path.basename(dirpath)
        new_dirname = sanitize_name(dirname)

        if new_dirname != dirname:
            new_dirpath = os.path.join(parent, new_dirname)
            if rename_item(dirpath, new_dirpath, dry_run):
                rename_count += 1

    return rename_count


def main():
    parser = argparse.ArgumentParser(
        description='Make filenames Unix-friendly by removing brackets and spaces.',
        epilog='Example: %(prog)s --dry-run /path/to/directory'
    )
    parser.add_argument(
        'directory',
        nargs='?',
        default='.',
        help='Directory to process (default: current directory)'
    )
    parser.add_argument(
        '-n', '--dry-run',
        action='store_true',
        help='Show what would be renamed without making changes'
    )
    parser.add_argument(
        '-v', '--verbose',
        action='store_true',
        help='Show verbose output'
    )

    args = parser.parse_args()

    if not os.path.isdir(args.directory):
        print(f"Error: '{args.directory}' is not a valid directory", file=sys.stderr)
        sys.exit(1)

    if args.dry_run:
        print("=== DRY RUN MODE - No changes will be made ===\n")

    count = process_directory(args.directory, dry_run=args.dry_run, verbose=args.verbose)

    if count == 0:
        print("No files or directories needed renaming.")
    else:
        action = "would be" if args.dry_run else "were"
        print(f"\n{count} item(s) {action} renamed.")


if __name__ == '__main__':
    main()

