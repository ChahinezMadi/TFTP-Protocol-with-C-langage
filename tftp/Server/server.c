#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#define SIZE 1024

void GestionWRQ(int sockfd, struct sockaddr_in addr)//fonction qui va gérer la requete d'ecriture que le client envoit 
{
    char nomfichier[SIZE]; //le nom du fichier
    int n;
    char buffer[SIZE];
    socklen_t addr_size = sizeof(addr);   
    n = recvfrom(sockfd, nomfichier, SIZE - 1, 0, (struct sockaddr*)&addr, &addr_size); //on reçoit le nom du fichier que le client envoit
    if (n <= 0) {
        perror("erreur lors d la réception du fichier");
        exit(1);
    }
    nomfichier[n] = '\0'; //on rajoute le char de fin de chaine
    printf("fichier reçu: %s\n", nomfichier);


    FILE* fp = fopen(nomfichier, "w");//on ouvre un fichier avec le nom correspondant en ecriture
    if (fp == NULL) {
        perror("erreur lors de l'ouverture du fichier");
        exit(1);
    }




    while (1) {//on reçoit et on écrit les données dans le fichier
        n = recvfrom(sockfd, buffer, SIZE, 0, (struct sockaddr*)&addr, &addr_size);
        if (n <= 0) {
            perror("Erreur lors de la réception ds données");
            break;
        }

        buffer[n] = '\0';// Assurer que la chaîne est bien formée
        if (strcmp(buffer, "END") == 0) {
            printf("Contenu reçu complet.\n");
            break;  //fin du transfert on arrete le processus
        }
        printf("Récépttion des données : %s", buffer);  //on affiche chaque morceau de données reçu
        fprintf(fp, "%s", buffer);  //on écrit dans le fichier qui sera dans le server les données qu'on reçoit
        bzero(buffer, SIZE);
    }

 fclose(fp);
 printf("Fichier reçu par le serveur et enregistré comme : %s\n", nomfichier);
}





int main()
{
    char* ip = "127.0.0.1";
    const int port = 8080;

    int server_sockfd; //on déclare les vars pour le socket
    struct sockaddr_in server_addr, client_addr;
    int e;

    //le socket UDP
    server_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_sockfd < 0) {
        perror("erreur lors de la création du socket");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip);

//on associe le socket à l'adresse et au port pour assurer la connexion enre le server et le client
    e = bind(server_sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (e < 0) {
        perror("erreur pour assurer la connexion");
        exit(1);
    }
 printf("le server UDP a commencé . En atente de fichiers \n");

    //on fait appel à la fonction pour recevoir et enregistrer le fichier GestionWRQ
  GestionWRQ(server_sockfd, client_addr);
    printf("Données transféré avec succés \n");
    printf("déconnexion du server.\n");

    close(server_sockfd);

    return 0;
}

