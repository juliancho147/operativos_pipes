All: directorio monitor sensor
directorio: directorio.c
	gcc -o directorio directorio.c -ansi
monitor: monitor.c
	gcc -o monitor monitor.c -ansi
client: client.c
	gcc -o sensor sensor.c -ansi
