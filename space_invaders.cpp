#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

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

void inicializarNivel(Juego* partida, int f, int c);
void dispararJugador(Proyectil** cabeza, int xActual, int yActual);
void liberarMemoria(Juego* partida);

int main() {
    Juego miPartida;
    int filasConfig = 5;
    int columnasConfig = 10;

    inicializarNivel(&miPartida, filasConfig, columnasConfig);

    dispararJugador(&miPartida.listaProyectiles, miPartida.navePos, 20);

    liberarMemoria(&miPartida);

    return 0;
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
    printf("[OK] Matriz de %dx%d creada dinamicamente.\n", f, c);
}

void dispararJugador(Proyectil** cabeza, int xActual, int yActual) {
    Proyectil* nuevoProyectil = (Proyectil*)malloc(sizeof(Proyectil));
    
    if (nuevoProyectil != NULL) {
        nuevoProyectil->posX = xActual;
        nuevoProyectil->posY = yActual;
        
        nuevoProyectil->siguiente = *cabeza;
        *cabeza = nuevoProyectil;
        
        printf("[EVENTO] Disparo generado en X:%d, Y:%d\n", xActual, yActual);
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
    printf("[CLEAN] Memoria liberada correctamente.\n");
}
