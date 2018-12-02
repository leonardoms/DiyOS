#include <stdio.h>

void
itoa(uint8_t *buf, uint32_t base, uint32_t d) {
   uint8_t *p = buf;
   uint8_t *p1, *p2;
   uint32_t ud = d;
   uint32_t divisor = 10;

   if(base == 'd' && d < 0) {
       *p++ = '-';
       buf++;
       ud = -d;
   } else
     if (base == 'x')
         divisor = 16;

   do{
       uint32_t remainder = ud % divisor;
       *p++ = (remainder < 10) ? remainder + '0' : remainder + 'a' - 10;
   } while (ud /= divisor);

   *p = 0;
   p1 = buf;
   p2 = p - 1;
   while (p1 < p2) {
     uint8_t tmp = *p1;
     *p1 = *p2;
     *p2 = tmp;
     p1++;
     p2--;
   }
}
