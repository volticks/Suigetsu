#include "../../mmu.h"
#include <cstdint>
#include <iostream>
#include <ostream>
// Unit testing shenanigans, compile seperately

int main() {

  MMU unit;
  PageDirectory &pd = unit.get_pd();

  std::cout << "Sizeof(page_entry) -> " << sizeof(page_entry) << std::endl;
  uint64_t virtual_addr = 0x7ffff000;
  pd.add_page(virtual_addr, PagePerms::rw_mask);
  std::cout << "GET------" << std::endl;
  page_entry &pe = pd.get_pte_from_vaddr(virtual_addr);
  printf("Physical addr: 0x%lx\n", pe.page_addr << 12);

  // Testing reading from end of page, works
  addr targ = virtual_addr + (page_size - 8);

  unit.write(targ, 0x4141414142);

  long val = unit.read<long>(targ);
  std::cout << "Value at " << std::hex << targ << " -> " << std::hex << val
            << std::endl;

  std::cout << "Testing cross page reads and writes..." << std::endl;

  virtual_addr = 0x1000;
  pd.add_page(virtual_addr, PagePerms::rd_mask);
  pd.add_page(virtual_addr + page_size, PagePerms::rd_mask);
  // unit.write(virtual_addr + (page_size - 5), 0x4142434445464748);
  // val = unit.read<unsigned int>(virtual_addr + (page_size - 5));
  // std::cout << "Cross page val: " << val << std::endl;

  // unit.read<unsigned int>(0x7fffcdfc);

  pd.add_page(0x14000, PagePerms::rw_mask);
  pd.add_page(0x41414000, PagePerms::rw_mask);

  page_entry &p1 = pd.get_pte_from_vaddr(0x14000);
  printf("p1 addr: %p\n", p1.page_addr << page_shift);
  page_entry &p2 = pd.get_pte_from_vaddr(0x41414000);
  printf("p2 addr: %p\n", p2.page_addr << page_shift);

  return 0;
}
