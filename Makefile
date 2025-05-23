FilaPrioridade:
	clear
	gcc -g -O0 -Wall main.c ./Modulos/Menu/Menu.c ./Modulos/ProcessoSimulado/Instrucao.c ./Modulos/ProcessoSimulado/ProcessoSimulado.c ./Modulos/GerenciadorProcessos/Cpu.c ./Modulos/GerenciadorProcessos/GerenciadorProcessos.c ./Modulos/ProcessoControle/ProcessoControle.c ./Modulos/Pipe/Pipe.c ./Modulos/ProcessoImpressao/ProcessoImpressao.c ./Modulos/EstruturasDeDados/Fila.c ./Modulos/EstruturasDeDados/Lista.c ./Modulos/GerenciadorProcessos/TabelaProcessos.c -o FilaPrioridade.exe
	./FilaPrioridade.exe 2

# Adicione este target para apenas compilar sem executar
build:
	clear
	gcc -g -O0 -Wall main.c ./Modulos/Menu/Menu.c ./Modulos/ProcessoSimulado/Instrucao.c ./Modulos/ProcessoSimulado/ProcessoSimulado.c ./Modulos/GerenciadorProcessos/Cpu.c ./Modulos/GerenciadorProcessos/GerenciadorProcessos.c ./Modulos/ProcessoControle/ProcessoControle.c ./Modulos/Pipe/Pipe.c ./Modulos/ProcessoImpressao/ProcessoImpressao.c ./Modulos/EstruturasDeDados/Fila.c ./Modulos/EstruturasDeDados/Lista.c ./Modulos/GerenciadorProcessos/TabelaProcessos.c -o FilaPrioridade.exe