#include "../include/cpu_kernel_dispatch.h"

void atender_cpu_kernel_dispatch(){
    int continuar = 1;
	while( continuar ){
		int cod_op = recibir_operacion(fd_kernel_dispatch); // pese a q esto es un while(1) NO es un bucle que esta todo el tiempo usando el recurso del procesador, porq recibir_operacion usa recv que es una sys bloqueante y ahi corta la ejecucion hasta recibir algo
		switch(cod_op){
			case MENSAJE:
				// ...
				break;
			case PAQUETE:
				// ...
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