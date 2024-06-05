#ifndef CPU_UTILS_
#define CPU_UTILS_

#include "cpu_gestor.h"

void agregar_instruccion_paquete(t_paquete *paquete, char* instruccion);
void agregar_pcb_paquete(t_paquete *paquete, t_pcb *pcb);
char* recibir_instruccion(int socket_cliente);
void ejecutar_instruccion(char **instruccion);

int pasar_registro_a_int(char* registro);
void ejecutar_SUM(t_instruccion* instruccion);
void ejecutar_SET(t_instruccion* instruccion);
void ejecutar_SUB(t_instruccion* instruccion);
void ejecutar_JNZ(t_instruccion* instruccion);
void ejecutar_MOV_OUT(t_instruccion* instruccion);
void ejecutar_MOV_IN(t_instruccion* instruccion);

bool son_dos_registros(t_list *argumentos);
bool son_registro_y_num(t_list *argumentos);
bool between(float valor, float min, float max);
int obtener_valor_registro(int reg);


#endif