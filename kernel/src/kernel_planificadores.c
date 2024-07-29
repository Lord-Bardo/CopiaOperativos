#include "../include/kernel_planificadores.h"

// VARIABLES GLOBALES
// Planificacion
t_estado_planificacion estado_planificacion;
sem_t sem_estado_planificacion_new_to_ready;
sem_t sem_estado_planificacion_ready_to_exec;
sem_t sem_estado_planificacion_blocked_to_ready_recurso;
sem_t sem_estado_planificacion_blocked_to_ready_interfaz;
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

// Procesos a finalizar
t_dictionary *diccionario_procesos_a_finalizar;
pthread_mutex_t mutex_diccionario_procesos_a_finalizar;

// Semaforos sockets
pthread_mutex_t mutex_socket_memoria;
pthread_mutex_t mutex_socket_cpu_interrupt;

// Semaforo quantum
pthread_mutex_t mutex_interrupcion_quantum;

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
    sem_init(&sem_estado_planificacion_blocked_to_ready_recurso, 0, 0);
    sem_init(&sem_estado_planificacion_blocked_to_ready_interfaz, 0, 0);
    sem_init(&sem_estado_planificacion_exec_to_exec_or_ready_or_blocked, 0, 0);
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
    // Diccionario procesos a finalizar
    diccionario_procesos_a_finalizar = dictionary_create();
    pthread_mutex_init(&mutex_diccionario_procesos_a_finalizar, NULL);
}

void inicializar_estructuras_sockets(){
    // Semaforos sockets
    pthread_mutex_init(&mutex_socket_memoria, NULL);
    pthread_mutex_init(&mutex_socket_cpu_interrupt, NULL);
    // Semaforo quantum 
    pthread_mutex_init(&mutex_interrupcion_quantum, NULL);
}

void iniciar_planificacion(){
    if( estado_planificacion == PAUSADA ){
        estado_planificacion = ACTIVA;
        sem_post(&sem_estado_planificacion_new_to_ready);
        sem_post(&sem_estado_planificacion_ready_to_exec);
        sem_post(&sem_estado_planificacion_blocked_to_ready_recurso);
        sem_post(&sem_estado_planificacion_blocked_to_ready_interfaz);
        sem_post(&sem_estado_planificacion_exec_to_exec_or_ready_or_blocked);
    }
}

void detener_planificacion(){
    if( estado_planificacion == ACTIVA ){
        estado_planificacion = PAUSADA;
        pthread_t hilo_detener_planificacion_new_to_ready, hilo_detener_planificacion_ready_to_exec, hilo_detener_planificacion_blocked_to_ready_recurso, hilo_detener_planificacion_blocked_to_ready_interfaz, hilo_detener_planificacion_exec_to_exec_or_ready_or_blocked;
        pthread_create(&hilo_detener_planificacion_new_to_ready, NULL, (void *)detener_planificacion_new_to_ready, NULL);
        pthread_create(&hilo_detener_planificacion_ready_to_exec, NULL, (void *)detener_planificacion_ready_to_exec, NULL);
        pthread_create(&hilo_detener_planificacion_blocked_to_ready_recurso, NULL, (void *)detener_planificacion_blocked_to_ready_recurso, NULL);
        pthread_create(&hilo_detener_planificacion_blocked_to_ready_interfaz, NULL, (void *)detener_planificacion_blocked_to_ready_interfaz, NULL);
        pthread_create(&hilo_detener_planificacion_exec_to_exec_or_ready_or_blocked, NULL, (void *)detener_planificacion_exec_to_exec_or_ready_or_blocked, NULL);
        pthread_join(hilo_detener_planificacion_new_to_ready, NULL);
        pthread_join(hilo_detener_planificacion_ready_to_exec, NULL);
        pthread_join(hilo_detener_planificacion_blocked_to_ready_recurso, NULL);
        pthread_join(hilo_detener_planificacion_blocked_to_ready_interfaz, NULL);
        pthread_join(hilo_detener_planificacion_exec_to_exec_or_ready_or_blocked, NULL);
    }
}

void detener_planificacion_new_to_ready(){
    sem_wait(&sem_estado_planificacion_new_to_ready);
}

void detener_planificacion_ready_to_exec(){
    sem_wait(&sem_estado_planificacion_ready_to_exec);
}

void detener_planificacion_blocked_to_ready_recurso(){
    sem_wait(&sem_estado_planificacion_blocked_to_ready_recurso);
}

void detener_planificacion_blocked_to_ready_interfaz(){
    sem_wait(&sem_estado_planificacion_blocked_to_ready_interfaz);
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
        sem_wait(estado_get_sem(estado_ready)); // debe haber elementos en la lista para poder desencolar - Pongo el wait del estado antes que el wait de la plani xq si lo hago al reves y no hay elementos en la cola, el hilo se va a bloquear con el wait de la plani tomado y nunca se va a detener la planificacion
        sem_wait(&sem_estado_planificacion_ready_to_exec);
        t_pcb *pcb = estado_desencolar_primer_pcb(estado_ready);
        proceso_a_exec(pcb);
        sem_post(&sem_estado_planificacion_ready_to_exec);

        enviar_contexto_de_ejecucion(pcb);
        
        t_codigo_operacion motivo_desalojo;
        t_buffer *buffer = crear_buffer();
        recibir_paquete(fd_cpu_dispatch, &motivo_desalojo, buffer); // Espera por el Dispatch la llegada del contexto actualizado tras la ejecucion del proceso (pid y registros). Junto con el contexto debe llegar el motivo por el cual finalizo la ejecucion (motivo de desalojo)
        buffer_desempaquetar_contexto_ejecucion(buffer, pcb); // Modifica al pcb con lo que recibe
        
        manejar_motivo_desalojo(pcb, motivo_desalojo, buffer, NULL, NULL);
        
        eliminar_buffer(buffer);        
    }
}

// t_pcb *elegir_proceso_segun_fifo(){
//     return estado_desencolar_primer_pcb(estado_ready);
// }

// RR
void planificador_corto_plazo_rr(){
    while(1){
        sem_wait(estado_get_sem(estado_ready)); // debe haber elementos en la lista para poder desencolar
        sem_wait(&sem_estado_planificacion_ready_to_exec);
        t_pcb *pcb = estado_desencolar_primer_pcb(estado_ready);
        proceso_a_exec(pcb);
        sem_post(&sem_estado_planificacion_ready_to_exec);

        t_contar_quantum_parametros *contar_quantum_parametros = crear_contar_quantum_parametros(QUANTUM, pcb_get_pid(pcb));
        enviar_contexto_de_ejecucion(pcb);
        t_temporal *clock_exec = temporal_create();
        
        pthread_t hilo_quantum;
	    pthread_create(&hilo_quantum, NULL, (void*)contar_quantum, (void *)contar_quantum_parametros);
        pthread_detach(hilo_quantum);

        t_codigo_operacion motivo_desalojo;
        t_buffer *buffer = crear_buffer();
        recibir_paquete(fd_cpu_dispatch, &motivo_desalojo, buffer);
        temporal_stop(clock_exec);
        pthread_mutex_lock(&mutex_interrupcion_quantum); // uso este mutex porq si cancelo el hilo con el mutex del socket cpu_interrupt tomado pero no libero, hago cagada y lo mas probable es q haya deadlock
        pthread_cancel(hilo_quantum);
        pthread_mutex_unlock(&mutex_interrupcion_quantum);
        buffer_desempaquetar_contexto_ejecucion(buffer, pcb); // Modifica al pcb con lo que recibe
        
        manejar_motivo_desalojo(pcb, motivo_desalojo, buffer, clock_exec, contar_quantum_parametros);
        
        eliminar_contar_quantum_parametros(contar_quantum_parametros);
        temporal_destroy(clock_exec);
        eliminar_buffer(buffer);
    }
}

// t_pcb *elegir_proceso_segun_rr(){
//     return estado_desencolar_primer_pcb(estado_ready); 
// }

void contar_quantum(void *contar_quantum_parametros_void){
    t_contar_quantum_parametros *contar_quantum_parametros = (t_contar_quantum_parametros *)contar_quantum_parametros_void;
    usleep( contar_quantum_parametros_get_quantum(contar_quantum_parametros) * 1000 ); // usleep para poder trabajar con microsegundos
    if( pthread_mutex_trylock(&mutex_interrupcion_quantum) == 0 ){
        t_paquete *paquete_interrupt_user = crear_paquete(INTERRUPT_QUANTUM);
        agregar_pid_a_paquete(paquete_interrupt_user, contar_quantum_parametros_get_pid(contar_quantum_parametros));
        pthread_mutex_lock(&mutex_socket_cpu_interrupt);
        enviar_paquete(fd_cpu_interrupt, paquete_interrupt_user);
        pthread_mutex_unlock(&mutex_socket_cpu_interrupt);
        log_info(kernel_logger, "Interrupcion por quantum enviada!");
        eliminar_paquete(paquete_interrupt_user);
        pthread_mutex_unlock(&mutex_interrupcion_quantum);
    }
}

// VRR
void planificador_corto_plazo_vrr(){
    while(1){
        t_estado *estado_proceso_a_desencolar;
        if( estado_contiene_pcbs(estado_ready_plus) ){
            estado_proceso_a_desencolar = estado_ready_plus;
            sem_wait(estado_get_sem(estado_ready_plus)); // debe haber elementos en la lista para poder desencolar
        }
        else{
            estado_proceso_a_desencolar = estado_ready;
            sem_wait(estado_get_sem(estado_ready)); // debe haber elementos en la lista para poder desencolar
        }
        sem_wait(&sem_estado_planificacion_ready_to_exec);
        t_pcb *pcb = estado_desencolar_primer_pcb(estado_proceso_a_desencolar);
        proceso_a_exec(pcb);
        sem_post(&sem_estado_planificacion_ready_to_exec);

        int quantum_inicial = pcb_get_quantum_inicial(pcb); // en este momento quantum_inicial = quantum_restante
        t_contar_quantum_parametros *contar_quantum_parametros = crear_contar_quantum_parametros(quantum_inicial, pcb_get_pid(pcb));
        enviar_contexto_de_ejecucion(pcb);
        t_temporal *clock_exec = temporal_create();
        
        pthread_t hilo_quantum;
	    pthread_create(&hilo_quantum, NULL, (void*)contar_quantum, (void *)contar_quantum_parametros);
        pthread_detach(hilo_quantum);

        t_codigo_operacion motivo_desalojo;
        t_buffer *buffer = crear_buffer();
        recibir_paquete(fd_cpu_dispatch, &motivo_desalojo, buffer);
        temporal_stop(clock_exec);
        pthread_mutex_lock(&mutex_interrupcion_quantum); // uso este mutex porq si cancelo el hilo con el mutex del socket cpu_interrupt tomado pero no libero, hago cagada y lo mas probable es q haya deadlock
        pthread_cancel(hilo_quantum);
        pthread_mutex_unlock(&mutex_interrupcion_quantum);
        int quantum_restante = quantum_inicial - (int)temporal_gettime(clock_exec);
        pcb_set_quantum_restante(pcb, quantum_restante);
        buffer_desempaquetar_contexto_ejecucion(buffer, pcb); // Modifica al pcb con lo que recibe
        
        manejar_motivo_desalojo(pcb, motivo_desalojo, buffer, clock_exec, contar_quantum_parametros);
        
        eliminar_contar_quantum_parametros(contar_quantum_parametros);
        temporal_destroy(clock_exec);
        eliminar_buffer(buffer);
    }
}

// t_pcb *elegir_proceso_segun_vrr(){
//     if( estado_contiene_pcbs(estado_ready_plus) ){
//         sem_wait(estado_get_sem(estado_ready_plus)); // debe haber elementos en la lista para poder desencolar
//         return estado_desencolar_primer_pcb(estado_ready_plus);
//     }
//     else{
//         sem_wait(estado_get_sem(estado_ready)); // debe haber elementos en la lista para poder desencolar
//         return estado_desencolar_primer_pcb(estado_ready);
//     }
// }

// Manda a CPU el contexto de la ejecucion (pid y registros) por el Dispatch
void enviar_contexto_de_ejecucion(t_pcb *pcb){
    t_paquete *paquete_contexto_de_ejecucion = crear_paquete(CONTEXTO_DE_EJECUCION);
    agregar_contexto_ejecucion_a_paquete(paquete_contexto_de_ejecucion, pcb);
    enviar_paquete(fd_cpu_dispatch, paquete_contexto_de_ejecucion);
    eliminar_paquete(paquete_contexto_de_ejecucion);
}

void manejar_motivo_desalojo(t_pcb *pcb, t_codigo_operacion motivo_desalojo, t_buffer *buffer, t_temporal *clock_exec, t_contar_quantum_parametros *contar_quantum_parametros){
    sem_wait(&sem_estado_planificacion_exec_to_exec_or_ready_or_blocked);

    pthread_mutex_lock(&mutex_diccionario_procesos_a_finalizar); 
    if( diccionario_procesos_a_finalizar_proceso_esta_pendiente_de_finalizar(diccionario_procesos_a_finalizar, pcb) ){ // esto es para contemplar los casos donde se mando la interrupcion, pero como el proceso se desalojo por otro motivo y no por la interrupcion, no se devolvio al pcb con motivo_desalojo = INTERRUP_USER. Es decir, esta ejecutando el proceso, se desalojo por un wait/signal/io/etc., pero justo durante la ejecucion se finalizo al proceso, por lo q se manda la interrupcion y se lo pone como pendiente de finalizar. Como se desaloja por otro motivo, no llega como motivo la finalizacion, por eso esta la lista y esta comprobacion
        motivo_desalojo = INTERRUPT_USER; // si esta pendiente de finalizar no necesito desencolarlo porq la funcion finalizar_proceso ya lo desencolo, solo me restaria mandarlo a exit, por ello hago que el motivo de desalojo sea INTERRUPT_USER
    }
    else{
        sem_wait(estado_get_sem(estado_exec)); // debe haber elementos en la lista para poder desencolar (aunque deberia haber siempre uno solo en este caso)
        pcb = estado_desencolar_primer_pcb(estado_exec); // desencolo aca y no en el planificador porq sino hay problemas para finalizar un proceso. Si el proceso es finalizado justo dsp de ser desalojado por cpu, pero antes de comenzar a manejar el motivo de desalojo => si desencolo el pcb, no va a quedar en ninguna cola y no lo voy a poder encontrar
        // puedo desencolar xq la plani se detiene una vez q termina el motivo de desalojo (si es q se detuvo mientras ya se estaba manejando). Por lo q se termina de manejar el motivo y el cambio de estado, y recien ahi se detiene la plani
    }
    pthread_mutex_unlock(&mutex_diccionario_procesos_a_finalizar); 

    switch(motivo_desalojo){
        case SUCCESS:
            proceso_a_exit(pcb, FINALIZACION_SUCCESS);
            // sem_post(&sem_grado_multiprogramacion);
            break;
        case INTERRUPT_QUANTUM:
            log_fin_quantum(pcb);
            pcb_set_quantum_inicial(pcb, QUANTUM);
            pcb_set_quantum_restante(pcb, QUANTUM);
            proceso_a_ready(pcb);
            break;
        case INTERRUPT_USER: // se mato al proceso por consola y justo estaba ejecutando
            proceso_a_exit(pcb, FINALIZACION_INTERRUPTED_BY_USER);
            // sem_post(&sem_grado_multiprogramacion);
            break;
        case OUT_OF_MEMORY: // Fallo el Resize
            proceso_a_exit(pcb, FINALIZACION_OUT_OF_MEMORY);
            // sem_post(&sem_grado_multiprogramacion);
            break;
        case COP_WAIT:
        { // uso "bloques compuestos" para evitar los errores de redefinicion de variables -> cada bloque de estos es como q esta aparte, si no los pongo el compilador me toma al switch como una sola cosa y me tira error de redefinicion con nombre_recurso y demas
            char *nombre_recurso = buffer_desempaquetar_string(buffer);
            // Verifico que el recurso exista
            pthread_mutex_lock(&mutex_diccionario_recursos); // los recursos no cambian, nose si es necesario el mutex para consultar si existe o no el recurso
            if( !diccionario_recursos_existe_recurso(diccionario_recursos, nombre_recurso) ){
                pthread_mutex_unlock(&mutex_diccionario_recursos);
                proceso_a_exit(pcb, FINALIZACION_INVALID_RESOURCE);
                // sem_post(&sem_grado_multiprogramacion); -> Tengo q definir si lo hago en proceso_a_exit, cuando libero los recursos o lo hago siempre seguido al proceso_a_exit
            }
            else{
                pthread_mutex_unlock(&mutex_diccionario_recursos);
                ejecutar_instruccion_wait(pcb, nombre_recurso);
                if( pcb_get_estado(pcb) == EXEC ){
                    if( strcmp(ALGORITMO_PLANIFICACION, "FIFO") == 0 ){
                        proceso_a_exec(pcb); // lo vuelvo a encolar en exec
                        sem_post(&sem_estado_planificacion_exec_to_exec_or_ready_or_blocked);

                        enviar_contexto_de_ejecucion(pcb);
                        
                        recibir_paquete(fd_cpu_dispatch, &motivo_desalojo, buffer);
                        buffer_desempaquetar_contexto_ejecucion(buffer, pcb); // Modifica al pcb con lo que recibe
                        
                        manejar_motivo_desalojo(pcb, motivo_desalojo, buffer, NULL, NULL);
                    }
                    else if( strcmp(ALGORITMO_PLANIFICACION, "RR") == 0 ){
                        int quantum_restante = QUANTUM - (int)temporal_gettime(clock_exec);
                        if( quantum_restante > 0 ){
                            proceso_a_exec(pcb); // lo vuelvo a encolar en exec
                            sem_post(&sem_estado_planificacion_exec_to_exec_or_ready_or_blocked);

                            contar_quantum_parametros_set_quantum(contar_quantum_parametros, quantum_restante);
                            enviar_contexto_de_ejecucion(pcb);
                            temporal_resume(clock_exec);

                            pthread_t hilo_quantum;
                            pthread_create(&hilo_quantum, NULL, (void*)contar_quantum, (void *)contar_quantum_parametros);
                            pthread_detach(hilo_quantum);

                            recibir_paquete(fd_cpu_dispatch, &motivo_desalojo, buffer);
                            temporal_stop(clock_exec);
                            pthread_mutex_lock(&mutex_interrupcion_quantum); // uso este mutex porq si cancelo el hilo con el mutex del socket cpu_interrupt tomado pero no libero, hago cagada y lo mas probable es q haya deadlock
                            pthread_cancel(hilo_quantum);
                            pthread_mutex_unlock(&mutex_interrupcion_quantum);
                            buffer_desempaquetar_contexto_ejecucion(buffer, pcb); // Modifica al pcb con lo que recibe

                            manejar_motivo_desalojo(pcb, motivo_desalojo, buffer, clock_exec, contar_quantum_parametros); // -> muy probablemente tenga q mandarlo a esta funcion y mandarle tambien el quantum restante y el clock, por si otra vez desalojo por un wait o signal
                        }
                        else{
                            log_fin_quantum(pcb);
                            // pcb_set_quantum_restante(pcb, QUANTUM); -> no es necesario xq en RR no modifico el quantum restante ni el inicial
                            proceso_a_ready(pcb);
                        }
                    }
                    else{ // VRR
                        int quantum_restante = pcb_get_quantum_restante(pcb);
                        if( quantum_restante > 0 ){
                            proceso_a_exec(pcb); // lo vuelvo a encolar en exec
                            sem_post(&sem_estado_planificacion_exec_to_exec_or_ready_or_blocked);
                            
                            contar_quantum_parametros_set_quantum(contar_quantum_parametros, quantum_restante);
                            enviar_contexto_de_ejecucion(pcb);
                            temporal_resume(clock_exec);

                            pthread_t hilo_quantum;
                            pthread_create(&hilo_quantum, NULL, (void*)contar_quantum, (void *)contar_quantum_parametros);
                            pthread_detach(hilo_quantum);

                            recibir_paquete(fd_cpu_dispatch, &motivo_desalojo, buffer);
                            temporal_stop(clock_exec);
                            pthread_mutex_lock(&mutex_interrupcion_quantum); // uso este mutex porq si cancelo el hilo con el mutex del socket cpu_interrupt tomado pero no libero, hago cagada y lo mas probable es q haya deadlock
                            pthread_cancel(hilo_quantum);
                            pthread_mutex_unlock(&mutex_interrupcion_quantum);
                            quantum_restante = pcb_get_quantum_inicial(pcb) - (int)temporal_gettime(clock_exec);
                            pcb_set_quantum_restante(pcb, quantum_restante);
                            buffer_desempaquetar_contexto_ejecucion(buffer, pcb); // Modifica al pcb con lo que recibe
                            
                            manejar_motivo_desalojo(pcb, motivo_desalojo, buffer, clock_exec, contar_quantum_parametros); // -> muy probablemente tenga q mandarlo a esta funcion y mandarle tambien el quantum restante y el clock, por si otra vez desalojo por un wait o signal
                        }
                        else{
                            log_fin_quantum(pcb);
                            pcb_set_quantum_inicial(pcb, QUANTUM);
                            pcb_set_quantum_restante(pcb, QUANTUM);
                            proceso_a_ready(pcb);
                        }
                    }
                }
            }
            break;
        }
        case COP_SIGNAL:
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
                // Forma 1: con el sanguchito wait-post aca no me convence porq puede q hagas un signal y no haya transicion alguna, pero si lo hago dentro del signal voy a dejar q se hagan cosas con un pcb q capaz es finalizado -> xq una vez haga el POST-EXEC-TO la plani ya se habria detenido por complpeto y se podria mandar a exit al proceso y liberar sus recursos, pero mientras tanto el signal se sigue ejecutando hasta que llegue a la zona critica de verdad. Ademas con el wait no puedo replicar esta metodologia de meter el sanguchito wait-signal dentro, porq hasta q no se completa nose si sigue ejecutando o no y si pongo el sanguchito adentro se nunca saldria y haria el POST-EXEC-TO. Ademas no se haria el unlock del recurso y si el proceso q quiero finalizar fue bloqueado por ese recurso va a darse un deadlock
                // PROCESO_A_EXEC
                // POST-EXEC-TO
                // WAIT-BLOCKED-READY // si hago el POST-EXEC-TO antes de hacer el signal puedo hacer el WAIT-BLOCKED-READY y el POST-BLOCKED-READY dentro del signal
                // ejecutar_instruccion_signal(pcb, nombre_recurso);
                // POST-BLOCKED-READY
                // Forma 2:
                ejecutar_instruccion_signal(pcb, nombre_recurso);
                // Despues del signal siempre sigue ejecutando el mismo proceso (salvo q se haya quedado sin quantum justo dsp de desalojarse)
                if( strcmp(ALGORITMO_PLANIFICACION, "FIFO") == 0 ){
                    proceso_a_exec(pcb);
                    sem_post(&sem_estado_planificacion_exec_to_exec_or_ready_or_blocked);
                    
                    enviar_contexto_de_ejecucion(pcb);
                    
                    recibir_paquete(fd_cpu_dispatch, &motivo_desalojo, buffer);
                    buffer_desempaquetar_contexto_ejecucion(buffer, pcb); // Modifica al pcb con lo que recibe
                    
                    manejar_motivo_desalojo(pcb, motivo_desalojo, buffer, NULL, NULL);
                }
                else if( strcmp(ALGORITMO_PLANIFICACION, "RR") == 0 ){
                    int quantum_restante = QUANTUM - (int)temporal_gettime(clock_exec);
                    if( quantum_restante > 0 ){
                        proceso_a_exec(pcb);
                        sem_post(&sem_estado_planificacion_exec_to_exec_or_ready_or_blocked);
                        
                        contar_quantum_parametros_set_quantum(contar_quantum_parametros, quantum_restante);
                        enviar_contexto_de_ejecucion(pcb);
                        temporal_resume(clock_exec);

                        pthread_t hilo_quantum;
                        pthread_create(&hilo_quantum, NULL, (void*)contar_quantum, (void *)contar_quantum_parametros);
                        pthread_detach(hilo_quantum);

                        recibir_paquete(fd_cpu_dispatch, &motivo_desalojo, buffer);
                        temporal_stop(clock_exec);
                        pthread_mutex_lock(&mutex_interrupcion_quantum); // uso este mutex porq si cancelo el hilo con el mutex del socket cpu_interrupt tomado pero no libero, hago cagada y lo mas probable es q haya deadlock
                        pthread_cancel(hilo_quantum);
                        pthread_mutex_unlock(&mutex_interrupcion_quantum);
                        buffer_desempaquetar_contexto_ejecucion(buffer, pcb); // Modifica al pcb con lo que recibe

                        manejar_motivo_desalojo(pcb, motivo_desalojo, buffer, clock_exec, contar_quantum_parametros); // -> muy probablemente tenga q mandarlo a esta funcion y mandarle tambien el quantum restante y el clock, por si otra vez desalojo por un wait o signal
                    }
                    else{
                        log_fin_quantum(pcb);
                        // pcb_set_quantum_restante(pcb, QUANTUM); -> no es necesario xq en RR no modifico el quantum restante
                        proceso_a_ready(pcb);
                    }
                }
                else{ // VRR
                    int quantum_restante = pcb_get_quantum_restante(pcb);
                    if( quantum_restante > 0 ){
                        proceso_a_exec(pcb);
                        sem_post(&sem_estado_planificacion_exec_to_exec_or_ready_or_blocked);

                        contar_quantum_parametros_set_quantum(contar_quantum_parametros, quantum_restante);
                        enviar_contexto_de_ejecucion(pcb);
                        temporal_resume(clock_exec);

                        pthread_t hilo_quantum;
                        pthread_create(&hilo_quantum, NULL, (void*)contar_quantum, (void *)&quantum_restante);
                        pthread_detach(hilo_quantum);

                        recibir_paquete(fd_cpu_dispatch, &motivo_desalojo, buffer);
                        temporal_stop(clock_exec);
                        pthread_mutex_lock(&mutex_interrupcion_quantum); // uso este mutex porq si cancelo el hilo con el mutex del socket cpu_interrupt tomado pero no libero, hago cagada y lo mas probable es q haya deadlock
                        pthread_cancel(hilo_quantum);
                        pthread_mutex_unlock(&mutex_interrupcion_quantum);
                        quantum_restante = pcb_get_quantum_inicial(pcb) - (int)temporal_gettime(clock_exec);
                        pcb_set_quantum_restante(pcb, quantum_restante);
                        buffer_desempaquetar_contexto_ejecucion(buffer, pcb); // Modifica al pcb con lo que recibe

                        manejar_motivo_desalojo(pcb, motivo_desalojo, buffer, clock_exec, contar_quantum_parametros); // -> muy probablemente tenga q mandarlo a esta funcion y mandarle tambien el quantum restante y el clock, por si otra vez desalojo por un wait o signal
                    }
                    else{
                        log_fin_quantum(pcb);
                        pcb_set_quantum_inicial(pcb, QUANTUM);
                        pcb_set_quantum_restante(pcb, QUANTUM);
                        proceso_a_ready(pcb);
                    }
                }
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
                            case COP_IO_GEN_SLEEP:
                                // Creo el paquete con la operacion a realizar y agrego el pid del proceso
                                t_paquete *paquete_solicitud_io = crear_paquete(operacion);
                                agregar_pid_a_paquete(paquete_solicitud_io, pcb_get_pid(pcb));
                                
                                // Desempaqueto los parametros de la operacion y los agrego al paquete
                                int unidades_de_trabajo;
                                buffer_desempaquetar(buffer, &unidades_de_trabajo);
                                agregar_int_a_paquete(paquete_solicitud_io, unidades_de_trabajo);
                                
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
                        switch( operacion ){
                            case COP_IO_STDIN_READ:
                                // Creo el paquete con la operacion a realizar y agrego el pid del proceso
                                t_paquete *paquete_solicitud_io = crear_paquete(operacion);
                                agregar_pid_a_paquete(paquete_solicitud_io, pcb_get_pid(pcb));
                                
                                // Desempaqueto los parametros de la operacion y los agrego al paquete
                                int cantidad_direcciones;
                                buffer_desempaquetar(buffer, &cantidad_direcciones);
                                agregar_int_a_paquete(paquete_solicitud_io, cantidad_direcciones);
                                
                                int direccion_fisica, bytes;
                                for(int i = 0; i < cantidad_direcciones; i++){
                                    buffer_desempaquetar(buffer, &direccion_fisica);
                                    agregar_int_a_paquete(paquete_solicitud_io, direccion_fisica);
                                    buffer_desempaquetar(buffer, &bytes);
                                    agregar_int_a_paquete(paquete_solicitud_io, bytes);
                                }
                                
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
                    case STDOUT:
                        switch( operacion ){
                            case COP_IO_STDOUT_WRITE:
                                // Creo el paquete con la operacion a realizar y agrego el pid del proceso
                                t_paquete *paquete_solicitud_io = crear_paquete(operacion);
                                agregar_pid_a_paquete(paquete_solicitud_io, pcb_get_pid(pcb));
                                
                                // Desempaqueto los parametros de la operacion y los agrego al paquete
                                int cantidad_direcciones;
                                buffer_desempaquetar(buffer, &cantidad_direcciones);
                                agregar_int_a_paquete(paquete_solicitud_io, cantidad_direcciones);
                                
                                int direccion_fisica, bytes;
                                for(int i = 0; i < cantidad_direcciones; i++){
                                    buffer_desempaquetar(buffer, &direccion_fisica);
                                    agregar_int_a_paquete(paquete_solicitud_io, direccion_fisica);
                                    buffer_desempaquetar(buffer, &bytes);
                                    agregar_int_a_paquete(paquete_solicitud_io, bytes);
                                }
                                
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
                    case DIALFS:
                        switch( operacion ){
                            case COP_IO_FS_CREATE:
                            {
                                // Creo el paquete con la operacion a realizar y agrego el pid del proceso
                                t_paquete *paquete_solicitud_io = crear_paquete(operacion);
                                agregar_pid_a_paquete(paquete_solicitud_io, pcb_get_pid(pcb));
                                
                                // Desempaqueto los parametros de la operacion y los agrego al paquete
                                char *nombre_archivo = buffer_desempaquetar_string(buffer);
                                agregar_string_a_paquete(paquete_solicitud_io, nombre_archivo);
                                
                                // Creo la solicitud de entrada salida
                                t_solicitud_io *solicitud_io = crear_solicitud_io(pcb, paquete_solicitud_io);
                                
                                // Bloqueo al proceso
                                proceso_a_blocked(pcb, nombre_interfaz);
                                
                                // Encolo la solicitud
                                interfaz_encolar_solicitud_io(interfaz, solicitud_io);
                                break;
                            }
                            case COP_IO_FS_DELETE:
                            {
                                // Creo el paquete con la operacion a realizar y agrego el pid del proceso
                                t_paquete *paquete_solicitud_io = crear_paquete(operacion);
                                agregar_pid_a_paquete(paquete_solicitud_io, pcb_get_pid(pcb));
                                
                                // Desempaqueto los parametros de la operacion y los agrego al paquete
                                char *nombre_archivo = buffer_desempaquetar_string(buffer);
                                agregar_string_a_paquete(paquete_solicitud_io, nombre_archivo);
                                
                                // Creo la solicitud de entrada salida
                                t_solicitud_io *solicitud_io = crear_solicitud_io(pcb, paquete_solicitud_io);
                                
                                // Bloqueo al proceso
                                proceso_a_blocked(pcb, nombre_interfaz);
                                
                                // Encolo la solicitud
                                interfaz_encolar_solicitud_io(interfaz, solicitud_io);
                                break;
                            }
                            case COP_IO_FS_TRUNCATE:
                            {
                                // Creo el paquete con la operacion a realizar y agrego el pid del proceso
                                t_paquete *paquete_solicitud_io = crear_paquete(operacion);
                                agregar_pid_a_paquete(paquete_solicitud_io, pcb_get_pid(pcb));
                                
                                // Desempaqueto los parametros de la operacion y los agrego al paquete
                                char *nombre_archivo = buffer_desempaquetar_string(buffer);
                                agregar_string_a_paquete(paquete_solicitud_io, nombre_archivo);

                                int tamanio;
                                buffer_desempaquetar(buffer, &tamanio);
                                agregar_int_a_paquete(paquete_solicitud_io, tamanio);
                                
                                // Creo la solicitud de entrada salida
                                t_solicitud_io *solicitud_io = crear_solicitud_io(pcb, paquete_solicitud_io);
                                
                                // Bloqueo al proceso
                                proceso_a_blocked(pcb, nombre_interfaz);
                                
                                // Encolo la solicitud
                                interfaz_encolar_solicitud_io(interfaz, solicitud_io);
                                break;
                            }
                            case COP_IO_FS_WRITE:
                            {
                                // Creo el paquete con la operacion a realizar y agrego el pid del proceso
                                t_paquete *paquete_solicitud_io = crear_paquete(operacion);
                                agregar_pid_a_paquete(paquete_solicitud_io, pcb_get_pid(pcb));
                                
                                // Desempaqueto los parametros de la operacion y los agrego al paquete
                                char *nombre_archivo = buffer_desempaquetar_string(buffer);
                                agregar_string_a_paquete(paquete_solicitud_io, nombre_archivo);

                                int puntero_archivo;
                                buffer_desempaquetar(buffer, &puntero_archivo);
                                agregar_int_a_paquete(paquete_solicitud_io, puntero_archivo);

                                int tamanio_a_escribir;
                                buffer_desempaquetar(buffer, &tamanio_a_escribir);
                                agregar_int_a_paquete(paquete_solicitud_io, tamanio_a_escribir);

                                int cantidad_direcciones;
                                buffer_desempaquetar(buffer, &cantidad_direcciones);
                                agregar_int_a_paquete(paquete_solicitud_io, cantidad_direcciones);
                                
                                int direccion_fisica, bytes;
                                for(int i = 0; i < cantidad_direcciones; i++){
                                    buffer_desempaquetar(buffer, &direccion_fisica);
                                    agregar_int_a_paquete(paquete_solicitud_io, direccion_fisica);
                                    buffer_desempaquetar(buffer, &bytes);
                                    agregar_int_a_paquete(paquete_solicitud_io, bytes);
                                }

                                // Creo la solicitud de entrada salida
                                t_solicitud_io *solicitud_io = crear_solicitud_io(pcb, paquete_solicitud_io);
                                
                                // Bloqueo al proceso
                                proceso_a_blocked(pcb, nombre_interfaz);
                                
                                // Encolo la solicitud
                                interfaz_encolar_solicitud_io(interfaz, solicitud_io);
                                break;
                            }
                            case COP_IO_FS_READ:
                            {
                                // Creo el paquete con la operacion a realizar y agrego el pid del proceso
                                t_paquete *paquete_solicitud_io = crear_paquete(operacion);
                                agregar_pid_a_paquete(paquete_solicitud_io, pcb_get_pid(pcb));
                                
                                // Desempaqueto los parametros de la operacion y los agrego al paquete
                                char *nombre_archivo = buffer_desempaquetar_string(buffer);
                                agregar_string_a_paquete(paquete_solicitud_io, nombre_archivo);

                                int puntero_archivo;
                                buffer_desempaquetar(buffer, &puntero_archivo);
                                agregar_int_a_paquete(paquete_solicitud_io, puntero_archivo);

                                int tamanio_a_leer;
                                buffer_desempaquetar(buffer, &tamanio_a_leer);
                                agregar_int_a_paquete(paquete_solicitud_io, tamanio_a_leer);

                                int cantidad_direcciones;
                                buffer_desempaquetar(buffer, &cantidad_direcciones);
                                agregar_int_a_paquete(paquete_solicitud_io, cantidad_direcciones);
                                
                                int direccion_fisica, bytes;
                                for(int i = 0; i < cantidad_direcciones; i++){
                                    buffer_desempaquetar(buffer, &direccion_fisica);
                                    agregar_int_a_paquete(paquete_solicitud_io, direccion_fisica);
                                    buffer_desempaquetar(buffer, &bytes);
                                    agregar_int_a_paquete(paquete_solicitud_io, bytes);
                                }

                                // Creo la solicitud de entrada salida
                                t_solicitud_io *solicitud_io = crear_solicitud_io(pcb, paquete_solicitud_io);
                                
                                // Bloqueo al proceso
                                proceso_a_blocked(pcb, nombre_interfaz);
                                
                                // Encolo la solicitud
                                interfaz_encolar_solicitud_io(interfaz, solicitud_io);
                                break;
                            }
                            default:
                                proceso_a_exit(pcb, FINALIZACION_INVALID_INTERFACE);
                                // sem_post(&sem_grado_multiprogramacion);
                        }
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
}

// PLANIFICADOR LARGO PLAZO ---------------------------------------------------

// Crea el pcb y lo encola en new
void iniciar_proceso(char *path){
    int pid = generar_pid();
    t_pcb *pcb = crear_pcb(pid, path);
    estado_encolar_pcb(estado_new, pcb);
    log_creacion_proceso(pcb);
    sem_post(estado_get_sem(estado_new));
}

void finalizar_proceso(int pid){
    // Detengo la planificacion para evitar las transiciones entre estados
    detener_planificacion(); // si detengo la plani deberia esperar a que se haya frenado por completo, porq si no se paro por completo puede haber procesos q hayan sido desencolados de una cola para poder meterlas en otra y no lo encuentre
    
    // pthread_mutex_lock(&mutex_lista_procesos_pendientes_a_finalizar);
    // list_add(procesos_pendientes_a_finalizar, pid); // cada vez q modifique la lista de procesos de un estado deberia corroborar que no este queriendo acceder a un proceso q se requiera finalizar
    // pthread_mutex_unlock(&mutex_lista_procesos_pendientes_a_finalizar);
    // FORMA 1
    
    // Busco al proceso entre las colas de los estados y lo desencolo cuando lo encuentro
    t_pcb *pcb = estado_rastrear_y_desencolar_pcb_por_pid(pid); // no esta mal porq lo encuentro y lo saco. Si llego no freno la plani, encontrar el estado y dsp aparte desencolarlo es al pedo porq pudo cambiar de estado
    // t_nombre_estado nombre_estado = estado_get_nombre_estado(pcb_get_estado(pcb));
    // FORMA 2
    // t_estado *estado = estado_rastrear_pcb_por_pid(pid);
    // if( estado == NULL ){
    //     log_error(kernel_logger, "El PCB solicitado no se encuentra en el sistema");
    // }// podria consultar si ya se encontro dsp de buscar en cada lista
    
    // Primero corroboro que el proceso buscado exista. En caso de existir actuo dependiendo el estado en el q se encuentre
    if( pcb != NULL ){
        // Agrego al proceso al diccionario de procesos a finalizar (util para estado exec y blocked)
        pthread_mutex_lock(&mutex_diccionario_procesos_a_finalizar); //nose si es necesario (el mutex), porq total la consulta en motivo_desalojo no se hace hasta q no se reanude la plani, es decir hasta q finalizar_proceso no termine -> SI ES NECESARIO PORQ LO MODIFICA EL HILO DE LIBERAR_PROCESOS_EXIT CUANDO TERMINA DE LIBERAR UN PROCESO
        diccionario_procesos_a_finalizar_agregar_proceso(diccionario_procesos_a_finalizar, pcb);
        pthread_mutex_unlock(&mutex_diccionario_procesos_a_finalizar);
        switch( pcb_get_estado(pcb) ){
            case NEW: // OK
                proceso_a_exit(pcb, FINALIZACION_INTERRUPTED_BY_USER);
                // sem_post(&sem_grado_multiprogramacion);
                break;
            case READY: // OK
                proceso_a_exit(pcb, FINALIZACION_INTERRUPTED_BY_USER);
                // sem_post(&sem_grado_multiprogramacion);
                break;
            case READY_PLUS: // OK
                proceso_a_exit(pcb, FINALIZACION_INTERRUPTED_BY_USER);
                // sem_post(&sem_grado_multiprogramacion);
                break;
            case BLOCKED: // OK
                // puedo agregar un atributo al pcb que sea nombre_recurso_causa_bloqueo donde almacene el nombre del recurso y sea mucho mas facil encontrar por q recurso se bloqueo y desbloquearlo
                // Reviso primero que la causa de bloqueo sea por un recurso. Si es por recurso => lo desencolo del recurso para que no genere inconvenientes el signal, pues si se mantiene en la lista del recurso, tras hacer un signal puede q se desbloquee a este proceso en lugar de a otro q si corresponde desbloquear. O q tras liberar sus recursos se desbloquee a el mismo, cuando de todas formas debe sacarse de esa cola. La liberacion de recursos (hacer los signals) se hace cuando ya esta en exit.
                if( pcb_esta_bloqueado_por_recurso(pcb) ){ // podria meterlo en una funcion tipo pcb_esta_bloqueado_por_recurso(pcb), aunque dsp tendria q volver a pedir el                 if( !string_is_empty(nombre_recurso_causa_bloqueo) ){ // podria meterlo en una funcion tipo pcb_esta_bloqueado_por_recurso(pcb), aunque dsp tendria q volver a pedir el nombre_recurso_causa_bloqueo dentro del if
                    // Obtengo el recurso por el cual se bloqueo el proceso
                    pthread_mutex_lock(&mutex_diccionario_recursos);
                    t_recurso *recurso = diccionario_recursos_get_recurso(diccionario_recursos, pcb_get_nombre_recurso_causa_bloqueo(pcb));
                    pthread_mutex_unlock(&mutex_diccionario_recursos);

                    // Desencolo al proceso de la lista de bloqueados del recurso
                    pthread_mutex_lock(recurso_get_mutex_recurso(recurso));
                    recurso_desencolar_proceso_por_pid(recurso, pcb_get_pid(pcb));
                    pthread_mutex_unlock(recurso_get_mutex_recurso(recurso));
                }

                // Si se bloqueo por interfaz no pasa nada. Si todavia no se atendio su solicitud, cuando sea atendida se descarta. Si la solicitud esta siendo atendida, cuando finaliza la io se descarta. (Para q esto funcione necesito usar la lista de pendiente finalizar, sino no tengo forma de saber q fue finalizado)

                proceso_a_exit(pcb, FINALIZACION_INTERRUPTED_BY_USER);
                // sem_post(&sem_grado_multiprogramacion);
                break;
            case EXEC: // OK
                // Mando interrupcion a CPU y la respuesta es manejada en manejar_motivo_desalojo
                t_paquete *paquete_interrupt_user = crear_paquete(INTERRUPT_USER);
                agregar_pid_a_paquete(paquete_interrupt_user, pcb_get_pid(pcb));
                pthread_mutex_lock(&mutex_socket_cpu_interrupt);
                enviar_paquete(fd_cpu_interrupt, paquete_interrupt_user);
                pthread_mutex_unlock(&mutex_socket_cpu_interrupt);
                eliminar_paquete(paquete_interrupt_user);
                break;
            case EXIT:
                log_error(kernel_logger, "El PCB solicitado ya esta siendo eliminado del sistema");
                break;
        }
    }
    else{
        log_error(kernel_logger, "El PCB solicitado no se encuentra en el sistema");
    }

    // Reanudo la planificacion
    iniciar_planificacion();
}

void planificador_largo_plazo(){
    // Manejar EXIT
    pthread_t hilo_liberar_procesos_exit;
    pthread_create(&hilo_liberar_procesos_exit, NULL, (void *)liberar_procesos_exit, NULL);
    pthread_detach(hilo_liberar_procesos_exit);

    // Manejar NEW -> READY
    while(1){
        sem_wait(&sem_control_cambio_grado_multiprogramacion); // Este semaforo es para garantizar que la modificacion (reduccion enrealidad es lo que genera complicaciones) se haga por completo y no se sigan mandando procesos a ready -> si reduzco el grado tengo que hacer waits, pero si ya estaba en 0 el hilo q reduce se va a bloquear al igual que este hilo al intentar mandar un proceso a ready y el signal tras finalizar un proceso podria activar cualquiera de los dos hilos y, en lugar de reducir el grado seguiria mandando procesos -> con este sem me aseguro q eso no pasa 
        sem_post(&sem_control_cambio_grado_multiprogramacion);
        sem_wait(&sem_grado_multiprogramacion);
        sem_wait(estado_get_sem(estado_new)); // debe haber elementos en la lista para poder desencolar - Pongo el wait del estado antes que el wait de la plani xq si lo hago al reves y no hay elementos en la cola, el hilo se va a bloquear con el wait de la plani tomado y nunca se va a detener la planificacion
        sem_wait(&sem_estado_planificacion_new_to_ready);
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
        sem_wait(estado_get_sem(estado_exit)); // debe haber elementos en la lista para poder desencolar
        t_pcb *pcb = estado_desencolar_primer_pcb(estado_exit);
        t_codigo_operacion respuesta_memoria = pedir_a_memoria_finalizar_proceso(pcb_get_pid(pcb));
        if( respuesta_memoria == CONFIRMACION_PROCESO_FINALIZADO ){
            if( !dictionary_is_empty(pcb_get_diccionario_recursos_usados(pcb)) ){
                liberar_recursos_usados(pcb);
            }
            pthread_mutex_lock(&mutex_diccionario_procesos_a_finalizar);
            if( diccionario_procesos_a_finalizar_proceso_esta_pendiente_de_finalizar(diccionario_procesos_a_finalizar, pcb) ){
                diccionario_procesos_a_finalizar_remover_proceso(diccionario_procesos_a_finalizar, pcb); // sacarlo del diccionario
            }
            pthread_mutex_unlock(&mutex_diccionario_procesos_a_finalizar);
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
    t_list *nombres_recursos = dictionary_keys(diccionario_recursos_usados);
    for(int i = 0; i < size_dict; i++){
        char *nombre_recurso = list_get(nombres_recursos, i);
        liberar_recurso(pcb, nombre_recurso);
    }

    list_destroy(nombres_recursos); // Cada char *nombre_recurso es liberado (free) en el signal con el dictionary_remove_and_destroy
}

void liberar_recurso(t_pcb *pcb, char *nombre_recurso){
    t_dictionary *diccionario_recursos_usados = pcb_get_diccionario_recursos_usados(pcb);
    int instancias_usadas = *(int *) dictionary_get(diccionario_recursos_usados, nombre_recurso);
    for(int i = 0; i < instancias_usadas; i++){
        sem_wait(&sem_estado_planificacion_blocked_to_ready_recurso); // de momento lo hago solo aca y no en manejar_motivo_desalojo xq ahi espero a terminar el signal para hacer el post de exec_to_... . No lo hago dentro del signal xq sino el post del exec_to_... nunca se llegaria a hacer.
        ejecutar_instruccion_signal(pcb, nombre_recurso);
        sem_post(&sem_estado_planificacion_blocked_to_ready_recurso);
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
    // sem_wait(&sem_estado_planificacion_blocked_to_ready_recurso); -> si dejo este semaforo nunca va a terminar el del motivo de desalojo => nunca se terminaria de detener la plani
    // OJO! MIENTRAS FINALIZO UNO PUEDE ESTAR HACIENDOSE SIGNALS PORQ SE ESTA LIBERANDO LOS RECURSOS DE OTRO FINALIZADO Y SE SEGUIRIAN HACIENDO LAS TRANSICIONES DE BLOCKED A READY -> PODRIA PONER EL SEMAFORO HACIENDOLE SANGUCHE AL SIGNAL
    if( recurso_debe_desbloquear_proceso(recurso) ){
        // Desbloqueo al primero proceso
        t_pcb *pcb_bloqueado = recurso_desencolar_primer_proceso(recurso);
        sem_wait(estado_get_sem(estado_blocked)); // debe haber elementos en la lista para poder desencolar
        estado_desencolar_pcb_por_pid(estado_blocked, pcb_get_pid(pcb_bloqueado)); // REVISAR SI ESTO ESTA BIEN, LA IDEA ES DESBLOQUEARLO DEL RECURSO Y TMB DE LA LISTA DE BLOQUEADOS
        pcb_set_nombre_recurso_causa_bloqueo(pcb_bloqueado, ""); // Para poder determinar que el proceso ya no esta bloqueado por ningun recurso
        // ACA PROBABLEMENTE TENGA Q HACER UN IF( LE_QUEDA_QUANTUM(PCB) ){ PROCESO_A_READY_PLUS } ELSE{ PROCESO_A_READY } -> NO! Por lo de abajo
        pcb_set_quantum_inicial(pcb_bloqueado, QUANTUM);
        pcb_set_quantum_restante(pcb_bloqueado, QUANTUM); // -> si no entendi mal, cuando un proceso se bloquea por wait, al desbloquearse siempre vuelve a ready y con el quantum inicial. Si el algoritmo es vrr, esto tmb se debe cumplir, xq la idea del mismo es beneficiar a los procesos que hacen io, por lo q un bloqueo por recurso no deberia ser beneficiado
        proceso_a_ready(pcb_bloqueado);
    }
    // sem_post(&sem_estado_planificacion_blocked_to_ready_recurso);

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
        pcb_set_nombre_recurso_causa_bloqueo(pcb, nombre_recurso); // Para poder determinar por cual recurso se bloqueo el proceso
        proceso_a_blocked(pcb, nombre_recurso);
    }

    pthread_mutex_unlock(recurso_get_mutex_recurso(recurso));
}

// PROCESO A ... ---------------------------------------------------

void proceso_a_ready(t_pcb *pcb){
	pcb_cambiar_estado_a(pcb, READY);
	estado_encolar_pcb(estado_ready, pcb);
    log_ingreso_ready(estado_ready);
    sem_post(estado_get_sem(estado_ready));
}

void proceso_a_ready_plus(t_pcb *pcb){
	pcb_cambiar_estado_a(pcb, READY_PLUS);
	estado_encolar_pcb(estado_ready_plus, pcb);
    log_ingreso_ready(estado_ready_plus);
    sem_post(estado_get_sem(estado_ready_plus));
}

void proceso_a_exec(t_pcb *pcb){
    pcb_cambiar_estado_a(pcb, EXEC);
    estado_encolar_pcb(estado_exec, pcb);
    sem_post(estado_get_sem(estado_exec));
}

void proceso_a_blocked(t_pcb *pcb, char *motivo_bloqueo){
    pcb_cambiar_estado_a(pcb, BLOCKED);
    estado_encolar_pcb(estado_blocked, pcb);
    log_motivo_bloqueo(pcb, motivo_bloqueo);
    sem_post(estado_get_sem(estado_blocked));
}

void proceso_a_exit(t_pcb *pcb, char *motivo_finalizacion){
    pcb_cambiar_estado_a(pcb, EXIT);
    estado_encolar_pcb(estado_exit, pcb);
    log_fin_proceso(pcb, motivo_finalizacion);
    sem_post(estado_get_sem(estado_exit));
}
