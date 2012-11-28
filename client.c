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
#include <wordexp.h>
#include <time.h>
#include <sys/time.h>

#define MAXBUFFERLEN 500

char * port_s;
int debug = 0;


void usage(char *program_name) {
	printf("Usage: %s [-d] -s <server> -p <port>\n", program_name);
}


void f_ping(char *port_s){
    
        // Get host ip
        char hostname[128];
        gethostname(hostname, sizeof(hostname));
	struct hostent *lh = gethostbyname(hostname);
	char* ip = inet_ntoa(*(struct in_addr*)lh->h_addr);
	strcat(ip,":");
	strcat(ip,port_s);
	printf("c> client ip: %s\n",ip);
        /////////////////////////////////////////////

        int status;
        struct addrinfo hints;
        struct addrinfo *servinfo;
        memset(&hints, 0, sizeof hints);

        /////// Set parameters and port of socket ////////////////////////////////
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
        hints.ai_flags = AI_PASSIVE; // fill in my IP for me

        if ((status = getaddrinfo(NULL, port_s, &hints, &servinfo)) != 0) {
            fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
            exit(1);
        }
        ///////////////////////////////////////////////////////////////////////////////

        //// Declare socket and obtain descriptor /////////////////////////////////////
        int sd1;
        sd1 = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);

        ////////////////////////////////////////////////////////////////////////////////

        ///// Message to send ////////////////////////////////////////////////////
        char *msg = " ping";
        strcat(ip,msg);

        ///////////////////////////////////////////////////////////

        int length = strlen(ip);
        struct sockaddr_storage their_addr;
        socklen_t addr_size;
        addr_size = sizeof(their_addr);
        int received;

        // Connect and bind port to receive ACK ////////////////////////

        //sleep(5);
	while(connect(sd1, servinfo->ai_addr, servinfo->ai_addrlen) == -1);
        //bind(sd1, servinfo->ai_addr, servinfo->ai_addrlen);
        //listen(sd1,100);

        /////////////////////////////////////////////////////////////////

        ///////// SEND OPERATION REQUEST ///////////////////////////////
            int sent = -1;
            char *operation = "0";
            while(sent == -1)
            {
                sent = send(sd1,operation,sizeof(operation),0);
            }
        ////////////////////////////////////////////////////////////////

            char buffer[1];
            sent = -1;
            struct timeval start;
            struct timeval end;
            int rc1,rc2;

            rc1=gettimeofday(&start, NULL); //Get current time

            ///////////// DO PING /////////////////////////
            while(sent == -1)
            {
                sent = send(sd1,ip,length,0);
            }
            printf("Sent IP %s\n",ip);

            received = -1;
            while(received == -1)
            {
                received = recv(sd1, buffer, MAXBUFFERLEN, 0);
            }
            //////////////////////////////////////////////////

            rc2=gettimeofday(&end, NULL); //Get current time

             printf("%u.%06u s\n", (end.tv_sec - start.tv_sec)/2, (end.tv_usec - start.tv_usec)/2); //Print time interval

        close(sd1);

}

void f_swap(char *src, char *dst){
	        // Get host ip
        char hostname[128];
        gethostname(hostname, sizeof(hostname));
	struct hostent *lh = gethostbyname(hostname);
	char* ip = inet_ntoa(*(struct in_addr*)lh->h_addr);
	strcat(ip,":");
	strcat(ip,port_s);
	printf("c> client ip: %s\n",ip);

        /////////////////////////////////////////////

        int status;
        struct addrinfo hints;
        struct addrinfo *servinfo;
        memset(&hints, 0, sizeof hints);

        /////// Set parameters and port of socket ////////////////////////////////
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
        hints.ai_flags = AI_PASSIVE; // fill in my IP for me

        if ((status = getaddrinfo(NULL, port_s, &hints, &servinfo)) != 0) {
            fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
            exit(1);
        }

        ///////////////////////////////////////////////////////////////////////////////

        //// Declare socket and obtain descriptor /////////////////////////////////////
        int sd1;
        sd1 = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);

        ////////////////////////////////////////////////////////////////////////////////

        
        struct sockaddr_storage their_addr;
        socklen_t addr_size;
        addr_size = sizeof(their_addr);
        int received;

        // Connect and bind port to receive ACK ////////////////////////

	while(connect(sd1, servinfo->ai_addr, servinfo->ai_addrlen) == -1);
        bind(sd1, servinfo->ai_addr, servinfo->ai_addrlen);
        listen(sd1,100);

        /////////////////////////////////////////////////////////////////

            char buffer[1];

            ////////// SEND OPERATION REQUEST ///////////////////////////
            int sent = -1;
            char *operation = "1";
            while(sent == -1)
            {
                sent = send(sd1,operation,sizeof(operation),0);
            }
            ///////////////////////////////////////////////////////////


            ///////////// SEND IP //////////////////////////////////////
            sent = -1;
            while(sent == -1)
            {
                sent = send(sd1,ip,MAXBUFFERLEN,0);
            }

            sent = -1;
            //////////////////////////////////////////////////
            
 
            //Open file //////////////////
            char *se = (char *) malloc(sizeof(char)*501);
            FILE *fd1 = fopen(src,"r");
            if(fd1 == NULL) perror("fopen");

            //Obtain size /////////////////
            int file_size = 0;
            fseek (fd1 , 0 , SEEK_END);
            file_size = ftell (fd1);
            rewind (fd1);

            // Default characters to read from file
            int readchars = sizeof(char) * 500;



            /// SEND FILE SIZE ///////////////////////
            while(sent == -1)
            {
                sent = send(sd1,&file_size,sizeof(int),0);
            }
            /////////////////////////////////////////////

                sent = -1;
  
            int written = 0;  //Write file pointer
            FILE *fd2 = fopen(dst,"a"); // Write file open
            if(fd2 == NULL) perror("fopen");
            int recin; //Received integer

            while(file_size > 0)
            {

                readchars = 500; // Set chars to read to default

                if(file_size < readchars) readchars = file_size;  //If there are less than 500 to EOF, read that number///
                if(readchars < 0) readchars = 500 + readchars;    ////////////////////////////////////////////////////////

                file_size -= 500; //Decrement chars left to read
                fread(se,1,readchars,fd1);
                se[readchars] = '\0';  // REPLACE EOF WITH END OF STRING

                /////// SEND FILE CHUNK ////////////////////////
                sent = -1;
                while(sent == -1)
                {
                    sent = send(sd1,se,MAXBUFFERLEN,0);
                }
                /////////////////////////////////////////////////////

                /////////// RECEIVE SWAPPED LETTERS COUNT ///////////
                received = -1;
                
                while(received == -1)
                {
                    received = recv(sd1, &recin, sizeof(recin), 0);
                }
                buffer[received] = '\0';
                
                ///////////////////////////////////////////////////////

                ////////// RECEIVE SWAPPED CHAIN //////////////////////
                received = -1;
                char capsch[MAXBUFFERLEN + 1];
                while(received == -1)
                {
                    received = recv(sd1, capsch, sizeof(capsch), 0);
                }
                //printf("Received %s\n",capsch);
                /////////////////////////////////////////////////////////

                ///////// STORE RESULT IN FILE ///////////////////////////


                if(readchars != 500) capsch[readchars - 1] = '\n';
                fseek(fd2,written,SEEK_SET);
                written += readchars;
                fwrite(capsch,1,readchars,fd2);
                
                se[readchars - 1] = '\0';  // REPLACE EOF WITH END OF STRING
                ////////////////////////////////////////////////////////////

            }
            printf("%d\n",recin); //Print swapped letters

            //Close descriptors for future connections & file manipulation
            fclose(fd1);
            fclose(fd2);

            close(sd1);
}

void f_hash(char *src){
        char hostname[128];
        gethostname(hostname, sizeof(hostname));
	struct hostent *lh = gethostbyname(hostname);
	char* ip = inet_ntoa(*(struct in_addr*)lh->h_addr);
	strcat(ip,":");
	strcat(ip,port_s);
	printf("c> client ip: %s\n",ip);

        /////////////////////////////////////////////

        int status;
        struct addrinfo hints;
        struct addrinfo *servinfo;
        memset(&hints, 0, sizeof hints);

        /////// Set parameters and port of socket ////////////////////////////////
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
        hints.ai_flags = AI_PASSIVE; // fill in my IP for me

        if ((status = getaddrinfo(NULL, port_s, &hints, &servinfo)) != 0) {
            fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
            exit(1);
        }

        ///////////////////////////////////////////////////////////////////////////////

        //// Declare socket and obtain descriptor /////////////////////////////////////
        int sd1;
        sd1 = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);

        ////////////////////////////////////////////////////////////////////////////////

        struct sockaddr_storage their_addr;
        socklen_t addr_size;
        addr_size = sizeof(their_addr);
        int received;


        // Connect and bind port to receive ACK ////////////////////////

	while(connect(sd1, servinfo->ai_addr, servinfo->ai_addrlen) == -1);
        bind(sd1, servinfo->ai_addr, servinfo->ai_addrlen);
        listen(sd1,100);

        /////////////////////////////////////////////////////////////////

        /////// SEND OPERATION CODE ///////////////////////////////
            int sent = -1;
            char *operation = "2";
            while(sent == -1)
            {
                sent = send(sd1,operation,sizeof(operation),0);
            }
        ///////////////////////////////////////////////////////////////

        ////////////// SEND IP //////////////////////////////////////////
            sent = -1;
            while(sent == -1)
            {
                sent = send(sd1,ip,MAXBUFFERLEN,0);
            }

            sent = -1;
            //////////////////////////////////////////////////


            // Initialize file chunk iteration variables
            char *se = (char *) malloc(sizeof(char)*501);
            FILE *fd1 = fopen(src,"r");
            if(fd1 == NULL) perror("fopen");
            int file_size = 0;
            fseek (fd1 , 0 , SEEK_END);
            file_size = ftell (fd1);
            rewind (fd1);
            int readchars = sizeof(char) * 500;

            ///////// SEND FILE SIZE //////////////////////////////////
            while(sent == -1)
            {
                sent = send(sd1,&file_size,sizeof(int),0);
            }

                sent = -1;

            //////////////////////////////////////////////////////////


            while(file_size > 0)
            {

                readchars = 500; //Set read pointer to default
                if(file_size < readchars) readchars = file_size; ///Check and adjust pointer if 500 bytes not left
                if(readchars < 0) readchars = 500 + readchars;   /////////////////////////////////////////////////

                file_size -= 500;  //Decrement chars left


                fread(se,1,readchars,fd1);
                se[readchars] = '\0';  // REPLACE EOF WITH END OF STRING
                
                /////// SEND FILE CHUNK ////////////////////////
                sent = -1;
                while(sent == -1)
                {
                    sent = send(sd1,se,MAXBUFFERLEN,0);
                }
                //printf("Iteration File size is %d %d %s\n",file_size, readchars, se);
                /////////////////////////////////////////////////////

            }
                /////////// RECEIVE HASH ///////////
                received = -1;
                int recin;
                while(received == -1)
                {
                    received = recv(sd1, &recin, sizeof(int), 0);
                }
                ///////////////////////////////////////////////////////
                printf("%d\n", recin);
            //////////////////////////////////////////////////////////
                // Close descriptors for future socket & file use
                fclose(fd1);
                close(sd1);
}

void f_check(char *src, int hash){
       char hostname[128];
        gethostname(hostname, sizeof(hostname));
	struct hostent *lh = gethostbyname(hostname);
	char* ip = inet_ntoa(*(struct in_addr*)lh->h_addr);
	strcat(ip,":");
	strcat(ip,port_s);
	printf("c> client ip: %s\n",ip);

        /////////////////////////////////////////////

        int status;
        struct addrinfo hints;
        struct addrinfo *servinfo;
        memset(&hints, 0, sizeof hints);

        /////// Set parameters and port of socket ////////////////////////////////
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
        hints.ai_flags = AI_PASSIVE; // fill in my IP for me

        if ((status = getaddrinfo(NULL, port_s, &hints, &servinfo)) != 0) {
            fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
            exit(1);
        }

        ///////////////////////////////////////////////////////////////////////////////

        //// Declare socket and obtain descriptor /////////////////////////////////////
        int sd1;
        sd1 = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);

        ////////////////////////////////////////////////////////////////////////////////

        struct sockaddr_storage their_addr;
        socklen_t addr_size;
        addr_size = sizeof(their_addr);
        int received;

        // Connect and bind port to receive ACK ////////////////////////

	while(connect(sd1, servinfo->ai_addr, servinfo->ai_addrlen) == -1);
        bind(sd1, servinfo->ai_addr, servinfo->ai_addrlen);
        listen(sd1,100);

        /////////////////////////////////////////////////////////////////

        /// SEND OPERATION CODE /////////////////////////////////////////
            int sent = -1;
            char *operation = "3";
            while(sent == -1)
            {
                sent = send(sd1,operation,sizeof(operation),0);
            }
        /////////////////////////////////////////////////////////////////

         /////// SEND IP /////////////////////////////////////////////////
            sent = -1;
            while(sent == -1)
            {
                sent = send(sd1,ip,MAXBUFFERLEN,0);
            }

            sent = -1;
        /////////////////////////////////////////////////////////////////////

        ////////// GET FILE DATA ///////////////////////

            char *se = (char *) malloc(sizeof(char)*501);
            FILE *fd1 = fopen(src,"r");
            if(fd1 == NULL) perror("fopen");
            int file_size = 0;
            fseek (fd1 , 0 , SEEK_END);
            file_size = ftell (fd1);
            rewind (fd1);
            int readchars = sizeof(char) * 500;


            /////// SEND FILE SIZE ////////////////////////////
            while(sent == -1)
            {
                sent = send(sd1,&file_size,sizeof(int),0);
            }
            ////////////////////////////////////////////////////


            while(file_size > 0)
            {

                readchars = 500; /// SET DEFAULT READ POINTER VALUE
                if(file_size < readchars) readchars = file_size; /// ADJUST POINTER VALUE ////////////////
                if(readchars < 0) readchars = 500 + readchars;   /////////////////////////////////////////

                file_size -= 500; /// DECREMENT CHARS TO READ

                fread(se,1,readchars,fd1);
                se[readchars] = '\0';  // REPLACE EOF WITH END OF STRING

                /////// SEND FILE STRING ////////////////////////
                sent = -1;
                while(sent == -1)
                {
                    sent = send(sd1,se,MAXBUFFERLEN,0);
                }
                /////////////////////////////////////////////////////

            }

            ///////// SEND HASH INPUT ////////////////////////////////////
                sent = -1;
                while(sent == -1)
                {
                    sent = send(sd1, &hash, sizeof(int), 0);
                }
            //////////////////////////////////////////////////////////////


            ///////// RECEIVE SERVER VERDICT ///////////////////////////////
                received = -1;
                char *res = (char *) malloc(sizeof(char)*5);
                while(received == -1)
                {
                    received = recv(sd1,res,sizeof(res),0);
                }
            /////////////////////////////////////////////////////////////////
                printf("%s\n", res);
            //////////////////////////////////////////////////////////

                //CLOSE DESCRIPTORS FOR FUTURE USE
                fclose(fd1);
                close(sd1);
}

void f_stat(){
        // Get host ip
        char hostname[128];
        gethostname(hostname, sizeof(hostname));
	struct hostent *lh = gethostbyname(hostname);
	char* ip = inet_ntoa(*(struct in_addr*)lh->h_addr);
	strcat(ip,":");
	strcat(ip,port_s);
	printf("c> client ip: %s\n",ip);

        /////////////////////////////////////////////

        int status;
        struct addrinfo hints;
        struct addrinfo *servinfo;
        memset(&hints, 0, sizeof hints);

        /////// Set parameters and port of socket ////////////////////////////////
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
        hints.ai_flags = AI_PASSIVE; // fill in my IP for me

        if ((status = getaddrinfo(NULL, port_s, &hints, &servinfo)) != 0) {
            fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
            exit(1);
        }

        ///////////////////////////////////////////////////////////////////////////////

        //// Declare socket and obtain descriptor /////////////////////////////////////
        int sd1;
        sd1 = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);

        ////////////////////////////////////////////////////////////////////////////////


        ///////////////////////////////////////////////////////////
        int length = strlen(ip);
        struct sockaddr_storage their_addr;
        socklen_t addr_size;
        addr_size = sizeof(their_addr);
        int received;

        // Connect and bind port to receive ACK ////////////////////////

	while(connect(sd1, servinfo->ai_addr, servinfo->ai_addrlen) == -1);
        bind(sd1, servinfo->ai_addr, servinfo->ai_addrlen);
        listen(sd1,100);

        /////////////////////////////////////////////////////////////////

            int sent = -1;
            
            while(sent == -1)
            {
                sent = send(sd1,"4",1,0);
            }


            sent = -1;

            
            while(sent == -1)
            {
                sent = send(sd1,ip,length,0);
            }

            ////////////// Ping stat //////////////////////////
            received = -1;
            int pingstat;
            while(received == -1)
            {
                received = recv(sd1, &pingstat, sizeof(int), 0);
            }
            printf("Ping %d\n",pingstat);
            ///////////////////////////////////////////////////

            ////////////// Swap stat //////////////////////////
            received = -1;
            int swapstat;
            while(received == -1)
            {
                received = recv(sd1, &swapstat, sizeof(int), 0);
            }
            printf("Swap %d\n",swapstat);
            ////////////////////////////////////////////////////

            ////////////// Hash stat /////////////////////////
            received = -1;
            int hashstat;
            while(received == -1)
            {
                received = recv(sd1, &hashstat, sizeof(int), 0);
            }
            printf("Hash %d\n",hashstat);
            ////////////////////////////////////////////////////

            ////////////// Check stat ///////////////////////////////
            received = -1;
            int checkstat;
            while(received == -1)
            {
                received = recv(sd1, &checkstat, sizeof(int), 0);
            }
            printf("Check %d\n",checkstat);
            ////////////////////////////////////////////////////////

            ///////////// Stat stat ////////////////////////////////////
            received = -1;
            int stat;
            while(received == -1)
            {
                received = recv(sd1, &stat, sizeof(int), 0);
            }
            printf("Stat %d\n",stat);
            ////////////////////////////////////////////////////////////
            
            close(sd1);
}

void quit(){
}

void shell() {
	char line[1024];
	char *pch;
	int exit = 0;
	
	wordexp_t p;
	char **w;
	int ret;
	
	memset(&p, 0, sizeof(wordexp));
	
	do {
		fprintf(stdout, "c> ");
		memset(line, 0, 1024);
		pch = fgets(line, 1024, stdin);
		
		if ( (strlen(line)>1) && ((line[strlen(line)-1]=='\n') || (line[strlen(line)-1]=='\r')) )
			line[strlen(line)-1]='\0';
		
		ret=wordexp((const char *)line, &p, 0);
		if (ret == 0) {
			w = p.we_wordv;
		
			if ( (w != NULL) && (p.we_wordc > 0) ) {
				if (strcmp(w[0],"ping")==0) {
					if (p.we_wordc == 1)
						f_ping(port_s);
					else
						printf("Syntax error. Use: ping\n");
				} else if (strcmp(w[0],"swap")==0) {
					if (p.we_wordc == 3)
						f_swap(w[1],w[2]);
					else
						printf("Syntax error. Use: swap <source_file> <destination_file>\n");
				} else if (strcmp(w[0],"hash")==0) {
					if (p.we_wordc == 2)
						f_hash(w[1]);
					else
						printf("Syntax error. Use: hash <source_file>\n");
				} else if (strcmp(w[0],"check")==0) {
					if (p.we_wordc == 3)
						f_check(w[1], atoi(w[2]));
					else
						printf("Syntax error. Use: check <source_file> <hash>\n");
				} else if (strcmp(w[0],"stat")==0) {
					if (p.we_wordc == 1)
						f_stat();
					else
						printf("Syntax error. Use: stat\n");
				} else if (strcmp(w[0],"quit")==0) {
					quit();
					exit = 1;
				} else {
					fprintf(stderr, "Error: command '%s' not valid.\n", w[0]);
				}
			}
			
			wordfree(&p);
		}
	} while ((pch != NULL) && (!exit));
}

int main(int argc, char *argv[]){
	char *program_name = argv[0];
	int opt, port=0;
	char *server;
	
	// Parse command-line arguments
	while ((opt = getopt(argc, argv, "ds:p:")) != -1) {
		switch (opt) {
			case 'd':
				debug = 1;
				break;
			case 's':
				server = optarg;
				break;
			case 'p':
				port_s = optarg;
				port = strtol(optarg, NULL, 10);
				break;
			case '?':
				if ((optopt == 's') || (optopt == 'p'))
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
		printf("SERVER: %s PORT: %d\n",server, port);

        //f_ping(port_s);
        //while(1)
        //{
            //f_ping(port_s);
        //}
	shell();
	
	exit(EXIT_SUCCESS);
}

