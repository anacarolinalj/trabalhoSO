/*
 * Memoria.cpp
 *
 *  Created on: 29 de nov de 2016
 *      Author: Jorge
 */

#include "Memoria.h"

Memoria::Memoria() {
	// Inicializa o array de blocos com -1
	for (int i = 0; i < MEMORY_SIZE; i++)
	{
		blocos[i] = -1;
	}

}

// Aloca um bloco de mem�ria
int Memoria::alocaBloco(int id, TipoProcesso tipo, int tamanho)
{
	int inicio = (tipo == TempoReal) ? 0 : USER_START;
	int fim = (tipo == TempoReal) ? 64 : MEMORY_SIZE;

	int contador = 0;
	int inicioAlocacao = -1;

	for (int i = inicio; i < fim; i++)
	{
		// Procura o primeiro bloco livre
		if (blocos[i] == -1)
		{
			inicioAlocacao = i;

			// Verifica se existem blocos livres cont�guos suficientes para a aloca��o
			while ( (contador < tamanho) && (i + contador < fim) && (blocos[i + contador] == -1))
				contador++;

			// � poss�vel fazer a aloca��o
			if (contador == tamanho)
			{
				while (contador >= 0)
				{
					blocos[i + contador] = id;
					contador--;
				}

				// Sai do loop
				break;
			}
			else
				inicioAlocacao = -1;
		}
	}

	return inicioAlocacao;
}

void Memoria::liberaBloco(int id)
{
    int i = 0;

    while(blocos[i] != id && i < MEMORY_SIZE)
    {
        i++;
    }

    while(blocos[i] == id && i < MEMORY_SIZE)
    {
        blocos[i] = -1;
        i++;
    }

    return;
}

Memoria::~Memoria() {
	// TODO Auto-generated destructor stub
}




