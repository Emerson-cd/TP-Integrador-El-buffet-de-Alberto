#include <iostream>

#include <cstdio>
#include <cstring>
using namespace std;

struct comanda {
    int idmozo;
    int codigoproducto;
    int cantidad;
    float comision;
};

int main() {
File* diarios[7];
File* semanal;

comanda actual[7];
bool disponible[7];

int cantidaddias, semana, mes;

char fecha[11], nombrearchivo[40], nombresemanal[50];

cout<< "numero de semana: ";
cin>> semana;

cout<< "numero de mes: ";
cin>> mes;

cout<< " cantidad de plantillas diarias a unir (1 a 7): ";
cin>> cantidaddias;
//inicia los archivos y disponibilidad 
if (cantidaddias < 1 || cantidaddias > 7) {
    cout << "Error: La cantidad de plantillas diarias debe estar entre 1 y 7." << endl;
    return 1;
} 
// inicializar las variables
for (int i=0;i<cantidaddias;i++){
    diarios[i]=NULL;
    disponible[i]= false;
}
//abre las plantillas
for (int i=0; i<cantidaddias; i++){
 cout<< "fecha del dia "<< i+1<<" (dd/mm/aaaa): ";
 cin>> fecha;

 sprintf(nombrearchivo, "comandas_%s.txt", fecha);
 diarios[i] = fopen(nombrearchivo, "rb");
 if (!diarios[i]== NULL) {
     cout << "AVISO:NO EXISTE LA PLANTILLA " << nombrearchivo << ". SE CONTINUARA CON LOS DEMAS DIAS."<< endl;
        continue; 
     }
 
 //leer la primera comanda
 disponible[i]= fread(&actual[i], sizeof(comanda), 1, diarios[i]) == 1;

 if(!disponible[i] && ferror (diarios[i])){
    cout<< "ERROR AL LEER"<< nombrearchivo<<"."<<endl;
     for(int j=0 ; j< cantidaddias; j++){
        if(diarios[j] != NULL){
         fclose(diarios[j]);
        }
     }
     return 1;
 }
 if (!disponible[i]){
    cout<< "aviso: la plantilla "<< nombrearchivo<< " esta vacia."<<endl;
 }  
} 
sprintf(nombresemanal, "comandas_semana_s%d_mes_%02d.txt", semana, mes);
semanal = fopen(nombresemanal, "wb");
if (!semanal) {
    cout << "Error: No se pudo crear el archivo semanal." << endl;
    for (int i=0; i<cantidaddias; i++) {
       if(diarios[i]!=NULL){
         fclose(diarios[i]);
       }
    }
    return 1;
}
int totalcomandas=0,totalproductos=0;
float totalcomisiones=0;

while (true){
    int menor=-1;

    for(int i=0; i<cantidaddias; i++){
        if (disponible[i]){
            if (menor==-1 || actual[i].idmozo<actual[menor].idmozo){
                menor=i;
            }
        }
    }
    if (menor==-1) {
        break;
    }
    fwrite(&actual[menor], sizeof(comanda), 1, semanal);
    totalcomandas++;
    totalproductos += actual[menor].cantidad;
    totalcomisiones += actual[menor].comision;
    disponible[menor]= fread(&actual[menor], sizeof(comanda), 1, diarios[menor]) == 1;
}
fclose(semanal);
for( int i=0; i<cantidaddias; i++){
    fclose(diarios[i]);
}
    cout << " CIERRE SEMANAL ";
    cout << "Archivo semanal creado: " << nombresemanal << endl;
    cout << "Total de comandas: " << totalcomandas << endl;
    cout << "Total de productos vendidos: " << totalproductos << endl;
    cout << "Total de comisiones: $" << totalcomisiones << endl;
    return 0;
}
