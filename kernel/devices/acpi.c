#include <arch.h>
#include <kernel.h>
#include <stdio.h>

void
_page_map(uint32_t p_addr, uint32_t v_addr, uint32_t length) {
  uint32_t  *pd, *table, p_idx, t_idx;
  uint32_t pages;

  pages = length / 0x1000;
  if((pages % 0x1000) != 0)
    pages++;

  __asm__ __volatile__("movl %%cr3, %%eax; movl %%eax, %0;":"=m"(pd)::"%eax");
  pd = (uint32_t*)( (uint32_t)pd & 0xFFFFF000 );
  while(pages>0) {
    t_idx = v_addr >> 22;
    pd[t_idx] = pd[t_idx] | PAGE_FLAG_PRESENT | PAGE_FLAG_RW;

    table = (uint32_t*) (pd[t_idx] & 0xFFFFF000);
    p_idx = ( v_addr >> 12 ) & 0x3FF;
    table[p_idx] = (p_addr & 0xFFFFF000) | PAGE_FLAG_PRESENT | PAGE_FLAG_RW | PAGE_FLAG_BITMAP_END | PAGE_FLAG_BITMAP_START;

    v_addr += 0x1000;
    p_addr += 0x1000;
    pages --;
  }

}

void
acpi_handler(isr_regs_t regs) {
  printf("ACPI interrupt!\n");
}

struct RSDPDescriptor {
 char Signature[8];
 uint8_t Checksum;
 char OEMID[6];
 uint8_t Revision;
 uint32_t RsdtAddress;
} __attribute__ ((packed));

struct ACPISDTHeader {
  char Signature[4];
  uint32_t Length;
  uint8_t Revision;
  uint8_t Checksum;
  char OEMID[6];
  char OEMTableID[8];
  uint32_t OEMRevision;
  uint32_t CreatorID;
  uint32_t CreatorRevision;
} __attribute__ ((packed));

struct RSDT {
  struct ACPISDTHeader h;
  uint32_t PointerToOtherSDT[16];
} __attribute__ ((packed));

struct GenericAddressStructure
{
  uint8_t AddressSpace;
  uint8_t BitWidth;
  uint8_t BitOffset;
  uint8_t AccessSize;
  uint32_t Address;
} __attribute__ ((packed));

struct FADT
{
    struct   ACPISDTHeader h;
    uint32_t FirmwareCtrl;
    uint32_t Dsdt;

    // field used in ACPI 1.0; no longer in use, for compatibility only
    uint8_t  Reserved;

    uint8_t  PreferredPowerManagementProfile;
    uint16_t SCI_Interrupt;
    uint32_t SMI_CommandPort;
    uint8_t  AcpiEnable;
    uint8_t  AcpiDisable;
    uint8_t  S4BIOS_REQ;
    uint8_t  PSTATE_Control;
    uint32_t PM1aEventBlock;
    uint32_t PM1bEventBlock;
    uint32_t PM1aControlBlock;
    uint32_t PM1bControlBlock;
    uint32_t PM2ControlBlock;
    uint32_t PMTimerBlock;
    uint32_t GPE0Block;
    uint32_t GPE1Block;
    uint8_t  PM1EventLength;
    uint8_t  PM1ControlLength;
    uint8_t  PM2ControlLength;
    uint8_t  PMTimerLength;
    uint8_t  GPE0Length;
    uint8_t  GPE1Length;
    uint8_t  GPE1Base;
    uint8_t  CStateControl;
    uint16_t WorstC2Latency;
    uint16_t WorstC3Latency;
    uint16_t FlushSize;
    uint16_t FlushStride;
    uint8_t  DutyOffset;
    uint8_t  DutyWidth;
    uint8_t  DayAlarm;
    uint8_t  MonthAlarm;
    uint8_t  Century;

    // reserved in ACPI 1.0; used since ACPI 2.0+
    uint16_t BootArchitectureFlags;

    uint8_t  Reserved2;
    uint32_t Flags;

    struct GenericAddressStructure ResetReg;

    uint8_t  ResetValue;
    uint8_t  Reserved3[3];
} __attribute__ ((packed));

#define SCI_EN  (1 << 0)
#define SLP_EN  (1 << 13)
#define PWRBTN_EN (1 << 8)

struct RSDT* rsdt;
struct FADT* facp;
struct ACPISDTHeader* dsdt;
uint16_t SLP_TYPa, SLP_TYPb; // 3-bit value read from FACP \S5_ package

uint8_t*
acpi_search_S5(struct ACPISDTHeader* dsdt);

void
acpi_enable() {
  ASSERT_PANIC(facp != NULL);

  outportb(facp->SMI_CommandPort, facp->AcpiEnable);  // enable ACPI
  while( !(facp->PM1aControlBlock && SCI_EN) );    // wait ACPI transfer control to SCI_Interrupt events

  if ( facp->PM1bControlBlock != 0 ) {
    outportb(facp->SMI_CommandPort, facp->AcpiEnable);  // enable ACPI
    while( !(facp->PM1bControlBlock && SCI_EN) );    // wait ACPI transfer control to SCI_Interrupt events
  }
}

void
acpi_power_down()
{
  acpi_enable();

  outportw(facp->PM1aControlBlock, (SLP_TYPa << 0) | SLP_EN );

  if ( facp->PM1bControlBlock != 0 )
    outportw(facp->PM1aControlBlock, (SLP_TYPb << 0) | SLP_EN );

  PANIC("ACPI power down failed!");
}

void
acpi_reset() {

  acpi_enable();
  outportw(facp->ResetReg.Address, facp->ResetValue);
}

void
acpi_power_button_enable() {
  acpi_enable();

  outportw(facp->PM1aEventBlock, PWRBTN_EN);
}

uint8_t
acpi_checksum(void* ptr, uint32_t size, uint8_t checksum) {
  uint32_t i, sum = 0;
  for( i = 0; i < size; i++ ) {
    sum += ((uint8_t*)ptr)[i];
  }
  return sum == 0;
}

#define return_val_if_fail(test,val) if(!test) return val;

uint32_t
acpi() {
  uint32_t  ptr;
  uint32_t  i, i_max;
  uint8_t*  S5Block;
  struct RSDPDescriptor* rsdp;
  struct ACPISDTHeader* sdt;

  printf("ACPI ");

  // search for Root System Description Pointer address
  for( ptr = 0; ptr < 0x100000; ptr += 16 ) {

    // search for signature
    if( !strncmp((char*)ptr, "RSD PTR ", 8) ) {
      rsdp = (struct RSDPDescriptor*)ptr;

      // return_val_if_fail(acpi_checksum(rsdp,sizeof(struct RSDPDescriptor),rsdp->Checksum), 0);

      for( i = 0; i < 6; i++ ) {
        if(rsdp->OEMID[i] == ' ') {
          rsdp->OEMID[i] = '\0';
          break;
        }
      }

      printf("found (OEM: %s Rev. %d) ",
                      rsdp->OEMID,
                      rsdp->Revision );

      rsdt = (struct RSDT*)rsdp->RsdtAddress;
      _page_map(rsdp->RsdtAddress, rsdp->RsdtAddress, 0x1000); // make rsdt header accessible
      _page_map(rsdp->RsdtAddress, rsdp->RsdtAddress, rsdt->h.Length);
      i_max = (rsdt->h.Length - sizeof(struct ACPISDTHeader)) / 4;

      for( i = 0; i < i_max; i++ ) {
        sdt = (struct ACPISDTHeader*)rsdt->PointerToOtherSDT[i];

        _page_map(rsdt->PointerToOtherSDT[i], rsdt->PointerToOtherSDT[i], 0x1000);
        _page_map(rsdt->PointerToOtherSDT[i], rsdt->PointerToOtherSDT[i], sdt->Length);

        switch( *((uint32_t*)sdt->Signature) ) {
          case 'TDSS': // same than DSDT
              // printf("[+SSDT] ");
              S5Block = acpi_search_S5(sdt);
              if( S5Block != NULL ) {
                // printf("\\_S5 block found!");
                S5Block += 4; // skip \_S5_ signature
                S5Block += ((*S5Block & 0xC0) >> 6) + 2; // skip PkgLength

                if( *S5Block == 0x0A ) // skip BytePrefix AML OpCode
                  S5Block++;
                SLP_TYPa = *S5Block;// & 0x7; // 3-bit
                S5Block++;

                if( *S5Block == 0x0A ) // skip BytePrefix AML OpCode
                  S5Block++;
                SLP_TYPb = *S5Block;// & 0x7;

                // printf("SLP_TYPa=0x%x SLP_TYPb=0x%x ", SLP_TYPa, SLP_TYPb);
              }
              break;
          case 'PCAF': // "FACP"
            // printf("[+FACP] ");
            facp = (struct FADT*)sdt;

            irq_install_callback(facp->SCI_Interrupt, acpi_handler); // install ACPI IRQ handler

            // printf("Profile=%d ", facp->PreferredPowerManagementProfile);
            // printf("ResetReg=0x%x(0x%x)\n", facp->ResetReg.Address, facp->ResetValue);

            dsdt = (struct ACPISDTHeader*)facp->Dsdt;
            if( dsdt != NULL ) {
              _page_map(facp->Dsdt, facp->Dsdt, 0x1000); // make DSDT header accessible
              _page_map(facp->Dsdt, facp->Dsdt, dsdt->Length); // map the correct size
            }

            if( (*((uint32_t*)dsdt) == 'TDSD') )  { // "DSDT"
              S5Block = acpi_search_S5(dsdt);
              if( S5Block != NULL ) {
                // printf("\\_S5 block found!");
                S5Block += 4; // skip \_S5_ signature
                S5Block += ((*S5Block & 0xC0) >> 6) + 2; // skip PkgLength

                if( *S5Block == 0x0A ) // skip BytePrefix AML OpCode
                  S5Block++;
                SLP_TYPa = *S5Block;// & 0x7; // 3-bit
                S5Block++;

                if( *S5Block == 0x0A ) // skip BytePrefix AML OpCode
                  S5Block++;
                SLP_TYPb = *S5Block;// & 0x7;

                // printf("SLP_TYPa=0x%x SLP_TYPb=0x%x ", SLP_TYPa, SLP_TYPb);
              }
            } else {
              dsdt = (struct ACPISDTHeader*)NULL;
            }

            break;
          case 'TEPH': // HPET
            // printf("[+HPET] ");
            break;
          default:
            // printf("[-%c%c%c%c] ", sdt->Signature[0], sdt->Signature[1], sdt->Signature[2], sdt->Signature[3]);
            break;
        }

      }

      acpi_power_button_enable();
      printf("SCI_Interrupt=%d SMI_CommandPort=0x%x ShutDown=%c\n", facp->SCI_Interrupt, facp->SMI_CommandPort, (S5Block==NULL)?'N':'Y');
      return ptr;
    }

  }
  printf("RSDP nao encontrado.\n");
  return 0;
}

uint8_t*
acpi_search_S5(struct ACPISDTHeader* hdr) {
  uint32_t l;
  uint32_t*  S5;

  l = hdr->Length - sizeof(struct ACPISDTHeader);
  S5 = (uint32_t*)(hdr + sizeof(struct ACPISDTHeader)); // skip header
  while(l--) {
    if( *S5 == (uint32_t)'_5S_' ) { // "_S5_"
      // make sure that is a ACPI block
      //TODO: check 0x8 OpCode
      return (uint8_t*)S5;
    }
    S5 = (uint32_t*)((uint32_t)S5 + 1);
  }

  return (uint8_t*)NULL;
}
