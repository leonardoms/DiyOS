#include <x86/frame.h>
#include <debug/bochs.h>

// a posicao fisica do frame usado em malloc e kmalloc nao importa,
// a unica coisa que importa é a tabela onde ele esta inserido, seja
// ela a da task ou a do kernel. portanto
// malloc: frame_get e alloca o frame na tabela do processo, atravez de syscall "getFrame"
// kmalloc: frame_get e alloca o frame na tabela do kernel.

uint32_t   frames_count, frame_idx;
uint32_t   frames_free;

#define    BITMAP_SIZE     0x20000
uint8_t    physical_bitmap[BITMAP_SIZE];  // bitmap for physical memory

void
frame_set(uint32_t i) {
  physical_bitmap[i / 8] = 1 << (i % 8);
  frames_free--;
}

void
frame_set_address(uint32_t addr) {
  uint32_t i;

  i = addr / FRAME_SIZE;

  physical_bitmap[i / 8] = 1 << (i % 8);
  frames_free--;
}

void
frame_unset(uint32_t i) {

  if( i > frames_count )
    return;

  physical_bitmap[i / 8] &= ~(1 << (i % 8));
  frames_free++;
}

void
frame_unset_address(uint32_t addr) {
  uint32_t i;

  i = addr / FRAME_SIZE;

  if( i > frames_count )
    return;

  physical_bitmap[i / 8] &= ~(1 << (i % 8));
  frames_free++;
}

void
frame_setup(uint32_t mem_size, uint32_t reserved_start, uint32_t reserved_end) {
  uint32_t i;
  frames_count = mem_size / FRAME_SIZE;
  frames_free = frames_count;
  frame_idx = 0;

  // printf("frame_setup(): sz=%d, s=0x%x, e=0x%x\n", mem_size, reserved_start, reserved_end);

  // set kernel frames as diry
  for( i = reserved_start; i < reserved_end; i += FRAME_SIZE) {
    frame_set_address(i);
    frame_idx++;
  }
}

uint32_t
frame_get_blk(uint32_t blk_sz) {
    uint32_t blk_left, blk_start, i,
             i_byte, i_bit;

    blk_left = blk_sz;
    for ( i = 0; i < frames_count; i++) {
      i_byte = i / 8;
      i_bit = i % 8;

      if(physical_bitmap[i_byte] && ( 1 << i_bit) ) { // is it a free bit?
         if( blk_left < blk_sz ) {
           blk_left = blk_sz;
         }
         continue; // dirty bit
      }

      if( blk_left == blk_sz ) {
        blk_start = i;
      }

      blk_left--;

      if(blk_left == 0)
        break;
    }

    for ( i = blk_start; i < (blk_start+blk_sz); i++) {
      frame_set(i);
    }

    frame_idx += blk_sz;

    return (uint32_t)(blk_start * FRAME_SIZE);
}

uint32_t
frame_get() {

  uint32_t i_byte, i_bit, i;

  // printf("frame_get().frame_idx = %d\n", frame_idx);

  if( frames_free == 0 ) {
      return 0;
  }

  for(i = frame_idx; i < frames_count; i ++) {
    i_byte = i / 8;
    i_bit = i % 8;

    if(physical_bitmap[i_byte] && ( 1 << i_bit) ) { // is it a free bit?
       continue; // dirty bit
    }

    frame_set(i);
    frame_idx++;
    // printf("frame_get(): 0x%x\n", (uint32_t)((i_byte * 8 + i_bit) * FRAME_SIZE));
    return (uint32_t)((i_byte * 8 + i_bit) * FRAME_SIZE) ;
  }

  // if here, based on 'frames_free', has free frame and thats not found (above current idx), then reset frame_idx and find it!

  frame_idx = 0;
  return frame_get();
}

void
frame_free(uint32_t addr) {
  frame_unset_address(addr);
}

uint32_t
frame_count() {
  return frames_count;
}

uint32_t
frame_free_count() {
  return frames_free;
}
