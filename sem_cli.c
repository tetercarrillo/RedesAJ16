/*
* Ejemplo de cliente de chat simple con datagramas (UDP).
*
* Leandro Lucarella - Copyleft 2004
* Basado en diversos ejemplos públicos.
*
*/


#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
#define BUFFER_LEN 1024
#define SERVER_PORT 4321

int main(int argc, char *argv[]){

	int sockfd; /* descriptor a usar con el socket */
	struct sockaddr_in their_addr; /* almacenara la direccion IP y numero de puerto del servidor */
	struct hostent *he; /* para obtener nombre del host */
	int addr_len, numbytes; /* conteo de bytes a escribir */
	char buf[BUFFER_LEN];
	char buf_entrada[BUFFER_LEN];	
	char s[100];
	int num_puerto;
	int opcion;
	char *id_vehiculo;
	char buffer[BUFFER_LEN];

	if (argc != 9) {
        fprintf(stderr,"ERROR, cantidad invalida de argumentos\n");
        fprintf(stderr,"La invocacion correcta es:\n./sem_cli -d <nombre_módulo_atención> -p <puerto_sem_svr> - c <op> -i <identificación_vehiculo>\n");
        exit(1);
    }

    if ((strcmp(argv[1],"-d")!=0) &&
        (strcmp(argv[1],"-p")!=0) &&
        (strcmp(argv[1],"-c")!=0) &&
        (strcmp(argv[1],"-i")!=0)) {

        fprintf(stderr,"ERROR, flag invalido.\nEl primer flag puede tener las siguientes opciones:\n-d\n-p\n-c\n-i\n");
        exit(1);
    }

    if ((strcmp(argv[3],"-d")!=0) &&
        (strcmp(argv[3],"-p")!=0) &&
        (strcmp(argv[3],"-c")!=0) &&
        (strcmp(argv[3],"-i")!=0)) {

        fprintf(stderr,"ERROR, flag invalido.\nEl segundo flag puede tener las siguientes opciones:\n-d\n-p\n-c\n-i\n");
        exit(1);
    }

    if ((strcmp(argv[5],"-d")!=0) &&
        (strcmp(argv[5],"-p")!=0) &&
        (strcmp(argv[5],"-c")!=0) &&
        (strcmp(argv[5],"-i")!=0)) {

        fprintf(stderr,"ERROR, flag invalido.\nEl terce flag puede tener las siguientes opciones:\n-d\n-p\n-c\n-i\n");
        exit(1);
    }

    if ((strcmp(argv[7],"-d")!=0) &&
        (strcmp(argv[7],"-p")!=0) &&
        (strcmp(argv[7],"-c")!=0) &&
        (strcmp(argv[7],"-i")!=0)) {

        fprintf(stderr,"ERROR, flag invalido.\nEl cuarto flag puede tener las siguientes opciones:\n-d\n-p\n-c\n-i\n");
        exit(1);
    }

    //COMPARACION 1111111
    if (strcmp(argv[1],"-d")==0){
    	/* convertimos el hostname a su direccion IP */
		if ((he=gethostbyname(argv[2])) == NULL) {
		perror("gethostbyname");
		exit(1);
		}      
    }
    if (strcmp(argv[1],"-p")==0){
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

    if (strcmp(argv[1],"-c")==0){

        if ((strcmp(argv[2],"e")!=0 ) && (strcmp(argv[2],"s")!=0)){
     		fprintf(stderr, "ERROR, opcion invalida. Ingrese la opcion 'e' o 's'\n");
     		exit(1);
     	}
     	if (strcmp(argv[2],"e") == 0 ){
     		opcion = 1 ;
     	}
     	if (strcmp(argv[2],"s") == 0 ){
     		opcion = -1 ;
     	}
    
    }

    if (strcmp(argv[1],"-i")==0){
    	id_vehiculo = argv[2];
    }


    //COMPARACION 33333333///////////////////////////////////////////////////
    if (strcmp(argv[3],"-d")==0){
    	/* convertimos el hostname a su direccion IP */
		if ((he=gethostbyname(argv[4])) == NULL) {
		perror("gethostbyname");
		exit(1);
		}      
    }
    if (strcmp(argv[3],"-p")==0){
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

    if (strcmp(argv[3],"-c")==0){

        if ((strcmp(argv[4],"e")!=0 ) && (strcmp(argv[4],"s")!=0)){
     		fprintf(stderr, "ERROR, opcion invalida. Ingrese la opcion 'e' o 's'\n");
     		exit(1);
     	}
     	if (strcmp(argv[4],"e") == 0 ){
     		opcion = 1 ;
     	}
     	if (strcmp(argv[4],"s") == 0 ){
     		opcion = -1 ;
     	}
    
    }

    if (strcmp(argv[3],"-i")==0){
    	id_vehiculo = argv[4];
    }

    //COMPARACION 55555555555555///////////////////////////////////////////////////
    if (strcmp(argv[5],"-d")==0){
    	/* convertimos el hostname a su direccion IP */
		if ((he=gethostbyname(argv[6])) == NULL) {
		perror("gethostbyname");
		exit(1);
		}      
    }
    if (strcmp(argv[5],"-p")==0){
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

    if (strcmp(argv[5],"-c")==0){

        if ((strcmp(argv[6],"e")!=0 ) && (strcmp(argv[6],"s")!=0)){
     		fprintf(stderr, "ERROR, opcion invalida. Ingrese la opcion 'e' o 's'\n");
     		exit(1);
     	}
     	if (strcmp(argv[6],"e") == 0 ){
     		opcion = 1 ;
     	}
     	if (strcmp(argv[6],"s") == 0 ){
     		opcion = -1 ;
     	}
    
    }

    if (strcmp(argv[5],"-i")==0){
    	id_vehiculo = argv[4];
    }

    //COMPARACION 7777777777777///////////////////////////////////////////////////
    if (strcmp(argv[7],"-d")==0){
    	/* convertimos el hostname a su direccion IP */
		if ((he=gethostbyname(argv[8])) == NULL) {
		perror("gethostbyname");
		exit(1);
		}      
    }
    if (strcmp(argv[7],"-p")==0){
        if (!sscanf( argv[8], "%d", &num_puerto)){
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

    if (strcmp(argv[7],"-c")==0){

        if ((strcmp(argv[8],"e")!=0 ) && (strcmp(argv[4],"s")!=0)){
     		fprintf(stderr, "ERROR, opcion invalida. Ingrese la opcion 'e' o 's'\n");
     		exit(1);
     	}
     	if (strcmp(argv[8],"e") == 0 ){
     		opcion = 1 ;
     	}
     	if (strcmp(argv[8],"s") == 0 ){
     		opcion = -1 ;
     	}
    
    }

    if (strcmp(argv[7],"-i")==0){
    	id_vehiculo = argv[8];
    }


	/* Creamos el socket */
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		perror("socket");
		exit(2);
	}

	// ANADIMOS EN EL BUFFER LA INFORMACION RESPECTO A LA ENTRADA O SALIDA DEL VEHICULO Y SU PLACA
	sprintf(buffer,"%d %s %s",opcion," ",id_vehiculo);


	/* a donde mandar */
	their_addr.sin_family = AF_INET; /* usa host byte order */
	their_addr.sin_port = htons(SERVER_PORT); /* usa network byte order */
	their_addr.sin_addr = *((struct in_addr *)he->h_addr);
	bzero(&(their_addr.sin_zero), 8); /* pone en cero el resto */
	/* enviamos el mensaje */
	if
		((numbytes=sendto(sockfd,buffer,strlen(buffer),0,(struct sockaddr*) & their_addr,
		sizeof(struct sockaddr))) == -1) {
		perror("sendto");
		exit(2);
	}


	while(1){
		if ((numbytes=recvfrom(sockfd, buf_entrada, BUFFER_LEN, 0, (struct sockaddr *)&their_addr, (socklen_t *)&addr_len)) == -1) {
			perror("recvfrom");
			exit(3);
		}


		buf_entrada[numbytes] = '\0';
		printf("LA INFORMACION DEL ESTACIOAMIENTO ES: %s\n", buf_entrada);

		    fgets(s, sizeof(s), stdin);
		    sprintf(buf, "%s", s);
		    
		    if ((sendto(sockfd,buf,strlen(buf),0,(struct sockaddr *)&their_addr, sizeof(struct sockaddr))) == -1) {
		      perror("sendto");
		      exit(2);
		    }
}
/* cierro socket */
//close(sockfd);
//exit (0);
}