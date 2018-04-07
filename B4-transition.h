#ifndef TRANSITION
#define TRANSITION

    #include <vector>
    #include <iostream>
    #include <algorithm>

    bool trier(int i, int j);

    class Transition
    {
        public:
            // Constructeurs
            Transition();
            Transition(int source, char caractere, int destination);
            // Setters
            void setSource(int source);
            void setCaractere(char caractere);
            void setDestination(int destination);
            void setNbSource(int source);
            void setNbDestination(int destination);
            // Getters
            int getSource(int index) const;
            char getCaractere() const;
            int getDestination(int index) const;
            int getNbSources();
            int getNbDestinations();
            // Méthodes
            void creerSource(int source);
            void creerDestination(int destination);
            void trierDestinations(int debut);
            void trierSources(int debut);
            bool destinationPresente(int destination);
        private:
            // Attributs
            int _nbSources;
            int _nbDestinations;
            std::vector<int> _source; // Tableau d'entiers (pour gérer les états à virgules : ex: 0,1)
            char _caractere;
            std::vector<int> _destination; // Tableau d'entiers (pour gérer les états à virgules : ex: 0,1)
    };

#endif
