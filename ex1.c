#include <stdio.h>
#include <stdlib.h>

typedef struct Cliente {
    int codCliente;
    struct Cliente* prox;  // Para encadeamento
} Cliente;

typedef struct HashTable {
    Cliente** tabela;
    int tamanho;
} HashTable;

int funcaoHash(int chave, int tamanho) {
    return chave % tamanho;
}

HashTable* inicializarTabela(int tamanho) {
    HashTable* tabelaHash = (HashTable*)malloc(sizeof(HashTable));
    tabelaHash->tabela = (Cliente**)malloc(tamanho * sizeof(Cliente*));
    tabelaHash->tamanho = tamanho;
    for (int i = 0; i < tamanho; i++) {
        tabelaHash->tabela[i] = NULL;
    }
    return tabelaHash;
}

void inserirCliente(HashTable* tabelaHash, int codCliente) {
    // Calcular o índice usando a função hash
    int indice = funcaoHash(codCliente, tabelaHash->tamanho);

    // Criar novo cliente
    Cliente* novoCliente = (Cliente*)malloc(sizeof(Cliente));
    novoCliente->codCliente = codCliente;
    novoCliente->prox = NULL;

    // Inserir o cliente no início da lista encadeada
    if (tabelaHash->tabela[indice] == NULL) {
        tabelaHash->tabela[indice] = novoCliente;
    } else {
        // Encadeamento exterior (inserir no início da lista)
        novoCliente->prox = tabelaHash->tabela[indice];
        tabelaHash->tabela[indice] = novoCliente;
    }
}

void exibirTabela(HashTable* tabelaHash) {
    printf("Tabela Hash:\n");
    for (int i = 0; i < tabelaHash->tamanho; i++) {
        printf("[%d]: ", i);
        Cliente* atual = tabelaHash->tabela[i];
        while (atual != NULL) {
            printf("%d -> ", atual->codCliente);
            atual = atual->prox;
        }
        printf("NULL\n");
    }
}

int main() {
    // Inicializar a tabela hash com tamanho 7
    int tamanho = 7;
    HashTable* tabelaHash = inicializarTabela(tamanho);

    // Inserir as chaves mencionadas
    int chaves[] = {10, 3, 5, 7, 12, 6, 14, 4, 8};
    int numChaves = sizeof(chaves) / sizeof(chaves[0]);

    // Inserindo todas as chaves na tabela
    for (int i = 0; i < numChaves; i++) {
        inserirCliente(tabelaHash, chaves[i]);
    }

    // Exibir a tabela hash resultante
    exibirTabela(tabelaHash);

    // Liberar memória alocada
    for (int i = 0; i < tamanho; i++) {
        Cliente* atual = tabelaHash->tabela[i];
        while (atual != NULL) {
            Cliente* temp = atual;
            atual = atual->prox;
            free(temp);
        }
    }
    free(tabelaHash->tabela);
    free(tabelaHash);

    return 0;
}
