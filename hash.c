#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NOME_TAM 100

// Estrutura para representar um cliente
typedef struct Cliente {
    int codCliente;
    char nome[NOME_TAM];
    struct Cliente* prox;  // Para tratamento de colisão
} Cliente;

// Estrutura da Tabela Hash
typedef struct HashTable {
    Cliente** tabela;
    int tamanho;
    int numClientes;  // Contador de elementos
    int fatorCarga;   // Fator de carga máximo antes da expansão
    int duplicacoes;  // Contador de expansões
    int proximoExpandir;  // Índice do próximo compartimento a ser expandido
} HashTable;

HashTable* inicializarTabela(int tamanhoInicial, int fatorCarga) {
    HashTable* tabelaHash = (HashTable*)malloc(sizeof(HashTable));
    tabelaHash->tamanho = tamanhoInicial;
    tabelaHash->numClientes = 0;
    tabelaHash->fatorCarga = fatorCarga;
    tabelaHash->duplicacoes = 0;
    tabelaHash->proximoExpandir = 0;

    tabelaHash->tabela = (Cliente**)malloc(tamanhoInicial * sizeof(Cliente*));
    for (int i = 0; i < tamanhoInicial; i++) {
        tabelaHash->tabela[i] = NULL;
    }

    return tabelaHash;
}

int funcaoHash(int codCliente, int tamanho, int duplicacoes) {
    return codCliente % (tamanho * (1 << duplicacoes));  // m * 2^l
}

void inserirCliente(HashTable* tabelaHash, int codCliente, const char* nome) {
    // Criar o novo cliente
    Cliente* novoCliente = (Cliente*)malloc(sizeof(Cliente));
    novoCliente->codCliente = codCliente;
    strncpy(novoCliente->nome, nome, NOME_TAM);
    novoCliente->prox = NULL;

    // Calcular índice usando a função hash
    int indice = funcaoHash(codCliente, tabelaHash->tamanho, tabelaHash->duplicacoes);

    // Inserir o cliente na lista encadeada
    if (tabelaHash->tabela[indice] == NULL) {
        tabelaHash->tabela[indice] = novoCliente;
    } else {
        Cliente* atual = tabelaHash->tabela[indice];
        while (atual->prox != NULL) {
            atual = atual->prox;
        }
        atual->prox = novoCliente;
    }

    tabelaHash->numClientes++;

    // Verificar se é necessário expandir a tabela
    if ((float)tabelaHash->numClientes / tabelaHash->tamanho >= tabelaHash->fatorCarga) {
        // Expandir tabela
        printf("Expandindo tabela...\n");
        // Lógica de expansão vai aqui...
    }
}

void expandirTabela(HashTable* tabelaHash) {
    // Expansão para uma tabela maior
    int novoTamanho = tabelaHash->tamanho * 2;
    Cliente** novaTabela = (Cliente**)malloc(novoTamanho * sizeof(Cliente*));

    for (int i = 0; i < novoTamanho; i++) {
        novaTabela[i] = NULL;
    }

    // Re-hashing dos clientes
    for (int i = 0; i < tabelaHash->tamanho; i++) {
        Cliente* atual = tabelaHash->tabela[i];
        while (atual != NULL) {
            Cliente* prox = atual->prox;

            // Re-calcular o índice para a nova tabela
            int novoIndice = funcaoHash(atual->codCliente, novoTamanho, tabelaHash->duplicacoes);
            atual->prox = novaTabela[novoIndice];
            novaTabela[novoIndice] = atual;

            atual = prox;
        }
    }

    free(tabelaHash->tabela);
    tabelaHash->tabela = novaTabela;
    tabelaHash->tamanho = novoTamanho;
    tabelaHash->duplicacoes++;
}

void imprimirTabela(HashTable* tabelaHash) {
    for (int i = 0; i < tabelaHash->tamanho; i++) {
        printf("Índice %d: ", i);
        Cliente* atual = tabelaHash->tabela[i];
        if (atual == NULL) {
            printf("Vazio\n");
        } else {
            while (atual != NULL) {
                printf("Cliente(cod: %d, nome: %s) -> ", atual->codCliente, atual->nome);
                atual = atual->prox;
            }
            printf("NULL\n");
        }
    }
}


int main() {
    HashTable* tabela = inicializarTabela(10, 3);  // Tamanho inicial e fator de carga
    inserirCliente(tabela, 1, "Cliente 1");
    inserirCliente(tabela, 2, "Cliente 2");
    inserirCliente(tabela, 3, "Cliente 3");

    // Imprimir a tabela após as inserções
    imprimirTabela(tabela);

    return 0;
}

