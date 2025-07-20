#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#define BUFFER_SIZE 4096

//Funcion que enviara mensaje de error y salir del programa
void error(const char* msg){
  perror(msg);
  exit(1);
}

int main(int argc, char* argv[]){
  printf("argc size: %i\n", argc);
  if(argc < 2){
    //Verificar que se de el puerto en la llamada
    fprintf(stderr,"Falto proveer el puerto\n");
    exit(1);
  }
  int sockfd, newsockfd, portno, n;
  
  char buffer[BUFFER_SIZE];

  struct sockaddr_in serv_addr, cli_addr;

  socklen_t clilength;

  //Crear socket
  // Dominio: AF_INET, TIPO TCP (SOCK_STREAM), Protocolo 0 (TCP por defecto)
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  fprintf(stdout, "Socket file descriptor: %i\n",sockfd);
  //sockfd err statusCode < 0
  if (sockfd < 1){
    error("Error al abrir el socket");
  }

  //Limpia la estructura serv_addr (Por defecto se rellena de valores basura)
  bzero((char*) &serv_addr, sizeof(serv_addr));
  //Castear valor argv (Puerto)
  portno = atoi(argv[1]);

  //Configuraciones del server
  serv_addr.sin_family = AF_INET; //IPv4
  serv_addr.sin_addr.s_addr = INADDR_ANY; //Escucha en cualquier interfaz
  serv_addr.sin_port = htons(portno); //Convierte a formato de red

  //Asocia el socket a la direccion IP y el puerto seleccionados
  if(bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    error("Fallo al vincular datos del socketfd");

  listen(sockfd, 5); //Va aceptar 5 conexiones a nuestro FD (socket)
  
  clilength = sizeof(cli_addr);

  //Nuevo socket que acepta las conexiones del socket anterior 
  newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilength);

  if(newsockfd < 0) 
    error("Error al aceptar conexiones");
  
  while(1){
    //Limpiar contenido dentro del buffer
    bzero(buffer, BUFFER_SIZE);
    n = read(newsockfd, buffer, BUFFER_SIZE);
    //fallo al leer el FD
    if( n < 0)
      error("Error al leer [server] en newsockfd");
    //Imprimir mensaje del cliente 
    printf("OUTPUT: %s\n", buffer);
    bzero(buffer, BUFFER_SIZE);
    //Obtener mensaje del server
    fgets(buffer, BUFFER_SIZE, stdin);

    n = write(newsockfd, buffer, strlen(buffer));

    //Error al escribir en el buffer desde el server
    if( n < 0 )
      error("Error al enviar [Server] newsockfd");

    //Salir si se escribe bye
    int i = strncmp("Bye",buffer,3);
    if(i == 0)
    break;
    
  }
  close(newsockfd);
  close(sockfd);
  return 0;
}
