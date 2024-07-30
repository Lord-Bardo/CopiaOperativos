#include "../include/memoria_kernel.h"

void* espacio_usuario; 
t_bitarray* frames_libres; // indicará cuáles frames están libres y cuáles están ocupados.
t_list* procesos; // En esta lista voy a ir colocando todos mis procesos.
size_t num_instruccion; // Número de instrucciones leídas de un archivo de pseudocodigo.
int pid_kernel;

// Mutex
pthread_mutex_t mutex_espacio_usuario; // USAR EN LEER Y ESCRIBIR
pthread_mutex_t mutex_procesos; // USAR EN LEER Y ESCRIBIR
pthread_mutex_t mutex_bitmap;

void atender_memoria_kernel(){
	while(1){
		t_codigo_operacion cod_op;
		t_buffer *buffer = crear_buffer();
		recibir_paquete(fd_kernel, &cod_op, buffer);
		switch(cod_op){
			case SOLICITUD_INICIAR_PROCESO:
                // TIEMPO DE RETARDO
                usleep(RETARDO_RESPUESTA*1000);

                // Creo e inicializo estructuras necesarias.
			    t_pcb_memoria *proceso_recibido = inicializar_proceso();
                num_instruccion = 0;

                // Desempaqueto lo recibido.
				buffer_desempaquetar_proceso(buffer, proceso_recibido); 
                
                // Creo el proceso.
				crear_proceso(proceso_recibido);

                //Libero memoria.
                eliminar_buffer(buffer);
                
                break;

			case SOLICITUD_FINALIZAR_PROCESO:
                // TIEMPO DE RETARDO
                usleep(RETARDO_RESPUESTA*1000);

                // Declaro estructura necesaria y desempaqueto lo recibido.
                buffer_desempaquetar(buffer, &pid_kernel);

                // Finalizo el proceso.
                pthread_mutex_lock(&mutex_procesos);
                list_remove_and_destroy_by_condition(procesos, comparar_pid_kernel, destruir_proceso);
                pthread_mutex_unlock(&mutex_procesos);

                //Avisar a KERNEL 
                enviar_codigo_operacion(fd_kernel, CONFIRMACION_PROCESO_FINALIZADO);

                // Libero memoria.
                eliminar_buffer(buffer);

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
    // char* ruta_completa = malloc(strlen(PATH_INSTRUCCIONES) + strlen(proceso->path) + 1); // Ruta completa empieza vacía. Intentar usar string_new en vez de malloc si genera segment fault.
    char* ruta_completa = string_duplicate(PATH_INSTRUCCIONES);
    // if (ruta_completa == NULL) {
    //     perror("Error al asignar memoria");
    //     enviar_codigo_operacion(fd_kernel, ERROR_CREACION_PROCESO); 
    //     exit(EXIT_FAILURE);
    // } 
    // strcpy(ruta_completa, PATH_INSTRUCCIONES); // Lleno ruta completa con ruta a carpeta de archivos pseudocodigo. Si genera segment fault intentar con: memcpy(ruta_completa, PATH_INSTRUCCIONES, strlen(PATH_INSTRUCCIONES) + 1)

    // Concateno la ruta al archivo con la ruta completa usando string_append.
    string_append(&ruta_completa, proceso->path); 

    // Abro el archivo
    FILE *archivo = fopen(ruta_completa, "r");
    if (archivo == NULL) {
        perror("Error al abrir el archivo");
        fclose(archivo);
        free(ruta_completa);
        destruir_proceso(proceso);
        enviar_codigo_operacion(fd_kernel, ERROR_CREACION_PROCESO); 
        exit(EXIT_FAILURE);
    }
	
	// Leer el archivo instruccion por instruccion.
    while (1) 
    {
        // Definir un búfer de tamaño adecuado
        char buffer[256];
	    //char *buffer = string_new();

        // Leer una línea del archivo
        if (fgets(buffer, sizeof(buffer), archivo) == NULL) {
            // Si se alcanza el final del archivo o hay un error, salir del bucle
            break;
        }

        // Eliminar el salto de línea al final de cada instrucción
        buffer[strcspn(buffer, "\n")] = '\0';

        // Validar si lo leído es una instrucción válida
        if (!instruccion_valida(buffer)) {
            perror("Instrucción inválida en el archivo");
            fclose(archivo);
            free(ruta_completa);
            destruir_proceso(proceso);
            enviar_codigo_operacion(fd_kernel, ERROR_CREACION_PROCESO); 
            exit(EXIT_FAILURE);
        }

        // Duplicar la instrucción para almacenarla en la lista
        char *instruccion = string_duplicate(buffer);
        if (instruccion == NULL) {
            perror("Error al duplicar la instrucción");
            fclose(archivo);
            free(ruta_completa);
            destruir_proceso(proceso);
            enviar_codigo_operacion(fd_kernel, ERROR_CREACION_PROCESO); 
            exit(EXIT_FAILURE);
        }

        // Almacenar la instrucción en memoria_de_instrucciones
        list_add(proceso->memoria_de_instrucciones, instruccion);
	//free(buffer);
    }

    pthread_mutex_lock(&mutex_procesos);
    list_add(procesos, proceso); // Agreso el proceso a lista de procesos.
    pthread_mutex_unlock(&mutex_procesos);
    enviar_codigo_operacion(fd_kernel, CONFIRMACION_PROCESO_INICIADO); 

    // Cerrar el archivo y liberar ruta.
    fclose(archivo);
    free(ruta_completa);
}


