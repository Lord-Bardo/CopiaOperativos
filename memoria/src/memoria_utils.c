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
    
    while(i < size && frame < TAM_MEMORIA/TAM_PAGINA){
        t_pagina* pagina = crear_pagina(); 
        frame = pagina->num_frame;
        list_add(proceso->tabla_paginas, pagina);
        i++;
    }

    if(i == size && frame <= TAM_MEMORIA/TAM_PAGINA){
        // Log mínimo y obligatorio - Creación de Tabla de Páginas
        printf("Log mínimo y obligatorio - Creación de Tabla de Páginas \n");
        log_info(memoria_logger_min_y_obl, "PID: %d - Tamaño: %d\n", proceso->pid, list_size(proceso->tabla_paginas)*TAM_PAGINA);
        enviar_codigo_operacion(fd_cpu, CONFIRMACION_RESIZE);
    }
    else
        enviar_codigo_operacion(fd_cpu, OUT_OF_MEMORY);
}

void aumentar_proceso(t_pcb_memoria* proceso, int size)
{
    //Log mínimo y obligatorio - Ampliación de Proceso
    printf("Log mínimo y obligatorio - Ampliación de Proceso\n");
    log_info(memoria_logger_min_y_obl, "PID: %d - Tamaño Actual: %d - Tamaño a Ampliar: %d\n", proceso->pid, list_size(proceso->tabla_paginas)*TAM_PAGINA, size*TAM_PAGINA);

    int i = list_size(proceso->tabla_paginas);
    int frame = list_size(proceso->tabla_paginas);
    
    while(i < size && frame < TAM_MEMORIA/TAM_PAGINA){
        t_pagina* pagina = crear_pagina(); 
        frame = pagina->num_frame;
        list_add(proceso->tabla_paginas, pagina);
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
    log_info(memoria_logger_min_y_obl, "PID: %d - Tamaño Actual: %d - Tamaño a Reducir: %d\n", proceso->pid, list_size(proceso->tabla_paginas)*TAM_PAGINA, size*TAM_PAGINA);

    int frame;
    int tam_original = list_size(proceso->tabla_paginas);
    
    for(int i = tam_original - 1; i >= size; i--){
        obtener_frame(i, &frame);
        pthread_mutex_lock(&mutex_bitmap);
        bitarray_clean_bit(frames_libres, frame);
        pthread_mutex_unlock(&mutex_bitmap);
        list_remove_and_destroy_element(proceso->tabla_paginas, i, free);
    }
    enviar_codigo_operacion(fd_cpu, CONFIRMACION_RESIZE);
}

void obtener_frame(int pag, int* frame) 
{
	// Obtengo el proceso con el pid.
    pthread_mutex_lock(&mutex_procesos);
	t_pcb_memoria* proceso = list_find(procesos, comparar_pid_cpu);
    pthread_mutex_unlock(&mutex_procesos);

	// Obtengo el frame.
	t_pagina* pagina_recibida = list_get(proceso->tabla_paginas, pag);
   
	*frame = pagina_recibida->num_frame;

	// Log mínimo y obligatorio - Acceso a Tabla de Páginas.
	printf("Log mínimo y obligatorio - Acceso a Tabla de Páginas \n");
	log_info(memoria_logger_min_y_obl, "PID: %d - Pagina: %d - Marco: %d\n", proceso->pid, pag, *frame);
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

int obtener_primer_frame_libre()
{
    int num_frame = 0;
    pthread_mutex_lock(&mutex_bitmap);
    while(bitarray_test_bit(frames_libres, num_frame) == true && num_frame < TAM_MEMORIA/TAM_PAGINA)
        num_frame++;
    pthread_mutex_unlock(&mutex_bitmap);
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
	memmove(espacio_usuario + direc_fisica, dato, bytes);
    pthread_mutex_unlock(&mutex_espacio_usuario);

    //Imprimo lo escrito.
    uint8_t dato_escrito;
    pthread_mutex_lock(&mutex_espacio_usuario);
    memcpy(&dato_escrito, espacio_usuario + direc_fisica, sizeof(uint8_t));
    pthread_mutex_unlock(&mutex_espacio_usuario);
    //printf("Valor escrito en el espacio de usuario: %d\n", dato_escrito);
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
    memmove(dato, espacio_usuario + direc_fisica, bytes);
    pthread_mutex_unlock(&mutex_espacio_usuario);    

    // Imprimo lo leído.
    //printf("Valor leído del espacio de usuario: %d\n", *(uint8_t*)dato);
}

bool comparar_pid_kernel(void *pid){return pid_kernel == *(int*) pid;}
bool comparar_pid_cpu(void *pid){return pid_cpu == *(int*) pid;}
bool comparar_pid_es(void *pid){return pid_es == *(int*) pid;}

t_pagina *crear_pagina()
{
    t_pagina* pagina = malloc(sizeof(t_pagina));
    if( pagina == NULL ){
        log_info(memoria_logger, "Error al asignar memoria para la PAGINA!");
        return NULL;
    }
    pagina->num_frame = asignar_frame_libre();
    
    return pagina;
}

int asignar_frame_libre()
{
    int frame = obtener_primer_frame_libre();
    pthread_mutex_lock(&mutex_bitmap);
    bitarray_set_bit(frames_libres, frame);
    pthread_mutex_unlock(&mutex_bitmap);
    return frame;
}

// MANEJO DE BUFFER.
void buffer_desempaquetar_proceso(t_buffer *buffer, t_pcb_memoria *proceso)
{
    buffer_desempaquetar(buffer, &(proceso->pid));
    proceso->path = buffer_desempaquetar_string(buffer);
}
