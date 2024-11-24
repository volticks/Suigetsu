#pragma once
// The MN103 has a 4gb addr space, so we are making this with that in mind.
// Probably gonna have 3 levels of paging, as thats all thats prolly necessary
// for a 32bit addr space.

#include <cstdint>
#include <memory>
#include <string>
#include <vector>
typedef uint32_t virt_addr;
typedef uint64_t phys_addr;
// Encompasses phys and virt addr
typedef uint64_t addr;
typedef uint8_t byte;
typedef int bit_t;

// Gets only the 9 bits of the page index
const uint32_t page_idx_mask = 0x1ff;
const uint32_t page_idx_sz = 9; // bits
const uint32_t page_shift = 12;
const uint32_t paging_levels = 3;
const uint32_t page_size = 0x1000;

// Represents a page table entry in our MMU, very lightweight version of an
// actual page table used on x86. May contain the actual physical address of the
// page OR simply an offset to a directory entry.
struct page_entry {
  // Read/write/execute
  bit_t rwx : 3;
  // Have we touched this page yet?
  bit_t modified : 1;
  // Have we read from this page yet
  bit_t read : 1;
  // Are we actually using this? If not, should page fault on access MAYBE.
  bit_t present : 1;
  // Can either be directory offset or phys addr mapped to virt addr.
  //
  // Real mem addresses wont use more than 6 bytes, leaving us with max 48
  /// bits.
  // Take 12 for the page shift and we only need 36 to store real physical
  // addresses.
  addr page_addr : 36;

  page_entry(addr pa);
  page_entry();
} __attribute__((packed, aligned(1))) typedef page;

struct page_dir_entry : page {
  // Directory address of this entry, shifted right by 12 since we are page
  // aligned, dont rlly need to care.
  virt_addr dir_addr : 20;

  // Struct constructor for pte
  page_dir_entry();
} typedef page_dir_entry;

class PageDirectory {
public:
  // Needs to reserve space on the vector
  PageDirectory();
  // Needs to free up all our pte's
  ~PageDirectory();
  // Add a virtual page into our page table, mapping it to the corresponding
  // physical addr
  bool add_page(virt_addr v_page);
  page_entry &get_pte_from_vaddr(virt_addr vaddr);

  phys_addr alloc_page();
  // Wrapper around emplacing into the page entry vec
  void add_page_entry(uint32_t idx, addr pa);
  // Just a getter lol
  page_entry &get_page_entry(uint32_t idx);

private:
  // Our page table entries, index into here using the page directory index
  // First level is Page Lower Directory, storing indices into the pmd
  // std::vector<page_entry> pld;
  // Second is page middle directory, storing indices into the pte
  // std::vector<page_entry> pmd;
  // Finally, Page Table Entries contains actual "real" physical mappings
  //
  // Disregard above, this performs all these functions by itself
  std::vector<page_entry> page_entries;

  // Within page_entries, we store entries for the pld, pmd and pte, so there
  // needs to be partition points for each section probably.
  uint32_t pte_part = 0;
  uint32_t pmd_part = 0;
  uint32_t pld_part = 0;
};

// TODO something special with this. Maybe sending a "signal" of sorts to the
// emulated code.
class PageException : public std::exception {
public:
  PageException(char *newMsg) : msg(newMsg) {
    std::snprintf(msg.get(), 1024, "%s", newMsg);
  }
  PageException(char *newMsg, addr vaddr) {
    std::snprintf(msg.get(), 1024, "%s 0x%lx", newMsg, vaddr);
  }
  // Only returning data() here to silence compiler warning
  char *what() { return msg.get(); }

private:
  // Should hopefully manage itself properly
  std::unique_ptr<char[]> msg = std::make_unique<char[]>(1024);
};

class MMU {
public:
  // Reading and writing to virtual addresses
  byte read(virt_addr addr);
  void write(virt_addr addr, byte val);
  // TODO: may end up writing more here with different read/write granularity,

private:
  std::vector<page_entry> page_directory;
};
