#include <cstdio>
#include <cstring>

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

struct Nodo {
    Mozo info;
    Nodo* sig;
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
Nodo* buscarMozoPorNombre(Nodo* inicio, const char nombre[]) {
    Nodo* actual = inicio;
    while (actual != NULL) {
        if (strcmp(actual->info.nombre, nombre) == 0) {
            return actual;
        }
        actual = actual->sig;
    }
    return NULL;
}

Nodo* agregarAlFinal(Nodo* inicio, Mozo mozo) {
    Nodo* nuevoNodo = new Nodo();
    nuevoNodo->info = mozo;
    nuevoNodo->sig = NULL;

    if (inicio == NULL) {
        inicio = nuevoNodo;
    } else {
        Nodo* actual = inicio;
        while (actual->sig != NULL) {
            actual = actual->sig;
        }
        actual->sig = nuevoNodo;
    }
    return inicio;
}


 
int main() {
    FILE* archivoHistorico = fopen("comandas_historicas.dat", "rb");
    if (archivoHistorico == NULL) {
        printf("Error: no se pudo abrir comandas_historicas.dat.\n");
        return 1;
    }
 
    Nodo* listaMozos = NULL;
    int cantidadMozos = 0;
 
    // Una sola pasada: por cada venta, busco al mozo; si no esta, lo creo.
    ComandaHistorica historica;
    while (fread(&historica, sizeof(ComandaHistorica), 1, archivoHistorico) == 1) {
        Nodo* nodoMozo = buscarMozoPorNombre(listaMozos, historica.nombreMozo);
 
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
 
    Nodo* actual = listaMozos;
    while (actual != NULL) {
        fwrite(&actual->info, sizeof(Mozo), 1, archivoMozos);
        actual = actual->sig;
    }
    fclose(archivoMozos);
 
    // Libero la memoria de la lista
    while (listaMozos != NULL) {
        Nodo* temp = listaMozos;
        listaMozos = listaMozos->sig;
        delete temp;
    }
 
    printf("Normalizacion terminada. Mozos registrados: %d.\n", cantidadMozos);
    return 0;
}
