#!/bin/sh
set -eu
set -f

test_dir=$(mktemp -d "${TMPDIR:-/tmp}/ir-c-test.XXXXXX")
trap 'rm -rf "$test_dir"' EXIT HUP INT TERM

# Make supplies the compiler and flags; the generated C file is the first argument.
${IR_C_TEST_CC:-cc} ${IR_C_TEST_CFLAGS:-} \
	-I"${IR_C_TEST_SRC_DIR:-.}" -I"${IR_C_TEST_BUILD_DIR:-.}" \
	"$1" "${IR_C_TEST_LIB}" ${IR_C_TEST_LDFLAGS:-} -o "$test_dir/test"
"$test_dir/test"
