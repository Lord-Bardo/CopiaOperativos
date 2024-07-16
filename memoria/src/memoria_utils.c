#include "../include/memoria_utils.h"



int agregar_proceso(t_pcb_memoria proceso) // Podría poner esto en un utils, no śe...
{
    for (int i = 0; i < (TAM_MEMORIA/TAM_PAGINA); i++) {
        if (procesos[i].pid == -1) { // Celda vacía encontrada
            procesos[i] = proceso;
            return 1; // Éxito
        }
    }
    return 0; // Error, no hay celdas vacías (supongo que la máxima cant. de procesos que puede haber será TAM_MEMORIA/TAM_PAGINA, es decir procesos de una sola pág.)
}

int eliminar_proceso(int index)
{
//  Si no encuentra el proceso, salgo de la función.
    if(procesos[index].pid == -1)
        return 0; // 0 Si no se encontró el proceso

//  Desplazar los elementos del array para llenar el hueco dejado por el proceso eliminado.
    for (int j = index; j < (TAM_MEMORIA/TAM_PAGINA) - 1; j++) {
        procesos[j] = procesos[j + 1];
    }

    return 1; // Éxito, 1 si se encontró el proceso y se eliminó.  
}

int encontrar_proceso(int pid)
{
    int i = 0;
    while(procesos[i].pid != -1){
        if(procesos[i].pid == pid)
            return i;
    }
    return i;
}

bool instruccion_valida(char* instruccion) // Nos dice si la instruccion leida del archivo de pseudocodigo comienza con alguno de estos strings.
{
    return strstr(instruccion, "SET") == instruccion ||
           strstr(instruccion, "SUM") == instruccion ||
           strstr(instruccion, "SUB") == instruccion ||
           strstr(instruccion, "JNZ") == instruccion ||
           strstr(instruccion, "WAIT") == instruccion ||
           strstr(instruccion, "EXIT") == instruccion ||
           strstr(instruccion, "MOV_IN") == instruccion ||
           strstr(instruccion, "RESIZE") == instruccion ||
           strstr(instruccion, "SIGNAL") == instruccion ||
           strstr(instruccion, "MOV_OUT") == instruccion ||
           strstr(instruccion, "IO_FS_READ") == instruccion ||
           strstr(instruccion, "COPY_STRING") == instruccion ||
           strstr(instruccion, "IO_FS_WRITE") == instruccion ||
           strstr(instruccion, "IO_GEN_SLEEP") == instruccion ||
           strstr(instruccion, "IO_FS_CREATE") == instruccion ||
           strstr(instruccion, "IO_STDIN_READ") == instruccion ||
           strstr(instruccion, "IO_FS_DELETE ") == instruccion ||
           strstr(instruccion, "IO_FS_TRUNCATE") == instruccion ||
           strstr(instruccion, "IO_STDOUT_WRITE") == instruccion;
}

void liberar_pcb_memoria(t_pcb_memoria* proceso)
{
    for (int i = 0; i < TAM_MEMORIA; i++) 
        free(proceso->memoria_de_instrucciones[i]);
        
    free(proceso->memoria_de_instrucciones);
    free(proceso->tabla_paginas);
}

// MANEJO DE BUFFER.
void buffer_desempaquetar_proceso(t_buffer *buffer, t_pcb_memoria *proceso)
{
    buffer_desempaquetar(buffer, &(proceso->pid));
    proceso->path = buffer_desempaquetar_string(buffer);
}