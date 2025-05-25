# Compiler and flags
CC = gcc
CFLAGS = -g -O0 -Wall -Wextra -std=c99
TARGET = FilaPrioridade.exe
THREAD_TARGET = FilaPrioridadeThreads.exe

# Source files
MAIN_SRC = main.c
THREAD_MAIN_SRC = mainThreads.c

# Module source files
MODULE_SRCS = ./Modulos/ProcessoSimulado/Instrucao.c \
              ./Modulos/ProcessoSimulado/ProcessoSimulado.c \
              ./Modulos/GerenciadorProcessos/Cpu.c \
              ./Modulos/GerenciadorProcessos/CpuRR.c \
              ./Modulos/GerenciadorProcessos/GerenciadorProcessosRR.c \
              ./Modulos/GerenciadorProcessos/GerenciadorProcessos.c \
              ./Modulos/ProcessoControle/ProcessoControle.c \
              ./Modulos/Pipe/Pipe.c \
              ./Modulos/ProcessoImpressao/ProcessoImpressao.c \
              ./Modulos/ProcessoImpressao/ProcessoImpressaoRR.c \
              ./Modulos/EstruturasDeDados/Fila.c \
              ./Modulos/EstruturasDeDados/Lista.c \
              ./Modulos/GerenciadorProcessos/TabelaProcessos.c

# Thread source files
THREAD_SRCS = ./Threads/Threads.c

# Object files
OBJS = $(MAIN_SRC:.c=.o) $(MODULE_SRCS:.c=.o)
THREAD_OBJS = $(THREAD_MAIN_SRC:.c=.o) $(MODULE_SRCS:.c=.o) $(THREAD_SRCS:.c=.o)

# Default target
.PHONY: all clean help run-priority run-threads build build-threads

all: $(TARGET)

# Build main priority queue program
$(TARGET): $(OBJS)
	@echo "Linking $(TARGET)..."
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

# Build threads version
$(THREAD_TARGET): $(THREAD_OBJS)
	@echo "Linking $(THREAD_TARGET)..."
	$(CC) $(CFLAGS) $(THREAD_OBJS) -o $(THREAD_TARGET)

# Compile source files to object files
%.o: %.c
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# Original targets for compatibility
FilaPrioridade: clean $(TARGET) run-priority

Threads: clean $(THREAD_TARGET) run-threads

# New cleaner targets
build: $(TARGET)
	@echo "Build complete: $(TARGET)"

build-threads: $(THREAD_TARGET)
	@echo "Build complete: $(THREAD_TARGET)"

# Run targets
run-priority: $(TARGET)
	@echo "Running priority queue version..."
	./$(TARGET) 2

run-threads: $(THREAD_TARGET)
	@echo "Running threads version..."
	./$(THREAD_TARGET) 2

# Debug targets
debug-priority: $(TARGET)
	@echo "Running priority queue version with GDB..."
	gdb --args ./$(TARGET) 2

debug-threads: $(THREAD_TARGET)
	@echo "Running threads version with GDB..."
	gdb --args ./$(THREAD_TARGET) 2

# Clean target
clean:
	@echo "Cleaning up..."
	@rm -f $(TARGET) $(THREAD_TARGET)
	@find . -name "*.o" -type f -delete
	@clear

# Help target
help:
	@echo "Available targets:"
	@echo "  all              - Build priority queue version (default)"
	@echo "  build            - Build priority queue version without running"
	@echo "  build-threads    - Build threads version without running"
	@echo "  run-priority     - Build and run priority queue version"
	@echo "  run-threads      - Build and run threads version"
	@echo "  debug-priority   - Run priority queue version with GDB"
	@echo "  debug-threads    - Run threads version with GDB"
	@echo "  clean            - Remove all compiled files"
	@echo "  help             - Show this help message"
	@echo ""
	@echo "Legacy targets (for compatibility):"
	@echo "  FilaPrioridade   - Clean, build and run priority queue version"
	@echo "  Threads          - Clean, build and run threads version"

