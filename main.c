#include "./Modulos/ProcessoControle/ProcessoControle.h"
#include "./Modulos/ProcessoImpressao/ProcessoImpressao.h"
#include <sys/wait.h>
#include <signal.h>

int impressaoExecutando = 0;
void semaforoImpressao(int signum)
{
    impressaoExecutando = 0; // INDICA QUE O PROCESSO IMPRESSAO NAO VAI EXECUTAR MAIS
}

int main(int argc, char **argv)
{
    int fd[2]; // Tamanho do pipe, escrever no pipe: fd[1], ler do pipe: fd[0]
    char comando = '!'; // Valo nao utilizado
    FILE *arquivoDeEntrada;
    int numCPUs = atoi(argv[1]);
    GerenciadorProcessos *gerenciador = inicializaGerenciador(numCPUs);

    int opcao = MenuInicial(&arquivoDeEntrada); // Arquivo ou teclado
    int opcaoImpressao = 0;
    int PID;

    if (pipe(fd) == -1) // Verificar se pipe foi inicializado com sucesso
    {
        perror("Erro ao criar o pipe");
        return 1;
    }

    pid_t pidImpressao; // Cria processo de impressao
    pid_t pid = fork(); // gera processo filho

    if (pid == -1)
    {
        perror("Erro ao criar o processo");
        return 1;
    }

    if (pid > 0)
    {
        // A funcao signal quando recebe o sinal SIGUSR1 executa o que esta dentro de semaforoImpressao
        signal(SIGUSR1, semaforoImpressao); // Ela registra o sinal do processo impressao

        while (1) // Loop processo pai
        {
            if (opcao == 2) // Entrada por arquivo
            {
                comando = controle(arquivoDeEntrada, opcao);
                escreverCaracterePipe(fd[1], comando); // Envia para o filho
            }
            else // Entrada por teclado (usuario)
            {
                scanf(" %c", &comando);
                escreverCaracterePipe(fd[1], comando);

                if (comando == 'I')
                {
                    impressaoExecutando = 1; // Ativa o semaforo de impressao

                    while (impressaoExecutando) // Envia o sinal e aguarda o termino da impressao
                    {
                        sleep(1);
                    }
                }
            }

            if (comando == 'M') // Comando de termino
            {
                break;
            }
        }
        wait(NULL); // Aguarda o fim o processo filho
    }
    else
    {
        while (1) // Loop processo filho
        {

            comando = lerCaracterePipe(fd[0]); // le comando vindo do processo pai

            if (comando == 'U') // Comando para fim de uma unidade de tempo
            {
                gerenciadorProcessos(gerenciador, comando); // Chama o gerenciador de processos
            }

            else if (comando == 'I') // Comando de impressao
            {
                if (opcao == 2) // Imprime diretamente se for vindo de arquivo
                {
                    impressaoArquivo(gerenciador);
                }
                else
                {

                    pidImpressao = fork(); // Cria processo filho pra impressao
                    if (pidImpressao < 0)
                    {
                        printf("ERRO NO FORK() IMPRESSAO\n");
                    }

                    
                    if (pidImpressao > 0) // indica que e um processo filho
                    {
                        wait(NULL);               // Aguarda o termino do processo de impressao
                        kill(getppid(), SIGUSR1); // ENVIA O SINAL PRO PROCESSO QUE LE DO PIPE
                        sleep(1);
                    }
                    else
                    {
                        // PROCESSO IMPRESSAO
                        ImprimeGerenciadorProcessos(gerenciador);

                        exit(0); // VOLTA PARA O WAIT(NULL)
                    }
                }
            }

            if (comando == 'M') // Comando de termino   
            {
                break;
            }
        }
    }
    return 0;
}