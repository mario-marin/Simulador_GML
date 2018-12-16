README

Programador: Mario Marin,     ROL:201321048-1
	     Luis Araya,      ROL:201330045-6
	     Carlos Elgueta,  ROL:201330018-9
Asignatura: TEL317
Fecha de entrega: 17/12/2018

OBS:

	El simulador es intencivo en el uso de CPU, tener esto en consideracion. 

	El simulador fue solo ejecutado en sistemas GNU/Linux


Core C++

Compilacion: 
	Ejecutar el compilador de el sistema, se recomienda g++
	usango g++: 
		$ g++ simulador_elastico.cpp

Ejecucion:
	en linux:
		$ ./a.out {Nombre de topologuia (string)} {Capacidad canal (int)} {Ton (float)} {Carga promedio (float)} {Generador (1 ó 0)}
		
Si la variable Generador tiene valor 1  el simulador generara un esenario nuevo, si por otro lado tiene valor 0, el simulador cargara un esenario previemente guardado en un archivo "timeline" en la carpeta del simulador.
	
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
