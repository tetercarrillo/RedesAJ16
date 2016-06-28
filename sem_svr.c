/*
* Fecha: 27/06/2016 
* Nombre del archivo: sem_svr.c
* Descripción: Este archivo contiene el script del manejo de sockets 
* del cliente para el Centro Comercial Moriah.
* Integrantes: Daniel Zeait	08-11216
*              Maria Esther Carrillo 10-10122
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
#include <time.h>
#include <sys/time.h>
#include <inttypes.h>
#define BUFFER_LEN 255
#define TICKET_LEN 50
#define PLACA_LEN 30
#define MAX_VEHICULOS 200


/**
 * Estructura que almacena la información
 * de vehículos en el estacionamiento.
 *
 */
typedef struct vehiculo{
	int activo;
	char placa_vehiculo[PLACA_LEN];
	time_t entrada;
	int identificador;
	char ticket_entrada[TICKET_LEN];
} vehiculos;

/**
 * Estructura que almacena la información
 * a ser pasada al hilo creado durante
 * la conexión
 */
typedef struct info {
	int socket;
	int numbytes;
    char *bitacora_entrada;
    char *bitacora_salida;
    char buf[BUFFER_LEN]; /* Buffer de recepción */
    struct sockaddr_in their_addr;
} data;

/* Arreglo global que almacena hasta 200 vehículos*/
vehiculos veh_estacionados[MAX_VEHICULOS];

/* Variable global que indica la cantidad de hilos creados*/
int cont = 0;
/* Variable global que indica la capacidad máxima del estacionamiento*/
int capacidad = MAX_VEHICULOS;

/**
 * Procedimiento que ejecuta las
 * operaciones del vehículo que
 * corresponden al hilo creado
 */
void *handle_connection(void *data);

/**
 * Procedimiento que calcula el monto
 * a cancelar de un vehículo.
 */
int tarifa (time_t inicio, time_t salida);

/**
 * Procedimiento que recibe la placa
 * del vehículo y retorna en qué posición
 * del arreglo se encuentra.
 */
int posicion_vehiculo(char *placa);

/**
 * Procedimiento que recibe una placa y libera la información
 * almacenada en la posición del arreglo y retorna el monto
 * a pagar por el cliente
 */
int salida_vehiculo(char *placa, time_t salida);

/**
 * Procedimiento que almacena la información del
 * vehículo en una de las posiciones del arreglo
 */
void estacionar_vehiculo(char* ticket, char *placa, time_t entrada);

/**
 * Procedimiento que recibe una placa y verifica
 * si se encuentra en el estacionamiento. Retorna 1
 * en caso afirmativo y 0 en caso contrario.
 */
int verificar_placa(char *placa);

/**
 * Funcion principal del programa
 */
int main(int argc, char *argv[]){
	int sockfd, *new_sock; // Descriptor para el socket
	struct sockaddr_in my_addr; // Direccion IP y numero de puerto local
	int num_puerto; // Número de puerto pasado como parámetro de entrada 
	int addr_len, numbytes; /* addr_len contiene el tamaño de la estructura sockadd_in
	numbytes elnumero de bytes recibidos */
    struct sockaddr_in their_addr; // Dirección IP y numero de puerto del cliente
    char *bitacora_entrada,*bitacora_salida; //Archivos de la bitácora de entrada y salida
    char buf[BUFFER_LEN]; // Buffer con la información de entrada del cliente 
    char buf_salida[BUFFER_LEN]; // Buffer con la información de salida para el cliente 
    char buf_aux[BUFFER_LEN]; // Buffer  auxiliar con la información de salida para el cliente

	// Se verifica que la cantidad de argumentos sea correcta
	if (argc != 7) {
        fprintf(stderr,"ERROR, cantidad invalida de argumentos\n");
        fprintf(stderr,"La invocacion correcta es:\n./sem_svr -l <puerto_sem_svr> -i <bitácora_entrada> -o <bitácora_salida>\n");
        exit(1);
    }

    // Verificación de flag válido para el argumento 1
    if ((strcmp(argv[1],"-l")!=0) &&
	    (strcmp(argv[1],"-i")!=0) &&
	    (strcmp(argv[1],"-o")!=0)) {

	    fprintf(stderr,"ERROR, flag invalido.\nEl primer flag puede tener las siguientes opciones:\n-l\n-i\n-o\n");
	    exit(1);
    }
    // Verificación de flag válido para el argumento 3
    if ((strcmp(argv[3],"-l")!=0) &&
	    (strcmp(argv[3],"-i")!=0) &&
	    (strcmp(argv[3],"-o")!=0)) {

	    fprintf(stderr,"ERROR, flag invalido.\nEl segundo flag puede tener las siguientes opciones:\n-l\n-i\n-o\n");
	    exit(1);
    }
    // Verificación de flag válido para el argumento 5
    if ((strcmp(argv[5],"-l")!=0) &&
	    (strcmp(argv[5],"-i")!=0) &&
	    (strcmp(argv[5],"-o")!=0)) {

	    fprintf(stderr,"ERROR, flag invalido.\nEl tercer flag puede tener las siguientes opciones:\n-l\n-i\n-o\n");
	    exit(1);
    }

    /* Verificación de qué flag se uitilizó en el cada argumento y la correctitud de la información del mismo */

    // Argumento 1: flag -l
    if (strcmp(argv[1],"-l")==0){
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
    // Argumento 1: flag -i
    if (strcmp(argv[1],"-i")==0){
            bitacora_entrada = argv[2];
    }
    // Argumento 1: flag -o
    if (strcmp(argv[1],"-o")==0){
            bitacora_salida = argv[2];
    }

    // Argumento 3: flag -l
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
    // Argumento 3: flag -i
    if (strcmp(argv[3],"-i")==0){
            bitacora_entrada = argv[4];

    }
    // Argumento 3: flag -o
    if (strcmp(argv[3],"-o")==0){
            bitacora_salida = argv[4];
    }

    // Argumento 5: flag -l
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
    // Argumento 5: flag -i
    if (strcmp(argv[5],"-i")==0){
            bitacora_entrada = argv[6];
    }
    // Argumento 5: flag -o
    if (strcmp(argv[5],"-o")==0){
            bitacora_salida = argv[6];

    }   	

    // Verificación de que ambos archivos para la bitácora no sean iguales
    if (strcmp(bitacora_entrada,bitacora_salida)==0){
    	printf("ERROR, el nombre del archivo de bitacora de entrada debe ser diferente al de salida.\n");
    	exit(1);
    }

	// Creación del socket 
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}

	//Establecimiento de la estructura my_addr para luego llamar a bind()
	my_addr.sin_family = AF_INET; // Uso de host byte order
	my_addr.sin_port = htons(num_puerto); // Uso de network byte order 
	my_addr.sin_addr.s_addr = INADDR_ANY; // Se escuchan todas las IP 
	bzero(&(my_addr.sin_zero), 8); // Se rellena con ceros el resto de la estructura 

	// Se le asigna un nombre al socket
	if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1) {
		perror("bind"); 
		exit(2);
	}


	while (1){
        addr_len = sizeof(struct sockaddr);
        printf("Esperando datos de la taquilla\n");

        // Se recibe la información del cliente
        if ((numbytes=recvfrom(sockfd, buf, BUFFER_LEN, 0, (struct sockaddr *)&their_addr,
            (socklen_t *)&addr_len)) == -1) {
            perror("recvfrom");
            exit(3);
        }
        else{
        	// Caso en el que no se ejecutan las 3 taquillas a la vez
            if(cont < 3){
            	/*Creación de hilos y asignación de los respectivos campos de la estructura
            	a ser pasada a la función*/
                cont++;
                // Reservación del espacio para la data de los hilos
                data* mis_datos = malloc(sizeof(data));
                mis_datos->socket = sockfd;
                mis_datos->numbytes = numbytes;
                mis_datos->bitacora_entrada = bitacora_entrada;
                mis_datos->bitacora_salida = bitacora_salida;
                strcpy(mis_datos->buf,buf);
                mis_datos->their_addr = their_addr;
                pthread_t tid;
                // Creación del hilo
                if (pthread_create(&tid,NULL,&handle_connection,mis_datos)) {
                    fprintf(stderr, "No threads for you.\n");
                    return 1;
                }
                // Se libera el hilo
                pthread_detach(tid);    
            }
            // Caso en el que las tres taquillas se ejecutan a la vez
            else{
                char bytes[5];
                sprintf(bytes, "%d", numbytes);
                strcat(bytes,"!");
                memset(buf_salida, 0, sizeof(buf_salida));
                strcpy(buf_salida,bytes);
                sprintf(buf_aux,"Las taquillas se encuentran ocupadas. Por favor espere.");
                strcat(buf_salida,buf_aux);
                // Se envia la informacion al cliente
                if((numbytes=sendto(sockfd,buf_salida,strlen(buf_salida),0,(struct sockaddr*) & their_addr,
                sizeof(struct sockaddr))) == -1) {
                    perror("sendto");
                    exit(2);
                }
            }
                
        }
        
	}
	// Cierre descriptor del socket
	close(sockfd);
}

void *handle_connection(void *datos) {
    data recibe = *((data *)datos); // Creación de la estructura
    // Asingación de los campos recibidos a variables
    int sockfd = recibe.socket;
    int numbytes = recibe.numbytes;
    char buf[BUFFER_LEN];
    strcpy(buf, recibe.buf);
    char *bitacora_entrada = recibe.bitacora_entrada;
    char *bitacora_salida = recibe.bitacora_salida;
    char buf_salida[BUFFER_LEN];
	char buf_aux[BUFFER_LEN];
	char ticket_entrada[TICKET_LEN];
	char ticket_salida[TICKET_LEN];
	FILE *fp_entrada, *fp_salida;
    struct sockaddr_in their_addr = recibe.their_addr;
	char *ptr;
	int opcion, p, tarifa_total;
	char placa_vehiculo[PLACA_LEN],identificador[10];
	
	// Se guarda la cantidad de bytes recibidos en bytes
	char bytes[5];
	sprintf(bytes, "%d", numbytes);
	strcat(bytes,"!");

	// Se visualiza lo recibido por el cliente
	

	buf[numbytes] = '\0';
	ptr = strtok(buf, " ");

	// Petición de entrada o salida del vehículo
	opcion = atoi(ptr);
	while(ptr != NULL){
		// Obtención de la placa del vehículo
		strcpy(placa_vehiculo,ptr);
		ptr = strtok(NULL, " ");
	}


	// El vehículo solicitó entrar estacionamiento
	if (opcion == 1){
		int i;
		// Se chequea si hay puestos disponibles para el nuevo vehículo
		if (capacidad>0){
			// Se verifica que no haya un vehiculo con la placa de entrada
			if (verificar_placa(placa_vehiculo) == 1){
				memset(buf_salida, 0, sizeof(buf_salida));
				strcpy(buf_salida,bytes);
				sprintf(buf_aux,"ERROR, ya existe un vehiculo con la placa %s estacionado",placa_vehiculo);
				strcat(buf_salida,buf_aux);
				// Se le notifica al cliente de que no puede ingresar un carro con dicha placa
    			if((numbytes=sendto(sockfd,buf_salida,strlen(buf_salida),0,(struct sockaddr*) & their_addr,
					sizeof(struct sockaddr))) == -1) {
					perror("sendto");
					exit(2);
				}
				cont--;
			}

			// Hay puestos disponibles para el nuevo ingreso
			else{
				// Se decrementa la capacidad del estacionamiento
				capacidad--;
				// Se obtiene el tiempo de ingreso
				time_t inicio = time(NULL);
				// Se transforma a zona horaria local
	    		struct tm *tmp = localtime(&inicio);
	    		// Se imprime la fecha en el tícket de entrada
	    		strftime(ticket_entrada, sizeof(ticket_entrada), "%a %Y-%m-%d %H:%M:%S %Z", tmp);
	    		// Se ingresa la información del vehículo a una posición del arreglo de vehículos
	    		estacionar_vehiculo (ticket_entrada,placa_vehiculo,inicio);
	    		// Se obtiene la posición del arreglo donde se ingresó el vehículo
	    		p = posicion_vehiculo(placa_vehiculo);
	    		// Se abre la bitácora de entrada
	    		fp_entrada = fopen(bitacora_entrada,"a");
		        if (!(fp_entrada)){
					fprintf(stderr,"ERROR, el archivo de salida no se abrió correctamente\n");
		   		}
		   		// Se imprime la información correspondiente al ingreso
		   		fprintf(fp_entrada,"FECHA Y HORA DE INGRESO: %s  PLACA VEHICULO: %s CODIGO VEHÍCULO: %d	\n",
	   			veh_estacionados[p].ticket_entrada,placa_vehiculo,veh_estacionados[p].identificador);
	    		fclose(fp_entrada);

	    		memset(buf_salida, 0, sizeof(buf_salida));
	    		strcpy(buf_salida,bytes);
	    		sprintf(buf_aux,"El codigo del ticket es %d\nLa hora de ingreso es %s",
	    			veh_estacionados[p].identificador,ticket_entrada);
	    		strcat(buf_salida,buf_aux);
                
                // Se le manda la información al cliente respecto a su ticket de estacioamiento
	    		if((numbytes=sendto(sockfd,buf_salida,strlen(buf_salida),0,(struct sockaddr*) & their_addr,
				sizeof(struct sockaddr))) == -1) {
					perror("sendto");
					exit(2);
				}
				cont--;
			}
		}

		// No hay puestos disponibles para el ingreso del vehículo
		else{
			memset(buf_salida, 0, sizeof(buf_salida));
			strcpy(buf_salida,bytes);
			sprintf(buf_aux,"El estacionamiento esta lleno. Por favor espere.");
			strcat(buf_salida,buf_aux);

			// Se envia la informacion al cliente de que no hay puestos dispinibles
    		if((numbytes=sendto(sockfd,buf_salida,strlen(buf_salida),0,(struct sockaddr*) & their_addr,
			sizeof(struct sockaddr))) == -1) {
				perror("sendto");
				exit(2);
			}
			cont--;
		}
		
	}


	// El vehículo desea salir del estacionamiento
	else{
		// Se verifica que haya un vehículo con la placa de la solicitud de salida
		if (verificar_placa(placa_vehiculo) == 1){
			// Aumenta la capacidad del estacionamiento
			capacidad++;
			// Se obtiene el tiempo de salida
	    	time_t fin = time(NULL);
	    	// Se convierte a la zona horaria local
	    	struct tm *tmp = localtime(&fin);
	    	// Imrpime la hora de salida del vehículo
	    	strftime(ticket_salida, sizeof(ticket_salida), "%a %Y-%m-%d %H:%M:%S %Z", tmp);
	    	// Se obtiene la posición del vehículo
    		p = posicion_vehiculo(placa_vehiculo);
	    	// Calculo de tarifa 
    		tarifa_total = salida_vehiculo(placa_vehiculo,fin);

    		// Se abre el archivo de la bitácora de salida
	    	fp_salida = fopen(bitacora_salida,"a");
	        if (!(fp_salida)){
				fprintf(stderr,"ERROR, el archivo de salida no se abrió correctamente\n");
	   		}
	   		// Se imprime la información correspondiente a la salida
	   		fprintf(fp_salida,"FECHA Y HORA DE INGRESO: %s 	FECHA Y HORA DE SALIDA: %s	PLACA VEHICULO: %s 	CODIGO VEHÍCULO: %d	 MONTO A CANCELAR: %d\n",
	   			veh_estacionados[p].ticket_entrada,ticket_salida,placa_vehiculo,veh_estacionados[p].identificador,tarifa_total);

	    	fclose(fp_salida);

    		memset(buf_salida, 0, sizeof(buf_salida));
    		strcpy(buf_salida,bytes);
    		memset(buf_aux, 0, sizeof(buf_aux));
	    	sprintf(buf_aux,"La tarifa total a pagar es %d",tarifa_total);
	    	strcat(buf_salida,buf_aux);
			// Se envia la informacion al cliente
    		if((numbytes=sendto(sockfd,buf_salida,strlen(buf_salida),0,(struct sockaddr*) & their_addr,
			sizeof(struct sockaddr))) == -1) {
				perror("sendto");
				exit(2);
			}
			cont--;
		}

		// El vehículo ingresó con una placa inválida
		else{

			memset(buf_salida, 0, sizeof(buf_salida));
			strcpy(buf_salida,bytes);
			sprintf(buf_aux,"No existe ningún vehiculo con la placa %s\n",placa_vehiculo);
			strcat(buf_salida,buf_aux);
			// Se envia la informacion al cliente
    		if((numbytes=sendto(sockfd,buf_salida,strlen(buf_salida),0,(struct sockaddr*) & their_addr,
			sizeof(struct sockaddr))) == -1) {
				perror("sendto");
				exit(2);
			}
			cont--;
		}
	}
}

/**
 * Procedimiento que calcula el monto
 * a cancelar de un vehículo.
 */
int tarifa (time_t inicio, time_t salida){
	double segundos_totales;
	int minutos,fraccion,fraccion_aux,total;
	segundos_totales = difftime(inicio, salida);
	minutos = segundos_totales/60;

	if (minutos>60){
		minutos = minutos - 60;
		fraccion = minutos/60;
		fraccion_aux = minutos%60;
		if (fraccion_aux>0){
			minutos = fraccion + 1;
		}
		else{
			minutos = fraccion;
		}

		total = 80 + (minutos*30);
	}

	else{
		total = 80;
	}

	return total;
}

/**
 * Procedimiento que recibe la placa
 * del vehículo y retorna en qué posición
 * del arreglo se encuentra.
 */
int posicion_vehiculo(char *placa){
	int i = 0;

	for(i=0; i<MAX_VEHICULOS; i++){

		if (strcmp(veh_estacionados[i].placa_vehiculo,placa)==0){
			return i;
		}
	}
	return -100;
}

/**
 * Procedimiento que recibe una placa y libera la información
 * almacenada en la posición del arreglo y retorna el monto
 * a pagar por el cliente
 */
int salida_vehiculo(char *placa, time_t salida){
	int posicion,tarifa_v;
	posicion = posicion_vehiculo(placa);
	fprintf(stderr,"VOY A SALIR , ESTOY EN SALIDA VEHICULO, MI PLACA ES %s Y ESTOY EN LA POSICION %d",placa,posicion);
	tarifa_v = tarifa (veh_estacionados[posicion].entrada,salida);
	// Se procede a liberar la informacion correspondiente al puesto
	veh_estacionados[posicion].activo = 0;
	memset(veh_estacionados[posicion].placa_vehiculo, 0, sizeof(veh_estacionados[posicion].placa_vehiculo));

	return tarifa_v;

}

/**
 * Procedimiento que almacena la información del
 * vehículo en una de las posiciones del arreglo
 */
void estacionar_vehiculo(char* ticket, char *placa, time_t entrada){
	int i;

	for(i=0;i<MAX_VEHICULOS;i++){
		// Encontrar posición libre del arreglo de vehiculos en el estacionamiento
		if (veh_estacionados[i].activo == 0){
			strcpy(veh_estacionados[i].placa_vehiculo,placa);
			veh_estacionados[i].identificador = i;
			veh_estacionados[i].entrada = entrada;
			veh_estacionados[i].activo = 1;
			strcpy(veh_estacionados[i].ticket_entrada,ticket);
			i=MAX_VEHICULOS;
		}
	}
}

/**
 * Procedimiento que recibe una placa y verifica
 * si se encuentra en el estacionamiento. Retorna 1
 * en caso afirmativo y 0 en caso contrario.
 */
int verificar_placa(char *placa){
	int i = 0;

	for(i=0; i<MAX_VEHICULOS; i++){
		if ((veh_estacionados[i].activo == 1) && (strcmp(veh_estacionados[i].placa_vehiculo,placa)==0)){
			return 1;
		}
	}
	return 0;
}
