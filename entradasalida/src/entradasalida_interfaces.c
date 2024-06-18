#include "../include/entradasalida_interfaces.h"

// El valor de la cant_unidades_trabajo viene de kernel y se multiplica por el valor que tiene el config de tiempo_unidad_trabajo
void interfaz_generica(int cant_unidades_trabajo) {
    int tiempo_unidad_trabajo = TIEMPO_UNIDAD_TRABAJO; // Tomar el valor del config
    int tiempo_total = (cant_unidades_trabajo * tiempo_unidad_trabajo) / 1000;

    // Simular el tiempo de espera
    log_info(entradasalida_logger, "E/S: haciendo un sleep");
    sleep(tiempo_total);
    log_info(entradasalida_logger, "E/S: deje de hacer un sleep");
}

void interfaz_stdin(/* Registro Dirección, Registro Tamaño */){
    // TODO: Preguntarle a mati si podríamos hacer una funcion en el utils porque se suele usar en varios lados el hecho de leer la linea de comandos
    char *leido;
	leido = readline("> ");

	while( leido[0] != '\0' ){
		// atender_comando(leido);
		leido = readline("> ");
	}
    // TODO: Ver como guardar el texto en memoria - ¿con guardar se refiere a escribir? Pregunta conectada con el módulo de memoria en la consigna

	free(leido);
}

// void interfaz_stdout(/* Registro Dirección, Registro Tamaño */){
// }
