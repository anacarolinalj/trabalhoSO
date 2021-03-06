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
#include <cstdio>

using namespace std;

/*
 * Variáveis globais
 */

// Lista de requisições
std::list<Processo> cliente[TEMPO_MAX];

// Filas de processos para escalonamento.
Filas filas;

// Gerente da memória.
Memoria gerenteMemoria;

// Gerente de recursos.
Recurso gerenteRecursos;

// Temporizador.
int temporizador;

// Pid do processo que está sendo executado pela CPU.
int cpu;

/*
 * Protótipos.
 */
std::list<Processo> lerArquivo(ifstream & arquivo);
void executarCPU(Processo processo, int tempo);
void escalonadorUsuario();
void escalonador();
void executarCPU(Processo processo, int tempo);
void pseudoSO(int fimExecucao);


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
	cpu = -1;

	// Adiciona todos os processos à fila global de processos.
	for (int it = 0; it < fimExecucao; it++) {
		for (std::list<Processo>::iterator processo = cliente[it].begin(); processo != cliente[it].end(); processo++) {
			filas.adicionarFilas(*processo);
		}
		if (temporizador <= it) escalonador();

	}

	// Chama o escalonador até acabar de executar todos os processos.
	while (	(!filas.processosReais.empty()) ||
				(!filas.processosUsuario.fila3.empty()) ||
				(!filas.processosUsuario.fila2.empty()) ||
				(!filas.processosUsuario.fila1.empty()))
		escalonador();

	cout << "P" << cpu << " return SIGINT\n";
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

			// Retira processo da fila de processos reais e globais.
			filas.removerProcesso(processo.prioridade,processo.pid);

		}
	}
}

/*
 * Escalona os processos de usuário.
 */
void escalonadorUsuario() {
	Processo *processo = NULL;
	std::list<Processo> *fila = NULL;

	// Identifica a fila de maior prioridade não vazia.
	if (!filas.processosUsuario.fila1.empty()) fila = &filas.processosUsuario.fila1;
	else if (!filas.processosUsuario.fila2.empty()) fila = &filas.processosUsuario.fila2;
	else if (!filas.processosUsuario.fila3.empty()) fila = &filas.processosUsuario.fila3;

	if (fila) {
		processo = &fila->front();

		// Verifica se memória já foi alocada.
		if (processo->offset == -1) {
			processo->offset =
				gerenteMemoria.alocaBloco(processo->pid,Memoria::Usuario,processo->blocoMem);
		}

		// Aloca recursos
		bool recursosAlocados = true;

		// Verifica se o processo precisa de recursos.
		if (processo->precisaRecursos())
        {
			bool reqScanner = true, reqModem = true, reqImpressora = true, reqDisco = true;

            if(processo->reqScanner) {
            	reqScanner = gerenteRecursos.alocaRecurso(processo->pid, Recurso::Scanner);
            	if (!reqScanner) filas.promoverProcessoComRecurso(processo->prioridade, gerenteRecursos, Recurso::Scanner);
            }

            if(processo->reqModem) {
                reqModem = gerenteRecursos.alocaRecurso(processo->pid, Recurso::Modem);
                if (!reqModem) filas.promoverProcessoComRecurso(processo->prioridade, gerenteRecursos, Recurso::Modem);
            }

            if(processo->reqImpressora) {
                reqImpressora = gerenteRecursos.alocaRecurso(processo->pid, Recurso::Impressora);
                if (!reqImpressora)filas.promoverProcessoComRecurso(processo->prioridade, gerenteRecursos, Recurso::Impressora);
            }
            if(processo->reqDisco) {
                reqDisco = gerenteRecursos.alocaRecurso(processo->pid, Recurso::Sata);
                if (!reqDisco) filas.promoverProcessoComRecurso(processo->prioridade, gerenteRecursos, Recurso::Impressora);
            }

            if ((!reqDisco) || (!reqModem) || (!reqImpressora) || (!reqDisco)) recursosAlocados = false;
            if (!recursosAlocados) {
            	gerenteRecursos.liberaRecurso(processo->pid,Recurso::Impressora);
            	gerenteRecursos.liberaRecurso(processo->pid,Recurso::Sata);
            	gerenteRecursos.liberaRecurso(processo->pid,Recurso::Modem);
            	gerenteRecursos.liberaRecurso(processo->pid,Recurso::Scanner);
            }
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
				filas.removerProcesso(processo->prioridade,processo->pid);
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
		static int instruction = 1;

		// Verifica se processo que está sendo executado deve continuar.
		if (cpu != processo.pid) {

			// Caso seja outro processo, interrompe o anterior e começa o novo.
			if (cpu != -1) cout << "P" << cpu << " return SIGINT\n";

			//cout << "\nSYSTEM TIME: " << temporizador;

			cout << "\n dispatcher => \n";

			cout << "\t PID: " << processo.pid << '\n';
			cout << "\t offset: " << processo.offset << '\n';
			cout << "\t blocks: " << processo.blocoMem << '\n';
			cout << "\t priority: " << processo.prioridade << '\n';
			cout << "\t time: " << processo.tempoProcessador << '\n';
			cout << "\t printers: " << processo.reqImpressora << '\n';
			cout << "\t scanners: " << processo.reqScanner << '\n';
			cout << "\t modems: " << processo.reqModem << '\n';
			cout << "\t drives: " << processo.reqDisco << '\n';
			cout << "\n";

			cout << "Process " << processo.pid << "\n";
			cout << "P" << processo.pid << " STARTED \n";

			instruction = 1;
		}

		for (int it = 1; it < tempo+1 ; it++) {
			cout << "P" << processo.pid << " instruction " << instruction++ << "\n";
			filas.age();
			//sleep(1);
		}

		cpu = processo.pid;
}

std::list<Processo> lerArquivo(ifstream & arquivo) {
	std::list<Processo> processos;
	int pid = 1;

	string linha;


	// Lê arquivo linha a linha.
	while(!arquivo.eof()){

		std::getline(arquivo,linha);

		if (!linha.empty()) {
			int tempoInicializacao, prioridade, tempoProcessador, blocoMem;
			int reqImpressora, reqScanner, reqModem, reqDisco;
			int idade = 0, offset = -1;

			sscanf(linha.c_str(),"%d, %d, %d, %d, %d, %d, %d, %d\n",
					&tempoInicializacao, &prioridade, &tempoProcessador,
					&blocoMem, &reqImpressora, &reqScanner, &reqModem, &reqDisco);

			Processo processo(tempoInicializacao, prioridade, tempoProcessador, blocoMem,
			reqImpressora, reqScanner, reqModem, reqDisco, idade, offset, pid++);

			if ((!processo.prioridade) &&
					((processo.reqDisco != 0) || (processo.reqImpressora != 0) ||
					(processo.reqModem != 0) || (processo.reqScanner != 0))) {
				cout << "\nRequisicao de recurso para processo real. Isso nao e permitido. Requisicao"
						" sera ignorada.\n";

				processo.reqImpressora = 0;
				processo.reqModem = 0;
				processo.reqScanner = 0;
				processo.reqDisco = 0;
			}

			if ((processo.prioridade < 0) || (processo.prioridade > 3)) {
				cout << "\nProcesso com prioridade () " << processo.prioridade
						<< " nao aceita. As prioridade deve estar entre 0 e 3. "
							"Processo sera ignorado.\n";

			} else if ( (processo.blocoMem > MEMORY_USER) || ( (!processo.prioridade) && (processo.blocoMem > MEMORY_REAL))) {
				cout <<"\nProcesso tenta alocar mais memória do que o permitido para a sua prioridade."
						"Os processos de usuario devem estar na faixa 0-960 e os de tempo real na faixa 0-64."
						"Processo sera ignorado.\n";
			} else if ((processo.reqDisco < 0) || (processo.reqDisco > 1) ||
						(processo.reqImpressora < 0) || (processo.reqImpressora > 1) ||
						(processo.reqModem < 0) || (processo.reqModem > 1) ||
						(processo.reqScanner < 0) || (processo.reqScanner > 1)) {
				cout << "\nNumero invalido para requisicao de recurso. Requisicao deve ser 0 ou 1.\n\n";

			} else {
				cliente[processo.tempoInicializacao].push_back(processo);
				processos.push_back(processo);
			}
		}
	}
	arquivo.close();

	return processos;
}
