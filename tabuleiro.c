#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <time.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <sys/types.h>

const char *NOME_FILA = "/fila_jogadas";

typedef struct Message
{
    int id;
    int column;
    int row;
} StructMessage;

ssize_t get_msg_buffer_size(mqd_t queue);
void insereJogada(StructMessage *message);
void exibeTabuleiro();
int checaColuna();
int checaDiagonal();
int checaLinha();
int verificaEmpate();
int verificaFimDoJogo();
int addJogador(int playerId);

int turno = 1;
int tabuleiro[3][3];
int fd;
int jogadores[2];
int ultimoAJogar;

void sig_handler(int signo){
    
}

int main()
{
    printf("PID: %d\n", getpid());
    exibeTabuleiro();

    mqd_t queue;
    char *buffer = NULL;
    ssize_t tam_buffer;
    ssize_t nbytes;

    fd = open("log_de_jogadas.txt", O_CREAT | O_RDWR, 0600);
	if (fd == -1) perror("Falha no open()");

    lseek(fd, 0, SEEK_END);

    if (write(fd, "\r\n\r\n", 4) != 4) perror("escrita buf1");

    queue = mq_open(NOME_FILA, O_RDONLY | O_CREAT, 0770, NULL);

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
	
        insereJogada((StructMessage *)buffer);
        sleep(1);
    }

    mq_close(queue);
    exit(0);
}

int addJogador(int playerId){
	if(jogadores[1] == playerId || jogadores[0] == playerId) {
		return 0;
	} else if(jogadores[1] != 0 && jogadores[0] != 0){
		return -1;
	} else {
		if(jogadores[0] == 0) {
			jogadores[0] = playerId;
			return 0;
		}
		if(jogadores[1] == 0) {
			jogadores[1] = playerId;
			return 0;
		}
		return 0;
	}
}

void insereLog(int row, int column, int playerdId, char *status){
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    char log[29 + strlen(status)];

    snprintf(log, sizeof(log), "%d-%d-%d %d:%d:%d;%d;%d %d;%s\r\n", tm.tm_mday, tm.tm_mon, tm.tm_year, tm.tm_hour, tm.tm_min, tm.tm_sec, playerdId, row + 1, column + 1, status);

    lseek(fd, 0, SEEK_END);

    if (write(fd, log, sizeof(log)) != sizeof(log)) perror("escrita buf1");
}

void insereJogada(StructMessage *message)
{
    int row = message->row;
    int column = message->column;
    int playerId = message->id;

	if (addJogador(playerId) == -1){
		printf("Você não pode participar dessa partida!\n");
	} else if(ultimoAJogar == playerId) {
		printf("Jogada Inválida: Não é a sua vez de jogar!\n");
	} else if(tabuleiro[row][column] != 0) {
        insereLog(row, column, playerId, "jogada inválida");
		printf("Jogada Inválida: Essa posição já está ocupada!\n");
	} else {
        insereLog(row, column, playerId, "jogada válida");
        if (turno % 2 == 0)
        {
            tabuleiro[row][column] = 1;
		    ultimoAJogar = playerId;
        }
        else
        {
            tabuleiro[row][column] = -1;
		    ultimoAJogar = playerId;
        }

        turno++;
        exibeTabuleiro();

        if(verificaFimDoJogo() == 1){
            insereLog(row, column, playerId, "jogada vencedora");
            exit(0);
        }
    }
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

int verificaFimDoJogo()
{
    if (checaLinha() == 3 || checaColuna() == 3 || checaDiagonal() == 3)
    {
        printf("Jogador 2 venceu\n");
        return 1;
    }
    else if (checaLinha() == -3 || checaColuna() == -3 || checaDiagonal() == -3)
    {
        printf("Jogador 1 venceu\n");
        return 1;
    }
    else if (verificaEmpate() == 1)
    {
        printf("O jogo empatou\n");
        return 1;
    }

    return 0;
}

int verificaEmpate()
{
    int linha, coluna;

    for (linha = 0; linha < 3; linha++)
        for (coluna = 0; coluna < 3; coluna++)
            if (tabuleiro[linha][coluna] == 0)
                return 0;

    return 1;
}

int checaLinha()
{
    int soma;

    for (int linha = 0; linha < 3; linha++)
    {
        soma = 0;

        for (int coluna = 0; coluna < 3; coluna++)
            soma += tabuleiro[linha][coluna];

        if (soma == 3 || soma == -3)
            return soma;
    }

    return 0;
}

int checaColuna()
{
    int soma;

    for (int coluna = 0; coluna < 3; coluna++)
    {
        soma = 0;

        for (int linha = 0; linha < 3; linha++)
            soma += tabuleiro[linha][coluna];

        if (soma == 3 || soma == -3)
            return soma;
    }

    return 0;
}

int checaDiagonal()
{
    int somaDiagonalP = 0;
    int somaDiagonalS = 0;

    for (int i = 0; i < 3; i++)
    {
        somaDiagonalP += tabuleiro[i][i];
        somaDiagonalS += tabuleiro[i][2 - i];
    }

    if (somaDiagonalP == 3 || somaDiagonalP == -3)
        return somaDiagonalP;
    if (somaDiagonalS == 3 || somaDiagonalS == -3)
        return somaDiagonalS;
    return 0;
}

void exibeTabuleiro()
{
    printf("\n");

    for (int linha = 0; linha < 3; linha++)
    {
        for (int coluna = 0; coluna < 3; coluna++)
        {
            if (tabuleiro[linha][coluna] == 0)
                printf("   ");
            else if (tabuleiro[linha][coluna] == 1)
                printf(" X ");
            else
                printf(" O ");

            if (coluna != (3 - 1))
                printf("|");
        }
        printf("\n");
    }
    printf("\n");
}