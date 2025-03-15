#include "emulator.h"
#include "mmu.h"
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
  // TODO: This check is really really stupid
  if (sz == 0xffffffff) {
    std::cerr << "FileReader::read_file failed to open, does it exist?"
              << std::endl;
    exit(-1);
  }
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

  try {
    Instructions insns = FileReader::read_file(argv[1]);

    Emulator emu;
    // Setup some emu resources
    // Memory
    // Init: Need to map space for the program code starting at configurable
    // address, then write trhe instructions to that location,
    const virt_addr prog_start = 0x0;
    const uint32_t n_pages = (insns.size() / page_size) + 1;
    // R&X by default
    // --- Changed to rwx
    // Make sure we map enough space.
    emu.get_mmu().map_range(prog_start, n_pages, 0b111);
    // Copy in the instructions
    emu.get_mmu().write_many(prog_start, insns.data(), insns.size());
    // TODO: I did make flags specifcally for me to use for specifying perms
    // lol.
    // Some data pages, R&W
    // emu.get_mmu().map_range(prog_start + (n_pages * page_size), 100, 0b110);
    // RWX
    emu.get_mmu().map_range(prog_start + (n_pages * page_size), 100, 0b111);

    // Another data test page
    emu.get_mmu().get_pd().add_page(0x41414000, PagePerms::rwx_mask);
    // Constructor for page_entry already sets these up, but just to make sure.
    // emu.get_mmu().get_pd().get_last_alloc()->rwx |= PagePerms::rw_mask;

    bool ret = emu.emu_loop(insns, prog_start);
    // Do the execution.
  } catch (std::exception &e) {
    std::cerr << "EX: " << e.what() << std::endl;
  }
  return 0;
}
