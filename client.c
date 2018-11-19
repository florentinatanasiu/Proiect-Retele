#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>

int main()
{
   int socket_desc, val;
   struct sockaddr_in client_addr;
   char buffer[2000];

   printf("Introduceti portul:\n");
   int port;
   scanf("%d", &port);

   socket_desc = socket(AF_INET, SOCK_STREAM, 0);

   client_addr.sin_family = AF_INET;
   client_addr.sin_addr.s_addr =  INADDR_ANY; 
   client_addr.sin_port = htons(port);

  
   if(connect(socket_desc, (struct sockaddr*)&client_addr, sizeof(client_addr)) == 0)
	printf("CONEXIUNE REUSITA!\n\n");
   else
   {
        perror("Conecatarea la server esuata !\n");
        exit(EXIT_FAILURE);
   }

   while(1) 
   {
	
	bzero(buffer,2000);
	
	read(socket_desc,buffer,2000); 
	printf("Mesaj din partea serverului:\n %s \n",buffer);
        printf("Mesaj catre server: ");

	bzero(buffer,2000);
        scanf(" %[^\t\n]s",&buffer);
      
        if(strcmp(buffer,"iesi")==0)
        {
            close(socket_desc);
            exit(1);
        }
	
	write(socket_desc,buffer,strlen(buffer));
	bzero(buffer,2000);
	
    }
	close(socket_desc);

	return 0;	
}
