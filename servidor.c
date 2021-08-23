/*
 *  Servidor para aplicacao da Escrita Colaborativa.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#define SERVER_PORT 12345		// Porta do servidor
#define BUF_SIZE	  1024		// Tamanho maximo dos buffers
#define MSG_SUCCESS	"Texto salvo com sucesso!"
#define MSG_FAILED	"Nao foi possivel salvar o texto!"

// Funcao para visualizar erros
int fatal(char *str1, char *str2) {
    fprintf(stderr, str1, str2);
    exit(-1);
}

// Funcao que adiciona o texto no livro
int write_text_book(char *text) {
  FILE * fp;
  int size;
  fp = fopen("livro.txt","a");
  if (fp == NULL)
    return(-1);
  size = fprintf(fp,"%s",text);
  fclose(fp);
  return(size);
}

// Funcao principal da aplicacao servidor
int main(int argc, char *argv[])
{
  int server_socket, socket_connection, on;
  int writing_success = 0;
  char buffer_input[BUF_SIZE];
  char txtToHash[64];
  char txtHashed[BUF_SIZE];
  struct sockaddr_in	ip_server;
  struct sockaddr_in ip_client;
  socklen_t slen = sizeof(ip_client);
  unsigned char *hash;

  memset(&ip_server, 0, sizeof(ip_server));
  ip_server.sin_family = AF_INET;
  ip_server.sin_addr.s_addr = htonl(INADDR_ANY);
  ip_server.sin_port = htons(SERVER_PORT);

  server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if(server_socket < 0)
    fatal("%s: socket() falhou\n", argv[0]);

  setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on));

  if(bind(server_socket, (struct sockaddr *)&ip_server, sizeof(ip_server)) < 0)
    fatal("%s: bind() falhou\n", argv[0]);

  if(listen(server_socket, 3) < 0)
    fatal("%s: listen() falhou\n", argv[0]);


  memset(&buffer_input, 0, sizeof(BUF_SIZE));
  printf("********************************************************\n");
  printf(" Servidor da Escrita Colaborativa iniciado com sucesso!\n");
  printf("********************************************************\n\n");
  printf("Por padrao, o servidor ficara rodando indefinidamente. Para encerra-lo entre com CTRL+C.\n\n");
  while(1) {
    printf("Aguardando clientes dos autores...\n");
    socket_connection = accept(server_socket, (struct sockaddr *)&ip_client, &slen);
    if(socket_connection < 0)
      fatal("%s: accept() falhou\n", argv[0]);

    printf("Cliente conectado: IP %s, Porta %d\n", inet_ntoa(ip_client.sin_addr),ntohs(ip_client.sin_port));
    memset(&buffer_input, 0, sizeof(BUF_SIZE));

    while(strcmp(buffer_input,"sair\n") != 0) {
      read(socket_connection, buffer_input, BUF_SIZE);
      printf("\n\n\nTexto recebido: %s\n\n\n", buffer_input);

      if (strcmp(buffer_input,"sair\n") != 0) {
        writing_success = write_text_book(buffer_input);
        if (writing_success) {
          // O +1 adicionado ao tamanho considera o byte \0 para indicar o fim da string
          write(socket_connection, MSG_SUCCESS, strlen(MSG_SUCCESS)+1);

        }
        else {
          // O +1 adicionado ao tamanho considera o byte \0 para indicar o fim da string
          write(socket_connection, MSG_FAILED, strlen(MSG_FAILED)+1);
        }
      }
      
    }
    printf("Cliente desconectado: IP %s, Porta %d\n", inet_ntoa(ip_client.sin_addr),ntohs(ip_client.sin_port));
    close(socket_connection);
  }
}
