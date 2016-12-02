/*
 * Recurso.h
 *
 *  Created on: 29 de nov de 2016
 *      Author: Jorge
 */

#ifndef RECURSO_H_
#define RECURSO_H_

class Recurso {
public:
    enum TipoRecurso{Scanner, Impressora, Modem, Sata};
    int scanner;
    int impressora[2];
    int modem;
    int sata[2];

	Recurso();
	virtual ~Recurso();

    bool alocaRecurso(int id, TipoRecurso tipo);

    // Checa se um processo possui recursos alocados
    bool checaRecurso(int id);

    // Libera os recursos de um tipo alocados para um processo
    void liberaRecurso(int id, TipoRecurso tipo);

    // Libera todos os recursos alocados para um processo
    void liberaTodos(int id);

};

#endif /* RECURSO_H_ */
