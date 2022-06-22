#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <ctype.h>

struct inType
{
    int origen, destino, hora;
};

struct typeViaje
{
    int idOrigen;
    int idDestino;
    int hora;
    float meanTravelTime;
    float stDevTravelTime;
    float geoMeanTravelTime;
    float geoStDesTravelTime;
    int nextIdOrigen;
};

struct typeIndice
{
    int posicion;
};


int buscarIndice(int origenId)
{
    FILE *archivo;
    archivo = fopen("indices.dat", "rb");
    int posicion = 0;
    if (archivo == NULL)
        exit(-1);
    fseek(archivo, sizeof(struct typeIndice) * origenId, SEEK_SET);
    struct typeIndice indice;
    fread(&indice, sizeof(struct typeIndice), 1, archivo);
    fclose(archivo);
    return indice.posicion;
}
float busqueda(int origenId, int destinoId, int hora)
{
    int posicion = buscarIndice(origenId);
    float mediaViaje = -1;
    if (posicion == -1)
    {
        return mediaViaje;
    }
    FILE *archivo;
    archivo = fopen("datos.dat", "rb");
    if (archivo == NULL)
        exit(-1);
    fseek(archivo, posicion * sizeof(struct typeViaje), SEEK_SET);
    struct typeViaje viaje;
    fread(&viaje, sizeof(struct typeViaje), 1, archivo);

    fseek(archivo, posicion * sizeof(struct typeViaje), SEEK_SET);

    while (viaje.idDestino != destinoId || viaje.hora != hora)
    {
        posicion = viaje.nextIdOrigen;
        fseek(archivo, posicion * sizeof(struct typeViaje), SEEK_SET);
        fread(&viaje, sizeof(struct typeViaje), 1, archivo);
        if (viaje.nextIdOrigen == -1)
            break;
    }
    if (viaje.idDestino == destinoId && viaje.hora == hora)
    {
        mediaViaje = viaje.meanTravelTime;
    }
    fclose(archivo);
    return mediaViaje;
}
void main()
{
    int r;
    struct inType *ingreso;
    ingreso = malloc(sizeof(struct inType));

    struct sockaddr_in server_addr;
    struct sockaddr_in remote_addr;

    memset(&server_addr, 0, sizeof(server_addr));
    int sockfd = socket(PF_INET, SOCK_STREAM, 0);

    server_addr.sin_family = PF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = inet_addr("192.168.42.93");

    if (sockfd < 0)
    {
        perror("Error en el socket padre");
    }

    int ret = bind(sockfd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr));

    if (ret < 0)
    {
        perror("Error en bind");
        exit(EXIT_FAILURE);
    }

    r = listen(sockfd, 5);

    if (r < 0)
    {
        perror("Error en listen");
        exit(-1);
    }
    else
    {
        printf("[SERVER] : Listening on port 8080\n");
    }

    int opc = 0;
    int addrlen = sizeof(struct sockaddr);

    int newSockfd = -1;

    while (1)
    {
        clock_t tiempo1 = clock();
        if (opc == 0)
        {
            newSockfd = accept(sockfd, (struct sockaddr *)&remote_addr, &addrlen);

            //printf("%d\n",newSockfd);
            if (newSockfd < 0)
            {
                perror("Error en accept");
                exit(1);
            }

            if (newSockfd != -1)
            {
                opc = 1;
            }
        }

        if (opc == 1)
        {
            long recibido = recv(newSockfd, ingreso, sizeof(struct inType), 0);

            if (recibido == -1)
            {
                opc = 0;
                continue;
            }
            printf("DATO RECIBIDO: Origen:%d Destino:%d Hora:%d\n", ingreso->origen, ingreso->destino, ingreso->hora);
            float media = busqueda(ingreso->origen, ingreso->destino, ingreso->hora);

            char * name;
            printf("IP DEL CLIENTE:%d \n", getsockname(newSockfd,(struct sockaddr *)&remote_addr,&addrlen)); 
            recibido = send(newSockfd, &media, sizeof(float), 0);

            clock_t tiempo2 = clock();

            double tiempo = (double)(tiempo2 - tiempo1) / CLOCKS_PER_SEC;

            printf("Tiempo de ejecución: %f segundos\n", tiempo);
            //close(sockfd);
            close(newSockfd);
        }
    }
}