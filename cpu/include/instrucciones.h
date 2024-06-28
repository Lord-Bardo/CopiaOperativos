#ifndef INSTRUCCIONES_H_
#define INSTRUCCIONES_H_


#include "procesos.h"



t_instruccion* fetch(int pid, int pc);
t_instruccion* pedidoAMemoria(int pid, int pc);


void execute(t_instruccion *instruccion);
void ejecutarSet(char * registro_string, char * valor_string);
void ejecutarSum(char * registro_destino, char * registro_origen);
void ejecutarSub(char * registro_destino, char * registro_origen);
void ejecutarJnz(char * registro_string, char * nro_instruccion_string);



t_instruccion* crear_instruccion();
void inicializarInstruccion(t_instruccion* instr, t_instr_code code, char* args[5]);
void liberar_instruccion(t_instruccion* instr);
void mostrarInstruccion(t_instruccion instr);

int obtener_numero_instruccion(char * cadena);
#endif