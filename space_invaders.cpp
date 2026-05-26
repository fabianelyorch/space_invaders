#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <windows.h>
typedef struct {
    int puntos; //Puntos al eliminar un enemigo
    bool estaActivo; //Vida del enemigo
    char simbolo; // Representacion en pantalla
} Enemigo;

typedef struct Proyectil {
    int posX, posY; //Coordenadas del proyectil
    struct Proyectil* siguiente; //Siguiente proyectil en la lista
} Proyectil;

typedef struct {
    Enemigo** horda; //Matriz dinamica de los enemigos
    int filas; // Filas de enmigos 
    int columnas; //Columnas de enemigos
    int navePos; //Posicion actual del jugador
    Proyectil* listaProyectiles; //Puntero al iniciar disparo de proyectiles
} Juego;

typedef struct{
    int filas;
    int columnas; //se guardan los valores del archivo para cargar el nivel
    int velocidad;
} configNivel;

void inicializarNivel(Juego* partida, int f, int c);
void graficos(int, int);
configNivel cargarNivel(const char*);//lee los valores del archivo para cargar el nivel
void dispararJugador(Proyectil** cabeza, int xActual, int yActual); //crea el proyectil
void liberarMemoria(Juego* partida);
void borrarProyectil(Proyectil** inicio, Proyectil* proyectilABorrar);
void detectarColisiones(Juego* partida);
void actualizarJuego(Juego* partida);

int main() {
    /*Juego miPartida;
    int filasConfig = 5;
    int columnasConfig = 10;

    inicializarNivel(&miPartida, filasConfig, columnasConfig);


    dispararJugador(&miPartida.listaProyectiles, miPartida.navePos, 20);

    liberarMemoria(&miPartida);

    return 0;
    */

    //ocultar cursor
   CONSOLE_CURSOR_INFO info = {1, FALSE};
   SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
   configNivel config = cargarNivel("archivo.txt");
   Juego miPartida;
   inicializarNivel(&miPartida, config.filas, config.columnas);//agarra filas y las columnas que se leyeron del archivo
   dispararJugador(&miPartida.listaProyectiles, miPartida.navePos, 20);
   liberarMemoria(&miPartida);


}

void inicializarNivel(Juego* partida, int f, int c) {
    partida->filas = f;
    partida->columnas = c;
    partida->navePos = c / 2;
    partida->listaProyectiles = NULL;

    partida->horda = (Enemigo**)malloc(f * sizeof(Enemigo*));
    
    for (int i = 0; i < f; i++) {
        partida->horda[i] = (Enemigo*)malloc(c * sizeof(Enemigo));
        
        for (int j = 0; j < c; j++) {
            partida->horda[i][j].estaActivo = true;
            partida->horda[i][j].simbolo = 'W';
            partida->horda[i][j].puntos = 10;
        }
    }
    printf("Matriz de %dx%d creada dinamicamente.\n", f, c);
}

void dispararJugador(Proyectil** cabeza, int xActual, int yActual) {
    Proyectil* nuevoProyectil = (Proyectil*)malloc(sizeof(Proyectil));
    
    if (nuevoProyectil != NULL) {
        nuevoProyectil->posX = xActual;
        nuevoProyectil->posY = yActual;
        
        nuevoProyectil->siguiente = *cabeza;
        *cabeza = nuevoProyectil;
        
        printf("Disparo generado en X:%d, Y:%d\n", xActual, yActual);
    }
}

void liberarMemoria(Juego* partida) {
    for (int i = 0; i < partida->filas; i++) {
        free(partida->horda[i]);
    }
    free(partida->horda);

    Proyectil* actual = partida->listaProyectiles;
    while (actual != NULL) {
        Proyectil* temporal = actual;
        actual = actual->siguiente;
        free(temporal);
    }
    printf("Memoria liberada correctamente.\n");
}

void borrarProyectil(Proyectil** inicio, Proyectil* proyectilABorrar) {
    if (*inicio == NULL || proyectilABorrar == NULL) return;

    if (*inicio == proyectilABorrar) {
        *inicio = proyectilABorrar->siguiente;
        free(proyectilABorrar);
        return;
    }

    Proyectil* actual = *inicio;
    while (actual->siguiente != NULL && actual->siguiente != proyectilABorrar) {
        actual = actual->siguiente;
    }

    if (actual->siguiente == proyectilABorrar) {
        actual->siguiente = proyectilABorrar->siguiente;
        free(proyectilABorrar);
    }
}
void detectarColisiones(Juego* partida) {
    Proyectil* aux = partida->listaProyectiles;
    while (aux != NULL) {
        Proyectil* proximo = aux->siguiente; 
        bool huboImpacto = false;

        if (aux->posY >= 0 && aux->posY < partida->filas &&
            aux->posX >= 0 && aux->posX < partida->columnas) {
            
            if (partida->horda[aux->posY][aux->posX].estaActivo) {
                partida->horda[aux->posY][aux->posX].estaActivo = false;
                printf("Enemigo eliminado en: %d, %d\n", aux->posX, aux->posY);
                borrarProyectil(&partida->listaProyectiles, aux);
                huboImpacto = true;
            }
        }

        if (!huboImpacto && aux->posY < 0) {
            borrarProyectil(&partida->listaProyectiles, aux);
        }
        aux = proximo;
    }
}
void actualizarJuego(Juego* partida) {
    Proyectil* p = partida->listaProyectiles;
    while (p != NULL) {
        p->posY--; // El proyectil sube
        p = p->siguiente;
    }
    detectarColisiones(partida);
}

void graficos(int x, int y){
    COORD coord;
    coord.X = x; // estructura de windows que representa la posicion en la consola
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord); // obtiene el control de la consola
}

configNivel cargarNivel(const char* archivo) {
    configNivel config = {5, 10, 3};  // valores default
    FILE* f = fopen(archivo, "r");
    
    if (f == NULL) {
        printf("No se encontro el archivo, usando config default.\n"); //funcion que sirve para leer los valores del archivo y cargar el nivel
        return config;
    }
    
    fscanf(f, "filas=%d\n", &config.filas);
    fscanf(f, "columnas=%d\n", &config.columnas);
    fscanf(f, "velocidad=%d\n", &config.velocidad);
    
    fclose(f);
    return config;
}
