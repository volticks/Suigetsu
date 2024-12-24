
#include "instruction.h"
#include <cstdint>

class Decoder {

public:
  Instruction decode_inst(const inst_data *curr_data, const inst_data *end);

private:
  // For decoding the specific S* or D* ops.
  void decode_sn_op(const inst_data *inst_data, Instruction &ins_out);
  void decode_dn_op(const inst_data *inst_data, Instruction &ins_out);
};
