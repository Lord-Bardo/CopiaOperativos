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
    memcpy(paquete->buffer + offset + sizeof(uint64_t), value, *tamanio_dato_a_agregar;
}//notese que la funcion se lo mandamos al paquete y este agrega al buffer

t_package* package_decode(void* source, uint64_t* offset) {
	bool discarded_offset = offset == NULL; //revisar si es necesario ya que sería para "descartar el offset" si el puntero offset fue descartado
	if (discarded_offset) {
		offset = s_malloc(sizeof(uint64_t)); //s_malloc es un wrappler de malloc
		*offset = 0;}

	t_package* package = s_malloc(sizeof(t_package));
	memcpy(&(package->size), source + *offset, sizeof(uint64_t));
	*offset += sizeof(uint64_t);
	memcpy(&(package->type), source + *offset, sizeof(int32_t));
	*offset += sizeof(int32_t);
	package->buffer = s_malloc(package->size);
	memcpy(package->buffer, source + *offset, package->size);
	*offset += package->size;
	if (discarded_offset) free(offset);
	return package;
}

t_pcb* deserializar_pcb (t_paquete* paquete) {
	t_execution_context* ec = s_malloc(sizeof(t_execution_context));
	ec->kernel_request = NULL;
	uint64_t offset = 0;
	while (package_decode_isset(package, offset)) { //isset indica si hay algun valor valido para extraer de la estructura
		t_paquete* paquete_anidado = package_decode(package->buffer, &offset);
		switch (nested_package->type) {
			case EC_INSTRUCTIONS:
				ec->instructions = queue_create();
				deserialize_instructions(nested_package, ec->instructions);
				break;
			case PROGRAM_COUNTER:
				package_decode_buffer(nested_package->buffer, &(ec->program_counter), NULL);
				break;
			case LAST_BURST_TIME:
				package_decode_buffer(nested_package->buffer, &(ec->last_burst_time), NULL);
				break;
			case PROCESS_PID:
				package_decode_buffer(nested_package->buffer, &(ec->pid), NULL);
				break;
			case CPU_REGISTERS:
				ec->cpu_register = deserialize_cpu_registers(nested_package->buffer);
				break;
			case SEGMENTS_TABLE:
				ec->segments_table = deserialize_segment_table(nested_package);
				break;
			case KERNEL_REQUEST: {
				t_package* kernel_request = package_decode(nested_package->buffer, NULL);
				ec->kernel_request = deserialize_instruction(kernel_request);
				package_destroy(kernel_request);
				break;
			}
			default:
				printf("Error: Tipo de paquete desconocido.\n");
				return NULL;
		}
		package_destroy(nested_package);
	}
	return ec;
}





