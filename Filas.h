/*
 * Filas.h
 *
 *  Created on: 29 de nov de 2016
 *      Author: ana
 */

#ifndef FILAS_H_
#define FILAS_H_

#include <list>
#include "Processo.h"

#define MEMORY_REAL 64

class FilaProcessosUsuario {
public:
	std::list<Processo> fila1;
	std::list<Processo> fila2;
	std::list<Processo> fila3;
};

class Filas {
public:
	std::list<Processo> processosReais;
	FilaProcessosUsuario processosUsuario;
	Filas();
	virtual ~Filas();

	void adicionarFilas(Processo processo, std::list<Processo> *processos);
	void promoverProcessos();
	void age();
};

#endif /* FILAS_H_ */
