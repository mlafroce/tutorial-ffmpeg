# Demo ffmpeg

El repositorio contiene 3 aplicaciones de ejemplo para utilizar la biblioteca ffmpeg.

* **01-readinfo**: Esta aplicación recibe un nombre de archivo por parámetro e intenta cargar los metadatos correspondientes al archivo de video en cuestión. Esta aplicación estilo "hola mundo" es recomendable utilizarla para validar la correcta instalación de la biblioteca y generar supresiones de valgrind.

* **02-write-video**: Esta aplicación recibe un nombre de archivo y genera un video con datos de prueba. El contenedor se deduce de la extensión del archivo (`.mp4`, `.mkv`, `.webm`, etc.), si no se indica ninguna se genera un `.mp4` con video h.264.

* **03-sdl**: Esta aplicación abre una ventana de SDL, permite mover una imagen dentro de la misma, y graba un video con cada frame dibujado por SDL.

## Instalación de dependencias

Instalar las bibliotecas `libavutil-dev`, `libswresample-dev`, `libavformat-dev`, `libavcodec-dev` y `libswscale-dev`. El ejemplo de SDL necesita además `libsdl2-dev` y `libsdl2-image-dev`

### Instalación en ubuntu

~~~
sudo apt install libavutil-dev libswresample-dev libavformat-dev libavcodec-dev libswscale-dev libsdl2-dev libsdl2-image-dev
~~~

## Notas de diseño

Los ejemplos hacen un uso muy vago de RAII y excepciones, siendo responsabilidad del alumno en mejorar el diseño del las clases utilizadas para las demostraciones, adaptandolas a sus necesidades.

## Compilación

Compilar con cmake ejecutando

~~~
mkdir build
cd build
cmake ..
make -j<N cores>
~~~

Con *N cores* como el número de procesos en paralelo para compilar.

Descargar algún video de prueba, por ejemplo:

~~~
wget http://hubblesource.stsci.edu/sources/video/clips/details/images/grb_1.mpg -O demo.mp4
~~~
