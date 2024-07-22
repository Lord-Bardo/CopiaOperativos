#include "../include/memoria_utils.h"

// PROCESOS.
t_pcb_memoria* inicializar_proceso()
{
    t_pcb_memoria *proceso = malloc(sizeof(t_pcb_memoria));
    proceso->tabla_paginas = malloc((TAM_MEMORIA / TAM_PAGINA) * sizeof(t_pagina));
    proceso->path = NULL;
    proceso->memoria_de_instrucciones = malloc(TAM_MEMORIA * sizeof(char) * 40);

    for (int i = 0; i < (TAM_MEMORIA / TAM_PAGINA); i++) 
        proceso->tabla_paginas[i].num_frame = -1;

    for (int i = 0; i < TAM_MEMORIA; i++) 
        proceso->memoria_de_instrucciones[i] = malloc(sizeof(char) * 40);

    // Verificar si la asignación de memoria fue exitosa
    if (proceso->tabla_paginas == NULL || proceso->memoria_de_instrucciones == NULL) {
        // Manejar el error de asignación de memoria
        fprintf(stderr, "Error al asignar memoria\n");
        enviar_codigo_operacion(fd_kernel, ERROR_CREACION_PROCESO);
        exit(EXIT_FAILURE);
    }    

    return proceso;
}

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

int sizeof_proceso(t_pcb_memoria proceso){ // El tamaño de un proceso es igual a su cantidad de páginas.
    int size = 0;
    while(proceso.tabla_paginas[size].num_frame != -1 && size < (TAM_MEMORIA/TAM_PAGINA))
        size++;
    return size;
}

void asignar_size_proceso(int index, int size)
{
    int i = 0;
    while(i < size && procesos[index].tabla_paginas[i].num_frame <= TAM_MEMORIA/TAM_PAGINA){
            procesos[index].tabla_paginas[i].num_frame = frame_libre();
            frames_libres[procesos[index].tabla_paginas[i].num_frame] = false;
            procesos[index].tabla_paginas[i].bit_presencia = false;
            i++;
        }

    if(i == size && procesos[index].tabla_paginas[i].num_frame <= TAM_MEMORIA/TAM_PAGINA){
        // Log mínimo y obligatorio - Creación de Tabla de Páginas
        printf("Log mínimo y obligatorio - Creación de Tabla de Páginas \n");
        log_info(memoria_logger, "PID: %d - Tamaño: %d\n", procesos[index].pid, sizeof_proceso(procesos[index]));
        enviar_codigo_operacion(fd_cpu, CONFIRMACION_RESIZE);
    }
    else
        enviar_codigo_operacion(fd_cpu, OUT_OF_MEMORY);
}

void aumentar_proceso(int index, int size)
{
    //Log mínimo y obligatorio - Ampliación de Proceso
    printf("Log mínimo y obligatorio - Ampliación de Proceso\n");
    log_info(memoria_logger, "PID: %d - Tamaño Actual: %d - Tamaño a Ampliar: %d\n", procesos[index].pid, sizeof_proceso(procesos[index]), size);

    int i = sizeof_proceso(procesos[index]);
    while(i < size && procesos[index].tabla_paginas[i].num_frame <= TAM_MEMORIA/TAM_PAGINA){
        procesos[index].tabla_paginas[i].num_frame = frame_libre();
        frames_libres[procesos[index].tabla_paginas[i].num_frame] = false;
        procesos[index].tabla_paginas[i].bit_presencia = false;
        i++;
    }
    if(i == size && procesos[index].tabla_paginas[i].num_frame <= TAM_MEMORIA/TAM_PAGINA)
        enviar_codigo_operacion(fd_cpu, CONFIRMACION_RESIZE);
    else{
        log_info(memoria_logger, "Proceso excede el tamanio maximo de la memoria! PID: %d - Tamaño a Ampliar: %d\n", procesos[index].pid, size);
        enviar_codigo_operacion(fd_cpu, OUT_OF_MEMORY);
    }

}

void reducir_proceso(int index, int size)
{
    //Log mínimo y obligatorio - Reducción de Proceso
    printf("Log mínimo y obligatorio - Reducción de Proceso\n");
    log_info(memoria_logger, "PID: %d - Tamaño Actual: %d - Tamaño a Reducir: %d\n", procesos[index].pid, sizeof_proceso(procesos[index]), size);

    for(int i = sizeof_proceso(procesos[index]); i >= size; i--){
        frames_libres[procesos[index].tabla_paginas[i].num_frame] = true;
        procesos[index].tabla_paginas[i].num_frame = -1;
    }
    enviar_codigo_operacion(fd_cpu, CONFIRMACION_RESIZE);
}

void liberar_proceso(t_pcb_memoria* proceso)
{
    for (int i = 0; i < TAM_MEMORIA; i++) 
        free(proceso->memoria_de_instrucciones[i]);
        
    free(proceso->memoria_de_instrucciones);
    int cant_pags = sizeof_proceso(*proceso);
    free(proceso->tabla_paginas);
    // Log mínimo y obligatorio - Destrucción de Tabla de Páginas
    printf("Log mínimo y obligatorio - Destrucción de Tabla de Páginas:\n");
    log_info(memoria_logger, "PID: %d - Tamaño: %d", proceso->pid, cant_pags);
}

// AUXILIARES.
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

int frame_libre()
{
    int num_frame = 0;
    while(frames_libres[num_frame] == false && num_frame < TAM_MEMORIA/TAM_PAGINA)
        num_frame++;
    return num_frame;
}

int buscar_pid(int direc_fisica)
{
    int pid=0;
    return pid;
}

void escribir(int direc_fisica, int bytes, void* dato)
{
    // Verifico si la riección es válida.
   if(direc_fisica < 0 || direc_fisica + bytes > TAM_MEMORIA){
        printf("Dirección inválida\n");
        enviar_codigo_operacion(fd_cpu, ERROR_ESCRITURA);
        exit(EXIT_FAILURE);
    }

    // Escribo en el espacio de usuaro en la posición indicada.
    pthread_mutex_lock(&mutex_espacio_usuario); //protejo con semáfotos mutex el espacio de usuario
	memmove((char*)espacio_usuario + direc_fisica, dato, bytes);
    pthread_mutex_unlock(&mutex_espacio_usuario);
}

void leer(int direc_fisica, int bytes, void* dato)
{
    // Verifico si la riección es válida
    if(direc_fisica < 0 || direc_fisica + bytes > TAM_MEMORIA){
        printf("Dirección inválida\n");
        enviar_codigo_operacion(fd_cpu, ERROR_ESCRITURA);
        exit(EXIT_FAILURE);
    }

    // Leo el espacio de memoria y almaceno el dato leído.
    pthread_mutex_lock(&mutex_espacio_usuario); //protejo con semáfotos mutex el espacio de usuario
    memmove(dato, (char*)espacio_usuario + direc_fisica, bytes);
    pthread_mutex_unlock(&mutex_espacio_usuario);    
}

// MANEJO DE BUFFER.
void buffer_desempaquetar_proceso(t_buffer *buffer, t_pcb_memoria *proceso)
{
    buffer_desempaquetar(buffer, &(proceso->pid));
    proceso->path = buffer_desempaquetar_string(buffer);
}