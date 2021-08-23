/*
 *  Cliente para aplicacao da Escrita Colaborativa.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>

#define SERVER_PORT 12345 // Porta do servidor
#define BUF_SIZE	  1024  // Tamanho maximo dos buffers

// Funcao para visualizar erros
int fatal(char *str1, char *str2) {
  fprintf(stderr, str1, str2);
  exit(-1);
}

// Funcao principal da aplicacao cliente
int main(int argc, char *argv[]) {
  int socket_client;
  char buffer_input[BUF_SIZE];
  char buffer_output[BUF_SIZE];
  struct hostent *server_address;
  struct sockaddr_in ip_server;
  struct timespec start_hash, end_hash;
  uint64_t delta_us_hash;

  if(argc < 2)
    fatal("Para usar: %s <IP-servidor>\n", argv[0]);

  server_address = gethostbyname(argv[1]);	/* get server's IP address */
  if(!server_address)
    fatal("%s: gethostbyname() falhou\n", argv[0]);

  socket_client = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  if(socket_client < 0)
    fatal("%s: socket() falhou\n", argv[0]);

  memset(&ip_server, 0, sizeof(ip_server));
  ip_server.sin_family = AF_INET;
  memcpy(&ip_server.sin_addr.s_addr, server_address->h_addr, server_address->h_length);
  ip_server.sin_port = htons(SERVER_PORT);

  if(connect(socket_client, (struct sockaddr *)&ip_server, sizeof(ip_server)) < 0) {
    fatal("%s: connect() falhou\n", argv[0]);
  }
  else {
    memset(&buffer_input, 0, sizeof(BUF_SIZE));
    memset(&buffer_output, 0, sizeof(BUF_SIZE));
    printf("********************************************************\n");
    printf(" Bem-vindo a aplicacao da Escrita Colaborativa!\n");
    printf("********************************************************\n\n");
    while(strcmp(buffer_input,"sair\n") != 0) {
      printf(" Entre com o texto (com no maximo 1024 caracteres) a ser adicionado no livro \"O Grande Livro\" ou com o texto \"sair\" para encerrar a aplicacao:\n");
      fgets(buffer_input, BUF_SIZE, stdin);

      clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start_hash);
      // O +1 adicionado ao tamanho considera o byte \0 para indicar o fim da string
      write(socket_client, buffer_input, (strlen(buffer_input) + 1));
      read(socket_client, buffer_output, BUF_SIZE);
      printf(" Resposta do servidor: %s\n", buffer_output);
      clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end_hash);

      //Calculando tempo
      delta_us_hash = (end_hash.tv_sec - start_hash.tv_sec) * 1000000 + (end_hash.tv_nsec - start_hash.tv_nsec) / 1000;
      
      printf("\nTotal time = %ld us (10^-6 seconds)\n", delta_us_hash);
    }
  }
  close(socket_client);
  printf("\n********************************************************\n");
  printf(" Aplicacao encerrada. Ate mais!\n");
  printf("********************************************************\n\n");
}
