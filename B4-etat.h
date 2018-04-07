#ifndef ETAT
#define ETAT

    // Includes
    #include "B4-transition.h"
    #include <vector>

    class Etat
    {
        public:
            // Constructeur
            Etat();
            // Setters
            void setId(int id);
            void setInitial(bool initial);
            void setTerminal(bool terminal);
            void setNbTransitions(int nbTransitions);
            // Getters
            int getId();
            bool getInitial();
            bool getTerminal();
            int getNbTransitions();
            int getTailleTransitions();
            //Méthodes
            void creerTransition(int source, char caractere, int destination);
            Transition * getIndexTransitions(int index);
            bool destinationPresente(int index, int destination);
            bool sourcePresente(int index, int source);
        private:
            // Attributs
            int _id;
            bool _initial;
            bool _terminal;
            int _nbTransitions;
            std::vector<Transition> _transitions; // Tableau de transitions de l'état
    };

#endif
