#include "mmu.h"
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>

page_entry::page_entry(addr pa) {
  this->present |= 1;
  // Read, write, nx for now
  this->rwx |= 0b110;
  this->read |= 0;
  this->modified |= 0;
  // To be changed by the instantiator
  this->page_addr = (pa >> 12);
}

// Default
page_entry::page_entry() {
  this->present = 0;
  this->rwx = 0;
  this->read = 0;
  this->modified = 0;
  this->page_addr = 0;
}

PageDirectory::PageDirectory() {
  // Pre-allocate entries, may be more efficient, depends on memory access usage
  // ig.
  //
  // *3 since we wanna store entries for pte, pmd, and pld. Max index is
  // page_idx_mask, so just multiply that by 3 for now.
  this->page_entries.resize((page_idx_mask * 3) + 1);
  this->pte_part = page_idx_mask * 2;
  this->pmd_part = page_idx_mask * 1;
  this->pld_part = page_idx_mask * 0;
  this->last_allocated = NULL;
}

PageDirectory::~PageDirectory() {
  for (uint32_t pte_idx = 1; pte_idx <= page_idx_mask; pte_idx++) {
    if (this->page_entries[this->pte_part + pte_idx].page_addr == 0)
      continue;
    void *page_addr =
        (void *)(this->page_entries[this->pte_part + pte_idx].page_addr
                 << page_shift);
    std::cout << "Off: " << std::hex << this->pte_part + pte_idx << std::endl;
    std::cout << "Freeing page addr: 0x" << std::hex << page_addr << std::endl;
    std::free(page_addr);
  }
}

static uint32_t get_page_index(virt_addr v_page, byte level) {
  // Depending on level of index desired, we wanna change the shift
  assert(level <= paging_levels);
  return v_page >> (page_shift + (level * page_idx_sz)) & page_idx_mask;
}

// Offset into the physical page our virtual addr has.
static uint32_t get_page_offset(virt_addr v_addr) { return v_addr & 0xfff; }

// TODO: We dont free any of this yet, fix that or we WILL have memory leaks.
// Also probably could be static. Also we assume aligned_alloc will be nice and
// actually return us the alignment that we want, so *maybe* check that in
// future?
phys_addr PageDirectory::alloc_page() {
  void *page = std::aligned_alloc(page_size, page_size);
  if (page == nullptr) {
    throw PageException(
        (char *)"PageDirectory::alloc_page returned null in allocation.");
  }
  std::memset(page, 0, page_size);

  return (phys_addr)page;
}

void PageDirectory::add_page_entry(uint32_t idx, addr pa) {
  if (idx >= this->page_entries.size())
    return; // Dont forget size check lol
  // this->page_entries.emplace(this->page_entries.begin() + idx, pa);
  this->page_entries[idx] = page_entry(pa);
  // Store ref to last allocated page so we can recall it ez
  this->last_allocated = &this->page_entries[idx];
}

page_entry &PageDirectory::get_page_entry(uint32_t idx) {
  // Lazy assert, should prolly be an exception but this will do for now.
  assert(idx < this->page_entries.size());
  return this->page_entries[idx];
}

// Returns true: one or many layers of paging already had entries corresponding
// to parts of our virtual address. False: This is completely new.
bool PageDirectory::add_page(virt_addr v_page) {

  bool ret = false;
  // Need to add page to pte, pmd and pld
  uint32_t pte_idx = get_page_index(v_page, 0);
  uint32_t pmd_idx = get_page_index(v_page, 1);
  uint32_t pld_idx = get_page_index(v_page, 2);
  std::cout << "PLD idx: " << std::hex << pld_idx << std::endl;
  std::cout << "PMD idx: " << std::hex << pmd_idx << std::endl;
  std::cout << "PTE idx: " << std::hex << pte_idx << std::endl;

  // Have we already put a pte here?
  if (!this->page_entries[this->pte_part + pte_idx].present) {
    // If no pte here yet, make sure we allocate some memory first
    addr page = (addr)alloc_page();
    std::cout << "Page: 0x" << std::hex << page << std::endl;
    this->add_page_entry(this->pte_part + pte_idx, (addr)page);
    ret = true;
  }

  if (!this->page_entries[this->pmd_part + pmd_idx].present) {
    std::cout << "(add)PMD idx: " << this->pmd_part + pmd_idx << std::endl;
    this->add_page_entry(this->pmd_part + pmd_idx,
                         this->pte_part << page_shift);
    ret = true;
  }

  if (!this->page_entries[this->pld_part + pld_idx].present) {
    std::cout << "PLD adding: " << std::hex << this->pld_part + pld_idx
              << std::endl;
    this->add_page_entry(this->pld_part + pld_idx,
                         this->pmd_part << page_shift);
    ret = true;
  }
  return ret;
}

page_entry &PageDirectory::get_pte_from_vaddr(virt_addr vaddr) {
  // TODO: Remember to actually do stuff here and not just doing maths to get
  // the page table idx lol.

  // Need to add page to pte, pmd and pld
  uint32_t pte_idx = get_page_index(vaddr, 0);
  uint32_t pmd_idx = get_page_index(vaddr, 1);
  uint32_t pld_idx = get_page_index(vaddr, 2);

  std::cout << "PLD idx: " << pld_idx << std::endl;
  std::cout << "PMD idx: " << pmd_idx << std::endl;
  std::cout << "PTE idx: " << pte_idx << std::endl;

  // Now we have IDXs, navigate the layers of paging
  page_entry &pld_e = this->get_page_entry(this->pld_part + pld_idx);
  if (!pld_e.present) {
    // Do a page fault or something, this address doesnt exist.
    throw PageException((char *)"Page fault in PLD, not present. Off:",
                        pld_idx);
  }

  std::cout << "PMD Entry addr: " << pld_e.page_addr + pmd_idx << std::endl;
  uint32_t pmd_part = pld_e.page_addr;
  page_entry &pmd_e = this->get_page_entry(pmd_part + pmd_idx);
  if (!pmd_e.present) {
    // Do a page fault or something, this address doesnt exist.
    throw PageException((char *)"Page fault in PMD, not present. Off:",
                        pte_idx);
  }

  std::cout << "PTE Entry addr: " << pmd_e.page_addr << std::endl;
  uint32_t pte_part = pmd_e.page_addr;
  page_entry &pte_e = this->get_page_entry(pte_part + pte_idx);
  if (!pte_e.present) {
    // Do a page fault or something, this address doesnt exist.
    throw PageException((char *)"Page fault in PTE, not present. Off:",
                        pte_idx);
  }

  std::cout << "Entry addr: " << pte_e.page_addr << std::endl;
  // If all present, we assume we ok to return entry
  return pte_e;
}
page_entry *PageDirectory::get_last_alloc() { return this->last_allocated; }

PageDirectory &MMU::get_pd() { return this->page_directory; }

page_range::page_range(virt_addr start, uint32_t num_pages, bit_t rwx) {

  // If we would overflow the address space, throw
  if (start + (num_pages * page_size) > max_vaddr || num_pages > max_pages) {
    throw PageException((char *)"Too many pages. Num:", num_pages);
  }
  if (start & (page_size - 1)) {
    throw PageException((char *)"Not page aligned. Addr:", start);
  }

  this->vma_start = start;
  this->rwx = rwx;
  this->num_pages = num_pages;
}

void MMU::map_range(virt_addr start, uint32_t num, byte rwx) {
  page_range pr(start, num, rwx);

  for (int i = 0; i < num; i++) {

    if (!this->page_directory.add_page(start + (page_size * i))) {
      throw PageException(
          (char *)"MMU::map_range add_page failed failed, addr:",
          start + (page_size * i));
    }

    page_entry *curr = this->get_pd().get_last_alloc();
    curr->rwx = rwx;
  }
}
