#!/usr/bin/env python3
import argparse
import subprocess
import sys
from pathlib import Path
from datetime import datetime
import shutil


def run(cmd, cwd, capture=False, allow_fail=False):
    if isinstance(cmd, str):
        printable = cmd
    else:
        printable = " ".join(str(part) for part in cmd)
    print(f"+ {printable}")
    if capture:
        result = subprocess.run(cmd, cwd=cwd, check=False, capture_output=True, text=True)
    else:
        result = subprocess.run(cmd, cwd=cwd, check=False)
    if result.returncode != 0 and not allow_fail:
        sys.exit(result.returncode)
    return result


def parse_status(output):
    if "NOT SATISFIABLE!" in output:
        return "NOT SATISFIABLE!"
    if "SATISFIABLE!" in output:
        return "SATISFIABLE!"
    return "ERROR"


def extract_expected(*paths):
    combined = []
    for path in paths:
        try:
            lines = Path(path).read_text(errors="ignore").splitlines()[:50]
        except OSError:
            continue
        combined.append("\n".join(lines).lower())
    text = "\n".join(combined)

    if "unsat" in text or "not satisfiable" in text:
        return "NOT SATISFIABLE!"
    if "non-equivalent" in text or "not equivalent" in text or "not be equivalent" in text or "different from" in text:
        return "SATISFIABLE!"
    if "sat" in text or "satisfiable" in text:
        return "SATISFIABLE!"
    if "equivalent" in text:
        return "NOT SATISFIABLE!"
    return "UNKNOWN"


def run_fev_tests(title, tests, wet02_dir, stdcell_path, expected_map=None, use_headers=False):
    rows = []
    failures = []

    for name, spec_top, spec_file, impl_top, impl_file in tests:
        result = run(
            [
                "./gl_verilog_fev",
                "-s",
                spec_top,
                str(stdcell_path),
                str(spec_file),
                "-i",
                impl_top,
                str(stdcell_path),
                str(impl_file),
            ],
            str(wet02_dir),
            capture=True,
        )
        status = parse_status(result.stdout or "")
        expected = "UNKNOWN"
        if use_headers:
            expected = extract_expected(spec_file, impl_file)
        if expected == "UNKNOWN" and expected_map is not None:
            expected = expected_map.get(name, "UNKNOWN")

        rows.append((name, status, expected))
        if expected != "UNKNOWN" and status != expected:
            failures.append((name, status, expected))

    return rows, failures


def print_table(title, rows):
    name_w = max(len(r[0]) for r in rows)
    res_w = max(len(r[1]) for r in rows)
    exp_w = max(len(r[2]) for r in rows)

    print(f"\n==================== {title} ====================")
    print(f"{'test':<{name_w}} | {'result':<{res_w}} | {'expected result':<{exp_w}}")
    print(f"{'-' * name_w} | {'-' * res_w} | {'-' * exp_w}")
    for name, status, expected in rows:
        print(f"{name:<{name_w}} | {status:<{res_w}} | {expected:<{exp_w}}")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--no-build", action="store_true", help="Skip make clean/make")
    args = parser.parse_args()

    wet02_dir = Path(__file__).resolve().parent
    if not args.no_build:
        run(["make", "clean"], str(wet02_dir), allow_fail=True)
        run(["make"], str(wet02_dir))

    verilog_dir = wet02_dir / "verilog inputs"
    stdcell = verilog_dir / "stdcell.v"

    main_tests = [
        ("c0409 vs c0410", "TopLevel0409", verilog_dir / "c0409.v", "TopLevel0410", verilog_dir / "c0410.v"),
        ("c1355 vs c1356", "TopLevel1355", verilog_dir / "c1355.v", "TopLevel1356", verilog_dir / "c1356.v"),
        ("c1404 vs c1405", "TopLevel1404", verilog_dir / "c1404.v", "TopLevel1405", verilog_dir / "c1405.v"),
        ("c2670 vs c2671", "TopLevel2670", verilog_dir / "c2670.v", "TopLevel2671", verilog_dir / "c2671.v"),
        ("c2670 vs c2672", "TopLevel2670", verilog_dir / "c2670.v", "TopLevel2672", verilog_dir / "c2672.v"),
        ("c2806 vs c2807", "TopLevel2806", verilog_dir / "c2806.v", "TopLevel2807", verilog_dir / "c2807.v"),
    ]

    main_expected = {
        "c0409 vs c0410": "SATISFIABLE!",
        "c1355 vs c1356": "NOT SATISFIABLE!",
        "c1404 vs c1405": "NOT SATISFIABLE!",
        "c2670 vs c2671": "SATISFIABLE!",
        "c2670 vs c2672": "NOT SATISFIABLE!",
        "c2806 vs c2807": "SATISFIABLE!",
    }

    ob_dir = wet02_dir / "utilities" / "OB_tests"
    ob_tests = [
        ("test0000 vs test0001", "TopLevel0000", ob_dir / "test0000.v", "TopLevel0001", ob_dir / "test0001.v"),
        ("test1110 vs test1111", "TopLevel1110", ob_dir / "test1110.v", "TopLevel1111", ob_dir / "test1111.v"),
        ("test0000 vs test0000", "TopLevel0000", ob_dir / "test0000.v", "TopLevel0000", ob_dir / "test0000.v"),
        ("test_nand5_a vs test_nand5_b", "TopLevelNand5A", ob_dir / "test_nand5_a.v", "TopLevelNand5B", ob_dir / "test_nand5_b.v"),
        ("test_or7_a vs test_or7_b", "TopLevelOr7A", ob_dir / "test_or7_a.v", "TopLevelOr7B", ob_dir / "test_or7_b.v"),
        ("test4440 vs test4441", "TopLevel4440", ob_dir / "test4440.v", "TopLevel4441", ob_dir / "test4441.v"),
        ("test5550 vs test5551", "TopLevel5550", ob_dir / "test5550.v", "TopLevel5551", ob_dir / "test5551.v"),
        ("c0409 vs c0410", "TopLevel0409", ob_dir / "c0409.v", "TopLevel0410", ob_dir / "c0410.v"),
        ("c1355 vs c1356", "TopLevel1355", ob_dir / "c1355.v", "TopLevel1356", ob_dir / "c1356.v"),
        ("c1404 vs c1405", "TopLevel1404", ob_dir / "c1404.v", "TopLevel1405", ob_dir / "c1405.v"),
        ("c2670 vs c2671", "TopLevel2670", ob_dir / "c2670.v", "TopLevel2671", ob_dir / "c2671.v"),
        ("c2670 vs c2672", "TopLevel2670", ob_dir / "c2670.v", "TopLevel2672", ob_dir / "c2672.v"),
        ("c2806 vs c2807", "TopLevel2806", ob_dir / "c2806.v", "TopLevel2807", ob_dir / "c2807.v"),
    ]

    ob_expected = {
        "test0000 vs test0001": "NOT SATISFIABLE!",
        "test1110 vs test1111": "SATISFIABLE!",
        "test0000 vs test0000": "NOT SATISFIABLE!",
        "test_nand5_a vs test_nand5_b": "NOT SATISFIABLE!",
        "test_or7_a vs test_or7_b": "NOT SATISFIABLE!",
        "test4440 vs test4441": "NOT SATISFIABLE!",
        "test5550 vs test5551": "NOT SATISFIABLE!",
        "c0409 vs c0410": "SATISFIABLE!",
        "c1355 vs c1356": "NOT SATISFIABLE!",
        "c1404 vs c1405": "NOT SATISFIABLE!",
        "c2670 vs c2671": "SATISFIABLE!",
        "c2670 vs c2672": "NOT SATISFIABLE!",
        "c2806 vs c2807": "SATISFIABLE!",
    }

    main_rows, main_failures = run_fev_tests(
        "MAIN VERILOG TESTS",
        main_tests,
        wet02_dir,
        stdcell,
        expected_map=main_expected,
    )
    ob_rows, ob_failures = run_fev_tests(
        "OB TESTS",
        ob_tests,
        wet02_dir,
        stdcell,
        expected_map=ob_expected,
        use_headers=True,
    )

    print_table("MAIN VERILOG TESTS", main_rows)
    print_table("OB TESTS", ob_rows)

    failures = main_failures + ob_failures

    output_base = wet02_dir / "outputs"
    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    output_dir = output_base / timestamp
    output_dir.mkdir(parents=True, exist_ok=True)
    for cnf_file in wet02_dir.glob("*.cnf"):
        shutil.move(str(cnf_file), str(output_dir / cnf_file.name))

    if failures:
        print("\nExpected-result mismatches:")
        for name, status, expected in failures:
            print(f"- {name}: got {status}, expected {expected}")
        sys.exit(1)


if __name__ == "__main__":
    main()
