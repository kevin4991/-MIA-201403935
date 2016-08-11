#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

//////////////////////////////////
////// LISTA DE COMANDOS /////////
//////////////////////////////////

struct token
{
    char valor [200];
    char tipo [200];
    int fila;
    int columna;
    struct token *siguiente;
};

typedef struct token Token_componente;

struct lista{
    Token_componente *primero;
    Token_componente *ultimo;
    int numero_ingresado;
};

typedef struct lista Lista_comando;

void inicializar_lista(Lista_comando **listado)
{
    (*listado)->primero = (Token_componente*) malloc(sizeof(Token_componente));
    (*listado)->ultimo= (Token_componente*) malloc(sizeof(Token_componente));
}

void inicializarlista(Lista_comando **lista)
{
    (*lista) = (Lista_comando*) malloc(sizeof(Lista_comando));
}

void imprimir( Token_componente *primero)
{
    printf("LISTA DE COMANDOS: \n");
    while(primero!=NULL)
    {
        printf("%s",primero->valor);
        primero = primero->siguiente;
    }
    printf("\n");
}

void insertar(Lista_comando **value, char valor [200], char tipo [200], int columna, int fila)
{
    Token_componente *nuevo = (Token_componente*) malloc(sizeof(Token_componente));
    strcpy(nuevo->tipo,tipo);
    strcpy(nuevo->valor,valor);
    nuevo->columna = columna;
    nuevo->fila = fila;
    nuevo->siguiente = NULL;

    if((*value)->primero!=NULL){
        (*value)->primero = nuevo;
        (*value)->ultimo = nuevo;
    }
    else{
        (*value)->ultimo->siguiente = nuevo;
        (*value)->ultimo = nuevo;
    }


}

//////////////////////////////////
////////  ESTRUCTURAS  ///////////
//////////////////////////////////

struct PARTITION{
    char part_status;
    char part_type;
    char part_fit;
    int part_start;
    int part_size;
    char part_name[16];
};
typedef struct PARTITION Particion;

struct MASTER_BOOT_RECORD
{
    int mbr_tamano;
    time_t mbr_fecha_creacion;
    int mbr_disk_signature;
    Particion mbr_partition[4];
};
typedef struct MASTER_BOOT_RECORD MBR;

struct EXTENDED_BOOT_RECORD
{
    char part_status;
    char part_fit;
    int part_start;
    int part_size;
    int part_next;
    char part_name[16];
};
typedef struct EXTENDED_BOOT_RECORD EBR;

//////////////////////////////////
//////   METODOS DE MANEJO  //////
//////////////////////////////////

// --- ACCIONES DE LA CONSOLA
// MKDISK

int tamano_asignacion(int unidades, int tamano){

    int tamano_disco;
    //printf("Tamano_asignado: %i  -  %i\n",unidades,tolower('B'));

    if(unidades==tolower('M'))
    {
        //tamano_disco = tamano * 1000000;
        tamano_disco = 1000000;
    }
    else if (unidades==tolower('K'))
    {
        //tamano_disco = tamano * 1024;
        tamano_disco = 1024;
    }
    else if (unidades==tolower('B'))
    {
        //tamano_disco = tamano;
        tamano_disco = 1;
    }
    else if (unidades==tolower(' '))
    {
        tamano_disco = 1024;
    }

    return tamano_disco;
}

void MKDISK_C (int tamano, char unidades, char *path, char *nombre)
{

    char auxiliar [300];
    char path_completo [300];
    strcpy(path_completo,"'");
    strcat(path_completo,path);

    //PRIMERO REMOVEMOS EL ARCHIVO SI EXISTIESE
    strcpy(auxiliar,"");
    strcat(auxiliar, "rm -dfr ");
    strcat(auxiliar,path_completo);
    //strcat(auxiliar,nombre);
    strcat(auxiliar,"'");
    system(auxiliar);

    //CREACION DEL DIRECTORIO - Se realizara por medio de lineas de comando de consola
    strcpy(auxiliar,"");
    strcat(auxiliar, "mkdir -p -m777 ");
    strcat(auxiliar,path_completo);
    //strcat(auxiliar,nombre);
    strcat(auxiliar,"'");
    printf("%s\n",auxiliar);
    system(auxiliar);
    //fgets(auxiliar,300,stdin);
    //printf(auxiliar);
    strcat(path_completo,"'");


    //ASIGNACION DEL TAMAÑO DEL DISCO

    char buffer [tamano_asignacion(unidades,tamano)];
    int indice = 0;
    for(indice = 0; indice <tamano_asignacion(unidades,tamano); indice++)
    {
        buffer[indice] = '\0';
    }

    char path_nuevo [500];
    strcpy(&path_nuevo,"");
    strcat(&path_nuevo,path);
    strcat(&path_nuevo,"/");
    strcat(&path_nuevo,nombre);
    strcat(&path_nuevo,"");
    //printf("Aqui estoy %s\n",path_nuevo);
    //printf("voy1\n");
    FILE *fichero;
    fichero=fopen(path_nuevo,"ab+");

    int RE =  tamano;
    RE = tamano_asignacion(tolower(unidades),tamano);
    //printf("%i",RE);

    int fus = 0;
    if(fichero){
        fseek(fichero,0,SEEK_SET);
        for(fus=0;fus<tamano;fus++){
            fwrite(&buffer,RE,1,fichero);
        }
        fclose(fichero);
    }

    //CREACION DEL MBR DE LA MASTER BOOT RECORD
    srand(time(NULL));
    int signature_unico = rand();
    time_t t;
    struct tm *tm;
    char fechayhora[100];

    t=time(NULL);
    tm=localtime(&t);
    strftime(fechayhora, 100, "%d/%m/%Y", tm);
    //printf ("Hoy es: %s\n", fechayhora);


    MBR mbr_de_particion;
    mbr_de_particion.mbr_fecha_creacion = t;
    mbr_de_particion.mbr_tamano = RE * tamano;
    mbr_de_particion.mbr_disk_signature = signature_unico;

    //INICIALIZACION DE LAS PARTICIONES DEL DISCO DURO

    int inde = 0;
    for(inde=0;inde<4;inde++)
    {
        mbr_de_particion.mbr_partition[inde].part_fit = '\0';
        strcpy(mbr_de_particion.mbr_partition[inde].part_name,"");
        mbr_de_particion.mbr_partition[inde].part_size = 0;
        mbr_de_particion.mbr_partition[inde].part_start = 0;
        mbr_de_particion.mbr_partition[inde].part_status = '\0';
        mbr_de_particion.mbr_partition[inde].part_type = '\0';
    }

    //INGRESO DEL MBR AL NUEVO DISCO DURO

    FILE *disco;
    disco = fopen(path_nuevo,"rb+");
    fseek(disco,0,SEEK_SET);
    fwrite(&mbr_de_particion,sizeof(mbr_de_particion),1,disco);
    fclose(disco);
}

void RMDISK_C (char *path)
{
    char auxiliar [300];
    char path_completo [300];
    strcpy(path_completo,"'");
    strcat(path_completo,path);
    //PRIMERO REMOVEMOS EL ARCHIVO SI EXISTIESE
    strcpy(auxiliar,"");
    strcat(auxiliar, "rm -dfr ");
    strcat(auxiliar,path_completo);
    //strcat(auxiliar,nombre);
    strcat(auxiliar,"'");
    system(auxiliar);
}

//////////////////////////////////
//////  METODOS DE REPORTE  //////
//////////////////////////////////


void listar_mbr(char *path)
{
    FILE *disco;
    disco = fopen(path,"rb");
    MBR mbr_particion;
    fread(&mbr_particion,sizeof(MBR),1,disco);

    time_t t;
    struct tm *tm;
    char fechayhora[100];
    t=mbr_particion.mbr_fecha_creacion;
    tm=localtime(&t);
    strftime(fechayhora, 100, "%d/%m/%Y", tm);

    printf("\nParticion: %d Bytes\n",mbr_particion.mbr_tamano);
    printf("Fecha de creacion: %s\n", fechayhora);
    printf("Identificador del disco: %i\n",mbr_particion.mbr_disk_signature);

}


Lista_comando *lista_tokens;

int main()
{
    inicializarlista(&lista_tokens);
    inicializar_lista(&lista_tokens);
    insertar(&lista_tokens,"FLOW","HOLI",1,1);
    imprimir(lista_tokens->primero);

    MKDISK_C(200,'b',"/home/kevin/chatos/como_son","chatos.dsk");
    listar_mbr("/home/kevin/chatos/como_son/chatos.dsk");
    //RMDISK_C("/home/kevin/chatos/como_son/chatos.dsk");
    return 0;
}
