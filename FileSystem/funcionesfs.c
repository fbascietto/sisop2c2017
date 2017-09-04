
//format, rm, rename, mv, cat, mkdir, cpfrom, cpto, cpblock, md5, ls, info
escucharConsola(){
	char * linea;

	  while(1) {
		linea = readline("yamafs:" );
		if(linea)
		  add_history(linea);

		if(!strncmp(linea, "exit", 4)) {
		   free(linea);
		   break;
		} else
		if(!strncmp(linea, "format", 6)) {
			printf("Seleccionaste formatear\n");

		}else
		if(!strncmp(linea, "rm", 2)) {
			printf("Seleccionaste remover\n");

		}else
		if(!strncmp(linea, "rename", 6)) {
			printf("Seleccionaste renombrar\n");
		}
		else
		if(!strncmp(linea, "mv", 2)) {
			printf("Seleccionaste mover\n");

		}
		else
		if(!strncmp(linea, "cat", 3)) {
			printf("Seleccionaste concatenar\n");
		}
		else
		if(!strncmp(linea, "mkdir", 5)) {
			printf("Seleccionaste crear carpeta\n");

		}
		else
		if(!strncmp(linea, "cpfrom", 6)) {
			printf("Seleccionaste copiar desde\n");

		}else
		if(!strncmp(linea, "cpto", 4)) {
			printf("Seleccionaste copiar hasta\n");

		}else
		if(!strncmp(linea, "cpblock", 7)) {
			printf("Seleccionaste copiar bloque\n");

		}else
		if(!strncmp(linea, "md5", 3)) {
			printf("Seleccionaste obtener md5\n");

		}else
		if(!strncmp(linea, "ls", 2)) {
			printf("Seleccionaste ver directorios y archivos\n");

		}else
		if(!strncmp(linea, "info", 4)) {
			printf("Seleccionaste obtener informacion\n");

		}
		else {
			printf("no se reconoce el comando %s\n",linea);
		}

		free(linea);
  }
}
