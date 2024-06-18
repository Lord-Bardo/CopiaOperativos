#include "../include/memoria_utils.h"

int agregar_proceso_a_procesos(t_pcb_memoria proceso) // Podría poner esto en un utils, no śe...
{
    for (int i = 0; i < (TAM_MEMORIA/TAM_PAGINA); i++) {
        if (procesos[i].id == -1) { // Celda vacía encontrada
            procesos[i] = proceso;
            return 1; // Éxito
        }
    }
    return 0; // Error, no hay celdas vacías (supongo que la máxima cant. de procesos que puede haber será TAM_MEMORIA/TAM_PAGINA, es decir procesos de una sola pág.)
}

bool instruccion_valida(const char* instruccion) // Nos dice si la instruccion leida del archivo de pseudocodigo comienza con alguno de estos strings.
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

// MANEJO DE BUFFER.
void buffer_desempaquetar_proceso(t_buffer *buffer, t_pcb_memoria *proceso)
{
    buffer_desempaquetar(buffer, &(proceso->pid));
    buffer_desempaquetar(buffer, &(proceso->path));
}