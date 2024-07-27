#ifndef IO_INTERFACE_H
#define IO_INTERFACE_H

#include "./entradasalida_gestor.h"

void interfaz_generica(int cant_unidades_trabajo);
void interfaz_stdin(t_list *lista_direcciones, int pid);
void interfaz_stdout(t_list* lista_direcciones, int cant_direcciones, int pid);
void interfaz_fs_create(char* filename, int pid);
void interfaz_fs_delete(char* filename, int pid);
void interfaz_fs_truncate(char* filename, int nuevo_tamanio, int pid);
void interfaz_fs_write(char* filename, int indice_archivo, int tamanio_a_escribir, int cant_direcciones, t_list* lista_direcciones, int pid);
void interfaz_fs_read(char* filename, int indice_archivo, int tamanio_a_leer, int cant_direcciones, t_list* lista_direcciones, int pid);

int cantidadBloques(int tamanio_bytes);
bool aumentarTamanioArchivo(t_config* metadata_file_config, int bloque_inicial, int tamanio_archivo, int cant_bloques_tam_archivo, int cant_bloques_a_aumentar, int pid);
void reducirTamanioArchivo(int bloque_inicial, int cant_bloques_a_reducir, int cant_bloques_tam_archivo);
void leerBloque(FILE* archivo_bloques, void* contenido, int bytes_desde, int cantidad_bytes);
void leerBloqueCompleto(FILE* archivo_bloques, void* contenido, int bytes_desde);
void leerArchivo(FILE* archivo_bloques, void* contenido, int tamanio_archivo, int bloque_inicial);
void escribirArchivo(FILE* archivo_bloques, void* contenido, int tamanio_archivo, int bloque_inicial);
void moverContenidoBloques(int bloque_inicial, int bloque_inicial_nuevo, int tamanio_archivo);
void liberarBloques(int bloque_inicial, int cant_bloques_tam_archivo);
bool hayEspacioContiguo(int* bloque_inicial, int cant_bloques_totales);
void ocuparBloques(int ocupar_desde, int cant_bloques_a_aumentar);
bool hayEspacioInmediato(int bloque_inicial, int cant_bloques_a_aumentar);
FILE* abrirArchivoBloques();
bool hayEspacioSuficiente(int cant_bloques_totales);
void guardarContenidoArchivo(void *contenido_archivo, int bloque_inicial, int tamanio_archivo);
void compactar(int *ultimo_bloque_ocupado,  int pid);

#endif