# Projet Aire Rapace
Projet Aire Rapace réalisé au Lycée Jean Perrin à Marseille lors de la seconde année du BTS Système Numérique Option Informatique et Réseau.

## Utilisation
Pour utiliser ce projet, il vous faudra une RaspberryPi avec l'une des dernières version de Qt5 installé, une camera pour RaspberryPi, la librairie raspciam trouvable [ici](https://github.com/cedricve/raspicam).

Clonez ensuite les codes, ouvrez le fichier .pro avec Qt5 installé sur une RaspberryPi.

Créez le serveur sur un PC tiers ainsi qu'une application sur un PC qui permettra d'envoyer des informations par communication série RS232.

Le PC RS232 enverra deux trames :
 - "$TRACQ1,x.x,y.y,*"
 - "$TRACQ2,a.a,b.b,c.c,*"

Le PC en réseau recevra une image en .ppm et une trame au format "$TRACQ,x.x,y.y,a.a,b.b,c.c,*"
