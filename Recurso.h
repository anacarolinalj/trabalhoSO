/*
 * Recurso.h
 *
 *  Created on: 29 de nov de 2016
 *      Author: ana
 */

#ifndef RECURSO_H_
#define RECURSO_H_

class Recurso {
public:
    enum TipoRecurso{Scanner, Impressora, Modem, Sata};
	Recurso();
	virtual ~Recurso();

    bool alocaRecurso(int id, TipoRecurso tipo);

    // Libera os recursos de um tipo alocados para um processo
    void liberaRecurso(int id, TipoRecurso tipo);

    // Libera todos os recursos alocados para um processo
    void liberaTodos(int id);

private:
    int scanner;
    int impressora[2];
    int modem;
    int sata[2];
};

#endif /* RECURSO_H_ */
