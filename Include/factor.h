#ifndef FACT_H
#define FACT_H


int checks_factor(FILE* tabClientes, int table_size, float load_factor_limit) {
    // Contar o número de elementos no arquivo de clientes
    int num_elements = 0;
    Client current_client;
    fseek(tabClientes, 0, SEEK_SET);
    
    // Contar clientes ocupados
    while (fread(&current_client, sizeof(Client), 1, tabClientes) == 1) {
        if (current_client.ocupado) {
            num_elements++;
        }
    }

    // Calcular o fator de carga
    float load_factor = (float)num_elements / table_size;

    // Verificar se o fator de carga ultrapassa o limite
    if (load_factor > load_factor_limit) {
        return 1;  // A tabela precisa ser expandida
    } else {
        return 0;  // Não precisa expandir
    }
}

#endif