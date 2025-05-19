#include "Pipe.h"

// Escreve um caractere no pipe
void escreverCaracterePipe(int fd, char caractere){
    write(fd, &caractere, sizeof(char));
}

// Le um caractere do pipe
char lerCaracterePipe(int fd){
    char caractere;
    read(fd, &caractere, sizeof(char));
   
    return caractere;
}

