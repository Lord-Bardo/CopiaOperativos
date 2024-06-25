#include "../include/kernel_planificadores.h"

// VARIABLES GLOBALES
// Planificacion
t_estado_planificacion estado_planificacion;

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

// Semaforos
pthread_mutex_t mutex_grado_multiprogramacion;
sem_t sem_grado_multiprogramacion;;

// Recursos
t_dictionary *diccionario_recursos;

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
    inicializar_estructuras_estados();
    inicializar_estructuras_pid();
    inicializar_semaforos();
    inicializar_diccionario_recursos();
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

void inicializar_semaforos(){
    pthread_mutex_init(&mutex_grado_multiprogramacion, NULL);
    sem_init(&sem_grado_multiprogramacion, 0, GRADO_MULTIPROGRAMACION);
}

void inicializar_diccionario_recursos(){
    diccionario_recursos = crear_diccionario_recursos(RECURSOS, INSTANCIAS_RECURSOS);
}

void iniciar_planificacion(){
    estado_planificacion = ACTIVA;
}

void detener_planificacion(){
    estado_planificacion = PAUSADA;
}

void cambiar_grado_multiprogramacion_a(int nuevo_grado_multiprogramacion){
    pthread_mutex_lock(&mutex_grado_multiprogramacion);
    
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

    pthread_mutex_unlock(&mutex_grado_multiprogramacion);
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
    while( estado_planificacion ){
        t_pcb *pcb = elegir_proceso_segun_fifo();
        proceso_a_exec(pcb);
        enviar_contexto_de_ejecucion(pcb);
        recibir_contexto_de_ejecucion_actualizado(); // Modifica directo al pcb con lo que recibe
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
        recibir_contexto_de_ejecucion_actualizado(); // Modifica directo al pcb con lo que recibe
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
        recibir_contexto_de_ejecucion_actualizado(); // Modifica directo al pcb con lo que recibe
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

// Manda a CPU el contexto de la ejecucion (pid y registros) por el Dispatch
void enviar_contexto_de_ejecucion(t_pcb *pcb){
    t_paquete *paquete_contexto_de_ejecucion = crear_paquete(CONTEXTO_DE_EJECUCION);
    agregar_contexto_ejecucion_a_paquete(paquete_contexto_de_ejecucion, pcb);
    enviar_paquete(fd_cpu_dispatch, paquete_contexto_de_ejecucion);
    eliminar_paquete(paquete_contexto_de_ejecucion);
}

void recibir_contexto_de_ejecucion_actualizado(){ // TERMINAR
    // Espera por el Dispatch la llegada del contexto actualizado tras la ejecucion del proceso (pid y registros)
    // Junto con el contexto debe llegar el motivo por el cual finalizo la ejecucion (motivo de desalojo)
    // En cualquier caso se lo debe desencolar de EXEC -> NOTA: no necesariamente, si el proceso quiere hacer un signal deberia seguir ejecutando el mismo proceso
    // Si puede seguir ejecutando se lo encola en READY
    // Si no se bloqueo entonces se lo encola en BLOCKED
    t_codigo_operacion motivo_desalojo;
    t_buffer *buffer = crear_buffer();
    recibir_paquete(fd_cpu_dispatch, &motivo_desalojo, buffer);
    t_pcb *pcb = estado_desencolar_primer_pcb(estado_exec); // No se si lo correcto seria desencolarlo por lo del signal por ejemplo -> se podria desencolar, pero al hacer el signal se deberia meter primero en la cola de ready de maxima prioridad
    buffer_desempaquetar_contexto_ejecucion(buffer, pcb); // Modifica al pcb con lo que recibe
    
    switch(motivo_desalojo){
        case SUCCESS:
            break;
        case INTERRUPT_QUANTUM:
            break;
        case INTERRUPT_USER:
            break;
        case OUT_OF_MEMORY:
            break;
        case IO:
            // char *nombre_interfaz = buffer_desempaquetar_string(buffer);
            // if( interfaz_es_valida(nombre_interfaz) ){
            //     // Ejecutar interfaz
            //     // Hay varias, deberia hacer un switch para saber cual es y recibir lo q necesite
            // }
            // else{
            //     // Matar al proceso
            // }
            break;
        case WAIT:
            break;
        case SIGNAL:
            break;
        // ...
        default:
            log_error(kernel_logger, "Motivo de desalojo desconocido");
    }

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
    detener_planificacion();
    // Me gusta mas la idea de la lista negra
    // pthread_mutex_lock(mutex_lista_procesos_pendientes_a_finalizar);
    // list_add(procesos_pendientes_a_finalizar, pid);
    // pthread_mutex_unlock(mutex_lista_procesos_pendientes_a_finalizar);
    // FORMA 1
    // t_pcb *pcb = estado_rastrear_y_desencolar_pcb_por_pid(pid);
    // FORMA 2
    t_estado *estado = estado_rastrear_pcb_por_pid(pid);
    if( estado == NULL ){
        log_error(kernel_logger, "El PCB solicitado no se encuentra en el sistema");
    }
    switch(estado_get_nombre_estado(estado)){
        case BLOCKED:
            // Deberia mandarlo a exit cuando la io avise q termino
            break;
        case EXEC:
            // Mandar interrupcion a CPU
            break;
        case EXIT:
            log_error(kernel_logger, "El PCB solicitado ya esta siendo eliminado del sistema");
            break;
        default:
            t_pcb *pcb = estado_desencolar_pcb_por_pid(estado, pid);
            sem_post(&sem_grado_multiprogramacion);
    }
    
    // Se deberia liberar la memoria asignada al proceso
    // list_remove_by_condition();
    // proceso_a_exit();
    // sem_post(&sem_grado_multiprogramacion);

    // Pseudocodigo
    // detener_planificacion(); -> tiene que frenar las transiciones?
}

void planificador_largo_plazo(){
    // Manejar ESTADO -> EXIT
    pthread_t hilo_liberar_procesos_exit;
    pthread_create(&hilo_liberar_procesos_exit, NULL, (void *)liberar_procesos_exit, NULL);
    pthread_detach(hilo_liberar_procesos_exit);

    // Manejar NEW -> READY
    while( estado_planificacion ){
        sem_wait(&sem_grado_multiprogramacion);
        t_pcb *pcb = estado_desencolar_primer_pcb(estado_new);
        pedir_a_memoria_iniciar_proceso(pcb_get_pid(pcb), pcb_get_path(pcb));
        if( recibir_confirmacion_memoria_proceso_iniciado() == CONFIRMACION_PROCESO_INICIADO ){
            proceso_a_ready(pcb);
        }
        else{
            proceso_a_exit(pcb);
        } 
    }

    
    // // Estas funciones se podrían unir en una sola y hacer un for, pero capaz queda mas simple hacer estas 4 funciones y listo
    // manejador_new_exit();
    // manejador_ready_exit();
    // manejador_exec_exit();
    // manejador_blocked_exit();
}

void pedir_a_memoria_iniciar_proceso(int pid, char *path){
    t_paquete *paquete_solicitud_iniciar_proceso = crear_paquete(SOLICITUD_INICIAR_PROCESO);
    agregar_pid_a_paquete(paquete_solicitud_iniciar_proceso, pid);
    agregar_string_a_paquete(paquete_solicitud_iniciar_proceso, path);
    enviar_paquete(fd_memoria, paquete_solicitud_iniciar_proceso);
    eliminar_paquete(paquete_solicitud_iniciar_proceso);
}

t_codigo_operacion recibir_confirmacion_memoria_proceso_iniciado(){
    t_codigo_operacion codigo_operacion;
    recibir_codigo_operacion(fd_memoria, &codigo_operacion);
    return codigo_operacion;
}

void liberar_procesos_exit(){
    while(1){
        t_pcb *pcb = estado_desencolar_primer_pcb(estado_exit);
        pedir_a_memoria_finalizar_proceso(pcb_get_pid(pcb));
        // liberar_recursos(pcb); -> podria hacerlo dentro de eliminar_pcb()
        eliminar_pcb(pcb);
        sem_post(&sem_grado_multiprogramacion);
    }
}

void pedir_a_memoria_finalizar_proceso(int pid){
    t_paquete *paquete_solicitud_finalizar_proceso = crear_paquete(SOLICITUD_FINALIZAR_PROCESO);
    agregar_pid_a_paquete(paquete_solicitud_finalizar_proceso, pid);
    enviar_paquete(fd_memoria, paquete_solicitud_finalizar_proceso);
    eliminar_paquete(paquete_solicitud_finalizar_proceso);
}

void liberar_recursos(t_pcb *pcb){
    // TODO
}

// void manejador_new_exit() {
//     while (estado_planificacion) {
//         sem_wait(estado_get_sem(estado_new)); 
//         t_pcb *pcb = estado_desencolar_primer_pcb(estado_new);
//         proceso_a_exit(pcb);
//     }
// }

// void manejador_ready_exit() {
//     while (estado_planificacion) {
//         sem_wait(estado_get_sem(estado_ready));
//         t_pcb *pcb = estado_desencolar_primer_pcb(estado_ready);
//         proceso_a_exit(pcb);
//     }
// }

// void manejador_exec_exit() {
//     while (estado_planificacion) {
//         sem_wait(estado_get_sem(estado_exec));
//         t_pcb *pcb = estado_desencolar_primer_pcb(estado_exec);
//         proceso_a_exit(pcb);
//     }
// }

// void manejador_blocked_exit() {
//     while (estado_planificacion) {
//         sem_wait(estado_get_sem(estado_blocked));
//         t_pcb *pcb = estado_desencolar_primer_pcb(estado_blocked);
//         proceso_a_exit(pcb);
//     }
// }

// PROCESO A ... ---------------------------------------------------
void proceso_a_ready(t_pcb *pcb){
	pcb_cambiar_estado_a(pcb, READY);
	estado_encolar_pcb(estado_ready, pcb);
    log_ingreso_ready();
}

void proceso_a_exec(t_pcb *pcb){
    pcb_cambiar_estado_a(pcb, EXEC);
    estado_encolar_pcb(estado_exec, pcb);
}

void proceso_a_blocked(t_pcb *pcb){
    pcb_cambiar_estado_a(pcb, BLOCKED);
    estado_encolar_pcb(estado_blocked, pcb);
}

void proceso_a_exit(t_pcb *pcb){ // REVISAR
    pcb_cambiar_estado_a(pcb, EXIT);
    estado_encolar_pcb(estado_exit, pcb);
    // deberia loggear aca el finalizar_proceso o cuando lo elimino por completo o cuando llamo a finalizar proceso?
}