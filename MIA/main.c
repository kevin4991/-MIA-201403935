#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define TRUE 1
#define FALSE 0

//////////////////////////////////
////// LISTA DE COMANDOS /////////
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

//// LISTA MOUNT

struct nodo_mount
{
    Particion contenido;
    struct nodo_mount *siguiente;
};

typedef struct nodo_mount NODO_MOUNT;

struct lista_mount
{
    NODO_MOUNT *primero;
    NODO_MOUNT *ultimo;
};

typedef struct lista_mount Lista_MOUNT;

void iniciar_lista_mount(Lista_MOUNT **lista)
{
    (*lista) = (Lista_MOUNT*) malloc(sizeof(Lista_MOUNT));
    (*lista)->primero = (NODO_MOUNT*) NULL;
    (*lista)->ultimo = (NODO_MOUNT*) NULL;
}


void insertar_mount(Lista_MOUNT **value, Particion val)
{
    NODO_MOUNT *nuevo = (NODO_MOUNT*) malloc(sizeof(NODO_MOUNT));
    nuevo->contenido = val;
    nuevo->siguiente = NULL;

    if((*value)->primero==NULL)
    {
        (*value)->primero = nuevo;
        (*value)->ultimo = nuevo;
    }
    else
    {
        (*value)->ultimo->siguiente = nuevo;
        (*value)->ultimo = nuevo;
    }

}


//////////////////////////////////
////////  ESTRUCTURAS  ///////////
//////////////////////////////////

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

void MKDISK_C (int tamano, char unidades, char *path, char *nombre){

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
        mbr_de_particion.mbr_partition[inde].part_status = 'N';
        mbr_de_particion.mbr_partition[inde].part_type = '\0';
    }

    //INGRESO DEL MBR AL NUEVO DISCO DURO

    FILE *disco;
    disco = fopen(path_nuevo,"rb+");
    fseek(disco,0,SEEK_SET);
    fwrite(&mbr_de_particion,sizeof(mbr_de_particion),1,disco);
    fclose(disco);
}

void RMDISK_C (char *path){
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

void FDISK_C(int tamano, char unidades, char *path, char tipo, char *fit, char *del_t, char *name, int add){

    char path_completo [500];
    strcpy(&path_completo,"");
    strcat(&path_completo,path);
    //strcat(&path_completo,"/");
    //strcat(&path_completo,name);
    //strcat(&path_completo,"'");

    //VERIFICACION DE DIRECTORIO Y ARCHIVO

    FILE *disco_consulta;
    disco_consulta = fopen(path_completo,"rb");
    //printf("\nMi path: %s\n",path_completo);
    int boolean_caso = FALSE;
    MBR mbr_particion;

    if(disco_consulta == NULL)
    {
        boolean_caso = FALSE;
        //printf("\n NO EXISTE");
    }
    else{
        boolean_caso = TRUE;
        fread(&mbr_particion,sizeof(MBR),1,disco_consulta);
        //printf("\n EXISTE");
    }

    int espacio_mbr = (int) sizeof(MBR);
    int espacio_particiones = 0;
    int espacion_nueva_particion = tamano_asignacion(tolower(unidades),tamano) * tamano;
    int tamano_disco = mbr_particion.mbr_tamano;
    int tamano_real = 0;
    int particiones_disponibles = 0;
    int particiones_extendidas = 0;
    int particiones_primarias = 0;
    int particiones_sintipo = 0;
    int primer_ajuste_particion=100;
    int tipo_operacion = 0;
    int ACCESO_PARTICION  = false;
    int inicio_particion = (int) sizeof(MBR);


    if (tolower(del_t)!=tolower(' '))
    {
        tipo_operacion = 1; //ELIMINACION DE UNA PARTICION
    }
    else if (add==1)
    {
        tipo_operacion = 2; //ADICION DE ESPACIO A UNA PARTICION
    }
    else
    {
        tipo_operacion = 3; //CREACION DE UNA PARTICION
    }

    if(boolean_caso == TRUE)
    {
        //VERIFICAMOS EL ESPACIO EN PARTICION
        for(int i = 0; i < 4; i++){
            espacio_particiones += mbr_particion.mbr_partition[i].part_size;
            char status = mbr_particion.mbr_partition[i].part_status;
            char itipo = mbr_particion.mbr_partition[i].part_type;


            if(tolower(status) == tolower('n'))
            {
                //printf("\nindice_dispo: %i\n",primer_ajuste_particion);
                if(primer_ajuste_particion==100)
                {
                    //printf("\nEleji un indice: %i\n",i);
                    primer_ajuste_particion = i;
                }
                else{
                    primer_ajuste_particion = primer_ajuste_particion;
                }

                particiones_disponibles++;
            }
            else{
                if(primer_ajuste_particion==100)
                {
                    inicio_particion += mbr_particion.mbr_partition[i].part_size;
                }else{

                }
            }

            //printf("\nindice_dispo: %i\n",primer_ajuste_particion);

            if(tolower(itipo) == tolower('p'))
            {
                particiones_primarias++;
            }
            else if(tolower(itipo) == tolower('e'))
            {
                particiones_extendidas++;
            }
            else if(tolower(itipo) == tolower('\0'))
            {
                particiones_sintipo++;
            }
            //printf("\n%c\n",status);
        }

        tamano_real = tamano_disco - espacio_particiones - espacio_mbr;

       // printf("\nEspacio MBR: %i",espacio_mbr);
       // printf("\nEspacio Particiones: %i",espacio_particiones);
       // printf("\nEspacio Tamano Disco: %i",tamano_disco);
       // printf("\nEspacio Real: %i",tamano_real);
       // printf("\nEspacio Nueva Particion: %i",espacion_nueva_particion);
       // printf("\nParticiones Disponibles: %i",particiones_disponibles);
       // printf("\nParticiones Primarias: %i",particiones_primarias);
       // printf("\nParticiones Extendidas: %i",particiones_primarias);
       // printf("\nParticiones Sin tipo: %i\n",particiones_sintipo);

        if(tamano_real > espacion_nueva_particion && particiones_disponibles>0)
        {
            ACCESO_PARTICION = TRUE;
            //printf("\n\nSE PUEDE PARTICIONAR! \n");
        }
        else
        {
            ACCESO_PARTICION = FALSE;
            //printf("\nNO SE PUEDE PARTICIONAR! \n");
        }
    }


    //ACCION DE LAS OPERACIONES

    if(tipo_operacion==1){
        printf(" \n ELIMINACION ");
    }else if (tipo_operacion == 2)
    {
        //printf(" \n AUMENTO DE TAMAÑO");
    }
    else
    {
        //printf(" \n CREACION DE TAMAÑO");
    }

    if(ACCESO_PARTICION==TRUE)
    {
        if(tolower(tipo)==tolower('p'))
        {
            //printf("\n:: PRIMARIA::\n");
            printf("\n  :::::::: SE CREO LA PARTICION:   %s   :::::::: \n",name);
            mbr_particion.mbr_partition[primer_ajuste_particion].part_fit = fit;
            strcat(mbr_particion.mbr_partition[primer_ajuste_particion].part_name,name);
            mbr_particion.mbr_partition[primer_ajuste_particion].part_size = espacion_nueva_particion;
            mbr_particion.mbr_partition[primer_ajuste_particion].part_start = inicio_particion;
            mbr_particion.mbr_partition[primer_ajuste_particion].part_status = 'A';
            mbr_particion.mbr_partition[primer_ajuste_particion].part_type = tipo;

            //printf("INICIO: %i",mbr_particion.mbr_partition[primer_ajuste_particion].part_start);

            FILE *disco;
            disco = fopen(path_completo,"rb+");
            //ESCRITURA DEL MBR
            fseek(disco,0,SEEK_SET);
            fwrite(&mbr_particion,sizeof(MBR),1,disco);
            fclose(disco);
            //ESCRITURA DE LA PARTICION
            disco = fopen(path_completo,"rb+");
            fseek(disco,mbr_particion.mbr_partition[primer_ajuste_particion].part_start,SEEK_SET);
            fwrite(&mbr_particion.mbr_partition[primer_ajuste_particion],sizeof(Particion),1,disco);
            fclose(disco);
        }
        else{
            //printf("\n:: EXTENDIDA::\n");
            printf("\n  :::::::: SE CREO LA PARTICION:   %s   :::::::: \n",name);
            mbr_particion.mbr_partition[primer_ajuste_particion].part_fit = fit;
            strcat(mbr_particion.mbr_partition[primer_ajuste_particion].part_name,name);
            mbr_particion.mbr_partition[primer_ajuste_particion].part_size = espacion_nueva_particion;
            mbr_particion.mbr_partition[primer_ajuste_particion].part_start = inicio_particion;
            mbr_particion.mbr_partition[primer_ajuste_particion].part_status = 'A';
            mbr_particion.mbr_partition[primer_ajuste_particion].part_type = tipo;

            //CREACION DE LA PARTICION EXTENDIDA

            EBR extendida_part;
            extendida_part.part_fit = fit;
            strcat(extendida_part.part_name,name);
            extendida_part.part_size = espacion_nueva_particion;
            extendida_part.part_start = inicio_particion;
            extendida_part.part_status = 'A';
            extendida_part.part_next = inicio_particion + 1 ;

            //printf("INICIO: %i",mbr_particion.mbr_partition[primer_ajuste_particion].part_start);

            FILE *disco;
            disco = fopen(path_completo,"rb+");
            //ESCRITURA DEL MBR
            fseek(disco,0,SEEK_SET);
            fwrite(&mbr_particion,sizeof(MBR),1,disco);
            fclose(disco);
            //ESCRITURA DE LA PARTICION
            disco = fopen(path_completo,"rb+");
            fseek(disco,mbr_particion.mbr_partition[primer_ajuste_particion].part_start,SEEK_SET);
            fwrite(&extendida_part,sizeof(EBR),1,disco);
            fclose(disco);
        }
    }
    else
    {
        printf("\n\n  :::::::: IMPOSIBLE CREAR LA PARTICION:   %s   :::::::: \n\n",name);
    }
 }

void MOUNT_C(char *path, char* name)
{

}

void UNMOUNT(char *id_n)
{

}
//////////////////////////////////
//////  METODOS DE REPORTE  //////
//////////////////////////////////


void listar_mbr(char *path)
{
    FILE *disco;
    disco = fopen(path,"rb");
    MBR mbr_particion;
    Particion PART;

    fseek(disco,0,SEEK_SET);
    fread(&mbr_particion,sizeof(MBR),1,disco);
    fclose(disco);

    //printf("\nNOMBRE: %i",sizeof(mbr_particion));

    disco = fopen(path,"rb");
    fseek(disco,mbr_particion.mbr_partition[0].part_start,SEEK_SET);
    fread(&PART,sizeof(Particion),1,disco);
    fclose(disco);

    //printf("\n::::PARTE 1 - nombre: %s",PART.part_name);
    //printf("\n::::PARTE 1 - tamano bytes: %i",PART.part_size);
    //printf("\n::::PARTE 1 - status: %c",PART.part_status);
    //printf("\n::::PARTE 1 - tipo: %c",PART.part_type);
    //printf("\n::::PARTE 1: %s",PART.part_size);

    //fread(&PART,mbr_particion.mbr_partition[0].part_start,1,disco);
    //printf("\n\nPRIMERA_PARTICION: %s",PART.part_name);

    time_t t;
    struct tm *tm;
    char fechayhora[100];
    t=mbr_particion.mbr_fecha_creacion;
    tm=localtime(&t);
    strftime(fechayhora, 100, "%d/%m/%Y", tm);

    printf("\n\n::::MBR DE DISCO::::: \n");
    printf("\nParticion: %d Bytes\n",mbr_particion.mbr_tamano);
    printf("Fecha de creacion: %s\n", fechayhora);
    printf("Identificador del disco: %i\n",mbr_particion.mbr_disk_signature);
    printf("\nParticion 1: %s - %c - %i ",mbr_particion.mbr_partition[0].part_name,mbr_particion.mbr_partition[0].part_type,mbr_particion.mbr_partition[0].part_start);
    printf("\nParticion 2: %s - %c - %i " ,mbr_particion.mbr_partition[1].part_name,mbr_particion.mbr_partition[1].part_type,mbr_particion.mbr_partition[1].part_start);
    printf("\nParticion 3: %s - %c - %i ",mbr_particion.mbr_partition[2].part_name,mbr_particion.mbr_partition[2].part_type,mbr_particion.mbr_partition[2].part_start);
    printf("\nParticion 4: %s - %c - %i \n\n",mbr_particion.mbr_partition[3].part_name,mbr_particion.mbr_partition[3].part_type,mbr_particion.mbr_partition[3].part_start);
}


Lista_comando *lista_tokens;

int main()
{
    //inicializarlista(&lista_tokens);
    //inicializar_lista(&lista_tokens);
    //insertar(&lista_tokens,"FLOW","HOLI",1,1);
    //imprimir(lista_tokens->primero);

    MKDISK_C(100000,'b',"/home/kevin/chatos/como_son","chatos.dsk");
    //listar_mbr("/home/kevin/chatos/como_son/chatos.dsk");
    //RMDISK_C("/home/kevin/chatos/como_son/chatos.dsk");
    FDISK_C(100,'b',"/home/kevin/chatos/como_son/chatos.dsk",'p',"BF",'fd',"JULIOS",0);
    FDISK_C(100,'b',"/home/kevin/chatos/como_son/chatos.dsk",'e',"BF",'fd',"JULIOS2",0);
    FDISK_C(100,'b',"/home/kevin/chatos/como_son/chatos.dsk",'e',"BF",'fd',"JULIOS3",0);
    FDISK_C(100,'b',"/home/kevin/chatos/como_son/chatos.dsk",'e',"BF",'fd',"JULIOS4",0);
    FDISK_C(100,'b',"/home/kevin/chatos/como_son/chatos.dsk",'e',"BF",'fd',"JULIOS5",0);
    //FDISK_C(100,'b',"/home/kevin/chatos/como_son/chatos.dsk",'e',"BF",' ',"JULIOS5",0);
    listar_mbr("/home/kevin/chatos/como_son/chatos.dsk");
    return 0;
}
