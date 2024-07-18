#include "../include/memoria_kernel.h"

void* espacio_usuario; 
int frame_libre; //contendrá el índice del siguiente frame vacío del espacio_usuario
t_pcb_memoria* procesos; // En esta lista voy a ir colocando todos mis procesos.
size_t num_instruccion; // Número de instrucciones leídas de un archivo de pseudocodigo.

void atender_memoria_kernel(){
    int continuar = 1;
	while( continuar ){
		t_codigo_operacion cod_op;
		t_buffer *buffer = crear_buffer();
		recibir_paquete(fd_kernel, &cod_op, buffer);
		switch(cod_op){
			case SOLICITUD_INICIAR_PROCESO:
                // TIEMPO DE RETARDO
                usleep(RETARDO_REPUESTA);

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
                usleep(RETARDO_REPUESTA);

                // Declaro estructura necesaria y desempaqueto lo recibido.
                int pid_recibido;
                buffer_desempaquetar(buffer, &pid_recibido);

                // Finalizo el proceso.
                finalizar_proceso(pid_recibido);

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
    char* ruta_completa = malloc(strlen(PATH_INSTRUCCIONES) + strlen(proceso->path) + 1); // Ruta completa empieza vacía. Intentar usar string_new en vez de malloc si genera segment fault.
    if (ruta_completa == NULL) {
        perror("Error al asignar memoria");
        enviar_codigo_operacion(fd_kernel, ERROR_CREACION_PROCESO); 
        exit(EXIT_FAILURE);
    } 
    strcpy(ruta_completa, PATH_INSTRUCCIONES); // Lleno ruta completa con ruta a carpeta de archivos pseudocodigo. Si genera segment fault intentar con: memcpy(ruta_completa, PATH_INSTRUCCIONES, strlen(PATH_INSTRUCCIONES) + 1)

    // Concateno la ruta al archivo con la ruta completa usando string_append.
    string_append(&ruta_completa, proceso->path); 

    // Abro el archivo
    FILE *archivo = fopen(ruta_completa, "r");
    if (archivo == NULL) {
        perror("Error al abrir el archivo");
        fclose(archivo);
        free(ruta_completa);
        liberar_proceso(proceso);
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
            liberar_proceso(proceso);
            enviar_codigo_operacion(fd_kernel, ERROR_CREACION_PROCESO); 
            exit(EXIT_FAILURE);
        }

        // Almacenar la instruccion en memoria_de_instrucciones
        proceso->memoria_de_instrucciones[num_instruccion] = strdup(instruccion);
        if (proceso->memoria_de_instrucciones[num_instruccion] == NULL) {
            perror("Error al duplicar la cadena");
            fclose(archivo);
            free(ruta_completa);
            liberar_proceso(proceso);
            enviar_codigo_operacion(fd_kernel, ERROR_CREACION_PROCESO); 
            exit(EXIT_FAILURE);
        }
        num_instruccion++;
    }
    agregar_proceso(*proceso); // Agreso el proceso a lista de procesos.
    enviar_codigo_operacion(fd_kernel, CONFIRMACION_PROCESO_INICIADO); 

    // Cerrar el archivo
    fclose(archivo);
    free(ruta_completa);

    // Borrar una vez terminado el testeo
    printf("Imprimo primer y ultima instruccion del proceso: %d\n", procesos[0].pid);
    printf("Primera instruccion: %s\n", procesos[0].memoria_de_instrucciones[0]);
	printf("Ultima instruccion: %s\n", procesos[0].memoria_de_instrucciones[20]);
	log_info(memoria_logger, "Entré a crear proceso y cree proceso existosamente :)");
}

void finalizar_proceso(int pid_recibido)
{
    //Encuentro índice del proceso en cuestión.
    int i = encontrar_proceso(pid_recibido);
    t_pcb_memoria* proceso = &procesos[i];

    //Libero memoria del proceso 
    liberar_proceso(proceso); // Supongo que con el free de tabla de paginas, los frames pasan a estar disponibles (no se debe borrar la info del frame a pesar de finalizar proceso).

    //Elimino proceso de lista de procesos
    eliminar_proceso(i);

    //Avisar a KERNEL (creo)
    enviar_codigo_operacion(fd_kernel, CONFIRMACION_PROCESO_FINALIZADO);
}
