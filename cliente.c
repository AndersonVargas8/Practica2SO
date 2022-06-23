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
#include <stdio_ext.h>


struct inType
{
    int origen, destino, hora;
};

void ejecucion(struct inType *ingreso)
{
    
    struct sockaddr_in server_addr;

    int sockfd = socket(PF_INET, SOCK_STREAM, 0);

    server_addr.sin_family = PF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (sockfd < 0)
    {
        perror("Error en el socket hijo");
    }

    int addrlen = sizeof(struct sockaddr);

   // connect(sockfd, (struct sockaddr *)&server_addr, addrlen);
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Connection failed\n");
        exit(-1);
    }

    printf("Connected with server...\n");

    send(sockfd, ingreso , sizeof(struct inType), 0);

    float media;

    recv(sockfd,&media,sizeof(float),0);

    printf("\nMedia recibida: %lf \n",media);

    close(sockfd);
}

void confirmar(char mensaje[], int dato){

    if(dato != -1){
        printf("%s %d\n",mensaje,dato);
    }else{
        printf("%s\n",mensaje);
    }
    
    printf("%s","Presione ENTER para continuar...");
    getchar();
    getchar();
    __fpurge(stdin);
}

void main()
{
    printf("BIENVENIDO. **COMUNICACIÓN MEDIANTE PASO DE MENSAJES**\n--------------------------------------------\n");
    int opc = 0;
    struct inType *ingreso;
    ingreso = malloc(sizeof(struct inType));

    while (opc != 5)
    {
        printf("\n1. Ingresar origen\n2. Ingresar destino\n3. Ingresar hora\n4. Buscar tiempo de viaje medio\n5. Salir\n=> ");
        scanf("%d", &opc);

        switch (opc)
        {
        case 1:
            printf("\nIngrese el origen: ");
            scanf("%i", &ingreso->origen);
            int origen = (int)ingreso->origen;

            confirmar("\nSe ha guardado el origen:",origen);
            break;

        case 2:
            printf("\nIngrese el destino: ");
            scanf("%i", &ingreso->destino);

            int destino = (int)ingreso->destino;

            confirmar("\nSe ha guardado el destino:",destino);
            break;

        case 3:
            printf("\nIngrese la hora: ");
            scanf("%i", &ingreso->hora);
            int hora = (int)ingreso->hora;

            confirmar("\nSe ha guardado la hora:",hora);
            break;

        case 4:

            printf("\nSe realizará la búsqueda con los siguientes datos:\n");
            printf("\nOrigen: %d", ingreso->origen);
            printf("\nDestino: %d", ingreso->destino);
            printf("\nHora: %d\n", ingreso->hora);
            confirmar("",-1);
            ejecucion(ingreso);

            printf("\nSe regresará al menú\n");
            printf("%s","Presione ENTER para continuar...");
            getchar();
            __fpurge(stdin);

            break;

        case 5:
            exit(EXIT_SUCCESS);
            break;
        }
    }
}