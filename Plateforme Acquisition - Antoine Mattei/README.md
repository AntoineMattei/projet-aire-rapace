# Projet Aire Rapace
Projet Aire Rapace réalisé au Lycée Jean Perrin à Marseille (France) lors de la seconde année du BTS Système Numérique Option Informatique et Réseau.

## Utilisation
Pour utiliser ce projet, il vous faudra une RaspberryPi avec la dernière version de Qt5 installé, une camera pour RaspberryPi (la caméra utilisée pendant le projet peut être trouvée [ici](https://www.raspberrypi.org/products/camera-module-v2/), la bibliothèque raspicam trouvable [ici](https://github.com/cedricve/raspicam).

La bibliothèque ssd1306 provient de la bibliothèque C (modifiée en bibliothèque C++) trouvable [ici](https://github.com/iliapenev/ssd1306_i2c).

Clonez ensuite les codes, ouvrez le fichier .pro avec Qt5 installé sur la RaspberryPi.

Créez le serveur sur un PC tiers ainsi qu'une application sur un PC qui permettra d'envoyer des informations par communication série RS232.

Le PC RS232 enverra deux trames :
 - "$TRACQ1,x.x,y.y,*"
 - "$TRACQ2,a.a,b.b,c.c,*"

Le PC en réseau recevra une image en .ppm et une trame au format "$TRACQ,x.x,y.y,a.a,b.b,c.c,*"

***

# Aire Rapace Project
Aire Rapace Project done at Jean Perrin high school in Marseille (France) during the second year of the Associate Degree in Digital System, Computer and Network Option.

## Use
To use the project, you'll need a RaspberryPi with the last Qt5's version installed, a RaspiCam (the camera used in the project is findable [here](https://www.raspberrypi.org/products/camera-module-v2/) and the raspicam Library that you can find [here](https://github.com/cedricve/raspicam).

The ssd1306 library provides from the C library (modified in C++ library) findable [here](https://github.com/iliapenev/ssd1306_i2c).

Clone the code and open the .pro file with Qt5 installed on the RaspberryPi.

On a second computer, create a server and an app that will send the informations by RS232.

The RS232's app will send two frames:
 - "$TRACQ1,x.x,y.y,*"
 - "$TRACQ2,a.a,b.b,c.c,*"

The computer will receive a .ppm picture and a frame in the following format : ""$TRACQ,x.x,y.y,a.a,b.b,c.c,*"
