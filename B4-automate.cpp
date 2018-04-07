#include "B4-automate.h"

using namespace std;

// Fonctions de comparaison pour trier le tableau de transitions (TRIE LES TRANSITIONS)
// Selon la destination
bool cmd(const Transition & s1, const Transition & s2)
{
   if(s1.getDestination(0) < s2.getDestination(0))
        return true;
    else
        return false;
}
// Selon le caract�re
bool cmd2(const Transition & s1, const Transition & s2)
{
   if(s1.getCaractere() < s2.getCaractere())
        return true;
    else
        return false;
}
// Selon la source
bool cmd3(const Transition & s1, const Transition & s2)
{
   if(s1.getSource(0) < s2.getSource(0))
        return true;
    else
        return false;
}

// Constructeurs
Automate::Automate()
{
    // Initialisation des variables
    _nbSymboles = _nbEtats = _nbEntrees = _nbSorties = 0;
    _poubelle = false;
}

// Chargement des donn�es � la cr�ation de l'automate
Automate::Automate(string file)
{
    // Ouverture en mode lecture
    ifstream flux(file.c_str(), ios::in);

    // Chargement des donn�es
    flux >> _nbSymboles >> _nbEtats;

    // R�servation des �tats
    for(int i=0;i<_nbEtats;i++)
    {
		_etats.push_back(new Etat);
		_etats[i]->setId(i);
	}

    // Entr�e(s) : tableau d'entiers
    flux >> _nbEntrees;
    if(_nbEntrees > 0) // Test si aucune entr�e
    {
        for(int i=0;i<_nbEntrees;i++)
        {
            _entrees.push_back(0);
            flux >> _entrees[i];
            _entrees[i]--;
        }
        // Trie des entr�es
        sort(_entrees.begin(), _entrees.end(), trier);
    }

    // Initialisation des �tats initiaux
    for(int i=0;i<_nbEtats;i++)
    {
        for(int j=0;j<_nbEntrees;j++)
        {
            if(i == _entrees[j])
            {
                _etats[i]->setId(_entrees[j]);
                _etats[i]->setInitial(true);
            }
        }
    }

    // Sortie(s) : tableau d'entiers
    flux >> _nbSorties;
    if(_nbSorties > 0) // Test si aucune sortie
    {
        for(int i=0;i<_nbSorties;i++)
        {
            _sorties.push_back(0);
            flux >> _sorties[i];
            _sorties[i]--;
        }
        // Trie des entr�es
        sort(_sorties.begin(), _sorties.end(), trier);
    }

    // Initialisation des �tats terminaux
    for(int i=0;i<_nbEtats;i++)
    {
        for(int j=0;j<_nbSorties;j++)
        {
            if(i == _sorties[j])
            {
                _etats[i]->setId(_sorties[j]);
                _etats[i]->setTerminal(true);
            }
        }
    }

    // Transitions
    int nbTransitions(0);
    flux >> nbTransitions;

    // Tmp : tableau temporaire de toute les transitions qui sera ordonn� dans _transitions
    vector<Transition> _tmp;
    for(int i=0;i<nbTransitions;i++)
    {
        int a;
        char b;
        int c;
        flux >> a >> b >> c;
        if(b == '*')
            b = '{'; // Pour que la transition epsilon soit trier apr�s le dernier symbole
        _tmp.push_back(Transition(a-1, b, c-1)); // -1 car le sujet impose �tat � partir de 0
    }

    // Trie du tableau
    sort(_tmp.begin(), _tmp.end(), cmd);
    sort(_tmp.begin(), _tmp.end(), cmd2);
    sort(_tmp.begin(), _tmp.end(), cmd3);

    // Une fois tri�, on peut transformer les { en *
    for(int i=0;i<nbTransitions;i++)
    {
        if(_tmp[i].getCaractere() == '{')
            _tmp[i].setCaractere('*');
    }

    // Pour chaque �tat, r�server les transitions et le nombre de transitions
    int cpt(0);
    for(int i=0;i<_nbEtats;i++)
    {
        cpt = 0;
        // R�servation du nombre voulu de transitions
        for(int j=0;j<nbTransitions;j++) // = _tmp.size()
        {
            if(i == _tmp[j].getSource(0))
            {
                _etats[i]->creerTransition(_tmp[j].getSource(0), _tmp[j].getCaractere(), _tmp[j].getDestination(0)); // Cr�ation de la transition
                 cpt++;
            }
        }
        _etats[i]->setNbTransitions(cpt);
	}

    for(int i=0;i<_nbEtats;i++)
    {
        for(int j=0;j<_etats[i]->getNbTransitions();j++)
        {
            if(_etats[i]->getIndexTransitions(j)->getCaractere() == '*')
                _etats[i]->getIndexTransitions(j)->setCaractere('{');
        }
    }

	for(int i=0;i<_nbEtats;i++)
        sort(_etats[i]->getIndexTransitions(0), _etats[i]->getIndexTransitions(_etats[i]->getNbTransitions()), cmd);
    for(int i=0;i<_nbEtats;i++)
        sort(_etats[i]->getIndexTransitions(0), _etats[i]->getIndexTransitions(_etats[i]->getNbTransitions()), cmd2);
    for(int i=0;i<_nbEtats;i++)
        sort(_etats[i]->getIndexTransitions(0), _etats[i]->getIndexTransitions(_etats[i]->getNbTransitions()), cmd3);

    for(int i=0;i<_nbEtats;i++)
    {
        for(int j=0;j<_etats[i]->getNbTransitions();j++)
        {
            if(_etats[i]->getIndexTransitions(j)->getCaractere() == '{')
                _etats[i]->getIndexTransitions(j)->setCaractere('*');
        }
    }

    _poubelle = false;
}

// Getters
int Automate::getNbEntrees()
{
    return _nbEntrees;
}

int Automate::getNbSymboles()
{
    return _nbSymboles;
}

// M�thodes
bool Automate::estSynchrone()
{
    // Si transition * (epsilon), automate asynchrone
    for(int i=0;i<_nbEtats;i++)
    {
        for(int j=0;j<_etats[i]->getNbTransitions();j++)
        {
            if(_etats[i]->getIndexTransitions(j)->getCaractere() == '*')
                return false;
        }
    }
    return true;
}

bool Automate::estDeterministe()
{
    // Automate non d�terministe si :
    //  - plusieurs entr�es
    if(_nbEntrees > 1)
        return false;

    //  - plusieurs transitions sortantes de m�me caract�re d'un �tat
    for(int i=0;i<_nbEtats;i++) // On parcours tout les �tats
    {
        for(int j=0;j<_etats[i]->getNbTransitions();j++) // On parcours toutes les transitions
        {
            // Puisque le tableau est tri�, on regarde si la transition suivante a le m�me caract�re
            char caracterePrecedent;
            caracterePrecedent = _etats[i]->getIndexTransitions(j)->getCaractere();

            // Si oui, l'automate n'est pas d�terministe
            if(j+1 < _etats[i]->getNbTransitions() && caracterePrecedent == _etats[i]->getIndexTransitions(j+1)->getCaractere())
                return false;
        }
    }
    return true;
}

bool Automate::estComplet()
{
    // Complet: 1 �tat = nb symboles de transition
    for(int i=0;i<_nbEtats;i++) // On parcours tous les �tats
    {
        if(_etats[i]->getNbTransitions() != _nbSymboles) // Si le nombre de transitions de l'�tat est diff�rent du nombre de symboles, il n'est pas complet
            return false;
    }
    return true;
}

void Automate::afficher()
{
    // Affichage complexe car il prend en compte les espaces, les virgules � afficher ...

    // On recherche la source qui est la plus grande � afficher pour calculer les espaces
    int nbChiffresMax(0);
    for(int j=0;j<_nbEtats;j++) // On parcours tous les �tats
    {
        int nbChiffres(0);
        if(_etats[j]->getNbTransitions() == 0) // Si l'�tat n'a pas de transition, on affichera uniquement l'id (un ou deux chiffre(s) maximum)
            nbChiffresMax = 2;
        else // Sinon
        {
            // On compte le nombre total de chiffres de la source
            for(int k=0;k<_etats[j]->getIndexTransitions(0)->getNbSources();k++) // On regarde la premi�re transition car toutes les transitions ont la m�me source
            {
                // On compte le nombre de chiffre
                if(_etats[j]->getIndexTransitions(0)->getSource(k) < 10)
                    nbChiffres = nbChiffres + 1;
                else if(_etats[j]->getIndexTransitions(0)->getSource(k) >= 10)
                    nbChiffres = nbChiffres + 2;
            }

            if(nbChiffresMax < nbChiffres) // On enregistre le nombre maximum de chiffres de toutes les sources
                nbChiffresMax = nbChiffres;
        }
    }

    // Calcul des espaces n�cessaires (4 chiffres correspond environ � un \t
    int nbEspacesMax(0);
    if(nbChiffresMax % 4 == 0)
        nbEspacesMax = nbChiffresMax / 4;
    else
        nbEspacesMax = (nbChiffresMax / 4) + 1;

    // Affichage des espaces
    for(int k=0;k<nbEspacesMax+1;k++)
        cout << "\t";

    // Affichage premier symbole
    char a = 'a';
    cout << a;

    // Affichage de tous les symboles avec le bon nombre d'espaces
    for(int i=0;i<_nbSymboles;i++)
    {
        // Espaces apr�s le symbole
        // M�me m�thode que celle pr�sent�e pr�c�demment sauf que l'on regarde les destinations et non les sources
        nbChiffresMax = 0;
        for(int j=0;j<_nbEtats;j++)
        {
            int nbChiffres(0);
            for(int i=0;i<_etats[j]->getNbTransitions();i++)
            {
                if(a == _etats[j]->getIndexTransitions(i)->getCaractere())
                {
                    for(int k=0;k<_etats[j]->getIndexTransitions(i)->getNbDestinations();k++)
                    {
                        // On compte le nombre de chiffre
                        if(_etats[j]->getIndexTransitions(i)->getSource(k) < 10)
                            nbChiffres = nbChiffres + 1;
                        else if(_etats[j]->getIndexTransitions(i)->getSource(k) >= 10)
                            nbChiffres = nbChiffres + 2;
                    }

                    if(nbChiffresMax < nbChiffres)
                        nbChiffresMax = nbChiffres;

                    i = _etats[j]->getNbTransitions();
                }
            }
        }

        nbEspacesMax = 0;
        if(nbChiffresMax % 4 == 0)
            nbEspacesMax = nbChiffresMax / 4;
        else
            nbEspacesMax = (nbChiffresMax / 4) + 1;

        // Affichage des espaces n�cessaires
        if(nbChiffresMax % 4 == 0)
        {
            for(int k=0;k<nbEspacesMax + 1;k++)
                cout << "\t";
        }
        else
        {
            for(int k=0;k<nbEspacesMax;k++)
                cout << "\t";
        }

        // Affichage des symboles
        a++;
        if(i < _nbSymboles - 1)
            cout << a;
    }

    // Si l'automate est synchrone, on affiche * (repr�sente les transitions epsilons)
    if(this->estSynchrone() == false)
        cout << "*";
    cout << endl;

    int nbChiffresMaxSources(0); // Sert pour les espaces

    // On parcours tous les �tats � afficher
    for(int j=0;j<_nbEtats;j++)
    {
        // Affichage entr�e/sortie/poubelle
        if(_etats[j]->getInitial() == true)
            cout << " E";
        if(_etats[j]->getTerminal() == true)
            cout << " S";

        // Sert pour les espaces
        int nbChiffres(0);
        int nbEspaces(0);

        if(_poubelle == true && j == _nbEtats-1)
            cout << "\tP";
        else
        {
            cout << "\t"; // S�paration entre entr�es/sorties et les �tats

            // Affichage des sources (avec / sans virgule(s))
            if(_etats[j]->getNbTransitions() == 0) // Affichage id si l'�tat n'a pas de transition
                cout << _etats[j]->getId();
            else
            {
                // Affichage de la source + calcul des espaces n�cessaires (m�me m�thode que pr�c�demment)
                for(int i=0;i<_etats[j]->getNbTransitions();i++)
                {
                    for(int k=0;k<_etats[j]->getIndexTransitions(i)->getNbSources();k++)
                    {
                        if(_etats[j]->getIndexTransitions(i)->getSource(k) < 10)
                            nbChiffres = nbChiffres + 1;
                        else if(_etats[j]->getIndexTransitions(i)->getSource(k) >= 10)
                            nbChiffres = nbChiffres + 2;

                        cout << _etats[j]->getIndexTransitions(i)->getSource(k);
                        if(_etats[j]->getIndexTransitions(i)->getNbSources() > 1 && k+1 <_etats[j]->getIndexTransitions(i)->getNbSources())
                        {
                            cout << ",";
                        }
                    }

                    i = _etats[j]->getNbTransitions();
                }
                if(nbChiffresMaxSources < nbChiffres)
                    nbChiffresMaxSources = nbChiffres;
            }
        }

        // Calcul espaces n�cessaires
        int nbEspacesMax(0);
        if(nbChiffresMaxSources % 4 == 0)
            nbEspacesMax = nbChiffresMaxSources / 4;
        else
            nbEspacesMax = (nbChiffresMaxSources / 4) + 1;

        // 4 chiffres correspond environ � un \t
        nbEspaces = (nbChiffres-1) / 4;

        // Affichage des espaces apr�s la source
        if(nbChiffresMaxSources % 4 == 0)
        {
            for(int k=0;k<nbEspacesMax - nbEspaces + 1;k++)
                cout << "\t";
        }
        else
        {
            for(int k=0;k<nbEspacesMax - nbEspaces;k++)
                cout << "\t";
        }

        // Regarder la premi�re transition (si 'a' : aucun probl�me sinon afficher plus d'espaces pour aligner correctement les destinations)
        int index(0); // Index de la transition
        for(int k=0;k<_nbSymboles;k++) // On parcours les symboles
        {
            if(_etats[j]->getNbTransitions() > 0 && ('a'+k == _etats[j]->getIndexTransitions(0)->getCaractere() || _etats[j]->getIndexTransitions(0)->getCaractere() == '*'))
            {
                if(_etats[j]->getIndexTransitions(0)->getCaractere() == '*')
                    index = _nbSymboles;
                else
                    index = k;

                // Il manque les transitions avant index
                int nbEspacesTotal(0);
                for(int n=0;n<index;n++)
                {
                    // Pour chaque transition, compter le nombre d'espaces correspondant
                    nbChiffresMax = 0;
                    nbEspacesMax = 0;
                    for(int k=0;k<_nbEtats;k++) // Calcul des espaces (m�me m�thode que pr�c�demment)
                    {
                        for(int l=0;l<_etats[k]->getNbTransitions();l++)
                        {
                            if(_etats[k]->getIndexTransitions(l)->getCaractere() == 'a'+n)
                            {
                                int nbChiffres(0);
                                for(int m=0;m<_etats[k]->getIndexTransitions(l)->getNbDestinations();m++)
                                {
                                    if(_etats[k]->getIndexTransitions(l)->getDestination(m) < 10)
                                        nbChiffres = nbChiffres + 1;
                                    else if(_etats[k]->getIndexTransitions(l)->getDestination(m) >= 10)
                                        nbChiffres = nbChiffres + 2;
                                }
                                if(nbChiffresMax < nbChiffres)
                                    nbChiffresMax = nbChiffres;

                                l = _etats[k]->getNbTransitions();
                            }
                        }
                    }
                    if(nbChiffresMax % 4 == 0)
                        nbEspacesMax = nbChiffresMax / 4;
                    else
                        nbEspacesMax = (nbChiffresMax / 4) + 1;

                    nbEspacesTotal = nbEspacesTotal + nbEspacesMax;
                }

                // Affichage des espaces n�cessaires
                for(int l=0;l<nbEspacesTotal;l++)
                    cout << "\t";

                if(_etats[j]->getIndexTransitions(0)->getCaractere() == '*')
                    k = _nbSymboles;
            }
        }

        // Affichage des destinations
        for(int i=0;i<_etats[j]->getNbTransitions();i++) // On parcours les transitions
        {
            // Calcul des espaces n�cessaires (m�me m�thode)
            nbChiffresMax = 0;
            nbEspacesMax = 0;
            for(int k=0;k<_nbEtats;k++)
            {
                for(int l=0;l<_etats[k]->getNbTransitions();l++)
                {
                    if(_etats[k]->getIndexTransitions(l)->getCaractere() == _etats[j]->getIndexTransitions(i)->getCaractere())
                    {
                        int nbChiffres(0);
                        for(int m=0;m<_etats[k]->getIndexTransitions(l)->getNbDestinations();m++)
                        {
                            if(_etats[k]->getIndexTransitions(l)->getDestination(m) < 10)
                                nbChiffres = nbChiffres + 1;
                            else if(_etats[k]->getIndexTransitions(l)->getDestination(m) >= 10)
                                nbChiffres = nbChiffres + 2;
                        }
                        if(nbChiffresMax < nbChiffres)
                            nbChiffresMax = nbChiffres;

                        l = _etats[k]->getNbTransitions();
                    }
                }
            }

            if(nbChiffresMax % 4 == 0)
                nbEspacesMax = nbChiffresMax / 4;
            else
                nbEspacesMax = (nbChiffresMax / 4) + 1;

            // Affichage des destinations
            int nbChiffres(0);
            int nbEspaces(0);
            bool affichageEspace(false);
            if(_etats[j]->getIndexTransitions(i)->getDestination(0) == -1) // Destination = -1 => Etat poubelle
            {
                cout << 'P';
                nbChiffres = 1;
            }
            else
            {
                if(this->estDeterministe() == true) // Pour le calcul des espaces
                {
                    for(int k=0;k<_etats[j]->getIndexTransitions(i)->getNbDestinations();k++)
                    {
                        cout << _etats[j]->getIndexTransitions(i)->getDestination(k);
                        // plusieurs transitions avec meme caractere
                        if((_etats[j]->getIndexTransitions(i)->getNbDestinations() > 1 && k+1 < _etats[j]->getIndexTransitions(i)->getNbDestinations()))
                            cout << ",";

                        if(_etats[j]->getIndexTransitions(i)->getDestination(k) < 10)
                            nbChiffres = nbChiffres + 1;
                        else if(_etats[j]->getIndexTransitions(i)->getDestination(k) >= 10)
                            nbChiffres = nbChiffres + 2;
                    }
                }
                else // Pas d'espace n�cessaire
                {
                    for(int k=0;k<_etats[j]->getIndexTransitions(i)->getNbDestinations();k++)
                    {
                        cout << _etats[j]->getIndexTransitions(i)->getDestination(k) << ",";
                    }

                    if(i+1 < _etats[j]->getNbTransitions())
                    {
                        if(_etats[j]->getIndexTransitions(i+1)->getCaractere() != _etats[j]->getIndexTransitions(i)->getCaractere())
                        {
                            affichageEspace = true;
                        }
                    }
                }
            }

            nbEspaces = nbChiffres / 4;

            // Affichage des espaces
            if(this->estDeterministe() == true || affichageEspace == true)
            {
                if(nbChiffresMax % 4 == 0)
                {
                    for(int k=0;k<nbEspacesMax - nbEspaces + 1;k++)
                        cout << "\t";
                }
                else
                {
                    for(int k=0;k<nbEspacesMax - nbEspaces;k++)
                        cout << "\t";
                }
            }
        }
        cout << endl;
    }
    cout << endl << endl;
}

Automate * Automate::determinisation()
{
    // Cr�ation du nouvel automate
    Automate * afd = new Automate;
    // Initialisation
    afd->_nbEntrees = 1;
    afd->_nbSorties = 0;
    afd->_nbSymboles = _nbSymboles;
    afd->_nbEtats = 1;

    // Initialisation du premier �tat de l'AFD (entr�e)
    afd->_etats.push_back(new Etat);
    afd->_etats[0]->setInitial(true);
    afd->_etats[0]->setId(0);
    afd->_etats[0]->setNbTransitions(0);

    // Tmp : tableau temporaire de transitions des �tats initiaux
    vector<Transition> _tmp;
    for(int i=0;i<_nbEtats;i++) // On parcours les �tats
    {
        if(_etats[i]->getInitial() == true) // S'il est initial
        {
            for(int j=0;j<_etats[i]->getNbTransitions();j++) // On sauvegarde les transitions
            {
                _tmp.push_back(Transition(i, _etats[i]->getIndexTransitions(j)->getCaractere(), _etats[i]->getIndexTransitions(j)->getDestination(0)));
            }
        }
    }

    // R�servation des transitions (1 pour chaque caractere)
    for(int j=0;j<_nbSymboles;j++) // On parcours les symboles
    {
        for(unsigned int i=0;i<_tmp.size();i++) // On parcours les transitions
        {
            if(_tmp[i].getCaractere() == (char)('a'+j) || _tmp[i].getCaractere() == '*') // On r�serve une transition pour chaque caract�re (symbole)
            {
                afd->_etats[0]->setNbTransitions(afd->_etats[0]->getNbTransitions()+1); // Augmentation du nombre de transitions puisque l'on en cr�e une
                afd->_etats[0]->creerTransition(0, (char)('a'+j), 0); // Initialisation transition avec son caract�re
                i = _tmp.size();
            }
        }
    }

    // On initialise les sources de l'�tat initial
    for(int j=0;j<_nbEtats;j++)
    {
        for(unsigned int i=0;i<_tmp.size();i++)
        {
            if(j == 0) // Lorsque l'on cr�e une transition, une source est automatiquement cr��e, on la met alors � jour
            {
                for(int k=0;k<afd->_etats[0]->getNbTransitions();k++)
                {
                    afd->_etats[0]->getIndexTransitions(k)->setSource(0+j);
                }
                i = _tmp.size();
            }
            else if(j != 0 && _tmp[i].getSource(0) == 0+j) // Si on a besoin d'une source avec virgule, on cr�e une nouvelle source (+ mise � jour du nombre de sources)
            {
                for(int k=0;k<afd->_etats[0]->getNbTransitions();k++)
                {
                    afd->_etats[0]->getIndexTransitions(k)->setNbSource(afd->_etats[0]->getIndexTransitions(k)->getNbSources()+1);
                    afd->_etats[0]->getIndexTransitions(k)->creerSource(0+j);
                }
                i = _tmp.size();
            }
        }
    }

    // Si automate asynchrone, il faut ajouter les transitions epsilons � la source
    if(this->estSynchrone() == false)
    {
        // Les entr�es ont-elles des transitions epsilons ?
        for(int i=0;i<afd->_etats[0]->getIndexTransitions(0)->getNbSources();i++)
        {
            for(int j=0;j<_nbEtats;j++) // On parcours les �tats
            {
                for(int k=0;k<_etats[j]->getNbTransitions();k++) // On parcours les transitions
                {
                    if(_etats[j]->getIndexTransitions(k)->getSource(0) == afd->_etats[0]->getIndexTransitions(0)->getSource(i)) // Si on a trouv� la source
                    {
                        if(_etats[j]->getIndexTransitions(k)->getCaractere() == '*') // Et que la transition est epsilon
                        {
                            // On v�rifie avant que l'�tat n'est pas d�j� pr�sent dans la source
                            if(afd->_etats[0]->sourcePresente(0, _etats[j]->getIndexTransitions(k)->getDestination(0)) == false) // Si non, on l'ajoute
                            {
                                afd->_etats[0]->getIndexTransitions(0)->setNbSource(afd->_etats[0]->getIndexTransitions(0)->getNbSources()+1);
                                afd->_etats[0]->getIndexTransitions(0)->creerSource(_etats[j]->getIndexTransitions(k)->getDestination(0));
                            }
                        }
                    }
                }
            }
        }
    }

    // Trie des sources dans l'ordre
    if(afd->_etats[0]->getNbTransitions() > 0)
        afd->_etats[0]->getIndexTransitions(0)->trierSources(0);

    // On initialise les destinations de l'�tat initial
    for(int h=0;h<afd->_nbSymboles;h++) // On parcours les symboles
    {
        bool first(true);
        for(int i=0;i<afd->_etats[0]->getIndexTransitions(0)->getNbSources();i++) // On parcours les sources
        {
            for(int j=0;j<_nbEtats;j++) // On parcours les �tats
            {
                for(int k=0;k<_etats[j]->getNbTransitions();k++) // On parcours chaque transition
                {
                    if(afd->_etats[0]->getIndexTransitions(0)->getSource(i) == _etats[j]->getIndexTransitions(k)->getSource(0)) // Si on a trouv� la m�me source
                    {
                        if((char)('a'+h) == _etats[j]->getIndexTransitions(k)->getCaractere()) // Avec le m�me caract�re
                        {
                            if(first == true) // Mise � jour destination
                            {
                                afd->_etats[0]->getIndexTransitions(h)->setDestination(_etats[j]->getIndexTransitions(k)->getDestination(0));
                                first = false;
                            }
                            else // Cr�ation destination
                            {
                                afd->_etats[0]->getIndexTransitions(h)->setNbDestination(afd->_etats[0]->getIndexTransitions(h)->getNbDestinations()+1);
                                afd->_etats[0]->getIndexTransitions(h)->creerDestination(_etats[j]->getIndexTransitions(k)->getDestination(0));
                            }
                        }
                    }
                }
            }
        }
    }

    if(this->estSynchrone() == false) // si l'automate est asynchrone, il faut ajouter les transitions epsilons aux destinations
    {
        for(int i=0;i<afd->_etats[0]->getNbTransitions();i++)
        {
            int nbDestinationsInitiales = afd->_etats[0]->getIndexTransitions(i)->getNbDestinations();

            for(int j=0;j<afd->_etats[0]->getIndexTransitions(i)->getNbDestinations();j++)
            {
                for(int k=0;k<_nbEtats;k++)
                {
                    if(_etats[k]->getNbTransitions() > 0)
                    {
                        if(_etats[k]->getIndexTransitions(0)->getSource(0) == afd->_etats[0]->getIndexTransitions(i)->getDestination(j))
                        {
                            for(int l=0;l<_etats[k]->getNbTransitions();l++)
                            {
                                if(_etats[k]->getIndexTransitions(l)->getCaractere() == '*')
                                {
                                    afd->_etats[0]->getIndexTransitions(i)->creerDestination(_etats[k]->getIndexTransitions(l)->getDestination(0));
                                    afd->_etats[0]->getIndexTransitions(i)->setNbDestination(afd->_etats[0]->getIndexTransitions(i)->getNbDestinations()+1);
                                }
                            }
                        }
                    }
                }
            }
            // Trie des destinations dans l'ordre
            afd->_etats[0]->getIndexTransitions(i)->trierDestinations(nbDestinationsInitiales);
        }
    }

    if(afd->_etats[0]->getNbTransitions() > 0)
    {
        for(int l=0;l<afd->_etats[0]->getNbTransitions();l++) // Trie des destinations dans l'ordre
            afd->_etats[0]->getIndexTransitions(l)->trierDestinations(0);
    }

    // L'�tat initial est-il aussi terminal ?
    for(int i=0;i<_nbSorties;i++) // On parcours les sorties
    {
        if(afd->_etats[0]->getNbTransitions() > 0) // Si �tat avec transition(s)
        {
            for(int k=0;k<afd->_etats[0]->getIndexTransitions(0)->getNbSources();k++) // On parcours les sources
            {
                if(_sorties[i] == afd->_etats[0]->getIndexTransitions(0)->getSource(k)) // Si on trouve la source dans le tableau de sortie
                    afd->_etats[0]->setTerminal(true); // Mise � jour du statut de l'�tat
            }
        }
        else // Etat sans transition
        {
            if(_sorties[i] == afd->_etats[0]->getId()) // Comparaison avec l'id
                afd->_etats[0]->setTerminal(true);
        }
    }
    if(afd->_etats[0]->getTerminal() == true) // Si c'est une sortie, on met � jour le nombre de sorties
        afd->_nbSorties++;

    // Boucle de cr�ation des nouveaux �tats
    for(int i=0;i<afd->_nbEtats;i++) // On parcours les �tats
    {
        bool nouvelEtat(true); // Variable pour savoir si on ajoute ou non un �tat

        for(int j=0;j<afd->_etats[i]->getNbTransitions();j++) // On parcours les transitions
        {
            // On regarde si l'�tat est pr�sent dans notre liste
            bool etatPresent(false);
            for(int k=0;k<afd->_nbEtats;k++) // On parcours les �tats
            {
                if(afd->_etats[k]->getNbTransitions() > 0) // Si l'�tat a au moins une transition
                {
                    if(afd->_etats[k]->getIndexTransitions(0)->getNbSources() == afd->_etats[i]->getIndexTransitions(j)->getNbDestinations()) // Comparaison de la taille des sources
                    {
                        // Si elles sont �gales, on compare 1 � 1 les termes des sources
                        int cpt(0);
                        for(int m=0;m<afd->_etats[k]->getIndexTransitions(0)->getNbSources();m++)
                        {
                            if(afd->_etats[k]->getIndexTransitions(0)->getSource(m) == afd->_etats[i]->getIndexTransitions(j)->getDestination(m))
                            {
                                cpt++;
                            }
                        }

                        // Si le compteur est �gale au nombre de destinations, l'�tat est d�j� pr�sent
                        if(cpt == afd->_etats[i]->getIndexTransitions(j)->getNbDestinations())
                            etatPresent = true;
                    }
                }
                else // Sinon si l'�tat n'a pas de transition
                {
                    if(afd->_etats[i]->getIndexTransitions(j)->getNbDestinations() == 1)
                    {
                        if(afd->_etats[k]->getId() == afd->_etats[i]->getIndexTransitions(j)->getDestination(0)) // On compare avec l'id
                        {
                            etatPresent = true;
                        }
                    }
                }
            }

            // Si l'�tat est pr�sent, pas besoin d'en cr�er un nouveau
            if(etatPresent == true)
                nouvelEtat = false;
            else
                nouvelEtat = true;

            // R�servation d'un nouvel �tat
            if(nouvelEtat == true)
            {
                // Initialisation
                afd->_etats.push_back(new Etat);
                afd->_etats[afd->_nbEtats]->setId(afd->_etats[i]->getIndexTransitions(j)->getDestination(0));
                afd->_nbEtats++;

                // Tmp : tableau de transitions du nouvel �tat
                vector<Transition> _tmp;
                for(int k=0;k<afd->_etats[i]->getIndexTransitions(j)->getNbDestinations();k++) // On parcours les destinations
                {
                    for(int l=0;l<_nbEtats;l++) // On parcours les �tats
                    {
                        if(l == afd->_etats[i]->getIndexTransitions(j)->getDestination(k)) // On trouve le bon �tat
                        {
                            // Sauvegarde des transitions de l'�tat l
                            for(int m=0;m<_etats[l]->getNbTransitions();m++)
                            {
                                _tmp.push_back(Transition(l, _etats[l]->getIndexTransitions(m)->getCaractere(), _etats[l]->getIndexTransitions(m)->getDestination(0)));
                            }
                        }
                    }
                }

                // R�servation des transitions (1 pour chaque caractere)
                for(int k=0;k<_nbSymboles;k++)
                {
                    for(unsigned int l=0;l<_tmp.size();l++)
                    {
                        if(_tmp[l].getCaractere() == (char)('a'+k)) // Pour chaque caract�re, on ne r�serve qu'une transition
                        {
                            afd->_etats[afd->_nbEtats-1]->setNbTransitions(afd->_etats[afd->_nbEtats-1]->getNbTransitions()+1);
                            afd->_etats[afd->_nbEtats-1]->creerTransition(-1, (char)('a'+k), -1);
                            l = _tmp.size();
                        }
                    }
                }

                // Mise � jour des sources du nouvel �tat
                for(int l=0;l<afd->_etats[afd->_nbEtats-1]->getNbTransitions();l++)
                {
                    afd->_etats[afd->_nbEtats-1]->getIndexTransitions(l)->setSource(afd->_etats[i]->getIndexTransitions(j)->getDestination(0));
                    afd->_etats[afd->_nbEtats-1]->getIndexTransitions(l)->setNbSource(afd->_etats[i]->getIndexTransitions(j)->getNbDestinations());

                    for(int k=1;k<afd->_etats[i]->getIndexTransitions(j)->getNbDestinations();k++)
                    {
                        afd->_etats[afd->_nbEtats-1]->getIndexTransitions(l)->creerSource(afd->_etats[i]->getIndexTransitions(j)->getDestination(k));
                    }
                }

                // Mise � jour des destinations du nouvel �tat
                for(unsigned int k=0;k<_tmp.size();k++)
                {
                    for(int l=0;l<afd->_etats[afd->_nbEtats-1]->getNbTransitions();l++)
                    {
                        if(_tmp[k].getCaractere() == afd->_etats[afd->_nbEtats-1]->getIndexTransitions(l)->getCaractere())
                        {
                            // Premi�re it�ration, destination d�j� cr��e
                            if(afd->_etats[afd->_nbEtats-1]->getIndexTransitions(l)->getDestination(0) == -1)
                            {
                                afd->_etats[afd->_nbEtats-1]->getIndexTransitions(l)->setDestination(_tmp[k].getDestination(0));
                            }
                            // Sinon on cr�e une destination
                            else if(afd->_etats[afd->_nbEtats-1]->destinationPresente(l, _tmp[k].getDestination(0)) == false)
                            {
                                afd->_etats[afd->_nbEtats-1]->getIndexTransitions(l)->setNbDestination(afd->_etats[afd->_nbEtats-1]->getIndexTransitions(l)->getNbDestinations()+1);
                                afd->_etats[afd->_nbEtats-1]->getIndexTransitions(l)->creerDestination(_tmp[k].getDestination(0));
                            }
                        }
                        // Trie des destinations
                        afd->_etats[afd->_nbEtats-1]->getIndexTransitions(l)->trierDestinations(0);
                    }
                }

                // Si l'automate est asynchrone, il faut aussi prendre les transitions epsilons
                if(this->estSynchrone() == false)
                {
                    for(int l=0;l<afd->_etats[afd->_nbEtats-1]->getNbTransitions();l++) // On parcours les transitions
                    {
                        int nbDestinationsInitiales = afd->_etats[afd->_nbEtats-1]->getIndexTransitions(l)->getNbDestinations();

                        for(int k=0;k<afd->_etats[afd->_nbEtats-1]->getIndexTransitions(l)->getNbDestinations();k++) // On parcours les destinations
                        {
                            for(int m=0;m<_nbEtats;m++) // On parcours les �tats de l'AF
                            {
                                for(int n=0;n<_etats[m]->getNbTransitions();n++) // On parcours les transitions de l'AF
                                {
                                    if(_etats[m]->getIndexTransitions(n)->getSource(0) == afd->_etats[afd->_nbEtats-1]->getIndexTransitions(l)->getDestination(k)) // Si la source est �gale � la destination
                                    {
                                        if(_etats[m]->getIndexTransitions(n)->getCaractere() == '*') // Et que le caract�re est epsilon
                                        {
                                            if(afd->_etats[afd->_nbEtats-1]->getIndexTransitions(l)->destinationPresente(_etats[m]->getIndexTransitions(n)->getDestination(0)) == false) // On v�rifie que la destination n'est pas d�j� pr�sente
                                            {
                                                // Sinon on l'ajoute
                                                afd->_etats[afd->_nbEtats-1]->getIndexTransitions(l)->setNbDestination(afd->_etats[afd->_nbEtats-1]->getIndexTransitions(l)->getNbDestinations()+1);
                                                afd->_etats[afd->_nbEtats-1]->getIndexTransitions(l)->creerDestination(_etats[m]->getIndexTransitions(n)->getDestination(0));
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        // Trie des destinations dans l'ordre
                        afd->_etats[afd->_nbEtats-1]->getIndexTransitions(l)->trierDestinations(nbDestinationsInitiales);
                    }
                }

                // Le nouvel �tat est-il terminal ?
                for(int l=0;l<_nbSorties;l++) // On parcours les sorties
                {
                    if(afd->_etats[afd->_nbEtats-1]->getNbTransitions() > 0) // Etat avec transition(s)
                    {
                        for(int k=0;k<afd->_etats[afd->_nbEtats-1]->getIndexTransitions(0)->getNbSources();k++) // Que l'on compare aux sources
                        {
                            if(_sorties[l] == afd->_etats[afd->_nbEtats-1]->getIndexTransitions(0)->getSource(k)) // Si elles sont �gales, l'�tat est terminal
                            {
                                afd->_etats[afd->_nbEtats-1]->setTerminal(true);
                                k = afd->_etats[afd->_nbEtats-1]->getIndexTransitions(0)->getNbSources();
                                l = _nbSorties;
                            }
                        }
                    }
                    else // Etat sans transition
                    {
                        if(_sorties[l] == afd->_etats[afd->_nbEtats-1]->getId()) // Comparaison avec l'id
                            afd->_etats[afd->_nbEtats-1]->setTerminal(true);
                    }
                }
                if(afd->_etats[afd->_nbEtats-1]->getTerminal() == true) // Mise � jour du nombre de sorties
                    afd->_nbSorties++;
            }
        }
    }

    afd = afd->completion(); // Une fois d�terminis�, on compl�te l'afd

    return afd;
}

Automate * Automate::completion()
{
    bool completer(false);
    for(int i=0;i<_nbEtats;i++) // On parcours les �tats pour trouver si une transition manque
    {
        // Trouver quelle(s) transition(s) n'existe pas
        int cpt(1);
        for(int j=0;j<_etats[i]->getNbTransitions();j++)
        {
            // Compter nombre de symboles
            char caracterePrecedent;
            caracterePrecedent = _etats[i]->getIndexTransitions(j)->getCaractere();

            if(j+1 < _etats[i]->getNbTransitions() && caracterePrecedent != _etats[i]->getIndexTransitions(j+1)->getCaractere())
                cpt++;
        }

        // Etat � compl�ter
        if(cpt != _nbSymboles || _etats[i]->getNbTransitions() == 0)
        {
            completer = true;
            for(int k=0;k<_nbSymboles;k++) // On parcours les symboles
            {
                bool transitionPresente(false);
                if(_etats[i]->getNbTransitions() > 0) // Etat avec transitions
                {
                    for(int j=0;j<_etats[i]->getNbTransitions();j++) // On parcours les transitions
                    {
                        if(_etats[i]->getIndexTransitions(j)->getCaractere() == (char)('a'+k)) // Si on trouve le caract�re, pas desoin de compl�ter
                        {
                            transitionPresente = true;
                        }
                    }

                    if(transitionPresente == false) // Si on ne l'a pas trouv�, on compl�te
                    {
                        // Manque transition 'a'+k
                        // Ajout de la transition � la fin du tableau _transitions
                        _etats[i]->setNbTransitions(_etats[i]->getNbTransitions()+1);
                        _etats[i]->creerTransition(_etats[i]->getIndexTransitions(0)->getSource(0), (char)('a'+k), -1); // Cr�er transition avec la m�me source
                        for(int l=1;l<_etats[i]->getIndexTransitions(0)->getNbSources();l++)
                            _etats[i]->getIndexTransitions(_etats[i]->getNbTransitions()-1)->creerSource(_etats[i]->getIndexTransitions(0)->getSource(l));
                        // Mise � jour nbSources
                        _etats[i]->getIndexTransitions(_etats[i]->getNbTransitions()-1)->setNbSource(_etats[i]->getIndexTransitions(0)->getNbSources());
                    }
                }
                else // Si l'�tat n'a pas de transition, on le compl�te
                {
                    // Cr�er transition avec �tat poubelle
                     _etats[i]->creerTransition(_etats[i]->getId(), (char)('a'+k), -1);
                }
            }
            _etats[i]->setNbTransitions(_etats[i]->getTailleTransitions()); // Mise � jour du nombre de transitions
        }
    }

    if(completer == true) // Si on doit compl�ter l'automate
    {
        // Trie du tableau
        for(int i=0;i<_nbEtats;i++)
            sort(_etats[i]->getIndexTransitions(0), _etats[i]->getIndexTransitions(_etats[i]->getNbTransitions()), cmd);
        for(int i=0;i<_nbEtats;i++)
            sort(_etats[i]->getIndexTransitions(0), _etats[i]->getIndexTransitions(_etats[i]->getNbTransitions()), cmd2);
        for(int i=0;i<_nbEtats;i++)
            sort(_etats[i]->getIndexTransitions(0), _etats[i]->getIndexTransitions(_etats[i]->getNbTransitions()), cmd3);

        // Ajout de l'�tat poubelle
        _etats.push_back(new Etat);
        _etats[_nbEtats]->setId(_nbEtats);
        _etats[_nbEtats]->setNbTransitions(_nbSymboles);
        _nbEtats++;
        _poubelle = true;

        // Transitions de la poubelle
        for(int i=0;i<_etats[_nbEtats-1]->getNbTransitions();i++)
        {
            _etats[_nbEtats-1]->creerTransition(-1, (char)('a'+i), -1);
        }
    }

    return this;
}

Automate * Automate::minimisation()
{
    // Partition initiale (Tableau de chaines de caract�res : Terminal (T) ou Non terminal (N))
    vector<string> partitionInitiale(_nbEtats);
    // Mise � jour de la partition initiale
    for(int i=0;i<_nbEtats;i++)
    {
        partitionInitiale[i] = 'N';

        if(_etats[i]->getTerminal() == true) // Il suffit de regarder s'il est terminal
            partitionInitiale[i] = 'T';
    }

    // Affichage partition initiale
    cout << "O0 :" << endl;
    for(int i=0;i<_nbEtats;i++)
    {
        cout << partitionInitiale[i] << endl;
    }

    bool end(false); // Condition d'arr�t de la minimisation
    int numeroPartition(1); // Permet de diff�rencier les d�coupages (N1 / N2)

    // Tableau des partitions interm�diaires
    int const tailleX(_nbSymboles);
    int const tailleY(_nbEtats);
    string partitionIntermediaire[tailleX][tailleY];

    // Nouvelle partition
    vector<string> nouvellePartition(_nbEtats);

    // Compte le nombre de signletons
    int nbSingletons(0);

    do
    {
        // Initialisation partition interm�diaire
        for(int i=0;i<_nbEtats;i++) // On parcours les �tats
        {
            for(int j=0;j<_nbSymboles;j++) // On parcours les symboles
            {
                for(int n=0;n<_nbEtats;n++) // On parcours les �tats de l'AFDC
                {
                    if(_etats[i]->getIndexTransitions(j)->getNbDestinations() == _etats[n]->getIndexTransitions(0)->getNbSources()) // On cherche la source
                    {
                        // On trouve le num�ro de la destination
                        for(int m=0;m<_etats[i]->getIndexTransitions(j)->getNbDestinations();m++)
                        {
                            if(_etats[i]->getIndexTransitions(j)->getDestination(m) == _etats[n]->getIndexTransitions(0)->getSource(m)) // Mise � jour du tableau interm�diaire
                            {
                                partitionIntermediaire[j][i] = (string)partitionInitiale[n];
                                m = _etats[i]->getIndexTransitions(j)->getNbDestinations();
                                n = _nbEtats;
                            }
                        }
                    }
                }
            }
        }

        // D�finition de la nouvelle partition
        for(int i=0;i<_nbEtats;i++)
        {
            string etat = partitionInitiale[i];
            int nbOccurencesEtat(1);
            int singleton(0);

            for(int j=0;j<_nbEtats;j++) // On parcours les �tats
            {
                if(etat == partitionInitiale[j] && i != j)
                {
                    nbOccurencesEtat++;

                    // Comparaison 1 � 1
                    int cpt(0);
                    for(int k=0;k<_nbSymboles;k++)
                    {
                        if(partitionIntermediaire[k][i] == partitionIntermediaire[k][j])
                            cpt++;
                    }

                    if(cpt == _nbSymboles) // Etats �gaux, pas besoin de les diff�rencier
                        nouvellePartition[i] = partitionInitiale[i];
                    else // Diff�renciation des �tats
                    {
                        if(i < j)
                        {
                            nouvellePartition[i] = partitionInitiale[i];
                            ostringstream oss;
                            oss << nbOccurencesEtat - 1;
                            nouvellePartition[i] += oss.str();
                        }
                        else
                        {
                            nouvellePartition[i] = partitionInitiale[i];
                            ostringstream oss;
                            oss << nbOccurencesEtat;
                            nouvellePartition[i] += oss.str();
                        }
                    }
                }
                else
                {
                    singleton++;
                }
            }

            if(singleton == _nbEtats)
            {
                nouvellePartition[i] = partitionInitiale[i];
                nbSingletons++;
            }
        }

        // Affichage partitionInterm�diaire
        cout << endl;
        for(int i=0;i<_nbSymboles;i++)
            cout << (char)('a'+i) << "\t";
        cout << "O" << numeroPartition;
        cout << endl;
        for(int i=0;i<_nbEtats;i++)
        {
            for(int j=0;j<_nbSymboles;j++)
            {
                cout << (string)partitionIntermediaire[j][i] << "\t";
            }
            cout << nouvellePartition[i] << endl;
        }

        //Comparaison nouvelle partition avec la partition initiale
        int cpt(0);
        for(int i=0;i<_nbEtats;i++)
        {
            if(partitionInitiale[i] == nouvellePartition[i])
                cpt++;
        }
        // Conditions d'arr�t
        if(cpt == _nbEtats)
            end = true;
        if(nbSingletons == _nbEtats)
            end = true;

        // Mise � jour partition Interm�diaire pour le tableau de transition final
        if(end == true)
        {
            // M�me m�thode pr�c�demment utilis�e
            for(int i=0;i<_nbEtats;i++)
            {
                for(int j=0;j<_nbSymboles;j++)
                {
                    for(int n=0;n<_nbEtats;n++) // On parcours les �tats de l'AFDC
                    {
                        if(_etats[i]->getIndexTransitions(j)->getNbDestinations() == _etats[n]->getIndexTransitions(0)->getNbSources())
                        {
                            // Trouver num�ro de la destination
                            for(int m=0;m<_etats[i]->getIndexTransitions(j)->getNbDestinations();m++)
                            {
                                if(_etats[i]->getIndexTransitions(j)->getDestination(m) == _etats[n]->getIndexTransitions(0)->getSource(m))
                                {
                                    partitionIntermediaire[j][i] = (string)partitionInitiale[n];
                                    m = _etats[i]->getIndexTransitions(j)->getNbDestinations();
                                    n = _nbEtats;
                                }
                            }
                        }
                    }
                }
            }
        }

        // Nouvelle partition devient partition initiale
        for(int i=0;i<_nbEtats;i++)
            partitionInitiale[i] = nouvellePartition[i];

        numeroPartition++;
    } while(end == false);

    // Affichage partitionInterm�diaire
    cout << endl;
    for(int i=0;i<_nbSymboles;i++)
        cout << (char)('a'+i) << "\t";
    cout << "O" << numeroPartition - 1;
    cout << endl;
    for(int i=0;i<_nbEtats;i++)
    {
        for(int j=0;j<_nbSymboles;j++)
        {
            cout << (string)partitionIntermediaire[j][i] << "\t";
        }
        cout << partitionInitiale[i] << endl;
    }

    // Enlever les �tats en double pour faire afdcm
    if(nbSingletons != _nbEtats)
    {
        for(unsigned int i=0;i<partitionInitiale.size();i++)
        {
            for(unsigned int j=0;j<partitionInitiale.size();j++)
            {
                if(i != j)
                {
                    if(partitionInitiale[i] == partitionInitiale[j])
                    {
                        partitionInitiale.erase(partitionInitiale.begin()+i);
                    }
                }
            }
        }
    }

    // Construction de l'AFDCM
    Automate * afdcm = new Automate;

    // Initialisation
    afdcm->_nbEntrees = _nbEntrees;
    afdcm->_nbSorties = 0;
    afdcm->_nbSymboles = _nbSymboles;
    afdcm->_nbEtats = partitionInitiale.size();

    for(unsigned int i=0;i<partitionInitiale.size();i++)
    {
        afdcm->_etats.push_back(new Etat);
        if(i == 0)
            afdcm->_etats[i]->setInitial(true);
        afdcm->_etats[i]->setId(i);
        afdcm->_etats[i]->setNbTransitions(_nbSymboles);
    }

    // Remplir les sources
    for(int i=0;i<afdcm->_nbEtats;i++)
    {
        for(int j=0;j<afdcm->_nbSymboles;j++)
        {
            afdcm->_etats[i]->creerTransition(i, (char)('a'+j), 0); // i correspond � la i�me case de partitionInitiale (ex: 0 = N1)
        }
    }

    // Remplir les destinations
    for(int i=0;i<_nbEtats;i++) // On parcours les �tats
    {
        for(int j=0;j<_nbSymboles;j++) // On parcours les symboles
        {
            // i = index source
            int indexEtatARemplir(0);
            for(int k=0;k<afdcm->_nbEtats;k++)
            {
                if(nouvellePartition[i] == partitionInitiale[k])
                {
                    indexEtatARemplir = k;
                    k = afdcm->_nbEtats;
                }
            }

            // Parcourir l'AFDC
            for(int n=0;n<_nbEtats;n++)
            {
                if(_etats[i]->getIndexTransitions(j)->getNbDestinations() == _etats[n]->getIndexTransitions(0)->getNbSources())
                {
                    // Trouver num�ro de la destination
                    for(int m=0;m<_etats[i]->getIndexTransitions(j)->getNbDestinations();m++)
                    {
                        if(_etats[i]->getIndexTransitions(j)->getDestination(m) == _etats[n]->getIndexTransitions(0)->getSource(m))
                        {
                            int indexDestinationARemplir(0);
                            for(int k=0;k<afdcm->_nbEtats;k++)
                            {
                                if(nouvellePartition[n] == partitionInitiale[k])
                                {
                                    indexDestinationARemplir = k;
                                    k = afdcm->_nbEtats;
                                }
                            }
                            afdcm->_etats[indexEtatARemplir]->getIndexTransitions(j)->setDestination(indexDestinationARemplir);
                        }
                    }
                }
            }
        }
    }

    // Mise � jour des sorties de l'AFDCM
    for(int i=0;i<_nbEtats;i++)
    {
        if(_etats[i]->getTerminal() == true)
        {
            for(int j=0;j<afdcm->_nbEtats;j++)
            {
                if(nouvellePartition[i] == partitionInitiale[j])
                {
                    afdcm->_etats[j]->setTerminal(true);
                }
            }
        }
    }

    cout << endl << "Automate fini deterministe complet minimise" << endl;
    afdcm->afficher();

    // Affichage des �quivalents
    cout << "Avec :" << endl;
    for(int i=0;i<afdcm->_nbEtats;i++)
    {
        cout << "\t- " << i << " = " << partitionInitiale[i] << endl;
    }

    return this;
}

bool Automate::test(string mot)
{
    bool motReconnu(true);

    // V�rification du mot vide (*)
    if(mot == "*")
    {
        for(int i=0;i<_nbEtats;i++)
        {
            if(_etats[i]->getInitial() == true && _etats[i]->getTerminal() == true) // Si une entr�e est une sortie, c'est bon
            {
                motReconnu = true;
            }
        }
    }
    else // Mot avec des symboles
    {
        for(int j=0;j<_nbEtats;j++) // On parcours les �tats
        {
            if(_etats[j]->getInitial() == true) // On part d'une entr�e
            {
                for(unsigned int i=0;i<mot.size();i++) // S�parer le mot lettre par lettre
                {
                    int cpt2(0);
                    for(int k=0;k<_etats[j]->getNbTransitions();k++) // On parcours les transitions
                    {
                        // Si l'�tat poss�de une transition avec ce caract�re, on passe � l'�tat suivant
                        if(_etats[j]->getIndexTransitions(k)->getCaractere() == mot[i])
                        {
                            // Pour cela, il faut trouver il'ndex de la destination
                            int index(0);

                            for(int l=0;l<_nbEtats;l++) // On parcours les �tats
                            {
                                int cpt(0);
                                if(_etats[l]->getNbTransitions() > 0)
                                {
                                    if(_etats[l]->getIndexTransitions(0)->getNbSources() == _etats[j]->getIndexTransitions(k)->getNbDestinations()) // On trouve la m�me source
                                    {
                                        for(int m=0;m<_etats[j]->getIndexTransitions(k)->getNbDestinations();m++) // Comparaison 1 � 1 de la source
                                        {
                                            if(_etats[j]->getIndexTransitions(k)->getDestination(m) == _etats[l]->getIndexTransitions(0)->getSource(m))
                                                cpt++;
                                        }
                                    }
                                }

                                if(cpt == _etats[j]->getIndexTransitions(k)->getNbDestinations()) // Si j'ai trouv� la source, on a l'index du prochain �tat : l
                                {
                                    index = l;
                                    l = _nbEtats;
                                }
                            }
                            k = _etats[j]->getNbTransitions();
                            j = index; // On passe donc � l'�tat suivant
                        }
                        else
                        {
                            cpt2++;
                        }
                    }
                    if(cpt2 == _etats[j]->getNbTransitions())
                    {
                        motReconnu = false;
                    }
                }
                // Il faut que le dernier �tat soit forc�ment une sortie
                if(_etats[j]->getTerminal() == false)
                    motReconnu = false;
            }

            if(motReconnu == true) // Si le mot a �t� reconnu, pas besoin de tester pour les autres entr�es
                j = _nbEtats;
        }
    }

    return motReconnu; // Retourne vrai ou faux si le mot a �t� reconnu ou non
}
