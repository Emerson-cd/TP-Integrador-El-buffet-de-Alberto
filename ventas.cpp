#include <iostream>
#include <cstdio>
#include <cstring>
using namespace std;



struct Mozo{
    int idMozo;
    char nombre[50];
    char password[20];
    float totalComision
};



struct Producto{
    int codigo;
    char descripcion[50];
    float precio;
    int stockActual
};



struct Comanda {
    int idMozo;
    int codigoProducto;
    int cantidad;
    float comision
};



void encriptarClave(char clave[]){
    int contador = 0;
    while (clave[contador] != 0){
        clave[contador] = clave[contador] + 5;
        contador++;
    }
}



bool revisarMozo(int idIngresado, char claveIngresada[]) {
    FILE* archivoMozos;
    archivoMozos = fopen("mozos.dat", "rb");
    if (archivoMozos == NULL) {
        cout << "Error: No se pudo abrir el archivo de mozos." << endl;
        return false;
    }
    Mozo unMozo;
    bool loEncontre = false;
    while (fread(&unMozo, sizeof(Mozo), 1, archivoMozos) == 1) {
        if (unMozo.idMozo == idIngresado) {
            if (strcmp(unMozo.password, claveIngresada) == 0) {
                loEncontre = true;
            }
        }
    }
    fclose(archivoMozos);
    return loEncontre;
}




bool procesarVenta(int codigoProd, int cantidadPedida, float &comisionCalculada) {
    FILE* archivoInventario;
    archivoInventario = fopen("inventario.dat", "r+b"); 
    if (archivoInventario == NULL) {
        cout << "Error: No se encontro el inventario." << endl;
        return false;
    }
    Producto unProducto;
    bool sePudoVender = false;
    while (fread(&unProducto, sizeof(Producto), 1, archivoInventario) == 1) {
        if (unProducto.codigo == codigoProd) {
            if (unProducto.stockActual >= cantidadPedida) {
                unProducto.stockActual = unProducto.stockActual - cantidadPedida;
                float totalPlata = unProducto.precio * cantidadPedida;
                comisionCalculada = totalPlata * 0.10f;
                fseek(archivoInventario, -sizeof(Producto), SEEK_CUR);
                fwrite(&unProducto, sizeof(Producto), 1, archivoInventario);

                sePudoVender = true;
            }
        }
    }

    fclose(archivoInventario);
    return sePudoVender;
}



void ordenarVentas(char nombreDelArchivo[]) {
    FILE* archivoVentas;
    archivoVentas = fopen(nombreDelArchivo, "rb");
    if (archivoVentas == NULL) {
        return;
    }
    fseek(archivoVentas, 0, SEEK_END);
    int pesoTotal = ftell(archivoVentas);
    int cantidadDeVentas = pesoTotal / sizeof(Comanda);
    rewind(archivoVentas);
    if (cantidadDeVentas > 1) {
        Comanda* arregloDinamico;
        arregloDinamico = new Comanda[cantidadDeVentas];
        fread(arregloDinamico, sizeof(Comanda), cantidadDeVentas, archivoVentas);
        fclose(archivoVentas);
        for (int i = 0; i < cantidadDeVentas - 1; i++) {
            for (int j = 0; j < cantidadDeVentas - 1 - i; j++) {
                
                if (arregloDinamico[j].idMozo > arregloDinamico[j+1].idMozo) {
                    Comanda auxiliar = arregloDinamico[j];
                    arregloDinamico[j] = arregloDinamico[j+1];
                    arregloDinamico[j+1] = auxiliar;
                }
            }
        }
        archivoVentas = fopen(nombreDelArchivo, "wb");
        fwrite(arregloDinamico, sizeof(Comanda), cantidadDeVentas, archivoVentas);
        fclose(archivoVentas);
        delete[] arregloDinamico;
    } else {
        fclose(archivoVentas);
    }
}



int main() {
    char fechaDelDia[11];
    char nombreArchivoDia[50];
    int opcionParaSeguir = 1;
    cout << "Hola! Ingrese la fecha de hoy (DD-MM-AAAA): ";
    cin >> fechaDelDia;
    sprintf(nombreArchivoDia, "comandas_%s.dat", fechaDelDia);
    FILE* archivoInicial = fopen(nombreArchivoDia, "ab");
    if (archivoInicial != NULL) {
        fclose(archivoInicial);
    }
    while (opcionParaSeguir == 1) {
        int idIngresado;
        char claveIngresada[20];
        cout << "\n--- Cargar nueva venta ---" << endl;
        cout << "Ingrese el ID del mozo: ";
        cin >> idIngresado;
        cout << "Ingrese la clave: ";
        cin >> claveIngresada;
        encriptarClave(claveIngresada);
        if (revisarMozo(idIngresado, claveIngresada) == true) {
            cout << "Mozo correcto!" << endl;
            int codigoProducto;
            int cantidadProducto;
            cout << "Ingrese el codigo del producto: ";
            cin >> codigoProducto;
            cout << "Ingrese la cantidad: ";
            cin >> cantidadProducto;
            float comisionParaElMozo = 0;
            if (procesarVenta(codigoProducto, cantidadProducto, comisionParaElMozo) == true) {
                Comanda nuevaComanda;
                nuevaComanda.idMozo = idIngresado;
                nuevaComanda.codigoProducto = codigoProducto;
                nuevaComanda.cantidad = cantidadProducto;
                nuevaComanda.comision = comisionParaElMozo;
                FILE* archivoDelDia = fopen(nombreArchivoDia, "ab");
                fwrite(&nuevaComanda, sizeof(Comanda), 1, archivoDelDia);
                fclose(archivoDelDia);

                cout << "Venta guardada. Comision para el mozo: $" << comisionParaElMozo << endl;
            } else {
                cout << "No se pudo hacer la venta (falta stock o no existe el codigo)." << endl;
            }

        } else {
            cout << "No podes cargar ventas, ID o clave incorrectos." << endl;
        }

        cout << "Quiere cargar otra venta? (Ponga 1 para Si, 0 para No): ";
        cin >> opcionParaSeguir;
    }
    cout << "Ordenando las ventas por mozo..." << endl;
    ordenarVentas(nombreArchivoDia);
    cout << "Fin del dia. Guardado correctamente!" << endl;

    return 0;
}
