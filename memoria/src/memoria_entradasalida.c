#include "../include/memoria_entradasalida.h"

void atender_memoria_entradasalida(){
    int continuar = 1;
	while( continuar ){
		int cod_op = recibir_operacion(fd_entradasalida);
		switch(cod_op){
			//case SOLICITUD_ACCESO_TABLAS_PAGINAS: //POSIBLE NOMBRE DEL MENSAJE 
			case SOLICITUD_ESCRITURA: //desp confirmar el nombre con lucho (YA LO PUSE EN EL CONEXION.H){
                t_pcb_memoria solicitud;
				recibir_paquete(fd_entradasalida, &cod_op, buffer);
                t_pcb_memoria* proceso = obtener_proceso(solicitud.pid);
                if (proceso == NULL) {
                    log_error(memoria_logger, "Proceso no encontrado: PID %d", solicitud.pid);
                    break;
                }
                int resultado = escribir_memoria(proceso, solicitud.num_pagina, solicitud.offset, solicitud.buffer, solicitud.tamanio);
                if (resultado == 0) {
                    log_info(memoria_logger, "Escritura exitosa: PID %d, Pagina %d", solicitud.pid, solicitud.num_pagina);
                } else {
                    log_error(memoria_logger, "Error en escritura: PID %d, Pagina %d", solicitud.pid, solicitud.num_pagina);
                }
                break;
				break;

			case SOLICITUD_LECTURA:{
                t_pcb_memoria solicitud;
                recibir_datos(fd_entradasalida, &solicitud, sizeof(t_pcb_memoria));
                t_pcb_memoria* proceso = obtener_proceso(solicitud.pid);
                if (proceso == NULL) {
                    log_error(memoria_logger, "Proceso no encontrado: PID %d", solicitud.pid);
                    break;
                }
                void* buffer = malloc(solicitud.tamanio);
                int resultado = leer_memoria(proceso, solicitud.num_pagina, solicitud.offset, buffer, solicitud.tamanio);
                if (resultado == 0) {
                    enviar_datos(fd_entradasalida, buffer, solicitud.tamanio);
                    log_info(memoria_logger, "Lectura exitosa: PID %d, Pagina %d", solicitud.pid, solicitud.num_pagina);
                } else {
                    log_error(memoria_logger, "Error en lectura: PID %d, Pagina %d", solicitud.pid, solicitud.num_pagina);
                }
                free(buffer);
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

int escribir_memoria (t_pcb_memoria, int num_pagina, int offset, void* buffer, int tamanio)
{
	if (num_pagina >= t_pcb_memoria->cant_paginas) { //primero verificar posibles errores
        fprintf(stderr, "Número de página fuera de rango\n");
        return -1;
    }
	t_pagina* pagina = t_pcb_memoria->tabla_paginas[num_pagina];
    if (!pagina->bit_presencia) {
        fprintf(stderr, "Página no presente en memoria\n");
        return -1;
    }
	// Calcular la dirección en la memoria contigua
    int direccion_memoria = t_pagina->frame * TAM_PAGINA + offset;
    memcpy(espacio_usuario + direccion_memoria, buffer, tamanio);
    return 0;

}

int leer_memoria (t_pcb_memoria, int num_pagina, int offset, void* buffer, int tamanio) {
    if (num_pagina >= t_pcb_memoria->cant_paginas) { //primero verificar posibles errores
        fprintf(stderr, "Número de página fuera de rango\n");
        return -1;

    t_pagina* pagina = t_pcb_memoria->tabla_paginas[num_pagina];
    if (!pagina->bit_presencia) {
        fprintf(stderr, "Página no presente en memoria\n");
        return -1;
	}

    // Calcular la dirección en la memoria contigua
    int direccion_memoria = t_pagina->frame * TAM_PAGINA + offset;
    memcpy(buffer, espacio_usuario + direccion_memoria, tamanio);
    return 0;
}
