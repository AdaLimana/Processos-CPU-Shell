#include "lista.h"
#include <pthread.h>


int separaComando(char *comando, char *tipoComando, char *nome, int *tempoExec, int *pidKill);
int separaCreate(char *comando, char *nome, int *tempoExec, int indice);
int separaKill(char *comando, int *pidKill, int indice);
int processoLista(lista *l, int *pids, char *nome, int tempoExec);/*coloca um processo na lista*/
int killProcesso(lista *l, int pidKill);
void psProcesso(lista *l);
void mostraTime(time_t *t);
void shutdownProcessos(lista *l, pthread_t *t);
void *nucleo(void *l);
void legenda();

int shutdown = 0;

int main() {

	lista l;/*lista que armazena os processos */
	inicializa(&l);

	/*Var. p/ controle o comando dado pelo usuário*/
	char comando[50];
	char tipoComando[50];
	char nome[50];
	int tempoExec = 0;
	int pidKill = 0;
	time_t tempo;

	int opcao = 0;
	int i = 0;
	int pids = 0;/*controle dos PIDs*/

	/*varialveis para controle thread*/
	pthread_t threadNucleo;/*identifica a thread*/

	/*Chama a thread que será rresponsável por "executar" os processos
	  criando assim, um fluxo de execução paralelo ao (main), a função(nucleo)
	*/
	pthread_create(&threadNucleo, NULL, nucleo, &l);

	legenda();/*Mostra exemplos de comandos*/
	while(1){
		gets(comando);
		opcao = separaComando(comando, tipoComando, nome, &tempoExec, &pidKill);

		if(opcao){
			if(opcao == 1){/*se o comando for create a funcao retorna (1) */
                		if(processoLista(&l, &pids, nome, tempoExec)){
                        		printf("\nProcesso Criado\n");
                		}
               			else{
                        		printf("\nProcesso nao Criado\n");
                		}
			}
		        else if(opcao == 2){/*se o comando for (kill) a função retorna (2)*/
				if( killProcesso(&l, pidKill)){
					printf("\nkill processo PID=%d\n", pidKill);
				}
				else{
					printf("\nprocesso PID=%d nao existe\n", pidKill);
				}
        		}
        		else if(opcao == 3){/*se o comando for (ps) a função retorna (3)*/
				psProcesso(&l);
        		}
        		else if(opcao == 4){/*se o comando for (time) a função retorna (4)*/
             			time(&tempo);
				mostraTime(&tempo);
        		}
        		else if(opcao == 5){/*se o comando for (shutdown) a função retorna (5)*/
				shutdownProcessos(&l, &threadNucleo);
				break;
        		}
		}
		else{
			printf("\nComando invalido\n");
		}


	}
	return (0);
}


/*
  Legenda dos comando a serem digitados
*/
void legenda(){
	printf("\n##################################################################\n");
	  printf("## Comando CREATE NOME TEMPO_EXECUCAO,          ex.(create a 10)##\n");
	  printf("## Comando KILL PID                             ex.(kill 12)    ##\n");
	  printf("## Comando TIME                                 ex.(time )      ##\n");
	  printf("## Comando PS                                   ex.(ps )        ##\n");
	  printf("## Comando SHUTDOWN                             ex.(shutdown)   ##\n");
	  printf("##################################################################\n");
}

/*
  "Quebra" o comando informado em
  partes para o mesmo ser interpretado,
  o comando pode ser: CREATE; KILL;
  TIME; PS; SHUTDOWN    
*/
int separaComando(char *comando, char *tipoComando, char *nome, int *tempoExec, int *pidKill){

	int indice=0;
	int j=0;

	/*Separa até a primeira parte da string que deve ser o comando*/
	while(comando[indice]!=' ' && comando[indice]!='\0'){
		tipoComando[j] = comando[indice];
		indice++;
		j++;
	}

	tipoComando[j] = '\0';/*Coloca o caracter terminador na string tipoComando*/

	/*Comando CREATE ?*/
	if(!strcmp(tipoComando,"create")){
		if(separaCreate(comando, nome, tempoExec, indice)){
			return 1;
		}
		return 0;
	}

	/*Comando KILL ?*/
	if(!strcmp(tipoComando, "kill")){
		if(separaKill(comando, pidKill, indice)){
			return 2;
		}
		return 0;
	}

	/*Comando PS ?*/
	if(!strcmp(tipoComando, "ps")){
		return 3;
	}

	/*Comando TIME ?*/
	if(!strcmp(tipoComando, "time")){
		return 4;
	}

	/*Comando SHUTDOWN ?*/
	if(!strcmp(tipoComando, "shutdown")){
		return 5;
	}

	return 0;

}

/*
  Verifica o comando CREATE
*/
int separaCreate(char *comando, char *nome, int *tempoExec, int indice){
	char tempo[10];
	int j=0;
	if(comando[indice] == '\0'){/*verifica se o comando  'create'*/
		return 0;
	}
	indice++;

	while(comando[indice] != ' ' && comando[indice] != '\0'){
		nome[j] = comando[indice];
		j++;
		indice++;
	}
	nome[j] = '\0';/*Coloca o finalizador na string*/
	j=0;

	if(comando[indice] == '\0'){/*verifica se o comano é so 'create' e 'nome' */
		return 0;
	}
	indice++;

	while(comando[indice] != '\0'){
		tempo[j] = comando[indice];
		j++;
		indice++;
	}
	tempo[j] = '\0'; /*Coloca o finalizador na string*/

	*tempoExec = atoi(tempo); /*converte os numeros da string em int, caso não possua números retorna zero*/

	if(!(*tempoExec)){/*não pode ter tempo de exec igual a zero*/
		return 0;
	}

	return 1;
}

/*
  Verifica o comando KILL
*/
int separaKill(char *comando, int *pidKill, int indice){

	char pid[10];/*recebe o pid do comando*/
	int j=0;
	if(comando[indice] == '\0'){/*verifica se o comando kill está certo*/
		return 0;
	}
	indice++;

	while(comando[indice] != '\0'){/*pega da string comando o pid informado*/
		pid[j] = comando[indice];
		j++;
		indice++;
	}
	pid[j] = '\0'; /*Coloca o marcado final na string*/

	if(pid[0] == '0'){/*testa aqui, pois quando converte*/
		*pidKill = 0;/*string para inteiro com atoi()*/
		return 1;    /*qualquer caracter que não for*/
	}		     /*número recebe zero*/

	*pidKill = atoi(pid);/*conver a string para inteiro*/

	if(!(*pidKill)){/*se for zero quer dizer que o caracter não era número*/
		return 0;
	}
	return 1;
}

/*
  Coloca um processo na lista*/
*/
int processoLista(lista *l, int *pids,char *nome,int tempoExec){
	processo *p;
	p = criaProcesso((*pids), nome, tempoExec);/*cria o processo e retorna a referência do mesmo*/

	if(p){/*verifica se o processo foi criado*/
		inserefim(l,p);/*Coloca o novo processo no fim da lista*/
		(*pids)++;
		return 1;
	}
	return 0;
}

/*
  Mata um processo
*/
int killProcesso(lista *l, int pidKill){
	no *noPid;/*recebe o NÓ da POSICÃO em questão*/
	processo *proPid;/*recebe o PROCESSO do NÓ em questão*/
	int i = 1;/*variavel para comecar a busca pelo primeiro NÓ*/

	while(i<=l->tamanho){/*busca em todos os NÓs*/
		noPid = retornaNoPosicao(l, i);
		proPid =  noPid->memoria;
		if(proPid->pid == pidKill){
			if(proPid->estado){/*Se o processo está em execução precisa ser parado*/
				proPid->kill = 1;/*ou seja, tirado da thread*/
				return 1;
			}
			excluiEm(l, i);
			return 1;
		}
		i++;
	}
	return 0;
}

/*
  Mostra todos os processos
  do sistema
*/
void psProcesso(lista *l){

	struct tm *x;/*recebe a estrutura da hora*/

	int i = 0;
	no *noAtual;
	processo  *pAtual;
	if(l->tamanho){/*se tem processo criado*/
		noAtual = (no*)l->primeiro;
		pAtual = noAtual->memoria;
		printf("\nPID\tNOME PROCESSO\tTEMPO EXEC\tTEMPO CRIACAO\tESTADO\n");
	}

	while(i < l->tamanho){
		printf("%d\t",pAtual->pid);
		printf("%s",pAtual->nome);
		printf("\t\t");
		printf("%d\t\t",pAtual->tempoExec);

		x=mostraHora(pAtual);
		printf("%d:%d:%d\t\t",x->tm_hour, x->tm_min, x->tm_sec);

		if(pAtual->estado){
			printf("%s\n","executando");
		}
		else{
			printf("%s\n","apto");
		}
		i++;
		if(i < l->tamanho){/*Na ultima iteração não tem proximo, então não existe memória para ele*/
			noAtual = (no*)noAtual->prox;
			pAtual= noAtual->memoria;
		}
	}
}

/*
  Mostra o tempo do sistema
*/
void mostraTime(time_t *t){
	struct tm *time;
	time = gmtime(t);
	printf("\n%d:%d:%d\n", time->tm_hour, time->tm_min, time->tm_sec);

}

/*
  Finaliza (mata) todos os processos
*/
void shutdownProcessos(lista *l, pthread_t *t){
	int i=0;
	shutdown = 1; /*informa a thread nucleo para finalizar-se*/
	pthread_join(*t, NULL);/*espera a thread nucleo finalizar*/
	no *noAtual;
	processo *procAtual;
	if(l->tamanho){
		noAtual =(no*) l->primeiro;
		procAtual = noAtual->memoria;

		while(i < l->tamanho){
			printf("\nFinalizando Processo\t%d\n",procAtual->pid);
			if(noAtual->prox){/*Se for o último, não tem próximo*/
				noAtual = (no*) noAtual->prox;
				procAtual = noAtual->memoria;
			}
			i++;
		}
		liberalista(l);/*Exclui todos os processos*/
	}
}

/*
  Thread responsavel pela 
  "execucao" dos processos
*/
void *nucleo(void *l){
	lista *list;
	list = (lista *)l;
	no *noAtual;
	processo *proc;
	time_t inicioExec;
	time_t tempoAtual;
	while(1){
		if(list->tamanho){/*Se tem processo na lista*/
			noAtual = (no*) list->primeiro;
			proc = noAtual->memoria;
			proc->estado = 1;/*coloca no estado executando*/
			time(&inicioExec);
			time(&tempoAtual);
			while((tempoAtual-inicioExec < proc->tempoExec)){
				if(proc->kill){
					break;
				}
				if(shutdown){
					pthread_exit(NULL);
				}
				time(&tempoAtual);
			}
			excluiInicio(list);

		}
		else if(shutdown){
			pthread_exit(NULL);
		}
	}

}
