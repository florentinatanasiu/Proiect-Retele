#include <stdio.h>
#include <string.h>    
#include <stdlib.h>    
#include <unistd.h>    
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>




struct nod {
             struct nod* urm;
             char user[100];
             char content[200];
};

struct piesa {
               int vot;
               char *nume;
               char *descriere;
               char *gen;
               char *link;
               struct nod* comentarii;
};

struct dr_vot {
                char user[100];
                int acc;
};
                    
             
struct dr_vot users[1000];
struct piesa p[300];
int nr_piese, nr_usr;
      

int vot_right(struct dr_vot users[1000])
{
    FILE * f1;
    size_t len=0;
    ssize_t read;

    f1 = fopen("usernames.txt", "r");

    if(f1 == NULL)
    {
       perror("can't open the usernames.txt");
       exit(EXIT_FAILURE);
    }

    nr_usr=0;
    while(!feof(f1))
    {
        if( fscanf(f1,"%s",users[nr_usr].user)==0)
        {
            perror("can't read from usernames.txt");
            exit(EXIT_FAILURE);
        }
        users[nr_usr].acc=1;
        nr_usr++; 
    }
    fclose(f1);
          
    return nr_usr;
}

int populate_songList(struct piesa p[300])
{
    FILE * f1;
    FILE * f2;
    FILE * f3;
    FILE * f4;
    int nr_piese=0;
    size_t len = 0;
    ssize_t read;

    f1 = fopen("piese.txt", "r");
    f2 = fopen("descrieri.txt", "r");
    f3 = fopen("gen.txt", "r");
    f4 = fopen("link.txt","r");
 

    if(f1 == NULL)
    {
       perror("can't open the piese.txt");
       exit(EXIT_FAILURE);
    }

    if(f2 == NULL)
    {
       perror("can't open the descrieri.txt");
       exit(EXIT_FAILURE);
    }

    if(f3 == NULL)
    {
       perror("can't open the gen.txt");
       exit(EXIT_FAILURE);
    }

    if(f4 == NULL)
    {
       perror("can't open the link.txt");
       exit(EXIT_FAILURE);
    }


    printf("\n populate the song list ..\n");

     while ((read = getline(&p[nr_piese].nume, &len, f1)) != -1) 
    {
           
           nr_piese++;
    }
    
    int i=0; 
    while ((read = getline(&p[i].descriere, &len, f2)) != -1) 
    {
           
           i++;
    }
    i=0; 
    while ((read = getline(&p[i].gen, &len, f3)) != -1) 
    {
           
           i++;
    }
    
    i=0; 
    while ((read = getline(&p[i].link, &len, f4)) != -1) 
    {
           
           i++;
    }
    
   for(int i=0;i<nr_piese;i++)
   {
       p[i].vot=0; 
       p[i].comentarii= NULL;
   }
 
   return nr_piese;
   
}

int partition (struct piesa p[300], int start, int stop)
{
    int pivot = p[stop].vot;    
    int i = (start - 1);  
 
    for (int j = start; j <= stop- 1; j++)
    {
        if (p[j].vot <= pivot)
        {
            i++;    
            struct piesa x;
            x.nume=p[i].nume;
            x.vot=p[i].vot;
            x.descriere=p[i].descriere;
            x.link=p[i].link;
            x.gen=p[i].gen;

            p[i].nume=p[j].nume;
            p[i].vot=p[j].vot;
            p[i].descriere=p[j].descriere;
            p[i].link=p[j].link;
            p[i].gen=p[j].gen;
         
            p[j].nume=x.nume;
            p[j].vot=x.vot;
            p[j].descriere=x.descriere;
            p[j].link=x.link;
            p[j].gen=x.gen;

        }
    }
    struct piesa x;
    x.nume=p[i+1].nume;
    x.vot=p[i+1].vot;
    x.descriere=p[i+1].descriere;
    x.link=p[i+1].link;
    x.gen=p[i+1].gen;

    p[i+1].nume=p[stop].nume;
    p[i+1].vot=p[stop].vot;
    p[i+1].descriere=p[stop].descriere;
    p[i+1].link=p[stop].link;
    p[i+1].gen=p[stop].gen;
         
    p[stop].nume=x.nume;
    p[stop].vot=x.vot;
    p[stop].descriere=x.descriere;
    p[stop].link=x.link;
    p[stop].gen=x.gen;
    return (i + 1);
}
 
void quickSort(struct piesa p[300], int start, int stop)
{
    if (start < stop)
    {
        int nr = partition(p, start, stop);

        quickSort(p, start, nr - 1);
        quickSort(p, nr + 1, stop);
    }
}
           

int create_newCont( char ans[100])
{
     FILE * user;  
     user = fopen("usernames.txt", "r+");
     char check[100];
     int repeat=0;
         
     if(user == NULL)
     {
          perror("can't open the usernames.txt");
          exit(EXIT_FAILURE);
     }
     while(!feof(user))
     {
           if( fscanf(user,"%s",check)== 0)
           {
              perror("can't read from usernames.txt");
              exit(EXIT_FAILURE);
           }

           if(strcmp(ans,check)==0)
           {
              repeat=1;
           }
     }
         
     if(repeat==0)
     {
          fprintf(user, "%s\n", ans);
          fclose(user);
          printf("\n new cont created.\n");
         
     }
        
     return repeat;
}

int login(char ans[100])
{
    char check[100];
    FILE * user;
    int access=0;

    user = fopen("usernames.txt", "r");

    if(user == NULL)
    {
       perror("can't open the usernames.txt");
       exit(EXIT_FAILURE);
    }
    printf("\n one more client try to login..procesing..\n");
    sleep(1);
    printf("\n searching in usernames.txt.. \n");

    while(!feof(user))
    {
           if( fscanf(user,"%s",check)==0)
           {
              perror("can't read from usernames.txt");
              exit(EXIT_FAILURE);
           }

           if(strcmp(ans,check)==0)
           {
              
              access=1;
             
           }
    }

    fclose(user);

    if(access==1)
    printf("\n...success..connected..\n");  

    return access;
}


 
