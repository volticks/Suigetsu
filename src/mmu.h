#pragma once
// The MN103 has a 4gb addr space, so we are making this with that in mind.
// Probably gonna have 3 levels of paging, as thats all thats prolly necessary
// for a 32bit addr space.

#include "instruction.h"
#include <arpa/inet.h>
#include <cstdint>
#include <iostream>
#include <memory>
#include <ostream>
#include <string>
#include <vector>
typedef uint32_t virt_addr;
typedef uint64_t phys_addr;
// Encompasses phys and virt addr
typedef uint64_t addr;
typedef uint8_t byte;
typedef uint8_t bit_t;

// Gets only the 9 bits of the page index
const uint32_t page_idx_mask = 0x1ff;
const uint32_t page_idx_sz = 9; // bits
const uint32_t page_shift = 12;
const uint32_t paging_levels = 3;
const uint32_t page_size = 0x1000;
const uint32_t page_mask = 0xf000;
const uint32_t max_vaddr = 0xffffffff;
const uint32_t max_pages = max_vaddr / page_size;
// If we go oob when decoding an instruction
const uint32_t decoding_deadzone = max_ins;

namespace PagePerms {
const uint32_t rd_mask = 0b100;
const uint32_t wr_mask = 0b010;
const uint32_t ex_mask = 0b001;
const uint32_t rwx_mask = 0b111;
const uint32_t rw_mask = 0b110;
const uint32_t rx_mask = 0b101;
} // namespace PagePerms

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
  page_entry(addr pa, uint32_t perms);
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
  bool add_page(virt_addr v_page, uint32_t perms);
  page_entry &get_pte_from_vaddr(virt_addr vaddr);

  phys_addr alloc_page();
  // Wrapper around emplacing into the page entry vec
  void add_page_entry(uint32_t idx, addr pa, uint32_t perms);
  // Just a getter lol
  page_entry &get_page_entry(uint32_t idx);

  // Returns ptr to last allocated page
  page_entry *get_last_alloc();

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
  // Used for tracking and freeing of pages after we are done with them
  std::vector<phys_addr> pages;
  // Additional entries will be allocated and tagged on from here
  page_entry pld_ents[page_idx_mask];
  // Store the last page allocated for easy access.
  page_entry *last_allocated;

  // Within page_entries, we store entries for the pld, pmd and pte, so there
  // needs to be partition points for each section probably.
  uint32_t pte_part = 0;
  uint32_t pmd_part = 0;
  uint32_t pld_part = 0;
};

// Defines a range of pages to be mapped and their perms
struct page_range {
  virt_addr vma_start = 0;
  // Number of pages to be mapped
  uint32_t num_pages = 0;
  bit_t rwx : 3 = 0;
  page_range(virt_addr start, uint32_t num_pages, bit_t rwx);
};

// TODO something special with this. Maybe sending a "signal" of sorts to the
// emulated code.
class PageException : public std::exception {
public:
  PageException(char *newMsg) /*: msg(newMsg)*/ {
    std::snprintf(msg.get(), 1024, "%s", newMsg);
  }
  PageException(char *newMsg, addr vaddr) {
    std::snprintf(msg.get(), 1024, "%s 0x%lx", newMsg, vaddr);
  }
  // Only returning data() here to silence compiler warning
  const char *what() const noexcept { return msg.get(); }

private:
  // Should hopefully manage itself properly
  std::unique_ptr<char[]> msg = std::make_unique<char[]>(1024);
};

class MMU {
public:
  // Reading and writing to virtual addresses
  // Because these are templated, must be visible to compiler, so prolly need to
  // be in the header file Ugly, i know.
  // TODO: Writing and reading on boundaries?
  template <typename T> T read(virt_addr addr) {
    T val = 0;

    // Variables for if we would overflow the page and need to resume the read
    // on another
    bool split = 0;
    page_entry *pe_split = NULL;

    std::cout << "MMU::read reading: " << std::hex << addr << std::endl;

    // We would go over to the next page when reading
    if (((addr + sizeof(T) - 1) & page_mask) != (addr & page_mask)) {
      // If we do, need to check that this page is also present and has good
      // perms
      std::cout << "Would overflow page" << std::endl;
      pe_split = &this->page_directory.get_pte_from_vaddr(addr + sizeof(T));

      if (!pe_split->present || !(pe_split->rwx & PagePerms::rd_mask))
        throw PageException(
            (char *)"SEGFAULT. Page not present or not readable, vaddr:", addr);

      split = true;
    }

    const page_entry &pe = this->page_directory.get_pte_from_vaddr(addr);

    if (!pe.present || !(pe.rwx & PagePerms::rd_mask))
      throw PageException(
          (char *)"SEGFAULT. Page not present or not readable, vaddr:", addr);

    std::cout << "MMU::read Page address: " << std::hex << pe.page_addr
              << std::endl;

    // Get the page offset and read the val
    uint32_t page_off = addr & 0xfff;

    if (!split) {
      val = *(T *)((pe.page_addr << page_shift) + page_off);
    } else {
      // Need to split the read across multiple pages, quite slow
      uint32_t split_point = sizeof(T) - ((addr + sizeof(T) - 1) & 0xf);
      byte *curr = (byte *)((pe.page_addr << page_shift) + page_off);
      for (int i = 0; i < sizeof(T); i++) {
        if (i == split_point - 1) {
          curr = (byte *)((pe_split->page_addr << page_shift));
        }
        val <<= 8;
        val += *curr;
        curr++;
      }
      // Flip endianess
      if (sizeof(T) > 4)
        val = ((uint64_t)htonl(val & 0xffffffff) << 32) | (htonl(val >> 32));
      else
        val = htonl(val);
    }

    return val;
  }
  template <typename T> void write(virt_addr addr, T val) {

    bool split = 0;
    page_entry *pe_split = NULL;

    std::cout << "MMU::write writing: " << std::hex << val << " --> "
              << std::hex << addr << std::endl;
    // We would go over to the next page when writing
    if (((addr + sizeof(T) - 1) & page_mask) != (addr & page_mask)) {
      // If we do, need to check that this page is also present and has good
      // perms
      pe_split = &this->page_directory.get_pte_from_vaddr(addr + sizeof(T));

      if (!pe_split->present || !(pe_split->rwx & PagePerms::wr_mask))
        throw PageException(
            (char *)"SEGFAULT. Page not present or not writable, vaddr:", addr);
      split = true;
    }

    const page_entry &pe = this->page_directory.get_pte_from_vaddr(addr);
    if (!pe.present || !(pe.rwx & PagePerms::wr_mask))
      throw PageException(
          (char *)"SEGFAULT. Page not present or not writable, vaddr:", addr);

    std::cout << "MMU::write Page address: " << std::hex << pe.page_addr
              << std::endl;

    // Get the page offset and write the val
    uint32_t page_off = addr & 0xfff;

    if (!split) {
      *(T *)((pe.page_addr << page_shift) + page_off) = val;
    } else {
      // Flip endianess
      // if (sizeof(T) > 4)
      //  val = ((uint64_t)htonl(val & 0xffffffff) << 32) | (htonl(val >> 32));
      // else
      //  val = htonl(val);
      std::cout << "val again: " << val << std::endl;
      // Need to split the read across multiple pages, quite slow
      uint32_t split_point = sizeof(T) - ((addr + sizeof(T) - 1) & 0xf);
      byte *curr = (byte *)((pe.page_addr << page_shift) + page_off);
      for (int i = 0; i < sizeof(T); i++) {
        if (i == split_point - 1) {
          curr = (byte *)((pe_split->page_addr << page_shift));
        }
        *curr = val & 0xff;
        val >>= 8;
        curr++;
      }
    }
  }

  PageDirectory &get_pd();

  void map_range(virt_addr start, uint32_t num, byte rwx);

  void write_many(virt_addr start, const byte *data, uint32_t num);
  // Logs num dwords from start virt addr.
  void log_many(virt_addr start, uint32_t num);

  bool is_rx(virt_addr va);
  bool is_rx(const page_entry &pe);

private:
  PageDirectory page_directory;
};
