#ifndef CLIENT
#define CLIENT

#define CLIENT_NAME_SIZE 100

#include <stdbool.h>  // Para usar true e false

typedef struct {
    int codCliente;
    char nome[100];
    bool ocupado;  // Adiciona o campo para marcar se o cliente está ocupado
    int posicao;  // Campo para armazenar a posição do cliente na tabela de hash
} Client;

void inserir_clientes(char* dir_tabClientes) {
    FILE* tabClientes = fopen(dir_tabClientes, "wb");
    if (tabClientes == NULL) {
        perror("Erro ao abrir arquivo de clientes");
        exit(1);
    }

    Client clientes[10] = {
        {101, "Cliente A", false, -1},
        {102, "Cliente B", false, -1},
        {103, "Cliente C", false, -1},
        {104, "Cliente D", false, -1},
        {105, "Cliente E", false, -1},
        {106, "Cliente F", false, -1},
        {107, "Cliente G", false, -1},
        {108, "Cliente H", false, -1},
        {109, "Cliente I", false, -1},
        {110, "Cliente J", false, -1},
    };

    fwrite(clientes, sizeof(Client), 10, tabClientes);
    fclose(tabClientes);
}

#endif