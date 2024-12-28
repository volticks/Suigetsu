
#include "instruction.h"
#include <cstdint>

// Used in decoding to get the register from the instruction.
const ArgKind dn_registers[] = {ArgKind::D0, ArgKind::D1, ArgKind::D2,
                                ArgKind::D3};
const ArgKind an_registers[] = {ArgKind::A0, ArgKind::A1, ArgKind::A2,
                                ArgKind::A3};

class Decoder {

public:
  void decode_inst(const inst_data *curr_data, const inst_data *end,
                   Instruction &ins);

private:
  // For decoding the specific S* or D* ops.
  void decode_sn_op(const inst_data *inst_data, Instruction &ins_out);
  void decode_dn_op(const inst_data *inst_data, Instruction &ins_out);
};
