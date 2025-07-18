#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
static int daemongen(char* const argv[]){
  //Separar padre e hijo con primer fork
  pid_t pid = fork();

  //Manejo de errores al crear fork()
  if(pid < 0) return -1;
  if(pid > 0) return (int)pid; //Devuelve PID al caller

  //Crear nueva sesion para ejecutarse fuera de la terminal
  if(setsid() == -1 ) _exit(1);
  
  //Segundo fork(Separar proceso que se asigna en el argv )
  pid = fork();
  if(pid < 0) _exit(1);
  if(pid > 0) _exit(0); //Primer hijo sale


  int fd = open("log.txt", O_WRONLY | O_CREAT | O_TRUNC);

  if(fd != -1){
    dup2(fd, STDIN_FILENO);
    dup2(fd, STDOUT_FILENO);
    dup2(fd, STDERR_FILENO);
    if( fd > 2 ) close(fd);
  }

  //Si execv funciona, el programa ya no regresa (exit no se ejecuta)
  execv(argv[0], argv);
  perror("Fallo execv");
  
  _exit(1);
}

int main(){
  signal(SIGCHLD,SIG_IGN);
  char *args[]= {"./systest", NULL};

  int child_pid = daemongen(args);
  if(child_pid < 0){
    perror("Error al lanzar");
    return EXIT_FAILURE;
  }

  fprintf(stdout, "Se lanzo %s con el PID: %d\n",args[0],child_pid+1);
}
