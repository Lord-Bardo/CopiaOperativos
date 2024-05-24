#include "../include/serializar.h"

//--------------------------------------------------------------
void* s_malloc(size_t size) {
	void* ptr = malloc(size);
	if (ptr == NULL) {
		printf("Error: No hay más memoria disponible.\n");
		abort();
	}
	return ptr;
}
//--------------------------------------------------------------------


void agregar_paquete(t_paquete* paquete, void* dato_a_agregar, uint64_t* tamanio_dato_a_agregar) { //mando como 1er parametro en que paquete, 2do parametro lo que yo quiera agregar al paquete, 3er tamaño de lo que quiero agregar
    uint64_t offset = paquete->size;
    paquete->tamanio += sizeof(uint64_t) + *tamanio_dato_a_agregar;
    paquete->buffer = realloc(paquete->buffer, paquete->tamanio);
    memcpy(paquete->buffer + offset, tamanio_dato_a_agregar, sizeof(uint64_t));
    memcpy(paquete->buffer + offset + sizeof(uint64_t), value, *tamanio_dato_a_agregar); //falta manejo excepcion
}//notese que la funcion se lo mandamos al paquete y este agrega al buffer

t_package* decodificar_paquete(void* source, uint64_t* offset) { //verificar si sirve almacenando los datos decodificados en una nueva estructura t_paquete
	bool discarded_offset = offset == NULL; //revisar si es necesario ya que sería para "descartar el offset" si el puntero offset fue descartado
	if (discarded_offset) {
		offset = s_malloc(sizeof(uint64_t)); //s_malloc es un wrappler de malloc
		*offset = 0;}

	t_paquete* paquete = s_malloc(sizeof(t_paquete));
	memcpy(&(paquete->size), source + *offset, sizeof(uint64_t)); //revisar los uint
	*offset += sizeof(uint64_t);
	memcpy(&(paquete->type), source + *offset, sizeof(int32_t));
	*offset += sizeof(int32_t);
	paquete->buffer = s_malloc(paquete->size);
	memcpy(paquete->buffer, source + *offset, paquete->size);
	*offset += paquete->size;
	if (discarded_offset) free(offset);
	return paquete;
}

t_pcb* deserializar_pcb (t_paquete* paquete) {
	t_pcb* pcb = s_malloc(sizeof(t_pcb));
	uint64_t offset = 0;
	while (package_decode_isset(package, offset)) { //isset indica si hay algun valor valido para extraer de la estructura
		t_paquete* paquete_anidado = decodificar_paquete(paquete->buffer, &offset);
		switch (paquete_anidado->type) {
			/*case EC_INSTRUCTIONS: //ver que hacer con esto
				pcb->instrucciones = queue_create();
				deserialize_instructions(nested_package, pcb->instructions);
				break;*/
			case PC:
				decodificar_paquete_buffer(paquete_anidado->buffer, &(pcb->PC), NULL); //crear definicion del program_counter
				break;
			/*case LAST_BURST_TIME:
				package_decode_buffer(nested_package->buffer, &(ec->last_burst_time), NULL);
				break;*/
			case PROCESS_PID:
				decodificar_paquete_buffer(paquete_anidado->buffer, &(pcb->PID), NULL);
				break;
			case registros:
				pcb->registros= deserializar_cpu_registros(paquete_anidado->buffer);
				break;
			/*case SEGMENTS_TABLE:
				ec->segments_table = deserializar_segment_table(nested_package);
				break;*/
			default:
				printf("Error: Tipo de paquete desconocido.\n");
				return NULL;
		}
		destruir_paquete(paquete_anidado);
	}
	return pcb;
}
void decodificar_paquete_buffer(void* buffer, void* dest, uint64_t* offset) {
	bool discarded_offset = offset == NULL;
	if (discarded_offset) {
		offset = s_malloc(sizeof(uint64_t));
		*offset = 0;
	}
	uint64_t *size = s_malloc(sizeof(uint64_t));
	memcpy(size, buffer + *offset, sizeof(uint64_t));
	*offset += sizeof(uint64_t);
	memcpy(dest, buffer + *offset, *size);
	*offset += *size;
	free(size);
	if (discarded_offset) free(offset);
}


void destruir_paquete(t_paquete* paquete){
	free(paquete->buffer);
	paquete->buffer = NULL;
	free(paquete);
	paquete = NULL;
}



