/*
* Fecha: 27/06/2016 
* Nombre del archivo: sem_svr.c
* Descripción: Este archivo contiene el script del manejo de sockets 
* del servidor para el Centro Comercial Moriah.
* Integrantes: Daniel Zeait 08-11216
*              Maria Esther Carrillo 10-10122
*
*/

#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
#define BUFFER_LEN 255

int main(int argc, char *argv[]){

	int sockfd; // Descriptor para el socket
	struct sockaddr_in their_addr; // Direccion IP y numero de puerto local
	struct hostent *he; // Para obtención del host
	int addr_len, numbytes; /* addr_len contiene el tamaño de la estructura sockadd_in
    numbytes elnumero de bytes recibidos */
	char buf[BUFFER_LEN]; // Buffer con la información a mandar
	char buf_entrada[BUFFER_LEN]; // Buffer con la información de entrada del servidor
	char s[100];
	int num_puerto; // Número de puerto pasado como parámetro de entrada 
    int numbytes_send; // Número de bytes mandados al servidor
    struct timeval timeout; // Para llevar el tiempo de timeout
	int opcion,i;
	char *id_vehiculo; 
	char buffer[BUFFER_LEN];
    char *ptr;
    int bytes_recv;
    char mensaje[BUFFER_LEN];
    char buf_aux[BUFFER_LEN];

    // Se verifica que la cantidad de argumentos sea correcta
	if (argc != 9) {
        fprintf(stderr,"ERROR, cantidad invalida de argumentos\n");
        fprintf(stderr,"La invocacion correcta es:\n./sem_cli -d <nombre_módulo_atención> -p <puerto_sem_svr> - c <op> -i <identificación_vehiculo>\n");
        exit(1);
    }
    // Verificación de flag válido para el argumento 1
    if ((strcmp(argv[1],"-d")!=0) &&
        (strcmp(argv[1],"-p")!=0) &&
        (strcmp(argv[1],"-c")!=0) &&
        (strcmp(argv[1],"-i")!=0)) {

        fprintf(stderr,"ERROR, flag invalido.\nEl primer flag puede tener las siguientes opciones:\n-d\n-p\n-c\n-i\n");
        exit(1);
    }
    // Verificación de flag válido para el argumento 3
    if ((strcmp(argv[3],"-d")!=0) &&
        (strcmp(argv[3],"-p")!=0) &&
        (strcmp(argv[3],"-c")!=0) &&
        (strcmp(argv[3],"-i")!=0)) {

        fprintf(stderr,"ERROR, flag invalido.\nEl segundo flag puede tener las siguientes opciones:\n-d\n-p\n-c\n-i\n");
        exit(1);
    }
    // Verificación de flag válido para el argumento 5
    if ((strcmp(argv[5],"-d")!=0) &&
        (strcmp(argv[5],"-p")!=0) &&
        (strcmp(argv[5],"-c")!=0) &&
        (strcmp(argv[5],"-i")!=0)) {

        fprintf(stderr,"ERROR, flag invalido.\nEl terce flag puede tener las siguientes opciones:\n-d\n-p\n-c\n-i\n");
        exit(1);
    }
    // Verificación de flag válido para el argumento 7
    if ((strcmp(argv[7],"-d")!=0) &&
        (strcmp(argv[7],"-p")!=0) &&
        (strcmp(argv[7],"-c")!=0) &&
        (strcmp(argv[7],"-i")!=0)) {

        fprintf(stderr,"ERROR, flag invalido.\nEl cuarto flag puede tener las siguientes opciones:\n-d\n-p\n-c\n-i\n");
        exit(1);
    }

    /* Verificación de qué flag se uitilizó en el cada argumento y la correctitud de la información del mismo */

    // Argumento 1: flag -d
    if (strcmp(argv[1],"-d")==0){
    	/* convertimos el hostname a su direccion IP */
		if ((he=gethostbyname(argv[2])) == NULL) {
		perror("gethostbyname");
		exit(1);
		}      
    }
    // Argumento 1: flag -p
    if (strcmp(argv[1],"-p")==0){
        if (!sscanf( argv[2], "%d", &num_puerto)){
				fprintf(stderr, "ERROR, puerto invalido.\n");
				exit(1);
            }
        else{
        	if (num_puerto!=20122 && num_puerto!=21216){
        		fprintf(stderr, "ERROR, numero de puerto invalido.\nUtilice el puerto 20122 ó 21216\n");
        		exit(1);
        	}
        }
    }
    // Argumento 1: flag -c
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
    // Argumento 1: flag -i
    if (strcmp(argv[1],"-i")==0){
    	id_vehiculo = argv[2];
    }

    // Argumento 3: flag -d
    if (strcmp(argv[3],"-d")==0){
    	/* convertimos el hostname a su direccion IP */
		if ((he=gethostbyname(argv[4])) == NULL) {
		perror("gethostbyname");
		exit(1);
		}      
    }
    // Argumento 3: flag -p
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
    // Argumento 3: flag -c
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
    // Argumento 3: flag -i
    if (strcmp(argv[3],"-i")==0){
    	id_vehiculo = argv[4];
    }

    // Argumento 5: flag -d
    if (strcmp(argv[5],"-d")==0){
    	/* convertimos el hostname a su direccion IP */
		if ((he=gethostbyname(argv[6])) == NULL) {
		perror("gethostbyname");
		exit(1);
		}      
    }
    // Argumento 5: flag -puerto
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
    // Argumento 5: flag -c
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
    // Argumento 5: flag -i
    if (strcmp(argv[5],"-i")==0){
    	id_vehiculo = argv[4];
    }

    // Argumento 7: flag -d
    if (strcmp(argv[7],"-d")==0){
    	/* convertimos el hostname a su direccion IP */
		if ((he=gethostbyname(argv[8])) == NULL) {
		perror("gethostbyname");
		exit(1);
		}      
    }
    // Argumento 7: flag -p
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
    // Argumento 7: flag -c
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
    // Argumento 7: flag -i
    if (strcmp(argv[7],"-i")==0){
    	id_vehiculo = argv[8];
    }

	// Creación del socket 
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		perror("socket");
		exit(2);
	}

    // Inicialización de variables
    timeout.tv_sec = 4;
    timeout.tv_usec = 0;

    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) == -1) {
        perror("Fallo asignando tiempo de espera para el Socket.\n");
        exit(1);
    }
    if (setsockopt (sockfd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) < 0){
        perror("setsockopt failed\n");
    }
    
    // Se añade al buffer la información a mandar al cliente
	sprintf(buffer,"%d %s %s",opcion," ",id_vehiculo);

	their_addr.sin_family = AF_INET; // Uso de host byte order
	their_addr.sin_port = htons(num_puerto); // Uso de network byte order  
	their_addr.sin_addr = *((struct in_addr *)he->h_addr); // Se escuchan todas las IP 
	bzero(&(their_addr.sin_zero), 8); // Se rellena con ceros el resto de la estructura 

    // Se manda la información hasta 3 intentos
	for(i = 0; i < 3; i++){
        // Se manda la información del cliente
        if((numbytes_send=sendto(sockfd,buffer,strlen(buffer),0,(struct sockaddr*) & their_addr, sizeof(struct sockaddr))) == -1) {
        perror("ERROR: No se pudo enviar el mensaje al servidor.\n");
        exit(2);
        }
        //Se recibe la información del servidor
		if ((numbytes=recvfrom(sockfd, buf_entrada, BUFFER_LEN, 0, (struct sockaddr *)&their_addr, (socklen_t *)&addr_len)) == -1) {
            fprintf(stderr, "%s\n", "ERROR: Tiempo de espera agotado. El servidor no responde.");
            exit(3);
        }

        strcpy(buf_aux, buf_entrada);
        buf_aux[numbytes] = '\0';
        buf_entrada[numbytes] = '\0';
        ptr = strtok(buf_aux, "!");
        // Se obtiene la cantidad de bytes recibidos
        bytes_recv = atoi(ptr);
        while(ptr != NULL){
            // Se obtiene la información de respuesta sobre el proceso de entrada o salida
            strcpy(mensaje,ptr);
            ptr = strtok(NULL, "!");

        }
        
        // La cantidad de bytes recibidos es igual a la cantidad de bytes mandados por el servidor
        if (bytes_recv == numbytes_send){
            printf("Los datos enviados al servidor se enviaron correctamente.\n");
            printf("LA INFORMACION DEL ESTACIONAMIENTO ES: %s\n", mensaje);
            break;
        }
        /* La cantidad de bytes recibidos es menor a la cantidad de bytes enviados por lo que 
        se perdió información durante el envío*/
        else if (bytes_recv < numbytes_send){
            i = 0;
            printf("Se ha perdido información durante el envío. Intente de nuevo.\n");
        }
        // La cantidad de bytes recibidos es menor a la cantidad de bytes enviados
        else{
            i = 0;
            printf("Se ha encontrado información duplicada en el elvío de los datos. Intente de nuevo.\n");
        }

        // Se superaron el número de intentos y no hubo respuesta del servidor
        if (i == 2) {
            close(sockfd); // Cierre del socket
            printf("Se ha superado el número de intentos.\n");
            exit(1);
        }
        else {
            printf("El servidor no responde. Intente nuevamente.\n");
        }
		    
    }
    // Cierre del socket
    close(sockfd);
    exit (0);
}