/*
 * Processo.h
 *
 *  Created on: 29 de nov de 2016
 *      Author: ana
 */

#ifndef PROCESSO_H_
#define PROCESSO_H_

class Processo {
public:
	int pid;
	int idade;
	int offset;
	int tempoInicializacao;
	int prioridade;
	int tempoProcessador;
	int blocoMem;
	int reqImpressora;
	int reqScanner;
	int reqModem;
	int codDisco;
	Processo();
	virtual ~Processo();
};

#endif /* PROCESSO_H_ */
