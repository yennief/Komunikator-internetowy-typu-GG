#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <cstring>

#define SERVER_PORT 1234
#define QUEUE_SIZE 5
#define MAX 100

int count=0;
int flg=0;

//struktura zawierająca dane, które zostaną przekazane do wątku
typedef struct
{
 int fd;
 int id;
 int status;
 char username[MAX];
 char password[MAX];
}thread_data_t;

thread_data_t clients[MAX];

int users_number=0;


pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;


/*void queue_add(thread_data_t *cl){
  pthread_mutex_lock(&clients_mutex);

  for(int i=0; i < MAX; ++i){
    if(!clients[i]){
      clients[i] = cl;
      break;
    }
  }

  pthread_mutex_unlock(&clients_mutex);
}

void queue_remove(int id){
  pthread_mutex_lock(&clients_mutex);

  for(int i=0; i < MAX; ++i){
    if(clients[i]){
      if(clients[i]->id == id){
        clients[i] = NULL;
        break;
      }
    }
  }

  pthread_mutex_unlock(&clients_mutex);
}

void remove_all(){
  pthread_mutex_lock(&clients_mutex);

  for(int i=0; i < MAX; ++i){
    if(clients[i]){
        clients[i] = NULL;
      }
    }
  pthread_mutex_unlock(&clients_mutex);
}

void show_queue(){

  char buf[100];

  for(int i=0; i < MAX; ++i){
    if(clients[i]){
      sprintf(buf, "%s, ", clients[i]->username);
      //printf("%d, ", clients[i]->id);
      write(1,buf,strlen(buf));
    }

    }
    sprintf(buf, "\n");
    write(1,buf,strlen(buf));

}


void send_message(char *s, int id){
  pthread_mutex_lock(&clients_mutex);

  for(int i=0; i<MAX; ++i){
    if(clients[i]){
      if(clients[i]->id != id){
        if(write(clients[i]->fd, s, strlen(s)) < 0){
          perror("ERROR: write to descriptor failed");
          break;
        }
      }
    }
  }

  pthread_mutex_unlock(&clients_mutex);
}

void send_to_all(char *s){

    pthread_mutex_lock(&clients_mutex);

    for(int i=0; i<MAX; ++i){
        if(clients[i]){
           if(write(clients[i]->fd, s, strlen(s)) < 0){
             perror("ERROR: write to descriptor failed");
             break;
           }
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}*/


//funkcja opisującą zachowanie wątku - musi przyjmować argument typu (void *) i zwracać (void *)
void *ThreadBehavior(void *t_data)
{

    thread_data_t *th_data = (thread_data_t*)t_data;



    count++;
    int flag=0;

    char buf[100];
    char buf2[100];
    while(1){
        memset(buf, 0, sizeof(buf));
        for(int i=0;i<sizeof(buf);i++){buf[i]='\0';}
        read((*th_data).fd,buf,sizeof(buf));
        //write(1,buf,strlen(buf));

        if(strcmp(buf,"quit\n")==0){
          flag=1;
          sprintf(buf2, "Usuwanie klienta\n");
          write(1,buf2,strlen(buf2));
          memset(buf2, 0, sizeof(buf2));

        }
        /*if(flag==0){
          send_message(buf,(*th_data).id);
        }
        if(flag){
          break;
        }*/

        //write(1,buf,strlen(buf));
    }
    //dostęp do pól struktury: (*th_data).pole
    //TODO (przy zadaniu 1) klawiatura -> wysyłanie albo odbieranie -> wyświetlanie
    close((*th_data).fd);
    //queue_remove((*th_data).id);
    free(th_data);
    count--;

    pthread_detach(pthread_self());
    pthread_exit(NULL);
}

/*
void *if_exit(void *t_data)
{

    thread_data_t *th_data = (thread_data_t*)t_data;
    char buf[100];
    while(1){
        memset(buf, 0, sizeof(buf));
        for(int i=0;i<sizeof(buf);i++){buf[i]='\0';}
        if(strcmp(fgets(buf,sizeof(buf),stdin),"quit\n")==0){

        //send_to_all(buf);
        //remove_all();
        exit(1);

        }
    }
    //dostęp do pól struktury: (*th_data).pole
    //TODO (przy zadaniu 1) klawiatura -> wysyłanie albo odbieranie -> wyświetlanie
    close((*th_data).fd);
    //queue_remove((*th_data).id);
    free(th_data);


    pthread_detach(pthread_self());
    pthread_exit(NULL);
}*/


int check(char* username){
    int flag=1;

    for(int i=0; i<MAX; ++i){

          if(strcmp(clients[i].username,username)==0){
            flag=0;
            break;

          }

      }
    return flag;

}

/*int check_psswd(char* password,char* username){
    int flag=0;

        for(int i=0; i<MAX; ++i){
            if(clients[i]){
              if(strcmp(clients[i].username,username)==0){
                if(strcmp(clients[i].password,password)==0){
                    flag=1;
                    break;

                }

              }
            }
          }
        return flag;

}*/

void register_user(int connection_socket_descriptor, char buf[MAX]){

   char username[MAX]="";
   char password[MAX]="";

   int idx;
   int found = 1;
   int log_size=0;

   for(int i=2;i<MAX && buf[i]!='\t';i++){
              username[i-2]=buf[i];
              log_size=i;
       }

   printf("Username: %s\n", username);

   for (int i=0;i<MAX;i++){

            found = strcmp(clients[i].username,username);
            if(found==0){
                idx=i;
                break;
            }


    }


   if(found==0){
            char buf[]="User already exists\n";
            printf("User already exists\n");
            write(connection_socket_descriptor, buf, sizeof(buf));
            close(connection_socket_descriptor);

    }
    else{


         //printf("Check reg\n");

         for(int i=log_size+2;i<MAX && buf[i]!='\t';i++){
               password[i-log_size-2]=buf[i];
         }

         printf("Added password: %s\n", password);


        strcpy(clients[users_number].username,username);
        strcpy(clients[users_number].password,password);
        clients[users_number].id = users_number;
        users_number+=1;

        //printf("Added username: %s\n", clients[users_number-1].username);


        //printf("Check reg2\n");

        char buf[]="User registered\n";
        write(connection_socket_descriptor, buf, sizeof(buf));
        //clients[users_registered]->fd = connection_socket_descriptor;

        //queue_add(client);

        printf("User registered - %s\n",username);



    }

}

int login(int connection_socket_descriptor, char buf[MAX]){

   char username[MAX]="";
   char password[MAX]="";

   int correct_psswd=0;
   int idx;
   int found = 1;
   int log_size=0;

   for(int i=2;i<MAX && buf[i]!='\t';i++){
           username[i-2]=buf[i];
           log_size=i;
       }

   //printf("Username: %s\n", username);


   for (int i=0;i<MAX;i++){

        found = strcmp(clients[i].username,username);
        if(found==0){
            //printf("JEST ZNALEZIONY!!!");

             idx=i;
             break;
        }
    }

    //printf("FOUND STATUS: %d\n", found);

    if(found!=0){
        //printf("Check2\n");
        char buf[]="User not found\n";
        printf("User not found\n");
        write(connection_socket_descriptor, buf, sizeof(buf));
        close(connection_socket_descriptor);
        return -1;
    }
    else{
        for(int i=log_size+2;i<MAX && buf[i]!='\t';i++){
            password[i-log_size-2]=buf[i];

        }
        int pass_correct = strcmp(password,clients[idx].password);
        if(pass_correct!=0){
            char buf[]="Wrong password\n";
            int readOutput = write(connection_socket_descriptor, buf, sizeof(buf));
            close(connection_socket_descriptor);
            return -1;

        }
        else{
            clients[idx].fd = connection_socket_descriptor;
            clients[idx].status = 1;
            printf("Login successful\n");
            char buf[]="Login successful\n";
            write(connection_socket_descriptor, buf, sizeof(buf));
            return -1; //ZMIENIC NA 1 POTEM


        }

    }

   return 1;

}


int main(int argc, char* argv[])
{
   int server_socket_descriptor;
   int connection_socket_descriptor;
   int bind_result;
   int listen_result;
   char reuse_addr_val = 1;
   struct sockaddr_in server_address;
   struct sockaddr_in cli_addr;

   pthread_t thread, thread2;
   int create_result = 0;
   int create_result2 = 0;

   //inicjalizacja gniazda serwera

   memset(&server_address, 0, sizeof(struct sockaddr));
   server_address.sin_family = AF_INET;
   server_address.sin_addr.s_addr = htonl(INADDR_ANY);
   server_address.sin_port = htons(SERVER_PORT);

   server_socket_descriptor = socket(AF_INET, SOCK_STREAM, 0);
   if (server_socket_descriptor < 0)
   {
       fprintf(stderr, "%s: Błąd przy próbie utworzenia gniazda..\n", argv[0]);
       exit(1);
   }
   setsockopt(server_socket_descriptor, SOL_SOCKET, SO_REUSEADDR, (char*)&reuse_addr_val, sizeof(reuse_addr_val));

   bind_result = bind(server_socket_descriptor, (struct sockaddr*)&server_address, sizeof(struct sockaddr));
   if (bind_result < 0)
   {
       fprintf(stderr, "%s: Błąd przy próbie dowiązania adresu IP i numeru portu do gniazda.\n", argv[0]);
       exit(1);
   }

   listen_result = listen(server_socket_descriptor, QUEUE_SIZE);
   if (listen_result < 0) {
       fprintf(stderr, "%s: Błąd przy próbie ustawienia wielkości kolejki.\n", argv[0]);
       exit(1);
   }
   char buf[100];


   socklen_t clilen = sizeof(cli_addr);
   //printf("Check\n");

   while(1)
   {


       connection_socket_descriptor = accept(server_socket_descriptor, (struct sockaddr*)&cli_addr, &clilen);
       if (connection_socket_descriptor < 0)
       {
           fprintf(stderr, "%s: Błąd przy próbie utworzenia gniazda dla połączenia.\n", argv[0]);
           exit(1);
       }

       if((users_number) == MAX){

        write(connection_socket_descriptor, "Too many users\n", 14* sizeof(char));
        close(connection_socket_descriptor);


       }
       else{

           char option[MAX];

           memset(option, 0, sizeof(option));
           //for(int i=0;i<sizeof(option);i++){option[i]='\0';}
           read(connection_socket_descriptor,option,sizeof(option));
           flg=0;

           if(option[0] == 'R'){

               register_user(connection_socket_descriptor, option);

           }

           else if(option[0]=='L'){
            int login_status;
            //printf("Check\n");

            login_status = login(connection_socket_descriptor, option);
            if(login_status != -1){


                thread_data_t *client = (thread_data_t*)malloc(sizeof(thread_data_t));


                create_result = pthread_create(&thread, NULL, ThreadBehavior, (void *)client);
                if (create_result){
                    printf("Błąd przy próbie utworzenia wątku, kod błędu: %d\n", create_result);
                    exit(-1);
                }

            }


           }

       }

   }

   close(server_socket_descriptor);
   return(0);
}