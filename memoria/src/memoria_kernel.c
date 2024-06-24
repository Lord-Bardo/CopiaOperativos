#include "../include/memoria_kernel.h"

void atender_memoria_kernel(){
    int continuar = 1;
	while( continuar ){
		t_codigo_operacion cod_op;
		t_buffer *buffer = crear_buffer();
		recibir_paquete(fd_kernel, &cod_op, buffer);
		switch(cod_op){
			case SOLICITUD_INICIAR_PROCESO:
                // Inicializo tabla y lista de instrucciones del proceso recibido.
			    t_pcb_memoria *proceso_recibido;
                proceso_recibido->tabla_paginas = malloc((TAM_MEMORIA / TAM_PAGINA) * sizeof(t_pagina));
                proceso_recibido->memoria_de_instrucciones = malloc(TAM_MEMORIA * sizeof(char*));

                for (int i = 0; i < TAM_MEMORIA; i++) 
                    proceso_recibido->memoria_de_instrucciones[i] = malloc(sizeof(char));
                
                num_instruccion = 0;
                
                // Verificar si la asignación de memoria fue exitosa
                if (proceso_recibido->tabla_paginas == NULL || proceso_recibido->memoria_de_instrucciones == NULL) {
                    // Manejar el error de asignación de memoria
                    fprintf(stderr, "Error al asignar memoria\n");
                    exit(EXIT_FAILURE);
                }

				buffer_desempaquetar_proceso(buffer, proceso_recibido); // función hecha en utils de memoria.

				crear_proceso(proceso_recibido);
                break;

			case SOLICITUD_FINALIZAR_PROCESO:
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

void crear_proceso(t_pcb_memoria *proceso)
{
    // Inicializo la cadena de ruta completa.
    char* ruta_completa = malloc(strlen(PATH_INSTRUCCIONES) + 1); // Ruta completa empieza vacía.
    if (ruta_completa == NULL) {
        perror("Error al asignar memoria");
        enviar_codigo_operacion(fd_kernel, ERROR_CREACION_PROCESO);
        exit(EXIT_FAILURE);
    }
    strcpy(ruta_completa, PATH_INSTRUCCIONES); // Lleno ruta completa con ruta a carpeta de archivos pseudocodigo.

    // Concateno la ruta al archivo con la ruta completa usando string_append.
    string_append(&ruta_completa, proceso->path); 

    // Abro el archivo
    FILE *archivo = fopen(ruta_completa, "r");
    if (archivo == NULL) {
        perror("Error al abrir el archivo");
        free(ruta_completa);
        enviar_codigo_operacion(fd_kernel, ERROR_CREACION_PROCESO);
        exit(EXIT_FAILURE);
    }

	// Leer el archivo instruccion por instruccion.
    char instruccion[256];
    while (fgets(instruccion, sizeof(instruccion), archivo) != NULL) 
    {
        // Eliminar el salto de línea al final de cada instruccion
        instruccion[strcspn(instruccion, "\n")] = '\0';

        // Valido si lo leido es una instrucción válida.
        if (!instruccion_valida(instruccion)) {
            perror("Instrucción inválida en el archivo");
            fclose(archivo);
            free(ruta_completa);
            liberar_pcb_memoria(proceso);
            enviar_codigo_operacion(fd_kernel, ERROR_CREACION_PROCESO);
            exit(EXIT_FAILURE);
        }

        // Almacenar la instruccion en memoria_de_instrucciones
        proceso->memoria_de_instrucciones[num_instruccion] = strdup(instruccion);
        if (proceso->memoria_de_instrucciones[num_instruccion] == NULL) {
            perror("Error al duplicar la cadena");
            fclose(archivo);
            free(ruta_completa);
            liberar_pcb_memoria(proceso);
            enviar_codigo_operacion(fd_kernel, ERROR_CREACION_PROCESO);
            exit(EXIT_FAILURE);
        }
        num_instruccion++;
    }
    agregar_proceso_a_procesos(*proceso); // Función hecha en utils de memoria.
    enviar_codigo_operacion(fd_kernel, CONFIRMACION_PROCESO_INICIADO);

    // Cerrar el archivo
    fclose(archivo);
    free(ruta_completa);
}

