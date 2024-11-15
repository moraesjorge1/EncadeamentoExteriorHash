#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NOME_TAM 100

typedef struct Cliente {
    int codCliente;
    char nome[NOME_TAM];
    struct Cliente* prox;  // Para tratamento de colisão
} Cliente;

typedef struct HashTable {
    Cliente** tabela;
    int tamanho;
    int numClientes;
    int fatorCarga;
    int duplicacoes;
    int proximoExpandir;
} HashTable;

void expandirTabela(HashTable* tabelaHash);

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
    return codCliente % (tamanho * (1 << duplicacoes));
}

void inserirCliente(HashTable* tabelaHash, int codCliente, const char* nome) {
    // Criar o novo cliente
    Cliente* novoCliente = (Cliente*)malloc(sizeof(Cliente));
    novoCliente->codCliente = codCliente;
    strncpy(novoCliente->nome, nome, NOME_TAM);
    novoCliente->prox = NULL;

    // Calcular índice usando a função hash
    int indice = funcaoHash(codCliente, tabelaHash->tamanho, tabelaHash->duplicacoes);

    // Buscar na lista associada ao índice
    Cliente* atual = tabelaHash->tabela[indice];
    Cliente* anterior = NULL;
    while (atual != NULL) {
        if (atual->codCliente == codCliente) {
            // Se o cliente já existe, sinalizar erro e não inserir
            printf("Erro: Cliente com código %d já existe.\n", codCliente);
            free(novoCliente); // Libera memória do novo cliente
            return;
        }
        anterior = atual;
        atual = atual->prox;
    }

    // Inserir o cliente no final da lista
    if (anterior == NULL) {
        // O índice está vazio, insere diretamente
        tabelaHash->tabela[indice] = novoCliente;
    } else {
        // Inserir no final da lista
        anterior->prox = novoCliente;
    }

    tabelaHash->numClientes++;

    // Verificar se é necessário expandir a tabela
    if ((float)tabelaHash->numClientes / tabelaHash->tamanho >= tabelaHash->fatorCarga) {
        printf("Expandindo tabela...\n");
        expandirTabela(tabelaHash);
    }
}

Cliente* buscarCliente(HashTable* tabelaHash, int codCliente) {
    // Calcular índice usando a função hash
    int indice = funcaoHash(codCliente, tabelaHash->tamanho, tabelaHash->duplicacoes);

    // Percorrer a lista encadeada associada ao índice
    Cliente* atual = tabelaHash->tabela[indice];

    // Percorrer a lista até encontrar o cliente ou chegar ao final
    while (atual != NULL) {
        if (atual->codCliente == codCliente) {
            // Cliente encontrado, retornar ponteiro para o cliente
            return atual;
        }
        atual = atual->prox;
    }

    // Se chegar ao final da lista, o cliente não foi encontrado
    return NULL;
}

int excluirCliente(HashTable* tabelaHash, int codCliente) {
    // Calcular o índice usando a função hash
    int indice = funcaoHash(codCliente, tabelaHash->tamanho, tabelaHash->duplicacoes);

    // Inicializar ponteiros para percorrer a lista encadeada
    Cliente* atual = tabelaHash->tabela[indice];
    Cliente* anterior = NULL;

    // Percorrer a lista encadeada associada ao índice
    while (atual != NULL) {
        if (atual->codCliente == codCliente) {
            // Cliente encontrado
            if (anterior == NULL) {
                // O cliente está no início da lista
                tabelaHash->tabela[indice] = atual->prox;
            } else {
                // Cliente está no meio ou fim da lista
                anterior->prox = atual->prox;
            }

            free(atual);  // Liberar memória do cliente
            tabelaHash->numClientes--;  // Decrementar contador de clientes
            return 1;  // Sucesso na exclusão
        }

        // Mover para o próximo cliente na lista
        anterior = atual;
        atual = atual->prox;
    }

    // Se chegar ao final da lista, o cliente não foi encontrado
    return 0;  // Erro: cliente não encontrado
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

int main() {
    // Inicializar a tabela hash
    int tamanhoInicial = 10;
    int fatorCarga = 2; // Exemplo de fator de carga
    HashTable* tabela = inicializarTabela(tamanhoInicial, fatorCarga);

    // Inserir alguns clientes
    printf("Inserindo clientes...\n");
    inserirCliente(tabela, 1, "Cliente 1");
    inserirCliente(tabela, 2, "Cliente 2");
    inserirCliente(tabela, 3, "Cliente 3");
    inserirCliente(tabela, 4, "Cliente 4");
    inserirCliente(tabela, 5, "Cliente 5");

    // Buscar clientes
    printf("\nBuscando clientes...\n");
    Cliente* cliente = buscarCliente(tabela, 3);
    if (cliente != NULL) {
        printf("Cliente encontrado: %d, %s\n", cliente->codCliente, cliente->nome);
    } else {
        printf("Cliente não encontrado.\n");
    }

    cliente = buscarCliente(tabela, 6);
    if (cliente != NULL) {
        printf("Cliente encontrado: %d, %s\n", cliente->codCliente, cliente->nome);
    } else {
        printf("Cliente não encontrado.\n");
    }

    // Excluir um cliente
    printf("\nExcluindo cliente 2...\n");
    if (excluirCliente(tabela, 2)) {
        printf("Cliente excluído com sucesso.\n");
    } else {
        printf("Erro: Cliente não encontrado.\n");
    }

    // Tentar excluir um cliente que não existe
    printf("\nTentando excluir cliente 6...\n");
    if (excluirCliente(tabela, 6)) {
        printf("Cliente excluído com sucesso.\n");
    } else {
        printf("Erro: Cliente não encontrado.\n");
    }

    // Buscar cliente excluído
    printf("\nBuscando cliente 2 (excluído)...\n");
    cliente = buscarCliente(tabela, 2);
    if (cliente != NULL) {
        printf("Cliente encontrado: %d, %s\n", cliente->codCliente, cliente->nome);
    } else {
        printf("Cliente não encontrado.\n");
    }

    // Limpar a tabela hash
    free(tabela->tabela);
    free(tabela);

    return 0;
}