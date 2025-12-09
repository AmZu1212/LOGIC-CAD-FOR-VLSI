#!/usr/bin/env bash

# Generate submission archive with only the required source files.
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"

ID1=212606222
ID2=323686683

ARCHIVE_NAME="wet01_${ID1}_${ID2}.tar.gz"

# clean previous archive if present (stored alongside this script)
ARCHIVE_PATH="${SCRIPT_DIR}/${ARCHIVE_NAME}"
if [ -f "$ARCHIVE_PATH" ]; then
	rm -f "$ARCHIVE_PATH"
fi

# create archive from repo root so files are under wet01/ inside the tar
tar czf "$ARCHIVE_PATH" -C "$ROOT_DIR" wet01/HW1ex1.cc wet01/HW1ex2.cc
echo "Created $ARCHIVE_NAME with wet01/HW1ex1.cc and wet01/HW1ex2.cc"
