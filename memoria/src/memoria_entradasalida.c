#include "../include/memoria_entradasalida.h"

int cantidad_procesos = 0; // Definición de las variables globales
t_pcb_memoria* obtener_proceso(int pid); // Declaración de la función obtener_proceso


void atender_memoria_entradasalida(){
    int continuar = 1;
	while( continuar ){
		t_codigo_operacion cod_op;
		t_buffer *buffer = crear_buffer();
		recibir_paquete(fd_entradasalida, &cod_op, buffer);

        uint32_t direccion_fisica; //solamente recibo la direccion fìsica y tamanio y el vALOR ni necesito el pid
        int tamanio; //tamanio de datos a almacenar
        void* data; // almacena datos que se van a escribir en memoria después de ser desempaquetados.

		switch(cod_op){ //hasta acá está bien
			//case SOLICITUD_ACCESO_TABLAS_PAGINAS: //POSIBLE NOMBRE DEL MENSAJE 

			case SOLICITUD_ESCRITURA: //desp confirmar el nombre con lucho (YA LO PUSE EN EL CONEXION.H)
                // TIEMPO DE RETARDO
                usleep(RETARDO_REPUESTA);

                buffer_desempaquetar_proceso(buffer, &direccion_fisica); //manda lo que quiere escribir(char*), la dire fisica (aka frame y desplazamiento) y el tamanio
                buffer_desempaquetar(buffer, &tamanio);
                data = malloc(tamanio);
                buffer_desempaquetar(buffer, data);//guarda lo del buffer en data
                int resultado_escritura = escribir_memoria(direccion_fisica, data, tamanio);
                // Manejo de logs y resultado
                if (resultado_escritura == 0) {
                    log_info(memoria_logger, "Escritura exitosa: Direccion Fisica %u", direccion_fisica);}
                else {
                    log_error(memoria_logger, "Error en escritura: Direccion Fisica %u", direccion_fisica);}

                // Limpiar recursos
                free(data);
                eliminar_buffer(buffer);

                // Preparar y enviar el paquete de respuesta
                t_paquete* paquete = crear_paquete(MENSAJE); //preparo el paquete para que avise con el OK que salió bien la operación
                char* mensaje = "OK";
                agregar_a_paquete(paquete, mensaje, sizeof(char) * 2 + 1); //el 2 es por el "o" "k" y el +1 por el caracter nulo que finaliza la palabra
                enviar_paquete(fd_entradasalida, paquete);
                eliminar_paquete(paquete);
                break;

			case SOLICITUD_LECTURA:
                // TIEMPO DE RETARDO
                usleep(RETARDO_REPUESTA);
                
                buffer_desempaquetar(buffer, &direccion_fisica);
                buffer_desempaquetar(buffer, &tamanio);

                // Asignar memoria para los datos a leer
                data = malloc(tamanio);
                // Leer los datos desde la memoria
                int resultado_lectura = leer_memoria(direccion_fisica, data, tamanio);
                // Manejo de logs y resultado
                if (resultado_lectura == 0) {
                    log_info(memoria_logger, "Lectura exitosa: Direccion Fisica %u", direccion_fisica);} 
                else {
                    log_error(memoria_logger, "Error en lectura: Direccion Fisica %u", direccion_fisica);}

                // Preparar el paquete de respuesta con los datos leídos
                t_paquete* paquete_respuesta = crear_paquete(MENSAJE);
                agregar_a_paquete(paquete_respuesta, data, tamanio);
                
                // Enviar el paquete de respuesta
                enviar_paquete(fd_entradasalida, paquete_respuesta);
                
                // Limpiar recursos
                free(data);
                eliminar_buffer(buffer);
                eliminar_paquete(paquete_respuesta);
                break;

			default:
				log_warning(memoria_logger, "MEMORIA: Operacion desconocida recibida de ENTRADASALIDA");
				break;
		}
	}
}

int escribir_memoria(uint32_t direccion_fisica, void* buffer, int tamanio) {
    // Verificar que la escritura no exceda los límites de la memoria
    if (direccion_fisica + tamanio > TAM_MEMORIA) {
        fprintf(stderr, "Escritura fuera de los límites de la memoria\n");
        return -1;
    }

    // Realizar la escritura en la memoria
    pthread_mutex_lock(&mutex_espacio_usuario);
    memcpy(espacio_usuario + direccion_fisica, buffer, tamanio);
    pthread_mutex_unlock(&mutex_espacio_usuario);
    
    return 0;
}

int leer_memoria(uint32_t direccion_fisica, void* buffer, int tamanio) {
    // Verificar que la lectura no exceda los límites de la memoria
    if (direccion_fisica + tamanio > TAM_MEMORIA) {
        fprintf(stderr, "Lectura fuera de los límites de la memoria\n");
        return -1;
    }

    // Realizar la lectura desde la memoria
    pthread_mutex_lock(&mutex_espacio_usuario);
    memcpy(buffer, espacio_usuario + direccion_fisica, tamanio);
    pthread_mutex_unlock(&mutex_espacio_usuario);

    return 0;
} 