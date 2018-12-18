
void
strncpy(char dest[], const char source[], int size)
{
    int i = 0, sz = size;
    while (1)
    {
        dest[i] = source[i];

        if (dest[i] == '\0')
        {
            break;
        }

        if( i >= size ) {
          dest[i] = '\0';
          break;
        }

        i++;
    }
}
