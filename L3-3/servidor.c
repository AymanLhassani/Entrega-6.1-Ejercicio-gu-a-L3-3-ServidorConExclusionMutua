#include <stdio.h>
#include <mysql.h>  //libreria para hacer las operaciones con mysql
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <pthread.h>

int contador;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *AtenderCliente(void *socket) {
	
	int sock_conn;
	int *s;
	s = (int *) socket;
	sock_conn = *s;
	
	char peticion[512];
	char respuesta[512];
	int ret;
	
	//Bucle atenci￳n al cliente
	int terminar = 0;
	while(terminar == 0){
		ret = read(sock_conn, peticion, sizeof(peticion));
		printf("Recibido\n");
		
		peticion[ret]='\0';
		
		printf("Peticion: %s\n", peticion);
		
		char *p = strtok(peticion, "/");
		int codigo = atoi(p);
		char nombre[20];
		char nombreBackWords[20];
		
		
		if((codigo != 0)&&(codigo !=4))
		{
			p = strtok(NULL, "/");
			strcpy(nombre, p);
			printf("Codigo: %d, Nombre: %s\n", codigo, nombre);
		}
		if(codigo == 0)
		   terminar = 1;
		
		else if(codigo == 1)
			sprintf (respuesta, "%d", strlen(nombre));
		else if (codigo == 4)
			sprintf (respuesta, "%d", contador);
		else if (codigo == 2){
			if((nombre[0] == 'M') || (nombre[0]=='S'))
				strcpy (respuesta, "SI");
			else
				strcpy(respuesta, "NO");
		}
		else if (codigo == 3){
			p = strtok(NULL, "/");
			float altura = atof (p);
			if(altura > 1.70)
				sprintf(respuesta, "%s: eres alto", nombre);
			else
				sprintf(respuesta, "%s: eres bajo", nombre);
		}
		else {
			
			int i;
			for(i=0;i<(strlen(nombre));i++){
				nombreBackWords[strlen(nombre) - i] = nombre[i];
			}
			printf("%s\n", nombreBackWords);
			if(strcmp(nombre,nombreBackWords) == 0)
				sprintf(respuesta, "Tu nombre SI es palindromo");
			else
				sprintf(respuesta, "Tu nombre NO es palindromo");
		}
		
		if(codigo != 0){
			printf("Respuesta: %s\n", respuesta);
			
			write (sock_conn, respuesta, strlen(respuesta));
		}
		if((codigo == 1)|| (codigo == 2) || (codigo ==3))
		   pthread_mutex_lock( &mutex); //no interrumpas
		   contador=contador+1;
		   pthread_mutex_unlock(&mutex); //ya se puede interrumpir
	}
	close(sock_conn);
	
}

int main(int argc, char *argv[]){
	int sock_conn, sock_listen, ret; 
	struct sockaddr_in serv_adr;
	char peticion[512];
	char respuesta[512];
	if((sock_listen = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		error("Error creant socket");
	
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_adr.sin_port = htons(9080);
	
	if(bind(sock_listen, (struct sockaddr*)&serv_adr, sizeof(serv_adr))<0)
		printf("Error al bind\n");
	
	if(listen(sock_listen, 3)<0)
		printf("Error en listen\n");
	
	int i=0;
	int sockets[100];
	//todos los identificadores los metera en el mismo sitio, ya que de momento no los utilizaremos
	pthread_t thread;
	
	for(;;){
		printf("Escuchando\n");
		
		sock_conn = accept(sock_listen, NULL, NULL);
		printf("He recibido conexion\n");
		
		sockets[i] = sock_conn;
		
		pthread_create (&thread, NULL, AtenderCliente, &sockets[i]);
		i++;
	}
	//for(i=0;i<5;i++){
	//pthread_join(thread[i], NULL);
	//}
}



