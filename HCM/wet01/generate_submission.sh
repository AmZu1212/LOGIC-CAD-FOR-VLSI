#!/usr/bin/env bash

# Generate submission archive with only the required source files.
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

ID1=212606222
ID2=323686683

ARCHIVE_NAME="wet01_${ID1}_${ID2}.tar.gz"

# clean previous archive if present
if [ -f "$ARCHIVE_NAME" ]; then
	rm -f "$ARCHIVE_NAME"
fi

tar czf "$ARCHIVE_NAME" HW1ex1.cc HW1ex2.cc
echo "Created $ARCHIVE_NAME with HW1ex1.cc and HW1ex2.cc"
