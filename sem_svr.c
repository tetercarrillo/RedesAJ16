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
#include <time.h>
#include <sys/time.h>
#include <inttypes.h>
#define BUFFER_LEN 1024
#define MAX_VEHICULOS 200


typedef struct vehiculo{
	int activo;
	char placa_vehiculo[10];
	time_t entrada;
	int identificador;
	char ticket_entrada[50];
} vehiculos;

typedef struct info {
	int socket;
	int numbytes;
    char *bitacora_entrada;
    char *bitacora_salida;
    char buf[BUFFER_LEN]; /* Buffer de recepción */
    struct sockaddr_in their_addr;
} data;

vehiculos veh_estacionados[200];

int cont = 0;

int tarifa (time_t inicio, time_t salida);
int posicion_vehiculo(char *placa);
int salida_vehiculo(char *placa, time_t salida);
unsigned long generador_ids(int id);
void estacionar_vehiculo(char* ticket, char *placa, time_t entrada);
int verificar_placa(char *placa);
void *handle_connection(void *data);



int main(int argc, char *argv[]){
	int sockfd, *new_sock;
	int num_puerto;
	struct sockaddr_in my_addr; /* direccion IP y numero de puerto local */
	int addr_len, numbytes;
    struct sockaddr_in their_addr; /* direccion IP y numero de puerto del cliente */
    char *bitacora_entrada;
    char *bitacora_salida;
    char buf[BUFFER_LEN];
    char buf_salida[255] = "";
    char buf_aux[255] = "";

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


    // Verificacion informacion primer flag
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
            bitacora_entrada = argv[2];
    }

    if (strcmp(argv[1],"-o")==0){
            bitacora_salida = argv[2];
    }


    // Verificacion informacion tercer flag
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

    }

    if (strcmp(argv[3],"-o")==0){
            bitacora_salida = argv[4];
    }


    // Verificacion informacion quinto flag
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
    }

    if (strcmp(argv[5],"-o")==0){
            bitacora_salida = argv[6];

    }   	

    if (strcmp(bitacora_entrada,bitacora_salida)==0){
    	printf("ERROR, el nombre del archivo de bitacora de entrada debe ser diferente al de salida.\n");
    	exit(1);
    }

	/* se crea el socket */
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}
	/* Se establece la estructura my_addr para luego llamar a bind() */
	my_addr.sin_family = AF_INET; /* usa host byte order */
	my_addr.sin_port = htons(num_puerto); /* usa network byte order */
	my_addr.sin_addr.s_addr = INADDR_ANY; /* escuchamos en todas las IPs */
	bzero(&(my_addr.sin_zero), 8); /* rellena con ceros el resto de la estructura */
	/* Se le da un nombre al socket (se lo asocia al puerto e IPs) */
	printf("Asignado direccion al socket ....\n");


	if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1) {
		perror("bind");
		exit(2);
	}

	while (1){
        addr_len = sizeof(struct sockaddr);
        printf("Esperando datos ....\n");

        if ((numbytes=recvfrom(sockfd, buf, BUFFER_LEN, 0, (struct sockaddr *)&their_addr,
            (socklen_t *)&addr_len)) == -1) {
            perror("recvfrom");
            exit(3);
        }
        else{
            if(cont < 3){
                cont++;
                data* mis_datos = malloc(sizeof(data));
                mis_datos->socket = sockfd;
                mis_datos->numbytes = numbytes;
                mis_datos->bitacora_entrada = bitacora_entrada;
                mis_datos->bitacora_salida = bitacora_salida;
                strcpy(mis_datos->buf,buf);
                mis_datos->their_addr = their_addr;
                pthread_t tid;
                if (pthread_create(&tid,NULL,&handle_connection,mis_datos)) {
                    fprintf(stderr, "No threads for you.\n");
                    return 1;
                }
                pthread_detach(tid);    
            }
            else{
                char bytes[5];
                sprintf(bytes, "%d", numbytes);
                strcat(bytes,"!");
                fprintf(stderr,"BYTES QUE RECIBI  %s\n",bytes);
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
		// new_sock = malloc(1);
  //       *new_sock = sockfd;
        
	}
	/* cerramos descriptor del socket */
	close(sockfd);
	//exit (0);
}

void *handle_connection(void *datos) {
	// int sockfd = *(int*) data;
    data recibe = *((data *)datos);
    int sockfd = recibe.socket;
    int numbytes = recibe.numbytes;
    char buf[BUFFER_LEN];
    strcpy(buf, recibe.buf);
    char *bitacora_entrada = recibe.bitacora_entrada;
    char *bitacora_salida = recibe.bitacora_salida;
    char buf_salida[255] = "";
	char buf_aux[255] = "";
	char ticket_entrada[50]="";
	char ticket_salida[50]="";
	FILE *fp_entrada, *fp_salida;
	int capacidad = 200;
		 /* descriptor para el socket */
	/* addr_len contendra el taman~o de la estructura sockadd_in y numbytes el
	* numero de bytes recibidos */
	// int addr_len;
    struct sockaddr_in their_addr = recibe.their_addr; /* direccion IP y numero de puerto del cliente */
	

    /* Se reciben los datos (directamente, UDP no necesita conexión) */
	int dia_incio, mes_inicio, ano_inicio, h_inicio, m_inicio, s_inicio,tarifa_total;
	
	char bytes[5];
	sprintf(bytes, "%d", numbytes);
	strcat(bytes,"!");
	fprintf(stderr,"BYTES QUE RECIBI  %s\n",bytes);

	/* Se visualiza lo recibido */
	char *ptr;
	int opcion;
	char placa_vehiculo[10];

	buf[numbytes] = '\0';
	ptr = strtok(buf, " ");

	/* Obtengo si el vehiculo desea salir o entrar*/
	opcion = atoi(ptr);

	while(ptr != NULL){
		/* Obtengo la placa del vehiculo */
		strcpy(placa_vehiculo,ptr);
		ptr = strtok(NULL, " ");
	}
	fprintf(stderr,"LA PLACA DEL VEHICULO QUE ENTRO ES %s\n",placa_vehiculo);
	// ENTRADA DE UN VEHICULO
	if (opcion == 1){
		int i;

		// Se chequea si hay espacio para dejar entrar al vehicula
		if (capacidad>0){
			fprintf(stderr,"ESTOY SOLICITANDO ENTRAR AL ESTANCIONAMIENTO, MI PLACA ES %s\n",placa_vehiculo);
			// Se verifica que no haya un vehiculo con la placa
			if (verificar_placa(placa_vehiculo) == 1){
				fprintf(stderr,"MI PLACA ES %s Y YA EXISTE\n",placa_vehiculo);
				memset(buf_salida, 0, sizeof(buf_salida));
				strcpy(buf_salida,bytes);
				sprintf(buf_aux,"ERROR, ya existe un vehiculo con la placa %s estacionado",placa_vehiculo);
				strcat(buf_salida,buf_aux);
				// Se le notifica al cliente de que no puede ingresar un carro con dicha placa
                cont--;
    			if((numbytes=sendto(sockfd,buf_salida,strlen(buf_salida),0,(struct sockaddr*) & their_addr,
					sizeof(struct sockaddr))) == -1) {
					perror("sendto");
					exit(2);
				}
			}

			else{
				capacidad--;
				int posicion;
				time_t inicio = time(NULL);
	    		struct tm *tmp = localtime(&inicio);

	    		strftime(ticket_entrada, sizeof(ticket_entrada), "%a %Y-%m-%d %H:%M:%S %Z", tmp);
	    		estacionar_vehiculo (ticket_entrada,placa_vehiculo,inicio);

	    		posicion = posicion_vehiculo(placa_vehiculo);

	    		fp_entrada = fopen(bitacora_entrada,"a");
		        if (!(fp_entrada)){
					fprintf(stderr,"ERROR, el archivo de salida no se abrió correctamente\n");
		   		}

		   		fprintf(fp_entrada,"FECHA Y HORA DE INGRESO: %s  PLACA VEHICULO: %s CODIGO VEHÍCULO: %d	\n",
	   			veh_estacionados[posicion].ticket_entrada,placa_vehiculo,veh_estacionados[posicion].identificador);
	    		fclose(fp_entrada);

	    		memset(buf_salida, 0, sizeof(buf_salida));
	    		strcpy(buf_salida,bytes);
	    		strcpy(buf_aux, ticket_entrada);
	    		strcat(buf_salida,buf_aux);
                cont--;
	    		if((numbytes=sendto(sockfd,buf_salida,strlen(buf_salida),0,(struct sockaddr*) & their_addr,
				sizeof(struct sockaddr))) == -1) {
					perror("sendto");
					exit(2);
				}//if sendto
			}// else de placa que no existe en el estacionamiento
		}// if de que hay capacidad en el estacionamiento

		// No hay capacidad para almacenar el vehiculo
		else{
			memset(buf_salida, 0, sizeof(buf_salida));
			strcpy(buf_salida,bytes);
			sprintf(buf_aux,"El estacionamiento esta lleno. Por favor espere.");
			strcat(buf_salida,buf_aux);
			// Se envia la informacion al cliente
            cont--;
    		if((numbytes=sendto(sockfd,buf_salida,strlen(buf_salida),0,(struct sockaddr*) & their_addr,
			sizeof(struct sockaddr))) == -1) {
				perror("sendto");
				exit(2);
			}

		}
		
	} // cierre del if de entrada


	// El vehiculo desea salir del estacionamiento
	else{
		// Verificacion de placa valida
		if (verificar_placa(placa_vehiculo) == 1){
			int tarifa_total,p;
			capacidad++;

			fprintf(stderr,"ESTOY TRATANDO DE SALIR MI PLACA ES %s",placa_vehiculo);
	    	time_t fin = time(NULL);
	    	struct tm *tmp = localtime(&fin);

	    	strftime(ticket_salida, sizeof(ticket_salida), "%a %Y-%m-%d %H:%M:%S %Z", tmp);

	    	// Calculo de tarifa 
    		p = posicion_vehiculo(placa_vehiculo);
    		fprintf(stderr,"ESTOY TRATANDO DE SALIR MI IDENTIFICADOR ES %d",veh_estacionados[p].identificador);
    		tarifa_total = salida_vehiculo(placa_vehiculo,fin);


	    	fp_salida = fopen(bitacora_salida,"a");
	        if (!(fp_salida)){
				fprintf(stderr,"ERROR, el archivo de salida no se abrió correctamente\n");
	   		}
	   		fprintf(fp_salida,"FECHA Y HORA DE INGRESO: %s 	FECHA Y HORA DE SALIDA: %s	PLACA VEHICULO: %s 	CODIGO VEHÍCULO: %d	 MONTO A CANCELAR: %d\n",
	   			veh_estacionados[p].ticket_entrada,ticket_salida,placa_vehiculo,veh_estacionados[p].identificador,tarifa_total);

	    	fclose(fp_salida);

    		memset(buf_salida, 0, sizeof(buf_salida));
    		strcpy(buf_salida,bytes);
    		memset(buf_aux, 0, sizeof(buf_aux));
	    	sprintf(buf_aux,"La tarifa total a pagar es %d",tarifa_total);
	    	strcat(buf_salida,buf_aux);
			// Se envia la informacion al cliente
            cont--;
    		if((numbytes=sendto(sockfd,buf_salida,strlen(buf_salida),0,(struct sockaddr*) & their_addr,
			sizeof(struct sockaddr))) == -1) {
				perror("sendto");
				exit(2);
			}


		}

		// El vehiculo ingreso con una placa invalida
		else{

			memset(buf_salida, 0, sizeof(buf_salida));
			strcpy(buf_salida,bytes);
			sprintf(buf_aux,"No existe ningún vehiculo con la placa %s\n",placa_vehiculo);
			strcat(buf_salida,buf_aux);
			// Se envia la informacion al cliente
            cont--;
    		if((numbytes=sendto(sockfd,buf_salida,strlen(buf_salida),0,(struct sockaddr*) & their_addr,
			sizeof(struct sockaddr))) == -1) {
				perror("sendto");
				exit(2);
			}
		}
	}
}


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

int posicion_vehiculo(char *placa){
	int i = 0;

	for(i=0; i<MAX_VEHICULOS; i++){

		if (strcmp(veh_estacionados[i].placa_vehiculo,placa)==0){
			return i;
		}
	}
	return -100;
}

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


void estacionar_vehiculo(char* ticket, char *placa, time_t entrada){
	int i;

	for(i=0;i<MAX_VEHICULOS;i++){
		fprintf(stderr,"EN LA POSICION %d EL ESTADO ES  %d\n",i,veh_estacionados[i].activo);
	// Encontrar posición libre del arreglo de vehiculos en el estacionamiento
		if (veh_estacionados[i].activo == 0){

			strcpy(veh_estacionados[i].placa_vehiculo,placa);
			veh_estacionados[i].identificador = i;
			veh_estacionados[i].entrada = entrada;
			veh_estacionados[i].activo = 1;
			strcpy(veh_estacionados[i].ticket_entrada,ticket);
			fprintf(stderr,"SE GUARDO UN CARRO EN LA POSICION %d\n",i);
			i=MAX_VEHICULOS;
		}

	}
}

int verificar_placa(char *placa){
	int i = 0;

	for(i=0; i<MAX_VEHICULOS; i++){

		if ((veh_estacionados[i].activo == 1) && (strcmp(veh_estacionados[i].placa_vehiculo,placa)==0)){
			fprintf(stderr,"LA PLACA EN LA POSICION %d ES LA SIGUIENTE %s\n",i,veh_estacionados[i].placa_vehiculo);
			fprintf(stderr,"ESTOY VERIFCICANDO -------> ESTOY EN LA POSICION %d Y MI PLACA ES %s\n",i,placa);
			return 1;
		}
	}
	return 0;
}

