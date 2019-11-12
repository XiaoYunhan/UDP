#include "headsock.h"

void str_ser(int sockfd);

int main(int argc, char *argv[]) {
	int sockfd;
	struct sockaddr_in my_addr;
	//struct sockaddr_in ack_addr;

	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		printf("error in socket");
		exit(1);
	}

	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(MYUDP_PORT);
	my_addr.sin_addr.s_addr = INADDR_ANY;
	bzero(&(my_addr.sin_zero), 8);
	if (bind(sockfd, (struct sockaddr *) &my_addr, sizeof(struct sockaddr)) == -1) {
		printf("error in binding");
		exit(1);
	}
	printf("start receiving\n");
	while(1) {
	    str_ser(sockfd);
	}
	close(sockfd);
	exit(0);
}

void str_ser(int sockfd) {
	char recvs[DATALEN];
	char recvs2[2*DATALEN];
    char buf[BUFSIZE];
    FILE *fp;
	int n = 0, len, end = 0;
    long lseek = 0;
	struct sockaddr_in addr;
	struct ack_so ack;
	ack.num = 1;
	ack.len = 0;
	int count = 0;

	len = sizeof (struct sockaddr_in);
    printf("receiving data!\n");

    while(!end) {
        if ((n=recvfrom(sockfd, &recvs, DATALEN, 0, (struct sockaddr *)&addr, &len)) == -1) {
            printf("error receiving");
            exit(1);
        }
        if(recvs[n-1] == '\0'){
            end = 1;
            n--;
        }

		if(count%3!=1) {
			//printf("no ack receive");
			if (sendto(sockfd, &ack, 2, 0, (struct sockaddr *)&addr, len)==-1) {
				printf("send error!");
				exit(1);
			}
		}
		//else {
			//printf("ack_receive");
		//}

		memcpy((buf+lseek), recvs, n);
		lseek += n;
		count++;
	}
		
	if ((fp = fopen ("myTCPreceive.txt","wt")) == NULL) {
		printf("File doesn't exit\n");
		exit(0);
	}

	fwrite (buf , 1 , lseek , fp);
	fclose(fp);
	printf("a file has been successfully received!\nthe total data received is %d bytes\n", (int)lseek);
}