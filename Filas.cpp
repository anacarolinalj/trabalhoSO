/*
 * Filas.cpp
 *
 *  Created on: 29 de nov de 2016
 *      Author: ana
 */

#include "Filas.h"

#define QUANTUM 1
#define IDADE_LIMITE 25

Filas::Filas() {
	// TODO Auto-generated constructor stub

}

Filas::~Filas() {
	// TODO Auto-generated destructor stub
}

void Filas::adicionarProcesso(Processo processo) {
	switch (processo.prioridade) {
		case 0: processosReais.push_back(processo) ; break;
		case 1: processosUsuario.fila1.push_back(processo); break;
		case 2: processosUsuario.fila2.push_back(processo); break;
		case 3: processosUsuario.fila3.push_back(processo); break;
	}
}

using namespace std;
#include <iostream>

void Filas::promoverProcesso() {
	if (!processosUsuario.fila1.empty()) {
		for (std::list<Processo>::iterator it = processosUsuario.fila1.begin(); it != processosUsuario.fila1.end(); it++) {
			//cout << "PID: " << it->pid << "--" << "IDADE: " << it->idade << "\n";

			// Se a idade limite ultrapassar o permitido, processo aumenta de prioridade.
			if (it->idade >= IDADE_LIMITE) {
				it->prioridade = 0;
				it->idade = 0;
				processosReais.push_back(*it);
				processosUsuario.fila1.erase(it);
				it = processosUsuario.fila1.begin();
			}
		}
	}

	if (!processosUsuario.fila2.empty()) {
		for (std::list<Processo>::iterator it = processosUsuario.fila2.begin(); it != processosUsuario.fila2.end(); it++) {
			//cout << "PID: " << it->pid << "--" << "IDADE: " << it->idade << "\n";

			// Se a idade limite ultrapassar o permitido, processo aumenta de prioridade.
			if (it->idade >= IDADE_LIMITE) {
				it->prioridade = 1;
				it->idade = 0;
				processosUsuario.fila1.push_back(*it);
				processosUsuario.fila2.erase(it);
				it = processosUsuario.fila2.begin();
			}
		}
	}

	if (!processosUsuario.fila3.empty()) {
		for (std::list<Processo>::iterator it = processosUsuario.fila3.begin(); it != processosUsuario.fila3.end(); it++) {

			//cout << "PID: " << it->pid << "--" << "IDADE: " << it->idade << "\n";

			// Se a idade limite ultrapassar o permitido, processo aumenta de prioridade.
			if (it->idade >= IDADE_LIMITE) {
				it->prioridade = 2;
				it->idade = 0;
				processosUsuario.fila2.push_back(*it);
				processosUsuario.fila3.erase(it);
				it = processosUsuario.fila3.begin();
			}
		}
	}
}

void Filas::age() {

	for (std::list<Processo>::iterator it = processosUsuario.fila1.begin(); it != processosUsuario.fila1.end(); it++) {
		it->idade += QUANTUM;
	}

	for (std::list<Processo>::iterator it = processosUsuario.fila2.begin(); it != processosUsuario.fila2.end(); it++) {
		it->idade += QUANTUM;
	}

	for (std::list<Processo>::iterator it = processosUsuario.fila3.begin(); it != processosUsuario.fila3.end(); it++) {
		it->idade += QUANTUM;
	}
}

