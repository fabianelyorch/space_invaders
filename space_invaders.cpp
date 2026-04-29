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
