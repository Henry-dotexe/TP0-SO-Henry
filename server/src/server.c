#include "server.h"

int main(void) {
	int rta_handshake = 1;
	logger = log_create("log.log", "Servidor", 1, LOG_LEVEL_DEBUG);

	int server_fd = iniciar_servidor();
	log_info(logger, "Servidor listo para recibir al cliente");
	int cliente_fd = esperar_cliente(server_fd);

	t_list* lista;
	while (1) {
		int cod_op = recibir_operacion(cliente_fd);
		switch (cod_op) {
		case MENSAJE:
			recibir_mensaje(cliente_fd);
			break;
		case PAQUETE:
			lista = recibir_paquete(cliente_fd);
			log_info(logger, "Me llegaron los siguientes valores:\n");
			list_iterate(lista, (void*) iterator);
			break;
		case CHIMICHURRI: //Handshake
			send(cliente_fd,&rta_handshake,sizeof(rta_handshake),0);
			log_debug(logger,"Reconozco al cliente!");
			break;
		case -1:
			log_error(logger, "el cliente se desconecto. Terminando servidor");
			return EXIT_FAILURE;
		default:
			log_warning(logger,"Operacion desconocida. No quieras meter la pata");
			break;
		}
	}
	terminar_programa(cliente_fd,server_fd,logger); //Destruyo los pointers y sockets
	return EXIT_SUCCESS;
}

void iterator(char* value) {
	log_info(logger,"%s", value);
}

void terminar_programa(int conexion_cliente,int conexion_servidor, t_log* logger)
{
	  log_destroy(logger);
	  close(conexion_cliente);
	  close(conexion_servidor);
}
