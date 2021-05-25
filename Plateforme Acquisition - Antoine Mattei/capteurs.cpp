#include "capteurs.h"

Capteurs::Capteurs()
{
    Capteurs::pr_vent        = 0;
    Capteurs::pr_poids       = 0;
    Capteurs::pr_humidite    = 0;
    Capteurs::pr_luminosity  = 0;
    Capteurs::pr_temperature = 0;
}

Capteurs::Capteurs(double co_vitesseVent, double co_poidsAir, double co_luminosite, double co_temperature, double co_humidite)
{
    Capteurs::pr_vent = co_vitesseVent;
    Capteurs::pr_poids = co_poidsAir;
    Capteurs::pr_luminosity = co_luminosite;
    Capteurs::pr_temperature = co_temperature;
    Capteurs::pr_humidite = co_humidite;
}

double Capteurs::methodeReturn(int typeCapteur){
    switch (typeCapteur) {
    case Capteurs::VitesseVent:{
        return Capteurs::pr_vent;
    }
    case Capteurs::PoidsAire:{
        return Capteurs::pr_poids;
    }
    case Capteurs::Luminosite:{
        return Capteurs::pr_luminosity;
    }
    case Capteurs::Temperature:{
        return Capteurs::pr_temperature;
    }
    case Capteurs::Humidite:{
        return Capteurs::pr_humidite;
    }
    default:
        return 0;
    }
}

void Capteurs::methodeSet(int typeCapteur, int valeur){
    switch (typeCapteur) {
    case Capteurs::VitesseVent:{
        Capteurs::pr_vent = valeur;
        break;
    }
    case Capteurs::PoidsAire:{
        Capteurs::pr_poids = valeur;
        break;
    }
    case Capteurs::Luminosite:{
        Capteurs::pr_luminosity = valeur;
        break;
    }
    case Capteurs::Temperature:{
        Capteurs::pr_temperature = valeur;
        break;
    }
    case Capteurs::Humidite:{
        Capteurs::pr_humidite = valeur;
        break;
    }
    default:
        break;
    }
}
