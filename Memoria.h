/*
 * Memoria.h
 *
 *  Created on: 29 de nov de 2016
 *      Author: Jorge
 */

#ifndef MEMORIA_H_
#define MEMORIA_H_

class Memoria {
public:
	enum TipoProcesso{TempoReal, Usuario};
	Memoria();
	virtual ~Memoria();

	// Aloca um bloco de memória contíguo
    // Retorna a posição inicial do bloco alocado(iniciando em 0), ou -1 se não foi possível alocar
	int alocaBloco(int id, TipoProcesso tipo, int tamanho);

	// Libera o bloco de memória alocado para um processo
	void liberaBloco(int id);

private:
	static const int MEMORY_SIZE = 1024; // Tamanho máximo da memória, sendo dividida em Tempo Real | Usuário
	static const int USER_START = 64; // Indice do início da região de usuário
	int blocos[MEMORY_SIZE];
};

#endif /* MEMORIA_H_ */
