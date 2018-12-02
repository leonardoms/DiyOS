#ifndef __ASPACE_H__
#define __ASPACE_H__

#include <ctypes.h>
#include <arch.h>

#define	SF_ZERO		0x10	// BSS - zerar antes de usar
#define	SF_LOAD		0x08	// Lido de um arquivo.
#define	SF_READ		0x04	// Leitura
#define	SF_WRITE	0x02	// Escrita
#define	SF_EXEC		0x01	// Executável
#define	SF_BSS	(SF_ZERO | SF_READ | SF_WRITE)

typedef struct sect {
	uint32_t   addr, size, flags;
	uint32_t   offset;
} sect_t;

// allocated space for a task
typedef struct aspace {
  uint32_t   magic;
	uint8_t*   addr;
	// uint32_t virt_adr;	// Menor valor do endereço virtual da tarefa.
	uint32_t   size;		// Tamanho total da tarefa.

	uint32_t   sects;
	sect_t*    sect;
	sect_t*    sect_phys;
	sect_t     heap;
} aspace_t;

uint8_t
aspace_section_create(aspace_t* as, uint32_t addr, uint32_t size, uint32_t flags,
                   uint32_t offset);

aspace_t*
aspace_create();

#endif
