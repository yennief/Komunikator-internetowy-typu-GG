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

#define QUEUE_SIZE 5
#define MAX 100
#define BUF 1024

pthread_mutex_t example_mutex = PTHREAD_MUTEX_INITIALIZER;

void register_user(int connection_socket_descriptor, char buf[MAX]);
int login(int connection_socket_descriptor, char buf[MAX]);

struct desc
{
    int fd;
};

typedef struct
{
 int fd;
 char username[MAX];
 char password[MAX];
 int if_logged_in;
 char messages[MAX][(4+MAX+BUF)*MAX];
 int friends_list[MAX];

}thread_data_t;

thread_data_t clients[MAX];

int users_number=0;

void add_friend(char *login_friend, int idx){

    int found;
    for (int i=0;i<users_number;i++){
        found = strcmp(clients[i].username,login_friend);
        if(found == 0 && clients[idx].friends_list[i]==0 && idx != i){

            clients[idx].friends_list[i]=1;
            clients[i].friends_list[idx]=1;
            printf("User %s added user %s\n",clients[idx].username, clients[i].username);
            printf("User %s automatically added user %s as well\n",clients[i].username,clients[idx].username);
            char buf[]="Added friend\n";
            write(clients[idx].fd ,buf, sizeof(buf));

            //Send a username to the added client of a friend who just added them, so they can be added to them automatically too
            char buf2[4+MAX]="a\t";
            strcat(buf2,clients[idx].username);
            strcat(buf2,"\n");
            write(clients[i].fd ,buf2, sizeof(buf2));
            return;
        }
        if(found == 0 && clients[idx].friends_list[i]==1 && idx != i){
            char buf[]="User is already a friend\n";
            write(clients[idx].fd ,buf, sizeof(buf));
            return;
        }
    }

    char buf[]="User does not exist\n";
    write(clients[idx].fd ,buf, sizeof(buf));

}

void load_conversation(char *username,int idx){
    int found;
    int length=0;
    for (int i=0;i<users_number;i++){
         found = strcmp(clients[i].username,username);

         if(found == 0 && clients[idx].friends_list[i]==1 && idx != i){
             char message[4+MAX+BUF]="";
             length = strlen(clients[idx].messages[i]);
             int k=0;
             for(int j=0;j<length;j++){
                 if (clients[idx].messages[i][j]=='\n'){
                     write(clients[idx].fd,message,strlen(message));
                     k=0;
                     memset(message,0,strlen(message));

                 }
                message[k]=clients[idx].messages[i][j];
                k++;
             }
             write(clients[idx].fd,"\n",1);
             write(clients[idx].fd,"stop\n",5);
         }
    }
}

void send_message(char* username,int idx,char received_msg[]){

    char message[4+MAX+BUF]="m\t";
    int found;
    for (int i=0;i<users_number;i++){
        found = strcmp(clients[i].username,username);
        if(found == 0 && clients[idx].friends_list[i]==1 && idx != i){
            strcat(message,clients[idx].username);
            strcat(message,":");
            strcat(message,"\t");
            strcat(message,received_msg);
            strcat(message,"\n");
            strcat(clients[i].messages[idx],message);
            strcat(clients[idx].messages[i],message);

            printf("%s sent a message to %s\n",clients[idx].username, clients[i].username);
            char buf[]="sent\n";
            write(clients[idx].fd ,buf, sizeof(buf));
            if(clients[i].if_logged_in==1){
                char buf2[4+MAX]="s\t";
                strcat(buf2,clients[idx].username);
                strcat(buf2,"\n");
                write(clients[i].fd ,buf2, sizeof(buf2));
            }

            return;
        }
    }
    char buf[]="User does not exist\n";
    write(clients[idx].fd ,buf, sizeof(buf));

}

void delete_friend(int idx, char *username){
    int found;
    for (int i=0;i<users_number;i++){
        found = strcmp(clients[i].username,username);
        if(found == 0 && idx != i && clients[idx].friends_list[i]==1){

            clients[idx].friends_list[i]=0;
            clients[i].friends_list[idx]=0;
            printf("User %s deleted user %s\n",clients[idx].username, clients[i].username);
            printf("User %s automatically deleted user %s as well\n",clients[i].username,clients[idx].username);
            char buf[]="Deleted friend\n";
            write(clients[idx].fd ,buf, sizeof(buf));
            char buf2[4+MAX]="d\t";
            strcat(buf2,clients[idx].username);
            strcat(buf2,"\n");
            write(clients[i].fd ,buf2, sizeof(buf2));
            return;
        }
    }

    char buf[]="User does not exist\n";
    write(clients[idx].fd ,buf, sizeof(buf));

}

void *ThreadBehavior(void *t_data)
{

    struct desc* data = (struct desc*)t_data;

    int cur_idx = -1;
    char option[MAX];

    memset(option, 0, sizeof(option));
    read(data->fd,option,sizeof(option));

   if(option[0] == 'R'){

        if((users_number) == MAX){

                write(data->fd, "Too many users\n", 14* sizeof(char));
                close(data->fd);
       }
       else{
            register_user(data->fd, option);
       }

   }
   else if(option[0]=='L'){
    cur_idx = login(data->fd, option);

  }

  if(cur_idx == -1){
         close(data->fd);
         return NULL;
    }

    while(1){

        char buf[4+MAX+BUF]="";
        read(clients[cur_idx].fd, buf, sizeof(buf));
        char username[MAX]="";
        char message[BUF]="";
        int login_size=0;
        for(int i=2;i<MAX+2 && buf[i]!='\t';i++){
            username[i-2]=buf[i];
            login_size=i;
        }

        if(buf[0]=='A'){
           pthread_mutex_lock(&example_mutex);
           add_friend(username,cur_idx);
           pthread_mutex_unlock(&example_mutex);

        }
        else if(buf[0]=='l'){

            pthread_mutex_lock(&example_mutex);
            load_conversation(username, cur_idx);
            pthread_mutex_unlock(&example_mutex);

        }
        else if(buf[0]=='M'){

            for(int i=2+login_size;i<2+login_size+BUF && buf[i]!='\t';i++){
                  message[i-2-login_size]=buf[i];

            }

            pthread_mutex_lock(&example_mutex);
            send_message(username,cur_idx,message);
            pthread_mutex_unlock(&example_mutex);

        }
        else if(buf[0]=='Q'){
            pthread_mutex_lock(&example_mutex);
            clients[cur_idx].if_logged_in=0;
            printf("%s logged out\n",username);
            pthread_mutex_unlock(&example_mutex);
        }
        else if(buf[0]=='D'){
            pthread_mutex_lock(&example_mutex);
            delete_friend(cur_idx,username);
            pthread_mutex_unlock(&example_mutex);
        }
    }

    pthread_exit(NULL);
}

void register_user(int connection_socket_descriptor, char buf[MAX]){

   char username[MAX]="";
   char password[MAX]="";
   int found = 1;
   int log_size=0;

   for(int i=2;i<MAX && buf[i]!='\t';i++){
              username[i-2]=buf[i];
              log_size=i;
       }

   for (int i=0;i<MAX;i++){

            found = strcmp(clients[i].username,username);
            if(found==0){
                break;
            }
    }

   if(found==0){
            char buf[]="User already exists\n";
            write(connection_socket_descriptor, buf, sizeof(buf));
            close(connection_socket_descriptor);
    }
    else{

         for(int i=log_size+2;i<MAX && buf[i]!='\t';i++){
               password[i-log_size-2]=buf[i];
         }

        strcpy(clients[users_number].username,username);
        strcpy(clients[users_number].password,password);

        users_number+=1;

        char buf[]="User registered\n";
        printf("User %s has registered\n", clients[users_number-1].username);
        write(connection_socket_descriptor, buf, sizeof(buf));

    }

}

int load_friends_list(int idx, char* list){

    for (int i=0; i<MAX;i++){
        if(clients[idx].friends_list[i]==1){
            strcat(list,clients[i].username);
            strcat(list,"\t");
        }
    }
    int size = strlen(list);
    list = realloc(list,size+1);
    strcat(list,"\n");

    return size;

}

int login(int connection_socket_descriptor, char buf[MAX]){

   char username[MAX]="";
   char password[MAX]="";
   int correct_psswd=1;
   int idx;
   int found = 1;
   int log_size=0;

   for(int i=2;i<MAX && buf[i]!='\t';i++){
           username[i-2]=buf[i];
           log_size=i;
       }

   for (int i=0;i<MAX;i++){
        found = strcmp(clients[i].username,username);
        if(found==0){
             idx=i;
             break;
        }
    }

    if(found!=0){

        char buf[]="User not found\n";
        write(connection_socket_descriptor, buf, sizeof(buf));
        close(connection_socket_descriptor);
        return -1;
    }
    else{
        for(int i=log_size+2;i<MAX && buf[i]!='\t';i++){
            password[i-log_size-2]=buf[i];

        }
        correct_psswd = strcmp(password,clients[idx].password);
        if(correct_psswd!=0){
            char buf[]="Wrong password\n";
            write(connection_socket_descriptor, buf, sizeof(buf));
            close(connection_socket_descriptor);
            return -1;
        }
        else{
            if(clients[idx].if_logged_in == 1){
                char buf[]="User is already logged in!\n";
                write(connection_socket_descriptor, buf, sizeof(buf));
                close(connection_socket_descriptor);
                return -1;
            }
            else{

                int size=0;
                char *list = malloc(MAX*(MAX+2)*sizeof(char));
                size = load_friends_list(idx,list);

                char buf[]="Login successful\n";
                printf("User %s has logged in\n", clients[idx].username);
                write(connection_socket_descriptor, buf, sizeof(buf));

                if(size!=0){
                    char buf3[]="full\n";
                    write(connection_socket_descriptor, buf3, sizeof(buf3));
                    char list2[size+1];
                    strcpy(list2, list);
                    strcat(list2, "\n");

                    write(connection_socket_descriptor, list2, sizeof(list2));
                }
                else{
                    char buf3[]="empty\n";
                    write(connection_socket_descriptor, buf3, sizeof(buf3));
                }
                clients[idx].fd = connection_socket_descriptor;
                clients[idx].if_logged_in = 1;

            }
            return idx;
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

   pthread_t thread;
   int create_result = 0;

   if (argc != 2)
      {
        fprintf(stderr, "Sposób użycia: %s port_number\n", argv[0]);
        exit(1);
      }

   memset(&server_address, 0, sizeof(struct sockaddr));
   server_address.sin_family = AF_INET;
   server_address.sin_addr.s_addr = htonl(INADDR_ANY);
   server_address.sin_port = htons(atoi(argv[1]));

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

   socklen_t clilen = sizeof(cli_addr);

   while(1)
   {

       connection_socket_descriptor = accept(server_socket_descriptor, (struct sockaddr*)&cli_addr, &clilen);
       if (connection_socket_descriptor < 0)
       {
           fprintf(stderr, "%s: Błąd przy próbie utworzenia gniazda dla połączenia.\n", argv[0]);
           exit(1);
       }

       struct desc *t_data0 = (struct desc*)malloc(sizeof(struct desc));
       t_data0->fd = connection_socket_descriptor;

       create_result = pthread_create(&thread, NULL, ThreadBehavior, (void *)t_data0);

       if (create_result){
           printf("Błąd przy próbie utworzenia wątku, kod błędu: %d\n", create_result);
           exit(-1);
       }
       pthread_detach(thread);

   }

   close(server_socket_descriptor);
   return(0);
}