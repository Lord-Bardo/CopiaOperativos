#ifndef REGISTROS_H_
#define REGISTROS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <commons/string.h>

// ESTRUCTURAS
typedef struct{
    // 1 byte
    char *AX;
    char *BX;
    char *CX;
    char *DX;
    // 4 bytes
    char *EAX;
    char *EBX;
    char *ECX;
    char *EDX;
    char *SI; //contiene la direccion logica de memoria de origen desde donde se va a copir un string
    char *DI; //contiene la direccion logica de memoria de destino desde donde se va a copir un string
} t_registros;

// FUNCIONES
t_registros *crear_registros(void);
void eliminar_registros(t_registros *registros);
// get
char *registros_get_registro(char *registro, int bytes_registro);
char *registros_get_registro_ax(t_registros *registros);
char *registros_get_registro_bx(t_registros *registros);
char *registros_get_registro_cx(t_registros *registros);
char *registros_get_registro_dx(t_registros *registros);
char *registros_get_registro_eax(t_registros *registros);
char *registros_get_registro_ebx(t_registros *registros);
char *registros_get_registro_ecx(t_registros *registros);
char *registros_get_registro_edx(t_registros *registros);
char *registros_get_registro_si(t_registros *registros);
char *registros_get_registro_di(t_registros *registros);
// set
void registros_set_registro(char **registro, char* valor, int bytes);
void registros_set_registro_ax(t_registros *registros, char *valor);
void registros_set_registro_bx(t_registros *registros, char *valor);
void registros_set_registro_cx(t_registros *registros, char *valor);
void registros_set_registro_dx(t_registros *registros, char *valor);
void registros_set_registro_eax(t_registros *registros, char *valor);
void registros_set_registro_ebx(t_registros *registros, char *valor);
void registros_set_registro_ecx(t_registros *registros, char *valor);
void registros_set_registro_edx(t_registros *registros, char *valor);
void registros_set_registro_si(t_registros *registros, char *valor);
void registros_set_registro_di(t_registros *registros, char *valor);


// OTRA FORMA:
// void registros_set_registro(char **registro, char* valor, int bytes_registro);

#endif