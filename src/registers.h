#pragma once
#include <cstdint>
#include <exception>
typedef uint32_t reg_type;
typedef uint32_t reg_idx;
const int num_reg = 4 + 3 + 6;
const int reg_usable = num_reg - 1;

// Should be able to index into the array with these enums
enum RegTypes {
  D0,
  D1,
  D2,
  A0,
  A1,
  A2,
  SP,
  PC,
  MDR,
  PSW, // 16bits
  LIR,
  LAR
};

class Reg {
public:
  Reg();

  // specialised functions cuz we prolly gonna be using these A LOT
  reg_type get_pc();
  void set_pc(reg_type pc);

  // general register access
  reg_type get(RegTypes reg);
  bool set(RegTypes reg, reg_type val);

private:
  reg_type registers[num_reg];
};

class RegisterException : public std::exception {
public:
  RegisterException(char *newMsg) : msg(newMsg) {}

  char *what() { return msg; }

private:
  char *msg;
};
