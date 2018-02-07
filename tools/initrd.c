#include <stdio.h>
#include <string.h>

#define INITRD_MAX_FILES    64
#define INITRD_FILE_NAME_SZ 64
#define INITRD_MAGIC        0xBF

// Estrutura que representa o cabeçalho de um arquivo
typedef struct
{
   unsigned char magic; // The magic number is there to check for consistency.
   char name[INITRD_FILE_NAME_SZ];
   unsigned int offset; // Offset in the initrd the file starts.
   unsigned int length; // Length of the file.
} initrd_file_t;

// Protótipos
inline void help(void);
inline void lista_arquivos(FILE *wstream);

int main(char argc, char **argv)
{
    int nheaders = (argc-1)/2;

// Estrutura para armazenar as informações do arquivo atual
    initrd_file_t file;
    unsigned i, offset = 0;

    FILE *wstream;

    if (argc == 1)
    {
        wstream = fopen("./initrd.img", "rb");

        if (wstream != NULL)
            lista_arquivos(wstream);
        else
            help();

        return 0;
    }

    printf("size of header: %d\n", sizeof(initrd_file_t));
    wstream = fopen("./initrd.img", "wb");

    if (nheaders < INITRD_MAX_FILES)
    for(i = 0; i < nheaders; i++)
    {
        FILE *stream = fopen(argv[i*2+1], "rb");

        if(stream == 0)
        {
            printf("Error: file not found: %s\n", argv[i*2+1]);
            return 1;
        }

        int buffer = 0;

        printf("writing file %s->%s\n", argv[i*2+1], argv[i*2+2]);

        memset(&file, 0, sizeof(initrd_file_t));
        strcpy(file.name, argv[i*2+2]);
        file.offset = ftell(wstream)+sizeof(initrd_file_t);
        fseek(stream, 0, SEEK_END);
        file.length = ftell(stream);
        fseek(stream, 0, SEEK_SET);
        file.magic = INITRD_MAGIC;

        fwrite(&file, sizeof(initrd_file_t), 1, wstream);
        while (buffer != EOF)
        {
            buffer = getc(stream);

            if (buffer != EOF)
                putc(buffer, wstream);
        }

        fclose(stream);
    }

    fclose(wstream);

   return 0;
}

inline void lista_arquivos(FILE *wstream)
{
    initrd_file_t file;
    unsigned sz, offset = 0;

    fseek(wstream, 0, SEEK_END);
    sz = ftell(wstream);
    fseek(wstream, 0, SEEK_SET);

    while (offset < sz)
    {
        fread(&file, sizeof(initrd_file_t), 1, wstream);

        if (file.magic != INITRD_MAGIC)
            break;

        printf("@FILE: %20s # @SIZE: %10u\n", file.name, file.length);
        fseek(wstream, file.length, SEEK_CUR);
        offset = ftell(wstream);
    }
}

inline void help(void)
{
    printf("initrd - Versao 0.0.1a\nAutor: Mauro Joel Schutz\n\tUse initrd [<arquivo> <nome>]*\n");
}
