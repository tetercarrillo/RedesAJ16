

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



unsigned long generador_ids(int id){
		struct timeval t;
        unsigned long identificador_vehiculo;
        gettimeofday(&t,NULL);
        identificador_vehiculo = ((t.tv_sec * 1000 * 1000) + (t.tv_usec * 1000))<< 42;
        identificador_vehiculo |= (id % 16777216) << 24;
        return identificador_vehiculo;

}

int main(){
	int i;
	unsigned long id;
	for(i=0;i<5;i++){
		id = generador_ids(i);
		printf("%lu\n",id);

	}

}

