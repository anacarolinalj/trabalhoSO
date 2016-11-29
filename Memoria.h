/*
 * Memoria.h
 *
 *  Created on: 29 de nov de 2016
 *      Author: ana
 */

#ifndef MEMORIA_H_
#define MEMORIA_H_

class Memoria {
public:
	enum TipoProcesso{TempoReal, Usuario};
	Memoria();
	virtual ~Memoria();

	/*
    //
	*/

	int alocaBloco(int id, TipoProcesso tipo, int tamanho);
	void liberaBloco(int id);

private:
	static const int MEMORY_SIZE = 1024; // Tamanho máximo da memória, sendo dividida em Tempo Real | Usuário
	static const int USER_START = 64; // Indice do início da região de usuário
	int blocos[MEMORY_SIZE];
};

#endif /* MEMORIA_H_ */
