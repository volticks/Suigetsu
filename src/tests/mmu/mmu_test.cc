#include "../../mmu.h"
#include <cstdint>
// Unit testing shenanigans, compile seperately

int main() {

  PageDirectory pd;

  uint64_t virtual_addr = 0xfffff000;
  pd.add_page(virtual_addr);
  page_entry &pe = pd.get_pte_from_vaddr(virtual_addr);
  printf("Physical addr: 0x%lx\n", pe.page_addr << 12);
  return 0;
}
