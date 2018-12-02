#ifndef _STRING_H_
#define _STRING_H_

void
memcpy(void* src, void* dst, uint32_t size);

void
memset(void* src, uint8_t value, uint32_t size);

int
strcmp(char *s1, char *s2);

void
strcpy(char dest[], const char source[]);

uint32_t
strlen(uint8_t* str);

int
strncmp(const char *s1, const char *s2, unsigned int n);

void
itoa(uint8_t *buf, uint32_t base, uint32_t d);



#endif
