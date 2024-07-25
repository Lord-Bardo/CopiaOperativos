#include "../include/cpu_kernel_interrupt.h"


/*
REcordemos prioridad de interrpts:
	-CLock
	-IO
	-NO SE
*/
nodo_t *lista_interrupciones=NULL; 

void atender_cpu_kernel_interrupt(){
    int continuar = 1;
	while(continuar){
		t_codigo_operacion cod_op;
		t_buffer * buffer = crear_buffer();
		int pid_interrupcion;
		t_interrupt interrupcion;
		recibir_paquete(fd_kernel_interrupt,&cod_op,buffer);
		switch(cod_op){
			case INTERRUPT_QUANTUM:
				log_info(cpu_logger, "Me llego una interrupcion de quantum de kernel");
				buffer_desempaquetar(buffer,&pid_interrupcion);
				interrupcion.pid=pid_interrupcion;
				interrupcion.motivo_desalojo=cod_op;
				eliminar_buffer(buffer);
				insertar_ordenado_lista_interrupciones(interrupcion);
				break;
			case INTERRUPT_USER:
				log_info(cpu_logger, "Me llego una interrupcion de quantum de kernel");
				buffer_desempaquetar(buffer,&pid_interrupcion);
				interrupcion.pid=pid_interrupcion;
				interrupcion.motivo_desalojo=cod_op;
				eliminar_buffer(buffer);
				insertar_ordenado_lista_interrupciones(interrupcion);
				break;
			default:
				log_warning(cpu_logger, "CPU INTERRUPT: Operacion desconocida recibida de KERNEL");
				//que deberia hacer aca??
				continuar = 0;
				break;
		}
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
