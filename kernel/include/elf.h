#ifndef _X86_ELF_H
#define _X86_ELF_H

#include <ctypes.h>
#include <debug/bochs.h>
#include <small.h>

#define	ELF_SIGNATURE	0x464C457F	// Assinatura ELF
#define	ELF_CLASS32	1	// 32 bits
#define	ET_REL		1	// Arquivo ELF Relocatable
#define	ET_EXEC		2	// Arquivo ELF Executavel
#define	ET_DYN		3	// Arquivo ELF Dinamico
#define	EM_386		3	// ELF para arquitetura intel

#define	SHN_UNDEF	0
#define	SHN_ABS		0xFFF1
#define	SHN_COMMON	0xFFF2
#define	SHN_SOMBRA	0xABCD

// Section Header Type
#define	SHT_PROGBITS	1
#define	SHT_SYMTAB	2
#define	SHT_STRTAB	3
#define	SHT_REL		4
#define	SHT_NOBITS	8
#define	SHT_RELA	9

// Section Header Flags
#define	SHF_WRITE	0x1
#define	SHF_ALLOC	0x2
#define	SHF_EXEC	0x4
#define	SHF_MASKPROC	0xF0000000

// Symbols Type
#define	STT_NOTYPE	0
#define	STT_OBJECT	1
#define	STT_FUNC	2
#define	STT_SECTION	3
#define	STT_FILE	4


// status da relocaliza��o (n�o padr�o).
#define	RELOC_OK		0	// Sem erros
#define	RELOC_UNDEF_SYMBOL	1	// S�mbolo indefinido. (sugest�o: definir e tentar relocalizar novamente)
#define	RELOC_ERROR_SYMTAB	2	// Symbol Table n�o � v�lida.
#define	RELOC_ERROR_TARGET	3	// Se��o alvo n�o � v�lida.
#define	RELOC_ERROR_SYMBOL	4	// S�mbolo n�o existe.
#define	RELOC_ERROR_MEM		5	// Mem�ria insuficiente.
#define	RELOC_ERROR		6	// Erro generico
#define	RELOC_COMMON		7	// COMMON SIMBOL n�o alocado.

#pragma pack(1)
typedef struct
{
	uint32_t magic;
	uint8_t bitness;
	uint8_t endian;
	uint8_t elf_ver_1;
	uint8_t res[9];
	uint16_t file_type;
	uint16_t machine;
	uint32_t elf_ver_2;
	uint32_t entry;
	uint32_t phtab_offset;
	uint32_t shtab_offset;
	uint32_t flags;
	uint16_t file_hdr_size;
	uint16_t ph_size;
	uint16_t num_ph;
	uint16_t sh_size;
	uint16_t num_sects;
	uint16_t shstrtab_index;
} elf_file_t;

#pragma pack(1)
typedef struct
{
	uint32_t sect_name;
	uint32_t type;
	uint32_t flags;
	uint32_t virt_adr;
	uint32_t offset;
	uint32_t size;
	uint32_t link;
	uint32_t info;
	uint32_t align;
	uint32_t ent_size;
} elf_sect_t;

#pragma pack(1)
typedef struct
{
	uint32_t adr;
	uint8_t type;
/* 16-bit Turbo C sez: "Bitfields must be signed or unsigned int" */
//	uint32_t symtab_index : 24;
	unsigned symtab_index : 24;
	uint32_t addend;
} elf_reloc_t;

#pragma pack(1)
typedef struct
{
	uint32_t name;
	uint32_t value;
	uint32_t size;
	unsigned type : 4;
	unsigned binding : 4;
	uint8_t zero;
	uint16_t section;
} elf_sym_t;

#pragma pack(1)
typedef struct
{
	uint32_t type;
	uint32_t offset;
	uint32_t virt_adr;
	uint32_t phys_adr;
	uint32_t disk_size;
	uint32_t mem_size;
	uint32_t flags;
	uint32_t align;
} elf_seg_t; /* 32 bytes */

int32_t load_elf_exec(char *image, unsigned *entry/*, aspace_t *as*/);
uint8_t	elf32_check(void* image, uint8_t type);
uint8_t	elf32_get_section_by_name(void* image, char* sect_name, elf_sect_t** sect);
int32_t	elf32_get_sections(void* image, uint32_t type, int32_t flags);
uint8_t	elf32_get_sections_i(void* image, uint32_t index, uint32_t type, int32_t flags, elf_sect_t** sect);
uint8_t	elf32_get_section_i(void* image, uint32_t index, elf_sect_t** sect);
uint8_t	elf32_get_symbol_i(void* image, uint32_t sym_idx, elf_sect_t* symtab, elf_sym_t** symbol);
uint8_t	elf32_get_symbol_by_name(void* image, const char* sym_name, int32_t type, elf_sect_t* symtab, elf_sect_t* strtab, elf_sym_t** symbol);
uint8_t	elf32_search_symbol(void* image, const char* name, uint32_t type, elf_sym_t** symbol);
uint8_t	elf32_get_name(void* image, uint32_t index, elf_sect_t* strtab, const char** name);
uint8_t	elf32_reloc_apply(void* image, elf_reloc_t* rel, uint32_t symtab_idx, uint32_t t_sect_idx);

#endif
