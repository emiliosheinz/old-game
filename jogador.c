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

int column = 0;
int row = 0;

int  main() {
  mqd_t queue;
  StructMessage message;

  queue = mq_open(NOME_FILA, O_WRONLY | O_CREAT, 0770, NULL);
  if(queue == (mqd_t) -1) {
    perror("mq_open");
    exit(2);
  }

  askColumnAndRow();

  message.column = --column;
  message.row = --row;
  message.id = getpid();

  if(mq_send(queue, (const char *) &message, sizeof(StructMessage), 29) != 0) {
    perror("Erro ao enviar jogada");
  }

  mq_close(queue);

  return 0;
}

void askColumnAndRow() {
  printf("Digite a coluna: ");
  scanf("%d", &column);
  printf("Digite a linha: ");
  scanf("%d", &row);

  if(column > 3 || column < 1 || row > 3 || row < 1) askColumnAndRow();
}