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
#define SERVER_PORT 4321
#define BUFFER_LEN 1024
int main(int argc, char *argv[])
{
int sockfd; /* descriptor a usar con el socket */
struct sockaddr_in their_addr; /* almacenara la direccion IP y numero de puerto del servidor */
struct hostent *he; /* para obtener nombre del host */
int addr_len, numbytes; /* conteo de bytes a escribir */
char buf[BUFFER_LEN];
char buf_entrada[BUFFER_LEN];	
char s[100];
if (argc != 3) {
fprintf(stderr,"\nuso: %s cliente hostname mensaje\n", argv[0]);
exit(1);
}
/* convertimos el hostname a su direccion IP */
if ((he=gethostbyname(argv[1])) == NULL) {
perror("gethostbyname");
exit(1);
}


/* Creamos el socket */
if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
perror("socket");
exit(2);
}
/* a donde mandar */
their_addr.sin_family = AF_INET; /* usa host byte order */
their_addr.sin_port = htons(SERVER_PORT); /* usa network byte order */
their_addr.sin_addr = *((struct in_addr *)he->h_addr);
bzero(&(their_addr.sin_zero), 8); /* pone en cero el resto */
/* enviamos el mensaje */
if
((numbytes=sendto(sockfd,argv[2],strlen(argv[2]),0,(struct sockaddr*) & their_addr,
sizeof(struct sockaddr))) == -1) {
perror("sendto");
exit(2);
}
printf("enviados %d bytes hacia %s\n",numbytes,inet_ntoa(their_addr.sin_addr));


while(1){
	if ((numbytes=recvfrom(sockfd, buf_entrada, BUFFER_LEN, 0, (struct sockaddr *)&their_addr, (socklen_t *)&addr_len)) == -1) {
	perror("recvfrom");
	exit(3);
	}

	/* Se visualiza lo recibido */
	printf("paquete proveniente de : %s\n",inet_ntoa(their_addr.sin_addr));
	printf("longitud del paquete en bytes: %d\n",numbytes);
	buf_entrada[numbytes] = '\0';
	printf("el paquete contiene: %s\n", buf_entrada);

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
