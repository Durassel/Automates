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
// Selon le caractère
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

// Chargement des données à la création de l'automate
Automate::Automate(string file)
{
    // Ouverture en mode lecture
    ifstream flux(file.c_str(), ios::in);

    // Chargement des données
    flux >> _nbSymboles >> _nbEtats;

    // Réservation des états
    for(int i=0;i<_nbEtats;i++)
    {
		_etats.push_back(new Etat);
		_etats[i]->setId(i);
	}

    // Entrée(s) : tableau d'entiers
    flux >> _nbEntrees;
    if(_nbEntrees > 0) // Test si aucune entrée
    {
        for(int i=0;i<_nbEntrees;i++)
        {
            _entrees.push_back(0);
            flux >> _entrees[i];
            _entrees[i]--;
        }
        // Trie des entrées
        sort(_entrees.begin(), _entrees.end(), trier);
    }

    // Initialisation des états initiaux
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
        // Trie des entrées
        sort(_sorties.begin(), _sorties.end(), trier);
    }

    // Initialisation des états terminaux
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

    // Tmp : tableau temporaire de toute les transitions qui sera ordonné dans _transitions
    vector<Transition> _tmp;
    for(int i=0;i<nbTransitions;i++)
    {
        int a;
        char b;
        int c;
        flux >> a >> b >> c;
        if(b == '*')
            b = '{'; // Pour que la transition epsilon soit trier après le dernier symbole
        _tmp.push_back(Transition(a-1, b, c-1)); // -1 car le sujet impose état à partir de 0
    }

    // Trie du tableau
    sort(_tmp.begin(), _tmp.end(), cmd);
    sort(_tmp.begin(), _tmp.end(), cmd2);
    sort(_tmp.begin(), _tmp.end(), cmd3);

    // Une fois trié, on peut transformer les { en *
    for(int i=0;i<nbTransitions;i++)
    {
        if(_tmp[i].getCaractere() == '{')
            _tmp[i].setCaractere('*');
    }

    // Pour chaque état, réserver les transitions et le nombre de transitions
    int cpt(0);
    for(int i=0;i<_nbEtats;i++)
    {
        cpt = 0;
        // Réservation du nombre voulu de transitions
        for(int j=0;j<nbTransitions;j++) // = _tmp.size()
        {
            if(i == _tmp[j].getSource(0))
            {
                _etats[i]->creerTransition(_tmp[j].getSource(0), _tmp[j].getCaractere(), _tmp[j].getDestination(0)); // Création de la transition
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

// Méthodes
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
    // Automate non déterministe si :
    //  - plusieurs entrées
    if(_nbEntrees > 1)
        return false;

    //  - plusieurs transitions sortantes de même caractère d'un état
    for(int i=0;i<_nbEtats;i++) // On parcours tout les états
    {
        for(int j=0;j<_etats[i]->getNbTransitions();j++) // On parcours toutes les transitions
        {
            // Puisque le tableau est trié, on regarde si la transition suivante a le même caractère
            char caracterePrecedent;
            caracterePrecedent = _etats[i]->getIndexTransitions(j)->getCaractere();

            // Si oui, l'automate n'est pas déterministe
            if(j+1 < _etats[i]->getNbTransitions() && caracterePrecedent == _etats[i]->getIndexTransitions(j+1)->getCaractere())
                return false;
        }
    }
    return true;
}

bool Automate::estComplet()
{
    // Complet: 1 état = nb symboles de transition
    for(int i=0;i<_nbEtats;i++) // On parcours tous les états
    {
        if(_etats[i]->getNbTransitions() != _nbSymboles) // Si le nombre de transitions de l'état est différent du nombre de symboles, il n'est pas complet
            return false;
    }
    return true;
}

void Automate::afficher()
{
    // Affichage complexe car il prend en compte les espaces, les virgules à afficher ...

    // On recherche la source qui est la plus grande à afficher pour calculer les espaces
    int nbChiffresMax(0);
    for(int j=0;j<_nbEtats;j++) // On parcours tous les états
    {
        int nbChiffres(0);
        if(_etats[j]->getNbTransitions() == 0) // Si l'état n'a pas de transition, on affichera uniquement l'id (un ou deux chiffre(s) maximum)
            nbChiffresMax = 2;
        else // Sinon
        {
            // On compte le nombre total de chiffres de la source
            for(int k=0;k<_etats[j]->getIndexTransitions(0)->getNbSources();k++) // On regarde la première transition car toutes les transitions ont la même source
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

    // Calcul des espaces nécessaires (4 chiffres correspond environ à un \t
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
        // Espaces après le symbole
        // Même méthode que celle présentée précédemment sauf que l'on regarde les destinations et non les sources
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

        // Affichage des espaces nécessaires
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

    // Si l'automate est synchrone, on affiche * (représente les transitions epsilons)
    if(this->estSynchrone() == false)
        cout << "*";
    cout << endl;

    int nbChiffresMaxSources(0); // Sert pour les espaces

    // On parcours tous les états à afficher
    for(int j=0;j<_nbEtats;j++)
    {
        // Affichage entrée/sortie/poubelle
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
            cout << "\t"; // Séparation entre entrées/sorties et les états

            // Affichage des sources (avec / sans virgule(s))
            if(_etats[j]->getNbTransitions() == 0) // Affichage id si l'état n'a pas de transition
                cout << _etats[j]->getId();
            else
            {
                // Affichage de la source + calcul des espaces nécessaires (même méthode que précédemment)
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

        // Calcul espaces nécessaires
        int nbEspacesMax(0);
        if(nbChiffresMaxSources % 4 == 0)
            nbEspacesMax = nbChiffresMaxSources / 4;
        else
            nbEspacesMax = (nbChiffresMaxSources / 4) + 1;

        // 4 chiffres correspond environ à un \t
        nbEspaces = (nbChiffres-1) / 4;

        // Affichage des espaces après la source
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

        // Regarder la première transition (si 'a' : aucun problème sinon afficher plus d'espaces pour aligner correctement les destinations)
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
                    for(int k=0;k<_nbEtats;k++) // Calcul des espaces (même méthode que précédemment)
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

                // Affichage des espaces nécessaires
                for(int l=0;l<nbEspacesTotal;l++)
                    cout << "\t";

                if(_etats[j]->getIndexTransitions(0)->getCaractere() == '*')
                    k = _nbSymboles;
            }
        }

        // Affichage des destinations
        for(int i=0;i<_etats[j]->getNbTransitions();i++) // On parcours les transitions
        {
            // Calcul des espaces nécessaires (même méthode)
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
                else // Pas d'espace nécessaire
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
    // Création du nouvel automate
    Automate * afd = new Automate;
    // Initialisation
    afd->_nbEntrees = 1;
    afd->_nbSorties = 0;
    afd->_nbSymboles = _nbSymboles;
    afd->_nbEtats = 1;

    // Initialisation du premier état de l'AFD (entrée)
    afd->_etats.push_back(new Etat);
    afd->_etats[0]->setInitial(true);
    afd->_etats[0]->setId(0);
    afd->_etats[0]->setNbTransitions(0);

    // Tmp : tableau temporaire de transitions des états initiaux
    vector<Transition> _tmp;
    for(int i=0;i<_nbEtats;i++) // On parcours les états
    {
        if(_etats[i]->getInitial() == true) // S'il est initial
        {
            for(int j=0;j<_etats[i]->getNbTransitions();j++) // On sauvegarde les transitions
            {
                _tmp.push_back(Transition(i, _etats[i]->getIndexTransitions(j)->getCaractere(), _etats[i]->getIndexTransitions(j)->getDestination(0)));
            }
        }
    }

    // Réservation des transitions (1 pour chaque caractere)
    for(int j=0;j<_nbSymboles;j++) // On parcours les symboles
    {
        for(unsigned int i=0;i<_tmp.size();i++) // On parcours les transitions
        {
            if(_tmp[i].getCaractere() == (char)('a'+j) || _tmp[i].getCaractere() == '*') // On réserve une transition pour chaque caractère (symbole)
            {
                afd->_etats[0]->setNbTransitions(afd->_etats[0]->getNbTransitions()+1); // Augmentation du nombre de transitions puisque l'on en crée une
                afd->_etats[0]->creerTransition(0, (char)('a'+j), 0); // Initialisation transition avec son caractère
                i = _tmp.size();
            }
        }
    }

    // On initialise les sources de l'état initial
    for(int j=0;j<_nbEtats;j++)
    {
        for(unsigned int i=0;i<_tmp.size();i++)
        {
            if(j == 0) // Lorsque l'on crée une transition, une source est automatiquement créée, on la met alors à jour
            {
                for(int k=0;k<afd->_etats[0]->getNbTransitions();k++)
                {
                    afd->_etats[0]->getIndexTransitions(k)->setSource(0+j);
                }
                i = _tmp.size();
            }
            else if(j != 0 && _tmp[i].getSource(0) == 0+j) // Si on a besoin d'une source avec virgule, on crée une nouvelle source (+ mise à jour du nombre de sources)
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

    // Si automate asynchrone, il faut ajouter les transitions epsilons à la source
    if(this->estSynchrone() == false)
    {
        // Les entrées ont-elles des transitions epsilons ?
        for(int i=0;i<afd->_etats[0]->getIndexTransitions(0)->getNbSources();i++)
        {
            for(int j=0;j<_nbEtats;j++) // On parcours les états
            {
                for(int k=0;k<_etats[j]->getNbTransitions();k++) // On parcours les transitions
                {
                    if(_etats[j]->getIndexTransitions(k)->getSource(0) == afd->_etats[0]->getIndexTransitions(0)->getSource(i)) // Si on a trouvé la source
                    {
                        if(_etats[j]->getIndexTransitions(k)->getCaractere() == '*') // Et que la transition est epsilon
                        {
                            // On vérifie avant que l'état n'est pas déjà présent dans la source
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

    // On initialise les destinations de l'état initial
    for(int h=0;h<afd->_nbSymboles;h++) // On parcours les symboles
    {
        bool first(true);
        for(int i=0;i<afd->_etats[0]->getIndexTransitions(0)->getNbSources();i++) // On parcours les sources
        {
            for(int j=0;j<_nbEtats;j++) // On parcours les états
            {
                for(int k=0;k<_etats[j]->getNbTransitions();k++) // On parcours chaque transition
                {
                    if(afd->_etats[0]->getIndexTransitions(0)->getSource(i) == _etats[j]->getIndexTransitions(k)->getSource(0)) // Si on a trouvé la même source
                    {
                        if((char)('a'+h) == _etats[j]->getIndexTransitions(k)->getCaractere()) // Avec le même caractère
                        {
                            if(first == true) // Mise à jour destination
                            {
                                afd->_etats[0]->getIndexTransitions(h)->setDestination(_etats[j]->getIndexTransitions(k)->getDestination(0));
                                first = false;
                            }
                            else // Création destination
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

    // L'état initial est-il aussi terminal ?
    for(int i=0;i<_nbSorties;i++) // On parcours les sorties
    {
        if(afd->_etats[0]->getNbTransitions() > 0) // Si état avec transition(s)
        {
            for(int k=0;k<afd->_etats[0]->getIndexTransitions(0)->getNbSources();k++) // On parcours les sources
            {
                if(_sorties[i] == afd->_etats[0]->getIndexTransitions(0)->getSource(k)) // Si on trouve la source dans le tableau de sortie
                    afd->_etats[0]->setTerminal(true); // Mise à jour du statut de l'état
            }
        }
        else // Etat sans transition
        {
            if(_sorties[i] == afd->_etats[0]->getId()) // Comparaison avec l'id
                afd->_etats[0]->setTerminal(true);
        }
    }
    if(afd->_etats[0]->getTerminal() == true) // Si c'est une sortie, on met à jour le nombre de sorties
        afd->_nbSorties++;

    // Boucle de création des nouveaux états
    for(int i=0;i<afd->_nbEtats;i++) // On parcours les états
    {
        bool nouvelEtat(true); // Variable pour savoir si on ajoute ou non un état

        for(int j=0;j<afd->_etats[i]->getNbTransitions();j++) // On parcours les transitions
        {
            // On regarde si l'état est présent dans notre liste
            bool etatPresent(false);
            for(int k=0;k<afd->_nbEtats;k++) // On parcours les états
            {
                if(afd->_etats[k]->getNbTransitions() > 0) // Si l'état a au moins une transition
                {
                    if(afd->_etats[k]->getIndexTransitions(0)->getNbSources() == afd->_etats[i]->getIndexTransitions(j)->getNbDestinations()) // Comparaison de la taille des sources
                    {
                        // Si elles sont égales, on compare 1 à 1 les termes des sources
                        int cpt(0);
                        for(int m=0;m<afd->_etats[k]->getIndexTransitions(0)->getNbSources();m++)
                        {
                            if(afd->_etats[k]->getIndexTransitions(0)->getSource(m) == afd->_etats[i]->getIndexTransitions(j)->getDestination(m))
                            {
                                cpt++;
                            }
                        }

                        // Si le compteur est égale au nombre de destinations, l'état est déjà présent
                        if(cpt == afd->_etats[i]->getIndexTransitions(j)->getNbDestinations())
                            etatPresent = true;
                    }
                }
                else // Sinon si l'état n'a pas de transition
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

            // Si l'état est présent, pas besoin d'en créer un nouveau
            if(etatPresent == true)
                nouvelEtat = false;
            else
                nouvelEtat = true;

            // Réservation d'un nouvel état
            if(nouvelEtat == true)
            {
                // Initialisation
                afd->_etats.push_back(new Etat);
                afd->_etats[afd->_nbEtats]->setId(afd->_etats[i]->getIndexTransitions(j)->getDestination(0));
                afd->_nbEtats++;

                // Tmp : tableau de transitions du nouvel état
                vector<Transition> _tmp;
                for(int k=0;k<afd->_etats[i]->getIndexTransitions(j)->getNbDestinations();k++) // On parcours les destinations
                {
                    for(int l=0;l<_nbEtats;l++) // On parcours les états
                    {
                        if(l == afd->_etats[i]->getIndexTransitions(j)->getDestination(k)) // On trouve le bon état
                        {
                            // Sauvegarde des transitions de l'état l
                            for(int m=0;m<_etats[l]->getNbTransitions();m++)
                            {
                                _tmp.push_back(Transition(l, _etats[l]->getIndexTransitions(m)->getCaractere(), _etats[l]->getIndexTransitions(m)->getDestination(0)));
                            }
                        }
                    }
                }

                // Réservation des transitions (1 pour chaque caractere)
                for(int k=0;k<_nbSymboles;k++)
                {
                    for(unsigned int l=0;l<_tmp.size();l++)
                    {
                        if(_tmp[l].getCaractere() == (char)('a'+k)) // Pour chaque caractère, on ne réserve qu'une transition
                        {
                            afd->_etats[afd->_nbEtats-1]->setNbTransitions(afd->_etats[afd->_nbEtats-1]->getNbTransitions()+1);
                            afd->_etats[afd->_nbEtats-1]->creerTransition(-1, (char)('a'+k), -1);
                            l = _tmp.size();
                        }
                    }
                }

                // Mise à jour des sources du nouvel état
                for(int l=0;l<afd->_etats[afd->_nbEtats-1]->getNbTransitions();l++)
                {
                    afd->_etats[afd->_nbEtats-1]->getIndexTransitions(l)->setSource(afd->_etats[i]->getIndexTransitions(j)->getDestination(0));
                    afd->_etats[afd->_nbEtats-1]->getIndexTransitions(l)->setNbSource(afd->_etats[i]->getIndexTransitions(j)->getNbDestinations());

                    for(int k=1;k<afd->_etats[i]->getIndexTransitions(j)->getNbDestinations();k++)
                    {
                        afd->_etats[afd->_nbEtats-1]->getIndexTransitions(l)->creerSource(afd->_etats[i]->getIndexTransitions(j)->getDestination(k));
                    }
                }

                // Mise à jour des destinations du nouvel état
                for(unsigned int k=0;k<_tmp.size();k++)
                {
                    for(int l=0;l<afd->_etats[afd->_nbEtats-1]->getNbTransitions();l++)
                    {
                        if(_tmp[k].getCaractere() == afd->_etats[afd->_nbEtats-1]->getIndexTransitions(l)->getCaractere())
                        {
                            // Première itération, destination déjà créée
                            if(afd->_etats[afd->_nbEtats-1]->getIndexTransitions(l)->getDestination(0) == -1)
                            {
                                afd->_etats[afd->_nbEtats-1]->getIndexTransitions(l)->setDestination(_tmp[k].getDestination(0));
                            }
                            // Sinon on crée une destination
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
                            for(int m=0;m<_nbEtats;m++) // On parcours les états de l'AF
                            {
                                for(int n=0;n<_etats[m]->getNbTransitions();n++) // On parcours les transitions de l'AF
                                {
                                    if(_etats[m]->getIndexTransitions(n)->getSource(0) == afd->_etats[afd->_nbEtats-1]->getIndexTransitions(l)->getDestination(k)) // Si la source est égale à la destination
                                    {
                                        if(_etats[m]->getIndexTransitions(n)->getCaractere() == '*') // Et que le caractère est epsilon
                                        {
                                            if(afd->_etats[afd->_nbEtats-1]->getIndexTransitions(l)->destinationPresente(_etats[m]->getIndexTransitions(n)->getDestination(0)) == false) // On vérifie que la destination n'est pas déjà présente
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

                // Le nouvel état est-il terminal ?
                for(int l=0;l<_nbSorties;l++) // On parcours les sorties
                {
                    if(afd->_etats[afd->_nbEtats-1]->getNbTransitions() > 0) // Etat avec transition(s)
                    {
                        for(int k=0;k<afd->_etats[afd->_nbEtats-1]->getIndexTransitions(0)->getNbSources();k++) // Que l'on compare aux sources
                        {
                            if(_sorties[l] == afd->_etats[afd->_nbEtats-1]->getIndexTransitions(0)->getSource(k)) // Si elles sont égales, l'état est terminal
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
                if(afd->_etats[afd->_nbEtats-1]->getTerminal() == true) // Mise à jour du nombre de sorties
                    afd->_nbSorties++;
            }
        }
    }

    afd = afd->completion(); // Une fois déterminisé, on complète l'afd

    return afd;
}

Automate * Automate::completion()
{
    bool completer(false);
    for(int i=0;i<_nbEtats;i++) // On parcours les états pour trouver si une transition manque
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

        // Etat à compléter
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
                        if(_etats[i]->getIndexTransitions(j)->getCaractere() == (char)('a'+k)) // Si on trouve le caractère, pas desoin de compléter
                        {
                            transitionPresente = true;
                        }
                    }

                    if(transitionPresente == false) // Si on ne l'a pas trouvé, on complète
                    {
                        // Manque transition 'a'+k
                        // Ajout de la transition à la fin du tableau _transitions
                        _etats[i]->setNbTransitions(_etats[i]->getNbTransitions()+1);
                        _etats[i]->creerTransition(_etats[i]->getIndexTransitions(0)->getSource(0), (char)('a'+k), -1); // Créer transition avec la même source
                        for(int l=1;l<_etats[i]->getIndexTransitions(0)->getNbSources();l++)
                            _etats[i]->getIndexTransitions(_etats[i]->getNbTransitions()-1)->creerSource(_etats[i]->getIndexTransitions(0)->getSource(l));
                        // Mise à jour nbSources
                        _etats[i]->getIndexTransitions(_etats[i]->getNbTransitions()-1)->setNbSource(_etats[i]->getIndexTransitions(0)->getNbSources());
                    }
                }
                else // Si l'état n'a pas de transition, on le complète
                {
                    // Créer transition avec état poubelle
                     _etats[i]->creerTransition(_etats[i]->getId(), (char)('a'+k), -1);
                }
            }
            _etats[i]->setNbTransitions(_etats[i]->getTailleTransitions()); // Mise à jour du nombre de transitions
        }
    }

    if(completer == true) // Si on doit compléter l'automate
    {
        // Trie du tableau
        for(int i=0;i<_nbEtats;i++)
            sort(_etats[i]->getIndexTransitions(0), _etats[i]->getIndexTransitions(_etats[i]->getNbTransitions()), cmd);
        for(int i=0;i<_nbEtats;i++)
            sort(_etats[i]->getIndexTransitions(0), _etats[i]->getIndexTransitions(_etats[i]->getNbTransitions()), cmd2);
        for(int i=0;i<_nbEtats;i++)
            sort(_etats[i]->getIndexTransitions(0), _etats[i]->getIndexTransitions(_etats[i]->getNbTransitions()), cmd3);

        // Ajout de l'état poubelle
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
    // Partition initiale (Tableau de chaines de caractères : Terminal (T) ou Non terminal (N))
    vector<string> partitionInitiale(_nbEtats);
    // Mise à jour de la partition initiale
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

    bool end(false); // Condition d'arrêt de la minimisation
    int numeroPartition(1); // Permet de différencier les découpages (N1 / N2)

    // Tableau des partitions intermédiaires
    int const tailleX(_nbSymboles);
    int const tailleY(_nbEtats);
    string partitionIntermediaire[tailleX][tailleY];

    // Nouvelle partition
    vector<string> nouvellePartition(_nbEtats);

    // Compte le nombre de signletons
    int nbSingletons(0);

    do
    {
        // Initialisation partition intermédiaire
        for(int i=0;i<_nbEtats;i++) // On parcours les états
        {
            for(int j=0;j<_nbSymboles;j++) // On parcours les symboles
            {
                for(int n=0;n<_nbEtats;n++) // On parcours les états de l'AFDC
                {
                    if(_etats[i]->getIndexTransitions(j)->getNbDestinations() == _etats[n]->getIndexTransitions(0)->getNbSources()) // On cherche la source
                    {
                        // On trouve le numéro de la destination
                        for(int m=0;m<_etats[i]->getIndexTransitions(j)->getNbDestinations();m++)
                        {
                            if(_etats[i]->getIndexTransitions(j)->getDestination(m) == _etats[n]->getIndexTransitions(0)->getSource(m)) // Mise à jour du tableau intermédiaire
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

        // Définition de la nouvelle partition
        for(int i=0;i<_nbEtats;i++)
        {
            string etat = partitionInitiale[i];
            int nbOccurencesEtat(1);
            int singleton(0);

            for(int j=0;j<_nbEtats;j++) // On parcours les états
            {
                if(etat == partitionInitiale[j] && i != j)
                {
                    nbOccurencesEtat++;

                    // Comparaison 1 à 1
                    int cpt(0);
                    for(int k=0;k<_nbSymboles;k++)
                    {
                        if(partitionIntermediaire[k][i] == partitionIntermediaire[k][j])
                            cpt++;
                    }

                    if(cpt == _nbSymboles) // Etats égaux, pas besoin de les différencier
                        nouvellePartition[i] = partitionInitiale[i];
                    else // Différenciation des états
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

        // Affichage partitionIntermédiaire
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
        // Conditions d'arrêt
        if(cpt == _nbEtats)
            end = true;
        if(nbSingletons == _nbEtats)
            end = true;

        // Mise à jour partition Intermédiaire pour le tableau de transition final
        if(end == true)
        {
            // Même méthode précédemment utilisée
            for(int i=0;i<_nbEtats;i++)
            {
                for(int j=0;j<_nbSymboles;j++)
                {
                    for(int n=0;n<_nbEtats;n++) // On parcours les états de l'AFDC
                    {
                        if(_etats[i]->getIndexTransitions(j)->getNbDestinations() == _etats[n]->getIndexTransitions(0)->getNbSources())
                        {
                            // Trouver numéro de la destination
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

    // Affichage partitionIntermédiaire
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

    // Enlever les états en double pour faire afdcm
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
            afdcm->_etats[i]->creerTransition(i, (char)('a'+j), 0); // i correspond à la ième case de partitionInitiale (ex: 0 = N1)
        }
    }

    // Remplir les destinations
    for(int i=0;i<_nbEtats;i++) // On parcours les états
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
                    // Trouver numéro de la destination
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

    // Mise à jour des sorties de l'AFDCM
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

    // Affichage des équivalents
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

    // Vérification du mot vide (*)
    if(mot == "*")
    {
        for(int i=0;i<_nbEtats;i++)
        {
            if(_etats[i]->getInitial() == true && _etats[i]->getTerminal() == true) // Si une entrée est une sortie, c'est bon
            {
                motReconnu = true;
            }
        }
    }
    else // Mot avec des symboles
    {
        for(int j=0;j<_nbEtats;j++) // On parcours les états
        {
            if(_etats[j]->getInitial() == true) // On part d'une entrée
            {
                for(unsigned int i=0;i<mot.size();i++) // Séparer le mot lettre par lettre
                {
                    int cpt2(0);
                    for(int k=0;k<_etats[j]->getNbTransitions();k++) // On parcours les transitions
                    {
                        // Si l'état possède une transition avec ce caractère, on passe à l'état suivant
                        if(_etats[j]->getIndexTransitions(k)->getCaractere() == mot[i])
                        {
                            // Pour cela, il faut trouver il'ndex de la destination
                            int index(0);

                            for(int l=0;l<_nbEtats;l++) // On parcours les états
                            {
                                int cpt(0);
                                if(_etats[l]->getNbTransitions() > 0)
                                {
                                    if(_etats[l]->getIndexTransitions(0)->getNbSources() == _etats[j]->getIndexTransitions(k)->getNbDestinations()) // On trouve la même source
                                    {
                                        for(int m=0;m<_etats[j]->getIndexTransitions(k)->getNbDestinations();m++) // Comparaison 1 à 1 de la source
                                        {
                                            if(_etats[j]->getIndexTransitions(k)->getDestination(m) == _etats[l]->getIndexTransitions(0)->getSource(m))
                                                cpt++;
                                        }
                                    }
                                }

                                if(cpt == _etats[j]->getIndexTransitions(k)->getNbDestinations()) // Si j'ai trouvé la source, on a l'index du prochain état : l
                                {
                                    index = l;
                                    l = _nbEtats;
                                }
                            }
                            k = _etats[j]->getNbTransitions();
                            j = index; // On passe donc à l'état suivant
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
                // Il faut que le dernier état soit forcément une sortie
                if(_etats[j]->getTerminal() == false)
                    motReconnu = false;
            }

            if(motReconnu == true) // Si le mot a été reconnu, pas besoin de tester pour les autres entrées
                j = _nbEtats;
        }
    }

    return motReconnu; // Retourne vrai ou faux si le mot a été reconnu ou non
}
