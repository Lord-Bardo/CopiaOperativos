#include "../include/memoria_entradasalida.h"
#define MAX_PROCESOS 100 // Define el número máximo de procesos CHEQUEAR
// Definición de las variables globales
t_pcb_memoria procesos[MAX_PROCESOS]; // Define el tamaño máximo adecuado para tus necesidades
int cantidad_procesos = 0;

// Declaración de la función obtener_proceso
t_pcb_memoria* obtener_proceso(int pid);

int escribir_memoria (t_pcb_memoria* proceso, int num_pagina, int offset, void* buffer, int tamanio)
{
	if (num_pagina >= proceso->cant_paginas) { //primero verificamos que no hayan errores
        fprintf(stderr, "Número de página fuera de rango\n");
        return -1;
    }
	t_pagina* pagina = &proceso->tabla_paginas[num_pagina];
    if (!pagina->bit_presencia) {
        fprintf(stderr, "Página no presente en memoria\n");
        return -1;
    }
	// Calcular la dirección en la memoria contigua
    int direccion_memoria = pagina ->num_frame * TAM_PAGINA + offset;
    memcpy(espacio_usuario + direccion_memoria, buffer, tamanio); //copia datos hacia la memoria del proceso desde el buffer.(lo toma como fuente)
    return 0;

}

int leer_memoria (t_pcb_memoria* proceso, int num_pagina, int offset, void* buffer, int tamanio) 
{
    if (num_pagina >= proceso->cant_paginas) { //primero verificar posibles errores
        fprintf(stderr, "Número de página fuera de rango\n");
        return -1;

    t_pagina* pagina = &proceso->tabla_paginas[num_pagina];
    if (!pagina->bit_presencia) {
        fprintf(stderr, "Página no presente en memoria\n");
        return -1;
	}

    // Calcular la dirección en la memoria contigua
    int direccion_memoria = pagina->num_frame * TAM_PAGINA + offset;
    memcpy(buffer, espacio_usuario + direccion_memoria, tamanio); //copia datos desde la memoria del proceso hacia el buffer.(lo toma como destino)

    return 0;
}

void atender_memoria_entradasalida(){
    int continuar = 1;
	while( continuar ){
		t_codigo_operacion cod_op;
		t_buffer *buffer = crear_buffer();
		recibir_paquete(fd_entradasalida, &cod_op, buffer);
		switch(cod_op){
			//case SOLICITUD_ACCESO_TABLAS_PAGINAS: //POSIBLE NOMBRE DEL MENSAJE 
			case SOLICITUD_ESCRITURA: //desp confirmar el nombre con lucho (YA LO PUSE EN EL CONEXION.H){
                t_pcb_memoria solicitud;
                int tamanio;
                void* data;

                buffer_desempaquetar_proceso(buffer, &solicitud);
                buffer_desempaquetar(buffer, &tamanio);
                data = malloc(tamanio);
                buffer_desempaquetar(buffer, data);
				
                t_pcb_memoria* proceso_recibido = obtener_proceso(solicitud.pid);
                if (proceso_recibido == NULL) {
                    log_error(memoria_logger, "Proceso no encontrado: PID %d", solicitud.pid);
                    eliminar_buffer(buffer);
                    break;
                }
                int resultado = escribir_memoria(proceso_recibido,  solicitud.num_pagina, solicitud.offset, data, tamanio);
                if (resultado == 0) {
                    log_info(memoria_logger, "Escritura exitosa: PID %d, Pagina %d", solicitud.pid, solicitud.num_pagina);
                } else {
                    log_error(memoria_logger, "Error en escritura: PID %d, Pagina %d", solicitud.pid, solicitud.num_pagina);
                }
				eliminar_buffer(buffer);
                break; 

			case SOLICITUD_LECTURA:

                buffer_desempaquetar(buffer, &solicitud);
                buffer_desempaquetar(buffer, &tamanio);

                if (proceso_recibido == NULL) {
                    log_error(memoria_logger, "Proceso no encontrado: PID %d", solicitud.pid);
					eliminar_buffer(buffer);
                    break;
                }

                t_buffer* buffer_lectura = crear_buffer();
				buffer_lectura->size = tamanio;
                buffer_lectura->stream = malloc(buffer_lectura->size);

                int resultado_lectura = leer_memoria(proceso_recibido, solicitud.num_pagina, solicitud.offset, buffer_lectura->stream, buffer_lectura->size);
                if (resultado_lectura == 0) {
                    enviar_datos(fd_entradasalida, buffer_lectura->stream, buffer_lectura->size);
                    log_info(memoria_logger, "Lectura exitosa: PID %d, Pagina %d", solicitud.pid, solicitud.num_pagina);
                } else {
                    log_error(memoria_logger, "Error en lectura: PID %d, Pagina %d", solicitud.pid, solicitud.num_pagina);
                }
                eliminar_buffer(buffer);
                free(buffer_lectura->stream);
				free(buffer_lectura);
                break; //lo mismo con el case anterior (este también esta en el coenxion.h)
			case -1:
				log_error(memoria_logger, "Se perdio la conexion con ENTRADASALIDA!");
				continuar = 0;
				break;

			default:
				log_warning(memoria_logger, "MEMORIA: Operacion desconocida recibida de ENTRADASALIDA");
				break;
		}
	}
}

// Función para obtener un proceso por su PID
t_pcb_memoria* obtener_proceso(int pid) {
    extern t_pcb_memoria procesos[]; // Declaración del array de procesos
    extern int cantidad_procesos;    // Declaración de la cantidad de procesos

    for (int i = 0; i < cantidad_procesos; i++) {
        if (procesos[i].pid == pid) {
            return &procesos[i];
        }
    }
    // Si no se encuentra el proceso, retornar NULL
    return NULL;
}

void enviar_datos(int socket, void* buffer, int size) { //todavía tengp que revisar esto que fue todo copy-paste
    int total_bytes_sent = 0;
    int bytes_left = size;
    int bytes_sent;

    while (total_bytes_sent < size) {
        bytes_sent = send(socket, buffer + total_bytes_sent, bytes_left, 0);
        if (bytes_sent == -1) {
            perror("Error al enviar datos");
            break;
        }
        total_bytes_sent += bytes_sent;
        bytes_left -= bytes_sent;
    }

    if (total_bytes_sent == size) {
        printf("Datos enviados con éxito\n");
    } else {
        printf("No se pudieron enviar todos los datos\n");
    }
} 