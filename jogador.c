#include <stdio.h>
#include <mqueue.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <semaphore.h>
#include <unistd.h>

typedef struct Message {
  int id;
  int column;
  int row;
} StructMessage;

const char* NOME_FILA = "/fila_jogadas";

void askColumnAndRow();
void doThePlay();

int column = 0;
int row = 0;
StructMessage message;
mqd_t queue;

int  main() {

  queue = mq_open(NOME_FILA, O_WRONLY, NULL);
  if(queue == (mqd_t) -1) {
    perror("mq_open");
    exit(2);
  }

  askColumnAndRow();

  doThePlay();

  mq_close(queue);

  return 0;
}

void doThePlay() {
  message.column = --column;
  message.row = --row;
  message.id = getpid();

  if(mq_send(queue, (const char *) &message, sizeof(StructMessage), 29) != 0) {
    perror("Erro ao enviar jogada");
  }

  printf("Jogada enviada para o tabuleiro com sucesso!\n\n");
  askColumnAndRow();
}

void askColumnAndRow() {
  printf("Digite a coluna: ");
  scanf("%d", &column);
  printf("Digite a linha: ");
  scanf("%d", &row);

  if(column > 3 || column < 1 || row > 3 || row < 1) askColumnAndRow();
  else doThePlay();
}