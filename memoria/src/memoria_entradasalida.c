#include "../include/memoria_entradasalida.h"

int pid_es;

void atender_memoria_entradasalida(void *fd_entradasalida_void){
	int fd_entradasalida = *(int *)fd_entradasalida_void;
	while(1){
		t_codigo_operacion cod_op;
		t_buffer *buffer = crear_buffer();

		if (recibir_paquete(fd_entradasalida, &cod_op, buffer) > 0){
			switch(cod_op){ 
				case SOLICITUD_ESCRITURA:
					// TIEMPO DE RETARDO
					usleep(RETARDO_RESPUESTA*1000);
					
					// Creo estructuras necesarias.
					int direc_fisica_write, bytes_write;
					void* dato_write;

					// Desempaqueto y almaceno información recibida.
					buffer_desempaquetar(buffer, &pid_es);
					buffer_desempaquetar(buffer, &direc_fisica_write);
					buffer_desempaquetar(buffer, &bytes_write);
					dato_write = malloc(bytes_write);
					buffer_desempaquetar(buffer, dato_write);

					// Escribo en el espacio de usuario de la memoria.
					escribir(direc_fisica_write, bytes_write, dato_write);

					// Log mínimo y obligatorio - Acceso a espacio de usuario.
					printf("Log mínimo y obligatorio - Acceso a espacio de usuario\n");
					log_info(memoria_logger_min_y_obl, "PID: %d - Accion: ESCRIBIR - Direccion fisica: %d - Tamaño: %d", pid_es, direc_fisica_write, bytes_write);

					// Envío confirmación de escritura.
			
					if(enviar_codigo_operacion(fd_entradasalida, CONFIRMACION_ESCRITURA) < 0){
						// Se ha desconectado el módulo de entrada/salida
						log_info(memoria_logger, "MEMORIA: Modulo ENTRADASALIDA desconectado.");
						eliminar_buffer(buffer);
						break;
					}

					// Libero memoria.
					eliminar_buffer(buffer);
					free(dato_write);
					break;

				case SOLICITUD_LECTURA:
					// TIEMPO DE RETARDO
					usleep(RETARDO_RESPUESTA*1000);
					
					// Creo estructuras necesarias.
					int direc_fisica_read, bytes_read;
					void* dato_read;

					// Desempaqueto y almaceno información recibida.
					buffer_desempaquetar(buffer, &pid_es);
					buffer_desempaquetar(buffer, &direc_fisica_read);
					buffer_desempaquetar(buffer, &bytes_read);
					dato_read = malloc(bytes_read);

					// Leo el espacio de usuario de la memoria.
					leer(direc_fisica_read, bytes_read, dato_read);

					// Log mínimo y obligatorio - Acceso a espacio de usuario.
					printf("Log mínimo y obligatorio - Acceso a espacio de usuario\n");
					log_info(memoria_logger_min_y_obl, "PID: %d - Accion: LEER - Direccion fisica: %d - Tamaño: %d", pid_es, direc_fisica_read, bytes_read);

					// Envío dato leído.
					t_paquete* paquete_read = crear_paquete(DATO);
					agregar_a_paquete(paquete_read, dato_read, bytes_read);
					if(enviar_paquete(fd_entradasalida, paquete_read) < 0){
						// Se ha desconectado el módulo de entrada/salida
						log_info(memoria_logger, "MEMORIA: Modulo ENTRADASALIDA desconectado.");
						eliminar_buffer(buffer);
						break;
					}

					// Libero memoria.
					eliminar_buffer(buffer);
					eliminar_paquete(paquete_read);
					free(dato_read);
					break;

				default:
					log_warning(memoria_logger, "MEMORIA: Operacion desconocida recibida de ENTRADASALIDA");
					break;
			}
	    }
	    else{
			// Se ha desconectado el módulo de entrada/salida
			log_info(memoria_logger, "MEMORIA: Modulo ENTRADASALIDA desconectado.");
			eliminar_buffer(buffer);
			break;
		}
	}
}
