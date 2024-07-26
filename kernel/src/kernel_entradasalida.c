#include "../include/kernel_entradasalida.h"

void atender_kernel_interfaz(void *nombre_interfaz){
	pthread_mutex_lock(&mutex_diccionario_interfaces);
    t_interfaz *interfaz = diccionario_interfaces_get_interfaz(diccionario_interfaces, (char *)nombre_interfaz);
	pthread_mutex_unlock(&mutex_diccionario_interfaces);
    int fd_interfaz = interfaz_get_fd(interfaz);

	t_codigo_operacion codigo_operacion;
	while(1){
		t_solicitud_io *solicitud_io = interfaz_desencolar_primera_solicitud_io(interfaz);
        t_pcb *pcb = solicitud_io_get_pcb(solicitud_io);
        if( pcb != NULL && !diccionario_procesos_a_finalizar_proceso_esta_pendiente_de_finalizar(diccionario_procesos_a_finalizar, pcb) ){ // Si pcb == NULL => significa que el proceso fue finalizado mientras esperaba a que se ejecute su solicitud de io, por lo que descarto la solicitud. Si esta pendiente de finalizar significa que fue trasladado a exit, pero aun no se liberaron todos sus recursos ni se elimino el pcb. Necesito q en cualquiera de los casos, no ejecute la solicitud.
            if( enviar_paquete(fd_interfaz, solicitud_io_get_paquete_solicitud_io(solicitud_io)) > 0 ){
                eliminar_solicitud_io(solicitud_io);
                if( recibir_codigo_operacion(fd_interfaz, &codigo_operacion) > 0 ){
                    switch(codigo_operacion){
                        case IO_FIN_OPERACION:
                            sem_wait(&sem_estado_planificacion_blocked_to_ready_interfaz);
                            if( pcb != NULL && !diccionario_procesos_a_finalizar_proceso_esta_pendiente_de_finalizar(diccionario_procesos_a_finalizar, pcb) ){ // Si pcb == NULL => significa que el proceso fue finalizado mientras esperaba a que se ejecute su solicitud de io, por lo que descarto la solicitud. Si esta pendiente de finalizar significa que fue trasladado a exit, pero aun no se liberaron todos sus recursos ni se elimino el pcb. Necesito q en cualquiera de los casos, no ejecute la solicitud.
                                // Desbloquear proceso
                                sem_wait(estado_get_sem(estado_blocked)); // debe haber elementos en la lista para poder desencolar (deberia estar si o si en la lista, pues aun esta bloqueado. Y si no esta es porq fue finalizado y no deberia haber ingresado en esta seccion)
                                estado_desencolar_pcb_por_pid(estado_blocked, pcb_get_pid(pcb));
                                // ACA PODRIA HACER UN IF(ESTA_EN_LA_BLACK_LIST(PCB)){LO MATO}ELSE{ PROCESO_A_READY(PCB)}
                                if( pcb_get_quantum_restante(pcb) == QUANTUM ){ // es FIFO o RR
                                    proceso_a_ready(pcb);
                                }
                                else{ // es VRR
                                    if( pcb_get_quantum_restante(pcb) > 0 ){ // Le sobro quantum
                                        pcb_set_quantum_inicial(pcb, pcb_get_quantum_restante(pcb));
                                        proceso_a_ready_plus(pcb);
                                    }
                                    else{ // Se quedo sin quantum justo cuando pidio la IO
                                        pcb_set_quantum_inicial(pcb, QUANTUM);
                                        pcb_set_quantum_restante(pcb, QUANTUM);
                                        proceso_a_ready(pcb);
                                    }
                                }
                            }
                            sem_post(&sem_estado_planificacion_blocked_to_ready_interfaz);
                            break;
                        default:
                            log_warning(kernel_logger, "KERNEL: Operacion desconocida recibida de ENTRADASALIDA");
                    }
                }
                else{
                    log_info(kernel_logger, "La interfaz %s se desconecto!", (char *)nombre_interfaz);

                    if( pcb != NULL && !diccionario_procesos_a_finalizar_proceso_esta_pendiente_de_finalizar(diccionario_procesos_a_finalizar, pcb) ){ // Si pcb == NULL o esta pendiente de finalizar => significa que el proceso fue finalizado mientras se hacia la io, por lo que envio a exit al proceso solo si todavia no fue finalizado
                        // Envio al proceso que estaba esperando por la io a exit
                        proceso_a_exit(pcb, FINALIZACION_DESCONEXION_INTERFAZ);
                        // sem_post(&sem_grado_multiprogramacion);
                    }

                    // Elimino la interfaz del diccionario (siempre y cuando nadie haya accedido al diccionario ni se este usando la instancia de t_interfaz)
                    pthread_mutex_lock(&mutex_diccionario_interfaces);
                    sem_wait(interfaz_get_sem_control_uso_interfaz(interfaz));
                    diccionario_interfaces_eliminar_interfaz(diccionario_interfaces, nombre_interfaz);
                    // el post del semaforo no lo hago porq para este momento la interfaz ya no existe y,en consecuencia, el semaforo tmp
                    pthread_mutex_unlock(&mutex_diccionario_interfaces);

                    break;
                }
            }
            else{
                log_info(kernel_logger, "La interfaz %s se desconecto!", (char *)nombre_interfaz);
                
                // Elimino la solicitud
                eliminar_solicitud_io(solicitud_io);

                if( pcb != NULL && !diccionario_procesos_a_finalizar_proceso_esta_pendiente_de_finalizar(diccionario_procesos_a_finalizar, pcb) ){ // Si pcb == NULL o esta pendiente de finalizar => significa que el proceso fue finalizado mientras se hacia el envio del paquete, por lo que envio a exit al proceso solo si todavia no fue finalizado
                    // Envio al proceso que estaba esperando por la io a exit
                    proceso_a_exit(pcb, FINALIZACION_DESCONEXION_INTERFAZ);
                    // sem_post(&sem_grado_multiprogramacion);
                }

                // Elimino la interfaz del diccionario (siempre y cuando nadie haya accedido al diccionario ni se este usando la instancia de t_interfaz)
                pthread_mutex_lock(&mutex_diccionario_interfaces);
                sem_wait(interfaz_get_sem_control_uso_interfaz(interfaz));
                diccionario_interfaces_eliminar_interfaz(diccionario_interfaces, nombre_interfaz);
                // el post del semaforo no lo hago porq para este momento la interfaz ya no existe y,en consecuencia, el semaforo tmp
                pthread_mutex_unlock(&mutex_diccionario_interfaces);

                break;
            }
        }
        else{
            eliminar_solicitud_io(solicitud_io);
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
    interfaz->lista_solicitudes_io = list_create();

    pthread_mutex_t *mutex = malloc(sizeof(pthread_mutex_t));
    if( mutex == NULL ){
        log_error(kernel_logger, "Error al asignar memoria para el MUTEX de la INTERFAZ");
        return NULL;
    }
    pthread_mutex_init(mutex, NULL);
    interfaz->mutex_lista_solicitudes_io = mutex;

    sem_t *sem_lsi = malloc(sizeof(sem_t));
    if( sem_lsi == NULL ){
        log_error(kernel_logger, "Error al asignar memoria para el SEMAFORO de la INTERFAZ");
        return NULL;
    }
    sem_init(sem_lsi, 0, 0); // La lista arranca vacia
    interfaz->sem_lista_solicitudes_io = sem_lsi;

    sem_t *sem_cui = malloc(sizeof(sem_t));
    if( sem_cui == NULL ){
        log_error(kernel_logger, "Error al asignar memoria para el SEMAFORO de la INTERFAZ");
        return NULL;
    }
    sem_init(sem_cui, 0, 1); // Si planificadores usa la interfaz => no se puede eliminar a la misma (por eso 1 la inicializacion)
    interfaz->sem_control_uso_interfaz = sem_cui;

    return interfaz;
}

void eliminar_interfaz(void *interfaz_void){
    t_interfaz *interfaz = (t_interfaz *)interfaz_void;
    
    if( interfaz != NULL ){
        if( !list_is_empty(interfaz->lista_solicitudes_io) ){
            int size = list_size(interfaz->lista_solicitudes_io);
            for(int i = 0; i < size; i++){
                t_solicitud_io *solicitud_io = interfaz_desencolar_primera_solicitud_io(interfaz);
                t_pcb *pcb = solicitud_io_get_pcb(solicitud_io);
                
                // Mando el proceso a exit
                proceso_a_exit(pcb, FINALIZACION_DESCONEXION_INTERFAZ);
                // sem_post(&sem_grado_multiprogramacion);

                // Elimino la solicitud
                eliminar_solicitud_io(solicitud_io);
            }
        }
        list_destroy(interfaz->lista_solicitudes_io);

        pthread_mutex_destroy(interfaz->mutex_lista_solicitudes_io);
        free(interfaz->mutex_lista_solicitudes_io);

        sem_destroy(interfaz->sem_lista_solicitudes_io);
        free(interfaz->sem_lista_solicitudes_io);

        sem_destroy(interfaz->sem_control_uso_interfaz);
        free(interfaz->sem_control_uso_interfaz);

        free(interfaz);
    }
}

t_tipo_interfaz interfaz_get_tipo_interfaz(t_interfaz *interfaz){
	return interfaz->tipo_interfaz;
}

int interfaz_get_fd(t_interfaz *interfaz){
	return interfaz->fd_interfaz;
}

t_list *interfaz_get_lista_solicitudes_io(t_interfaz *interfaz){
	return interfaz->lista_solicitudes_io;
}

pthread_mutex_t *interfaz_get_mutex_lista_solicitudes_io(t_interfaz *interfaz){
	return interfaz->mutex_lista_solicitudes_io;
}

sem_t *interfaz_get_sem_lista_solicitudes_io(t_interfaz *interfaz){
    return interfaz->sem_lista_solicitudes_io;
}

sem_t *interfaz_get_sem_control_uso_interfaz(t_interfaz *interfaz){
    return interfaz->sem_control_uso_interfaz;
}

t_solicitud_io *interfaz_desencolar_primera_solicitud_io(t_interfaz *interfaz){
	sem_wait(interfaz_get_sem_lista_solicitudes_io(interfaz)); // debe haber elementos en la lista para poder desencolar
    pthread_mutex_lock(interfaz_get_mutex_lista_solicitudes_io(interfaz));
    t_solicitud_io *solicitud_io = list_remove(interfaz_get_lista_solicitudes_io(interfaz), 0);
    pthread_mutex_unlock(interfaz_get_mutex_lista_solicitudes_io(interfaz));

    return solicitud_io;
}

void interfaz_encolar_solicitud_io(t_interfaz *interfaz, t_solicitud_io *solicitud_io){
	pthread_mutex_lock(interfaz_get_mutex_lista_solicitudes_io(interfaz));
    list_add(interfaz_get_lista_solicitudes_io(interfaz), solicitud_io);
    pthread_mutex_unlock(interfaz_get_mutex_lista_solicitudes_io(interfaz));
    sem_post(interfaz_get_sem_lista_solicitudes_io(interfaz));
}

// DICCIONARIO INTERFACES --------------------------------------------------

t_interfaz *diccionario_interfaces_get_interfaz(t_dictionary *diccionario_interfaces, char *nombre_interfaz){
	return dictionary_get(diccionario_interfaces, nombre_interfaz);
}

bool diccionario_interfaces_existe_interfaz(t_dictionary *diccionario_interfaces, char *nombre_interfaz){
	return dictionary_has_key(diccionario_interfaces, nombre_interfaz);
}

void diccionario_interfaces_eliminar_interfaz(t_dictionary *diccionario_interfaces, char *nombre_interfaz){
    dictionary_remove_and_destroy(diccionario_interfaces, nombre_interfaz, eliminar_interfaz);
}

// SOLICITUD IO
t_solicitud_io *crear_solicitud_io(t_pcb *pcb, t_paquete *paquete_solicitud_io){
    t_solicitud_io *solicitud_io = malloc(sizeof(t_solicitud_io));
    if( solicitud_io == NULL ){
        log_error(kernel_logger, "Error al asignar memoria para el ESTADO");
        return NULL;
    }

    solicitud_io->pcb = pcb;
    solicitud_io->paquete_solicitud_io = paquete_solicitud_io;

    return solicitud_io;
}

void eliminar_solicitud_io(t_solicitud_io *solicitud_io){
    if( solicitud_io != NULL ){
		if( solicitud_io->paquete_solicitud_io != NULL ){
			eliminar_paquete(solicitud_io->paquete_solicitud_io);
		}

		free(solicitud_io);
    }
}

t_pcb *solicitud_io_get_pcb(t_solicitud_io *solicitud_io){
    return solicitud_io->pcb;
}

t_paquete *solicitud_io_get_paquete_solicitud_io(t_solicitud_io *solicitud_io){
    return solicitud_io->paquete_solicitud_io;
}