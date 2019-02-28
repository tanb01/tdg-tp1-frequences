/* code r�alis� par Mr. Fercoq */
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>

/// Retourne la norme d'un vecteur : utilis� par le calcule de distance entre stations
inline double norme(double vx, double vy)
{
    return sqrt(vx*vx + vy*vy);
}

/// La classe Station repr�sente un sommet du graphe
/// Les adjacences sont d�duites des positions x,y des stations
/// durant le chargement du fichier, en fonction du param�tre
class Station
{
private:
    /// Voisinage : liste d'adjacence
    std::vector<const Station*> m_adjacents;

    /// Donn�es sp�cifiques du sommet
    int m_id;        // Indice num�rique pour l'affichage
    double m_x, m_y; // Position sur le terrain
    int m_frequence; // Fr�quence affect�e (ou 0 si pas encore de fr�quence)

public:

    /// La construction se fait directement depuis le fichier ouvert en cours de lecture...
    Station(std::istream& is)
    {
        is >> m_id >> m_x >> m_y;
        if ( is.fail() )
            throw std::runtime_error("Probleme lecture id,x,y d'une station");
        m_frequence = 0;
    }

    /// M�thode de calcul de distance � une autre station (pour d�terminer le voisinage)
    double distance(const Station* autre) const
    {
        return norme( autre->m_x - m_x, autre->m_y - m_y  );
    }

    /// M�thode de d�termination des stations adjacentes ( distance < dmin )
    void determineAdjacents(const std::vector<Station*>& stations, double dmin)
    {
        for (auto s : stations)
            if ( distance(s)<dmin && s!=this )
                m_adjacents.push_back(s);
    }

    /// Retourne le degr� du sommet ( = nombre d'ar�tes incidentes = nombre de sommets adjacents )
    int getDegre() const
    {
        return (int)m_adjacents.size();
    }

    /// Retourne la frequence (num�ro de fr�quence) actuellement affect� � la station
    /// Par convention la valeur 0 indique "pas encore de fr�quence affect�e"
    int getFrequence() const
    {
        return m_frequence;
    }

    /// Affecte une fr�quence � la station
    void setFrequence(int frequence)
    {
        m_frequence = frequence;
    }

    /// Test l'affectation d'une fr�quence
    /// retourne vrai si la fr�quence n'est pas en conflit avec une station adjacente
    /// faux sinon
    bool testFrequence(int frequence)
    {
        for (auto s : m_adjacents)
            if ( s->m_frequence && s->m_frequence == frequence )
                return false;

        return true;
    }

    /// Surcharge de l'op�rateur d'insertion vers un flot de sortie
    /// c'est la "m�thode d'affichage" des objets de type Station
    friend std::ostream& operator<<(std::ostream& out, const Station& s)
    {
        out << "id=" << std::setw(2) << s.m_id << "  x=" << s.m_x << "  y=" << s.m_y << "  freq=" << s.m_frequence << "  Adjacents=";
        for (const auto a : s.m_adjacents)
            out << a->m_id << " ";
        out << std::endl;
        return out;
    }

};

/// La classe Reseau repr�sente un graphe dans son ensemble
class Reseau
{
private:
    /// Le r�seau est constitu� d'une collection de stations
    std::vector<Station*> m_stations;

public:
    /// La construction du r�seau se fait � partir d'un fichier
    /// dont le nom est pass� en param�tre
    Reseau(std::string nomFichier)
    {
        std::ifstream ifs{nomFichier};
        if (!ifs)
            throw std::runtime_error( "Impossible d'ouvrir en lecture " + nomFichier );

        double dmin;
        ifs >> dmin;
        if ( ifs.fail() )
            throw std::runtime_error("Probleme lecture dmin entre stations");

        int ordre;
        ifs >> ordre;
        if ( ifs.fail() )
            throw std::runtime_error("Probleme lecture ordre du graphe");

        for (int i=0; i<ordre; ++i)
            m_stations.push_back( new Station{ifs} );

        for (auto s : m_stations)
            s->determineAdjacents(m_stations, dmin);
    }

    /// Destructeur du r�seau. Les stations ont �t� allou�es dynamiquement
    /// lors de la cr�ation d'une instance reseau, le r�seau est responsable de leur lib�ration
    ~Reseau()
    {
        for (auto s : m_stations)
            delete s;
    }

    /// Retourne l'odre du graphe (ordre = nombre de sommets)
    int getOrdre() const
    {
        return (int)m_stations.size();
    }

    /// Remet toutes les fr�quences des stations � 0 (non affect�es)
    void resetFrequences()
    {
        for (auto s : m_stations)
            s->setFrequence(0);
    }

    /// Surcharge de l'op�rateur d'insertion vers un flot de sortie
    /// c'est la "m�thode d'affichage" des objets de type Reseau
    friend std::ostream& operator<<(std::ostream& out, const Reseau& r)
    {
        out << "Reseau d'ordre " << r.getOrdre() << " :" << std::endl;
        for (const auto s : r.m_stations)
            out << *s;
        out << std::endl;
        return out;
    }

    /// ************* CODE ETUDIANT *************
    void attribuerNaif(int& nbFrequences)
    {
        /// A COMPLETER

        resetFrequences();
        int n=0;
        int freq;

        for (auto s : m_stations)
        {
            freq=1;
            while (s->testFrequence(freq) == false)
            {
                ++freq;
            }
            s->setFrequence(freq);
            if (freq>=n)
            {
                n=freq;
            }
        }
        nbFrequences = n;
    }

    void attribuerWelshPowell(int& nbFrequences)
    {
        /// A COMPLETER
        std::sort(m_stations.begin(), m_stations.end(), [](Station* s1, Station* s2)
        {
            return s1->getDegre() > s2->getDegre();
        });

        resetFrequences();
        int n=0;
        int freq;

        for (auto s : m_stations)
        {
            freq=1;
            while (s->testFrequence(freq) == false)
            {
                ++freq;
            }
            s->setFrequence(freq);
            if (freq>=n)
            {
                n=freq;
            }
        }
        nbFrequences = n;
    }
    /// ************* FIN CODE ETUDIANT *************

    /// M�thode "optimale", d�termination du nombre chromatique.
    /// Voir impl�mentation apr�s le main
    void attribuerSystematique(int& nbFrequences);
};


int main()
{
    try
    {
        /// Chargement du r�seau et affichage avant coloration
        Reseau reseau{"reseau3.txt"};
        std::cout << reseau;

        /// Coloration algorithme "na�f" et affichage
        int nbFreqNaif;
        reseau.attribuerNaif(nbFreqNaif);
        std::cout << "Naif : " << nbFreqNaif << " frequences utilisees" << std::endl;
        std::cout << reseau;

        /// Coloration algorithme "Welsh et Powell" et affichage
        int nbFreqWP;
        reseau.attribuerWelshPowell(nbFreqWP);
        std::cout << "Welsh & Powell : " << nbFreqWP << " frequences utilisees" << std::endl;
        std::cout << reseau;

        /// Coloration algorithme "Syst�matique" et affichage
        int nbFreqSyst;
        reseau.attribuerSystematique(nbFreqSyst);
        std::cout << "Nombre chromatique : " << nbFreqSyst << " frequences utilisees (optimal)" << std::endl;
        std::cout << reseau;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Attention : " << e.what() << std::endl;
    }

    return 0;
}



/// L'algorithme suivant trouve une colloration
/// en essayant syst�matiquement toutes les combinaisons
/// d'affectations avec 1 couleur, puis 2 couleurs, puis 3 ...
/// Cet algorithme garantit donc une meilleure coloration
/// ( nombre de couleur utilis�es = nombre chromatique )
/// mais il pr�sente un grave inconv�nient... Lequel ?
void Reseau::attribuerSystematique(int& nbFrequences)
{
    nbFrequences = 0;

    // Pour chaque nombre de fr�quences � utiliser
    while ( true )
    {
        ++nbFrequences;

        // On va tester les "nbFrequences puissance ordre" combinaisons
        // Exemple : pour r�partir 3 couleurs sur 4 sommets il y a 3^4 = 81 cas � �tudier
        // Ceci est �quivalent � un comptage en base 3 sur 4 chiffres : 0000 0001 0002 0010 0011 0012 0020 ...
        // On va tester les affectations correspondantes              : 1111 1112 1113 1121 1122 1123 1131 ...
        int nbCas = std::pow(nbFrequences, getOrdre());
        for (int k=0; k<nbCas; ++k)
        {
            // On va tester l'affectation de nbFrequences correspondant au k �tudi�
            resetFrequences();
            int nbAffecte = 0;
            int combi = k;
            // d�composition de l'entier k en base nbFrequences et affectations correspondante
            for (auto s : m_stations)
            {
                // R�cup�ration du chiffre suivant en base nbFrequences, +1 pour �viter le 0 (0 => pas encore de fr�quence)
                int freq = combi%nbFrequences + 1;
                // Si on trouve une incompatibilit� dans l'affectation alors on va passer au k suivant
                // en sortant de la boucle d'affection des sommets du k actuel
                if ( !s->testFrequence(freq) )
                    break;
                // Sinon l'affectation de ce sommet est pour l'instant possible...
                s->setFrequence(freq);
                ++nbAffecte;
                // Pr�paration de l'extraction du chiffre suivant en base nbFrequences
                combi /= nbFrequences;
            }

            // Tous les sommets sont affect�s : c'est gangn� on a bien une affectation globale optimale !
            if ( nbAffecte == getOrdre() )
                return;
        }
    }
}
