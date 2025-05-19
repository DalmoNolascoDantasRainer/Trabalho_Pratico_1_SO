#include "Pipe.h"

void escreverCaracterePipe(int fd, char caractere)
{
    write(fd, &caractere, sizeof(char));

}

char lerCaracterePipe(int fd)
{
    char caractere;
    read(fd, &caractere, sizeof(char));
   
    return caractere;
}