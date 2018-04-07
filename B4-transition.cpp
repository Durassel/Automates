#include "B4-transition.h"

using namespace std;

bool trier(int i,int j)
{
    return (i<j);
}

// Constructeurs
Transition::Transition()
{
    _caractere = ' ';
}

Transition::Transition(int source, char caractere, int destination)
{
	_caractere = caractere;
    _source.push_back(source);
    _destination.push_back(destination);
    _nbSources = _nbDestinations = 1;
}

// Setters
void Transition::setSource(int source)
{
    _source[0] = source;
}

void Transition::setCaractere(char caractere)
{
    _caractere = caractere;
}

void Transition::setDestination(int destination)
{
    _destination[0] = destination;
}

void Transition::setNbSource(int source)
{
    _nbSources = source;
}

void Transition::setNbDestination(int destination)
{
    _nbDestinations = destination;
}

// Getters
int Transition::getSource(int index) const
{
    return _source[index];
}

char Transition::getCaractere() const
{
    return _caractere;
}

int Transition::getDestination(int index) const
{
    return _destination[index];
}

int Transition::getNbSources()
{
    return _nbSources;
}

int Transition::getNbDestinations()
{
    return _nbDestinations;
}

// Créer sources avec virgule
void Transition::creerSource(int source)
{
    _source.push_back(source);
}
// Créer destinations avec virgule
void Transition::creerDestination(int destination)
{
    _destination.push_back(destination);
}
// Trie les destinations
void Transition::trierDestinations(int debut)
{
    sort(_destination.begin()+debut, _destination.end(), trier);
}
// Trie les sources
void Transition::trierSources(int debut)
{
    sort(_source.begin()+debut, _source.end(), trier);
}
// Destination déjà présente dans le tableau _destination
bool Transition::destinationPresente(int destination)
{
    for(int j=0;j<_nbDestinations;j++)
    {
        if(destination == _destination[j])
            return true;
    }
    return false;
}
