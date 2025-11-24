#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$0")"

rm Syndrome.stat || true
make clean
make gl_stat
./gl_stat -v Syndrome stdcell.v c1355high.v
