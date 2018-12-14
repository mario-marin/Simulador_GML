lamely we werent able yo solve Segmentation fault. 






README

Programador: Mario Andres Marin Soto, ROL:201321048-1
Asignatura: TEL341
Fecha de entrega: 24/07/2018

Supuestos:
	- Los usuarios hacen uso del canal de forma continua (no son considerados como paquetes) impidiendo que estos sean capases de volver
	  a entrar al canal antes de terminar de ser atendidos.
	- Los valores de lambda y mu son distintos de 0 ademas estos son determinados con ton y toff.
	- La cantidad de llegadas (criterio de parada), usuarios totales y capacidad de canal son mayores a 1.
	- El scheduler nunca estara vacio.

OBS:
	El scheduler fue proporsionado por el ayudante de la asignatura, este fue modificado, agregando un atributo "int usuario" permitiendo la identificacion 
	del usuario en el evento, ademas se cambio la variable "tiempo" de float a double.

	El codigo de lcgrand tambien fue proporcionado por el ayudante, este no fue modificado.

	El simulador es intencivo en el uso de CPU, tener esto en consideracion. 

	El simulador fue solo ejecutado en sistemas GNU/Linux

	Todas las simulaciones tienen como condicion de parada 1000000 llegadas, ademas de tener un Ton de 0.001 y una carga promedio de 0.3


Core C++

Compilacion: 
	Ejecutar el compilador de el sistema, se recomienda g++
	usango g++: 
		$ g++ main.cpp

Ejecucion:
	en linux:
		$ ./a.out {Nombre de topologuia (string)} {Capacidad canal (int)} {criterio de parada (int)} {Ton (float)} {Carga promedio (float)}

Jupyter

	Se ejecuta "jupyter notebook" en la carpeta del programa, luego se abre el archivo:
		 "Simulador_front_end.ipynb"

	Hecho esto se ejecuta el codigo de python de forma normal (porfavor tener paciencia).
	
	
_______________________________________________________________________

Herramienta adicional.

Para conocer la capacidad minima requerida para tener probabilidad de blockeo 0 se desarrollo una herramienta de apollo, esta se encuentra en la carpeta "network_tool"

Compilacion: 
	Ejecutar el compilador de el sistema, se recomienda g++
	usango g++: 
		$ g++ main.cpp

Ejecucion:
	en linux:
		$ ./a.out {Nombre de topologuia (string)}
