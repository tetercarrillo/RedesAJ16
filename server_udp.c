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
#define SERVER_PORT 4321
#define BUFFER_LEN 1024
#define VEN (-4.5)

typedef struct vehiculo{
	char *placa_vehiculo;
	int segundos_entrada;
	unsigned long identificador;
} vehiculos;

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

unsigned long generador_ids(int id){
		struct timeval t;
        unsigned long identificador_vehiculo;
        gettimeofday(&t,NULL);
        identificador_vehiculo = ((t.tv_sec * 1000 * 1000) + (t.tv_usec * 1000))<< 42;
        identificador_vehiculo |= (id % 16777216) << 24;
        return identificador_vehiculo;

}


int main(int argc, char *argv[]){
	char buf_salida[255] = "";
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
	int rep_l,rep_o,rep_i;
	rep_i =0;
	rep_o = 0;
	rep_l = 0;


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
	vehiculos veh_estacionados[200];
	memset(veh_estacionados, 0, sizeof(veh_estacionados));

	while (1){
		/* Se reciben los datos (directamente, UDP no necesita conexión) */
		int dia_incio, mes_inicio, ano_inicio, h_inicio, m_inicio, s_inicio,tarifa_total;
		addr_len = sizeof(struct sockaddr);
		printf("Esperando datos ....\n");

		if ((numbytes=recvfrom(sockfd, buf, BUFFER_LEN, 0, (struct sockaddr *)&their_addr,
			(socklen_t *)&addr_len)) == -1) {
			perror("recvfrom");
			exit(3);
		}

		/* Se visualiza lo recibido */
		char *ptr;
		int opcion;
		char *placa_vehiculo;

		buf[numbytes] = '\0';
		ptr = strtok(buf, " ");

		/* Obtengo si el vehiculo desea salir o entrar*/
		opcion = atoi(ptr);

		while(ptr != NULL){
			/* Obtengo la placa del vehiculo */
			placa_vehiculo = ptr;
			ptr = strtok(NULL, " ");
		}
	

		/* Se visualiza lo recibido */
		//printf("paquete proveniente de : %s\n",inet_ntoa(their_addr.sin_addr));
		//printf("longitud del paquete en bytes: %d\n",numbytes);
		if (opcion == 1){
			printf("EL VEHICULO DESEA ENTRAR\n");
			printf("LA PLACA DEL VEHICULA ES %s",placa_vehiculo);
		}

		else{
			printf("EL VEHICULO DESEA SALIR\n");
			printf("LA PLACA DEL VEHICULA ES %s",placa_vehiculo);


		}
		/////// IF LA OPCION DE BUFFER ES ENTRADAAAAAAAAAAAA
		time_t inicio = time(NULL);
        struct tm *tmp = localtime(&inicio);

		// PARA IMPRIMIR EN EL TICKET
        dia_incio = tmp->tm_mday;
        mes_inicio = tmp->tm_mon + 1;
        ano_inicio = tmp->tm_year;
        h_inicio = tmp->tm_hour;
        m_inicio = tmp->tm_min;
        s_inicio = tmp->tm_sec;

        sprintf(buf_salida,"Fecha: %d/%d/%d \n Hora: %d:%d:%d",dia_incio,mes_inicio,ano_inicio,h_inicio,m_inicio,s_inicio);	




        //////////// IF LA OPCION ES DE SALIDAAAAAA
        //time_t salida =time(NULL);
        //tarifa_total = tarifa(inicio,salida); 



		/* cerramos descriptor del socket */
		//close(sockfd);
		//exit (0);
		if((numbytes=sendto(sockfd,buf_salida,strlen(buf_salida),0,(struct sockaddr*) & their_addr,
			sizeof(struct sockaddr))) == -1) {
				perror("sendto");
				exit(2);
		}
	}
}



