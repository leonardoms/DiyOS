#include <ctypes.h>
#include <elf.h>
#include <aspace.h>
#include <task.h>

typedef void  (*module_init_callback_t)();

typedef struct module {
  char  name[64];



  struct module   *next;
} module_t;

void
module_load(const char* name, uint8_t* elf) {
    elf_sym_t* s[1];
    elf_sect_t* sect[1];
    module_init_callback_t c;

    printf("[M] %s: ", name);

    if(elf32_check(elf, ET_DYN) == FALSE) {
        printf("invalid MO file.\n");
        return ;
    }

    if( elf32_search_symbol(elf, "init", -1, s) ) {
        printf("init @ 0x%x ", (uint32_t)elf + s[0]->value);
        c = (module_init_callback_t)((uint32_t)elf + s[0]->value);
        // c();
    }
    if( elf32_search_symbol(elf, "foo", -1, s) ) {
      if(elf32_get_section_i(elf,s[0]->section, sect)) {
        printf("foo (0x%x): %s ", (uint32_t)(sect[0]->offset + s[0]->value + (uint32_t)elf), (uint8_t*)(sect[0]->offset + s[0]->value + (uint32_t)elf));
      }
    }

    printf("loaded.\n");
}
