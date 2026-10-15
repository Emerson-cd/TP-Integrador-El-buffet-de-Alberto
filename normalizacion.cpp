
#include <iostream>
#include <cstdio>
#include <cstring>
using namespace std;

const int K = 5;

struct ComandaHistorica {
    char fecha[11];
    char nombreMozo[50];
    int codigoProducto;
    int cantidad;
    float comision;
};

struct Mozo {
    int idMozo;
    char nombre[50];
    char password[20];
    float totalComision;
};

struct NodoMozo {
    Mozo info;
    NodoMozo* sig;
};

void codificarClave(char clave[]) {
    int i = 0;
    while (clave[i] != '\0') {
        clave[i] = clave[i] + K;
        i++;
    }
}

// Convierte un numero a texto sin usar sprintf (ej: 12 -> "12")
void numeroATexto(int numero, char texto[]) {
    char alReves[20];
    int cantidadDigitos = 0;

    if (numero == 0) {
        alReves[cantidadDigitos] = '0';
        cantidadDigitos++;
    }
    while (numero > 0) {
        alReves[cantidadDigitos] = '0' + numero % 10;
        numero = numero / 10;
        cantidadDigitos++;
    }

    int i = 0;
    while (i < cantidadDigitos) {
        texto[i] = alReves[cantidadDigitos - 1 - i];
        i++;
    }
    texto[cantidadDigitos] = '\0';
}

// Busqueda en la lista: devuelve el nodo del mozo o NULL si no esta
NodoMozo* buscarMozoPorNombre(NodoMozo* inicio, const char nombre[]) {
    NodoMozo* actual = inicio;
    while (actual != NULL) {
        if (strcmp(actual->info.nombre, nombre) == 0) {
            return actual;
        }
        actual = actual->sig;
    }
    return NULL;
}

NodoMozo* agregarAlFinal(NodoMozo* inicio, Mozo mozo) {
    NodoMozo* nuevoNodo = new NodoMozo();
    nuevoNodo->info = mozo;
    nuevoNodo->sig = NULL;

    if (inicio == NULL) {
        inicio = nuevoNodo;
    } else {
        NodoMozo* actual = inicio;
        while (actual->sig != NULL) {
            actual = actual->sig;
        }
        actual->sig = nuevoNodo;
    }
    return inicio;
}

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

struct NodoComanda {
    Comanda info;
    NodoComanda* sig;
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

NodoComanda* crearNodo(Comanda valor) {
    NodoComanda* nuevoNodo = new NodoComanda();
    nuevoNodo->info = valor;
    nuevoNodo->sig = NULL;
    return nuevoNodo;
}

NodoComanda* insertarOrdenado(NodoComanda* inicio, Comanda valor) {
    NodoComanda* nuevoNodo = crearNodo(valor);

    int idNuevo = valor.idMozo;

    if (inicio == NULL ||
        idNuevo < inicio->info.idMozo) {
        nuevoNodo->sig = inicio;
        return nuevoNodo;
    }

    NodoComanda* actual = inicio;

    while (actual->sig != NULL &&
           actual->sig->info.idMozo <= idNuevo) {
        actual = actual->sig;
    }

    nuevoNodo->sig = actual->sig;
    actual->sig = nuevoNodo;

    return inicio;
}

void escribirArchivoComandas(NodoComanda* inicio, const char nombreArchivo[]) {
    FILE* archivo = fopen(nombreArchivo, "wb");

    if (archivo == nullptr) {
        cout << "Error al crear " << nombreArchivo << endl;
        return;
    }

    NodoComanda* actual = inicio;

    while (actual != NULL) {
        fwrite(&actual->info, sizeof(Comanda), 1, archivo);
        actual = actual->sig;
    }

    fclose(archivo);
}

void liberarLista(NodoComanda* inicio) {
    while (inicio != NULL) {
        NodoComanda* auxiliar = inicio;
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
        NodoComanda* inicio = NULL;

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
    FILE* archivoHistorico = fopen("comandas_historicas.dat", "rb");
    if (archivoHistorico == NULL) {
        printf("Error: no se pudo abrir comandas_historicas.dat.\n");
        return 1;
    }

    NodoMozo* listaMozos = NULL;
    int cantidadMozos = 0;

    // Una sola pasada: por cada venta, busco al mozo; si no esta, lo creo.
    ComandaHistorica historica;
    while (fread(&historica, sizeof(ComandaHistorica), 1, archivoHistorico) == 1) {
        NodoMozo* nodoMozo = buscarMozoPorNombre(listaMozos, historica.nombreMozo);

        if (nodoMozo == NULL) {
            Mozo nuevo;
            cantidadMozos++;
            nuevo.idMozo = cantidadMozos;
            strcpy(nuevo.nombre, historica.nombreMozo);
            nuevo.totalComision = 0.0f;

            // Clave inicial: el numero de mozo como texto, ya codificado.
            numeroATexto(nuevo.idMozo, nuevo.password);
            codificarClave(nuevo.password);

            listaMozos = agregarAlFinal(listaMozos, nuevo);
            nodoMozo = buscarMozoPorNombre(listaMozos, historica.nombreMozo);
        }

        nodoMozo->info.totalComision += historica.comision;
    }
    fclose(archivoHistorico);

    if (listaMozos == NULL) {
        printf("No hay comandas historicas para procesar.\n");
        return 0;
    }

    // Recorro la lista y la escribo en mozos.dat
    FILE* archivoMozos = fopen("mozos.dat", "wb");
    if (archivoMozos == NULL) {
        printf("Error: no se pudo crear mozos.dat.\n");
        return 1;
    }

    NodoMozo* actual = listaMozos;
    while (actual != NULL) {
        fwrite(&actual->info, sizeof(Mozo), 1, archivoMozos);
        actual = actual->sig;
    }
    fclose(archivoMozos);

    // Libero la memoria de la lista
    while (listaMozos != NULL) {
        NodoMozo* temp = listaMozos;
        listaMozos = listaMozos->sig;
        delete temp;
    }
    MigrarComandas();
    printf("Normalizacion terminada. Mozos registrados: %d.\n", cantidadMozos);
    return 0;
}
