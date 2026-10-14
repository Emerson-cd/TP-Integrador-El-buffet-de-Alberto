#include <iostream>
#include <cstdio>
#include <cstring>
using namespace std;

struct ComandaHistorica {
    char fecha[11]; // "DD-MM-AAAA"
    char nombreMozo[50]; // el nombre completo, repetido en cada venta
    int codigoProducto;
    int cantidad;
    float comision;
};

struct Producto {
    int codigo;
    char descripcion[50];
    float precio; int stockActual;
};

int main()
{   
    ComandaHistorica Comanda;
    FILE* archivo = fopen("comandas_historicas.dat", "rb");
    if (archivo != nullptr){
        if(fread(&Comanda, sizeof(Comanda), 1, archivo) == 1){
            cout << Comanda.nombreMozo << ' '<< Comanda.codigoProducto <<' ' << Comanda.fecha << endl;
        }
    }
    return 0;
}
