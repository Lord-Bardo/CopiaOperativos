#include "../include/memoria_administracion.h"

//acá se desarrollan funciones paracumplir con las peticiones que llegan de CPU, Kernel y Entrada-Salida

espacio_usuario = malloc();
puntero_espacio_usuario = espacio_usuario;

void inicializar_tamanio_memoria() 
{
    espacio_de_usuario = malloc(TAM_MEMORIA);
    memset(espacio_de_usuario, 0, TAM_MEMORIA);
    puntero_espacio_usuario = espacio_usuario; //puntero hacia el primer frame.
}

t_pcb_memoria crear_proceso()
{
    // Creo que acá iría la lógica de recepción de un paquete con el 
    // mensaje de que se quiere crear un proceso o puede ir afuera de 
    // esta función y que la función reciba lo que se nos envió.

    t_tabla_paginas tabla_de_pags = {0, NULL}; // En realidad, debe inicializarse con el PID recibide en lugar de 0.
    agregar_tabla_a_tablas(tabla_de_pags); // Falta hacer esta funcion.

    t_pcb_memoria proceso = {0, "", tabla_de_pags, NULL}; // En realidad, proceso deberá ser inicializado con lo recibido desde kernel.
    
    // Inicializo la cadena de ruta completa.
    char* ruta_completa = malloc(strlen(PATH_INSTRUCCIONES) + 1); // Ruta completa empieza vacía.
    if (ruta_completa == NULL) {
        perror("Error al asignar memoria");
        exit(EXIT_FAILURE);
    }
    strcpy(ruta_completa, PATH_INSTRUCCIONES); // Lleno ruta completa con ruta a carpeta de archivos pseudocodigo.

    // Concateno el nombre del archivo a la ruta completa usando string_append.
    string_append(&ruta_completa, proceso.path_pseudocodigo); // Por el momento el path_pseudocodigo está vacío.

    // Abro el archivo
    FILE *archivo = fopen(ruta_completa, "r");
    if (archivo == NULL) {
        perror("Error al abrir el archivo");
        free(ruta_completa);
        exit(EXIT_FAILURE);
    }

    // Leo el archivo línea por línea y almaceno sus instrucciones en memoria_de_instrucciones
    char linea[256];
    while (fgets(linea, sizeof(linea), archivo) != NULL) 
    {
        // Elimino el salto de línea al final de cada línea del archivo.
        linea[strcspn(linea, "\n")] = '\0';

        // Reasigno memoria para almacenar una línea más en memoria_de_instrucciones
        proceso.memoria_de_instrucciones = realloc(proceso.memoria_de_instrucciones, (num_lineas + 1) * sizeof(char *));
        if (memoria_de_instrucciones == NULL) {
            perror("Error al reasignar memoria");
            fclose(archivo);
            free(ruta_completa);
            exit(EXIT_FAILURE);
        }

        // Almaceno la línea en memoria_de_instrucciones
        proceso.memoria_de_instrucciones[num_lines] = strdup(linea);
        if (memoria_de_instrucciones[num_lines] == NULL) {
            perror("Error al duplicar la cadena");
            fclose(archivo);
            free(ruta_completa);
            exit(EXIT_FAILURE);
        }
        num_lines++;
    }
    return proceso;

    /* 
    ------------------------------ DISCLAIMER! ----------------------------------
    En algún lado, ya sea en el main o en la función finalizar proceso o 
    simplemente cuando se terminen de enviar las instrucciones a CPU, 
    debe liberarse el campo memoria_de_instrucciones con free(). No es 
    responsabilidad de crear proceso() dicha liberación.
    -------------------------- Fin del DISCLAIMER ------------------------------- 
    */
}