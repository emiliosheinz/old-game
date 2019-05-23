#include <stdio.h>
#include <mqueue.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

const char* NOME_FILA = "/fila_jogadas";

typedef struct Message {
  int id;
  int column;
  int row;
} StructMessage;

int  main() {
  int column = 0;
  int row = 0;

  mqd_t queue;
  StructMessage message;

  queue = mq_open(NOME_FILA, O_WRONLY | O_CREAT, 0770, NULL);
  if(queue == (mqd_t) -1) {
    perror("mq_open");
    exit(2);
  }

  printf("Digite a coluna: ");
  scanf("%d", &column);
  printf("Digite a linha: ");
  scanf("%d", &row);

  message.column = column;
  message.row = row;
  message.id = 1;

  if(mq_send(queue, (const char *) &message, sizeof(StructMessage), 29) != 0) {
    perror("Erro ao enviar jogada");
  }

  mq_close(queue);

  return 0;
}