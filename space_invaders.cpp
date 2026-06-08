#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <windows.h>
#include <string.h>

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
    Enemigo** horda;//Matriz dinamica de los enemigos
    int filas;// Filas de enmigos
    int columnas;//Columnas de enemigos
    int navePos;//Posicion actual del jugador
    Proyectil* listaProyectiles;//Puntero al iniciar disparo de proyectiles
} Juego;

typedef struct {
    int filas;
    int columnas; //se guardan los valores del archivo para cargar el nivel
    int velocidad;
} configNivel;

void inicializarNivel(Juego*, const char*, int);
void graficos(int, int);
configNivel cargarNivel(const char*, int);
void dispararJugador(Proyectil** cabeza, int xActual, int yActual); 
void liberarMemoria(Juego* partida);
void borrarProyectil(Proyectil** inicio, Proyectil* proyectilABorrar);
void detectarColisiones(Juego* partida);
void actualizarJuego(Juego* partida);
void dibujarJuego(Juego*);
void limpiarAnterior(Juego*, int);

int main() {
    CONSOLE_CURSOR_INFO info = {1, FALSE};
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);

    int NivelJuego = 1;
    Juego miPartida;

    printf("Que nivel desea jugar? ");
    if (scanf("%d", &NivelJuego) != 1) {
        NivelJuego = 1;
    }
    
    configNivel config = cargarNivel("archivo.txt", NivelJuego);
    inicializarNivel(&miPartida, "archivo.txt", NivelJuego);
    
    int filaNave = 20;

    Sleep(1000); 
    system("cls"); 
    for (int i = 0; i < miPartida.filas; i++) {
     graficos(0, i); 
     for (int j = 0; j < miPartida.columnas; j++){ 
          if (miPartida.horda[i][j].estaActivo){
               printf("%c", miPartida.horda[i][j].simbolo);
          } else{
             printf(" ");
          }
     }
    }
    
    bool juegoejecutado = true;
    
    // Bucle principal
    while(juegoejecutado){
        int naveAnterior = miPartida.navePos;
        
        // Controles del jugador
        if (GetAsyncKeyState(VK_LEFT) & 0x8000){
            if (miPartida.navePos > 0){ 
                miPartida.navePos--;
            }
        }
        if (GetAsyncKeyState(VK_RIGHT) & 0x8000){
            if (miPartida.navePos < config.columnas - 1){
                miPartida.navePos++;
            }
        }
        if (GetAsyncKeyState(VK_SPACE) & 0x8000){
            dispararJugador(&miPartida.listaProyectiles, miPartida.navePos, filaNave - 1);
        }
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000){
            juegoejecutado = false;
        }
        
        limpiarAnterior(&miPartida, naveAnterior); 
        actualizarJuego(&miPartida);               
        dibujarJuego(&miPartida);                  
        
        Sleep(60);
    }
    
    system("cls");
    printf("Game over\n");
    liberarMemoria(&miPartida);

    return 0;
}

void inicializarNivel(Juego* partida, const char* archivo, int nivel) {
    configNivel config = cargarNivel(archivo, nivel);
    partida->filas = config.filas;
    partida->columnas = config.columnas;
    partida->navePos = config.columnas / 2;
    partida->listaProyectiles = NULL;

    partida->horda = (Enemigo**)malloc(partida->filas * sizeof(Enemigo*));
    for (int i = 0; i < partida->filas; i++) {
        partida->horda[i] = (Enemigo*)malloc(partida->columnas * sizeof(Enemigo));
    }

    FILE* file = fopen(archivo, "r");
    if (file == NULL) {
        printf("Error: No se pudo abrir el archivo para cargar la horda.\n");
        return;
    }

    char linea[50];
    char buscarNivel[10];
    sprintf(buscarNivel, "Nivel %d", nivel);

    while (fgets(linea, sizeof(linea), file)) {
        linea[strcspn(linea, "\n")] = 0;
        linea[strcspn(linea, "\r")] = 0;
        if (strcmp(linea, buscarNivel) == 0) break;
    }

    fgets(linea, sizeof(linea), file); 
    fgets(linea, sizeof(linea), file); 
    fgets(linea, sizeof(linea), file); 

    for (int i = 0; i < partida->filas; i++) {
        for (int j = 0; j < partida->columnas; j++) {
            char c = fgetc(file);

            while ((c == '\n' || c == '\r') && c != EOF) {
                c = fgetc(file);
            }
            
            if (c == EOF) {
                partida->horda[i][j].estaActivo = false;
                partida->horda[i][j].simbolo = ' ';
                partida->horda[i][j].puntos = 0;
            } else if (c == 'W') {
                partida->horda[i][j].estaActivo = true;
                partida->horda[i][j].simbolo = 'W';
                partida->horda[i][j].puntos = 10 * nivel;
            } else {
                partida->horda[i][j].estaActivo = false;
                partida->horda[i][j].simbolo = ' ';
                partida->horda[i][j].puntos = 0;
            }
        }
    }

    fclose(file);
    printf("¡Nivel %d cargado exitosamente (%dx%d)!\n", nivel, partida->filas, partida->columnas);
}

void dispararJugador(Proyectil** cabeza, int xActual, int yActual) {
    Proyectil* nuevoProyectil = (Proyectil*)malloc(sizeof(Proyectil));
    
    if (nuevoProyectil != NULL) {
        nuevoProyectil->posX = xActual;
        nuevoProyectil->posY = yActual;
        nuevoProyectil->siguiente = *cabeza;
        *cabeza = nuevoProyectil;
        
        graficos(0, 23);
        printf("Ultimo Disparo -> X: %2d, Y: %2d          ", xActual, yActual);
        fflush(stdout);  // Asegurar que se imprime inmediatamente
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

void detectarColisiones(Juego* partida){
    Proyectil* aux = partida->listaProyectiles;
    while (aux != NULL) {
        Proyectil* proximo = aux->siguiente; 
        bool huboImpacto = false;

        if (aux->posY >= 0 && aux->posY < partida->filas &&
            aux->posX >= 0 && aux->posX < partida->columnas){
            
            if (partida->horda[aux->posY][aux->posX].estaActivo){
                partida->horda[aux->posY][aux->posX].estaActivo = false;
                
                graficos(aux->posX, aux->posY);
                printf(" ");
                
                graficos(0, 24);
                printf("¡Enemigo Eliminado! -> X: %2d, Y: %2d      ", aux->posX, aux->posY);
                
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
        // Evitamos borrar la nave si el proyectil pasa por la fila 20
        if (p->posY >= 0 && p->posY < 25 && p->posY != 20) {
            graficos(p->posX, p->posY);
            printf(" ");
        }
        
        p->posY--;
        p = p->siguiente;
    }
    detectarColisiones(partida);
}

void graficos(int x, int y){
    COORD coord;
    coord.X = x; 
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord); 
}

configNivel cargarNivel(const char* archivo, int nivel){
    configNivel config = {5, 10, 3};  
    FILE* f = fopen(archivo, "r");
    
    if (f == NULL){
        printf("No se encontro el archivo \n"); 
        return config;
    }
    char linea[50];
    char buscarNivel[10];
    sprintf(buscarNivel, "Nivel %d", nivel);
    while (fgets(linea, sizeof(linea), f)){
        linea[strcspn(linea, "\n")] = 0; 
        linea[strcspn(linea, "\r")] = 0;

        if (strcmp(linea, buscarNivel) == 0) {
            printf("Cargando el nivel %d...\n", nivel);
            break;
        }
    }
    
    fscanf(f, "filas=%d\n", &config.filas);
    fscanf(f, "columnas=%d\n", &config.columnas);
    fscanf(f, "velocidad=%d\n", &config.velocidad);
    
    fclose(f);
    return config;
}

void dibujarJuego(Juego* partida){
    Proyectil* p = partida->listaProyectiles;
    while (p != NULL) {
        if (p->posY >= 0 && p->posY < 25 && p->posX >= 0 && p->posX < partida->columnas){ 
            graficos(p->posX, p->posY);
            printf("|");
        }
        p = p->siguiente;
    }
    if (partida->navePos >= 0 && partida->navePos < partida->columnas){
        graficos(partida->navePos, 20);
        printf("A");
    }
}

void limpiarAnterior(Juego* partida, int naveAnterior){
    graficos(naveAnterior, 20);
    printf(" ");
}