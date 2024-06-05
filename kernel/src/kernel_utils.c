#include "../include/kernel_utils.h"

void log_creacion_proceso(t_pcb *pcb){
    log_info(kernel_logger_min_y_obl, "Se crea el proceso %d en NEW", pcb_get_pid(pcb));
}

void log_fin_proceso(){

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
    log_info(kernel_logger_min_y_obl, "Cola Ready READY: %s:", lista_pids_string(estado_ready)); // Mati: No se que poner en <COLA> -> Lo cambiamos por READY pero sigue la duda
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

void *pcb_get_pid_transformer(void* pcb) {
    t_pcb *temp_pcb = (t_pcb*) pcb;
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