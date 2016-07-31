#include <stdio.h>
#include <stdlib.h>

struct particion
{
    int dato;
};


void insertar(int dato){
    int cod = dato;
    scanf("%d",&cod);

    particion *nuevo;
    nuevo->dato = cod;
    getchar();
    FILE *fichero;
    fichero=fopen("secuencial.bin","ab+");

    if(fichero){
         fseek(fichero,0,SEEK_SET);
          fwrite(&nuevo, sizeof(particion),1,fichero);
        fclose(fichero);
    }

}


void leer(){
    particion getdatos;
    FILE *fichero;
    fichero=fopen("secuencial.bin","rb+");
    if(fichero){
        fseek(fichero , 0, SEEK_END);
        int largo=ftell(fichero)/sizeof(getdatos);
                fseek(fichero,0,SEEK_SET);

              int i=0;
              for(;i<largo;i++){
                fread(&getdatos,sizeof(getdatos),1,fichero);
                if(getdatos.dato!=-1)
                printf("%d",getdatos.dato);
        }

                fclose(fichero);
    }


}


int main()
{

}
