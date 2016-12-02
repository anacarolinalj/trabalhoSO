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

using namespace std;
#include <iostream>

void Filas::promoverProcessoComRecurso(int prioridade, Recurso gerenteRecursos, Recurso::TipoRecurso tipo) {
	Processo *processo;
	int prioridadeRecurso = -1;
	int pidRecurso;

	// Descobre a qual fila o processo que possui o recurso pertence.
	for (std::list<Processo>::iterator it = filaGlobal.begin(); it != filaGlobal.end(); it++) {
		switch (tipo) {
			case Recurso::Scanner:
				if (gerenteRecursos.scanner == it->pid) {
					prioridadeRecurso = it->prioridade;
					pidRecurso = it->pid;
					processo = &(*it);
				}
				break;

			case Recurso::Impressora:
				if (gerenteRecursos.impressora[0] == it->pid) {
					if ((prioridadeRecurso == -1) || (prioridadeRecurso > it->prioridade)) {
						prioridadeRecurso = it->prioridade;
						pidRecurso = it->pid;
						processo = &(*it);
					}

				} else if (gerenteRecursos.impressora[1] == it->pid) {
					if ((prioridadeRecurso == -1) || (prioridadeRecurso > it->prioridade)) {
						prioridadeRecurso = it->prioridade;
						pidRecurso = it->pid;
						processo = &(*it);
					}
				}
				break;
			case Recurso::Modem:
				if (gerenteRecursos.modem == it->pid) {
					prioridadeRecurso = it->prioridade;
					pidRecurso = it->pid;
					processo = &(*it);
				}
				break;
			case Recurso::Sata:
				if (gerenteRecursos.impressora[0] == it->pid) {
					if ((prioridadeRecurso == -1) || (prioridadeRecurso > it->prioridade)) {
						prioridadeRecurso = it->prioridade;
						pidRecurso = it->pid;
						processo = &(*it);
					}

				} else if (gerenteRecursos.impressora[1] == it->pid) {
					if ((prioridadeRecurso == -1) || (prioridadeRecurso > it->prioridade)) {
						prioridadeRecurso = it->prioridade;
						pidRecurso = it->pid;
						processo = &(*it);
					}
				}
				break;
		}
	}

	// Caso a prioridade do processo que tem posse do recurso seja igual a
	//do processo que está solicitando o recurso não haverá promoção.
	if (prioridadeRecurso == prioridade) return;

	list<Processo> *filaOriginal, *filaNova;

	switch (prioridadeRecurso) {
		case 0: filaOriginal = &processosReais ; break;
		case 1: filaOriginal = &processosUsuario.fila1; break;
		case 2: filaOriginal = &processosUsuario.fila2; break;
		case 3: filaOriginal = &processosUsuario.fila3; break;
	}

	switch (prioridade) {
			case 0: filaNova = &processosReais ; break;
			case 1: filaNova = &processosUsuario.fila1; break;
			case 2: filaNova = &processosUsuario.fila2; break;
			case 3: filaNova = &processosUsuario.fila3; break;
	}

	// Processo é movido para a nova fila.
	for (std::list<Processo>::iterator it = filaOriginal->begin(); it != filaOriginal->end(); it++) {
			if (it->pid == pidRecurso) {
				// Prioridade do processo é alterada na fila global.
				processo->prioridade = prioridade;
				it->prioridade = prioridade;

				// Processo é salvo na nova fila.
				filaNova->push_back(*it);

				// Processo retirado da fila antiga.
				filaOriginal->erase(it);
				return;
			}
	}
}

void Filas::removerProcesso(int prioridade, int pid) {
	list<Processo> *fila;

	switch (prioridade) {
		case 0: fila = &processosReais ; break;
		case 1: fila = &processosUsuario.fila1; break;
		case 2: fila = &processosUsuario.fila2; break;
		case 3: fila = &processosUsuario.fila3; break;
	}

	// Apaga processo da sua fila.
	for (std::list<Processo>::iterator it = fila->begin(); it != fila->end(); it++) {
		if (it->pid == pid) {
			fila->erase(it);
			break;
		}
	}

	// Apaga processo da fila global.
	for (std::list<Processo>::iterator it = filaGlobal.begin(); it != filaGlobal.end(); it++) {
		if (it->pid == pid) {
			fila->erase(it);
			break;
		}
	}

}

void Filas::promoverProcessos() {
	if (!processosUsuario.fila1.empty()) {
		for (std::list<Processo>::iterator it = processosUsuario.fila1.begin(); it != processosUsuario.fila1.end(); it++) {
			//cout << "PID: " << it->pid << "--" << "IDADE: " << it->idade << "\n";

			// Se a idade limite ultrapassar o permitido, processo aumenta de prioridade.
			if ((it->idade >= IDADE_LIMITE) && (it->blocoMem <  MEMORY_REAL)) {
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

void Filas::adicionarFilas(Processo processo) {
	filaGlobal.push_back(processo);
	switch (processo.prioridade) {
			case 0:
				if (processosReais.size() < 1000)
					processosReais.push_back(processo);
				else cout << "\nFila de processos reais cheia.";
				break;
			case 1:
				if (processosUsuario.fila1.size() < 1000)
					processosUsuario.fila1.push_back(processo);
				else cout << "\nFila de processos de prioridade 1 cheia.";
				break;
			case 2:
				if (processosUsuario.fila2.size() < 1000)
					processosUsuario.fila2.push_back(processo);
				else cout << "\nFila de processos de prioridade 2 cheia.";
				break;
			case 3:
				if (processosUsuario.fila3.size() < 1000)
					processosUsuario.fila3.push_back(processo);
				else cout << "\nFila de processos de prioridade 3 cheia.";
				break;
	}
}
