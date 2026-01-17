#!/usr/bin/env python3
import subprocess
import sys
from pathlib import Path


def main():
    wet02_dir = Path(__file__).resolve().parent
    verilog_dir = wet02_dir / "verilog inputs"
    stdcell = verilog_dir / "stdcell.v"

    cmd = [
        "./gl_verilog_fev",
        "-s",
        "TopLevel0409",
        str(stdcell),
        str(verilog_dir / "c0409.v"),
        "-i",
        "TopLevel0410",
        str(stdcell),
        str(verilog_dir / "c0410.v"),
    ]

    result = subprocess.run(cmd, cwd=wet02_dir, check=False, text=True, capture_output=True)
    sys.stdout.write(result.stdout)
    sys.stderr.write(result.stderr)
    sys.exit(result.returncode)


if __name__ == "__main__":
    main()
