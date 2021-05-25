#ifndef CAPTEURS_H
#define CAPTEURS_H

/*! \mainpage Documentation du projet Aire Rapace
 * \section Introduction
 *
 * Les grandes agglomérations font face à une augmentation importante voire anarchique du nombre d’animaux considérés comme nuisibles par les habitants. Ce phénomène est dû en partie à la quasi-absence de prédateurs qui permettraient de réguler ces populations.
 * Les villes modernes ne présentent plus d’habitats typique propres à abriter ces prédateurs diurnes ou nocturnes du genre ruines, anfractuosités dans les murs ou les clochers.
 * Certaines municipalités en France et à l’étranger se sont rapprochées des associations de protection de la nature et plus particulièrement des oiseaux comme la LPO pour réfléchir à une réinsertion des rapaces en milieu urbain.
 * Elles ont décidé de développer des aires munies de capteurs permettant de collecter un ensemble de données exploitées pour déterminer quelles sont les meilleures conditions de nichage pour une espèce donnée. Ces données seront stockées pour être diffusées vers les villes intéressées par l’expérience.
 * Les aires seront installées en hauteur, à proximité du toit d’un bâtiment et la centrale d’acquisition qui lit les capteurs sera positionnée sur le toit. Cette centrale munie d’une caméra sera reliée à un système de diffusion d’images du nid en rez-de-chaussée de l’immeuble sur lequel on trouvera également la valeur des grandeurs mesurées : le poids de l’aire, la luminosité, l’humidité, la vitesse du vent et la température ambiante.
 *
 * Ce projet est accompagné de plusieurs classes.
 *
 * \section Classes
 *
 * \ref Capteurs
 *
 * \ref ssd1306
  */

/*!
 * \file capteurs.h capteurs.cpp
 * \class Capteurs capteurs.h "capteurs.h"
 * \brief The Capteurs class
 *
 * \author Antoine Mattei
 * \version 1.0
 * \date 2021
 * \pre Only the Capteurs::type_capteur enum can be use without initialization of the class.
 */
class Capteurs
{
private:
    /*!
     * \brief pr_vent
     * \details membre privé.
     */
    double pr_vent;
    /*!
     * \brief pr_poids
     * \details membre privé.
     */
    double pr_poids;
    /*!
     * \brief pr_luminosity
     * \details membre privé.
     */
    double pr_luminosity;
    /*!
     * \brief pr_temperature
     * \details membre privé.
     */
    double pr_temperature;
    /*!
     * \brief pr_humidite
     * \details membre privé.
     */
    double pr_humidite;
public:
    //! Constructeur par défaut.
    /*!
     * \brief Capteurs
     */
    Capteurs();
    //! Constructeur avec les valeurs à entrer.
    /*!
     * \brief Capteurs
     * \param co_vitesseVent Une valeur de type double correspondant à la vitesse du vent.
     * \param co_poidsAir Une valeur de type double correspondant au poids de l'aire.
     * \param co_luminosite Une valeur de type double correspondant à la luminosité
     * \param co_temperature Une valeur de type double correspondant à la température.
     * \param co_humidite Une valeur de type double correspondant à l'humidité.
     */
    Capteurs(double co_vitesseVent, double co_poidsAir, double co_luminosite, double co_temperature, double co_humidite);

    /*!
     * \brief The type_capteur enum
     */
    enum type_capteur{
        VitesseVent, /*!< Permet d'intéragir avec le capteur de vitesse de vent. Vaut 0x00*/
        PoidsAire,   /*!< Permet d'intéragir avec le capteur de poids de l'aire. Vaut 0x01*/
        Luminosite,  /*!< Permet d'intéragir avec le capteur de luminosité.      Vaut 0x02*/
        Temperature, /*!< Permet d'intéragir avec le capteur de tempéraure.      Vaut 0x03*/
        Humidite     /*!< Permet d'intéragir avec le capteur d'humidité.         Vaut 0x04*/
    };

    /*!
     * \brief methodeReturn
     * \param typeCapteur
     * \return
     *
     * \details Pour la valeur typeCapteur, il est conseillé d'utiliser l'énumération Capteurs::type_capteur

     *
     * \code{.cpp}
     * Capteurs capteur;
     * capteur.methodeReturn(Capteurs::VitesseVent); // Retourne la valeur de la vitesse du vent
     * \endcode
     */
    double methodeReturn(int typeCapteur);

    /*!
     * \brief methodeSet
     * \param typeCapteur
     * \param valeur
     *
     * \details Pour la valeur typeCapteur, il est conseillé d'utiliser l'énumération Capteurs::type_capteur
     *
     * \code{.cpp}
     * Capteurs capteur;
     * capteurs.methodeSet(Capteurs::PoidsAire, 13); // Donne la valeur 13 au poids de l'aire.
     * \endcode
     */
    void   methodeSet(int typeCapteur, int valeur);
};

#endif // CAPTEURS_H
