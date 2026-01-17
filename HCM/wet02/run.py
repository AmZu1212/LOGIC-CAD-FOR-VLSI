#!/usr/bin/env python3
import subprocess
import sys
from pathlib import Path


def run(cmd, cwd):
    print(f"+ {cmd}")
    result = subprocess.run(cmd, cwd=cwd, shell=True)
    if result.returncode != 0:
        sys.exit(result.returncode)


def main():
    wet02_dir = Path(__file__).resolve().parent
    run("make clean", str(wet02_dir))
    run("make", str(wet02_dir))

    verilog_dir = wet02_dir / "verilog inputs"
    stdcell = verilog_dir / "stdcell.v"

    # SAT/UNSAT example runs from the assignment
    run(
        f"./gl_verilog_fev -s TopLevel1355 {stdcell} {verilog_dir / 'c1355.v'} "
        f"-i TopLevel1356 {stdcell} {verilog_dir / 'c1356.v'}",
        str(wet02_dir),
    )
    run(
        f"./gl_verilog_fev -s TopLevel0409 {stdcell} {verilog_dir / 'c0409.v'} "
        f"-i TopLevel0410 {stdcell} {verilog_dir / 'c0410.v'}",
        str(wet02_dir),
    )


if __name__ == "__main__":
    main()
