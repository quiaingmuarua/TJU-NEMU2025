#!/bin/sh
set -eu

if [ "$#" -lt 2 ]; then
  echo "usage: run_nemu.sh <nemu_bin> <testcase_bin> [nemu_args...]" >&2
  exit 2
fi

NEMU="$1"
TEST="$2"
shift 2

printf 'c\nq' | "$NEMU" "$@" "$TEST"
