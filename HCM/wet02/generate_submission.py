#!/usr/bin/env python3
import tarfile
from pathlib import Path
import shutil
import sys


def main():
    wet02_dir = Path(__file__).resolve().parent
    submissions_dir = wet02_dir / "submissions"
    submissions_dir.mkdir(parents=True, exist_ok=True)

    id1 = "212606222"
    id2 = "323686683"
    archive_name = f"wet02_{id1}_{id2}.tar.gz"
    archive_path = submissions_dir / archive_name

    staging_dir = submissions_dir / "_staging_wet02"
    if staging_dir.exists():
        shutil.rmtree(staging_dir)
    staging_dir.mkdir(parents=True)

    # Only include HW2ex1.cc in the wet02 folder.
    wet02_stage = staging_dir / "wet02"
    wet02_stage.mkdir(parents=True)
    hw2 = wet02_dir / "HW2ex1.cc"
    if not hw2.exists():
        print("HW2ex1.cc not found.")
        sys.exit(1)
    shutil.copy2(hw2, wet02_stage / "HW2ex1.cc")

    if archive_path.exists():
        archive_path.unlink()

    with tarfile.open(archive_path, "w:gz") as tar:
        tar.add(wet02_stage, arcname="wet02")

    shutil.rmtree(staging_dir)
    print(f"Created: {archive_path}")


if __name__ == "__main__":
    main()
