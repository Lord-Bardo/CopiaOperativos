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
    char **instrucciones; // Array de punteros a cadenas de caracteres (instrucciones) del arch pseudocódigo
    int cantidad_instrucciones; // Número de instrucciones cargadas en el array instrucciones
} t_memoria_instrucciones;

// typedef struct {
//     int retardo; // Retardo en milisegundos
//     uint32_t tamanio_memoria;
//     uint32_t tamanio_pagina;
//     char* path
// } Configuracion;

typedef struct 
{
    char* path_pseudocodigo;
    int pid;
} t_pcb_memoria;


void leer_configuracion(const char *archivo_configuracion) {//función lee un archivo de configuración para obtener el valor del retardo y lo devuelve en una struct Configuracion.
    FILE *archivo = fopen(archivo_configuracion, "r");
    if (!archivo) {
        perror("No se pudo abrir el archivo de configuración");
        exit(EXIT_FAILURE);
    }

    fscanf(archivo, "retardo=%d", &config.retardo);
    fclose(archivo);
    return config;
}//ya está hecha

void leer_pseudocodigo(const char *archivo_pseudocodigo, t_pcb_memoria*pcb_memoria) { //primera parte que pide el tp
    FILE *archivo = fopen(path_config + path_pseudocodigo, "r");
    if (!archivo) {
        perror("No se pudo abrir el archivo de pseudocódigo");
        exit(EXIT_FAILURE);
    }

    // Leer el contenido del archivo línea por línea
    char* linea = NULL;
    size_t longitud = 0;
    ssize_t leido;
    while ((leido = getline(&linea, &longitud, archivo)) != -1) {
        printf("%s", linea);
    }

    // Liberar la memoria y cerrar el archivo
    free(linea);
    fclose(archivo); //hasta acá esta copiado, hay que acomodar y revisar si anda la concatenacion de los path

    pcb_memoria->instrucciones = malloc(MAX_INSTRUCCIONES * sizeof(char *));
    pcb_memoria->cantidad_instrucciones= 0;

    char linea[MAX_LINEA];
    while (fgets(linea, sizeof(linea), archivo) != NULL) {
        pcb_memoria->instrucciones[pcb->cantidad_instrucciones] = strdup(linea);
        pcb_memoria->cantidad_instrucciones++;
    }
    fclose(archivo);
}

char *obtener_instruccion(t_pcb_memoria *pcb_memoria, Configuracion config) {
    if (pcb_memoria->program_counter >= pcb_memoria->cantidad_instrucciones {
        return NULL; // No hay más instrucciones
    }
    usleep(config.retardo * 1000); // Aplicar retardo en milisegundos
    return pcb_memoria->instrucciones[pcb_memoria->program_counter++];
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

int main() {
    Configuracion config = leer_configuracion("config.txt");// kernel pasa la ruta del config
    t_memoria_instrucciones *memoria = cargar_instrucciones("pseudocodigo.txt");//ruta del kernel tambien
    if (!memoria) {
        return 1;
    }

    t_pcb proceso;
    proceso.PID = 1;
    proceso.registros.PC = 0;//faltaría el path

    char *instruccion;
    while ((instruccion = obtener_instruccion(memoria, proceso.registros.PC, config.retardo)) != NULL) {
        printf("Instrucción en PC %d: %s\n", proceso.registros.PC, instruccion);
        proceso.registros.PC++;
    }

    // Liberar memoria
    for (int i = 0; i < memoria->cantidad_instrucciones; i++) {
        free(memoria->instrucciones[i]);
    }
    free(memoria->instrucciones);
    free(memoria);

    return 0;
}

//falta iniciar proceso