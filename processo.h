#include <stdlib.h>
#include <time.h>
#include <string.h>

typedef struct{
    int pid;
    char *nome;
    time_t criacao;
    int tempoExec;
    int estado;
    int kill;/*setada para um quando o processo a ser matado é o que está "executando" na thread*/
}processo;

processo* criaProcesso(int pidProcesso, char *nomeProcesso, int tempoProcesso );
//char * mostraHora(processo *processo);
struct tm *mostraHora(processo *processo);
