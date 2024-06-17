#include "../include/entradasalida_interfaces.h"

// El valor de la cant_unidades_trabajo viene de kernel y se multiplica por el valor que tiene el config de tiempo_unidad_trabajo
void interfaz_generica(int cant_unidades_trabajo) {
    int tiempo_unidad_trabajo = TIEMPO_UNIDAD_TRABAJO; // Tomar el valor del config
    int tiempo_total = cant_unidades_trabajo * tiempo_unidad_trabajo;

    // Simular el tiempo de espera
    sleep(tiempo_total * 1000); // El *1000 es para pasar la unidad de trabajo a microsegundos
}

// void interfaz_stdin(/* Registro Dirección, Registro Tamaño */){
// }

// void interfaz_stdout(/* Registro Dirección, Registro Tamaño */){
// }

