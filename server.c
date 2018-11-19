#include <stdio.h>
#include <string.h>    
#include <stdlib.h> 
#include <unistd.h>   
#include <sys/socket.h>
#include <arpa/inet.h>    
#include <pthread.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "top.c"


#define port 3264
 
void *connect_top(void *);

extern int errno;

 
int main( )
{
    
    nr_piese=populate_songList(p);
    nr_usr=vot_right(users);
    puts("..Done..\n");
  

    int socket_desc , client_sock , c;
    struct sockaddr_in server , client;
     
   
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);

    if (socket_desc == -1)
    {
        perror("can't create the socket");
        return errno;
    }

    puts("Socket created\n");
     
    
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( port);
    
   
    int reuse_adr = 1;

    if (setsockopt(socket_desc, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse_adr, sizeof(reuse_adr)) < 0)
    {
        perror("Can not reuse the adress..\n");
        return errno;
    }
     
    
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        
        perror("bind() error..\n");
        return errno;
    }
   
    puts("Bind done\n");
     
    if (listen (socket_desc, SOMAXCONN) == -1)
    {
        perror ("listen() error..\n");
        return errno;
    }
     
   
    puts("Waiting for users to connect...");

    c = sizeof(struct sockaddr_in);
     
    pthread_t thread_id;
  
	
    while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
    {
        puts("Connection accepted");
         
        if( pthread_create( &thread_id , NULL ,  connect_top , (void*) &client_sock) < 0)
        {
            perror("Could not create thread");
            return errno;
        }
          
    }
     
      if (client_sock < 0)
    {
        perror("accept() failed");
    } 
    return 0;
}


void *connect_top(void *socket_desc)
{
    
    int sock = *(int*)socket_desc;
    int read_size;
    char message[2000] , client_message[2000],activ_usr[100];
    char detalii_piesa[2000]; 
    int acc=0;
    
    bzero(message,2000);

    strcat(message,"Momentan nu esti logat.\n Nu poti accesa informatii daca nu esti logat. \n");
    strcat(message,"Aveti deja un cont? da/nu \n");
    write(sock , message , strlen(message));

    bzero(message,2000);
    bzero(client_message,2000);

    read_size = read(sock , client_message ,2000);
  
    if(strcmp(client_message,"da")==0)
    {
            strcat(message,"Introduceti numele de utilizator:\n");
            write(sock , message , strlen(message));
            bzero(message,2000);
            read_size = read(sock , client_message ,2000);
            acc=login(client_message);
            while(acc==0)
            { 
                 strcat(message,"Cont inexistent..Va rugam introduceti alt nume de utilizator:");
                 write(sock , message , strlen(message));
                 bzero(client_message,2000);
                 read_size = read(sock , client_message ,2000);
                 acc=login(client_message);
             }

             bzero(message,2000);
             strcat(message,"\n conexiune reusita..\n\n   Salut " );
             strcat(message,client_message);
             write(sock , message , strlen(message));
             strcat(activ_usr,client_message);
         
       
    }

    if(strcmp(client_message,"nu")==0)
    {
             bzero(message,2000);
             strcat(message,"Daca nu iti creezi un cont nu poti vedea continutul.Te rog sa alegi un nume de cont:");
             write(sock , message , strlen(message));
             read_size = read(sock , client_message ,2000);

             if(strstr(client_message,"admin")!= NULL)
             {
                 bzero(message,2000);
                 strcat(message,"Nu poti crea un cot folosind numele 'admin'..alege alt nume de cont:");
                 write(sock , message , strlen(message));
                 bzero(client_message,2000);
                 read_size = read(sock , client_message ,2000); 
             }        
             acc=create_newCont(client_message);
             while(acc==1)
             {
                 bzero(message,2000);
                 strcat(message,"Numele deja exista..te rog sa alegi altul " );
                 write(sock , message , strlen(message));
                 bzero(client_message,2000);
                 read_size = read(sock , client_message ,2000);
                 if(strstr(client_message,"admin")!= NULL)
                 { 
                      bzero(message,2000);
                      strcat(message,"Nu poti crea un cot folosind numele 'admin'..alege alt nume de cont:");
                      write(sock , message , strlen(message));
                      bzero(client_message,2000);
                      read_size = read(sock , client_message ,2000); 
                 }        
                 acc=create_newCont(client_message);
             }
             if(acc==0) 
             {
                 
                 bzero(message,2000);
                 strcat(message,"Felicitari..ti-ai creat un nou cont!\n\n Acum esti logat cu numele ");
                 strcat(message,client_message);
                 write(sock , message , strlen(message));
                 strcpy(users[nr_usr].user,message);
                 users[nr_usr].acc=1;
                 nr_usr++;
                 strcat(activ_usr,client_message);
            
             }
    }
       
    bzero(message,2000);
    strcat(message,"\n\n Alege una din comenzile: \n  vot -> voteaza o piesa \n  vezi -> vezi detaliile unei piese \n  top -> consulta unul din topuri(general sau pentru un anumit gen) \n  adauga -> adauga o piesa \n  com -> adauga comentariu \n  rvot -> restrictioneaza vot (doar pentru admin) \n  sterge -> sterge o piesa (doar pentru admin) \n  iesi -> iesi din aplicatie");
    write(sock , message , strlen(message));
    bzero(client_message,2000);
    
    while( (read_size = read(sock , client_message ,2000)) > 0 )
    {
         if(strcmp(client_message,"vezi")==0)
         {
                
              bzero(message,2000);
              strcat(message,"Introdu numele piesei: " );
              write(sock , message , strlen(message));
              bzero(client_message,2000);
              read_size = read(sock , client_message ,2000);
               
              for (int i=0;i<nr_piese;i++)
              {
                  if( strstr(p[i].nume,client_message)!=NULL)
                  {
                     bzero(detalii_piesa,2000);
                     strcat(detalii_piesa,"\n Titlu: ");
                     strcat(detalii_piesa,p[i].nume); 
                     strcat(detalii_piesa,"\n Gen: ");
                     strcat(detalii_piesa,p[i].gen);       
                     strcat(detalii_piesa,"\n Descriere:\n ");
                     strcat(detalii_piesa,p[i].descriere);
                     strcat(detalii_piesa,"\n Urmareste link-ul urmator pentru a asculta piesa: ");
                     strcat(detalii_piesa,p[i].link);
                     strcat(detalii_piesa,"\n Comenarii: \n");
                     struct nod* new_nod2 = (struct nod*)malloc(sizeof(struct nod));
                     new_nod2=p[i].comentarii;
                     while(new_nod2!= NULL)
                     {
                         strcat(detalii_piesa,"user:");
                         strcat(detalii_piesa,&new_nod2->user);
                         strcat(detalii_piesa,"\n");
                         strcat(detalii_piesa,&new_nod2->content);
                         strcat(detalii_piesa,"\n\n");
                         new_nod2=new_nod2->urm;
                      }
                     write(sock , detalii_piesa , strlen(detalii_piesa));
                     break;
                     
                   }
                   
               }

               bzero(client_message,2000); 
           }

           if(strcmp(client_message,"vot")==0)
           { 
               for (int i=0;i<nr_usr;i++)
               {
                  if( strcmp(activ_usr,users[i].user)==0)
                  {
                       if(users[i].acc==0)
                       {
                           bzero(message,2000);
                           strcat(message,"Ne pare rau dar nu mai ai dreptul sa votezi o piesa :(" );
                           write(sock , message , strlen(message));
                       }
                       
                       else
                       {
                           bzero(message,2000);
                           strcat(message,"Introdu numele piesei: " );
                           write(sock , message , strlen(message));
                           bzero(client_message,2000);
                           read_size = read(sock , client_message ,2000);
               
                           for (int i=0;i<nr_piese;i++)
                           {
                              if( strstr(p[i].nume,client_message)!=NULL)
                              {
                                   p[i].vot++;
                                   bzero(message,2000);
                                   strcat(message,"Felicitari! Tocmai ati votat piesa aleasa ^_^" );
                                   write(sock , message , strlen(message));
                                   break;
                               }

                           }//for

                        }//else

                    }//if_user

                }//for_user
                
                bzero(client_message,2000);

             }//if_vot
 
             if(strcmp(client_message,"top")==0)
             {
                 quickSort(p,0,nr_piese-1);
                 bzero(message,2000);
                 strcat(message,"\n Va rog alegeti topul pe care vreti sa il consultati: general/(anumit_gen) \n" );
                 write(sock , message , strlen(message));
                 bzero(client_message,2000);
                 read_size = read(sock , client_message ,2000);
                 bzero(message,2000);
                 strcat(message,"\n Acesta este topul ");
                 strcat(message,client_message);
                 strcat(message," ^_^:\n\n");
                 
                 char c[2];
                 int index;
                 index=1;
                 if(strcmp(client_message,"general")==0)
                 {
                      
                      for(int i=nr_piese-1;i>=0;i--)
                      {
                           if(index<10)
                           {
                               c[0]=index+'0';
                               c[1]='\0';
                           }
                           else 
                           {
                               c[0]=index/10+'0';
                               c[1]=index%10+'0';
                               c[2]='\0';
                           }
                           strcat(message,c);
                           strcat(message,". ");
                           index++;
                           strcat(message,p[i].nume);
                           strcat(message,"\n");
                      }

                      write(sock , message , strlen(message));
                  }
                  else
                  {
                      char c[2];
                      int index;
                      index=1;
                      for(int i=nr_piese-1;i>=0;i--)
                      {
                           if((strstr(p[i].gen,client_message)!=NULL))
                           {
                               if(index<10)
                               {
                                   c[0]=index+'0';
                                   c[1]='\0';
                               }
                               else 
                               {
                                   c[0]=index/10+'0';
                                   c[1]=index%10+'0';
                                   c[2]='\0';
                               }
                               strcat(message,c);
                               strcat(message,". ");
                               index++;
                               strcat(message,p[i].nume);
                               strcat(message,"\n");
                           }
                       }

                      write(sock , message , strlen(message));
                  }

                  bzero(client_message,2000);

             }//if_top
             
             if(strcmp(client_message,"adauga")==0)
             {
                  bzero(message,2000);
                  strcat(message,"Introduceti numele piesei: " );
                  write(sock , message , strlen(message));
                  bzero(client_message,2000);
                  read_size = read(sock , client_message ,2000);

                  strcpy(p[nr_piese].nume,client_message);

                  bzero(message,2000);
                  strcat(message,"Introduceti genul de care apartine piesa: " );
                  write(sock , message , strlen(message));
                  bzero(client_message,2000);
                  read_size = read(sock , client_message ,2000);

                  strcpy(p[nr_piese].gen,client_message);

                  bzero(message,2000);
                  strcat(message,"Introduceti o descriere pentru piesa: " );
                  write(sock , message , strlen(message));
                  bzero(client_message,2000);
                  read_size = read(sock , client_message ,2000);

                  strcpy(p[nr_piese].descriere,client_message);

                  bzero(message,2000);
                  strcat(message,"Introduceti link-ul catre piesa: " );
                  write(sock , message , strlen(message));
                  bzero(client_message,2000);
                  read_size = read(sock , client_message ,2000);

                  strcpy(p[nr_piese].link,client_message);

                  p[nr_piese].vot=0;
                  nr_piese ++;
                  
                  bzero(client_message,2000);
             }

             if(strcmp(client_message,"com")==0)
             {   
                  bzero(message,2000);
                  strcat(message,"Introduceti numele piesei: " );
                  write(sock , message , strlen(message));
                  bzero(client_message,2000);
                  read_size = read(sock , client_message ,2000);  

                  for (int i=0;i<nr_piese;i++)
                  {
                      if( strstr(p[i].nume,client_message)!=NULL)
                      {     

                          struct nod* new_nod = (struct nod*)malloc(sizeof(struct nod));
                          strcpy(new_nod->user,activ_usr);

                          bzero(message,2000);
                          strcat(message,"Introduceti comentariul: " );
                          write(sock , message , strlen(message));
                          bzero(client_message,2000);
                          read_size = read(sock , client_message ,2000);
                          
                          strcpy(new_nod->content,client_message);
                          new_nod->urm = p[i].comentarii;
                          p[i].comentarii = new_nod; 

                          bzero(message,2000);
                          strcat(message,"Comentariu adaugat cu succes! ^_^ " );
                          write(sock , message , strlen(message));
                          
                       }

                    }
                     
                   bzero(client_message,2000);
               }
 
               if(strcmp(client_message,"sterge")==0)
               {  
                   int de_sters;
                   if(strstr(activ_usr,"admin")==NULL)   
                   {
                         bzero(message,2000);
                         strcat(message,"Doar administaratorul poate sterge o piesa ^_^ " );
                         write(sock , message , strlen(message));  
                    }
                   
                    else
                    {
                          bzero(message,2000);
                          strcat(message,"Introduceti titlul piesei: " );
                          write(sock , message , strlen(message));
                          bzero(client_message,2000);
                          read_size = read(sock , client_message ,2000); 
 
                          for (int i=0;i<nr_piese;i++)
                          {
                                if( strstr(p[i].nume,client_message)!=NULL)
                                {  
                                    de_sters=i;
                                    break;
                                }
                          }

                          for (int i=de_sters;i<nr_piese;i++)
                          {
                                p[i].nume=p[i+1].nume;
                                p[i].vot=p[i+1].vot;
                                p[i].descriere=p[i+1].descriere;
                                p[i].link=p[i+1].link;
                                p[i].gen=p[i+1].gen;
                                p[i].comentarii=p[i+1].comentarii;
                          }
                          nr_piese--;
                          bzero(message,2000);
                          strcat(message,"Tocmai ati sters piesa " );
                          strcat(message,client_message);
                          write(sock , message , strlen(message));
                          
                       }
                       bzero(client_message,2000);
                  }
                  
                   if(strcmp(client_message,"rvot")==0)
                   {  
                       int de_sters;
                       if(strstr(activ_usr,"admin")==NULL)   
                       {
                            bzero(message,2000);
                            strcat(message,"Doar administaratorul poate restrictiona dreptul de vot al unui utilizator ^_^ " );
                            write(sock , message , strlen(message));  
                        }
                   
                        else
                        {
                            bzero(message,2000);
                            strcat(message,"Introduceti numele utilizatorului: " );
                            write(sock , message , strlen(message));
                            bzero(client_message,2000);
                            read_size = read(sock , client_message ,2000); 
 
                            for (int i=0;i<nr_usr;i++)
                            {
                                if( strcmp(users[i].user,client_message)==0)
                                {  
                                    users[i].acc=0;
                                    break;
                                }
                            }
                            bzero(message,2000);
                            strcat(message,"Tocmai ati restrictionat dreptul de vot al utilizatorului " );
                            strcat(message,client_message);
                            write(sock , message , strlen(message));  
 
                         }//else
                         
                        bzero(client_message,2000);
                    }

                    if(strcmp(client_message,"iesi")==0)
                    {
                         close(sock);
                    }  

                    if( strstr("top vezi vot com rvot sterge iesi ",client_message)==NULL)
                    {
                         bzero(message,2000);
                         strcat(message,"Comanda inexistenta ^_^" );
                         write(sock , message , strlen(message)); 
                    }

 }//while

     
    if(read_size == 0)
    {
        puts("Client disconnected");
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        perror("recv() failed");
    }
                    
      
    return 0;
} 
