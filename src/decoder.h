
#include <cstdint>
typedef uint8_t inst_data;
typedef uint8_t inst_op;

struct Instruction {
  // Size of the current instruction in bytes.
  uint8_t sz;
  inst_op op;
  // May end up being 0 or more bytes depending on size of inst
  inst_data args[3];
};

class Decoder {

public:
  Instruction decode_inst(inst_data *curr_data);

private:
};
