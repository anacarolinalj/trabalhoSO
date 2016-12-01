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

/*
 * Variáveis globais
 */
std::list<Processo> cliente[TEMPO_MAX];

// Lista com todos os processos lidos do arquivo.
std::list<Processo> processos;

// Filas de processos para escalonamento.
Filas filas;

// Gerente da memória.
Memoria gerenteMemoria;

// Gerente de recursos.
Recurso gerenteRecursos;

// Temporizador.
int temporizador;

/*
 * Protótipos.
 */
std::list<Processo> lerArquivo(ifstream & arquivo);
void executarCPU(Processo processo, int tempo);
void escalonadorUsuario();
void escalonador();
void executarCPU(Processo processo, int tempo);
void pseudoSO(int fimExecucao);

//TODO Excluir depois
void imprimirProcesso (Processo processo);
void imprimirProcessos(std::list<Processo> processos);


int main(int argc, const char *argv[]) {
	string nomeArq;
	ifstream arquivo;
	std::list<Processo> processosArquivo;
	int fimExecucao;


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

		pseudoSO(fimExecucao);

		/*cout << "\n----fila1----\n";
		imprimirProcessos(filas.processosUsuario.fila1);
		cout << "\n--------------\n";*/
	}

}

/*
 * Simulação do Pseudo SO.
 */
void pseudoSO(int fimExecucao) {

	// Adiciona todos os processos à fila global de processos.
	for (int it = 0; it < fimExecucao; it++) {
		for (std::list<Processo>::iterator processo = cliente[it].begin(); processo != cliente[it].end(); processo++) {
			filas.adicionarFilas(*processo,&processos);
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

void escalonador() {
	if (filas.processosReais.empty()) escalonadorUsuario();

	else {
		// Escalona processos de tempo real.
		while (!filas.processosReais.empty()) {
			Processo processo = filas.processosReais.front();

			// Aloca memória
			if (processo.offset == -1 ) {
				processo.offset = gerenteMemoria.alocaBloco(processo.pid,Memoria::TempoReal,processo.blocoMem);
			}

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

void escalonadorUsuario() {
	Processo *processo = NULL;
	std::list<Processo> *fila = NULL;

	if (!filas.processosUsuario.fila1.empty()) fila = &filas.processosUsuario.fila1;
	else if (!filas.processosUsuario.fila2.empty()) fila = &filas.processosUsuario.fila2;
	else if (!filas.processosUsuario.fila3.empty()) fila = &filas.processosUsuario.fila3;

	//cout << "TEMPORIZADOR: " << temporizador << "\n";


	if (fila) {
		processo = &fila->front();

		// Verifica se memória já foi alocada.
		if (processo->offset == -1) {
			processo->offset =
				gerenteMemoria.alocaBloco(processo->pid,Memoria::Usuario,processo->blocoMem);
		}

		// Aloca recursos

		bool recursosAlocados = true;
		// Verifica se o processo precisa de recursos e estes ainda não foram alocados para ele
		if(processo->precisaRecursos() && ! gerenteRecursos.checaRecurso(processo->pid))
        {
            if(processo->reqScanner)
                recursosAlocados = gerenteRecursos.alocaRecurso(processo->pid, Recurso::Scanner);

            if(processo->reqModem)
                recursosAlocados = gerenteRecursos.alocaRecurso(processo->pid, Recurso::Modem);

            if(processo->reqImpressora)
                recursosAlocados = gerenteRecursos.alocaRecurso(processo->pid, Recurso::Impressora);

            if(processo->codDisco)
                recursosAlocados = gerenteRecursos.alocaRecurso(processo->pid, Recurso::Sata);
        }

		// Verifica novamente se memória pôde ser alocada e se os recursos necessários já foram alocados.
		if (processo->offset != -1 && recursosAlocados) {
			// Executa instrução.
			executarCPU(*processo,QUANTUM);

			// Incrementa temporizador
			temporizador += QUANTUM;

			// Decrementa tempo do processo
			processo->tempoProcessador -= QUANTUM;

			// Atualiza idade
			processo->idade = -1;

			// Verifica se processo acabou para desalocar memória e tirar da fila.
			if (!processo->tempoProcessador) {
				gerenteMemoria.liberaBloco(processo->pid);
				gerenteRecursos.liberaTodos(processo->pid);
				fila->pop_front();
			} else {
				// Copia primeiro processo para o fim da fila.
				fila->push_back(*processo);
				fila->pop_front();
			}
		}
		// Caso não tenha conseguido alocar memória ou recurso, processo vai para o fim da fila.
		else {
			// Copia primeiro processo para o fim da fila.
			if (fila->size() != 1) {
				fila->push_back(*processo);
				fila->pop_front();
			}

			temporizador += QUANTUM;
						//sleep(1);
			filas.age();
		}

	} else {
		temporizador += QUANTUM;
		//sleep(1);
		filas.age();
	}

	// Verifica se os processos das filas podem aumentar de prioridade.
	filas.promoverProcessos();
}

void executarCPU(Processo processo, int tempo) {
		cout << "\n dispatcher => \n";

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
			processo.offset = -1;
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


			if ((!processo.prioridade) &&
					((processo.codDisco != 0) || (processo.reqImpressora != 0) ||
					(processo.reqModem != 0) || (processo.reqScanner != 0))) {
				cout << "\nRequisicao de recurso para processo real. Isso nao e permitido. Requisicao"
						" sera ignorada.\n";

				processo.reqImpressora = 0;
				processo.reqModem = 0;
				processo.reqScanner = 0;
				processo.codDisco = 0;
			}

			if ((processo.prioridade < 0) || (processo.prioridade > 3)) {
				cout << "\nProcesso com prioridade () " << processo.prioridade
						<< " nao aceita. As prioridade deve estar entre 0 e 3. "
							"Processo sera ignorado.\n";

			} else if ( (processo.blocoMem > MEMORY_USER) || ( (!processo.prioridade) && (processo.blocoMem > MEMORY_REAL))) {
				cout <<"\nProcesso tenta alocar mais memória do que o permitido para a sua prioridade."
						"Os processos de usuario devem estar na faixa 0-960 e os de tempo real na faixa 0-64."
						"Processo sera ignorado.\n";
			} else if ((processo.codDisco < 0) || (processo.codDisco > 2) ||
						(processo.reqImpressora < 0) || (processo.reqImpressora > 2) ||
						(processo.reqModem < 0) || (processo.reqModem > 1) ||
						(processo.reqScanner < 0) || (processo.reqScanner > 1)) {
				cout << "\nRequisicao de recurso nao existente. Os recursos estao na faixa:"
						"\n SATA: 0 - 2"
						"\n Impressora: 0 - 1"
						"\n Modem: 0 - 1"
						"\n Scanner: 0 - 1"
						"\n Processo sera ignorado.\n\n";

			} else {
				cliente[processo.tempoInicializacao].push_back(processo);
				processos.push_back(processo);
			}
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

