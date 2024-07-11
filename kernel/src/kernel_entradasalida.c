// #include "../include/kernel_entradasalida.h"


// void atender_kernel_interfaz(void *nombre_interfaz){
// 	// sem wait
//     t_interfaz *interfaz = diccionario_interfaces_get_interfaz(diccionario_interfaces, (char *)nombre_interfaz);
// 	// sem post
//     int fd_interfaz = interfaz_get_fd(interfaz);

// 	t_codigo_operacion codigo_operacion;
// 	while(1){
// 		t_solicitud_io *solicitud_io = interfaz_desencolar_primera_solicitud_io(interfaz);
//         t_pcb *pcb = solicitud_io_get_pcb(solicitud_io);
//         if( enviar_paquete(fd_interfaz, solicitud_io_get_paquete_solicitud_io(solicitud_io)) > 0 ){
//             eliminar_solicitud_io(solicitud_io);
//             if( recibir_codigo_operacion(fd_interfaz, &codigo_operacion) > 0 ){
//                 switch(codigo_operacion){
//                     case IO_FIN_OPERACION:
//                         // Desbloquear proceso
//                         estado_desencolar_pcb_por_pid(estado_blocked, pcb_get_pid(pcb));
//                         // ACA PROBABLEMENTE TENGA Q HACER UN IF( LE_QUEDA_QUANTUM(PCB) ){ PROCESO_A_READY_PLUS } ELSE{ PROCESO_A_READY }
//                         proceso_a_ready(pcb);
//                         break;
//                     default:
//                         log_warning(kernel_logger, "KERNEL: Operacion desconocida recibida de ENTRADASALIDA");
//                 }
// 		    }
//             else{
//                 log_info(kernel_logger, "La interfaz %s se desconecto!", (char *)nombre_interfaz);
//                 eliminar_solicitud_io(solicitud_io);
//                 // wait
//                 // Logica de eliminar la interfaz
//                 // ...
//                 // post

//                 proceso_a_exit(pcb, FINALIZACION_INVALID_INTERFACE);
//                 // sem_post(&sem_grado_multiprogramacion);
//             }
//         }
//         else{
//             log_info(kernel_logger, "La interfaz %s se desconecto!", (char *)nombre_interfaz);
//             eliminar_solicitud_io(solicitud_io);
//             // wait
//             // Logica de eliminar la interfaz
//             // ...
//             // post

//             proceso_a_exit(pcb, FINALIZACION_INVALID_INTERFACE);
//             // sem_post(&sem_grado_multiprogramacion);
//         }
// 	}
// }

// // INTERFAZ --------------------------------------------------

// t_interfaz *crear_interfaz(int fd_interfaz, t_tipo_interfaz tipo_interfaz){
// 	t_interfaz *interfaz = malloc(sizeof(t_interfaz));
// 	if( interfaz == NULL ){
//         log_error(kernel_logger, "Error al asignar memoria para el INTERFAZ");
//         return NULL;
//     }

// 	interfaz->tipo_interfaz = tipo_interfaz;
//     interfaz->fd_interfaz = fd_interfaz;
//     interfaz->lista_solicitudes_io = list_create();

//     pthread_mutex_t *mutex = malloc(sizeof(pthread_mutex_t));
//     if( mutex == NULL ){
//         log_error(kernel_logger, "Error al asignar memoria para el MUTEX de la INTERFAZ");
//         return NULL;
//     }
//     pthread_mutex_init(mutex, NULL);
//     interfaz->mutex_interfaz = mutex;

//     sem_t *sem = malloc(sizeof(sem_t));
//     if( sem == NULL ){
//         log_error(kernel_logger, "Error al asignar memoria para el SEMAFORO de la INTERFAZ");
//         return NULL;
//     }
//     sem_init(sem, 0, 0); // La lista arranca vacia
//     interfaz->sem_interfaz = sem;

//     return interfaz;
// }

// void eliminar_interfaz(void *interfaz_void){
//     t_interfaz *interfaz = (t_interfaz *)interfaz_void;
    
//     if( interfaz != NULL ){
//         if( !list_is_empty(interfaz->lista_solicitudes_io) ){
//             for(int i = 0; i < list_size(interfaz->lista_solicitudes_io); i++){
//                 t_solicitud_io *solicitud_io = interfaz_desencolar_primera_solicitud_io(interfaz);
//                 t_pcb *pcb = solicitud_io_get_pcb(solicitud_io);
                
//                 // Mando el proceso a exit
//                 proceso_a_exit(pcb, FINALIZACION_INVALID_INTERFACE);
//                 // sem_post(&sem_grado_multiprogramacion);

//                 // Elimino la solicitud
//                 eliminar_solicitud_io(solicitud_io);
//             }
//         }
//         list_destroy(interfaz->lista_solicitudes_io);

//         pthread_mutex_destroy(interfaz->mutex_interfaz);
//         free(interfaz->mutex_interfaz);

//         sem_destroy(interfaz->sem_interfaz);
//         free(interfaz->sem_interfaz);

//         free(interfaz);
//     }
// }

// void solicitud_io_destroyer(void *solicitud_io){
//     t_solicitud_io *temp_solicitud_io = (t_solicitud_io *)solicitud_io;
//     eliminar_solicitud_io(temp_solicitud_io);
// }

// t_tipo_interfaz interfaz_get_tipo_interfaz(t_interfaz *interfaz){
// 	return interfaz->tipo_interfaz;
// }

// int interfaz_get_fd(t_interfaz *interfaz){
// 	return interfaz->fd_interfaz;
// }

// t_list *interfaz_get_lista_solicitudes_io(t_interfaz *interfaz){
// 	return interfaz->lista_solicitudes_io;
// }

// pthread_mutex_t *interfaz_get_mutex(t_interfaz *interfaz){
// 	return interfaz->mutex_interfaz;
// }

// sem_t *interfaz_get_sem(t_interfaz *interfaz){
//     return interfaz->sem_interfaz;
// }

// t_solicitud_io *interfaz_desencolar_primera_solicitud_io(t_interfaz *interfaz){
// 	sem_wait(interfaz_get_sem(interfaz)); // debe haber elementos en la lista para poder desencolar
//     pthread_mutex_lock(interfaz_get_mutex(interfaz));
//     t_solicitud_io *solicitud_io = list_remove(interfaz_get_lista_solicitudes_io(interfaz), 0);
//     pthread_mutex_unlock(interfaz_get_mutex(interfaz));

//     return solicitud_io;
// }

// void interfaz_encolar_solicitud_io(t_interfaz *interfaz, t_solicitud_io *solicitud_io){
// 	pthread_mutex_lock(interfaz_get_mutex(interfaz));
//     list_add(interfaz_get_lista_solicitudes_io(interfaz), solicitud_io);
//     pthread_mutex_unlock(interfaz_get_mutex(interfaz));
//     sem_post(interfaz_get_sem(interfaz));
// }

// // DICCIONARIO INTERFACES --------------------------------------------------

// t_interfaz *diccionario_interfaces_get_interfaz(t_dictionary *diccionario_interfaces, char *nombre_interfaz){
// 	return dictionary_get(diccionario_interfaces, nombre_interfaz);
// }

// bool diccionario_interfaces_existe_interfaz(t_dictionary *diccionario_interfaces, char *nombre_interfaz){
// 	return dictionary_has_key(diccionario_interfaces, nombre_interfaz);
// }

// void diccionario_interfaces_eliminar_interfaz(char *nombre_interfaz){
//     dictionary_remove_and_destroy(diccionario_interfaces, nombre_interfaz, eliminar_interfaz);
// }

// // SOLICITUD IO
// t_solicitud_io *crear_solicitud_io(t_pcb *pcb, t_paquete *paquete_solicitud_io){
//     t_solicitud_io *solicitud_io = malloc(sizeof(t_solicitud_io));
//     if( solicitud_io == NULL ){
//         log_error(kernel_logger, "Error al asignar memoria para el ESTADO");
//         return NULL;
//     }

//     solicitud_io->pcb = pcb;
//     solicitud_io->paquete_solicitud_io = paquete_solicitud_io;

//     return solicitud_io;
// }

// void eliminar_solicitud_io(t_solicitud_io *solicitud_io){
//     if( solicitud_io != NULL ){
// 		if( solicitud_io->paquete_solicitud_io != NULL ){
// 			eliminar_paquete(solicitud_io->paquete_solicitud_io);
// 		}

// 		free(solicitud_io);
//     }
// }

// t_pcb *solicitud_io_get_pcb(t_solicitud_io *solicitud_io){
//     return solicitud_io->pcb;
// }

// t_paquete *solicitud_io_get_paquete_solicitud_io(t_solicitud_io *solicitud_io){
//     return solicitud_io->paquete_solicitud_io;
// }