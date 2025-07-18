// Uso: filename hostname port
// argv[0] filename
// argv[1] hostname
// argv[2] port

#include <netdb.h>
#include <netinet/in.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

// Funcion de error del programa
void error(const char *msg) {
  perror(msg);
  exit(1);
}

int main(int argc, char *argv[]) {
  // Determinar numero de argumentos
  if (argc < 2) {
    fprintf(stderr, "No se proporciono el puerto");
  }

  // FD, Puerto, status (Salida)
  int sockfd, portno, n;
  // Estructura con la configuracion del servidor
  struct sockaddr_in serv_addr;
  // Estructura que almacena informacion de un host dado
  struct hostent *server;

  //buffer donde escribiremos los mensajes del cliente/server
  char buffer[255];
  if (argc > 3) {
    fprintf(stderr, "Llamada incorrecta. Uso: ./%s hostname puerto\n", argv[0]);
    exit(0);
  }

  //Convertir numero de puerto char*->int
  portno = atoi(argv[2]);
  // Socket de internet, TCP, protocolo 0 (TCP)
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  //Error al crear socket
  if (sockfd < 1)
    error("No se pudo crear el FD del socket");

  //convertir char en formato de direccion IP
  server = gethostbyname(argv[1]);
  //Error al conectarse al host
  if (server == NULL) {
    fprintf(stderr, "Error, no existe ese host. ");
  }
  // Limpiar la estructura contenedora de la info del server
  bzero((char *)&serv_addr, sizeof(serv_addr));

  //server IPV4
  serv_addr.sin_family = AF_INET;
  
  //Copiar direccion al server_address 
  // bcopy((char *)server->h_addr_list[0], (char *)&serv_addr.sin_addr.s_addr,
  //       server->h_length);
  memcpy(&serv_addr.sin_addr.s_addr, server->h_addr_list[0], server->h_length);
  //Asignar puerto a la estructura del server
  serv_addr.sin_port = htons(portno);
  //Intentar conexion entre el socket FD y los datos del servidor
  if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    error("No ha sido posible establecer la conexion");

  while (1) {
    //Limpiar buffer
    bzero(buffer, sizeof(buffer));
    //Obtener datos del cliente
    fgets(buffer, sizeof(buffer), stdin);
    n = write(sockfd, buffer, strlen(buffer));
    // Caso de error de n
    if (n < 0)
      error("Error al escribir");
    bzero(buffer, sizeof(buffer));
    //Leer datos que mande el servidor
    n = read(sockfd, buffer, sizeof(buffer) - 1);
    if (n < 0)
      error("Error al momento de leer desde el FD");
    buffer[n] = '\0';
    printf("Server: %s", buffer);

    //Buscar si el usuario introduce un codigo de salida
    if(strncmp("Bye", buffer, 3) == 0)
      break;
  }
  close(sockfd);
  return 0;
}
