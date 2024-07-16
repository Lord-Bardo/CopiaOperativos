#ifndef CONEXION_H_
#define CONEXION_H_

#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include<assert.h>
#include<commons/log.h>
#include<commons/collections/list.h>

typedef enum{
    // HANDSHAKE
    HANDSHAKE_KERNEL,
    HANDSHAKE_CPU,
    HANDSHAKE_CPU_INTERRUPT,
    HANDSHAKE_CPU_DISPATCH,
    HANDSHAKE_MEMORIA,
    HANDSHAKE_ENTRADASALIDA,
    HANDSHAKE_OK,
    HANDSHAKE_ERROR,
    // KERNEL - MEMORIA
    SOLICITUD_INICIAR_PROCESO,
    CONFIRMACION_PROCESO_INICIADO, // este mensaje para qué lo necesita kernel? con qué info debería enviarse?
    ERROR_CREACION_PROCESO, 
    SOLICITUD_FINALIZAR_PROCESO,
    CONFIRMACION_PROCESO_FINALIZADO,
    // KERNEL - CPU DISPATCH
    CONTEXTO_DE_EJECUCION,
    // Motivos desalojo
    SUCCESS,
    INTERRUPT_QUANTUM,
    INTERRUPT_USER,
    OUT_OF_MEMORY,
    IO,
    WAIT,
    SIGNAL,
    // KERNEL - ENTRADASALIDA (manejo de interfaces)
    IO_GEN_SLEEP,
    IO_STDIN_READ,
    IO_STDOUT_WRITE,
    IO_FS_CREATE,
    IO_FS_DELETE,
    IO_FS_TRUNCATE,
    IO_FS_WRITE,
    IO_FS_READ,
    IO_FIN_OPERACION,
    IO_OPERACION_INVALIDA,
    // CPU - MEMORIA
    MENSAJE,
    PAQUETE,
    FETCH,
    INSTRUCCION,
    DATO, 
    PCB,//usado para mov_in y mov_out
    FRAME,
    OP_RESIZE,
    FETCH_ERROR,
    FRAME_ERROR,
    //ENTRADASALIDA - MEMORIA
    SOLICITUD_ESCRITURA,
    SOLICITUD_LECTURA,
    LECTURA_RESPUESTA
} t_codigo_operacion;

typedef struct{
	int size;
	void* stream; // void* es un puntero generico (puede apuntar a cualquier tipo de dato, pero tmp sabe a q tipo de dato apunta asi que no puedo sacarle informacion de eso y por eso es importante el size)
} t_buffer;

typedef struct{
	t_codigo_operacion codigo_operacion;
	t_buffer* buffer;
} t_paquete;

typedef enum{
    GENERICA,
    STDIN,
    STDOUT,
    DIALFS
} t_tipo_interfaz;

// CONEXION
// Cliente
int crear_conexion(char* ip, char* puerto);
// Servidor
int iniciar_servidor(char* puerto);
int esperar_cliente(int socket_servidor);
// Cliente y Servidor
void liberar_conexion(int socket);
// Handshake
void enviar_handshake(int socket, t_codigo_operacion handshake);
t_codigo_operacion recibir_handshake(int socket);

// PAQUETE Y BUFFER
// Crear y Eliminar
t_buffer *crear_buffer(void);
void eliminar_buffer(t_buffer *buffer);
t_paquete* crear_paquete(t_codigo_operacion codigo);
void eliminar_paquete(t_paquete *paquete);
// Enviar
void agregar_a_paquete(t_paquete* paquete, void* valor, int tamanio);
void *serializar_paquete(t_paquete *paquete, int bytes);
int enviar_paquete(int socket, t_paquete* paquete);
void enviar_codigo_operacion(int socket, t_codigo_operacion codigo_operacion);
// Recibir
int recibir_codigo_operacion(int socket, t_codigo_operacion *codigo_operacion);
void recibir_buffer(int socket, t_buffer *buffer);
void recibir_paquete(int socket, t_codigo_operacion *codigo_operacion, t_buffer *buffer);
// Desempaquetar Buffer
void buffer_actualizar_size(t_buffer *buffer, int bytes);
void buffer_actualizar_stream(t_buffer *buffer, int bytes);
void buffer_actualizar(t_buffer *buffer, int bytes);
void buffer_desempaquetar(t_buffer *buffer, void *destino);
char *buffer_desempaquetar_string(t_buffer *buffer);

#endif