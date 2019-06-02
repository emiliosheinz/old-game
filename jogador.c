#include <stdio.h>
#include <mqueue.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>

typedef struct Message
{
  int id;
  int column;
  int row;
  char playerName[30];
} StructMessage;

const char *NOME_FILA = "/fila_jogadas";

void askColumnAndRow();
void doThePlay();

int column = 0;
int row = 0;
StructMessage message;
mqd_t queue;

void tratador(int signum){
    printf("\nSinal %d capturado!\n", signum);
}

int main()
{
	struct sigaction sa;
	memset(&sa, 0, sizeof(sa));

	sa.sa_handler = &tratador;

	if(sigaction(SIGUSR1, &sa, NULL) != 0){
		perror("Falha ao instalar tratador de sinal SIGUSR1");
		exit(-1);
	}
	if(sigaction(SIGUSR2, &sa, NULL) != 0){
		perror("Falha ao instalar tratador de sinal SIGUSR1");
		exit(-1);
	}
	if(sigaction(SIGCONT, &sa, NULL) != 0){
		perror("Falha ao instalar tratador de sinal SIGCONT");
		exit(-1);
	}
	if(sigaction(SIGTERM, &sa, NULL) != 0){
		perror("Falha ao instalar tratador de sinal SIGTERM");
		exit(-1);
	}
	if(sigaction(SIGINT, &sa, NULL) != 0){
		perror("Falha ao instalar tratador de sinal SIGINT");
		exit(-1);
	}
	if(sigaction(SIGTSTP, &sa, NULL) != 0){
		perror("Falha ao instalar tratador de sinal SIGTSTP");
		exit(-1);
	}
  queue = mq_open(NOME_FILA, O_WRONLY, NULL);
  if (queue == (mqd_t)-1)
  {
    perror("mq_open");
    exit(2);
  }

  printf("Escolha seu nome: ");
  scanf("%s", &(message.playerName));

  askColumnAndRow();

  doThePlay();

  mq_close(queue);

  return 0;
}

void doThePlay()
{
  message.column = --column;
  message.row = --row;
  message.id = getpid();

  if (mq_send(queue, (const char *)&message, sizeof(StructMessage), 29) != 0)
  {
    perror("Erro ao enviar jogada");
  }

  printf("Jogada enviada para o tabuleiro com sucesso!\n\n");
  askColumnAndRow();
}

void askColumnAndRow()
{
  printf("Digite a coluna: ");
  scanf("%d", &column);
  printf("Digite a linha: ");
  scanf("%d", &row);

  if (column > 3 || column < 1 || row > 3 || row < 1)
    askColumnAndRow();
  else
    doThePlay();
}