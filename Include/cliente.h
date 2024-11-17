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
    // Abrir arquivo binário para escrita
    FILE* tabClientes = fopen(dir_tabClientes, "wb");
    if (tabClientes == NULL) {
        perror("Erro ao abrir arquivo de clientes");
        exit(1);
    }

    // Array com 20 clientes para inserir
    Client clientes[10] = {
        {101, "Cliente A", "01/01/1990"},
        {102, "Cliente B", "02/02/1991"},
        {103, "Cliente C", "03/03/1992"},
        {104, "Cliente D", "04/04/1993"},
        {105, "Cliente E", "05/05/1994"},
        {106, "Cliente F", "06/06/1995"},
        {107, "Cliente G", "07/07/1996"},
        {108, "Cliente H", "08/08/1997"},
        {109, "Cliente I", "09/09/1998"},
        {110, "Cliente J", "10/10/1999"},
    };

    // Escrevendo os 20 clientes no arquivo
    fwrite(clientes, sizeof(Client), 20, tabClientes);

    // Fechar arquivo
    fclose(tabClientes);

    printf("20 clientes inseridos no arquivo com sucesso.\n");
}

#endif