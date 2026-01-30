#include "cpu/exec/helper.h"

#define DATA_BYTE 2
#include "cmov-template.h"
#undef instr
#include "cmovne-template.h"
#undef instr
#include "cmovns-template.h"
#undef instr
#undef DATA_BYTE

#define DATA_BYTE 4
#include "cmov-template.h"
#undef instr
#include "cmovne-template.h"
#undef instr
#include "cmovns-template.h"
#undef instr
#undef DATA_BYTE

make_helper_v(cmovs_rm2r)
make_helper_v(cmovne_rm2r)
make_helper_v(cmovns_rm2r)
