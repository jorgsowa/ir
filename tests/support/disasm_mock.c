/* Capstone stand-in for tests that exercise ir_disasm() without machine code. */
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#include <capstone/capstone.h>

#include "ir.h"

static cs_insn mock_insn;
static cs_detail mock_detail;
static unsigned int mock_group;

bool cs_disasm_iter(csh handle, const uint8_t **code, size_t *size,
                    uint64_t *address, cs_insn *insn)
{
	cs_detail *detail;

	if (*size == 0) {
		return false;
	}
	detail = insn->detail;
	*insn = mock_insn;
	insn->detail = detail;
	*detail = mock_detail;
	insn->address = *address;
	insn->size = 1;
	(*code)++;
	(*address)++;
	(*size)--;
	return true;
}

bool cs_insn_group(csh handle, const cs_insn *insn, unsigned int group_id)
{
	return mock_group != 0 && group_id == mock_group;
}

static inline void mock_instruction(const char *mnemonic, const char *operands)
{
	memset(&mock_insn, 0, sizeof(mock_insn));
	memset(&mock_detail, 0, sizeof(mock_detail));
	mock_group = 0;
	snprintf(mock_insn.mnemonic, sizeof(mock_insn.mnemonic), "%s", mnemonic);
	snprintf(mock_insn.op_str, sizeof(mock_insn.op_str), "%s", operands);
}

static inline void mock_negative_hex(uint64_t magnitude)
{
	mock_instruction("mov", "");
#if defined(IR_TARGET_AARCH64)
	snprintf(mock_insn.op_str, sizeof(mock_insn.op_str), "-#0x%" PRIx64, magnitude);
#else
	snprintf(mock_insn.op_str, sizeof(mock_insn.op_str), "$-0x%" PRIx64, magnitude);
#endif
}

static int mock_disasm_capture(char *output, size_t capacity)
{
	uint8_t code = 0;
	FILE *stream = tmpfile();
	size_t n;

	if (!stream) {
		return 0;
	}
	if (!ir_disasm("test", &code, 1, false, NULL, stream)) {
		fclose(stream);
		return 0;
	}
	rewind(stream);
	n = fread(output, 1, capacity - 1, stream);
	output[n] = '\0';
	fclose(stream);
	return 1;
}

static inline int mock_expect(const char *fragment)
{
	char output[256];
	int ok = mock_disasm_capture(output, sizeof(output)) && strstr(output, fragment) != NULL;

	ir_disasm_free();
	if (!ok) {
		fprintf(stderr, "missing disassembly text: %s\n", fragment);
		return 1;
	}
	puts("ok");
	return 0;
}
