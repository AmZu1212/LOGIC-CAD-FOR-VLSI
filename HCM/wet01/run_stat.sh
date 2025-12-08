#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$0")"

[ -e Syndrome.stat ] && rm Syndrome.stat
[ -e gl_rank ] && rm gl_rank

make clean
rm -f *.stat *.rank
make all

# gl_stat Runs (u can use "-v" for more prints)
./gl_stat TopLevel1355 stdcell.v c1355high.v
./gl_stat TopLevel2670 stdcell.v c2670high.v

# gl_rank Runs
./gl_rank TopLevel1355 stdcell.v c1355high.v
./gl_rank TopLevel2670 stdcell.v c2670high.v


# Compare stats: report per-field for a–e with expected/actual; f only says match/not
get_val() { awk -F': *' -v key="$1" '$1==key {print $2; exit}' "$2"; }

check_stats() {
  local file="$1" expected="$2" status=0

  for key in a b c d e; do
    actual=$(get_val "$key" "$file")
    expect=$(get_val "$key" "$expected")
    if [ "$actual" != "$expect" ]; then
      echo "$key mismatch: actual=$actual expected=$expect"
      status=1
    else
      echo "$key ok: $actual"
    fi
  done

  if diff -q "$expected" "$file" >/dev/null; then
    echo "f matches (full content)"
  else
    echo "f differs (content not shown; run 'diff -u \"$expected\" \"$file\"' to view)"
    status=1
  fi

  return $status
}

check_rank() {
  local file="$1" expected="$2"
  if diff -q "$expected" "$file" >/dev/null; then
    echo "rank matches: $file"
    return 0
  else
    echo "rank differs for $file (run 'diff -u \"$expected\" \"$file\"' to view)"
    return 1
  fi
}

echo "------------- Runs Are Done ------------------"

echo ""
echo ""
echo "===== 1355 Check ====="

overall_status=0

# run checks without exiting early on mismatch
set +e
check_stats "TopLevel1355.stat" "Expected Outputs/TopLevel1355.stat.expected" || overall_status=1
check_rank "TopLevel1355.rank" "Expected Outputs/TopLevel1355.rank.expected" || overall_status=1

echo ""
echo ""
echo ""
echo "===== 2670 Check ====="


check_stats "TopLevel2670.stat" "Expected Outputs/TopLevel2670.stat.expected" || overall_status=1
check_rank "TopLevel2670.rank" "Expected Outputs/TopLevel2670.rank.expected" || overall_status=1
set -e

echo ""
echo ""

exit $overall_status
