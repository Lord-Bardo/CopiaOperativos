#include "../include/kernel_planificadores.h"

// VARIABLES GLOBALES
// Planificacion
t_estado_planificacion estado_planificacion;
sem_t sem_estado_planificacion_new_to_ready;
sem_t sem_estado_planificacion_ready_to_exec;
sem_t sem_estado_planificacion_blocked_to_ready;
sem_t sem_estado_planificacion_exec_to_exec_or_ready_or_blocked;

// Estados
t_estado *estado_new;
t_estado *estado_ready;
t_estado *estado_ready_plus;
t_estado *estado_exec;
t_estado *estado_blocked;
t_estado *estado_exit;

// PID
int pid_actual;
pthread_mutex_t mutex_pid;

// Grado multiprogramacion
sem_t sem_grado_multiprogramacion;
sem_t sem_control_cambio_grado_multiprogramacion;

// Recursos
t_dictionary *diccionario_recursos;
pthread_mutex_t mutex_diccionario_recursos;

// Interfaces
t_dictionary *diccionario_interfaces;
pthread_mutex_t mutex_diccionario_interfaces;

// Semaforos sockets
pthread_mutex_t mutex_socket_memoria;

// INICIALIZACION PLANIFICADORES ------------------------------------------
void iniciar_planificadores(){
    // Estructuras
    inicializar_estructuras();

    // Planificador largo plazo
    pthread_t hilo_planificador_largo_plazo;
    pthread_create(&hilo_planificador_largo_plazo, NULL, (void*)planificador_largo_plazo, NULL);
    pthread_detach(hilo_planificador_largo_plazo);

    // Planificador corto plazo
    pthread_t hilo_planificador_corto_plazo;
    pthread_create(&hilo_planificador_corto_plazo, NULL, (void*)planificador_corto_plazo, NULL);
    pthread_detach(hilo_planificador_corto_plazo);

    iniciar_planificacion();
}

void inicializar_estructuras(){
    inicializar_estructuras_planificacion();
    inicializar_estructuras_estados();
    inicializar_estructuras_pid();
    inicializar_estructuras_grado_multiprogramacion();
    inicializar_estructuras_diccionarios();
    inicializar_estructuras_sockets();
}

void inicializar_estructuras_planificacion(){
    estado_planificacion = PAUSADA;
    sem_init(&sem_estado_planificacion_new_to_ready, 0, 0);
    sem_init(&sem_estado_planificacion_ready_to_exec, 0, 0);
    sem_init(&sem_estado_planificacion_exec_to_exec_or_ready_or_blocked, 0, 0);
    sem_init(&sem_estado_planificacion_blocked_to_ready, 0, 0); 
}

void inicializar_estructuras_estados(){
    estado_new = crear_estado(NEW);
    estado_ready = crear_estado(READY);
    estado_ready_plus = crear_estado(READY_PLUS);
    estado_exec = crear_estado(EXEC);
    estado_blocked = crear_estado(BLOCKED);
    estado_exit = crear_estado(EXIT);
}

void inicializar_estructuras_pid(void){
    pid_actual = 0;
    pthread_mutex_init(&mutex_pid, NULL);
}

void inicializar_estructuras_grado_multiprogramacion(){
    sem_init(&sem_grado_multiprogramacion, 0, GRADO_MULTIPROGRAMACION);
    sem_init(&sem_control_cambio_grado_multiprogramacion, 0, 1);
}

void inicializar_estructuras_diccionarios(){
    // Diccionario recursos
    diccionario_recursos = crear_diccionario_recursos(RECURSOS, INSTANCIAS_RECURSOS);
    pthread_mutex_init(&mutex_diccionario_recursos, NULL);
    // Diccionario interfaces
    diccionario_interfaces = dictionary_create();
    pthread_mutex_init(&mutex_diccionario_interfaces, NULL);
}

void inicializar_estructuras_sockets(){
    // Socket memoria
    pthread_mutex_init(&mutex_socket_memoria, NULL);
}

void iniciar_planificacion(){
    if( estado_planificacion == PAUSADA ){
        estado_planificacion = ACTIVA;
        sem_post(&sem_estado_planificacion_new_to_ready);
        sem_post(&sem_estado_planificacion_ready_to_exec);
        sem_post(&sem_estado_planificacion_blocked_to_ready);
        sem_post(&sem_estado_planificacion_exec_to_exec_or_ready_or_blocked);
    }
}

void detener_planificacion(){
    if( estado_planificacion == ACTIVA ){
        estado_planificacion = PAUSADA;
        pthread_t hilo_detener_planificacion_new_to_ready, hilo_detener_planificacion_ready_to_exec, hilo_detener_planificacion_blocked_to_ready, hilo_detener_planificacion_exec_to_exec_or_ready_or_blocked;
        pthread_create(&hilo_detener_planificacion_new_to_ready, NULL, (void *)detener_planificacion_new_to_ready, NULL);
        pthread_create(&hilo_detener_planificacion_ready_to_exec, NULL, (void *)detener_planificacion_ready_to_exec, NULL);
        pthread_create(&hilo_detener_planificacion_blocked_to_ready, NULL, (void *)detener_planificacion_blocked_to_ready, NULL);
        pthread_create(&hilo_detener_planificacion_exec_to_exec_or_ready_or_blocked, NULL, (void *)detener_planificacion_exec_to_exec_or_ready_or_blocked, NULL);
        pthread_detach(hilo_detener_planificacion_new_to_ready);
        pthread_detach(hilo_detener_planificacion_ready_to_exec);
        pthread_detach(hilo_detener_planificacion_blocked_to_ready);
        pthread_detach(hilo_detener_planificacion_exec_to_exec_or_ready_or_blocked);
    }
}

void detener_planificacion_new_to_ready(){
    sem_wait(&sem_estado_planificacion_new_to_ready);
}

void detener_planificacion_ready_to_exec(){
    sem_wait(&sem_estado_planificacion_ready_to_exec);
}

void detener_planificacion_blocked_to_ready(){
    sem_wait(&sem_estado_planificacion_blocked_to_ready);
}

void detener_planificacion_exec_to_exec_or_ready_or_blocked(){
    sem_wait(&sem_estado_planificacion_exec_to_exec_or_ready_or_blocked);
}

void cambiar_grado_multiprogramacion_a(int nuevo_grado_multiprogramacion){
    sem_wait(&sem_control_cambio_grado_multiprogramacion);

    int diferencia = nuevo_grado_multiprogramacion - GRADO_MULTIPROGRAMACION;
    GRADO_MULTIPROGRAMACION = nuevo_grado_multiprogramacion;
    
    if( diferencia > 0 ){
        for(int i = 0; i < diferencia; i++){
            sem_post(&sem_grado_multiprogramacion);
        }
    }
    else if( diferencia < 0 ){
        for(int i = 0; i < -diferencia; i++){
            sem_wait(&sem_grado_multiprogramacion);
        }
    }

    sem_post(&sem_control_cambio_grado_multiprogramacion);
}

// PLANIFICADOR CORTO PLAZO ---------------------------------------------------

void planificador_corto_plazo(){    
    if( strcmp(ALGORITMO_PLANIFICACION, "FIFO") == 0 ){
        planificador_corto_plazo_fifo();
    }
    else if( strcmp(ALGORITMO_PLANIFICACION, "RR") == 0 ){
        planificador_corto_plazo_rr();
    }
    else if( strcmp(ALGORITMO_PLANIFICACION, "VRR") == 0 ){
        planificador_corto_plazo_vrr();
    }
    else{
        log_error(kernel_logger, "Algoritmo de planificacion invalido!");
    }
}

// FIFO
void planificador_corto_plazo_fifo(){
    while(1){
        sem_wait(&sem_estado_planificacion_ready_to_exec);
        t_pcb *pcb = elegir_proceso_segun_fifo();
        proceso_a_exec(pcb);
        enviar_contexto_de_ejecucion(pcb);
        recibir_contexto_de_ejecucion_actualizado(pcb);
        sem_post(&sem_estado_planificacion_ready_to_exec);
    }
}

t_pcb *elegir_proceso_segun_fifo(){
    return estado_desencolar_primer_pcb(estado_ready);
}

// RR
void planificador_corto_plazo_rr(){ //TERMINAR
    while( estado_planificacion ){
        t_pcb *pcb = elegir_proceso_segun_rr();
        proceso_a_exec(pcb);
        enviar_contexto_de_ejecucion(pcb);
        recibir_contexto_de_ejecucion_actualizado(pcb);
    }
}

t_pcb *elegir_proceso_segun_rr(){
    return estado_desencolar_primer_pcb(estado_ready); 
}

// VRR
void planificador_corto_plazo_vrr(){ //TERMINAR
    while( estado_planificacion ){
        t_pcb *pcb = elegir_proceso_segun_rr();
        proceso_a_exec(pcb);
        enviar_contexto_de_ejecucion(pcb);
        recibir_contexto_de_ejecucion_actualizado(pcb);
        // estado_ordenar_por_quantum_restante(estado_ready_plus); lo comente para q pueda compilar nomas
    }
}

t_pcb *elegir_proceso_segun_vrr(){
    if( estado_contiene_pcbs(estado_ready_plus) ){
        return estado_desencolar_primer_pcb(estado_ready_plus);
    }
    else{
        return estado_desencolar_primer_pcb(estado_ready);
    }
}

// void ejecutar_proceso(t_pcb *pcb){
//     enviar_contexto_de_ejecucion(pcb);
//     recibir_contexto_de_ejecucion_actualizado(pcb); // Modifica directo al pcb con lo que recibe
// }

// Manda a CPU el contexto de la ejecucion (pid y registros) por el Dispatch
void enviar_contexto_de_ejecucion(t_pcb *pcb){
    t_paquete *paquete_contexto_de_ejecucion = crear_paquete(CONTEXTO_DE_EJECUCION);
    agregar_contexto_ejecucion_a_paquete(paquete_contexto_de_ejecucion, pcb);
    enviar_paquete(fd_cpu_dispatch, paquete_contexto_de_ejecucion);
    eliminar_paquete(paquete_contexto_de_ejecucion);
}

void recibir_contexto_de_ejecucion_actualizado(t_pcb *pcb){ // TERMINAR
    // Espera por el Dispatch la llegada del contexto actualizado tras la ejecucion del proceso (pid y registros)
    // Junto con el contexto debe llegar el motivo por el cual finalizo la ejecucion (motivo de desalojo)
    // En cualquier caso se lo debe desencolar de EXEC -> NOTA: no necesariamente, si el proceso quiere hacer un signal deberia seguir ejecutando el mismo proceso
    // Si puede seguir ejecutando se lo encola en READY
    // Si no se bloqueo entonces se lo encola en BLOCKED
    t_codigo_operacion motivo_desalojo;
    t_buffer *buffer = crear_buffer();
    recibir_paquete(fd_cpu_dispatch, &motivo_desalojo, buffer);
    buffer_desempaquetar_contexto_ejecucion(buffer, pcb); // Modifica al pcb con lo que recibe
    
    sem_wait(&sem_estado_planificacion_exec_to_exec_or_ready_or_blocked);
    switch(motivo_desalojo){
        case SUCCESS:
            proceso_a_exit(pcb, FINALIZACION_SUCCESS);
            // sem_post(&sem_grado_multiprogramacion);
            break;
        case INTERRUPT_QUANTUM:
            break;
        case INTERRUPT_USER: // se mato al proceso por consola y justo estaba ejecutando
            proceso_a_exit(pcb, FINALIZACION_INTERRUPTED_BY_USER);
            // sem_post(&sem_grado_multiprogramacion);
            break;
        case OUT_OF_MEMORY: // Fallo el Resize
            proceso_a_exit(pcb, FINALIZACION_SUCCESS);
            // sem_post(&sem_grado_multiprogramacion);
            break;
        case WAIT:
        { // uso "bloques compuestos" para evitar los errores de redefinicion de variables -> cada bloque de estos es como q esta aparte, si no los pongo el compilador me toma al switch como una sola cosa y me tira error de redefinicion con nombre_recurso y demas
            char *nombre_recurso = buffer_desempaquetar_string(buffer);
            // Verifico que el recurso exista
            pthread_mutex_lock(&mutex_diccionario_recursos);
            if( !diccionario_recursos_existe_recurso(diccionario_recursos, nombre_recurso) ){
                pthread_mutex_unlock(&mutex_diccionario_recursos);
                proceso_a_exit(pcb, FINALIZACION_INVALID_RESOURCE);
                // sem_post(&sem_grado_multiprogramacion); -> Tengo q definir si lo hago en proceso_a_exit, cuando libero los recursos o lo hago siempre seguido al proceso_a_exit
            }
            else{
                pthread_mutex_unlock(&mutex_diccionario_recursos);
                ejecutar_instruccion_wait(pcb, nombre_recurso);
                if( pcb_get_estado(pcb) == EXEC ){
                    sem_post(&sem_estado_planificacion_exec_to_exec_or_ready_or_blocked);
                    // sem_wait(&sem_estado_planificacion_ready_to_exec); // nose q tan necesario es volver a testear aca el estado porq deberia ser transparente para el q esta ejecutando el corte de la planificacion -> el wait es parte de la ejecucion del proceso, por lo que parar la plani en medio de su ejecucion seria raro -> si no es necesario puedo usar ejecutar_proceso()
                    enviar_contexto_de_ejecucion(pcb);
                    // sem_post(&sem_estado_planificacion_ready_to_exec);
                    recibir_contexto_de_ejecucion_actualizado(pcb);
                }
            }
            break;
        }
        case SIGNAL:
        {
            char *nombre_recurso = buffer_desempaquetar_string(buffer);
            // Verifico que el recurso exista
            pthread_mutex_lock(&mutex_diccionario_recursos);
            if( !diccionario_recursos_existe_recurso(diccionario_recursos, nombre_recurso) ){
                pthread_mutex_unlock(&mutex_diccionario_recursos);
                proceso_a_exit(pcb, FINALIZACION_INVALID_RESOURCE);
                // sem_post(&sem_grado_multiprogramacion); -> Tengo q definir si lo hago en proceso_a_exit, cuando libero los recursos o lo hago siempre seguido al proceso_a_exit
            }
            else{
                pthread_mutex_unlock(&mutex_diccionario_recursos);
                ejecutar_instruccion_signal(pcb, nombre_recurso);
                // Despues del signal siempre sigue ejecutando el mismo proceso
                sem_post(&sem_estado_planificacion_exec_to_exec_or_ready_or_blocked);
                // sem_wait(&sem_estado_planificacion_ready_to_exec); // nose q tan necesario es volver a testear aca el estado porq deberia ser transparente para el q esta ejecutando el corte de la planificacion -> el wait es parte de la ejecucion del proceso, por lo que parar la plani en medio de su ejecucion seria raro -> si no es necesario puedo usar ejecutar_proceso()
                enviar_contexto_de_ejecucion(pcb);
                // sem_post(&sem_estado_planificacion_ready_to_exec);
                recibir_contexto_de_ejecucion_actualizado(pcb);
            }
            break;
        }
        case IO:
        {
            char *nombre_interfaz = buffer_desempaquetar_string(buffer);
            pthread_mutex_lock(&mutex_diccionario_interfaces);
            if( diccionario_interfaces_existe_interfaz(diccionario_interfaces, nombre_interfaz) ){ // la corroboracion de la conexion se hace al momento de mandar la operacion a la interfaz (si existe, es muy probable q siga conectada (salvo q se desconecte justo dsp de la corroboracion))
                // Obtengo la interfaz
                t_interfaz *interfaz = diccionario_interfaces_get_interfaz(diccionario_interfaces, nombre_interfaz);
                // Marco con el semaforo que kernel_planificadores ya tomo la interfaz -> esto para que no pueda ser eliminada mientras la uso -> la eliminacion es a causa de una desconexion (si se desconecta en el medio del armado del paquete y todo eso, no pasa nada -> lo habre hecho todo al dope nomas, pero cuando haga el post se va a eliminar todo correctamente)
                sem_wait(interfaz_get_sem_control_uso_interfaz(interfaz));
                pthread_mutex_unlock(&mutex_diccionario_interfaces);
                
                // Desempaqueto la operacion a realizar y corroboro que sea valida
                t_codigo_operacion operacion;
                buffer_desempaquetar(buffer, &operacion);
                switch( interfaz_get_tipo_interfaz(interfaz) ){
                    case GENERICA:
                        switch( operacion ){
                            case IO_GEN_SLEEP:
                                // Desempaqueto los parametros de la operacion
                                char *parametros = buffer_desempaquetar_string(buffer);

                                // Creo el paquete con la operacion a realizar y sus parametros
                                t_paquete *paquete_solicitud_io = crear_paquete(operacion);
                                agregar_string_a_paquete(paquete_solicitud_io, parametros);
                                
                                // Creo la solicitud de entrada salida
                                t_solicitud_io *solicitud_io = crear_solicitud_io(pcb, paquete_solicitud_io);
                                
                                // Bloqueo al proceso
                                proceso_a_blocked(pcb, nombre_interfaz);

                                // Encolo la solicitud
                                interfaz_encolar_solicitud_io(interfaz, solicitud_io);
                                break;
                            default:
                                proceso_a_exit(pcb, FINALIZACION_INVALID_INTERFACE);
                                // sem_post(&sem_grado_multiprogramacion);
                        }
                        break;
                    case STDIN:
                        break;
                    case STDOUT:
                        break;
                    case DIALFS:
                        break;
                    default:
                        proceso_a_exit(pcb, FINALIZACION_INVALID_INTERFACE);
                        // sem_post(&sem_grado_multiprogramacion);
                }
                sem_post(interfaz_get_sem_control_uso_interfaz(interfaz));
            }
            else{
                pthread_mutex_unlock(&mutex_diccionario_interfaces);
                proceso_a_exit(pcb, FINALIZACION_INVALID_INTERFACE);
                // sem_post(&sem_grado_multiprogramacion);
            }
            break;
        }
        default:
            log_error(kernel_logger, "Motivo de desalojo desconocido");
    }
    sem_post(&sem_estado_planificacion_exec_to_exec_or_ready_or_blocked);

    eliminar_buffer(buffer);
}

// PLANIFICADOR LARGO PLAZO ---------------------------------------------------

// Crea el pcb y lo encola en new
void iniciar_proceso(char *path){
    int pid = generar_pid();
    t_pcb *pcb = crear_pcb(pid, path);
    estado_encolar_pcb(estado_new, pcb);
    log_creacion_proceso(pcb);
}

void finalizar_proceso(int pid){ // TERMINAR
    detener_planificacion(); // si detengo la plani deberia esperar a que se haya frenado por completo, porq si no se paro por completo puede haber procesos q hayan sido desencolados de una cola para poder meterlas en otra y no lo encuentre
    // pthread_mutex_lock(&mutex_lista_procesos_pendientes_a_finalizar);
    // list_add(procesos_pendientes_a_finalizar, pid); // cada vez q modifique la lista de procesos de un estado deberia corroborar que no este queriendo acceder a un proceso q se requiera finalizar
    // pthread_mutex_unlock(&mutex_lista_procesos_pendientes_a_finalizar);
    // FORMA 1
    t_pcb *pcb = estado_rastrear_y_desencolar_pcb_por_pid(pid); // no esta mal porq lo encuentro y lo saco. Si llego no freno la plani, encontrar el estado y dsp aparte desencolarlo es al pedo porq pudo cambiar de estado
    // t_nombre_estado nombre_estado = estado_get_nombre_estado(pcb_get_estado(pcb));
    // FORMA 2
    // t_estado *estado = estado_rastrear_pcb_por_pid(pid);
    // if( estado == NULL ){
    //     log_error(kernel_logger, "El PCB solicitado no se encuentra en el sistema");
    // }// podria consultar si ya se encontro dsp de buscar en cada lista
    if( pcb != NULL ){
        switch( pcb_get_estado(pcb) ){
            case NEW:
                proceso_a_exit(pcb, FINALIZACION_INTERRUPTED_BY_USER);
                // sem_post(&sem_grado_multiprogramacion);
                break;
            case READY:
                proceso_a_exit(pcb, FINALIZACION_INTERRUPTED_BY_USER);
                // sem_post(&sem_grado_multiprogramacion);
                break;
            case READY_PLUS:
                proceso_a_exit(pcb, FINALIZACION_INTERRUPTED_BY_USER);
                // sem_post(&sem_grado_multiprogramacion);
                break;
            case BLOCKED:
                proceso_a_exit(pcb, FINALIZACION_INTERRUPTED_BY_USER);
                // sem_post(&sem_grado_multiprogramacion);
                break;
            case EXEC:
                // Mandar interrupcion a CPU y la respuesta es manejada en manejar motivo desalojo
                enviar_codigo_operacion(fd_cpu_interrupt, INTERRUPT_USER);
                break;
            case EXIT:
                log_error(kernel_logger, "El PCB solicitado ya esta siendo eliminado del sistema");
                break;
        }
    }
    else{
        log_error(kernel_logger, "El PCB solicitado no se encuentra en el sistema");
    }

    iniciar_planificacion();
}

#include <unistd.h>

void planificador_largo_plazo(){
    // Manejar EXIT
    pthread_t hilo_liberar_procesos_exit;
    pthread_create(&hilo_liberar_procesos_exit, NULL, (void *)liberar_procesos_exit, NULL);
    pthread_detach(hilo_liberar_procesos_exit);

    // Manejar NEW -> READY
    while(1){
        sem_wait(&sem_estado_planificacion_new_to_ready);
        sem_wait(&sem_control_cambio_grado_multiprogramacion); // Este semaforo es para garantizar que la modificacion (reduccion enrealidad es lo que genera complicaciones) se haga por completo y no se sigan mandando procesos a ready -> si reduzco el grado tengo que hacer waits, pero si ya estaba en 0 el hilo q reduce se va a bloquear al igual que este hilo al intentar mandar un proceso a ready y el signal tras finalizar un proceso podria activar cualquiera de los dos hilos y, en lugar de reducir el grado seguiria mandando procesos -> con este sem me aseguro q eso no pasa 
        sem_wait(&sem_grado_multiprogramacion);
        sem_post(&sem_control_cambio_grado_multiprogramacion);
        t_pcb *pcb = estado_desencolar_primer_pcb(estado_new);
        t_codigo_operacion respuesta_memoria = pedir_a_memoria_iniciar_proceso(pcb_get_pid(pcb), pcb_get_path(pcb));
        if( respuesta_memoria == CONFIRMACION_PROCESO_INICIADO ){
            proceso_a_ready(pcb);
        }
        else{
            proceso_a_exit(pcb, FINALIZACION_ERROR);
            // sem_post(&sem_grado_multiprogramacion);
        }
        sem_post(&sem_estado_planificacion_new_to_ready);
        sleep(0.001); // !!!!!!!!!!!!!!!!!!!!
    }
}

t_codigo_operacion pedir_a_memoria_iniciar_proceso(int pid, char *path){
    t_codigo_operacion respuesta_memoria;
    t_paquete *paquete_solicitud_iniciar_proceso = crear_paquete(SOLICITUD_INICIAR_PROCESO);
    agregar_pid_a_paquete(paquete_solicitud_iniciar_proceso, pid);
    agregar_string_a_paquete(paquete_solicitud_iniciar_proceso, path);
    
    pthread_mutex_lock(&mutex_socket_memoria);
    // Envio la solicitud
    enviar_paquete(fd_memoria, paquete_solicitud_iniciar_proceso);
    // Recibo la respuesta
    recibir_codigo_operacion(fd_memoria, &respuesta_memoria);
    pthread_mutex_unlock(&mutex_socket_memoria);

    eliminar_paquete(paquete_solicitud_iniciar_proceso);

    return respuesta_memoria;
}

void liberar_procesos_exit(){
    while(1){
        t_pcb *pcb = estado_desencolar_primer_pcb(estado_exit);
        t_codigo_operacion respuesta_memoria = pedir_a_memoria_finalizar_proceso(pcb_get_pid(pcb));
        if( respuesta_memoria == CONFIRMACION_PROCESO_FINALIZADO ){
            if( !dictionary_is_empty(pcb_get_diccionario_recursos_usados(pcb)) ){
                liberar_recursos_usados(pcb);
            }
            eliminar_pcb(pcb);
            sem_post(&sem_grado_multiprogramacion);
        }
        else{
            log_error(kernel_logger, "MEMORIA no finalizo correctamente el proceso!");
        }
    }
}

t_codigo_operacion pedir_a_memoria_finalizar_proceso(int pid){
    t_codigo_operacion respuesta_memoria;
    t_paquete *paquete_solicitud_finalizar_proceso = crear_paquete(SOLICITUD_FINALIZAR_PROCESO);
    agregar_pid_a_paquete(paquete_solicitud_finalizar_proceso, pid);
    
    pthread_mutex_lock(&mutex_socket_memoria);
    // Envio la solicitud
    enviar_paquete(fd_memoria, paquete_solicitud_finalizar_proceso);
    // Recibo la respuesta
    recibir_codigo_operacion(fd_memoria, &respuesta_memoria);
    pthread_mutex_unlock(&mutex_socket_memoria);

    eliminar_paquete(paquete_solicitud_finalizar_proceso);

    return respuesta_memoria;
}

void liberar_recursos_usados(t_pcb *pcb){
    t_dictionary *diccionario_recursos_usados = pcb_get_diccionario_recursos_usados(pcb);
    int size_dict = dictionary_size(diccionario_recursos_usados);
    t_list *recursos = dictionary_keys(diccionario_recursos_usados);
    for(int i = 0; i < size_dict; i++){
        char *nombre_recurso = list_get(recursos, i);
        liberar_recurso(pcb, nombre_recurso);
    }

    list_destroy_and_destroy_elements(recursos, free);
}

void liberar_recurso(t_pcb *pcb, char *nombre_recurso){
    t_dictionary *diccionario_recursos_usados = pcb_get_diccionario_recursos_usados(pcb);
    int instancias_usadas = *(int *) dictionary_get(diccionario_recursos_usados, nombre_recurso);
    for(int i = 0; i < instancias_usadas; i++){
        ejecutar_instruccion_signal(pcb, nombre_recurso);
    }
}

void ejecutar_instruccion_signal(t_pcb *pcb, char *nombre_recurso){
    pthread_mutex_lock(&mutex_diccionario_recursos);
    t_recurso *recurso = diccionario_recursos_get_recurso(diccionario_recursos, nombre_recurso);
    pthread_mutex_unlock(&mutex_diccionario_recursos);
    
    pthread_mutex_lock(recurso_get_mutex_recurso(recurso));

    // Impacto el signal en el recurso (aumento en 1 la cantidad de instancias)
    recurso_signal(recurso);

    // Impacto el signal en el pcb (disminuyo en 1 las instancias tomadas). Esto lo realizo solo si el proceso tiene tomada una instancia del recurso. Si el proceso no usa el recurso (tiene tomadas 0 instancias), entonces el mismo esta creando una nueva instancia del recurso con el signal -> no hay nada que impactar en el pcb (la instancia del recurso ya fue creada arriba con recurso_signal).
    if( pcb_usa_recurso(pcb, nombre_recurso) ){
        t_dictionary *diccionario_recursos_usados = pcb_get_diccionario_recursos_usados(pcb);
        int *instancias_usadas = (int *) dictionary_get(diccionario_recursos_usados, nombre_recurso);
        if( *instancias_usadas > 1 ){
            // Si el proceso tomo mas de una instancia => le resto una instancia (ya que realizo un signal y libero una) y actualizo su diccionario
            (*instancias_usadas)--;
            dictionary_put(diccionario_recursos_usados, nombre_recurso, instancias_usadas); 
        }
        else{
            // Si el proceso solo tiene tomada una instancia => directamente elimino al recurso de su diccionario, ya que al hacer el signal no tendra ninguna instancia
            dictionary_remove_and_destroy(diccionario_recursos_usados, nombre_recurso, free);
        }
    }
    
    
    // Compruebo si tras el signal se debe desbloquear algun proceso
    if( recurso_debe_desbloquear_proceso(recurso) ){
        sem_wait(&sem_estado_planificacion_blocked_to_ready);
        // Desbloqueo al primero proceso
        t_pcb *pcb = recurso_desencolar_primer_proceso(recurso);
        estado_desencolar_pcb_por_pid(estado_blocked, pcb_get_pid(pcb)); // REVISAR SI ESTO ESTA BIEN, LA IDEA ES DESBLOQUEARLO DEL RECURSO Y TMB DE LA LISTA DE BLOQUEADOS
        // ACA PROBABLEMENTE TENGA Q HACER UN IF( LE_QUEDA_QUANTUM(PCB) ){ PROCESO_A_READY_PLUS } ELSE{ PROCESO_A_READY }
        proceso_a_ready(pcb);
        sem_post(&sem_estado_planificacion_blocked_to_ready);
    }

    pthread_mutex_unlock(recurso_get_mutex_recurso(recurso));
}

void ejecutar_instruccion_wait(t_pcb *pcb, char *nombre_recurso){
    pthread_mutex_lock(&mutex_diccionario_recursos);
    t_recurso *recurso = diccionario_recursos_get_recurso(diccionario_recursos, nombre_recurso);
    pthread_mutex_unlock(&mutex_diccionario_recursos);
    
    pthread_mutex_lock(recurso_get_mutex_recurso(recurso));
    
    // Impacto el wait en el recurso
    recurso_wait(recurso);

    // Impacto el wait en el pcb
    // Si el proceso ya tenia tomada una instancia le agrego una mas
    // Si el proceso no tenia tomada ninguna instancia le agrego el recurso a su diccionario con 1 instancia
    t_dictionary *diccionario_recursos_usados = pcb_get_diccionario_recursos_usados(pcb);
    if( pcb_usa_recurso(pcb, nombre_recurso) ){
        int *instancias_usadas = (int *) dictionary_get(diccionario_recursos_usados, nombre_recurso);
        (*instancias_usadas)++;
        dictionary_put(diccionario_recursos_usados, nombre_recurso, instancias_usadas);
    }
    else{
        int *instancias_usadas = malloc(sizeof(int));
        *instancias_usadas = 1; 
        dictionary_put(diccionario_recursos_usados, nombre_recurso, instancias_usadas);
    }
    
    // Compruebo si tras el wait se debe bloquear al proceso
    if( recurso_debe_bloquear_proceso(recurso) ){
        // Bloqueo al proceso (lo encolo en la cola del recurso)
        recurso_encolar_proceso(recurso, pcb);
        proceso_a_blocked(pcb, nombre_recurso);
    }

    pthread_mutex_unlock(recurso_get_mutex_recurso(recurso));
}

// PROCESO A ... ---------------------------------------------------

void proceso_a_ready(t_pcb *pcb){
	pcb_cambiar_estado_a(pcb, READY);
	estado_encolar_pcb(estado_ready, pcb);
    log_ingreso_ready(estado_ready);
}

void proceso_a_exec(t_pcb *pcb){
    pcb_cambiar_estado_a(pcb, EXEC);
    estado_encolar_pcb(estado_exec, pcb);
}

void proceso_a_blocked(t_pcb *pcb, char *motivo_bloqueo){
    pcb_cambiar_estado_a(pcb, BLOCKED);
    estado_encolar_pcb(estado_blocked, pcb);
    log_motivo_bloqueo(pcb, motivo_bloqueo);
}

void proceso_a_exit(t_pcb *pcb, char *motivo_finalizacion){
    pcb_cambiar_estado_a(pcb, EXIT);
    estado_encolar_pcb(estado_exit, pcb);
    log_fin_proceso(pcb, motivo_finalizacion);
}