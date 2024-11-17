#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include "../Include/cliente.h"
#include "../Include/hash.h"
#include "../Include/factor.h"

// Função de hash com base no código do cliente e tamanho da tabela
int hashFunction(int cod, int table_size, int L) {
    return cod % (int)(table_size * pow(2, L));
}

// Função para inserir um cliente na tabela hash
void insert(HashTable* ht, Client cliente, char* dir_tabClientes, char* dir_tabHash, float load_factor_limit) {
    int index = hashFunction(cliente.codCliente, ht->table_size, ht->L);
    int offset;

    FILE* tabHash = fopen(dir_tabHash, "r+b");
    if (tabHash == NULL) {
        perror("Erro ao abrir tabHash");
        exit(1);
    }

    // Lê a posição do primeiro cliente (se houver colisão)
    int current_index_value;
    fseek(tabHash, index * sizeof(int), SEEK_SET);
    fread(&current_index_value, sizeof(int), 1, tabHash);

    FILE* tabClientes = fopen(dir_tabClientes, "r+b");
    if (tabClientes == NULL) {
        perror("Erro ao abrir tabClientes");
        fclose(tabHash);
        exit(1);
    }

    // Busca um slot inativo no arquivo de clientes
    int found_inactive_slot = 0;
    int inactive_offset;
    Client current_client;
    fseek(tabClientes, 0, SEEK_SET);

    while (fread(&current_client, sizeof(Client), 1, tabClientes) == 1) {
        if (!current_client.ocupado) {
            inactive_offset = ftell(tabClientes) - sizeof(Client);
            found_inactive_slot = 1;
            break;
        }
    }

    if (found_inactive_slot) {
        // Slot inativo encontrado, insere o cliente lá
        cliente.posicao = -1;
        cliente.ocupado = true;
        fseek(tabClientes, inactive_offset, SEEK_SET);
        fwrite(&cliente, sizeof(Client), 1, tabClientes);

        // Atualiza o índice na tabela de hash
        if (current_index_value == -1) {
            fseek(tabHash, index * sizeof(int), SEEK_SET);
            fwrite(&inactive_offset, sizeof(int), 1, tabHash);
        } else {
            // Encadeamento exterior: procura o final da lista de colisões
            int last_offset = current_index_value;
            while (last_offset != -1) {
                fseek(tabClientes, last_offset, SEEK_SET);
                fread(&current_client, sizeof(Client), 1, tabClientes);

                if (current_client.posicao == -1) {
                    current_client.posicao = inactive_offset;
                    fseek(tabClientes, last_offset, SEEK_SET);
                    fwrite(&current_client, sizeof(Client), 1, tabClientes);
                    break;
                }

                last_offset = current_client.posicao;
            }
        }
    } else {
        // Não encontrou slot inativo, insere no final do arquivo
        fseek(tabClientes, 0, SEEK_END);
        offset = ftell(tabClientes);
        cliente.posicao = -1;
        cliente.ocupado = true;
        fwrite(&cliente, sizeof(Client), 1, tabClientes);

        // Atualiza o índice na tabela de hash
        if (current_index_value == -1) {
            fseek(tabHash, index * sizeof(int), SEEK_SET);
            fwrite(&offset, sizeof(int), 1, tabHash);
        } else {
            // Encadeamento exterior: procura o final da lista de colisões
            int last_offset = current_index_value;
            while (last_offset != -1) {
                fseek(tabClientes, last_offset, SEEK_SET);
                fread(&current_client, sizeof(Client), 1, tabClientes);

                if (current_client.posicao == -1) {
                    current_client.posicao = offset;
                    fseek(tabClientes, last_offset, SEEK_SET);
                    fwrite(&current_client, sizeof(Client), 1, tabClientes);
                    break;
                }

                last_offset = current_client.posicao;
            }
        }
    }

    // Verifica o fator de carga e expande a tabela se necessário
    if (checks_factor(tabClientes, ht->table_size, load_factor_limit) == 1) {
        printf("\n---------------------\nEXPANDINDO\n---------------------\n");
        expands_table(dir_tabClientes, dir_tabHash, &ht->table_size, &ht->L, &ht->P);
    }

    fclose(tabClientes);
    fclose(tabHash);
}


// Inicializa a tabela de hash
void init_hash_table(HashTable *ht, int table_size, int L) {
    ht->table_size = table_size;
    ht->num_elements = 0;
    ht->L = L;
    ht->table = (int *)malloc(table_size * sizeof(int));

    // Inicializa a tabela com -1 (indicando que não há elementos)
    for (int i = 0; i < table_size; i++) {
        ht->table[i] = -1;
    }
}

// Imprime a tabela de hash
void print_table(char* dir_tabHash, char* dir_tabClientes, int table_size) {
    FILE* tabHash = fopen(dir_tabHash, "r+b");
    if (!tabHash) {
        perror("Erro ao abrir tabHash");
        exit(1);
    }

    FILE* tabClientes = fopen(dir_tabClientes, "r+b");
    if (!tabClientes) {
        perror("Erro ao abrir tabClientes");
        fclose(tabHash);
        exit(1);
    }

    printf("Tabela de Hash:\n");
    for (int i = 0; i < table_size; i++) {
        int offset;
        fseek(tabHash, i * sizeof(int), SEEK_SET);
        fread(&offset, sizeof(int), 1, tabHash);

        if (offset != -1) {
            printf("Índice %d: ", i);
            while (offset != -1) {
                Client cliente;
                fseek(tabClientes, offset, SEEK_SET);
                fread(&cliente, sizeof(Client), 1, tabClientes);
                printf("Código: %d, Nome: %s -> ", cliente.codCliente, cliente.nome);
                offset = cliente.posicao;
            }
            printf("\n");
        } else {
            printf("Índice %d: Vazio\n", i);
        }
    }

    fclose(tabClientes);
    fclose(tabHash);
}

void expands_table(char* dir_tabClientes, char* dir_tabHash, int* table_size, int* L, int* P) {
    FILE* tabHash = fopen(dir_tabHash, "r+b");
    if (tabHash == NULL) {
        perror("Erro ao abrir tabHash");
        exit(1);
    }

    FILE* tabClientes = fopen(dir_tabClientes, "r+b");
    if (tabClientes == NULL) {
        perror("Erro ao abrir tabClientes");
        fclose(tabHash);
        exit(1);
    }

    // Aumenta o tamanho da tabela de hash
    int new_table_size = *table_size * 2;

    // Inicializa a nova tabela com -1 (vazio)
    for (int i = 0; i < new_table_size; i++) {
        fseek(tabHash, i * sizeof(int), SEEK_SET);
        int empty = -1;
        fwrite(&empty, sizeof(int), 1, tabHash);
    }

    // Re-hash e redistribui os clientes na nova tabela
    for (int i = 0; i < *table_size; i++) {
        fseek(tabHash, i * sizeof(int), SEEK_SET);
        int index_value;
        fread(&index_value, sizeof(int), 1, tabHash);

        if (index_value != -1) {  // Cliente encontrado
            Client cliente;
            fseek(tabClientes, index_value, SEEK_SET);
            fread(&cliente, sizeof(Client), 1, tabClientes);

            // Calcula a nova posição na tabela expandida usando a função de hash
            int new_index = hashFunction(cliente.codCliente, new_table_size, *L);

            // Realiza sondagem linear para encontrar o próximo índice livre
            while (1) {
                fseek(tabHash, new_index * sizeof(int), SEEK_SET);
                int hash_value;
                fread(&hash_value, sizeof(int), 1, tabHash);

                if (hash_value == -1) {  // Posição livre
                    fseek(tabHash, new_index * sizeof(int), SEEK_SET);
                    fwrite(&index_value, sizeof(int), 1, tabHash);  // Grava o índice do cliente
                    break;
                }

                // Sondagem linear: move para o próximo índice
                new_index = (new_index + 1) % new_table_size;
            }
        }
    }

    // Atualiza o tamanho da tabela e o fator de carga
    *table_size = new_table_size;
    (*L)++;

    fclose(tabClientes);
    fclose(tabHash);
}


int main() {
    // Definir os diretórios dos arquivos binários
    char* dir_tabClientes = "clientes.dat";
    char* dir_tabHash = "hash.dat";

    // Inserir 20 clientes no arquivo clientes.dat
    inserir_clientes(dir_tabClientes);

    // Inicializar a tabela de hash
    HashTable ht;
    init_hash_table(&ht, 10, 0); // Tamanho inicial da tabela de hash 10, e L inicial 0

    // Criar alguns clientes para testar a inserção
    Client clientes[] = {
        {101, "Cliente A", false, -1},
        {102, "Cliente B", false, -1},
        {103, "Cliente C", false, -1},
        {104, "Cliente D", false, -1},
        {105, "Cliente E", false, -1}
    };

    // Inicializar arquivo da tabela de hash com valores -1
    FILE* tabHash = fopen(dir_tabHash, "wb");
    if (tabHash == NULL) {
        perror("Erro ao abrir o arquivo hash.dat");
        return 1;
    }

    // Inicializar a tabela hash com valores vazios (-1)
    for (int i = 0; i < ht.table_size; i++) {
        int empty = -1;
        fwrite(&empty, sizeof(int), 1, tabHash);
    }

    fclose(tabHash);

    // Inserir os clientes na tabela hash
    for (int i = 0; i < 5; i++) {
        insert(&ht, clientes[i], dir_tabClientes, dir_tabHash, 0.7);
    }

    // Imprimir a tabela de hash após inserção
    printf("\nTabela de Hash após inserção dos clientes:\n");
    print_table(dir_tabHash, dir_tabClientes, ht.table_size);

    // Testar a expansão da tabela de hash
    printf("\nInserindo mais clientes para testar expansão:\n");
    Client novos_clientes[] = {
        {106, "Cliente F", false, -1},
        {107, "Cliente G", false, -1},
        {108, "Cliente H", false, -1}
    };
    
    // Inserir novos clientes e testar expansão
    for (int i = 0; i < 3; i++) {
        insert(&ht, novos_clientes[i], dir_tabClientes, dir_tabHash, 0.7);
    }

    // Imprimir a tabela de hash após a expansão
    printf("\nTabela de Hash após expansão e inserção:\n");
    print_table(dir_tabHash, dir_tabClientes, ht.table_size);

    return 0;
}

