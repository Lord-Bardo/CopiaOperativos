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
    pcb->PC = 0;
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

    pcb->diccionario_recursos_usados = dictionary_create(); // KEY: Nombre del recurso - Value: Cantidad de instancias usadas

    return pcb;
}

void eliminar_pcb(t_pcb *pcb){
    if( pcb != NULL ){
        if( pcb->path != NULL ){
            free(pcb->path);
        }

        if( pcb->diccionario_recursos_usados != NULL ){
            dictionary_destroy(pcb->diccionario_recursos_usados);
        }

        free(pcb);
        pcb = NULL; // para realizar comprobacion de si el proceso ya fue finalizado (en kernel_entradasalida.c por ejemplo)
    }
}

int generar_pid(){
	pthread_mutex_lock(&mutex_pid);
	int pid_proceso = pid_actual;
	pid_actual++;
	pthread_mutex_unlock(&mutex_pid);

	return pid_proceso;
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

void pcb_cambiar_estado_a(t_pcb *pcb, t_nombre_estado nuevo_estado){
    t_nombre_estado estado_anterior = pcb_get_estado(pcb);
    pcb->estado = nuevo_estado;
    log_cambio_estado(pcb, estado_anterior, nuevo_estado);
}

uint32_t pcb_get_pc(t_pcb *pcb){
    return pcb->PC;
}

char* pcb_get_path(t_pcb *pcb){
    return pcb->path;
}

t_dictionary *pcb_get_diccionario_recursos_usados(t_pcb *pcb){
    return pcb->diccionario_recursos_usados;
}

bool pcb_usa_recurso(t_pcb *pcb, char *nombre_recurso){
    return dictionary_has_key(pcb_get_diccionario_recursos_usados(pcb), nombre_recurso);
}