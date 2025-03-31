#include "emulator.h"
#include "mmu.h"
#include <cstdio>
#include <fstream>
#include <iostream>
#include <memory>
#include <vector>

// Should be able to abstract any file related stuff in here, *maybe*
namespace FileReader {

std::vector<uint8_t> read_file(const char *fname, virt_addr off) {
  std::ifstream ifs(fname);
  ifs.seekg(0, std::ios::end);
  int32_t sz = ifs.tellg();

  if (sz < 0) {
    std::cerr << "FileReader::read_file failed to open, does it exist?"
              << std::endl;
    exit(-1);
  }

  if (off < sz)
    sz -= off;
  else
    off = 0;

  // Off when unspecified is 0
  ifs.seekg(off, std::ios::beg);
  // TODO: This check is really really stupid
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
    std::cerr << "Optional: <program map start> <entrypoint> <load offset> "
                 "<text section end>"
              << std::endl;
    std::cerr << "\t<program map start>\t: Virtual address where program "
                 "should be loaded."
              << std::endl;
    std::cerr << "\t<entrypoint>\t\t: PC entrypoint." << std::endl;
    std::cerr
        << "\t<load offset>\t\t: Offset into the file to start loading the "
           "program."
        << std::endl;

    std::cerr << "\t<text section end>\t\t: Specified end of the text section. "
                 "If specified will add null byte padding to act as BSS "
                 "section coming after text section."
              << std::endl;
    std::cerr << "Default for all is 0" << std::endl;

    return 1;
  }

  virt_addr prog_start = 0x0;
  virt_addr entry = 0x0;
  // Offset into the file we wanna load from, for example if .text in ELF starts
  // at 0x1000 we can load at that addr.
  virt_addr load_start_off = 0x0;
  // Some test cases showed that gcc will put variables directly after the end
  // of the text section (bss), so we need to account for that by adding space
  // after the end until the next section. This is where you can specify the
  // offset of this padding -- basically end of the text section.
  virt_addr text_seg_sep = 0x0;
  const uint32_t npad = 0x1000;
  if (argc >= 3) {
    // Returns 0 on fail apparently so alls well
    // TODO: Should prolly abort if it doesnt turn into
    // valid addr.
    prog_start = std::atoi(argv[2]);
  }
  if (argc >= 4) {
    entry = std::atoi(argv[3]);
  }
  if (argc >= 5) {
    load_start_off = std::atoi(argv[4]);
  }
  if (argc >= 6) {
    text_seg_sep = std::atoi(argv[5]);
  }

  std::cout << "main: Entry -> " << std::hex << entry << std::endl;
  std::cout << "main: Mapping start -> " << std::hex << prog_start << std::endl;

  try {
    Instructions insns = FileReader::read_file(argv[1], load_start_off);

    if (text_seg_sep != 0) {
      // Insert padding after text section
      insns.insert(insns.begin() + text_seg_sep, npad, 0);
    }

    Emulator emu;
    // Setup some emu resources
    // Memory
    // Init: Need to map space for the program code starting at configurable
    // address, then write trhe instructions to that location,
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
    // emu.get_mmu().get_pd().add_page(0x41414000, PagePerms::rwx_mask);
    // Constructor for page_entry already sets these up, but just to make sure.
    // emu.get_mmu().get_pd().get_last_alloc()->rwx |= PagePerms::rw_mask;

    bool ret = emu.emu_loop(insns, entry);
    // For testing
    emu.get_regs().dump_regs();
    // Do the execution.
  } catch (std::exception &e) {
    std::cerr << "EX: " << e.what() << std::endl;
  }
  return 0;
}
