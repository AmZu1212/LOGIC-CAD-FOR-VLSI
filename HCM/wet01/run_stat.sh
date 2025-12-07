#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$0")"

[ -e Syndrome.stat ] && rm Syndrome.stat
[ -e gl_rank ] && rm gl_rank

make clean
make gl_stat

# gl_stat Runs (u can use "-v" for more prints)
./gl_stat TopLevel1355 stdcell.v c1355high.v
./gl_stat TopLevel2670 stdcell.v c2670high.v

# gl_rank Runs
##./gl_rank -v TopLevel1355 stdcell.v c1355high.v
##./gl_rank -v TopLevel2670 stdcell.v c2670high.v


# diff results with expected
compare_fields() {
  local file="$1" expected="$2" status=0

  get_val() { awk -F': *' -v key="$1" '$1==key {print $2; exit}' "$2"; }

  for key in a b c d e; do
    actual=$(get_val "$key" "$file")
    expect=$(get_val "$key" "$expected")
    if [ "$actual" != "$expect" ]; then
      echo "$key differs: actual=$actual expected=$expect"
      status=1
    fi
  done

  return $status
}

echo "------------- Runs Are Done ------------------"

echo ""
echo ""
echo "===== 1355 Check ====="

# diff check - generalize later
if compare_fields "TopLevel1355.stat" "Expected Outputs/TopLevel1355.stat.expected"; then
  echo ""
  echo ""
  echo "a,b,c,d,e all match."
else
  echo ""
  echo ""
  echo "Mismatch detected."
fi

echo ""
echo ""
echo ""
echo "===== 2670 Check ====="


# diff check - generalize later
if compare_fields "TopLevel2670.stat" "Expected Outputs/TopLevel2670.stat.expected"; then
  echo ""
  echo ""
  echo "a,b,c,d,e all match."
else
  echo ""
  echo ""
  echo "Mismatch detected."
fi

echo ""
echo ""