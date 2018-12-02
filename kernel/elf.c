/*****************************************************************************
**                                                                          **
**	Arquivo: elf.c	                                                    **
**	Descri��o: Suporte a 'Executable and Linking Format (ELF)'.	    **
**	Autor: Leonardo Monteiro Silva (gnomo)        			    **
**	Email: gnomo86 at gmail.com                    			    **
**	Data: 19/12/2008                                                    **
**	Notas: Este arquivo j� existia anteriormente, por�m sem	    	    **
**	assinaturas e possuindo apenas a fun��o load_elf_exec.		    **
**                                                                          **
*****************************************************************************/

#include <kernel.h>

int32_t load_elf_exec(char *image, unsigned *entry, aspace_t *as)
{
	elf_file_t *elf;
	elf_seg_t *seg;
	int i, err;

/* validate ELF headers */
	elf = (elf_file_t *)image;
	if(elf->magic != 0x464C457FL) /* "ELF" */
		return 1;
	if(elf->bitness != 1 ||		/* 1=32-bit, 2=64-bit */
		elf->endian != 1 ||	/* 1=little-endian, 2=big */
		elf->elf_ver_1 != 1 ||	/* 1=current ELF spec */
		elf->file_type != 2 ||	/* 1=reloc, 2=exec, 3=DLL */
		elf->machine != 3 ||	/* 3=i386 */
		elf->elf_ver_2 != 1)
			return 1;

/* get entry point */
	(*entry) = elf->entry;
/* seek to program headers (segments) */


	image += elf->phtab_offset;
	for(i = 0; i < elf->num_ph; i++)
	{

		seg = (elf_seg_t *)(image + elf->ph_size * i);
/* choke on 2=DYNAMIC and the forbidden 5=SHLIB segments */
		if(seg->type == 2 || seg->type == 5)
			return 0;
/* handle 1=LOAD segment */
		else if(seg->type == 1)
		{
			err = aspace_section_create(as, seg->virt_adr, seg->disk_size,
				SF_LOAD | (seg->flags & 7), seg->offset);
			if(err)
				return err;

/* if size-in-mem > size-on-disk, this segment contains the BSS */
			if(seg->mem_size > seg->disk_size)
			{

				err = aspace_section_create(as,
					seg->virt_adr + seg->disk_size,
					seg->mem_size - seg->disk_size,
					SF_ZERO | (seg->flags & 7),
					seg->offset);

				if(err)
					return err;
			}
		}
/* ignore 0=NULL, 6=PHDR, 3=INTERP, and 4=NOTE segments
		else
			nothing; */
	}

	return 0;
}

// Encontra se��o de nome 'sect_name'.
uint8_t	elf32_get_section_by_name(void* image, char* sect_name, elf_sect_t** sect)
{
	elf_file_t	*elf = (elf_file_t*)image;
	elf_sect_t	/**sect_table,*/ *str_sect;
	int8_t		*strings;
	uint32_t	i, index;

	// str_sect contem os nomes das se��es.
	str_sect = (elf_sect_t*)(image + elf->shtab_offset + (elf->sh_size * elf->shstrtab_index));
	if(str_sect->type != SHT_STRTAB)
		return 0;
	strings = (int8_t*)(str_sect->offset + image);
	//printf("%s", strings);

	// sect_table = (elf_sect_t*)(image + elf->shtab_offset);
	for(i = 0; i < elf->num_sects; i++)
	{
		// Pega o �ndice do nome da se��o atual.
		index = ((elf_sect_t*)(image + elf->shtab_offset + (elf->sh_size * i)))->sect_name;
		if(!strcmp((char*)(strings + index) , sect_name))
		{
			*sect = (elf_sect_t*)(image + elf->shtab_offset + (elf->sh_size * i));
			return 1;
		}
	}

	return 0;
}

/*
	Retorna n�mero de se��es do 'type' e 'flags' requerido.
	Se 'flags' < 0 ele ser� ignorado.
*/
int32_t	elf32_get_sections(void* image, uint32_t type, int32_t flags)
{
	elf_file_t*	elf = (elf_file_t*)image;
	elf_sect_t*	tmp;
	uint32_t	i;
	int32_t		retval = 0;

	for(i = 0; i < elf->num_sects; i++)
	{
		tmp = (elf_sect_t*)(image + elf->shtab_offset + (elf->sh_size * i));
		if( tmp->type == type)
		{
			if(flags >= 0)
			if(tmp->flags != (uint32_t)(flags))
				return 0;

			retval++;
		}
	}

	return retval;
}

/*
	Atribui � 'sect' a i-�sima ocorrencia de se��o do 'type' e 'flags' requerido.
	Se 'flags' < 0 ele ser� ignorado.
*/
uint8_t	elf32_get_sections_i(void* image, uint32_t index, uint32_t type, int32_t flags, elf_sect_t** sect)
{
	elf_file_t*	elf = (elf_file_t*)image;
	elf_sect_t*	tmp;
	uint32_t	i;

	for(i = 0; i < elf->num_sects; i++)
	{
		tmp = (elf_sect_t*)(image + elf->shtab_offset + (elf->sh_size * i));
		if( tmp->type == type)
		{
			if(flags >= 0)
			if(tmp->flags != (uint32_t)(flags))
				return 0;

			if(!index)
			{
				*sect = tmp;
				return 1;
			}
			index--;
		}
	}

	return 0;
}

uint8_t	elf32_get_section_i(void* image, uint32_t index, elf_sect_t** sect)
{
	elf_file_t*	elf = (elf_file_t*)image;
	elf_sect_t*	tmp;

	if(elf->num_sects < index)
		return 0;

	tmp = (elf_sect_t*)(image + elf->shtab_offset + elf->sh_size * index);
	if(sect != NULL)
		*sect = tmp;

	return 1;
}

uint8_t	elf32_get_symbol_i(void* image, uint32_t sym_idx, elf_sect_t* symtab, elf_sym_t** symbol)
{
	if(symtab->type != SHT_SYMTAB)
		return 0;

	if(sym_idx > symtab->size / symtab->ent_size)
		return 0;

	*symbol = (elf_sym_t*)(image + symtab->offset + symtab->ent_size * sym_idx);
	return 1;
}

uint8_t	elf32_get_symbol_by_name(void* image, const char* sym_name, int32_t type, elf_sect_t* symtab, elf_sect_t* strtab, elf_sym_t** symbol)
{
	elf_sym_t	*sym;
	uint32_t	i;
	int8_t		*strings;

	/* Verifica se as se��es s�o 'Symbol Table' e 'String Table' */
	if(symtab->type != SHT_SYMTAB || strtab->type != SHT_STRTAB)
		return 0;

	strings = (int8_t*)(image + strtab->offset);

	for(i = 0; i < (symtab->size / symtab->ent_size); i++)
	{
		sym = (elf_sym_t*)(image + symtab->offset + i * symtab->ent_size);
		if(!strcmp((char*)(strings + sym->name), (char*)sym_name))
		{
			if(type > 0 && sym->type != type)
				return 0;

			if(symbol != NULL)
				*symbol = sym;

			return 1;
		}
	}

	return 0;
}

/*
	Procura por um s�mbolo percorrendo todas as tabelas de s�mbolos.
	fun��o parecida com elf32_get_symbol_by_name() por�m mais gen�rica.
*/
uint8_t	elf32_search_symbol(void* image, const char* name, uint32_t type, elf_sym_t** symbol)
{
	elf_sect_t	*sect, *strtab;
	uint32_t	i;

	for(i = 0; i < elf32_get_sections(image, SHT_SYMTAB, -1); i++)	// pesquisa em todas tabelas de s�mbolos.
	{
		if(elf32_get_sections_i(image, i, SHT_SYMTAB, -1, &sect))	// carrega symtab #i
		if(elf32_get_section_i(image, sect->link, &strtab))		// carrega strtab correspondente.
		if(elf32_get_symbol_by_name(image,name,type, sect, strtab, symbol))	// procura pelo s�mbolo.
			return 1;
	}
	return 0;
}

uint8_t	elf32_get_name(void* image, uint32_t index, elf_sect_t* strtab, const char** name)
{
	int8_t*	strings;

	if(strtab->type != SHT_STRTAB)
		return 0;

	if(index > strtab->size)
		return 0;

	strings = (int8_t*)(image + strtab->offset);

	*name = (char*)(strings + index);

	return 1;
}

// Aplica relocaliza��o referenciada por membro de se��o SHT_REL ou SHT_RELA.
uint8_t	elf32_reloc_apply(void* image, elf_reloc_t* rel, uint32_t symtab_idx, uint32_t t_sect_idx)
{
	elf_sect_t	*symtab, *sect, *sect2;
	elf_sym_t	*symbol;
	uint32_t	*rel_val, sym_val;

	if(rel->type ==0)
		return RELOC_OK;

	if(!elf32_get_section_i(image, symtab_idx, &symtab))	// carrega 'Symbol Table' associada � 'Relocation Table'.
		return RELOC_ERROR_SYMTAB;

	if(!elf32_get_section_i(image, t_sect_idx, &sect))	// carrega se��o alvo da relocaliza��o.
		return RELOC_ERROR_TARGET;

	//printf("symtab.type=%d tsect.type=%d symbol.sect=%d symidx=%d\n", symtab->type & 0xff, sect->type & 0xff, symbol->section & 0xffff, symtab_idx);

	if(rel->symtab_index == 0)	// Refer�ncia: (If the index is STN_UNDEF, the undefined symbol index, the relocation uses 0 as the "symbol value.")
	{
		sym_val = 0;
		goto	ignore_symbol;
	}
	else
	{
		if(!elf32_get_symbol_i(image, rel->symtab_index, symtab, &symbol))	// carrega o s�mbolo a ser relocalizado.
			return RELOC_ERROR_SYMBOL;

//rel_retry:
		switch(symbol->section)
		{
			case	SHN_COMMON:
				return	RELOC_COMMON;
			case	SHN_ABS:
			case	SHN_SOMBRA:
				sym_val = (uint32_t)(symbol->value);// + image);
				break;

			default:
			if(!elf32_get_section_i(image, symbol->section, &sect2))
				return RELOC_ERROR_SYMBOL;
			else
				sym_val = (uint32_t)(symbol->value);
			break;
		}
	}

	if(symbol->section != 0)
	{

ignore_symbol:
		rel_val = (uint32_t*)(image + sect->offset + rel->adr);	// Endere�o da relocaliza��o
		if(symbol->section == SHN_SOMBRA)
			*rel_val = (uint32_t)sym_val + *rel_val;
		else
			*rel_val = (uint32_t)((uint32_t)(image + sym_val + sect2->offset) + *rel_val);

		switch(rel->type)
		{
			case 1:
				break;
			case 2:
				*rel_val -= (uint32_t)rel_val;
				break;

			default:
				printf("Tipo desconhecido de relocalizacao em 'ELF Relocatable'.\n");
				return RELOC_ERROR;
				break;
		}
		return RELOC_OK;
	}
	else
	{
			return RELOC_UNDEF_SYMBOL;
	}

	return RELOC_OK;
}

// retorna 1 se o arquivo for valido.
uint8_t	elf32_check(void* image, uint8_t type)
{
	elf_file_t*	elf = (elf_file_t*)image;

	if(elf->magic != ELF_SIGNATURE)
		return 0;

	if( elf->bitness != 1 || elf->endian != 1 || elf->elf_ver_1 != 1
		|| elf->file_type != type || elf->machine != 3 || elf->elf_ver_2 != 1)
	return 0;

	return 1;
}
