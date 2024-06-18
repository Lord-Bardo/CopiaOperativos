#include "../include/kernel_utils.h"

// LOGS OBLIGATORIOS ------------------------------------------------------------------

void log_creacion_proceso(t_pcb *pcb){
    log_info(kernel_logger_min_y_obl, "Se crea el proceso %d en NEW", pcb_get_pid(pcb));
}

// Finaliza el proceso <PID> - Motivo: <SUCCESS / INVALID_RESOURCE / INVALID_INTERFACE / OUT_OF_MEMORY / INTERRUPTED_BY_USER>
void log_fin_proceso(){
    //log_info(kernel_logger_min_y_obl, "Finaliza el proceso: %d - Motivo: " , pid , /* Acá iria el motivo en una variable (?) */); // Lucho: Completar
}

void log_cambio_estado(t_pcb *pcb, t_nombre_estado estado_anterior, t_nombre_estado nuevo_estado){
    log_info(kernel_logger_min_y_obl, "PID: %d - Estado Anterior: %s - Estado Actual: %s", pcb_get_pid(pcb), estado_get_nombre_estado_string(estado_anterior), estado_get_nombre_estado_string(nuevo_estado));
}

char *estado_get_nombre_estado_string(t_nombre_estado nombre_estado){
    switch(nombre_estado){
        case NEW:
            return "NEW";
        case READY:
            return "READY";
        case READY_PLUS:
            return "READY_PLUS";
        case EXEC:
            return "EXEC";
        case BLOCKED:
            return "BLOCKED";
        case EXIT:
            return "EXIT";
        default:
            return "";
    }
}

void log_motivo_bloqueo(){

}

void log_fin_quantum(){

}

void log_ingreso_ready(){
    log_info(kernel_logger_min_y_obl, "Cola Ready READY: %s", lista_pids_string(estado_ready)); // Mati: No se que poner en <COLA> -> Lo cambiamos por READY pero sigue la duda
}

char *lista_pids_string(t_estado *estado){
    t_list *lista_pids = list_map(estado_get_lista_procesos(estado), pcb_get_pid_transformer);

    char *lista_pids_string = string_new();
    string_append(&lista_pids_string, "[");
    for(int i=0; i < lista_pids->elements_count; i++){
        // Obtengo el pid
        // Casteo como puntero a int el puntero a void recibido de list_get y dsp con * accedo al contenido apuntado
        int pid = *(int *) list_get(lista_pids, i);
        
        // Paso el pid de int a string y lo agrego a la cadena
        // No lo hago todo en una linea para poder liberar correctamente la memoria reservada por string_itoa
        char *pid_string = string_itoa(pid);
        string_append(&lista_pids_string, pid_string);
        free(pid_string);

        // Agrego ',' si no es el ultimo pid de la lista
        if( i != (lista_pids->elements_count - 1) ){
            string_append(&lista_pids_string, ", ");
        }

    }
    string_append(&lista_pids_string,"]");

    list_destroy_and_destroy_elements(lista_pids, pid_destroyer);

    return lista_pids_string;
}

void *pcb_get_pid_transformer(void* pcb){
    t_pcb *temp_pcb = (t_pcb*) pcb;
    // Reservo memoria para persistir fuera de esta funcion al pid. Con malloc se va a alojar en el header y voy a poder usar el puntero en lista_pids_string(). En cambio, si lo declaro solo como int pid, se va a guardar en el stack de la funcion y al salir de esta funcion se va a perder el puntero.
    int *pid = malloc(sizeof(int));
    if( pid == NULL ){
        log_error(kernel_logger, "Error al reservar memoria para el PID");
        return NULL;
    }
    *pid = pcb_get_pid(temp_pcb);
    
    return (void*) pid;
}

void pid_destroyer(void *pid){
    free(pid);
}

// MANEJO PAQUETE ------------------------------------------------------------------

// Se podria generalizar a agregar_int_a_paquete(), pero me parecio mas representativo esto (pueden estar las dos de todas formas)
void agregar_pid_a_paquete(t_paquete *paquete, int pid){
    agregar_a_paquete(paquete, &pid, sizeof(pid));
}

void agregar_quantum_a_paquete(t_paquete *paquete, int quantum){ // NO SE USA
    agregar_a_paquete(paquete, &quantum, sizeof(quantum));
}

void agregar_estado_a_paquete(t_paquete *paquete, t_nombre_estado estado){
    agregar_a_paquete(paquete, &estado, sizeof(estado));
}

void agregar_uint32_a_paquete(t_paquete *paquete, uint32_t n){
    agregar_a_paquete(paquete, &n, sizeof(uint32_t));
}

void agregar_pc_a_paquete(t_paquete *paquete, uint32_t pc){
    agregar_uint32_a_paquete(paquete, pc);
}

void agregar_uint8_a_paquete(t_paquete *paquete, uint8_t n){ // NO SE USA
    agregar_a_paquete(paquete, &n, sizeof(uint8_t));
}

void agregar_string_a_paquete(t_paquete *paquete, char *string){
    agregar_a_paquete(paquete, string, string_length(string)+1); // +1 para contar el '\0'
}

void agregar_registros_a_paquete(t_paquete *paquete, t_registros *registros){
    char *ax = registros_get_registro_ax(registros);
    agregar_string_a_paquete(paquete, ax);
    free(ax);

    char *bx = registros_get_registro_bx(registros);
    agregar_string_a_paquete(paquete, bx);
    free(bx);

    char *cx = registros_get_registro_cx(registros);
    agregar_string_a_paquete(paquete, cx);
    free(cx);

    char *dx = registros_get_registro_dx(registros);
    agregar_string_a_paquete(paquete, dx);
    free(dx);

    char *eax = registros_get_registro_eax(registros);
    agregar_string_a_paquete(paquete, eax);
    free(eax);

    char *ebx = registros_get_registro_ebx(registros);
    agregar_string_a_paquete(paquete, ebx);
    free(ebx);

    char *ecx = registros_get_registro_ecx(registros);
    agregar_string_a_paquete(paquete, ecx);
    free(ecx);

    char *edx = registros_get_registro_edx(registros);
    agregar_string_a_paquete(paquete, edx);
    free(edx);

    char *si = registros_get_registro_si(registros);
    agregar_string_a_paquete(paquete, si);
    free(si);

    char *di = registros_get_registro_di(registros);
    agregar_string_a_paquete(paquete, di);
    free(di);
}

void agregar_pcb_a_paquete(t_paquete *paquete, t_pcb* pcb){ // NO SE USA
    agregar_pid_a_paquete(paquete, pcb_get_pid(pcb));
    agregar_quantum_a_paquete(paquete, pcb_get_quantum(pcb));
    agregar_estado_a_paquete(paquete, pcb_get_estado(pcb));
    agregar_registros_a_paquete(paquete, pcb_get_registros(pcb));
    agregar_string_a_paquete(paquete, pcb_get_path(pcb));
}

void agregar_contexto_ejecucion_a_paquete(t_paquete *paquete, t_pcb* pcb){
    agregar_pid_a_paquete(paquete, pcb_get_pid(pcb));
    agregar_pc_a_paquete(paquete, pcb_get_pc(pcb));
    agregar_registros_a_paquete(paquete, pcb_get_registros(pcb));
}

// MANEJO BUFFER
void buffer_desempaquetar_registros(t_buffer *buffer, t_registros *registros){
    // Si buffer_desempaquetar_string lee "\0", en registros_set_registro se va a dejar al registro como estaba -> Si estaba en NULL queda en NULL y sino quedara con lo q ya tenga
    registros_set_registro_ax(registros, buffer_desempaquetar_string(buffer));
    registros_set_registro_bx(registros, buffer_desempaquetar_string(buffer));
    registros_set_registro_cx(registros, buffer_desempaquetar_string(buffer));
    registros_set_registro_dx(registros, buffer_desempaquetar_string(buffer));
    registros_set_registro_eax(registros, buffer_desempaquetar_string(buffer));
    registros_set_registro_ebx(registros, buffer_desempaquetar_string(buffer));
    registros_set_registro_ecx(registros, buffer_desempaquetar_string(buffer));
    registros_set_registro_edx(registros, buffer_desempaquetar_string(buffer));
    registros_set_registro_si(registros, buffer_desempaquetar_string(buffer));
    registros_set_registro_di(registros, buffer_desempaquetar_string(buffer));
}

void buffer_desempaquetar_contexto_ejecucion(t_buffer *buffer, t_pcb* pcb){
    int pid_recibido;
    buffer_desempaquetar(buffer, &pid_recibido);
    if( pid_recibido != pcb_get_pid(pcb) ){
        log_error(kernel_logger, "El PID recibido no se corresponde con el PID del proceso en ejecucion");
    }
    
    buffer_desempaquetar(buffer, &(pcb->PC));
    buffer_desempaquetar_registros(buffer, pcb_get_registros(pcb));
}