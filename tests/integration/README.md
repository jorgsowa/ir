# C integration fixtures

These fixtures use the same `--TEST--`, `--CODE--`, and `--EXPECT--` sections
as `.irt` tests. `--CODE--` is a complete C program. The test runner compiles
it against `libir.a`, runs it, and compares its output with `--EXPECT--`.

Each `.ct` file is an independent case. It can call any externally visible IR
function, so new regressions need no change to the runner. Tests are platform
independent unless a case explicitly depends on a platform API. Tests of
`ir_disasm()` may include `tests/support/disasm_mock.c` to supply a synthetic
Capstone result without machine code. `mock_instruction()` sets the result;
`mock_expect()` runs the disassembler and checks its output. Run the fixtures
with `make test` or `make test-ci`; use UBSan to catch signed overflow cases.
