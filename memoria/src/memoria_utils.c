#include "../include/memoria_utils.h"

// PROCESOS.
t_pcb_memoria* inicializar_proceso()
{
    t_pcb_memoria *proceso = malloc(sizeof(t_pcb_memoria));
    proceso->tabla_paginas = list_create();
    proceso->path = NULL;
    proceso->memoria_de_instrucciones = list_create();

    return proceso;
}

void destruir_proceso(void* proceso_void)
{
    t_pcb_memoria* proceso = (t_pcb_memoria*) proceso_void;
    if(proceso != NULL){
        if(proceso->tabla_paginas != NULL)
            list_destroy_and_destroy_elements(proceso->tabla_paginas, free);
        if(proceso->path != NULL)
            free(proceso->path);
        if(proceso->memoria_de_instrucciones != NULL)
            list_destroy_and_destroy_elements(proceso->memoria_de_instrucciones, free);
        free(proceso);    
    }

}

void asignar_size_proceso(t_pcb_memoria* proceso, int size)
{
    int i = 0, frame = 0;
    t_pagina* pagina = malloc(sizeof(t_pagina));
    
    while(i < size && frame <= TAM_MEMORIA/TAM_PAGINA){
        pagina->num_frame = frame_libre();
        list_add(proceso->tabla_paginas, pagina);
        obtener_frame(i, &frame);
        bitarray_set_bit(frames_libres, frame);
        i++;
    }

    if(i == size && frame <= TAM_MEMORIA/TAM_PAGINA){
        // Log mínimo y obligatorio - Creación de Tabla de Páginas
        printf("Log mínimo y obligatorio - Creación de Tabla de Páginas \n");
        log_info(memoria_logger, "PID: %d - Tamaño: %d\n", proceso->pid, list_size(proceso->tabla_paginas));
        enviar_codigo_operacion(fd_cpu, CONFIRMACION_RESIZE);
    }
    else
        enviar_codigo_operacion(fd_cpu, OUT_OF_MEMORY);
}

void aumentar_proceso(t_pcb_memoria* proceso, int size)
{
    //Log mínimo y obligatorio - Ampliación de Proceso
    printf("Log mínimo y obligatorio - Ampliación de Proceso\n");
    log_info(memoria_logger, "PID: %d - Tamaño Actual: %d - Tamaño a Ampliar: %d\n", proceso->pid, list_size(proceso->tabla_paginas), size);

    int i = list_size(proceso->tabla_paginas), frame = 0;
    t_pagina* pagina = malloc(sizeof(t_pagina));
    
    while(i < size && frame <= TAM_MEMORIA/TAM_PAGINA){
        pagina->num_frame = frame_libre();
        list_add(proceso->tabla_paginas, pagina);
        obtener_frame(i, &frame);
        bitarray_set_bit(frames_libres, frame);;
        i++;
    }
    if(i == size && frame <= TAM_MEMORIA/TAM_PAGINA)
        enviar_codigo_operacion(fd_cpu, CONFIRMACION_RESIZE);
    else{
        log_info(memoria_logger, "Proceso excede el tamanio maximo de la memoria! PID: %d - Tamaño a Ampliar: %d - Tamaño Máximo: %d\n", proceso->pid, size, TAM_MEMORIA/TAM_PAGINA);
        enviar_codigo_operacion(fd_cpu, OUT_OF_MEMORY);
    }

}

void reducir_proceso(t_pcb_memoria* proceso, int size)
{
    //Log mínimo y obligatorio - Reducción de Proceso
    printf("Log mínimo y obligatorio - Reducción de Proceso\n");
    log_info(memoria_logger, "PID: %d - Tamaño Actual: %d - Tamaño a Reducir: %d\n", proceso->pid, list_size(proceso->tabla_paginas), size);

    int frame;
    
    for(int i = list_size(proceso->tabla_paginas); i >= size; i--){
        obtener_frame(i, &frame);
        bitarray_clean_bit(frames_libres, frame);
        list_remove_and_destroy_element(proceso->tabla_paginas, i, free);
    }
    enviar_codigo_operacion(fd_cpu, CONFIRMACION_RESIZE);
}

void obtener_frame(int pag, int* frame) 
{
	// Obtengo el proceso con el pid.
	t_pcb_memoria* proceso = list_find(procesos, comparar_pid_cpu);

	// Obtengo el frame.
	t_pagina* pagina_recibida = list_get(proceso->tabla_paginas, pag);
	*frame = pagina_recibida->num_frame;

	// Log mínimo y obligatorio - Acceso a Tabla de Páginas.
	printf("Log mínimo y obligatorio - Acceso a Tabla de Páginas \n");
	log_info(memoria_logger, "PID: %d - Pagina: %d - Marco: %d\n", proceso->pid, pag, *frame);
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
    while(bitarray_test_bit(frames_libres, num_frame) == true && num_frame < TAM_MEMORIA/TAM_PAGINA)
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

bool comparar_pid_kernel(void *pid){return pid_kernel == *(int*) pid;}
bool comparar_pid_cpu(void *pid){return pid_cpu == *(int*) pid;}
bool comparar_pid_es(void *pid){return pid_es == *(int*) pid;}

// MANEJO DE BUFFER.
void buffer_desempaquetar_proceso(t_buffer *buffer, t_pcb_memoria *proceso)
{
    buffer_desempaquetar(buffer, &(proceso->pid));
    proceso->path = buffer_desempaquetar_string(buffer);
}