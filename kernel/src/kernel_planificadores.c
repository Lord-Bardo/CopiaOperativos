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
sem_t sem_grado_multiprogramacion;
sem_t sem_cpu_disponible;
pthread_mutex_t mutex_socket_dispatch;
pthread_mutex_t mutex_socket_memoria;

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

// PLANIFICADOR CORTO PLAZO
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

void planificador_corto_plazo_fifo(){
    while( estado_planificacion ){
        t_pcb *pcb = elegir_proceso_segun_fifo();
        proceso_a_exec(pcb);
        enviar_contexto_de_ejecucion(pcb);
        recibir_contexto_de_ejecucion_actualizado(pcb); // Modifica directo al pcb con lo que recibe
    }
}

t_pcb *elegir_proceso_segun_fifo(){
    return estado_desencolar_primer_pcb(estado_ready);
}

// TODO: corto plazo RR y VRR
void planificador_corto_plazo_rr(){
    // TODO
}

void planificador_corto_plazo_vrr(){
    // TODO
    // Tener en cuenta READY_PLUS
}

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
    // En cualquier caso se lo debe desencolar de EXEC
    // Si puede seguir ejecutando se lo encola en READY
    // Si no se bloqueo entonces se lo encola en BLOCKED
    t_codigo_operacion motivo_desalojo;
    t_buffer *buffer = crear_buffer();
    recibir_paquete(fd_cpu_dispatch, &motivo_desalojo, buffer);
    buffer_desempaquetar_contexto_ejecucion(buffer, pcb); // Modifica al pcb con lo que recibe

    // switch(motivo_desalojo){
    //     case :
    // }

    eliminar_buffer(buffer);
}

// PLANIFICADOR LARGO PLAZO
// Crea el pcb y lo encola en new
void iniciar_proceso(char *path){
    int pid = generar_pid();
    t_pcb *pcb = crear_pcb(pid, path);
    estado_encolar_pcb(estado_new, pcb);
    log_creacion_proceso(pcb);
}

void finalizar_proceso(int pid){ // TERMINAR
    // Se deberia liberar la memoria asignada al proceso


    sem_post(&sem_grado_multiprogramacion);
}

// El manejo de NEW -> READY habria que mandarlo a un hilo aparte
void planificador_largo_plazo(){
    // Manejar NEW -> READY
    pthread_t hilo_new_ready;
    pthread_create(&hilo_new_ready, NULL, (void *)planificador_largo_plazo_new_ready, NULL);
    pthread_detach(hilo_new_ready);

    // // Manejar ESTADO -> EXIT
    // // Estas funciones se podrían unir en una sola y hacer un for, pero capaz queda mas simple hacer estas 4 funciones y listo
    // manejador_new_exit();
    // manejador_ready_exit();
    // manejador_exec_exit();
    // manejador_blocked_exit();
}

void planificador_largo_plazo_new_ready(){
    while( estado_planificacion ){
        sem_wait(&sem_grado_multiprogramacion);
        t_pcb *pcb = estado_desencolar_primer_pcb(estado_new);
        pedir_a_memoria_iniciar_proceso(pcb_get_pid(pcb), pcb_get_path(pcb)); // Mati: calculo que memoria tendra una tabla con PIDs y sus path asociados 
        recibir_confirmacion_memoria_proceso_iniciado();
        proceso_a_ready(pcb);
    }
}

void pedir_a_memoria_iniciar_proceso(int pid, char *path){
    t_paquete *paquete_solicitud_iniciar_proceso = crear_paquete(SOLICITUD_INICIAR_PROCESO);
    agregar_pid_a_paquete(paquete_solicitud_iniciar_proceso, pid);
    agregar_string_a_paquete(paquete_solicitud_iniciar_proceso, path);
    enviar_paquete(fd_memoria, paquete_solicitud_iniciar_proceso);
    eliminar_paquete(paquete_solicitud_iniciar_proceso);
}

void recibir_confirmacion_memoria_proceso_iniciado(){
    t_codigo_operacion codigo_operacion;
    recibir_codigo_operacion(fd_memoria, &codigo_operacion);
    switch(codigo_operacion){
        case CONFIRMACION_PROCESO_INICIADO:
            break;
        case OUT_OF_MEMORY:
            break;
        default:
            
    }
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

// PROCESO A ...
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
    finalizar_proceso(pcb_get_pid(pcb)); // Lucho: Todavia no está hecha
    //log_salida_exit(pcb_get_pid(pcb)); // Lucho: Cómo logeamos el motivo? - completar la función de log
}