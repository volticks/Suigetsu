
#include "instruction.h"
#include "mmu.h"
#include <cstdint>

// Add this in the array to get the same version of the reg but for derefs and
// such.
const uint8_t mem_id = 4; // Was 3
// Used in decoding to get the register from the instruction.
const ArgKind dn_registers[] = {ArgKind::D0,  ArgKind::D1,  ArgKind::D2,
                                ArgKind::D3,  ArgKind::MD0, ArgKind::MD1,
                                ArgKind::MD2, ArgKind::MD3};
const ArgKind an_registers[] = {ArgKind::A0,  ArgKind::A1,  ArgKind::A2,
                                ArgKind::A3,  ArgKind::MA0, ArgKind::MA1,
                                ArgKind::MA2, ArgKind::MA3};

class Decoder {
public:
  // For ordinary decoding: in bounds, readable memory within the same page
  void decode_inst(const inst_data *curr_data, const inst_data *end,
                   Instruction &ins);
  // For more complicated decodes
  void decode_inst(const inst_data *curr_data, uint32_t n_to_end,
                   page_entry &next, Instruction &ins);
  // For adding and maintaining arguments
  bool add_args = false;
  uint8_t arg_sz = 0;
  uint8_t num_args = 0;
  const inst_data *end = NULL;

private:
  // For decoding the specific S* or D* ops.
  void decode_sn_op(const inst_data *inst_data, Instruction &ins_out);
  void decode_dn_op(const inst_data *inst_data, Instruction &ins_out);
  // Individual Dn decoders
  void decode_dn_op_F0(const inst_data *data, Instruction &ins);
  void decode_dn_op_F1(const inst_data *data, Instruction &ins);
  void decode_dn_op_F2(const inst_data *data, Instruction &ins);
  void decode_dn_op_F3(const inst_data *data, Instruction &ins);
  void decode_dn_op_F4(const inst_data *data, Instruction &ins);
  void decode_dn_op_F5(const inst_data *data, Instruction &ins);
  void decode_dn_op_F6(const inst_data *data, Instruction &ins);
  void decode_dn_op_F7(const inst_data *data, Instruction &ins);
  void decode_dn_op_F8(const inst_data *data, Instruction &ins);
  void decode_dn_op_F9(const inst_data *data, Instruction &ins);
  void decode_dn_op_FA(const inst_data *data, Instruction &ins);
  void decode_dn_op_FB(const inst_data *data, Instruction &ins);
  void decode_dn_op_FC(const inst_data *data, Instruction &ins);
  void decode_dn_op_FD(const inst_data *data, Instruction &ins);
  void decode_dn_op_FE(const inst_data *data, Instruction &ins);
  void decode_dn_op_FF(const inst_data *data, Instruction &ins);
};
