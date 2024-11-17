#ifndef HASH_H
#define HASH_H

#include <stdio.h>
#include <stdlib.h>

#define HASH_TABLE_SIZE 10

typedef struct {
    int* table;
    int table_size;
    int num_elements;
    int L;  // Exponente para o tamanho da tabela
    int P;  // Por exemplo, número de colisões
} HashTable;

int checks_factor(FILE *tabClientes, int table_size, float load_factor_limit);

void expands_table(char* dir_tabClientes, char* dir_tabHash, int* table_size, int* L, int* P);




#endif // HASH_H