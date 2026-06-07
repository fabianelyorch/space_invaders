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

void inicializarNivel(Juego*, const char*, int);
void graficos(int, int);
configNivel cargarNivel(const char*, int);//lee los valores del archivo para cargar el nivel
void dispararJugador(Proyectil** cabeza, int xActual, int yActual); //crea el proyectil
void liberarMemoria(Juego* partida);
void borrarProyectil(Proyectil** inicio, Proyectil* proyectilABorrar);
void detectarColisiones(Juego* partida);
void actualizarJuego(Juego* partida);
void dibujarJuego(Juego*);
void limpiarAnterior(Juego*, int);

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
   int NivelJuego = 1;
   Juego miPartida;

   printf("Que nivel desea jugar?");
   scanf("%d",&NivelJuego);
   configNivel config = cargarNivel("archivo.txt", NivelJuego);
   inicializarNivel(&miPartida, "archivo.txt", NivelJuego);//agarra la informacion del archivo para inicializar el nivel
   int filaNave = 20;
   bool juegoejecutado = true;
   //empezamo con el bucle principal
   while(juegoejecutado){
    int naveAnterior = miPartida.navePos;
    if (GetAsyncKeyState(VK_LEFT) & 0x8000){
            if (miPartida.navePos > 0){ //mueve la nave a la izquierda
                miPartida.navePos--;
            }
        }
    if (GetAsyncKeyState(VK_RIGHT) & 0x8000){
        if (miPartida.navePos < config.columnas-1){//mueve la nave a la drcha
            miPartida.navePos++;
        }
    }
    if (GetAsyncKeyState(VK_SPACE) & 0x8000){
        dispararJugador(&miPartida.listaProyectiles, miPartida.navePos, filaNave - 1);//la nav dipara
    }
    if (GetAsyncKeyState(VK_ESCAPE) & 0x8000){
        juegoejecutado = false;//al dl jugo
    }
    actualizarJuego(&miPartida);
    limpiarAnterior(&miPartida, naveAnterior);
    dibujarJuego(&miPartida);
   }
   system("cls");
   printf("Game over");
   liberarMemoria(&miPartida);

   return 0;
}

void inicializarNivel(Juego* partida, const char* archivo, int nivel) {
    configNivel config = cargarNivel(archivo, nivel);
    partida->filas = config.filas;
    partida->columnas = config.columnas;
    partida->navePos = config.columnas/2;
    partida->listaProyectiles = NULL;

    partida->horda = (Enemigo**)malloc(partida->filas * sizeof(Enemigo*));
    
    for (int i = 0; i < partida->filas; i++) {
        partida->horda[i] = (Enemigo*)malloc(partida->columnas * sizeof(Enemigo));
    }
    FILE* file = fopen("archivo.txt", "r");
        
        /*for (int j = 0; j < c; j++) {
            partida->horda[i][j].estaActivo = true;
            partida->horda[i][j].simbolo = 'W';
            partida->horda[i][j].puntos = 10;
        }*/
    char linea[50];
    char buscarNivel[10];
    sprintf(buscarNivel, "Nivel %d", nivel);
    while (fgets(linea, sizeof(linea), file)) {
        linea[strcspn(linea, "\n")] = 0;
        linea[strcspn(linea, "\r")] = 0;
        if (strcmp(linea, buscarNivel) == 0) break;
    }
    char saltoDeLinea[25];
    fgets(saltoDeLinea, sizeof(saltoDeLinea), file);
    fgets(saltoDeLinea, sizeof(saltoDeLinea), file);
    fgets(saltoDeLinea, sizeof(saltoDeLinea), file);

    for (int i = 0; i < partida->filas; i++) {
        for (int j = 0; j < partida->columnas; j++) {
            char c = fgetc(file);       
            if (c == '\n' || c == '\r') {
                j--;
                continue;
            }
            if (c == 'W') {
                partida->horda[i][j].estaActivo = true;
                partida->horda[i][j].simbolo = 'W';
            } else {
                partida->horda[i][j].estaActivo = false;
                partida->horda[i][j].simbolo = ' ';
            }
            partida->horda[i][j].puntos = 10 * nivel;
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

configNivel cargarNivel(const char* archivo, int nivel){
    configNivel config = {5, 10, 3};  // valores default
    FILE* f = fopen(archivo, "r");
    
    if (f == NULL){
        printf("No se encontro el archivo \n"); //funcion que sirve para leer los valores del archivo y cargar el nivel
        return config;
    }
    char linea[50];
    char buscarNivel[10];
    bool encontrado = false;
    sprintf(buscarNivel, "Nivel %d", nivel);
    while (fgets(linea, sizeof(linea), f)){
        linea[strcspn(linea, "\n")] = 0; 
        linea[strcspn(linea, "\r")] = 0;

        if (strcmp(linea, buscarNivel) == 0) {
            printf("Cargando el nivel %d...\n", nivel);
            encontrado = true;
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
    for (int i = 0; i < partida->filas; i++) {
        graficos(0, i); 
        for (int j = 0; j < partida->columnas; j++){ //dibuja a lo nmigo
            if (partida->horda[i][j].estaActivo){
                printf("%c", partida->horda[i][j].simbolo);
            } else{
                printf(" ");
            }
        }
    }
    Proyectil* p = partida->listaProyectiles;
    while (p != NULL) {
        if (p->posY >= 0 && p->posY < 25){ //dibuja lo proyctil
            graficos(p->posX, p->posY);
            printf("|");
        }
        p = p->siguiente;
    }
    graficos(partida->navePos, 20);
    printf("A");
}

void limpiarAnterior(Juego* partida, int naveAnterior){
    graficos(naveAnterior, 20);
    printf(" ");
    Proyectil* p = partida->listaProyectiles; //borra poicion antrior d la nav
    while (p != NULL) {
        graficos(p->posX, p->posY + 1);
        printf(" ");//borra lo proyctil
        p = p->siguiente;
    }
}
