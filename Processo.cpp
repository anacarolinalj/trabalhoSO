#include "Processo.h"

using namespace std;

Processo::Processo(int tempoInicializacao, int prioridade, int tempoProcessador,
				   int  blocoMem, int  reqImpressora, int reqScanner, int reqModem,
				   int reqDisco, int idade, int offset, int  pid) {
	this->tempoInicializacao = tempoInicializacao;
	this->prioridade = prioridade;
	this->tempoProcessador = tempoProcessador;
	this->blocoMem = blocoMem;
	this->reqImpressora = reqImpressora;
	this->reqScanner = reqScanner;
	this->reqModem = reqModem;
	this->reqDisco = reqDisco;
	this->idade = idade;
	this->offset = offset;
	this->pid = pid;
}

bool Processo::precisaRecursos()
{
    if(reqImpressora || reqModem || reqScanner || reqDisco)
        return true;

    return false;
}

Processo::~Processo() {

}

