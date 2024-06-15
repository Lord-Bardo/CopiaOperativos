#include "../include/kernel_pcb.h"

t_pcb *crear_pcb(int pid, char* path){
    t_pcb *pcb = malloc(sizeof(t_pcb));
    if( pcb == NULL ){
        log_error(kernel_logger, "Error al asignar memoria para el PCB");
        return NULL;
    }

    pcb->pid = pid;
    pcb->estado = NEW;
    pcb->quantum = QUANTUM;
    pcb->registros.PC = 0;
    pcb->registros.AX = 0;
    pcb->registros.BX = 0;
    pcb->registros.CX = 0;
    pcb->registros.DX = 0;
    pcb->registros.EAX = 0;
    pcb->registros.EBX = 0;
    pcb->registros.ECX = 0;
    pcb->registros.EDX = 0;
    pcb->registros.SI = 0;
    pcb->registros.DI = 0;
    pcb->path = string_new();
    if( pcb->path == NULL ){
        log_error(kernel_logger, "Error al asignar memoria para el PATH");
        return NULL;
    }
    string_append(&(pcb->path), path);

    return pcb;
}

int generar_pid(){
	pthread_mutex_lock(&mutex_pid);
	int pid_proceso = pid_actual;
	pid_actual++;
	pthread_mutex_unlock(&mutex_pid);

	return pid_proceso;
}

void eliminar_pcb(t_pcb *pcb){
    if( pcb != NULL ){
        if( pcb->path != NULL ){
            free(pcb->path);
        }
        free(pcb);
    }
}

int pcb_get_pid(t_pcb *pcb){
    return pcb->pid;
}

int pcb_get_quantum(t_pcb *pcb){
    return pcb->quantum;
}

t_nombre_estado pcb_get_estado(t_pcb *pcb){
    return pcb->estado;
}

t_registros pcb_get_registros(t_pcb *pcb){
    return pcb->registros;
}

char* pcb_get_path(t_pcb *pcb){
    return pcb->path;
}

void pcb_cambiar_estado_a(t_pcb *pcb, t_nombre_estado nuevo_estado){
    t_nombre_estado estado_anterior = pcb_get_estado(pcb);
    pcb->estado = nuevo_estado;
    log_cambio_estado(pcb, estado_anterior, nuevo_estado);
}