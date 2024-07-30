#include "../include/cpu_kernel_interrupt.h"


/*
REcordemos prioridad de interrpts:
	-CLock
	-IO
	-New
*/ 
//nodo_t *lista_interrupciones=NULL;
t_dictionary *diccionario_interrupciones;

void atender_cpu_kernel_interrupt(){
	
    int continuar = 1;
	while(continuar){
		t_codigo_operacion cod_op;
		t_buffer *buffer = crear_buffer();
		int pid_interrupcion;
		// t_interrupt interrupcion;
		recibir_paquete(fd_kernel_interrupt,&cod_op,buffer);
		switch(cod_op){
			case INTERRUPT_QUANTUM:
			{
				log_info(cpu_logger, "Me llego una interrupcion de quantum de kernel");
				buffer_desempaquetar(buffer,&pid_interrupcion);
				// interrupcion.pid=pid_interrupcion;
				// interrupcion.motivo_desalojo=cod_op;
				//insertar_ordenado_lista_interrupciones(interrupcion);
				t_codigo_operacion *tipo_interrupcion = malloc(sizeof(t_codigo_operacion));
				*tipo_interrupcion = cod_op;
				pthread_mutex_lock(&mutex_diccionario);
				if(!dictionary_has_key(diccionario_interrupciones, string_itoa(pid_interrupcion))){
					dictionary_put(diccionario_interrupciones, string_itoa(pid_interrupcion), tipo_interrupcion);
				}
				pthread_mutex_unlock(&mutex_diccionario);
				break;
			}
			case INTERRUPT_USER:
			{
				log_info(cpu_logger, "Me llego una interrupcion de quantum de kernel");
				buffer_desempaquetar(buffer,&pid_interrupcion);
				// interrupcion.pid=pid_interrupcion;
				// interrupcion.motivo_desalojo=cod_op;
				t_codigo_operacion *tipo_interrupcion = malloc(sizeof(t_codigo_operacion));
				*tipo_interrupcion = cod_op;
				pthread_mutex_lock(&mutex_diccionario);
				dictionary_put(diccionario_interrupciones, string_itoa(pid_interrupcion), tipo_interrupcion);
				pthread_mutex_unlock(&mutex_diccionario);
				//insertar_ordenado_lista_interrupciones(interrupcion);
				break;
			}
			default:
				log_warning(cpu_logger, "CPU INTERRUPT: Operacion desconocida recibida de KERNEL");
				//que deberia hacer aca??
				continuar = 0;
				break;
		}
		eliminar_buffer(buffer);
	}
}

/*
SUCCESS,
INTERRUPT_QUANTUM,
INTERRUPT_USER,
OUT_OF_MEMORY,
IO,
WAIT,
SIGNAL,
*/
