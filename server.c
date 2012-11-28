#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <sysexits.h>
#include <ctype.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

typedef unsigned char BYTE;
#define MAXBUFFERLEN 500
pthread_mutex_t mutexping;
pthread_t pingthread;
pthread_t testthread;
pthread_attr_t attr;
typedef unsigned long size_t;

struct sockaddr_in *sain;
struct hostent *he;
struct in_addr a;
char hostname[128];
int debug = 0;
char *portg;
pthread_cond_t ping_signal;
pthread_cond_t swap_signal;
pthread_cond_t hash_signal;
pthread_cond_t check_signal;
pthread_cond_t stat_signal;

struct stat
{
     int ping;
     int swap;
     int hash;
     int check;
     int stat;
};

struct stat st;

void usage(char *program_name) {
	printf("Usage: %s [-d] -p <port>\n", program_name);
}

void sendACK(int *sd2)
{
            BYTE ack = (BYTE)'1';
            int sent = -1;
            while(sent == -1)
            {
                sent = send(*sd2, &ack,sizeof(BYTE) ,0);
            }
}

int hash(char *str)
    {
        int hash = 0;
        int c;

        while (c = *str++)
            hash = (c + hash) % 1000000000; /* hash * 33 + c */

        return hash;
    }

void pingRequest(void *ptr){



        pthread_mutex_lock(&mutexping);
        int *sd2 = ptr;
        st.ping++;
        //int id = st.ping; FOR CONCURRENCY TEST
        pthread_mutex_unlock(&mutexping);



        int received;

        char buffer[MAXBUFFERLEN + 1];


            //////////////////////////////////////////////////////////////////////



            /////// Receive Ip  //////////////////////////////////////////
            received = -1;
            while (received == -1)
            {

               received = recv(*sd2, buffer, MAXBUFFERLEN, 0);
            }
            

            if(buffer[received - 1] != '\n')
            {
                buffer[received] = '\0';
            }
            else
            {
                buffer[received - 1] = '\0';
            }

            printf("s> %s\n", buffer);

        /////////////////////////////////////////////////////////////////////


            received = -1;

            BYTE *a  = (BYTE *)"1";

            int sent = -1;
         //////////////////// Send Acknowledgement //////////////////////////
            while(sent == -1)
            {
                sent = send(*sd2,a,sizeof(a),0);
            }
         ////////////////////////////////////////////////////////////////////

            //sleep(5);                 CONCURRENCY TEST
            //printf("Done %d\n",id);
            close(*sd2);

            pthread_exit(NULL);

}

void swapRequest(void *ptr){

            char buffer[MAXBUFFERLEN + 1];

            pthread_mutex_lock(&mutexping);
            int *sd2 = ptr;
            st.swap++;
            //int id = st.swap; //FOR CONCURRENCY TEST
            pthread_mutex_unlock(&mutexping);

            sendACK(sd2);

            /////// Receive IP //////////////////////////////////////////
            int received = -1;

            char ip[MAXBUFFERLEN + 1];
            //printf("Waiting for ip\n");
            while (received == -1)
            {
               received = recv(*sd2, ip, MAXBUFFERLEN, 0);
            }
            ip[received] = '\0';
            printf("Received %s\n",ip);
            //sleep(10);
            
            //////////////////////////////////////////////////////////////

            //sendACK(sd2);

            /////////// Receive File Size ////////////////////////////
            int file_size;
            received = -1;
            printf("Size of long %ld\n", sizeof(int));
            while(received == -1)
            {
                received = recv(*sd2, &file_size,sizeof(int) ,0);
            }
            u_int aux = htonl(file_size);

            printf("s> %s init swap %d\n", ip,file_size);
            printf("File size is: %d\n",aux);
            /////////////////////////////////////////////////////////
            sendACK(sd2);
            sleep(223);
            
            int converted = 0;

            while(file_size > 0)
            {

                file_size -= 500;
                received = -1;
                ///// RECEIVE SWAP CHAIN ///////////////////////////////////
                while (received == -1)
                {
                   received = recv(*sd2, buffer, MAXBUFFERLEN, 0);
                }
                buffer[received + 1] = '\0';
                ///////////////////////////////////////////////////////
                printf("File size %d\nReceived chain: %s\n", file_size, buffer);
            /////////////// CONVERT TO UPPERCASE /////////////////////////
                char aux;
                char c = 'g';
                int i = 0;
                char conv_s[MAXBUFFERLEN + 1];
                while(c != '\0')
                {
                    int changed = 0;
                    c = buffer[i];
                    aux = toupper(c);
                    if(aux != c) changed = 1;
                    if(changed == 0)
                    {
                        aux = tolower(c);
                        if(aux != c)changed = 1;
                    }
                    if(changed == 1)converted++;
                    conv_s[i] = aux;
                    i++;

                }
            /////////////////////////////////////////////////////////////////////

                int sent = -1;
             //////////////////// Send number of letters changed //////////////////////////
                while(sent == -1)
                {
                    sent = send(*sd2,&converted,sizeof(converted),0);
                }
             ////////////////////////////////////////////////////////////////////
                //printf("Converted to %s\n",conv_s);
                //////////////////// Send uppercase chain ////////////////////////
                sent = -1;
                while(sent == -1)
                {
                    sent = send(*sd2,conv_s,MAXBUFFERLEN,0);
                }
                ///////////////////////////////////////////////////////////////
            }
            printf("s> %s swap = %d\n",ip,converted);
            //sleep(20);   //              CONCURRENCY TEST
            //printf("Done %d\n",id);

            close(*sd2);


            pthread_exit(NULL);

}

void hashRequest(void *ptr){

            char buffer[MAXBUFFERLEN + 1];

            pthread_mutex_lock(&mutexping);
            int *sd2 = ptr;
            st.hash++;
            //int id = st.hash; //FOR CONCURRENCY TEST
            pthread_mutex_unlock(&mutexping);


            /////// Receive IP //////////////////////////////////////////
            int received = -1;

            char ip[MAXBUFFERLEN + 1];
            while (received == -1)
            {
               received = recv(*sd2, ip, MAXBUFFERLEN, 0);
            }
            ip[received] = '\0';

            received = -1;
            //////////////////////////////////////////////////////////////

            int file_size;
            while(received == -1)
            {
                received = recv(*sd2, &file_size,sizeof(int),0);
            }

            printf("s> %s init hash %d\n", ip, file_size);
            //printf("File size is: %d\n",file_size);
            char *result = (char *)malloc(sizeof(char) * 500);
            while(file_size > 0)
            {

                file_size -= 500;
                received = -1;
                ///// RECEIVE HASH CHAIN ///////////////////////////////////
                while (received == -1)
                {
                   received = recv(*sd2, buffer, MAXBUFFERLEN, 0);
                }
                buffer[received + 1] = '\0';
                ///////////////////////////////////////////////////////
                //printf("File size %d\nReceived chain: %s\n", file_size, buffer);

             /////////////// CREATE HASH STRING /////////////////////////
                int converted = hash(buffer);
                //printf("Hash %s=%d\n",buffer,converted);
                sprintf(result,"%s%d",result,converted);
            /////////////////////////////////////////////////////////////////////

            }
            int servhash = atoi(result);
            printf("s> %s hash = %d\n",ip,servhash);
            int sent = -1;
            while(sent == -1)
            {
                sent = send(*sd2,&servhash,sizeof(int),0);
            }
            //Tell main that another swap service may be attended
            pthread_cond_signal(&hash_signal);


            //sleep(20);   //              CONCURRENCY TEST
            //printf("Done %d\n",id);

            close(*sd2);
            pthread_exit(NULL);

}

void checkRequest(void *ptr){

            char buffer[MAXBUFFERLEN + 1];

            pthread_mutex_lock(&mutexping);
            int *sd2 = ptr;
            st.check++;
            //int id = st.check; //FOR CONCURRENCY TEST
            pthread_mutex_unlock(&mutexping);


            /////// Receive IP //////////////////////////////////////////
            int received = -1;

            char ip[MAXBUFFERLEN + 1];
            while (received == -1)
            {
               received = recv(*sd2, ip, MAXBUFFERLEN, 0);
            }
            ip[received] = '\0';

            received = -1;
            //////////////////////////////////////////////////////////////

            int file_size;
            while(received == -1)
            {
                received = recv(*sd2, &file_size,sizeof(int),0);
            }
            int auxfs = file_size;
            //printf("File size is: %d\n",file_size);
            char *result = (char *)malloc(sizeof(char) * 500);
            while(file_size > 0)
            {

                file_size -= 500;
                received = -1;
                ///// RECEIVE HASH CHAIN ///////////////////////////////////
                while (received == -1)
                {
                   received = recv(*sd2, buffer, MAXBUFFERLEN, 0);
                }
                buffer[received + 1] = '\0';
                ///////////////////////////////////////////////////////
                //printf("File size %d\nReceived chain: %s\n", file_size, buffer);

             /////////////// CREATE HASH STRING /////////////////////////
                int converted = hash(buffer);
                //printf("Hash %s=%d\n",buffer,converted);
                sprintf(result,"%s%d",result,converted);
            /////////////////////////////////////////////////////////////////////

            }
            int servhash = atoi(result);
            //printf("Hash: %d\n",servhash);
            int receive = -1;
            int cliehash = 0;
            while(receive == -1)
            {
                receive = recv(*sd2,&cliehash,sizeof(int),0);
            }
            char *res = (char *) malloc(sizeof(char)*5);
            //printf("Cliehash: %d\n", cliehash);

            if(cliehash == servhash) res = "OK";
            if(cliehash != servhash) res = "FAIL";

            printf("s> %s init check %d %d\n", ip,auxfs,cliehash);

            printf("s> %s check = %s\n",ip, res);

            int sent = -1;
            while(sent == -1)
            {
                sent = send(*sd2,res,sizeof(res),0);
            }


            //sleep(20);   //              CONCURRENCY TEST
            //printf("Done %d\n",id);

            close(*sd2);

            pthread_exit(NULL);
}

void statRequest(void *ptr){

            pthread_mutex_lock(&mutexping);
            int *sd2 = ptr;
            st.stat++;
            //int id = st.stat; //FOR CONCURRENCY TEST
            pthread_mutex_unlock(&mutexping);


            /////// Receive IP //////////////////////////////////////////
            int received = -1;

            char ip[MAXBUFFERLEN + 1];

            while (received == -1)
            {
               received = recv(*sd2, ip, MAXBUFFERLEN, 0);
            }

            if(ip[received - 1] == '\n')
            {
                ip[received - 1] = '\0';
            }
            else
            {
                ip[received] = '\0';
            }
            printf("s> %s init stat\n", ip);
            received = -1;
            //////////////////////////////////////////////////////////////

           int sent = -1;
         //////////////////// Send ping  /////////////////////////////
           uint16_t sendshort = htons(st.ping);
            while(sent == -1)
            {
                sent = send(*sd2,&sendshort,sizeof(uint16_t),0);
            }
         ////////////////////////////////////////////////////////////////////

           sent = -1;
            sendshort = htons(st.swap);
         //////////////////// Send swap  /////////////////////////////
            //sleep(1);
            while(sent == -1)
            {
                sent = send(*sd2,&sendshort,sizeof(uint16_t),0);
            }

         ////////////////////////////////////////////////////////////////////

          sent = -1;
          sendshort = htons(st.hash);
         //////////////////// Send hash string  /////////////////////////////
            while(sent == -1)
            {
                sent = send(*sd2,&sendshort,sizeof(uint16_t),0);
            }
         ////////////////////////////////////////////////////////////////////

          sent = -1;
          sendshort = htons(st.check);
         //////////////////// Send hash string  /////////////////////////////
            while(sent == -1)
            {
                sent = send(*sd2,&sendshort,sizeof(uint16_t),0);
            }
         ////////////////////////////////////////////////////////////////////

          sent = -1;
          sendshort = htons(st.stat);
         //////////////////// Send hash string  /////////////////////////////
            while(sent == -1)
            {
                sent = send(*sd2,&sendshort,sizeof(uint16_t),0);
            }
         ////////////////////////////////////////////////////////////////////
          printf("s> %s stat= %d %d %d %d %d\n",ip, st.ping, st.swap, st.hash, st.check, st.stat);


          //sleep(20);   //              CONCURRENCY TEST
          //printf("Done %d\n",id);

          close(*sd2);

          pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
	char *program_name = argv[0];
	int opt, port=0;
	char *port_s;

	// Parse command-line arguments
	while ((opt = getopt(argc, argv, "dp:")) != -1) {
		switch (opt) {
			case 'd':
				debug = 1;
				break;
			case 'p':
				port_s = optarg;
				port = strtol(optarg, NULL, 10);
				break;
			case '?':
				if (optopt == 'p')
					fprintf (stderr, "Option -%c requires an argument.\n", optopt);
				else if (isprint (optopt))
					fprintf (stderr, "Unknown option `-%c'.\n", optopt);
				else
					fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
			default:
				usage(program_name);
				exit(EX_USAGE);
		}
	}

	if ((port < 1024) || (port > 65535)) {
		fprintf (stderr, "Error: Port must be in the range 1024 <= port <= 65535\n");
		usage(program_name);
		exit(EX_USAGE);
	}

	if (debug)
		fprintf(stderr, "DEBUG ON\n");

            	// Get Local IP

        gethostname(hostname, sizeof(hostname));

        int status;
        struct addrinfo hints;
        struct addrinfo *servinfo;
        memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
        hints.ai_flags = AI_PASSIVE; // fill in my IP for me

        if ((status = getaddrinfo(NULL, port_s, &hints, &servinfo)) != 0) {
            fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
            exit(1);
        }

        ///////////////////////////////////////////////////////////////////////

        //Create socket
        int sd1 = -1;
        sd1 = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);

        ///////////////////////////////////////////////////////////////////////

        //Force to listen through the socket
        bind(sd1, servinfo->ai_addr, servinfo->ai_addrlen);
        listen(sd1,100);

        /////////////////////////////////////////////////////////////////

	struct hostent *lh = gethostbyname(hostname);
	char* ip = inet_ntoa(*(struct in_addr*)lh->h_addr);
	strcat(ip,":");
	strcat(ip,port_s);
	printf("s> init server %s\n",ip);

        int sd2 = -1;

        pthread_mutex_init(&mutexping, NULL);
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);




    ///////////// INITIALIZE STAT STRUCT /////////////////////////////////
        st.ping = 0;
        st.swap = 0;
        st.check = 0;
        st.hash = 0;
        st.stat = 0;
     ////////////////////////////////////////////////////////////////////

        struct sockaddr_storage their_addr;
        socklen_t addr_size;
        addr_size = sizeof(their_addr);

        while(1)
        {
        //char buffer[MAXBUFFERLEN + 1];
        int received;
        //printf("%s\n","s> waiting");
        //Accept connection and receive first message
            printf("%s\n","s> waiting");
            //pthread_mutex_lock(&mutexping);
            //sd2 = -1;
            //pthread_mutex_unlock(&mutexping);
            int connected = -1;
            while(connected == -1)
            {
                //
                connected = accept(sd1, (struct sockaddr *)&their_addr, &addr_size);


            }
            pthread_mutex_lock(&mutexping);
            sd2 = connected;
            pthread_mutex_unlock(&mutexping);
            printf("%s\n","s> accepted");
        ////////////////////////////////////////////////////////////////////
            char *operation;

       ////////////// RECEIVE OPERATION CODE ///////////////////////////////
            received = -1;
            while(received == -1)
            {
                char op[5];
                //pthread_mutex_lock(&mutexping);
                received = recv(sd2, op, MAXBUFFERLEN, 0);
                //pthread_mutex_unlock(&mutexping);
                operation = op;
            }
            operation[received] = '\0';
       /////////////////////////////////////////////////////////////////////
            //printf("Operation %s\n",operation);
            int opno = atoi(operation);
            printf("Operation transformed %d\n",opno);
            //sleep(20);
        ///////// SWITCH TO CORRESPONDING THREAD
            switch(opno)
            {
                case 0:
                    //pthread_mutex_lock(&mutexping);
                    pthread_create (&testthread, &attr, (void *)pingRequest, &sd2);
                    //st.ping++; //INCREMENT STAT STRUCT
                    //pthread_mutex_unlock(&mutexping);
                    break;
                case 1:
                    pthread_create (&testthread, &attr, (void *)swapRequest, &sd2);
                    //pthread_cond_signal(&swap_signal);
                    //pthread_cond_wait(&swap_signal,&mutexping);
                    //close(sd2);
                    //st.swap++; //INCREMENT STAT STRUCT
                    //pthread_mutex_unlock(&mutexping);
                    break;
                case 2:
                    pthread_create (&testthread, &attr, (void *)hashRequest, &sd2);
                    //pthread_cond_signal(&hash_signal);
                    //pthread_cond_wait(&hash_signal,&mutexping);
                    //close(sd2);
                    //st.hash++; //INCREMENT STAT STRUCT
                    //pthread_mutex_unlock(&mutexping);
                    break;
                case 3:
                    pthread_create (&testthread, &attr, (void *)checkRequest, &sd2);
                    //pthread_cond_signal(&check_signal);
                    //pthread_cond_wait(&check_signal,&mutexping);
                    //close(sd2);
                    //st.check++; //INCREMENT STAT STRUCT
                    //pthread_mutex_unlock(&mutexping);
                    break;
                case 4:
                    pthread_create (&testthread, &attr, (void *)statRequest, &sd2);
                    //st.stat++; //INCREMENT STAT STRUCT
                    //pthread_cond_signal(&stat_signal);
                    //pthread_cond_wait(&stat_signal,&mutexping);
                    //close(sd2);
                    //pthread_mutex_unlock(&mutexping);
                    break;
            }



        }
}
