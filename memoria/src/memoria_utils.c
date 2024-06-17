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

// MANEJO DE BUFFER.
void buffer_desempaquetar_proceso(t_buffer *buffer, t_pcb_memoria *proceso)
{
    buffer_desempaquetar(buffer, &(proceso->pid));
    buffer_desempaquetar(buffer, &(proceso->path));
}