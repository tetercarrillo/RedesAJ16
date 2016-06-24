/*
* Ejemplo de server de chat simple con datagramas (UDP).
*
* Leandro Lucarella - Copyleft 2004
* Basado en otros ejemplos públicos.
*
*/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <string.h> 
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <pthread.h>
#define SERVER_PORT 4321
#define BUFFER_LEN 1024


int main(int argc, char *argv[]){
	char buf_salida[255] = "";
	sprintf(buf_salida,"%s","Bien y tu");	
	int sockfd; /* descriptor para el socket */
	struct sockaddr_in my_addr; /* direccion IP y numero de puerto local */
	struct sockaddr_in their_addr; /* direccion IP y numero de puerto del cliente */
	/* addr_len contendra el taman~o de la estructura sockadd_in y numbytes el
	* numero de bytes recibidos */
	int addr_len, numbytes;
	char buf[BUFFER_LEN]; /* Buffer de recepción */
	int num_puerto;
	char *bitacora_entrada, *bitacora_salida;
	FILE *fp_entrada, *fp_salida;


	if (argc != 7) {
        fprintf(stderr,"ERROR, cantidad invalida de argumentos\n");
        fprintf(stderr,"La invocacion correcta es:\n./sem_svr -l <puerto_sem_svr> -i <bitácora_entrada> -o <bitácora_salida>\n");
        exit(1);
    }

    if ((strcmp(argv[1],"-l")!=0) &&
	    (strcmp(argv[1],"-i")!=0) &&
	    (strcmp(argv[1],"-o")!=0)) {

	    fprintf(stderr,"ERROR, flag invalido.\nEl primer flag puede tener las siguientes opciones:\n-l\n-i\n-o\n");
	    exit(1);
    }

    if ((strcmp(argv[3],"-l")!=0) &&
	    (strcmp(argv[3],"-i")!=0) &&
	    (strcmp(argv[3],"-o")!=0)) {

	    fprintf(stderr,"ERROR, flag invalido.\nEl segundo flag puede tener las siguientes opciones:\n-l\n-i\n-o\n");
	    exit(1);
    }

    if ((strcmp(argv[5],"-l")!=0) &&
	    (strcmp(argv[5],"-i")!=0) &&
	    (strcmp(argv[5],"-o")!=0)) {

	    fprintf(stderr,"ERROR, flag invalido.\nEl tercer flag puede tener las siguientes opciones:\n-l\n-i\n-o\n");
	    exit(1);
    }


    //COMPARACION 1111111
    if (strcmp(argv[1],"-l")==0){
            if (!sscanf( argv[2], "%d", &num_puerto)){
				fprintf(stderr, "ERROR, puerto invalido.\n");
				exit(1);
            }
            else{
            	if (num_puerto<20122){
            		fprintf(stderr, "ERROR, numero de puerto invalido.\nUtilice el puerto 20122 ó 21216\n");
            		exit(1);
            	}
            }
    }
    if (strcmp(argv[1],"-i")==0){
            bitacora_entrada = argv[6];
            fp_entrada = fopen(bitacora_entrada,"w+");
            if (!(fp_entrada)){
   				fprintf(stderr,"ERROR, el archivo de entrada no se abrió correctamente\n");
   			}
   			fprintf(fp_entrada,"Bitácora de entrada\n");
    }

    if (strcmp(argv[1],"-o")==0){
            bitacora_salida = argv[6];
            fp_salida = fopen(bitacora_salida,"w+");
            if (!(fp_salida)){
   				fprintf(stderr,"ERROR, el archivo de salida no se abrió correctamente\n");
   			}
   			fprintf(fp_salida,"Bitácora de salida\n");
    }


    //COMPARACION 33333333
    if (strcmp(argv[3],"-l")==0){
            if (!sscanf( argv[4], "%d", &num_puerto)){
				fprintf(stderr, "ERROR, puerto invalido.\n");
				exit(1);
            }
            else{
            	if (num_puerto<20122){
            		fprintf(stderr, "ERROR, numero de puerto invalido.\nUtilice el puerto 20122 ó 21216\n");
            		exit(1);
            	}
            }
    }
    if (strcmp(argv[3],"-i")==0){
            bitacora_entrada = argv[4];
            fp_entrada = fopen(bitacora_entrada,"w+");
            if (!(fp_entrada)){
   				fprintf(stderr,"ERROR, el archivo de entrada no se abrió correctamente\n");
   			}
   			fprintf(fp_entrada,"Bitácora de entrada\n");
    }

    if (strcmp(argv[3],"-o")==0){
            bitacora_salida = argv[4];
            fp_salida = fopen(bitacora_salida,"w+");
            if (!(fp_salida)){
   				fprintf(stderr,"ERROR, el archivo de salida no se abrió correctamente\n");
   			}
   			fprintf(fp_salida,"Bitácora de salida\n");
    }


    // COMPARACIÓN 555555555555
    if (strcmp(argv[5],"-l")==0){
            if (!sscanf( argv[6], "%d", &num_puerto)){
				fprintf(stderr, "ERROR, puerto invalido.\n");
				exit(1);
            }
            else{
            	if (num_puerto<20122){
            		fprintf(stderr, "ERROR, numero de puerto invalido.\nUtilice el puerto 20122 ó 21216\n");
            		exit(1);
            	}
            }
    }
    if (strcmp(argv[5],"-i")==0){
            bitacora_entrada = argv[6];
            fp_entrada = fopen(bitacora_entrada,"w+");
            if (!(fp_entrada)){
   				fprintf(stderr,"ERROR, el archivo de entrada no se abrió correctamente\n");
   			}
   			fprintf(fp_entrada,"Bitácora de entrada\n");
    }

    if (strcmp(argv[5],"-o")==0){
            bitacora_salida = argv[6];
            fp_salida = fopen(bitacora_salida,"w+");
            if (!(fp_salida)){
   				fprintf(stderr,"ERROR, el archivo de salida no se abrió correctamente\n");
   			}
   			fprintf(fp_salida,"Bitácora de salida\n");
    }



	/* se crea el socket */
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}
	/* Se establece la estructura my_addr para luego llamar a bind() */
	my_addr.sin_family = AF_INET; /* usa host byte order */
	my_addr.sin_port = htons(SERVER_PORT); /* usa network byte order */
	my_addr.sin_addr.s_addr = INADDR_ANY; /* escuchamos en todas las IPs */
	bzero(&(my_addr.sin_zero), 8); /* rellena con ceros el resto de la estructura */
	/* Se le da un nombre al socket (se lo asocia al puerto e IPs) */
	printf("Asignado direccion al socket ....\n");


	if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1) {
		perror("bind");
		exit(2);
	}
	while (1){
		/* Se reciben los datos (directamente, UDP no necesita conexión) */
		addr_len = sizeof(struct sockaddr);
		printf("Esperando datos ....\n");

		
		if ((numbytes=recvfrom(sockfd, buf, BUFFER_LEN, 0, (struct sockaddr *)&their_addr,
			(socklen_t *)&addr_len)) == -1) {
			perror("recvfrom");
			exit(3);
		}

		/* Se visualiza lo recibido */
		printf("paquete proveniente de : %s\n",inet_ntoa(their_addr.sin_addr));
		printf("longitud del paquete en bytes: %d\n",numbytes);
		buf[numbytes] = '\0';
		printf("el paquete contiene: %s\n", buf);
		/* cerramos descriptor del socket */
		//close(sockfd);
		//exit (0);
		if((numbytes=sendto(sockfd,buf_salida,strlen(buf_salida),0,(struct sockaddr*) & their_addr,
			sizeof(struct sockaddr))) == -1) {
				perror("sendto");
				exit(2);
		}
		printf("enviados %d bytes hacia %s\n",numbytes,inet_ntoa(their_addr.sin_addr));
	}
}

