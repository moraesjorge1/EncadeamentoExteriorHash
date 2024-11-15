typedef struct Cliente {
    int codCliente;
    char nome[NOME_TAM];
    struct Cliente* prox;  // Para tratamento de colisão
} Cliente;

void initializeHashTable(Hash *tab, int m)
{
    int i;
    for (i = 0; i < m; i++)
    {
        tab[i] = NULL;
    }
}