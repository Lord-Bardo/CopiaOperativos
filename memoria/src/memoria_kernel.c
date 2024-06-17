#include "../include/memoria_kernel.h"

void atender_memoria_kernel(){
    int continuar = 1;
	while( continuar ){
		t_codigo_operacion cod_op;
		t_buffer *buffer = crear_buffer();
		recibir_paquete(fd_kernel, &cod_op, buffer);
		switch(cod_op){
			case SOLICITUD_CREAR_PROCESO:
			    t_pcb_memoria *proceso_recibido;
                proceso_recibido->tabla_de_pags = {0}; // lleno la tabla con paginas inicializadas en {0, NULL, false} para indicar que está vacía
                proceso_recibido->memoria_de_instrucciones = NULL;

				buffer_desempaquetar_proceso(buffer, proceso_recibido); // función hecha en utils de memoria.

				crear_proceso(proceso_recibido);

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
        exit(EXIT_FAILURE);
    }
    strcpy(ruta_completa, PATH_INSTRUCCIONES); // Lleno ruta completa con ruta a carpeta de archivos pseudocodigo.

    // Concateno el nombre del archivo a la ruta completa usando string_append.
    string_append(&ruta_completa, proceso->path); 

    // Abro el archivo
    FILE *archivo = fopen(ruta_completa, "r");
    if (archivo == NULL) {
        enviar_codigo_operacion(fd_kernel, ERROR_CREACION_PROCESO);
        perror("Error al abrir el archivo");
        free(ruta_completa);
        exit(EXIT_FAILURE);
    }

	// Leer el archivo línea por línea
    char linea[TAM_MEMORIA];
    while (fgets(linea, sizeof(linea), archivo) != NULL) {
        // Eliminar el salto de línea al final de cada línea
        linea[strcspn(linea, "\n")] = '\0';

        // Almacenar la línea en memoria_de_instrucciones
        proceso->memoria_de_instrucciones[num_lineas] = strdup(linea);
        if (proceso->memoria_de_instrucciones[num_lineas] == NULL) {
            perror("Error al duplicar la cadena");
            fclose(archivo);
            free(ruta_completa);
            exit(EXIT_FAILURE);
        }
        num_lineas++;
    }
    agregar_proceso_a_procesos(*proceso); // Función hecha en utils de memoria.
    enviar_codigo_operacion(fd_kernel, CONFIRMACION_PROCESO_INICIADO);

    // Cerrar el archivo
    fclose(archivo);
    free(ruta_completa);
}

