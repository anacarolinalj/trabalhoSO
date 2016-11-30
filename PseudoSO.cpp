#include "PseudoSO.h"
#include "Processo.h"
#include "Memoria.h"
#include "Recurso.h"
#include "Filas.h"
#include <iostream>
#include <fstream>
#include <istream>
#include <string>
#include <cstring>
#include <stdlib.h>
#include <list>
#include <unistd.h>

using namespace std;

#define QUANTUM 1
#define TEMPO_MAX 1000


std::list<Processo> timer[TEMPO_MAX];


// Tempo do último processo
int fimExecucao;

// Lista com todos os processos lidos do arquivo.
std::list<Processo> processos;

// Filas de processos para escalonamento.
Filas filas;

// Gerente da memória.
Memoria gerenteMemoria;

// Temporizador
int temporizador;

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

void adicionarFilas(Processo processo) {
	// Aloca memória
	processo.offset = (processo.prioridade == 0) ?
					   gerenteMemoria.alocaBloco(processo.pid,Memoria::TempoReal,processo.blocoMem) :
					   gerenteMemoria.alocaBloco(processo.pid,Memoria::Usuario,processo.blocoMem);

	// Caso tenha conseguido alocar, coloca nas filas.
	if (processo.offset != -1) {
		processos.push_back(processo);
		filas.adicionarProcesso(processo);
	}
}


void executarCPU(Processo processo, int tempo) {
	cout << "\ndispatcher => \n";

	cout << "\t PID: " << processo.pid << '\n';
	cout << "\t offset: " << processo.offset << '\n';
	cout << "\t blocks: " << processo.blocoMem << '\n';
	cout << "\t priority: " << processo.prioridade << '\n';
	cout << "\t time: " << processo.tempoProcessador << '\n';
	cout << "\t printers: " << processo.reqImpressora << '\n';
	cout << "\t scanners: " << processo.reqScanner << '\n';
	cout << "\t modems: " << processo.reqModem << '\n';
	cout << "\t drives: " << processo.codDisco << '\n';
	cout << "\n";

	cout << "Process " << processo.pid << "\n";
	cout << "P" << processo.pid << " STARTED \n";

	for (int it = 1; it < tempo+1 ; it++) {
		cout << "P" << processo.pid << " instruction " << it << "\n";
		filas.age();
		//sleep(1);
	}

	cout << "SIGINT" << "\n";
}

void escalonadorUsuario() {
	Processo *processo = NULL;
	std::list<Processo> *fila = NULL;

	if (!filas.processosUsuario.fila1.empty()) fila = &filas.processosUsuario.fila1;
	else if (!filas.processosUsuario.fila2.empty()) fila = &filas.processosUsuario.fila2;
	else if (!filas.processosUsuario.fila3.empty()) fila = &filas.processosUsuario.fila3;

	//cout << "TEMPORIZADOR: " << temporizador << "\n";


	if (fila) {
		processo = &fila->front();

		// Executa instrução.
		executarCPU(*processo, QUANTUM);

		// Incrementa temporizador
		temporizador += QUANTUM;

		// Decrementa tempo do processo
		(*processo).tempoProcessador -= QUANTUM;

		// Atualiza idade
		(*processo).idade = -1;

		// Verifica se processo acabou para desalocar memória e tirar da fila.
		if (!(*processo).tempoProcessador) {
			gerenteMemoria.liberaBloco((*processo).pid);
			(*fila).pop_front();
		} else {
			Processo temp = (*processo);

			(*fila).pop_front();
			(*fila).push_back(temp);
		}

	} else {
		temporizador += QUANTUM;
		//sleep(1);
		filas.age();
	}

	// Verifica se algum processo pode aumentar de prioridade.
	filas.promoverProcesso();
}

void escalonador() {
	if (filas.processosReais.empty()) escalonadorUsuario();

	else {
		while (!filas.processosReais.empty()) {
			Processo processo = filas.processosReais.front();

			// "Executa" instruções e mostra informações do processo tela.
			executarCPU(processo, processo.tempoProcessador);

			// Incrementa o temporizador.
			temporizador += processo.tempoProcessador;

			// Libera memória do bloco.
			gerenteMemoria.liberaBloco(processo.pid);

			// Retira processo da fila de processos reais.
			filas.processosReais.pop_front();

		}
	}
}

void despachante(int fimExecucao) {

	// Adiciona todos os processos à fila global de processos.
	for (int it = 0; it < fimExecucao; it++) {
		for (std::list<Processo>::iterator processo = timer[it].begin(); processo != timer[it].end(); processo++) {
			adicionarFilas(*processo);
		}
		if (temporizador <= it) escalonador();

	}

	// Chama o escalonador até acabar de executar todos os processos.
	while (	(!filas.processosReais.empty()) ||
				(!filas.processosUsuario.fila3.empty()) ||
				(!filas.processosUsuario.fila2.empty()) ||
				(!filas.processosUsuario.fila1.empty()))
		escalonador();
}

int main(int argc, const char *argv[]) {
	string nomeArq;
	ifstream arquivo;
	std::list<Processo> processosArquivo;


	// Verifica se nome do arquivo foi informado antes da execução.
	/*if (argc < 2) {
		cout << "\nInforme o nome do arquivo com os processos: ";
		cin >> nomeArq;
	} else nomeArq = argv[1];*/

	nomeArq = "processos.txt";

	arquivo.open(nomeArq.c_str(), ios::in);

	// Verifica se arquivo foi aberto.
	if (arquivo.is_open()) {
		processosArquivo = lerArquivo(arquivo);
		fimExecucao = processosArquivo.back().tempoInicializacao + 1;

		despachante(fimExecucao);
	}
}

std::list<Processo> lerArquivo(ifstream & arquivo) {
	std::list<Processo> processos;
	int pid = 1;

	string linha;


	// Lê arquivo linha a linha.
	while(!arquivo.eof()){

		std::getline(arquivo,linha);

		if (!linha.empty()) {
			Processo processo;
			char *dados = std::strtok(&linha[0], ", ");

			int it = 0;


			// Coloca as informações no processo.
			processo.pid = pid++;
			processo.offset = 0;
			processo.idade = 0;
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

			timer[processo.tempoInicializacao].push_back(processo);

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

