#include "../include/cpu_kernel_interrupt.h"

void atender_cpu_kernel_interrupt(){
    int continuar = 1;
	while( continuar ){
		t_codigo_operacion cod_op;
		t_buffer * buffer = crear_buffer();
		recibir_paquete(fd_memoria,&cod_op,buffer);
		switch(cod_op){
			case 9:
				log_error(cpu_logger, "KERNEL se desconecto del servidor CPU INTERRUPT!");
				continuar = 0;
				break;
				// return EXIT_FAILURE; // Salida un poco drastica, deberia poder seguir andando la CPU aunque se desconecte el Kernel. Lo cambio por lo de continuar y un break
			default:
				log_warning(cpu_logger, "CPU INTERRUPT: Operacion desconocida recibida de KERNEL");
				break;
		}
	}
}