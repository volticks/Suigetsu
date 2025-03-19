#include "mmu.h"
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>

page_entry::page_entry(addr pa) {
  // Make SURE everything is 0 BEFORE
  this->rwx = 0;
  this->present = 0;

  this->present |= 1;
  // Read, write, nx for now
  this->rwx |= PagePerms::rw_mask;
  this->read = 0;
  this->modified = 0;
  // To be changed by the instantiator
  this->page_addr = (pa >> 12);
}

page_entry::page_entry(addr pa, uint32_t perms) : page_entry(pa) {
  this->rwx = perms;
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
  // this->page_entries.resize((page_idx_mask * 5) + 1);

  memset(this->pld_ents, 0, sizeof(this->pld_ents));
  this->pte_part = page_idx_mask * 3;
  this->pmd_part = page_idx_mask * 1;
  this->pld_part = page_idx_mask * 0;
  this->last_allocated = NULL;
}

// Simply hacky thing to suppress freeing msgs
const bool do_mem_log = false;
PageDirectory::~PageDirectory() {
  for (auto page : this->pages) {
    std::free((void *)page);
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
  this->pages.emplace_back((phys_addr)page);
  std::memset(page, 0, page_size);

  return (phys_addr)page;
}

void PageDirectory::add_page_entry(uint32_t idx, addr pa, uint32_t perms) {
  if (idx >= this->page_entries.size())
    return; // Dont forget size check lol
  // this->page_entries.emplace(this->page_entries.begin() + idx, pa);
  this->page_entries[idx] = page_entry(pa, perms);
}

page_entry &PageDirectory::get_page_entry(uint32_t idx) {
  // Lazy assert, should prolly be an exception but this will do for now.
  assert(idx < this->page_entries.size());
  return this->page_entries[idx];
}

// Returns true: one or many layers of paging already had entries corresponding
// to parts of our virtual address. False: This is completely new.
bool PageDirectory::add_page(virt_addr v_page,
                             uint32_t perms = PagePerms::rw_mask) {

  std::cout << "V page: " << std::hex << v_page << std::endl;
  bool ret = false;
  // Need to add page to pte, pmd and pld
  uint32_t pte_idx = get_page_index(v_page, 0);
  uint32_t pmd_idx = get_page_index(v_page, 1);
  uint32_t pld_idx = get_page_index(v_page, 2);
  std::cout << "PLD idx: " << std::hex << pld_idx << std::endl;
  std::cout << "PMD idx: " << std::hex << pmd_idx << std::endl;
  std::cout << "PTE idx: " << std::hex << pte_idx << std::endl;

  // Testing
  page_entry *pld_e1 = &this->pld_ents[pld_idx];
  page_entry *pmd_ptr = (page_entry *)(pld_e1->page_addr << page_shift);
  if (!pld_e1->present) {
    pmd_ptr = (page_entry *)alloc_page();
    *pld_e1 = page_entry((addr)pmd_ptr, perms);
    ret = true;
  }
  page_entry *pmd_e1 = &pmd_ptr[pmd_idx];
  page_entry *pte_ptr = (page_entry *)(pmd_e1->page_addr << page_shift);
  if (!pmd_e1->present) {
    pte_ptr = (page_entry *)alloc_page();
    *pmd_e1 = page_entry((addr)pte_ptr, perms);
    ret = true;
  }

  page_entry *pte_e = &pte_ptr[pte_idx];
  phys_addr pte = (pte_e->page_addr << page_shift);
  if (!pte_e->present) {
    pte = alloc_page();
    *pte_e = page_entry((addr)pte, perms);
    ret = true;
  }
  //

  // page_entry &pld_e = this->page_entries[this->pld_part + pld_idx];
  // uint32_t pmd_part = pld_e.page_addr;
  // if (!pld_e.present) {
  //   std::cout << "PLD adding: " << std::hex << this->pld_part + pld_idx
  //             << std::endl;
  //   this->add_page_entry(this->pld_part + pld_idx, pld_idx << page_shift,
  //                        perms);
  //   ret = true;
  //   pmd_part = pld_idx;
  // }

  //// TODO: replace this with the ACTUAL pld part.
  // page_entry &pmd_e = this->page_entries[this->pmd_part + pmd_part +
  // pmd_idx]; uint32_t pte_part = pmd_e.page_addr; if (!pmd_e.present) {
  //   std::cout << "(add)PMD Entry addr: " << this->pmd_part + pmd_part +
  //   pmd_idx
  //             << std::endl;
  //   this->add_page_entry(this->pmd_part + pmd_part + pmd_idx,
  //                        pmd_idx << page_shift, perms);
  //   ret = true;
  //   pte_part = pmd_idx;
  // }

  // page_entry &pte_e = this->page_entries[this->pte_part + pte_part +
  // pte_idx];
  //// Have we already put a pte here?
  // if (!pte_e.present) {
  //   // If no pte here yet, make sure we allocate some memory first
  //   addr page = (addr)alloc_page();
  //   std::cout << "Page: 0x" << std::hex << page << std::endl;
  //   this->add_page_entry(this->pte_part + pte_part + pte_idx, (addr)page,
  //                        perms);

  //  // Store ref to last allocated page so we can recall it ez
  //  // Using idx here is fine as theres an assertion in add_page_entry that
  //  // checks it.
  //  // this->last_allocated = &this->page_entries[this->pte_part + pte_idx];
  //  ret = true;
  //}

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
  page_entry *pld_e1 = &this->pld_ents[pld_idx];
  page_entry *pmd_ptr = (page_entry *)(pld_e1->page_addr << page_shift);
  if (!pld_e1->present) {
    // Do a page fault or something, this address doesnt exist.
    throw PageException((char *)"Page fault in PLD, not present. Off:",
                        pld_idx);
  }
  page_entry *pmd_e1 = &pmd_ptr[pmd_idx];
  page_entry *pte_ptr = (page_entry *)(pmd_e1->page_addr << page_shift);
  if (!pmd_e1->present) {
    // Do a page fault or something, this address doesnt exist.
    throw PageException((char *)"Page fault in PMD, not present. Off:",
                        pmd_idx);
  }

  page_entry *pte_e = &pte_ptr[pte_idx];
  phys_addr pte = (pte_e->page_addr << page_shift);
  if (!pte_e->present) {
    // Do a page fault or something, this address doesnt exist.
    throw PageException((char *)"Page fault in PTE, not present. Off:",
                        pte_idx);
  }

  // uint32_t pmd_part = pld_e.page_addr;
  // uint32_t pmd_idx_full = this->pmd_part + pmd_part + pmd_idx;
  // std::cout << "PMD Entry addr: " << pmd_idx_full << std::endl;
  // page_entry &pmd_e = this->get_page_entry(pmd_idx_full);
  // if (!pmd_e.present) {
  //   // Do a page fault or something, this address doesnt exist.
  //   throw PageException((char *)"Page fault in PMD, not present. Off:",
  //                       pmd_idx_full);
  // }

  // uint32_t pte_part = pmd_e.page_addr;
  // uint32_t pte_idx_full = this->pte_part + pte_part + pte_idx;
  // std::cout << "PTE Entry addr: " << pmd_e.page_addr << std::endl;
  // page_entry &pte_e = this->get_page_entry(pte_idx_full);
  // if (!pte_e.present) {
  //   // Do a page fault or something, this address doesnt exist.
  //   throw PageException((char *)"Page fault in PTE, not present. Off:",
  //                       pte_idx);
  // }

  // std::cout << "Entry addr: " << pte_e.page_addr << std::endl;
  //  If all present, we assume we ok to return entry
  return *pte_e;
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

    if (!this->page_directory.add_page(start + (page_size * i), rwx)) {
      throw PageException(
          (char *)"MMU::map_range add_page failed failed, addr:",
          start + (page_size * i));
    }

    // page_entry *curr = this->get_pd().get_last_alloc();
    // curr->rwx |= rwx;
  }
}

void MMU::write_many(virt_addr start, byte *data, uint32_t num) {
  // If not page aligned, throw
  if (start & (page_size - 1))
    throw PageException(
        (char *)"MMU::write_many start address not page aligned, addr: ",
        start);
  // For convenience here we ignore page perms.
  uint32_t n = 0;
  virt_addr curr = start;
  while (num > 0) {
    // We just gonna memcpy the data page by page
    n = std::min(page_size, num);
    page_entry &pe = this->get_pd().get_pte_from_vaddr(curr);

    std::memcpy((void *)(pe.page_addr << page_shift), data, n);
    curr += n;
    data += n;

    num -= n;
  }
}

void MMU::log_many(virt_addr start, uint32_t num) {
  // For convenience here we ignore page perms.
  uint32_t n = 0;
  virt_addr curr = start;
  while (num > 0) {
    // We just gonna memcpy the data page by page
    n = std::min(page_size, num);
    page_entry &pe = this->get_pd().get_pte_from_vaddr(curr);
    uint32_t start_off = (start & (page_size - 1)) / 4;
    uint32_t *addr = (uint32_t *)(pe.page_addr << page_shift) + start_off;

    for (int i = 0; i < num; i++) {
      printf("0x%08llx // 0x%08x : 0x%08x\n", addr + i, start + (i * 4),
             addr[i]);
    }

    curr += n;
    num -= n;
    start = 0;
  }
}

bool MMU::is_rx(const page_entry &pe) {
  return pe.present && (pe.rwx & (PagePerms::rx_mask)) == PagePerms::rx_mask;
}

bool MMU::is_rx(virt_addr va) {
  const page_entry &pe = this->get_pd().get_pte_from_vaddr(va);
  return is_rx(pe);
}
