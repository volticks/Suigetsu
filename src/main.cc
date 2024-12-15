#include "emulator.h"
#include <cstdio>
#include <fstream>
#include <iostream>
#include <memory>
#include <vector>

// Should be able to abstract any file related stuff in here, *maybe*
namespace FileReader {

std::vector<uint8_t> read_file(const char *fname) {
  std::ifstream ifs(fname);
  ifs.seekg(0, std::ios::end);
  const uint32_t sz = ifs.tellg();
  ifs.seekg(0, std::ios::beg);
  std::unique_ptr<uint8_t[]> file_conts = std::make_unique<uint8_t[]>(sz);
  ifs.read((char *)file_conts.get(), sz);
  std::vector<uint8_t> fconts((char *)file_conts.get(),
                              (char *)file_conts.get() + sz);
  return fconts;
}

} // namespace FileReader

int main(int argc, char **argv) {

  if (argc < 2) {
    std::cerr << "Usage: ./Suigetsu <filepath>." << std::endl;
    return 1;
  }

  Instructions insns = FileReader::read_file(argv[1]);

  Emulator emu;
  bool ret = emu.emu_loop(insns);
  // Do the execution.

  return 0;
}
