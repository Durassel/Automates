#ifndef AUTOMATE
#define AUTOMATE

    // Includes
    #include <iostream>
    #include <iomanip>
    #include <string>
    #include <fstream>
    #include <vector>
    #include <algorithm>
    #include <sstream>
    #include "B4-etat.h"

    // Fonctions de comparaison pour trier le tableau de transitions
    bool cmd(const Transition & s1, const Transition & s2);
    bool cmd2(const Transition & s1, const Transition & s2);
    bool cmd3(const Transition & s1, const Transition & s2);

    class Automate
    {
        public :
            // Constructeurs
            Automate();
            Automate(std::string file);
            // Getters
            int getNbEntrees();
            int getNbSymboles();
            // Méthodes
            bool estSynchrone();
            bool estDeterministe();
            bool estComplet();
            void afficher();
            Automate * determinisation();
            Automate * completion();
            Automate * minimisation();
            bool test(std::string mot);
        private :
            // Attributs
            int _nbSymboles;
            int _nbEtats;
            int _nbEntrees;
            int _nbSorties;
            bool _poubelle;
            std::vector<int> _entrees; // Tableau des entrées de l'automate
            std::vector<int> _sorties; // Tableau des sorties de l'automate
            std::vector<Etat*> _etats; // Tableau d'états de l'automate
    };

#endif
