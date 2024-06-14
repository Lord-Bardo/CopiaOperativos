#include "../include/kernel_entradasalida.h"

void atender_kernel_entradasalida(){
    int continuar = 1;
	while( continuar ){
		// int cod_op = recibir_operacion(fd_entradasalida); // pese a q esto es un while(1) NO es un bucle que esta todo el tiempo usando el recurso del procesador, porq recibir_operacion usa recv que es una sys bloqueante y ahi corta la ejecucion hasta recibir algo
		// switch(cod_op){
		// 	// case :
		// 	// 	// ..
		// 	// 	break;
		// 	case -1:
		// 		log_error(kernel_logger, "ENTRADASALIDA se desconecto del servidor KERNEL!");
		// 		continuar = 0;
		// 		break;
		// 		// return EXIT_FAILURE; // Salida un poco drastica, deberia poder seguir andando KERNEL aunque se desconecte ENTRADASALIDA. Lo cambio por lo de continuar y un break
		// 	default:
		// 		log_warning(kernel_logger, "KERNEL: Operacion desconocida recibida de ENTRADASALIDA");
		// 		break;
		// }
	}
}