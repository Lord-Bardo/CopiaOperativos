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


void leer_pseudocodigo(const char *archivo_pseudocodigo, t_pcb *pcb ) { //primera parte que pide el tp
    FILE *archivo = fopen(archivo_pseudocodigo, "r");
    if (!archivo) {
        perror("No se pudo abrir el archivo de pseudocódigo");
        exit(EXIT_FAILURE);
    }
    pcb->instrucciones = malloc(MAX_INSTRUCCIONES * sizeof(char *));
    pcb->num_instrucciones = 0;

    char linea[MAX_LINEA];
    while (fgets(linea, sizeof(linea), archivo) != NULL) {
        pcb->instrucciones[pcb->num_instrucciones] = strdup(linea);
        pcb->num_instrucciones++;
    }
    fclose(archivo);
}

typedef struct {
    int retardo; // Retardo en milisegundos
} Configuracion; //

char *obtener_instruccion(t_pcb *pcb, Configuracion config) {
    if (pcb->program_counter >= pcb->num_instrucciones) {
        return NULL; // No hay más instrucciones
    }
    usleep(config.retardo * 1000); // Aplicar retardo en milisegundos
    return pcb->instrucciones[pcb->program_counter++];
}

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

