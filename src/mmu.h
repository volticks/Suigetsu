#pragma once
// The MN103 has a 4gb addr space, so we are making this with that in mind.
//

#include <cstdint>
#include <vector>
typedef uint32_t virt_addr;
typedef uint64_t *phys_addr;
typedef uint8_t byte;
typedef int bit_t;

const uint32_t page_size = 0x1000;
// Represents a page table entry in our MMU, very lightweight version of an
// actual page table used on x86.
struct page_entry {
  // Read/write/execute
  bit_t rwx : 3;
  // Have we touched this page yet?
  bit_t modified : 1;
  // Have we read from this page yet
  bit_t read : 1;
  // Are we actually using this? If not, should page fault on access MAYBE.
  bit_t present : 1;
  // Physical address of this entry, shifted right by 12 since we are page
  // aligned, dont rlly need to care.
  virt_addr phys_addr : 20;
} typedef page_entry;

class PageDirectory {
public:
  // Needs to reserve space on the vector
  PageDirectory(uint32_t num_entries);

  // Add a virtual page into our page table, mapping it to the corresponding
  // physical addr
  bool add_page(virt_addr v_page, phys_addr paddr);
  bool get_pte_from_vaddr(virt_addr vaddr);

private:
  // Our page table entries, index into here using
  std::vector<page_entry> pte;
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
