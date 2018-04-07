#include "B4-automate.h"

using namespace std;

int main()
{
    char nouvelAutomate; // R�ponse de l'utilisateur s'il veut recommencer le processus avec un autre automate

    do
    {
        cout << "AAA";
        // Fichier � ouvrir
        string file;
        int erreur = 0;

        // Tant que le nom du fichier est erron�, on recommence
        do
        {
            // Nom du fichier � ouvrir
            erreur = 0;
            cout << "Entrer le nom du fichier a ouvrir : ";
            cin >> file;
            file = "automates/" + file + ".txt"; // Emplacement du fichier dans le dossier "automates"

            // Ouverture en mode lecture
            ifstream flux(file.c_str(), ios::in);

            // Erreur lors de l'ouverture
            if(flux)
            {
                // Test du fichier vide
                char caractere;
                flux.get(caractere);
                if(flux.eof()) // Si le premier caract�re = fin du fichier
                {
                    cout << "Le fichier est vide." << endl;
                    erreur = 1;
                }
                // Fermeture du fichier
                flux.close();
            }
            else
            {
                cout << "ERREUR: Impossible d'ouvrir le fichier en lecture." << endl;
                erreur = 1;
            }
        } while(erreur == 1);

        // Automates
        Automate automate(file); // Automate issu du fichier (constructeur = chargement des donn�es)
        Automate * afd(NULL); // Automate fini d�terministe complet
        Automate * afdcm(NULL); // Automate fini d�terministe complet minimis�

        // Affichage de l'automate
        automate.afficher();

        // Test automate synchrone
        if(automate.estSynchrone() == false)
        {
            cout << "Automate asynchrone : transition *" << endl;
            // Proc�dure de d�terminisation et compl�tion de l'AF asynchrone
            afd = automate.determinisation();
            cout << "Automate fini deterministe complet" << endl;
            afd->afficher();
        }
        else
        {
            cout << "Automate synchrone" << endl;
            // Test automate d�terministe
            if(automate.estDeterministe() == false)
            {
                cout << "Automate non deterministe : " << endl;
                // Raisons
                if(automate.getNbEntrees() > 1)
                    cout << "\t- " << automate.getNbEntrees() << " entrees" << endl;
                else
                    cout << "\t- Plusieurs transitions sortantes avec le meme symbole" << endl;
                // Proc�dure de d�terminisation et compl�tion de l'AF non d�terministe
                afd = automate.determinisation();
                cout << "Automate fini deterministe complet" << endl;
                afd->afficher();
            }
            else
            {
                cout << "Automate deterministe" << endl;

                // Test automate complet
                if(automate.estComplet() == false)
                {
                    cout << "Automate non complet :" << endl;
                    cout << "\t- Un etat a une transition manquante." << endl;
                    // Proc�dure de compl�tion de l'AF
                    afd = automate.completion();
                    cout << "Automate fini deterministe complet" << endl;
                    afd->afficher();
                }
                else
                {
                    cout << "Automate complet" << endl;
                    afd = &automate; // Si on choisit l'afd pour tester un mot, il ne faut pas que afd soit vide
                }
            }
        }

        // Minimisation � partir de l'AFDC (obligatoire)
        if(automate.estComplet() == true)
            afdcm = automate.minimisation();
        else
            afdcm = afd->minimisation();

        char reponse; // Essayer un autre mot
        do
        {
            // Test d'un mot � partir de AF, AFDC, AFDCM
            cout << "Test d'un mot a partir de : " << endl;
            cout << "\tA- AF" << endl;
            cout << "\tB- AFDC" << endl;
            cout << "\tC- AFDCM" << endl;

            char result;
            cin >> result;
            while(result != 'A' && result != 'B' && result != 'C')
            {
                cout << "Erreur, veuillez recommencer." << endl;
                cin >> result;
            }

            cout << "Entrez le mot a tester (mot vide = *) : ";
            string mot;
            cin >> mot;

            // Test du mot
            bool testMot(false);
            if(result == 'A')
                testMot = automate.test(mot);
            else if(result == 'B')
                testMot = afd->test(mot);
            else if(result == 'C')
                testMot = afdcm->test(mot);

            // Affichage de la r�ponse
            if(testMot == true)
                cout << "Mot reconnu" << endl;
            else
                cout << "Mot non reconnu" << endl;

            cout << "Voulez-vous essayer un autre mot ?" << endl;
            cout << "\tA- Oui" << endl;
            cout << "\tB- Non" << endl;
            cin >> reponse;
            while(reponse != 'A' && reponse != 'B')
            {
                cout << "Erreur, veuillez recommencer." << endl;
                cin >> reponse;
            }
        } while(reponse == 'A');

        cout << "Voulez-vous essayer un autre automate ?" << endl;
        cout << "\tA- Oui" << endl;
        cout << "\tB- Non" << endl;
        cin >> nouvelAutomate;
        while(nouvelAutomate != 'A' && nouvelAutomate != 'B')
        {
            cout << "Erreur, veuillez recommencer." << endl;
            cin >> nouvelAutomate;
        }
    } while(nouvelAutomate == 'A');

    return 0;
}
