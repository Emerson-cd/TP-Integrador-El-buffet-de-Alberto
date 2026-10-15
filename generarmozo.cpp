
#include <iostream>
#include <cstdio>
using namespace std;

struct Mozo {
    int idMozo;
    char nombre[50];
    char password[20];
    float totalComision;
};

int main() {
    Mozo mozos[] = {
        {1, "Sofia Ramirez", "1234", 0},
        {2, "Bruno Diaz", "1234", 0},
        {3, "Carla Lopez", "1234", 0},
        {4, "Diego Sosa", "1234", 0},
        {5, "Emma Ruiz", "1234", 0}
    };

    float comisiones[] = {
        300, 240, 320, 240, 180,
        450, 150, 320, 420, 320,
        120, 450, 450, 320, 360,
        160, 420, 360, 450, 160,
        150, 210, 640, 160, 180,
        300, 120, 450, 160, 210
    };

    int ids[] = {
        1, 2, 3, 4, 5,
        1, 2, 3, 4, 5,
        1, 2, 3, 5, 1,
        4, 2, 3, 5, 2,
        3, 1, 4, 1, 4,
        5, 5, 2, 4, 3
    };

    for (int i = 0; i < 30; i++) {
        mozos[ids[i] - 1].totalComision += comisiones[i];
    }

    FILE* archivo = fopen("mozos.dat", "wb");

    if (archivo == NULL) {
        cout << "Error al crear mozos.dat" << endl;
        return 1;
    }

    fwrite(mozos, sizeof(Mozo), 5, archivo);
    fclose(archivo);

    cout << "mozos.dat generado correctamente" << endl;

    return 0;
}
