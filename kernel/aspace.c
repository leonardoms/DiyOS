#include <kernel.h>

aspace_t*
aspace_create() {
    aspace_t* a;

    a = (aspace_t*)malloc(sizeof(struct aspace));
    if(a == NULL)
        return NULL;

    memset(a,0,sizeof(struct aspace));

    a->magic = 0x10C00FFE;

    return a;
}

uint8_t
aspace_section_create(aspace_t* as, uint32_t addr, uint32_t size, uint32_t flags,
                   uint32_t offset) {
    sect_t*  s;
    // sect_t*  old_s;

    //TODO: replace with krealloc
    s = (sect_t*)malloc((as->sects + 1) * sizeof(struct sect));
    if(s == NULL)
      return TRUE;

    // old_s = as->sect;

    if(as->sect != NULL) {
      memcpy(as->sect, s, sizeof(struct sect) * as->sects);
      // kfree(as->sect);
    }

    // if(old_s)
      // kfree(old_s);

    as->sect = s;

    s[as->sects].addr = addr;
    s[as->sects].size = size;
    s[as->sects].flags = flags;
    s[as->sects].offset = offset;

    as->sects++;

    return FALSE;
}
