#include "../include/kernel_entradasalida.h"


void atender_kernel_interfaz(void *nombre_interfaz){
	t_interfaz *interfaz = diccionario_interfaces_get_interfaz(diccionario_interfaces, (char *)nombre_interfaz);
	int fd_interfaz = interfaz_get_fd(interfaz);

	t_codigo_operacion codigo_operacion;
	while(1){
		if( recibir_codigo_operacion(fd_interfaz, &codigo_operacion) > 0 ){
			switch(codigo_operacion){
				case IO_FIN_OPERACION:
					// Desbloquear proceso
					t_pcb *pcb = interfaz_desencolar_primer_proceso(interfaz);
					estado_desencolar_pcb_por_pid(estado_blocked, pcb_get_pid(pcb));
					proceso_a_ready(pcb);
					break;
				default:
					log_warning(kernel_logger, "KERNEL: Operacion desconocida recibida de ENTRADASALIDA");
			}
		}
		else{
			log_info(kernel_logger, "La interfaz %s se desconecto!", (char *)nombre_interfaz);
			// Logica de eliminar la interfaz
			return;
		}
	}
}

// INTERFAZ --------------------------------------------------

t_interfaz *crear_interfaz(int fd_interfaz, t_tipo_interfaz tipo_interfaz){
	t_interfaz *interfaz = malloc(sizeof(t_interfaz));
	if( interfaz == NULL ){
        log_error(kernel_logger, "Error al asignar memoria para el INTERFAZ");
        return NULL;
    }

	interfaz->tipo_interfaz = tipo_interfaz;
    interfaz->fd_interfaz = fd_interfaz;
    interfaz->estado_interfaz = crear_estado(INTERFAZ);

    return interfaz;
}

void eliminar_interfaz(t_interfaz *interfaz){
    if( interfaz != NULL ){
        if( interfaz->estado_interfaz != NULL ){
            eliminar_estado(interfaz->estado_interfaz);
        }

        free(interfaz);
    }
}

int interfaz_get_fd(t_interfaz *interfaz){
	return interfaz->fd_interfaz;
}

t_estado *interfaz_get_estado_interfaz(t_interfaz *interfaz){
	return interfaz->estado_interfaz;
}

t_pcb *interfaz_desencolar_primer_proceso(t_interfaz *interfaz){
    return estado_desencolar_primer_pcb(interfaz_get_estado_interfaz(interfaz));
}

void interfaz_encolar_proceso(t_interfaz *interfaz, t_pcb *pcb){
    estado_encolar_pcb(interfaz_get_estado_interfaz(interfaz), pcb);
}

// DICCIONARIO INTERFACES --------------------------------------------------

t_interfaz *diccionario_interfaces_get_interfaz(t_dictionary *diccionario_interfaces, char *nombre_interfaz){
	return dictionary_get(diccionario_interfaces, nombre_interfaz);
}

bool diccionario_interfaces_existe_interfaz(t_dictionary *diccionario_interfaces, char *nombre_interfaz){
	return dictionary_has_key(diccionario_interfaces, nombre_interfaz);
}