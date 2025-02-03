#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#define SIZE 1024

void WRQ(FILE* fp, int sockfd, struct sockaddr_in addr, const char *nomfichier)
{
  int n;
  char buffer[SIZE];

  
n = sendto(sockfd, nomfichier, strlen(nomfichier), 0, (struct sockaddr*)&addr, sizeof(addr));//le nom du fichier au serveur
  if (n == -1)
  {
    perror("Erreur lors de l'envoi du fichier au server ");
    exit(1);
  }
  printf("Nom du fichier en cours d'envoi : %s\n", nomfichier);

//les données du fichier
  while (fgets(buffer, SIZE, fp) != NULL)
  {
    printf("\nEnvoie en cours des données  : %s", buffer);

    n = sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr*)&addr, sizeof(addr));
    if (n == -1)
    {
      perror("Erreur lors de l'envoi au server.");
      exit(1);
    }
    bzero(buffer, SIZE);
  }


//'END' pour indiquer la fin du fichier
  strcpy(buffer, "END");
  sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr*)&addr, sizeof(addr));

  fclose(fp);
}




int main(int argc, char *argv[])
{
//verification que le fichier est bien passé en argument
  if (argc != 2)
  {
    fprintf(stderr, "[USAGE] %s <nom_du_fichier>\n", argv[0]);
    exit(1);
  }

  
  char *nomfichier = argv[1];//on récupère le nom du fichier depuis l'argument

  //on définit l'IP et le port
  char *ip = "127.0.0.1";
  const int port = 8080;

  int server_sockfd;//déclaration des variables pour le socket pour la connexion
  struct sockaddr_in server_addr;
  FILE *fp = fopen(nomfichier, "r");

  //on crée le socket UDP
  server_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (server_sockfd < 0)
  {
    perror("impossble de créer le socket");
    exit(1);
  }


  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);
  server_addr.sin_addr.s_addr = inet_addr(ip);

  if (fp == NULL)//on vérifie si le fichier a bien pu etre ouvert en lecture 
  {
    perror("Erreur lors de la lecture du fichier");
    exit(1);
  }

  
WRQ(fp, server_sockfd, server_addr, nomfichier);//on envoie maintenant le fichier au server
printf("Données transférées.\n");
printf("Déconnexion du server\n");

  close(server_sockfd); //on ferme le socket

  return 0;
}

