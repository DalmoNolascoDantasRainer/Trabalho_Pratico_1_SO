#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#define BUFFER 100  //Maximo de caracteres no pipe

void escreverCaracterePipe(int fd, char caractere);
char lerCaracterePipe(int fd);