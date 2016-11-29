#include "PseudoSO.h"
#include "Processo.h"
#include "Memoria.h"
#include "Recurso.h"
#include <iostream>
#include <fstream>
#include <istream>
#include <string>
#include <cstring>
#include <stdlib.h>
#include <list>

using namespace std;


std::list<Processo> lerArquivo(ifstream & arquivo);
void imprimirProcesso (Processo processo);
void imprimirProcessos(std::list<Processo> processos);


// Apenas para testes, remover depois
void testeMemoria()
{
    // Testes provisório do uso da memória
	Memoria gerenteMemoria;
	int inicio;

	inicio = gerenteMemoria.alocaBloco(1, Memoria::TempoReal, 30);
	cout << inicio << '\n';

	inicio = gerenteMemoria.alocaBloco(2, Memoria::TempoReal, 30);
	cout << inicio << '\n';

	gerenteMemoria.liberaBloco(2);
	gerenteMemoria.liberaBloco(3);

	inicio = gerenteMemoria.alocaBloco(3, Memoria::TempoReal, 30);
	cout << inicio << '\n';

	inicio = gerenteMemoria.alocaBloco(4, Memoria::Usuario, 30);
	cout << inicio << '\n';

	inicio = gerenteMemoria.alocaBloco(5, Memoria::Usuario, 500);
	cout << inicio << '\n';

	inicio = gerenteMemoria.alocaBloco(6, Memoria::Usuario, 500);
	cout << inicio << '\n';
}

// Apenas para testes, remover depois
void testeRecursos()
{
    // Testes provisórios da alocação de recursos
    Recurso gerenteRecursos;

    int resultado;

    resultado = gerenteRecursos.alocaRecurso(1, Recurso::Scanner);
    cout << "Recurso alocado? : " << resultado << '\n';

    resultado = gerenteRecursos.alocaRecurso(1, Recurso::Scanner);
    cout << "Recurso alocado? : " << resultado << '\n';

    gerenteRecursos.liberaRecurso(1, Recurso::Scanner);
    resultado = gerenteRecursos.alocaRecurso(1, Recurso::Scanner);
    cout << "Recurso alocado? : " << resultado << '\n';

    resultado = gerenteRecursos.alocaRecurso(1, Recurso::Scanner);
    cout << "Recurso alocado? : " << resultado << '\n';

    resultado = gerenteRecursos.alocaRecurso(1, Recurso::Impressora);
    cout << "Recurso alocado? : " << resultado << '\n';

    resultado = gerenteRecursos.alocaRecurso(1, Recurso::Impressora);
    cout << "Recurso alocado? : " << resultado << '\n';

    resultado = gerenteRecursos.alocaRecurso(1, Recurso::Modem);
    cout << "Recurso alocado? : " << resultado << '\n';

    gerenteRecursos.liberaTodos(1);
    resultado = gerenteRecursos.alocaRecurso(1, Recurso::Scanner);
    cout << "Recurso alocado? : " << resultado << '\n';

    resultado = gerenteRecursos.alocaRecurso(1, Recurso::Impressora);
    cout << "Recurso alocado? : " << resultado << '\n';

    resultado = gerenteRecursos.alocaRecurso(1, Recurso::Impressora);
    cout << "Recurso alocado? : " << resultado << '\n';

    resultado = gerenteRecursos.alocaRecurso(1, Recurso::Modem);
    cout << "Recurso alocado? : " << resultado << '\n';
}

int main(int argc, const char *argv[]) {
	string nomeArq;
	ifstream arquivo;
	std::list<Processo> processos;

    testeMemoria();
    testeRecursos();


	// Verifica se nome do arquivo foi informado antes da execução.
	/*if (argc < 2) {
		cout << "\nInforme o nome do arquivo com os processos: ";
		cin >> nomeArq;
	} else nomeArq = argv[1];*/

	nomeArq = "processos.txt";

	arquivo.open(nomeArq.c_str(), ios::in);

	// Verifica se arquivo foi aberto.
	if (arquivo.is_open()) {
		processos = lerArquivo(arquivo);

		imprimirProcessos(processos);
	}
}

std::list<Processo> lerArquivo(ifstream & arquivo) {
	std::list<Processo> processos;

	string linha;


	// Lê arquivo linha a linha.
	while(!arquivo.eof()){
		int pid = 1;

		std::getline(arquivo,linha);

		if (!linha.empty()) {
			Processo processo;
			char *dados = std::strtok(&linha[0], ", ");

			int it = 0;


			// Coloca as informações no processo.
			processo.pid = pid++;
			processo.offset = 0;
			while (dados != NULL) {
				switch (it) {
					case 0: processo.tempoInicializacao = atoi(dados); break;
					case 1: processo.prioridade = atoi(dados); break;
					case 2: processo.tempoProcessador = atoi(dados); break;
					case 3: processo. blocoMem = atoi(dados); break;
					case 4: processo.reqImpressora = atoi(dados); break;
					case 5: processo.reqScanner = atoi(dados); break;
					case 6: processo.reqModem = atoi(dados); break;
					case 7: processo.codDisco = atoi(dados); break;
				}
				dados = std::strtok(NULL, ", ");
				it++;
			}

			// Adiciona novo processo à lista de processos
			processos.push_back(processo);
		}
	}
	arquivo.close();

	return processos;
}

void imprimirProcesso (Processo processo) {
	cout << "PID: " << processo.pid << '\n';
	cout << "offset: " << processo.offset << '\n';
	cout << "blocks: " << processo.blocoMem << '\n';
	cout << "priority: " << processo.prioridade << '\n';
	cout << "time: " << processo.tempoProcessador << '\n';
	cout << "printers: " << processo.reqImpressora << '\n';
	cout << "scanners: " << processo.reqScanner << '\n';
	cout << "modems: " << processo.reqModem << '\n';
	cout << "drives: " << processo.codDisco << '\n';
}

void imprimirProcessos(std::list<Processo> processos) {
	for (std::list<Processo>::iterator it = processos.begin(); it != processos.end(); it++) {
		Processo processo = *it;
		cout << '\n';

		imprimirProcesso(processo);

		cout << '\n';
	}
}

