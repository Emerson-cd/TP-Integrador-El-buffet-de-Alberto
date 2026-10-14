
#include <iostream>
#include <stdio.h>

using namespace std;

struct Comanda
{
    int idMozo;
    int codigoProducto;
    int cantidad;
    float comision;
};

int main(){
    char nombreArchivo [50];
    cout << "ingrese el nombre del archivo semanal: ";
    cin >> nombreArchivo;

    FILE* archivo = fopen (nombreArchivo, "rb");

    if (archivo == NULL) {
        cout << "Nose se pudo abrir el archivo. "<<endl;
        return 1;
    }

    Comanda comanda;
    bool hayMozoActual = false;
    int idMozoActual = 0;
    int productosMozo = 0;
    float comisionMozo = 0;
    int productosBuffet = 0;
    
    while (fread (&comanda, sizeof(comanda), 1, archivo)==1){
        if(!hayMozoActual){
            idMozoActual = comanda.idMozo;
            hayMozoActual = true; 
        }
        else if (comanda.idMozo != idMozoActual){
            cout << "Mozo "<<idMozoActual<<endl;
            cout << " Productos vendidos: "<<productosMozo<< endl;
            cout << " Comision a pagar: $" <<comisionMozo <<endl;

            idMozoActual = comanda.idMozo;
            productosMozo = 0;
            comisionMozo = 0;
        }
        productosMozo = productosMozo + comanda.cantidad;
        comisionMozo = comisionMozo + comanda.comision;
        productosBuffet = productosBuffet + comanda.cantidad;
    }
    if (hayMozoActual){
        cout << "Mozo "<< idMozoActual << endl;
        cout << " Productos vendidos: " << productosMozo << endl;
        cout << " Comisiona a pagar: $" << comisionMozo << endl;
    }
    else {
        cout << "El archivo no contiene comandas." << endl;
    }
    cout << "Total de productos vendidos por el buffet: " << productosBuffet << endl;

    fclose(archivo);
    return 0;
}
