/*
 * Recurso.cpp
 *
 *  Created on: 29 de nov de 2016
 *      Author: ana
 */

#include "Recurso.h"

Recurso::Recurso() {
	// Inicializa recursos com -1
	scanner = -1;
    impressora[0] = -1;
    impressora[1] = -1;
    modem = -1;
    sata[0] = -1;
    sata[1] = -1;
}

Recurso::~Recurso() {
	// TODO Auto-generated destructor stub
}

bool Recurso::alocaRecurso(int id, TipoRecurso tipo)
{
    switch(tipo)
    {
    case Scanner:
        if(scanner == -1)
        {
            scanner = id;
            return true;
        }
        return false;

    case Impressora:
        if(impressora[0] == -1)
        {
            impressora[0] = id;
            return true;
        }

        if(impressora[1] == -1)
        {
            impressora[1] = id;
            return true;
        }
        return false;

    case Modem:
        if(modem == -1)
        {
            modem = id;
            return true;
        }
        return false;
    case Sata:
        if(sata[0] == -1)
        {
            sata[0] = id;
            return true;
        }

        if(sata[1] == -1)
        {
            sata[1] = id;
            return true;
        }
        return false;
    }

    return false;
}

void Recurso::liberaRecurso(int id, TipoRecurso tipo)
{
    switch(tipo)
    {
    case Scanner:
        if(scanner == id)
            scanner = -1;
        return;

    case Impressora:
        if(impressora[0] == id)
            impressora[0] = -1;

        if(impressora[1] == id)
            impressora[1] = -1;

        return;

    case Modem:
        if(modem == id)
            modem = -1;

        return;
    case Sata:
        if(sata[0] == id)
            sata[0] = -1;

        if(sata[1] == id)
            sata[1] = -1;

        return;
    }
}

void Recurso::liberaTodos(int id)
{
    if(scanner == id) scanner = -1;
    if(impressora[0] == id) impressora[0] = -1;
    if(impressora[1] == id) impressora[1] = -1;
    if(modem == id) modem = -1;
    if(sata[0] == id)  sata[0] = -1;
    if(sata[1] == id)  sata[1] = -1;
}

