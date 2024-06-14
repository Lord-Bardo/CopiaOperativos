//SIMPLEMENTE ESTOY INTENTANDO QUE ME SALGA ALGO POR FAVOR NO BARDEAR NI MOVER QUE SINO NO ENTIENDO NADA (TENGANME PACIENCIA)

//memoria de instrucciones
#define MAX_INSTRUCCIONES 256
#define MAX_LINEA 256

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <utils/conexion.h>
#include <utils/planificadores.h>
#include "memoria_gestor.h"
#include "inicializar_memoria.h"
#include "memoria_entradasalida.h"
#include "memoria_kernel.h"
#include "memoria_cpu.h"

typedef struct {
    char **instrucciones;
    int cantidad_instrucciones;
} t_memoria_instrucciones;

typedef struct {
    int retardo; // Retardo en milisegundos
} Configuracion; //

Configuracion leer_configuracion(const char *archivo_configuracion) {//
    Configuracion config;
    FILE *archivo = fopen(archivo_configuracion, "r");
    if (!archivo) {
        perror("No se pudo abrir el archivo de configuración");
        exit(EXIT_FAILURE);
    }

    fscanf(archivo, "retardo=%d", &config.retardo);
    fclose(archivo);
    return config;
}

void leer_pseudocodigo(const char *archivo_pseudocodigo, t_pcb *pcb ) { //primera parte que pide el tp
    FILE *archivo = fopen(archivo_pseudocodigo, "r");
    if (!archivo) {
        perror("No se pudo abrir el archivo de pseudocódigo");
        exit(EXIT_FAILURE);
    }
    pcb->instrucciones = malloc(MAX_INSTRUCCIONES * sizeof(char *));
    pcb->cantidad_instrucciones= 0;

    char linea[MAX_LINEA];
    while (fgets(linea, sizeof(linea), archivo) != NULL) {
        pcb->instrucciones[pcb->cantidad_instrucciones] = strdup(linea);
        pcb->cantidad_instrucciones++;
    }
    fclose(archivo);
}

char *obtener_instruccion(t_pcb *pcb, Configuracion config) {
    if (pcb->program_counter >= pcb->cantidad_instrucciones {
        return NULL; // No hay más instrucciones
    }
    usleep(config.retardo * 1000); // Aplicar retardo en milisegundos
    return pcb->instrucciones[pcb->program_counter++];
}

t_memoria_instrucciones *cargar_instrucciones(const char *nombre_archivo) {
    FILE *archivo = fopen(nombre_archivo, "r");
    if (!archivo) {
        perror("Error al abrir el archivo de pseudocódigo");
        return NULL;
    }

    t_memoria_instrucciones *memoria = malloc(sizeof(t_memoria_instrucciones));
    memoria->instrucciones = malloc(100 * sizeof(char *));
    memoria->cantidad_instrucciones = 0;

    char buffer[MAX_INSTRUCCION];
    while (fgets(buffer, MAX_INSTRUCCION, archivo)) {
        buffer[strcspn(buffer, "\n")] = 0; // Eliminar salto de línea
        memoria->instrucciones[memoria->cantidad_instrucciones] = strdup(buffer);
        memoria->cantidad_instrucciones++;
    }
    fclose(archivo);
    return memoria;
}

