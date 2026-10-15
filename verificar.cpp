
#include <iostream>
#include <cstdio>
using namespace std;

struct Comanda {
    int idMozo;
    int codigoProducto;
    int cantidad;
    float comision;
};

int main() {
    char nombreArchivo[100];
    Comanda comanda;

    cout << "Ingrese el nombre del archivo: ";
    cin >> nombreArchivo;

    FILE* archivo = fopen(nombreArchivo, "rb");

    if (archivo == nullptr) {
        cout << "Error al abrir el archivo" << endl;
        return 1;
    }

    while (fread(&comanda, sizeof(Comanda), 1, archivo) == 1) {
        //cout << "Fecha: " << comanda. << endl;
        cout << "Mozo: " << comanda.idMozo << endl;
        cout << "Producto: " << comanda.codigoProducto << endl;
        cout << "Cantidad: " << comanda.cantidad << endl;
        cout << "Comision: " << comanda.comision << endl;
        cout << "------------------------" << endl;
    }

    fclose(archivo);

    return 0;
}
