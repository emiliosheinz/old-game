#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>

const char *NOME_FILA = "/fila_jogadas";

typedef struct Message
{
    int id;
    int column;
    int row;
} StructMessage;

ssize_t get_msg_buffer_size(mqd_t queue);
void print_msg(StructMessage *message);

int main()
{
    mqd_t queue;
    char *buffer = NULL;
    ssize_t tam_buffer;
    ssize_t nbytes;

    queue = mq_open(NOME_FILA, O_RDONLY);

    if (queue == (mqd_t)-1)
    {
        perror("mq_open");
        exit(2);
    }

    tam_buffer = get_msg_buffer_size(queue);
    buffer = calloc(tam_buffer, 1);

    while (1)
    {
        nbytes = mq_receive(queue, buffer, tam_buffer, NULL);
        if (nbytes == -1)
        {
            perror("receive");
            exit(4);
        }

        print_msg((StructMessage *)buffer);
        sleep(5);
    }

    mq_close(queue);
    exit(0);
}

void print_msg(StructMessage *message)
{
    printf("id=%d, coluna= %d, linha= %d\n", message->id, message->column, message->row);
}

ssize_t get_msg_buffer_size(mqd_t queue)
{
    struct mq_attr attr;

    if (mq_getattr(queue, &attr) != -1)
    {
        return attr.mq_msgsize;
    }

    perror("aloca_msg_buffer");
    exit(3);
}