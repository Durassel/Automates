#include "B4-etat.h"

using namespace std;

// Constructeur
Etat::Etat()
{
    _id = -1;
    _initial = _terminal = false;
    _nbTransitions = 0;
}

// Setters
void Etat::setId(int id)
{
    _id = id;
}

void Etat::setInitial(bool initial)
{
    _initial = initial;
}

void Etat::setTerminal(bool terminal)
{
    _terminal = terminal;
}

void Etat::setNbTransitions(int nbTransitions)
{
    _nbTransitions = nbTransitions;
}

// Getters
int Etat::getId()
{
    return _id;
}

bool Etat::getInitial()
{
    return _initial;
}

bool Etat::getTerminal()
{
    return _terminal;
}

int Etat::getNbTransitions()
{
    return _nbTransitions;
}

int Etat::getTailleTransitions()
{
    return _transitions.size();
}

// Méthodes
void Etat::creerTransition(int source, char caractere, int destination) // Créer une transition (création automatique d'une source et d'une destination)
{
	_transitions.push_back(Transition(source, caractere, destination));
}

// Permet d'accéder au tableau _transitions depuis la classe automate
Transition * Etat::getIndexTransitions(int index)
{
    return &_transitions[index];
}

// Permet de savoir si une destionation est présente dans le tableau de transitions
bool Etat::destinationPresente(int index, int destination)
{
    for(int i=0;i<_transitions[index].getNbDestinations();i++)
    {
        if(destination == _transitions[index].getDestination(i))
            return true;
    }
    return false;
}

// Permet de savoir si une source est présente dans le tableau de transitions
bool Etat::sourcePresente(int index, int source)
{
    for(int i=0;i<_transitions[index].getNbSources();i++)
    {
        if(source == _transitions[index].getSource(i))
            return true;
    }
    return false;
}
