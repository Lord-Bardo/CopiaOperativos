#ifndef INSTRUCCIONES_H_
#define INSTRUCCIONES_H_


#include "procesos.h"
#include "mmu.h"


t_instruccion* fetch(int pid, int pc);
t_instruccion* pedidoAMemoria(int pid, int pc);
void print_element(void *data);
void execute(t_instruccion *instruccion);
void ejecutarSet(char * registro_string, char * valor_string);
void ejecutarMovIn(char* registro_datos, char* registro_direrccion);
void ejecutarMovOut(char* registro_direrccion,char* registro_datos);
void ejecutarSum(char * registro_destino, char * registro_origen);
void ejecutarSub(char * registro_destino, char * registro_origen);
void ejecutarJnz(char * registro_string, char * nro_instruccion_string);
void ejecutarResize(char* tamanio);
void ejecutarIoGenSleep(char* interfaz, char* tiempo_string);
void ejecutarCopyString(char *tamanio);
void ejecutarStdRead(char* interfaz, char *registro_direccion, char * registro_tamanio);
void ejecutarStdWrite(char * interfaz, char *registro_direccion, char * registro_tamanio);
void ejecutarWait(char * recurso);
void ejecutarSignal(char * recurso);
void ejecutarIOFsCreate(char * interfaz, char * archivo);
void ejecutarIOFsDelete(char * interfaz, char * archivo);
void ejecutarIOFsTruncate(char * interfaz, char * archivo, char * registro_tamanio);
void ejecutarIOFsWrite(char * interfaz, char * archivo,char* registro_direccion, char * registro_tamanio, char * registro_puntero);
void ejecutarIOFsRead(char * interfaz, char * archivo,char* registro_direccion, char * registro_tamanio, char * registro_puntero);

t_instruccion* crear_instruccion();
void inicializarInstruccion(t_instruccion* instr, t_instr_code code, char* args[5]);
void liberar_instruccion(t_instruccion* instr);
void mostrarInstruccion(t_instruccion instr);

int obtener_numero_instruccion(char * cadena);
#endif