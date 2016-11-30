#include "Processo.h"

Processo::Processo() {

}

bool Processo::precisaRecursos()
{
    if(reqImpressora || reqModem || reqScanner || codDisco)
        return true;

    return false;
}

Processo::~Processo() {

}

