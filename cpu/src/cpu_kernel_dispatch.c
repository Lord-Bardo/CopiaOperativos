#include "../include/cpu_kernel_dispatch.h"

void atender_cpu_kernel_dispatch(){
    int continuar = 1;
	while( continuar ){
		t_codigo_operacion *cod_op=NULL;
		t_buffer * buffer = crear_buffer();
		recibir_paquete(fd_memoria,cod_op,buffer);
		switch(*cod_op){
			case INSTRUCCION: // ... aca hay que recibir bb recive_buffer size instruccion
				
				break;
			case PCB:
				// ..
				break;
			case -1:
				log_error(cpu_logger, "KERNEL se desconecto del servidor CPU DISPATCH!");
				continuar = 0;
				break;
				// return EXIT_FAILURE; // Salida un poco drastica, deberia poder seguir andando la CPU aunque se desconecte el Kernel. Lo cambio por lo de continuar y un break
			default:
				log_warning(cpu_logger, "CPU DISPATCH: Operacion desconocida recibida de KERNEL");
				break;
		}
	}
}