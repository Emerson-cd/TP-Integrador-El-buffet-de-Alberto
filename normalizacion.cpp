
#include <iostream>
#include <cstdio>
#include <cstring>
using namespace std;

struct Mozo {
    int idMozo;
    char nombre[50];
    char password[20];
    float totalComision;
};

struct ComandaHistorica {
    char fecha[11];
    char nombreMozo[50];
    int codigoProducto;
    int cantidad;
    float comision;
};

struct Producto {
    int codigo;
    char descripcion[50];
    float precio;
    int stockActual;
};

struct Comanda {
    int idMozo;
    int codigoProducto;
    int cantidad;
    float comision;
};

struct Nodo {
    Comanda info;
    Nodo* sig;
};

int BuscarIdMozo(const char NombreMozo[]) {
    Mozo mozo;
    FILE* archivoMozos = fopen("mozos.dat", "rb");

    if (archivoMozos == nullptr) {
        return -1;
    }

    while (fread(&mozo, sizeof(Mozo), 1, archivoMozos) == 1) {
        if (strcmp(mozo.nombre, NombreMozo) == 0) {
            int id = mozo.idMozo;
            fclose(archivoMozos);
            return id;
        }
    }

    fclose(archivoMozos);
    return -1;
}

Nodo* crearNodo(Comanda valor) {
    Nodo* nuevoNodo = new Nodo();
    nuevoNodo->info = valor;
    nuevoNodo->sig = NULL;
    return nuevoNodo;
}

Nodo* insertarOrdenado(Nodo* inicio, Comanda valor) {
    Nodo* nuevoNodo = crearNodo(valor);

    int idNuevo = valor.idMozo;

    if (inicio == NULL ||
        idNuevo < inicio->info.idMozo) {
        nuevoNodo->sig = inicio;
        return nuevoNodo;
    }

    Nodo* actual = inicio;

    while (actual->sig != NULL &&
           actual->sig->info.idMozo <= idNuevo) {
        actual = actual->sig;
    }

    nuevoNodo->sig = actual->sig;
    actual->sig = nuevoNodo;

    return inicio;
}

void escribirArchivoComandas(Nodo* inicio, const char nombreArchivo[]) {
    FILE* archivo = fopen(nombreArchivo, "wb");

    if (archivo == nullptr) {
        cout << "Error al crear " << nombreArchivo << endl;
        return;
    }

    Nodo* actual = inicio;

    while (actual != NULL) {
        fwrite(&actual->info, sizeof(Comanda), 1, archivo);
        actual = actual->sig;
    }

    fclose(archivo);
}

void liberarLista(Nodo* inicio) {
    while (inicio != NULL) {
        Nodo* auxiliar = inicio;
        inicio = inicio->sig;
        delete auxiliar;
    }
}

// Lista de fechas procesadas
struct NodoFecha {
    char fecha[11];
    NodoFecha* sig;
};

bool fechaProcesada(NodoFecha* inicio, const char fecha[]) {
    while (inicio != NULL) {
        if (strcmp(inicio->fecha, fecha) == 0) {
            return true;
        }
        inicio = inicio->sig;
    }
    return false;
}

void agregarFecha(NodoFecha*& inicio, const char fecha[]) {
    NodoFecha* nuevo = new NodoFecha();
    strcpy(nuevo->fecha, fecha);
    nuevo->sig = inicio;
    inicio = nuevo;
}

void liberarFechas(NodoFecha* inicio) {
    while (inicio != NULL) {
        NodoFecha* auxiliar = inicio;
        inicio = inicio->sig;
        delete auxiliar;
    }
}

Comanda ConvertirComandaHaComanda(ComandaHistorica comandaH) {
    Comanda comanda;
    comanda.cantidad = comandaH.cantidad;
    comanda.codigoProducto = comandaH.codigoProducto;
    comanda.comision = comandaH.comision;
    comanda.idMozo = BuscarIdMozo(comandaH.nombreMozo);
    return comanda;
}

void actualizarInventario(ComandaHistorica comanda, FILE* archivo) {
    Producto productoActual;

    rewind(archivo);

    while (fread(&productoActual, sizeof(Producto), 1, archivo) == 1) {
        if (productoActual.codigo == comanda.codigoProducto) {

            productoActual.stockActual -= comanda.cantidad;

            fseek(archivo, -(long)sizeof(Producto), SEEK_CUR);

            fwrite(&productoActual, sizeof(Producto), 1, archivo);

            break;
        }
    }
}

void MigrarComandas() {
    FILE* archivo = fopen("comandas_historicas.dat", "rb");
    FILE* InventarioA = fopen("inventario.dat", "rb+");

    if (archivo == nullptr || InventarioA == nullptr) {
        cout << "Error al abrir comandas_historicas.dat o inventario.dat" << endl;

        if (archivo != nullptr) fclose(archivo);
        if (InventarioA != nullptr) fclose(InventarioA);

        return;
    }

    NodoFecha* fechas = NULL;
    ComandaHistorica ComandaH;

    // Primera pasada: identificar las fechas y actualizar inventario
    while (fread(&ComandaH, sizeof(ComandaHistorica), 1, archivo) == 1) {
        if (!fechaProcesada(fechas, ComandaH.fecha)) {
            agregarFecha(fechas, ComandaH.fecha);
        }

        actualizarInventario(ComandaH, InventarioA);
    }

    fclose(InventarioA);

    // Procesar cada fecha por separado
    NodoFecha* fechaActual = fechas;

    while (fechaActual != NULL) {
        Nodo* inicio = NULL;

        rewind(archivo);

        // Reunir las comandas de esta fecha
        while (fread(&ComandaH, sizeof(ComandaHistorica), 1, archivo) == 1) {
            if (strcmp(ComandaH.fecha, fechaActual->fecha) == 0) {
                inicio = insertarOrdenado(
                    inicio,
                    ConvertirComandaHaComanda(ComandaH)
                );
            }
        }

        // Construir nombre del archivo
        char nombreArchivo[50] = "comandas_";
        strcat(nombreArchivo, fechaActual->fecha);
        strcat(nombreArchivo, ".dat");

        // Escribir archivo ordenado
        escribirArchivoComandas(inicio, nombreArchivo);

        cout << "Archivo generado: " << nombreArchivo << endl;

        liberarLista(inicio);

        fechaActual = fechaActual->sig;
    }

    liberarFechas(fechas);
    fclose(archivo);
}

int main() {
    MigrarComandas();

    return 0;
}
