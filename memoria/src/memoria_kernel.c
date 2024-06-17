#include "../include/memoria_kernel.h"

void atender_memoria_kernel(){
    int continuar = 1;
	while( continuar ){
		t_codigo_operacion cod_op;
		t_buffer * buffer = crear_buffer();
		recibir_paquete(fd_kernel, &cod_op, buffer);
		switch(cod_op){
			case INICIAR_PROCESO:
			    char* path_recibido;

				buffer_desempaquetar_path(buffer, path_recibido); //esto recibe todo el paquete desde kernel

				crear_proceso(*path_recibido);

			case FINALIZAR_PROCESO:
                //TODO
			case -1:
				log_error(memoria_logger, "Se perdio la conexion con KERNEL!");
				continuar = 0;
				break;
			default:
				log_warning(memoria_logger, "MEMORIA: Operacion desconocida recibida de KERNEL");
				break;
		}
	}
}
t_pcb_memoria crear_proceso(char* path_proceso)
{
    // Asigno una tabla vacía al proceso entrante.
	t_tabla_paginas tabla_de_pags = {0, NULL}; // Pongo 0 como PID, no sé cuándo y cómo recibo el PID pero lo voy a necesitar.
    t_pcb_memoria proceso = {0, tabla_de_pags, NULL}; // En realidad, proceso deberá ser inicializado con lo recibido desde kernel.
	
    // Inicializo la cadena de ruta completa.
    char* ruta_completa = malloc(strlen(PATH_INSTRUCCIONES) + 1); // Ruta completa empieza vacía.
    if (ruta_completa == NULL) {
        perror("Error al asignar memoria");
        exit(EXIT_FAILURE);
    }
    strcpy(ruta_completa, PATH_INSTRUCCIONES); // Lleno ruta completa con ruta a carpeta de archivos pseudocodigo.

    // Concateno el nombre del archivo a la ruta completa usando string_append.
    string_append(&ruta_completa, path_proceso); 

    // Abro el archivo
    FILE *archivo = fopen(ruta_completa, "r");
    if (archivo == NULL) {
        perror("Error al abrir el archivo");
        free(ruta_completa);
        exit(EXIT_FAILURE);
    }

	// Leer el archivo línea por línea
    char linea[256];
    while (fgets(linea, sizeof(linea), archivo) != NULL) {
        // Eliminar el salto de línea al final de cada línea
        linea[strcspn(linea, "\n")] = '\0';

        // Reasignar memoria para almacenar una línea más
        proceso.memoria_de_instrucciones = realloc(proceso.memoria_de_instrucciones, (num_lineas + 1) * sizeof(char*));
        if (proceso.memoria_de_instrucciones == NULL) {
            perror("Error al reasignar memoria");
            fclose(archivo);
            free(ruta_completa);
            exit(EXIT_FAILURE);
        }

        // Almacenar la línea en memoria_de_instrucciones
        proceso.memoria_de_instrucciones[num_lineas] = strdup(linea);
        if (proceso.memoria_de_instrucciones[num_lineas] == NULL) {
            perror("Error al duplicar la cadena");
            fclose(archivo);
            free(ruta_completa);
            exit(EXIT_FAILURE);
        }
        num_lineas++;
    }
    agregar_proceso_a_procesos(proceso); // Función hecha en utils de memoria.

    // Aca podría ir la lógica en donde le envío mensaje de confirmación a kernel de que
    // el proceso fue creado con éxito (aunque no sé si hay que mandar una confirmación).

    // Cerrar el archivo
    fclose(archivo);
    free(ruta_completa);
}

