#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAM_PRODS 5
#define TAM_MOVS 7

#define ERROR_ARC 1
#define ERROR_ARCH 2

#define PRODS_ARC 1
#define MOVS_ARC 2

#define TODO_OK 0

typedef struct
{
    int codigo;
    char descripcion[51];
    int cantidad;
}Producto;

//PROTOTIPOS
int generarArchivos(const char* nomProds, const char* nomMovs);
int actualizarProductos(const char* nomProds, const char* nomMovs);
void acumularDuplicados(FILE* movsA, Producto* mov, FILE* prodsTmpA);
int mostrarProductos(const char* nombProds);
void cambiarExtension(const char* nombreOrig, char* nombreDest, const char* ext);

///NOMBRE DEL PROGRAMA PARAMETRO 1 PARAMETRO 2
///clase_2020-05-05 productos.dat movimientos.dat
///  argv[0]         argv[1]         argv[2]

int main(int argc, char* argv[])
{
    if(argc != 3)
    {
        puts("Cantidad incorrecta de argumentos");
        return ERROR_ARC;
    }

        int ret = generarArchivos(argv[PRODS_ARC], argv[MOVS_ARC]); //ret ES RETORNO DEL VALOR BOOLEANO

        if(ret != TODO_OK)
            return ret;

        ret = actualizarProductos(argv[PRODS_ARC], argv[MOVS_ARC]);

        if(ret != TODO_OK)
            return ret;

        ret = mostrarProductos(argv[PRODS_ARC]);

        if(ret != TODO_OK)
            return ret;

    system ("pause");
    return 0;
}

int generarArchivos(const char* nomProds, const char* nomMovs)
{
    static Producto prodsV[]= //NO ACLARO EL TAMAÑO YA QUE EL MISMO PROGRAMA SABRA DE CUANTO SERA
    {
        {2, "Mandarina", 40}, {4, "Banana", 100}, {7, "Pera", 50}, {9, "Manzana", 70}, {10, "Naranja", 90}
    };

    static Producto movsV[]=
    {
        {1, "Kiwi", 20}, {1, "Kiwi", -10},{7, "Pera", 20}, {7, "Pera", -10}, {8, "Uva", 40},{10, "Naranja", -40}, {11, "Mango", 50}
    };

        FILE* prodsA = fopen(nomProds, "wb");

            if(!prodsA)
            {
                puts("ERROR ABRIENDO ARCHIVO.");
                return ERROR_ARCH;
            }

            fwrite(prodsV, sizeof(Producto), 5, prodsA);
            fclose(prodsA);

                FILE* movsA = fopen(nomMovs, "wb");

                if(!movsA)
                {
                    puts("ERROR ABRIENDO ARCHIVO.");
                    return ERROR_ARCH;
                }

                    fwrite(movsV,sizeof(Producto), 7, movsA);
            fclose(movsA);

    return TODO_OK;
}
int actualizarProductos(const char* nomProds, const char* nomMovs)
{
    FILE* prodsA = fopen(nomProds,"rb"); //ABRO ARCHIVO DE PRODUCTOS

        if(!prodsA)
        {
            puts("ERROR ABRIENDO ARCHIVO.");
            return ERROR_ARCH;
        }
            FILE* movsA = fopen(nomMovs, "rb"); //ABRO ARCHIVO DE MOVIMIENTOS

        if(!movsA)
        {
            puts("ERROR ABRIENDO ARCHIVO.");
            return ERROR_ARCH;
        }

            char nomProdsTmp[501];

            cambiarExtension(nomProds, nomProdsTmp, "tmp");

                FILE* prodsTmpA = fopen(nomProdsTmp,"wb"); //ABRO ARCHIVO TEMPORAL PARA LA CARGA DE DATOS

                    if(!prodsTmpA)
                    {
                        puts("ERROR ABRIENDO ARCHIVO.");
                        return ERROR_ARCH;
                    }

                    Producto prod, mov;

                        int cmp; //COMPARADOR

                        fread(&prod, sizeof(Producto), 1, prodsA);
                        fread(&mov, sizeof(Producto), 1 , movsA);

                        while(!feof(prodsA) && !feof(movsA))
                        {
                            cmp= prod.codigo - mov.codigo;

                            if(cmp == 0) ///SIGNIFICA QUE HUBO MOVIMIENTO DE ESE PRODUCTO, POR LO QUE SE ACTUALIZA.
                            {
                                prod.cantidad += mov.cantidad;
                                fread(&mov, sizeof(Producto), 1, movsA);
                            }

                            if(cmp < 0)
                            {
                                fwrite(&prod, sizeof(Producto), 1, prodsTmpA);
                                fread(&prod, sizeof(Producto), 1, prodsA);
                            }

                            if(cmp > 0) ///SIGNIFICA QUE HAY UN PRODUCTO NUEVO.
                            {
                                acumularDuplicados(movsA, &mov, prodsTmpA);
                            }
                        }

                            while(!feof(prodsA))
                            {
                                fwrite(&prod, sizeof(Producto), 1 , prodsTmpA);
                                fread(&prod, sizeof(Producto), 1, prodsA);

                            }
                            while(!feof(movsA))
                            {
                               acumularDuplicados(movsA, &mov, prodsTmpA);
                            }

        fclose(prodsA);
        fclose(movsA);
        fclose(prodsTmpA);

        remove(nomProds); //NO ES POSIBLE ELIMINAR SIN HABER CERRADO EL ARCHVO
        rename(nomProdsTmp, nomProds); //CAMBIO EL NOMBRE DEL ARCHIVO TEMPORAL POR EL ORIGINAL

        return TODO_OK;

}

void acumularDuplicados(FILE* movsA, Producto* mov, FILE* prodsTmpA)
{
    Producto movDup;
        fread(&movDup, sizeof(Producto), 1, movsA);

            while(!feof(movsA) && mov->codigo == movDup.codigo)
            {
                mov->cantidad += movDup.cantidad;
                fread(&movDup, sizeof(Producto), 1, movsA);
            }

                    fwrite(mov, sizeof(Producto), 1, prodsTmpA);

                        if(!feof(movsA))
                            *mov=movDup;
}

void cambiarExtension(const char* nombreOrig, char* nombreDest, const char* ext)
{
    strcpy(nombreDest, nombreOrig);
    char* punto = strchr(nombreDest, '.'); //BUSCO HASTA EL CARACTER PUNTO Y GUARDO LA POSICION DE MEMORIA

        *(punto+1) = '\0'; //DESREFERENCIO EL PUNTERO CON * Y REEMPLAZO EN DICHA POSICION DE MEMORIA, EL CARACTER NULO
        strcat(nombreDest, ext); //CONCATENO AMBAS CADENAS
}

int mostrarProductos(const char* nomProds)
{
    FILE* prodsA = fopen(nomProds, "rb");

        if(!prodsA)
        {
            puts("ERROR ABRIENDO ARCHIVO.");
            return ERROR_ARCH;
        }
            Producto prod;
            fread(&prod, sizeof(Producto), 1, prodsA);

                while(!feof(prodsA))
                {
                    printf("[%d, %s, %d]\n", prod.codigo, prod.descripcion, prod.cantidad);
                    fread(&prod, sizeof(Producto), 1, prodsA);
                }

    fclose(prodsA);

    return TODO_OK;
}
