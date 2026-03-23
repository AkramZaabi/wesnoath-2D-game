/**Dernier dépôt réussi N°33 - effectué à la date : 2023-05-14 14:34:54*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#define MAX_ID_TRAITS 16
#define ID_TYPE_TERRAIN_CHATEAU 1
#define ID_TYPE_TERRAIN_VILLAGE 16
#define ID_TYPE_TERRAIN_DONJON 17
#define MAX_ELOIGNEMENT_CHATEAU_DONJON 2
#define NB_LIGNES_CARTE 30
#define NB_COLONNES_CARTE 38
#define MAX_LIGNES_JOUEURS 5
#define MAX_LIGNES_UNITESJOUEURS 50
#define MAX_LIGNES_FICHES_TYPES_UNITES 10
#define MAX_LIGNES_UNITESMAGASIN 8
#define MAX_LIGNES_VILLAGES 14
#define MAX_LIGNES_PERIODES 10
#define NB_UNITES_RANG_INF_SUP 3
#define MAX_ATTAQUES 3
#define NB_RESISTANCES 6
#define NB_TYPES_TERRAINS 17
#define NB_LIGNES_PERIODES 6
#define MAX_CAPACITES 3
#define MAX_TRAITS 3
#define MAX_EFFETS_ATTAQUES_SUBITES 10
#define DOUBLE_CLICK_MAX_DELAY 1000
int Tour= 0 ;
typedef struct
{
    int idAttaque;
    char nomAttaque[20];
    int degat;
    int nombreAttaques;
    int porteeAttaque;
    int typeAttaque ;
    int idAttaqueSpeciale1;
    int idAttaqueSpeciale2;
} Attaque;


enum
{
    VIDE,CHATEAU,CAVERNE,RECIF,EAU_PROFONDE,FAUX_VOILE,PLAT,FORET,GELE,CHAMPIGNONS,COLLINES,MONTAGNES,SABLE,EAU_PEU_PROFONDE,MARAIS,IMPRATICABLE,VILLAGE,DONJON
};
typedef struct
{
    int idTypeTerrain;
    int coutMouvement;
    int defense;
} RelationTerrain;

typedef struct
{
    int idFicheTypeUnite;
    int race;
    char nom[20];
    int prix;
    int pvMax;
    int mouvementsMax;
    int xpRequise;
    int niveau;
    int alignement;
    int idFichesTypeUniteRangInf[NB_UNITES_RANG_INF_SUP]; // 0 si non existant
    int idFichesTypeUniteRangSup[NB_UNITES_RANG_INF_SUP]; // 0 si non existant
    Attaque attaques [MAX_ATTAQUES];
    int resistances[NB_RESISTANCES] ;
    RelationTerrain relationsTerrains[NB_TYPES_TERRAINS];
    int idCapacites[MAX_CAPACITES];
} FicheTypeUnite;

typedef struct
{
    int idAttaqueSpeciale;
    int toursRestants;
} EffetAttaqueSubite;

typedef struct
{
    int idUnite;
    int idFicheTypeUnite;
    int idJoueur;
    char nomUnite[20];
    int active;
    int rang;
    int ligne;
    int colonne;
    int traits[MAX_TRAITS];
    int pv;
    int pvMax;
    int mouvements;
    int mouvementsMax;
    int xp;
    int xpRequise;
    int niveau;
    int finTour;
    EffetAttaqueSubite effetsAttaquesSubites[MAX_EFFETS_ATTAQUES_SUBITES];
    int alignement;
} UniteJoueur;

typedef struct
{
    int idJoueur;
    char symbole;
    int or;
    int groupe_allies;
    int nombreVillages;
    int nombreUnites;
    int entretien;
    int revenu;
} Joueur;

typedef struct
{
    int idUniteMagasin;
    int idFicheTypeUnite;
    int idJoueurAutorise;
} UniteMagasin;

typedef struct
{
    int idVillage;
    int ligne;
    int colonne;
    int idJoueurProprietaire;
} Village;

typedef struct
{
    int idTypeTerrain;
    char symboleTerrain;
    int codeAffichageTerrain;
    char nomTerrain[20];
} TypeTerrain;

typedef struct
{
    int idUnite;
    int idTypeTerrain;
    int codeAffichageTerrain;
    char symboleJoueur;
    int marque;
} CelluleCarte;

typedef struct
{
    int numOrdre;
    char moment[20];
    int bonus[4];
} Periode;

typedef enum {TRAIT_NEANT, TRAIT_INTELLIGENT, TRAIT_RAPIDE, TRAIT_ROBUSTE, TRAIT_FORT,
              TRAIT_HABILE, TRAIT_INTREPIDE, TRAIT_SAUVAGE, TRAIT_VIGOUREUX, TRAIT_AGE, TRAIT_IDIOT,
              TRAIT_ELEMENTAL, TRAIT_FIDELE, TRAIT_MECANIQUE, TRAIT_LENT, TRAIT_MORTVIVANT,
              TRAIT_FAIBLE
             } E_TRAITS;
int maximum(int a,int b)
{
    if(a>b)
    {
        return a;
    }
    else
    {
        return b;
    }
}
int minimum(int a,int b)
{
    if(a>b)
    {
        return b;
    }
    else
    {
        return a;
    }
}


int chercherIndiceJoueur(int idJoueur, int nbLignesReel, Joueur joueurs[MAX_LIGNES_JOUEURS])
{
    int i=0,pos=-1;
    while (pos==-1 && i<nbLignesReel)
    {
        if(joueurs[i].idJoueur==idJoueur)
        {
            pos=i;
            return i;
        }
        i++;
    }
    return pos;
}

int chercherIndiceFicheTypeUnite(int idFicheTypeUnite, int nbLignesReel, FicheTypeUnite fichesTypesUnites[MAX_LIGNES_FICHES_TYPES_UNITES])
{
    int i=0,pos=-1;
    while(pos==-1 && i< nbLignesReel)
    {
        if(fichesTypesUnites[i].idFicheTypeUnite==idFicheTypeUnite)
        {
            pos=i;
            return pos;
        }
        i++;
    }
    return pos;
}

int chercherIndiceRelationTerrain(int idTypeTerrain, RelationTerrain relationsTerrains[NB_TYPES_TERRAINS])
{
    int pos=-1;
    int i=0;
    while (pos==-1 && i<NB_TYPES_TERRAINS)
    {
        if(relationsTerrains[i].idTypeTerrain==idTypeTerrain)
        {
            pos=1;
            return i;
        }
        i++;
    }
    return pos;

}

int chercherIndiceTypeTerrain(char symboleTerrain, TypeTerrain typesTerrains[NB_TYPES_TERRAINS])
{
    int pos=-1;
    int i=0;
    while(pos==-1 && i<NB_TYPES_TERRAINS)
    {
        if(typesTerrains[i].symboleTerrain==symboleTerrain)
        {
            pos=i;
            return pos;
        }
        i++;
    }
    return pos;
}

int chercherIndiceUniteJoueur(int idUnite, int idJoueur, int active, UniteJoueur unitesJoueurs[MAX_LIGNES_UNITESJOUEURS])
{
    int i=0,pos=-1;
    while(pos==-1 && i<MAX_LIGNES_UNITESJOUEURS)
    {
        if(unitesJoueurs[i].idUnite==idUnite && unitesJoueurs[i].idJoueur==idJoueur && unitesJoueurs[i].active==active)
        {
            pos=i;
            return pos;
        }
        i++;
    }
    return pos;
}

int chercherGroupeAllies(char symbolJoueur, int nbJoueurs, Joueur joueurs[MAX_LIGNES_JOUEURS])
{

    int i=0,exist=0;
    while(exist==0 && i<nbJoueurs)
    {
        if(symbolJoueur==joueurs[i].symbole)
        {
            return joueurs[i].groupe_allies;
        }

        i++;
    }
    return -1;

}

int chercherIndiceJoueurHumain(int nbJoueurs, Joueur joueurs[MAX_LIGNES_JOUEURS])
{
    int pos=-1;
    int i=0;
    while (i<nbJoueurs)
    {
        if(joueurs[i].symbole=='J')
        {
            pos=i;
            return pos;
        }
        i++;
    }
    return pos;
}

int chercherIndiceVillage(int ligne, int colonne, int nbVillages, Village villages[MAX_LIGNES_VILLAGES])
{
    int pos=-1;
    int i=0;
    while(pos==-1 && i < nbVillages )
    {
        if(villages[i].colonne==colonne && villages[i].ligne==ligne)
        {
            pos=i;
            return pos;
        }
        i++;
    }
    return pos;
}

int chercherIndiceUniteMagasin(int idUnite, int nbUnitesMagasin, UniteMagasin unitesMagasin[MAX_LIGNES_UNITESMAGASIN])
{

    int trouve=0;
    int i=0;
    while(i<nbUnitesMagasin && trouve==0)
    {
        if(unitesMagasin[i].idUniteMagasin==idUnite)
        {
            return i;
        }
        i++;
    }
    return -1;

}

int chercherMaxIdUnitesJoueurs(UniteJoueur unitesJoueurs[MAX_LIGNES_UNITESJOUEURS])
{
    int Max_id=0;
    for(int i=0; i<MAX_LIGNES_UNITESJOUEURS; i++)
    {
        if(unitesJoueurs[i].idUnite>Max_id)
        {
            Max_id=unitesJoueurs[i].idUnite;
        }
    }
    return Max_id;
}

int chercherIndiceChefUnitesJoueurs(int idJoueur,UniteJoueur unitesJoueurs[MAX_LIGNES_UNITESJOUEURS])
{
    int trouve =0;
    int i=0;
    while(  i<MAX_LIGNES_UNITESJOUEURS&&trouve==0)
    {
        if(unitesJoueurs[i].active && unitesJoueurs[i].rang==1 && unitesJoueurs[i].idJoueur==idJoueur)
        {
            return i;
        }
        i++;
    }
    return -1;
}

int chercherIndiceJoueurParSymbole(char symbole, int nbJoueurs, Joueur joueurs[MAX_LIGNES_JOUEURS])
{

    int trouve=0;
    int i=0;
    while(i< nbJoueurs && trouve==0)
    {
        if(joueurs[i].symbole==symbole)
        {
            trouve=1;
            return i;
        }
        i++;
    }
    return -1;

}

int possedeTrait(int trait, int traits[MAX_TRAITS])
{

    int i=0,pos=-1;
    while(i<MAX_TRAITS && pos==-1)
    {
        if(trait==traits[i])
        {
            pos=1;
            return 1;
        }
        i++;

    }
    return 0;
}

void appliquerTrait(int indiceUniteJoueur, int trait, UniteJoueur unitesJoueurs[MAX_LIGNES_UNITESJOUEURS])
{

    switch(trait)
    {
    case TRAIT_NEANT:
        break;
    case TRAIT_INTELLIGENT:
        unitesJoueurs[indiceUniteJoueur].xpRequise*=0.8;
        break;
    case TRAIT_RAPIDE:
        unitesJoueurs[indiceUniteJoueur].pv*=0.95;
        unitesJoueurs[indiceUniteJoueur].mouvements+=1;
        break;
    case TRAIT_ROBUSTE:
        unitesJoueurs[indiceUniteJoueur].pv+=4;
        break;
    case TRAIT_FORT:
        unitesJoueurs[indiceUniteJoueur].pv+=1;
        break;
    case TRAIT_HABILE:

        break;
    case TRAIT_INTREPIDE:

        break;
    case TRAIT_SAUVAGE:

        break;
    case TRAIT_VIGOUREUX:
        unitesJoueurs[indiceUniteJoueur].pv+=1;
        break;
    case TRAIT_AGE:
        unitesJoueurs[indiceUniteJoueur].pv+=8;
        unitesJoueurs[indiceUniteJoueur].mouvements=maximum(unitesJoueurs[indiceUniteJoueur].mouvements-1,0);
        break;
    case TRAIT_IDIOT:
        unitesJoueurs[indiceUniteJoueur].xpRequise*=1.2;
        break;
    case TRAIT_ELEMENTAL:

        break;
    case TRAIT_FIDELE:

        break;
    case TRAIT_MECANIQUE:
        break;
    case  TRAIT_LENT:
        unitesJoueurs[indiceUniteJoueur].pv*=1.05;
        unitesJoueurs[indiceUniteJoueur].mouvements=maximum(unitesJoueurs[indiceUniteJoueur].mouvements-1,0);
        break;
    case TRAIT_MORTVIVANT:
        break;
    case TRAIT_FAIBLE:
        unitesJoueurs[indiceUniteJoueur].pv-=1;
        break;

    }
}

void miseAJourJoueurApresOccupationVillage(int indiceJoueur, Joueur joueurs[MAX_LIGNES_JOUEURS])
{
    joueurs[indiceJoueur].revenu=2+joueurs[indiceJoueur].nombreVillages-maximum(0,joueurs[indiceJoueur].entretien-joueurs[indiceJoueur].nombreVillages);

}

int chargerUnitesMagasinVersTableau(UniteMagasin unitesMagasin[MAX_LIGNES_UNITESMAGASIN], char* unitesmagasin)
{

    FILE *ptr_UniteMagsin;
    ptr_UniteMagsin=fopen(unitesmagasin,"r");


    int i=0;
    while(!feof(ptr_UniteMagsin))
    {
        fscanf(ptr_UniteMagsin,"%d %d %d",&unitesMagasin[i].idUniteMagasin,&unitesMagasin[i].idFicheTypeUnite,&unitesMagasin[i].idJoueurAutorise);
        i++;

    }
    fclose(ptr_UniteMagsin);
    return i-1;

}

int chargerVillagesVersTableau(Village villages[MAX_LIGNES_VILLAGES], char *Unite_village)
{
    FILE *ptr_villages;
    ptr_villages=fopen(Unite_village,"r");
    int i=0;
    while(!feof(ptr_villages))
    {
        fscanf(ptr_villages,"%d %d %d %d",&villages[i].idVillage,&villages[i].ligne,&villages[i].colonne,&villages[i].idJoueurProprietaire);
        i++;
    }
    fclose(ptr_villages);
    return i;

}

int chargerTypesTerrainsVersTableau(TypeTerrain typesTerrains[NB_TYPES_TERRAINS],char*typesTerrains_original)
{
    FILE *ptr_TypeTerrain;
    ptr_TypeTerrain=fopen(typesTerrains_original,"r");
    int i=0;

    while(!feof(ptr_TypeTerrain))
    {
        fscanf(ptr_TypeTerrain,"%d %c %d %s",&typesTerrains[i].idTypeTerrain,&typesTerrains[i].symboleTerrain,&typesTerrains[i].codeAffichageTerrain,typesTerrains[i].nomTerrain);
        i++;
    }
    fclose(ptr_TypeTerrain);


    return i;
}


int chargerPeriodesVersTableau(Periode periodes[MAX_LIGNES_PERIODES],char* periodes_original)
{
    FILE *ptr_periodes;

    ptr_periodes=fopen(periodes_original,"r");
    if(ptr_periodes==NULL)
    {
        printf("file not found!!!");
    }

    int i=0;

    while(!feof(ptr_periodes))
    {

        fscanf(ptr_periodes,"%d %s %d %d %d %d",&periodes[i].numOrdre,periodes[i].moment,&periodes[i].bonus[0],&periodes[i].bonus[1],&periodes[i].bonus[2],&periodes[i].bonus[3]);
        i++;
    }

    fclose(ptr_periodes);
    return i;
}

int chargerFichesTypesUnitesVersTableau(FicheTypeUnite fichesTypesUnites[MAX_LIGNES_FICHES_TYPES_UNITES], char* fiches_Unites)
{
    FILE *type_unite;
    type_unite=fopen(fiches_Unites,"r");
    int i =0;
    while(!feof(type_unite) )
    {
        fscanf(type_unite,"%d %d %s %d %d %d %d %d %d",&fichesTypesUnites[i].idFicheTypeUnite,&fichesTypesUnites[i].race,fichesTypesUnites[i].nom,&fichesTypesUnites[i].prix,&fichesTypesUnites[i].pvMax,&fichesTypesUnites[i].mouvementsMax,&fichesTypesUnites[i].xpRequise,&fichesTypesUnites[i].niveau,&fichesTypesUnites[i].alignement);
        for(int j=0; j<3; j++)
        {
            fscanf(type_unite,"%d ",&fichesTypesUnites[i].idFichesTypeUniteRangInf[j]);
        }
        for(int j=0; j<3; j++)
        {
            fscanf(type_unite,"%d ",&fichesTypesUnites[i].idFichesTypeUniteRangSup[j]);
        }
        for(int j=0; j<3; j++)
        {
            fscanf(type_unite,"%d %s %d %d %d %d %d %d",&fichesTypesUnites[i].attaques[j].idAttaque,fichesTypesUnites[i].attaques[j].nomAttaque,&fichesTypesUnites[i].attaques[j].degat,&fichesTypesUnites[i].attaques[j].nombreAttaques,&fichesTypesUnites[i].attaques[j].porteeAttaque,&fichesTypesUnites[i].attaques[j].typeAttaque,&fichesTypesUnites[i].attaques[j].idAttaqueSpeciale1,&fichesTypesUnites[i].attaques[j].idAttaqueSpeciale2);
        }
        for(int j=0; j<6; j++)
        {
            fscanf(type_unite,"%d ",&fichesTypesUnites[i].resistances[j]);
        }
        for(int j=0; j<17; j++)
        {
            fscanf(type_unite,"%d ",&fichesTypesUnites[i].relationsTerrains[j].coutMouvement);
        }
        for(int j=0; j<17; j++)
        {
            fscanf(type_unite,"%d ",&fichesTypesUnites[i].relationsTerrains[j].defense);
        }
        for(int j=0; j<3; j++)
        {
            fscanf(type_unite,"%d ",&fichesTypesUnites[i].idCapacites[j]);
        }
        i++;
    }
    fclose(type_unite);
    return i;
}

int chargerJoueursVersTableau(Joueur joueurs[MAX_LIGNES_JOUEURS], char* joueurs_original)
{
    FILE *fichier_joueur;
    fichier_joueur=fopen(joueurs_original,"r");
    if(fichier_joueur==NULL)
    {
        printf("file not found!!!");
    }
    int i=0;
    while(!feof(fichier_joueur))
    {
        fscanf(fichier_joueur,"%d %c %d %d",&joueurs[i].idJoueur,&joueurs[i].symbole,&joueurs[i].or,&joueurs[i].groupe_allies);
        joueurs[i].nombreUnites=0;
        joueurs[i].revenu=0;
        joueurs[i].nombreVillages=0;
        joueurs[i].entretien=0;
        i++;

    }
    fclose(fichier_joueur);
    return i-1;
}

void chargerUniteJoueurVersTableau(UniteJoueur unitesJoueurs[MAX_LIGNES_UNITESJOUEURS], char* unite_joueurs_original)
{

    FILE *fich_joueur;
    fich_joueur=fopen(unite_joueurs_original,"r");
    int i=0;
    while(!feof(fich_joueur))
    {
        fscanf(fich_joueur,"%d %d %d %s %d %d %d %d",&unitesJoueurs[i].idUnite,&unitesJoueurs[i].idFicheTypeUnite,&unitesJoueurs[i].idJoueur,unitesJoueurs[i].nomUnite,&unitesJoueurs[i].active,&unitesJoueurs[i].rang,&unitesJoueurs[i].ligne,&unitesJoueurs[i].colonne);
        fscanf(fich_joueur,"%d %d %d",&unitesJoueurs[i].traits[0],&unitesJoueurs[i].traits[1],&unitesJoueurs[i].traits[2]);
        unitesJoueurs[i].pv=0;
        unitesJoueurs[i].pvMax=0;
        unitesJoueurs[i].mouvements=0;
        unitesJoueurs[i].mouvementsMax=0;
        unitesJoueurs[i].xp=0;
        unitesJoueurs[i].xpRequise=0;
        unitesJoueurs[i].niveau=0;
        unitesJoueurs[i].finTour=0;
        i++;

    }
    for(int j=i ; j<MAX_LIGNES_UNITESJOUEURS; j++)
    {
        unitesJoueurs[j].idUnite=0;
        unitesJoueurs[j].idFicheTypeUnite=0;
        unitesJoueurs[j].idJoueur=0;
        strcpy(unitesJoueurs[j].nomUnite," ");
        unitesJoueurs[j].nomUnite[strlen(unitesJoueurs[j].nomUnite)]='\0';
        unitesJoueurs[j].active=0;
        unitesJoueurs[j].rang=0;
        unitesJoueurs[j].ligne=0;
        unitesJoueurs[j].colonne=0;
        unitesJoueurs[j].traits[0]=0;
        unitesJoueurs[j].traits[1]=0;
        unitesJoueurs[j].traits[2]=0;
        unitesJoueurs[j].pv=0;
        unitesJoueurs[j].pvMax=0;
        unitesJoueurs[j].mouvements=0;
        unitesJoueurs[j].mouvementsMax=0;
        unitesJoueurs[j].xp=0;
        unitesJoueurs[j].xpRequise=0;
        unitesJoueurs[j].niveau=0;
        unitesJoueurs[j].finTour=0;
    }

    fclose(fich_joueur);


}

void chargerCarteVersTableau(CelluleCarte carte[NB_LIGNES_CARTE][ NB_COLONNES_CARTE],TypeTerrain typesTerrains[NB_TYPES_TERRAINS], char* carte_original)
{

    FILE *fich_carte;
    fich_carte=fopen(carte_original,"r");

    char symbole_type_terrain;
    for(int i=0; i<NB_LIGNES_CARTE; i++)
    {
        for(int j=0; j<NB_COLONNES_CARTE; j++)
        {

            fscanf(fich_carte,"%c ",&symbole_type_terrain);

            carte[i][j].idTypeTerrain=typesTerrains[chercherIndiceTypeTerrain(symbole_type_terrain,typesTerrains)].idTypeTerrain;


            carte[i][j].codeAffichageTerrain=typesTerrains[chercherIndiceTypeTerrain(symbole_type_terrain,typesTerrains)].codeAffichageTerrain;
            carte[i][j].idUnite=0;
            carte[i][j].marque=0;
            carte[i][j].symboleJoueur=' ';
        }
    }

    fclose(fich_carte);

}

void chargerUnitesJoueursSauvegardeesVersTableau(UniteJoueur unitesJoueurs[MAX_LIGNES_UNITESJOUEURS], char* unite_joueurs_sauvegarde)
{
    FILE *fich_jsauv;

    fich_jsauv=fopen(unite_joueurs_sauvegarde,"r");
    int i;
    int max_indice;
    for(i=0; i<MAX_LIGNES_UNITESJOUEURS; i++)
    {


        fscanf(fich_jsauv,"%d %d %d %s %d %d %d %d",&unitesJoueurs[i].idUnite,&unitesJoueurs[i].idFicheTypeUnite,&unitesJoueurs[i].idJoueur,unitesJoueurs[i].nomUnite,&unitesJoueurs[i].active,&unitesJoueurs[i].rang,&unitesJoueurs[i].ligne,&unitesJoueurs[i].colonne);
        fscanf(fich_jsauv,"%d %d %d",&unitesJoueurs[i].traits[0],&unitesJoueurs[i].traits[1],&unitesJoueurs[i].traits[2]);
        unitesJoueurs[i].pv=0;
        unitesJoueurs[i].pvMax=0;
        unitesJoueurs[i].mouvements=0;
        unitesJoueurs[i].mouvementsMax=0;
        unitesJoueurs[i].xp=0;
        unitesJoueurs[i].xpRequise=0;
        unitesJoueurs[i].niveau=0;
        unitesJoueurs[i].finTour=0;
        if(unitesJoueurs[i].idUnite==0 && unitesJoueurs[i].idFicheTypeUnite==0)
        {
            max_indice=i;
            break;
        }
    }
    for(int j=max_indice; j<MAX_LIGNES_UNITESJOUEURS; j++)
    {

        unitesJoueurs[j].idUnite=0;
        unitesJoueurs[j].idFicheTypeUnite=0;
        unitesJoueurs[j].idJoueur=0;
        strcpy(unitesJoueurs[j].nomUnite," ");
        unitesJoueurs[j].nomUnite[strlen(unitesJoueurs[j].nomUnite)]='\0';
        unitesJoueurs[j].active=0;
        unitesJoueurs[j].rang=0;
        unitesJoueurs[j].ligne=0;
        unitesJoueurs[j].colonne=0;
        unitesJoueurs[j].traits[0]=0;
        unitesJoueurs[j].traits[1]=0;
        unitesJoueurs[j].traits[2]=0;
        unitesJoueurs[j].pv=0;
        unitesJoueurs[j].pvMax=0;
        unitesJoueurs[j].mouvements=0;
        unitesJoueurs[j].mouvementsMax=0;
        unitesJoueurs[j].xp=0;
        unitesJoueurs[j].xpRequise=0;
        unitesJoueurs[j].niveau=0;
        unitesJoueurs[j].finTour=0;

    }

    fclose(fich_jsauv);
}

void placerUnitesDansCarte(UniteJoueur unitesJoueurs[MAX_LIGNES_UNITESJOUEURS],int nbJoueurs, Joueur joueurs[MAX_LIGNES_JOUEURS],CelluleCarte carte[NB_LIGNES_CARTE][NB_COLONNES_CARTE])
{
    for(int i=0; i<MAX_LIGNES_UNITESJOUEURS; i++)
    {
        carte[unitesJoueurs[i].ligne][unitesJoueurs[i].colonne].idUnite=unitesJoueurs[i].idUnite;
        carte[unitesJoueurs[i].ligne][unitesJoueurs[i].colonne].symboleJoueur=joueurs[chercherIndiceJoueur(unitesJoueurs[i].idJoueur,nbJoueurs,joueurs)].symbole;
    }
}

void completerInitialisationUnitesJoueurs(UniteJoueur unitesJoueurs[MAX_LIGNES_UNITESJOUEURS],int nbFichesTypesUnites, FicheTypeUnite fichesTypesUnites[MAX_LIGNES_FICHES_TYPES_UNITES])
{
    int indice =0;
    for(int i=0; i<MAX_LIGNES_UNITESJOUEURS; i++)
    {
        indice=chercherIndiceFicheTypeUnite(unitesJoueurs[i].idFicheTypeUnite,nbFichesTypesUnites,fichesTypesUnites);
        if(indice!=-1)
        {

            unitesJoueurs[i].pv=fichesTypesUnites[indice].pvMax;
            unitesJoueurs[i].pvMax=fichesTypesUnites[indice].pvMax;
            unitesJoueurs[i].mouvements=fichesTypesUnites[indice].mouvementsMax;
            unitesJoueurs[i].mouvementsMax=fichesTypesUnites[indice].mouvementsMax;
            unitesJoueurs[i].xp=0;
            unitesJoueurs[i].xpRequise=fichesTypesUnites[indice].xpRequise;
            unitesJoueurs[i].niveau=fichesTypesUnites[indice].niveau;
            unitesJoueurs[i].finTour=0;
            unitesJoueurs[i].alignement=fichesTypesUnites[indice].alignement;
            for(int j=0; j<MAX_TRAITS; j++)
            {
                appliquerTrait(i,unitesJoueurs[i].traits[j],unitesJoueurs);
            }
        }
        else
        {
            unitesJoueurs[i].pv=0;
            unitesJoueurs[i].pvMax=0;
            unitesJoueurs[i].mouvements=0;
            unitesJoueurs[i].mouvementsMax=0;
            unitesJoueurs[i].xp=0;
            unitesJoueurs[i].xpRequise=0;
            unitesJoueurs[i].niveau=0;
            unitesJoueurs[i].finTour=0;
            unitesJoueurs[i].alignement=0;

        }


    }
}

void completerInitialisationJoueurs(int nbJoueurs, int nbVillages, Joueur joueurs[MAX_LIGNES_JOUEURS],Village villages[MAX_LIGNES_VILLAGES],UniteJoueur unitesJoueurs[MAX_LIGNES_UNITESJOUEURS])
{
    for(int i=0; i<nbJoueurs; i++)
    {

        for(int j=0; j<nbVillages; j++)
        {
            if(villages[j].idJoueurProprietaire==joueurs[i].idJoueur)
            {
                joueurs[i].nombreVillages++;
            }
        }
        for(int j=0; j<MAX_LIGNES_UNITESJOUEURS; j++)
        {
            if(  unitesJoueurs[j].idJoueur!=0 && unitesJoueurs[j].idJoueur==joueurs[i].idJoueur )
            {
                joueurs[i].nombreUnites++;
                if(possedeTrait(TRAIT_FIDELE,unitesJoueurs[j].traits))
                {
                    joueurs[i].entretien=0;
                }
                else
                {
                    joueurs[i].entretien+=unitesJoueurs[j].niveau;
                }
            }

        }
        if(joueurs[i].entretien-nbVillages>0)
        {
            joueurs[i].revenu=2+nbVillages- joueurs[i].entretien-nbVillages;
        }
        else
        {
            joueurs[i].revenu=2+joueurs[i].nombreVillages;
        }

    }
}

void afficherUnitesMagasin(int nb_lignes,UniteMagasin unitesMagasin[MAX_LIGNES_UNITESMAGASIN])
{
    printf("affichage des Unites Magasin:\n");
    for(int i=0; i<nb_lignes; i++)
    {
        printf("%d %d %d\n",unitesMagasin[i].idUniteMagasin,unitesMagasin[i].idFicheTypeUnite,unitesMagasin[i].idJoueurAutorise);
    }

}

void afficherVillages(int nb_lignes2, Village villages[MAX_LIGNES_VILLAGES])
{
    printf("affichage des Villages:\n");
    for(int i=0; i<nb_lignes2; i++)
    {
        printf("%d %d %d %d \n",villages[i].idVillage,villages[i].ligne,villages[i].colonne,villages[i].idJoueurProprietaire);
    }

}

void afficherTypesTerrains(int nb_lignes3,TypeTerrain typesTerrains[NB_TYPES_TERRAINS])
{
    printf("\n affichage du typesTerrains:\n");
    for(int i=0; i<nb_lignes3; i++)
    {
        printf("%d %c %d %s \n",typesTerrains[i].idTypeTerrain,typesTerrains[i].symboleTerrain,typesTerrains[i].codeAffichageTerrain,typesTerrains[i].nomTerrain);
    }

}

void afficherPeriodes(int nbLignes3, Periode periodes[MAX_LIGNES_PERIODES])
{
    printf("affichage des periodes:\n");
    for(int i=0; i<nbLignes3; i++)
    {
        printf("%d %s %d %d %d %d \n",periodes[i].numOrdre,periodes[i].moment,periodes[i].bonus[0],periodes[i].bonus[1],periodes[i].bonus[2],periodes[i].bonus[3]);

    }

}

void afficherFichesTypesUnites(int nbFichesTypesUnites, FicheTypeUnite fichesTypesUnites[MAX_LIGNES_FICHES_TYPES_UNITES])
{
    printf("\n affichage fiches unite \n");
    for(int i=0; i<nbFichesTypesUnites; i++)
    {
        printf("\n %d %d %s %d %d %d %d %d %d \n",fichesTypesUnites[i].idFicheTypeUnite,fichesTypesUnites[i].race,fichesTypesUnites[i].nom,fichesTypesUnites[i].prix,fichesTypesUnites[i].pvMax,fichesTypesUnites[i].mouvementsMax,fichesTypesUnites[i].xpRequise,fichesTypesUnites[i].niveau,fichesTypesUnites[i].alignement);
        for(int j=0; j<3; j++)
        {
            printf("%d ",fichesTypesUnites[i].idFichesTypeUniteRangInf[j]);
        }
        printf("\n");
        for(int j=0; j<3; j++)
        {
            printf("%d ",fichesTypesUnites[i].idFichesTypeUniteRangSup[j]);
        }
        printf("\n");
        for(int j=0; j<3; j++)
        {
            printf("%d %s %d %d %d %d %d %d\n",fichesTypesUnites[i].attaques[j].idAttaque,fichesTypesUnites[i].attaques[j].nomAttaque,fichesTypesUnites[i].attaques[j].degat,fichesTypesUnites[i].attaques[j].nombreAttaques,fichesTypesUnites[i].attaques[j].porteeAttaque,fichesTypesUnites[i].attaques[j].typeAttaque,fichesTypesUnites[i].attaques[j].idAttaqueSpeciale1,fichesTypesUnites[i].attaques[j].idAttaqueSpeciale2);
        }

        for(int j=0; j<6; j++)
        {
            printf("%d ",fichesTypesUnites[i].resistances[j]);
        }
        printf("\n");
        for(int j=0; j<17; j++)
        {
            printf("%d ",fichesTypesUnites[i].relationsTerrains[j].coutMouvement);
        }
        printf("\n");
        for(int j=0; j<17; j++)
        {
            printf("%d ",fichesTypesUnites[i].relationsTerrains[j].defense);
        }
        printf("\n");
        for(int j=0; j<3; j++)
        {
            printf("%d ",fichesTypesUnites[i].idCapacites[j]);
        }
        printf("\n");
    }
}

void afficherJoueurs(int nbJoueurs, Joueur joueurs[MAX_LIGNES_JOUEURS])
{
    printf("\n affichage joueurs: \n");
    for(int i=0; i<nbJoueurs; i++)
    {
        printf("%d %c %d %d ",joueurs[i].idJoueur,joueurs[i].symbole,joueurs[i].or,joueurs[i].groupe_allies);
        printf("\n");
    }
}

void afficherUniteJoueur( UniteJoueur unitesJoueurs[MAX_LIGNES_UNITESJOUEURS])
{
    printf(" \n affichage des unites_joueurs :\n");
    for(int i=0; i<MAX_LIGNES_UNITESJOUEURS; i++)
    {
        printf("%d %d %d %s %d %d %d %d ",unitesJoueurs[i].idUnite,unitesJoueurs[i].idFicheTypeUnite,unitesJoueurs[i].idJoueur,unitesJoueurs[i].nomUnite,unitesJoueurs[i].active,unitesJoueurs[i].rang,unitesJoueurs[i].ligne,unitesJoueurs[i].colonne);
        printf("\n");
        for(int j=0; j<3; j++)
        {
            printf("%d ",unitesJoueurs[i].traits[j]);
        }
        printf("\n");
        printf("%d %d %d %d %d %d %d %d \n",unitesJoueurs[i].pv,unitesJoueurs[i].pvMax,unitesJoueurs[i].mouvements,unitesJoueurs[i].mouvementsMax,unitesJoueurs[i].xp,unitesJoueurs[i].xpRequise,unitesJoueurs[i].niveau,unitesJoueurs[i].finTour);
    }
}

void afficherCarte(SDL_Surface *ecran,CelluleCarte carte[NB_LIGNES_CARTE][ NB_COLONNES_CARTE])
{
    printf("affichage de la carte: \n");
    for(int i=0; i<NB_LIGNES_CARTE; i++)
    {
        for(int j=0; j<NB_COLONNES_CARTE; j++)
        {
            printf("%c ",carte[i][j].codeAffichageTerrain);
        }
        printf("\n");
    }

    SDL_Surface *img_tab[18];
    img_tab[0]=IMG_Load("assets/PLAT2.png");;
    img_tab[1]=IMG_Load("assets/CASTLE2.png");
    img_tab[2]=IMG_Load("assets/CAVERNE2.png");
    img_tab[3]=IMG_Load("assets/RECIF2.png");
    img_tab[4]=IMG_Load("assets/EAU_PROFONDE2.png");
    img_tab[5]=IMG_Load("assets/FAUX_VOILE2.png");
    img_tab[6]=IMG_Load("assets/PLAT2.png");
    img_tab[7]=IMG_Load("assets/FORET2.png");
    img_tab[8]=IMG_Load("assets/GELE2.png");
    img_tab[9]=IMG_Load("assets/CHAMPIGNONS2.png");
    img_tab[10]=IMG_Load("assets/COLLINES2.png");
    img_tab[11]=IMG_Load("assets/MONTAGNES2.png");
    img_tab[12]=IMG_Load("assets/SABLE2.png");
    img_tab[13]=IMG_Load("assets/EAU_PEU_PROFONDE2.png");
    img_tab[14]=IMG_Load("assets/MARAIS2.png");
    img_tab[15]=IMG_Load("assets/IMPARTICABLE2.png");
    img_tab[16]=IMG_Load("assets/VILLAGE2.png");
    img_tab[17]=IMG_Load("assets/DONJON2.png");

    SDL_FillRect(ecran, NULL, SDL_MapRGB(ecran->format, 0, 0, 0));

    int indice_image=1;
    SDL_Rect positionIMG;
    positionIMG.x = 0;
    positionIMG.y = 0;
    for(int i=0; i<NB_LIGNES_CARTE; i++)
    {

        for(int j=0; j<NB_COLONNES_CARTE; j++)
        {

            indice_image=carte[i][j].idTypeTerrain;
            SDL_Surface *img;
            img=img_tab[indice_image];
            SDL_Rect positionIMG;
            positionIMG.x=j*25;
            positionIMG.y=i*25;
            SDL_BlitSurface(img, NULL, ecran, &positionIMG);



        }
    }


}

void afficherUnitesMagasinUnJoueur(int idJoueur, int nbUnitesMagasin, int nbFichesTypesUnites,UniteMagasin unitesMagasin[MAX_LIGNES_UNITESMAGASIN],FicheTypeUnite fichesTypesUnites[MAX_LIGNES_FICHES_TYPES_UNITES])
{

    printf("                                           Liste des unites disponibles pour achat :                            \n\n");
    printf("*** UNITES DISPONIBLES POUR ACHAT DANS LE MAGASIN***\n");
    char ch[50];
    printf("---------------------------------------------------------------------------------------------------------------\n");
    printf("|  idUnite |   race   |        type        |   prix   |  pvMax   | mvtsMax  |xpRequise |  niveau  |alignement |\n");
    printf("---------------------------------------------------------------------------------------------------------------\n");


    for(int i=0; i<nbUnitesMagasin; i++)
    {
        if(unitesMagasin[i].idJoueurAutorise==idJoueur)
        {
            int indice =chercherIndiceFicheTypeUnite(unitesMagasin[i].idFicheTypeUnite,nbFichesTypesUnites,fichesTypesUnites);
            strcpy(ch,fichesTypesUnites[indice].nom);
            for(int i=strlen(ch); i<20; i++)
            {
                ch[i]=' ';
            }
            ch[strlen(ch)]='\0';

            printf("|    %2d    |    %2d    |%s|    %2d    |    %2d    |    %2d    |   %3d    |     %1d    |     %1d     |\n",unitesMagasin[i].idUniteMagasin,fichesTypesUnites[indice].race,ch,fichesTypesUnites[indice].prix,fichesTypesUnites[indice].pvMax,fichesTypesUnites[indice].mouvementsMax,fichesTypesUnites[indice].xpRequise,fichesTypesUnites[indice].niveau,fichesTypesUnites[indice].alignement);
            printf("---------------------------------------------------------------------------------------------------------------\n");
        }

    }
}

void sauvegarderVillages(int nb_lignes2, Village villages[MAX_LIGNES_VILLAGES], char* Unite_village_sauvegarde)
{

    FILE *ptr_villages_sauvegarde;
    ptr_villages_sauvegarde=fopen(Unite_village_sauvegarde,"w");
    if(ptr_villages_sauvegarde==NULL)
    {
        printf("FILE not found!!!");
    }
    int i=0;
    while(i<=nb_lignes2)
    {
        fprintf(ptr_villages_sauvegarde,"%d %d %d %d \n",villages[i].idVillage,villages[i].ligne,villages[i].colonne,villages[i].idJoueurProprietaire);
        i++;
    }
    fclose(ptr_villages_sauvegarde);
}

void sauvegarderJoueurs(int nbJoueurs, Joueur joueurs[MAX_LIGNES_JOUEURS], char* joueurs_sauvegarde)
{
    FILE *fich_jsauv;
    fich_jsauv=fopen(joueurs_sauvegarde,"w");

    for(int i=0; i<nbJoueurs; i++)
    {
        fprintf(fich_jsauv,"%d %c %d %d\n",joueurs[i].idJoueur,joueurs[i].symbole,joueurs[i].or,joueurs[i].groupe_allies);
    }
    fclose(fich_jsauv);
}

void sauvegarderUniteJoueur(UniteJoueur unitesJoueurs[MAX_LIGNES_UNITESJOUEURS], char*unitesJoueurs_sauvegarde)
{
    FILE *fich_ujs;
    fich_ujs=fopen(unitesJoueurs_sauvegarde,"w");

    for(int i=0; i<MAX_LIGNES_UNITESJOUEURS; i++)
    {

        fprintf(fich_ujs,"%d %d %d %s %d %d %d %d\n",unitesJoueurs[i].idUnite,unitesJoueurs[i].idFicheTypeUnite,unitesJoueurs[i].idJoueur,unitesJoueurs[i].nomUnite,unitesJoueurs[i].active,unitesJoueurs[i].rang,unitesJoueurs[i].ligne,unitesJoueurs[i].colonne);
        fprintf(fich_ujs,"%d %d %d\n",unitesJoueurs[i].traits[0],unitesJoueurs[i].traits[1],unitesJoueurs[i].traits[2]);


    }

    fclose(fich_ujs);
}


void initialiserNouveauJeu(int *nbfichesTypesUnites, int *nbJoueurs, int *nbUnitesMagasin,int *nbVillages,FicheTypeUnite fichesTypesUnites[MAX_LIGNES_FICHES_TYPES_UNITES],Joueur joueurs[MAX_LIGNES_JOUEURS],CelluleCarte carte[NB_LIGNES_CARTE][NB_COLONNES_CARTE],UniteMagasin unitesMagasin[MAX_LIGNES_UNITESMAGASIN],Village villages[MAX_LIGNES_VILLAGES],TypeTerrain typesTerrains[NB_TYPES_TERRAINS],Periode periodes[NB_LIGNES_PERIODES],UniteJoueur unitesJoueurs[MAX_LIGNES_UNITESJOUEURS])
{

    char unitesmagasin[50]="assets/unitesMagasin.txt";
    char unitesMagasin_original_sauvegarde[50]="assets/unitesMagasin_sauvegarde.txt";
    char Unite_village[50]="assets/villages.txt";
    char Unite_village_sauvegarde[50]="assets/villages_sauvegarde.txt";
    char typesTerrains_original[50]="assets/typesTerrains.txt";
    char typesTerrains_original_sauvegarde[50]="assets/typesTerrains_sauvegarde.txt";
    char periodes_original[50]="assets/periodes.txt";
    char periodes_original_sauvegarde[50]="assets/periodes_sauvegarde.txt";
    char joueurs_original[50]="assets/joueurs.txt";
    char fiches_Unites[50]="assets/fichesUnites.txt";
    char carte_original[50]="assets/carte.txt";
    char unite_joueurs_original[50]="assets/unitesJoueurs.txt";
    char unite_joueurs_sauvegarde[50]="assets/unitesJoueurs_sauvegarde.txt";
    char joueurs_sauvegarde[50]="assets/Joueur_sauvegarde.txt";


    *nbVillages=chargerVillagesVersTableau(villages,Unite_village);
    *nbUnitesMagasin=chargerUnitesMagasinVersTableau(unitesMagasin,unitesmagasin);
    chargerTypesTerrainsVersTableau(typesTerrains,typesTerrains_original);
    chargerPeriodesVersTableau(periodes,periodes_original);
    chargerCarteVersTableau(carte,typesTerrains,carte_original);
    *nbfichesTypesUnites=chargerFichesTypesUnitesVersTableau(fichesTypesUnites,fiches_Unites);
    *nbJoueurs=chargerJoueursVersTableau(joueurs,joueurs_original);
    chargerUniteJoueurVersTableau(unitesJoueurs,unite_joueurs_original);
    placerUnitesDansCarte(unitesJoueurs,*nbJoueurs,joueurs,carte);
    completerInitialisationJoueurs(*nbJoueurs,*nbVillages,joueurs,villages,unitesJoueurs);
    completerInitialisationUnitesJoueurs(unitesJoueurs,*nbfichesTypesUnites,fichesTypesUnites);
}

void chargerJeuComplet(int *nbFichesTypesUnites, int *nbJoueurs, int *nbUnitesMagasin,int *nbVillages,FicheTypeUnite fichesTypesUnites[MAX_LIGNES_FICHES_TYPES_UNITES],Joueur joueurs[MAX_LIGNES_JOUEURS],CelluleCarte carte[NB_LIGNES_CARTE][NB_COLONNES_CARTE],UniteMagasin unitesMagasin[MAX_LIGNES_UNITESMAGASIN],Village villages[MAX_LIGNES_VILLAGES],TypeTerrain typesTerrains[NB_TYPES_TERRAINS],Periode periodes[NB_LIGNES_PERIODES],UniteJoueur unitesJoueurs[MAX_LIGNES_UNITESJOUEURS])
{


    char unitesmagasin[50]="assets/unitesMagasin.txt";
    char unitesMagasin_original_sauvegarde[50]="assets/unitesMagasin_sauvegarde.txt";
    char Unite_village[50]="assets/villages.txt";
    char Unite_village_sauvegarde[50]="assets/villages_sauvegarde.txt";
    char typesTerrains_original[50]="assets/typesTerrains.txt";
    char typesTerrains_original_sauvegarde[50]="assets/typesTerrains_sauvegarde.txt";
    char periodes_original[50]="assets/periodes.txt";
    char periodes_original_sauvegarde[50]="assets/periodes_sauvegarde.txt";
    char joueurs_original[50]="assets/joueurs.txt";
    char fiches_Unites[50]="assets/fichesUnites.txt";
    char carte_original[50]="assets/carte.txt";
    char unite_joueurs_original[50]="assets/unitesJoueurs.txt";
    char unite_joueurs_sauvegarde[50]="assets/unitesJoueurs_sauvegarde.txt";
    char joueurs_sauvegarde[50]="assets/Joueur_sauvegarde.txt";

    chargerTypesTerrainsVersTableau(typesTerrains,typesTerrains_original);
    chargerCarteVersTableau(carte,typesTerrains,"assets/carte.txt");

    *nbFichesTypesUnites=chargerFichesTypesUnitesVersTableau(fichesTypesUnites,fiches_Unites);

    chargerPeriodesVersTableau(periodes,periodes_original);

    *nbJoueurs=chargerJoueursVersTableau(joueurs,joueurs_sauvegarde);
    chargerUnitesJoueursSauvegardeesVersTableau(unitesJoueurs,unite_joueurs_original);
    //chargerUniteJoueurVersTableau(unitesJoueurs,unite_joueurs_original);

    *nbVillages=chargerVillagesVersTableau(villages,Unite_village_sauvegarde);

    *nbUnitesMagasin=chargerUnitesMagasinVersTableau(unitesMagasin,unitesmagasin);


    placerUnitesDansCarte(unitesJoueurs,*nbJoueurs,joueurs,carte);
    completerInitialisationJoueurs(*nbJoueurs,*nbVillages,joueurs,villages,unitesJoueurs);
    completerInitialisationUnitesJoueurs(unitesJoueurs,*nbFichesTypesUnites,fichesTypesUnites);
}

void afficherTableauxJeu(int nbLignesFichesTypesUnites, int nbLignesJoueurs,int nbLignesUnitesMagasin, int nbLignesVillages,FicheTypeUnite fichesTypesUnites[MAX_LIGNES_FICHES_TYPES_UNITES],Joueur joueurs[MAX_LIGNES_JOUEURS],CelluleCarte carte[NB_LIGNES_CARTE][NB_COLONNES_CARTE],UniteMagasin unitesMagasin[MAX_LIGNES_UNITESMAGASIN],Village villages[MAX_LIGNES_VILLAGES],TypeTerrain typesTerrains[NB_TYPES_TERRAINS],Periode periodes[NB_LIGNES_PERIODES],UniteJoueur unitesJoueurs[MAX_LIGNES_UNITESJOUEURS])
{
    printf("nombre des fiches unites = %d\n nombre des joueurs :%d \n nombre des unites magasin=%d \n nombre des villages= %d\n",nbLignesFichesTypesUnites,nbLignesJoueurs,nbLignesUnitesMagasin,nbLignesVillages);
    afficherTypesTerrains(NB_TYPES_TERRAINS,typesTerrains);
    afficherUnitesMagasin(nbLignesUnitesMagasin,unitesMagasin);
    afficherVillages(nbLignesVillages,villages);
    afficherPeriodes(NB_LIGNES_PERIODES,periodes);
    afficherJoueurs(nbLignesJoueurs,joueurs);
    afficherFichesTypesUnites(nbLignesFichesTypesUnites,fichesTypesUnites);
    afficherUniteJoueur(unitesJoueurs);
    // afficherCarte(carte);
}

void sauvegarderJeuComplet(int nbLignesVillages, int nbLignesJoueurs,Village villages[MAX_LIGNES_VILLAGES],Joueur joueurs[MAX_LIGNES_JOUEURS],UniteJoueur unitesJoueurs[MAX_LIGNES_UNITESJOUEURS])
{

    sauvegarderJoueurs(nbLignesJoueurs,joueurs,"assets/Joueur_sauvegarde.txt");
    sauvegarderVillages(nbLignesVillages,villages,"assets/villages_sauvegarde.txt");
    sauvegarderUniteJoueur(unitesJoueurs,"assets/unitesJoueurs_sauvegarde.txt");
}

void afficherJoueurJeu(int idJoueur,int nbJoueurs,Joueur joueurs[MAX_LIGNES_JOUEURS])
{
    printf("\n***JOUEUR***\n");
    int pos=chercherIndiceJoueur(idJoueur,nbJoueurs,joueurs);
    printf("-------------------------------------------------------------------------------------------\n");
    printf("| idJoueur | symbole  |   or     | grpallies |nbVillages| nbUnites |entretien |   revenu  |\n");
    printf("-------------------------------------------------------------------------------------------\n");
    printf("|   %3d    |   %c      |   %3d    |   %3d    |   %3d     |   %3d    |   %3d    |   %3d     |\n",joueurs[pos].idJoueur,joueurs[pos].symbole,joueurs[pos].or,joueurs[pos].groupe_allies,joueurs[pos].nombreVillages,joueurs[pos].nombreUnites,joueurs[pos].entretien,joueurs[pos].revenu);
    printf("-------------------------------------------------------------------------------------------\n");
}

void afficherUnitesJoueursJeu(int idJoueur,int active,UniteJoueur unitesJoueurs[MAX_LIGNES_UNITESJOUEURS])
{
    char ch[50];
    char ch2[10]="          ";
    printf("\n***UNITES***\n");
    printf("------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ \n");
    printf("|  idUnite |idFichTyun| idJoueur | nomUnite |  active  |   rang    |  ligne   | colonne  |  trait1  |  trait2  |  trait3  |    pv    |  pvMax   |   mvts   | MvtsMax  |    xp    |xpRequise |  niveau  |alignement|  finTour | \n");
    printf("------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
    for(int i=0; i<MAX_LIGNES_UNITESJOUEURS; i++)
    {
        if(unitesJoueurs[i].idJoueur==idJoueur && unitesJoueurs[i].active )
        {
            strcpy(ch,unitesJoueurs[i].nomUnite);
            if(strlen(ch)<10)
            {
                strncat(ch,ch2,10-strlen(ch));
            }
            ch[strlen(ch)]='\0';
            printf("|   %3d    |   %3d    |   %3d    |%s|    %1d     |     %1d     |    %2d    |    %2d    |    %2d    |   %2d     |   %2d     |   %3d    |   %3d    |   %3d    |  %3d     |   %3d    |   %3d    |   %3d    |   %3d    |   %3d    | \n",unitesJoueurs[i].idUnite,unitesJoueurs[i].idFicheTypeUnite,unitesJoueurs[i].idJoueur,ch,unitesJoueurs[i].active,unitesJoueurs[i].rang,unitesJoueurs[i].ligne,unitesJoueurs[i].colonne,unitesJoueurs[i].traits[0],unitesJoueurs[i].traits[1],unitesJoueurs[i].traits[2],unitesJoueurs[i].pv,unitesJoueurs[i].pvMax,unitesJoueurs[i].mouvements,unitesJoueurs[i].mouvementsMax,unitesJoueurs[i].xp,unitesJoueurs[i].xpRequise,unitesJoueurs[i].niveau,unitesJoueurs[i].alignement,unitesJoueurs[i].finTour);
            printf("------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ \n");

        }
    }
}

void afficherCarteJeu(SDL_Surface *ecran, SDL_Surface *img_tab[], SDL_Surface *characters[], CelluleCarte carte[NB_LIGNES_CARTE][NB_COLONNES_CARTE], int nbJoueurs, Joueur joueurs[MAX_LIGNES_JOUEURS], int nbVillages, Village villages[MAX_LIGNES_VILLAGES], UniteJoueur unitesJoueurs[MAX_LIGNES_UNITESJOUEURS])
{
    // ... (rest of your function remains the same, use img_tab and characters as needed)



    /*  printf("affichage de la carte: \n");
      for(int i=0; i<NB_LIGNES_CARTE; i++)
      {
          for(int j=0; j<NB_COLONNES_CARTE; j++)
          {
              printf("%c ",carte[i][j].codeAffichageTerrain);
          }
          printf("\n");
      }*/
    SDL_FillRect(ecran, NULL, SDL_MapRGB(ecran->format, 0, 0, 0));
    SDL_Surface *img_or,*shop,*fin_tour,*nb_village,*unites ;
    SDL_Color white = { 255, 255, 255 };
    SDL_Color couleur_a_utiliser= white;
    img_or= IMG_Load("assets/gold.png");
    shop=IMG_Load("assets/shop.png");
    SDL_Surface* texte = NULL;
    TTF_Font* font = NULL;
    font = TTF_OpenFont("assets/Oswald-VariableFont_wght.ttf", 25);
    SDL_FillRect(ecran, NULL, SDL_MapRGB(ecran->format, 0, 0, 0));
    SDL_Surface* character_bg = IMG_Load("assets/character_bg.jpg");
    fin_tour = IMG_Load("assets/fin_tour.png");
    int indice_image=1;
    SDL_Rect positionIMG;
    positionIMG.x = 950;
    positionIMG.y = 0;
    SDL_BlitSurface(character_bg, NULL, ecran, &positionIMG);
    positionIMG.x = 1000;
    positionIMG.y = 630;
    SDL_BlitSurface(fin_tour, NULL, ecran, &positionIMG);
  SDL_Rect positionTitre;
    char fin_tour_ph[50]="Fin Tour";
    int nb_Tour  =  Tour  ;
    char  rem [100] ;
    sprintf(rem,"%d",nb_Tour) ;
    char  tour[50]  ;
    strcpy(tour,"Tour :");
    strcat(tour,rem) ;
    texte = TTF_RenderText_Blended(font,tour, couleur_a_utiliser) ;
    positionTitre.x=1050;
    positionTitre.y=580;
    SDL_BlitSurface(texte, NULL, ecran, &positionTitre);
    texte = TTF_RenderText_Blended(font,fin_tour_ph, couleur_a_utiliser) ;

    positionTitre.x=1050;
    positionTitre.y=635;
    SDL_BlitSurface(texte, NULL, ecran, &positionTitre);
    int  nb_village_joueur = joueurs[0].nombreVillages;
    char village_occupied[50]="";
    char  x[50];
    sprintf(x,"%d",nb_village_joueur);
    strcat(village_occupied,x);
    strcat(village_occupied,"/");
    sprintf(x,"%d",MAX_LIGNES_VILLAGES);
    strcat(village_occupied,x);
    positionTitre.x=980;
    positionTitre.y=60;
    texte = TTF_RenderText_Blended(font,village_occupied, couleur_a_utiliser) ;
    SDL_BlitSurface(texte, NULL, ecran, &positionTitre);
    nb_village= IMG_Load("assets/capture.png");
    positionIMG.x=1000;
    positionIMG.y=0;
    SDL_BlitSurface(nb_village, NULL, ecran, &positionIMG);
    int  nb_unite =  joueurs[0].nombreUnites;
    char  nb_unites_joueur[50] ="" ,  remp[50]="";
    sprintf(remp,"%d",nb_unite);
    strcat(nb_unites_joueur,remp);
    strcat(nb_unites_joueur,"/");
    nb_unite+=joueurs[1].nombreUnites;
    sprintf(remp,"%d",nb_unite);
    strcat(nb_unites_joueur,remp);
    texte = TTF_RenderText_Blended(font,nb_unites_joueur, couleur_a_utiliser);
    positionIMG.x=1090;
    positionIMG.y=60;
    SDL_BlitSurface(texte, NULL, ecran, &positionIMG);
    unites=IMG_Load("assets/unites.png");
    positionIMG.x = 1150;
    positionIMG.y = 50;
    SDL_BlitSurface(unites, NULL, ecran, &positionIMG);

    char gold[10]="Or : ";
    sprintf(remp,"%d",joueurs[0].or);
    strcat(gold,remp);
    positionIMG.x = 980;
    positionIMG.y=10;
    texte = TTF_RenderText_Blended(font,gold, couleur_a_utiliser);
    SDL_BlitSurface(texte, NULL, ecran, &positionIMG);
    positionIMG.x+=70;
    SDL_BlitSurface(img_or, NULL, ecran, &positionIMG);
    char revenue[20]="revenue : ";
    sprintf(remp,"%d",joueurs[0].revenu);
    strcat(revenue,remp);
    positionIMG.x+=50;
    texte = TTF_RenderText_Blended(font,revenue, couleur_a_utiliser);
    SDL_BlitSurface(texte, NULL, ecran, &positionIMG);
    /*SDL_Surface *img_revenue =IMG_Load("assets/revenue.png");
    positionIMG.x+=80;
    SDL_BlitSurface(img_revenue, NULL, ecran, &positionIMG);*/


SDL_Surface *flag =IMG_Load("assets/flag.png"),*flag2=IMG_Load("assets/flag2.png");
    positionIMG.x = 0;
    positionIMG.y = 0;
    for(int i=0; i<NB_LIGNES_CARTE; i++)
    {

        for(int j=0; j<NB_COLONNES_CARTE; j++)
        {

            indice_image=carte[i][j].idTypeTerrain;
            SDL_Surface *img;
            img=img_tab[indice_image];
            SDL_Rect positionIMG;
            positionIMG.x=j*25;
            positionIMG.y=i*25;
            SDL_BlitSurface(img, NULL, ecran, &positionIMG);
            if(carte[i][j].idTypeTerrain==16 )
            {
                int indice  = chercherIndiceVillage(i,j,nbVillages,villages);

                if(villages[indice].idJoueurProprietaire==1)
                {

                 positionIMG.x=j*25-10;
                 SDL_BlitSurface(flag, NULL, ecran, &positionIMG);

                }
                if( villages[indice].idJoueurProprietaire==2)
                {
                    positionIMG.x=j*25-10;
                 SDL_BlitSurface(flag2, NULL, ecran, &positionIMG);
                }

            }



        }
    }

    for(int i=0; i<NB_LIGNES_CARTE; i++)
    {

        for(int j=0; j<NB_COLONNES_CARTE; j++)
        {

            if(carte[i][j].idUnite!=0)
            {

                int indice  =  carte[i][j].idUnite-1 ;
                if(unitesJoueurs[indice].idJoueur==2 && unitesJoueurs[indice].rang ==1 )
                {
                    SDL_Surface *img;
                    img=characters[1];
                    SDL_Rect positionIMG;
                    positionIMG.x=j*25;
                    positionIMG.y=i*25;
                    SDL_BlitSurface(img, NULL, ecran, &positionIMG);



                }
                else
                {
                    if(unitesJoueurs[indice].idJoueur==1 &&  unitesJoueurs[indice].rang==1)
                    {


                        SDL_Surface *img;
                        img=characters[0];
                        SDL_Rect positionIMG;
                        positionIMG.x=j*25;
                        positionIMG.y=i*25;
                        SDL_BlitSurface(img, NULL, ecran, &positionIMG);

                    }
                    else
                    {
                        if(unitesJoueurs[indice].idJoueur==2 && unitesJoueurs[indice].rang==0)
                        {
                            SDL_Surface *img;
                            img=characters[2];
                            SDL_Rect positionIMG;
                            positionIMG.x=j*25;
                            positionIMG.y=i*25;
                            SDL_BlitSurface(img, NULL, ecran, &positionIMG);

                        }
                        else
                        {

                            SDL_Surface *img;
                            img=characters[3];
                            if(unitesJoueurs[indice].idFicheTypeUnite==104)
                            {
                                img=characters[4];
                            }
                            if(unitesJoueurs[indice].idFicheTypeUnite==63)
                            {
                                img=characters[6];
                            }
                            if(unitesJoueurs[indice].idFicheTypeUnite==65)
                            {
                                img=characters[7];
                            }
                            if(unitesJoueurs[indice].idFicheTypeUnite==185)
                            {
                                img=characters[5];
                            }
                            SDL_Rect positionIMG;
                            positionIMG.x=j*25;
                            positionIMG.y=i*25;
                            SDL_BlitSurface(img, NULL, ecran, &positionIMG);

                        }
                    }
                }
            }

        }
    }
    int TAILLE_BLOC = 25;
     SDL_Surface *Case_a_remplir = NULL;
                Case_a_remplir = SDL_CreateRGBSurface(SDL_HWSURFACE, TAILLE_BLOC, TAILLE_BLOC, 32, 0, 0, 0, 0);
                SDL_FillRect(Case_a_remplir, NULL, SDL_MapRGB(ecran->format, 255, 0, 0));
                SDL_SetAlpha(Case_a_remplir, SDL_SRCALPHA, 100);
    for(int i=0; i<NB_LIGNES_CARTE; i++)
    {

        for(int j=0; j<NB_COLONNES_CARTE; j++)
        {

            if(carte[i][j].marque)
            {
                SDL_Surface *img;
                img=img_tab[18];
                SDL_Rect positionIMG;
                positionIMG.x=j*25;
                positionIMG.y=i*25;
                SDL_BlitSurface(Case_a_remplir, NULL, ecran, &positionIMG);
            }
        }
    }

    SDL_Flip(ecran);






    int L=0;
    int C=0;
    int pos=chercherIndiceJoueurHumain(nbJoueurs,joueurs);
    for(int i=0; i<nbJoueurs; i++)
    {
        if(joueurs[i].groupe_allies!=joueurs[pos].groupe_allies)
        {
            afficherJoueurJeu(joueurs[i].idJoueur,nbJoueurs,joueurs);
            afficherUnitesJoueursJeu(joueurs[i].idJoueur,1,unitesJoueurs);
        }
    }
    printf("\n  ");
    for(int i=0; i<NB_COLONNES_CARTE; i++)
    {
        printf("  %2d  ",i+1);
    }
    printf("\n");
    printf("   -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
    for(int i=0; i<NB_LIGNES_CARTE; i++)
    {
        printf("%2d ",i+1);
        for(int j=0; j<NB_COLONNES_CARTE; j++)
        {

            if(carte[i][j].idTypeTerrain==ID_TYPE_TERRAIN_VILLAGE)
            {
                if(carte[i][j].marque==1)
                {
                    int indice=chercherIndiceVillage(i,j,nbVillages,villages);
                    printf("|%c%-1d ++",carte[i][j].codeAffichageTerrain,villages[indice].idJoueurProprietaire);
                }
                else
                {
                    int indice=chercherIndiceVillage(i,j,nbVillages,villages);
                    if( carte[i][j].idUnite!=0)
                    {
                        printf("|%c%1d%c%-2d",carte[i][j].codeAffichageTerrain,villages[indice].idJoueurProprietaire,carte[i][j].symboleJoueur,carte[i][j].idUnite);
                    }
                    else
                    {
                        printf("|%c%-1d   ",carte[i][j].codeAffichageTerrain,villages[indice].idJoueurProprietaire);

                    }
                }
            }
            else
            {
                if(carte[i][j].marque==1)
                {
                    printf("|%c ++ ",carte[i][j].codeAffichageTerrain);
                }
                else
                {
                    if( carte[i][j].idUnite!=0)
                    {
                        printf("|%c %c%-2d",carte[i][j].codeAffichageTerrain,carte[i][j].symboleJoueur,carte[i][j].idUnite);
                    }
                    else
                    {
                        printf("|%c    ",carte[i][j].codeAffichageTerrain);
                    }
                }
            }
        }
        printf("|\n");
        printf("   -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
    }

    for(int i=0; i<nbJoueurs; i++)
    {
        if(joueurs[i].groupe_allies==joueurs[pos].groupe_allies)
        {
            afficherJoueurJeu(joueurs[i].idJoueur,nbJoueurs,joueurs);
            afficherUnitesJoueursJeu(joueurs[i].idJoueur,1,unitesJoueurs);
        }
    }

}

void marquerDeplacementsPossibles(CelluleCarte carte[NB_LIGNES_CARTE][NB_COLONNES_CARTE],int ligneCourante, int colonneCourante, int mouvements)
{

    int LigneMin=maximum(0,ligneCourante-mouvements),LigneMax=minimum(ligneCourante+mouvements,NB_LIGNES_CARTE-1);
    int ColMin=maximum(0,colonneCourante-mouvements),ColMax=minimum(colonneCourante+mouvements,NB_COLONNES_CARTE-1);

    for(int i=LigneMin; i<=LigneMax; i++)
    {
        for(int j=ColMin; j<=ColMax; j++)
        {
            if(abs(i-ligneCourante)+abs(j-colonneCourante)<=mouvements && carte[i][j].idUnite==0 && carte[i][j].idTypeTerrain!=4 )
            {
                carte[i][j].marque=1;
            }
        }
    }

}

void effacerDeplacementsPossibles(CelluleCarte carte[NB_LIGNES_CARTE][NB_COLONNES_CARTE])
{
    for(int i=0; i<NB_LIGNES_CARTE; i++)
    {
        for(int j=0; j<NB_COLONNES_CARTE; j++)
        {
            carte[i][j].marque=0;
        }
    }
}

void deplacerUnite(int numJoueur, int indiceDansUnitesJoueurs, int numUnite,int ligne_courante, int colonne_courante, int nouvelle_ligne, int nouvelle_colonne,int nbJoueurs, int nbVillages,CelluleCarte carte[NB_LIGNES_CARTE][NB_COLONNES_CARTE],Joueur joueurs[MAX_LIGNES_JOUEURS],UniteJoueur unitesJoueurs[MAX_LIGNES_UNITESJOUEURS],Village villages[MAX_LIGNES_VILLAGES])
{
    int pos=chercherIndiceVillage(nouvelle_ligne,nouvelle_colonne,nbVillages,villages);

    if(carte[nouvelle_ligne][nouvelle_colonne].idTypeTerrain==ID_TYPE_TERRAIN_VILLAGE )
    {

        if(villages[pos].idJoueurProprietaire!=0)
        {
            int posJ=chercherIndiceJoueur(villages[pos].idJoueurProprietaire,nbJoueurs,joueurs);

            if(joueurs[posJ].idJoueur!=joueurs[numJoueur].idJoueur)
            {
                villages[pos].idJoueurProprietaire=joueurs[numJoueur].idJoueur;
                joueurs[posJ].nombreVillages--;
                miseAJourJoueurApresOccupationVillage(posJ,joueurs);
                chercherIndiceJoueur(numJoueur,nbJoueurs,joueurs);
                joueurs[numJoueur].nombreVillages++;
                miseAJourJoueurApresOccupationVillage(numJoueur,joueurs);
            }
        }
        else
        {
            villages[pos].idJoueurProprietaire=joueurs[numJoueur].idJoueur;
            int posJ=chercherIndiceJoueur(numJoueur,nbJoueurs,joueurs);
            joueurs[numJoueur].nombreVillages++;
            miseAJourJoueurApresOccupationVillage(numJoueur,joueurs);
        }

    }
    if(unitesJoueurs[indiceDansUnitesJoueurs].mouvements==0)
    {
        printf("le tour de l'unité a fini le mouvgements =0 \n");
    }
    carte[nouvelle_ligne][nouvelle_colonne].idUnite=numUnite;
    carte[nouvelle_ligne][nouvelle_colonne].symboleJoueur=joueurs[numJoueur].symbole;
    carte[nouvelle_ligne][nouvelle_colonne].marque=0;
    carte[ligne_courante][colonne_courante].idUnite=0;
    carte[ligne_courante][colonne_courante].symboleJoueur=' ';
    unitesJoueurs[indiceDansUnitesJoueurs].colonne = nouvelle_colonne;
    unitesJoueurs[indiceDansUnitesJoueurs].ligne = nouvelle_ligne;
    int mouvements = unitesJoueurs[indiceDansUnitesJoueurs].mouvements;

    int mouvementsRestants = mouvements - (abs(ligne_courante - nouvelle_ligne) + abs(colonne_courante - nouvelle_colonne));
    if (mouvementsRestants < 0)
    {
        mouvementsRestants = 0;
    }
    unitesJoueurs[indiceDansUnitesJoueurs].mouvements=mouvementsRestants ;



}

void miseAJourJoueurApresAchatUnite(int indiceUniteJoueur, int indiceJoueur,int indiceFicheTypeUnite,UniteJoueur unitesJoueurs[MAX_LIGNES_UNITESJOUEURS],Joueur joueurs[MAX_LIGNES_JOUEURS],FicheTypeUnite fichesTypesUnites[MAX_LIGNES_FICHES_TYPES_UNITES])
{
    joueurs[indiceJoueur].or-=fichesTypesUnites[indiceFicheTypeUnite].prix;
    joueurs[indiceJoueur].nombreUnites++;
    if(!possedeTrait(TRAIT_FIDELE,unitesJoueurs[indiceUniteJoueur].traits))
    {
        joueurs[indiceJoueur].entretien+=unitesJoueurs[indiceUniteJoueur].niveau;
    }
}

int initialiserNouvelleUniteJoueur(int indiceUniteJoueur, int indiceFicheTypeUnite, int idJoueur,int ligne, int colonne,UniteJoueur unitesJoueurs[MAX_LIGNES_UNITESJOUEURS],FicheTypeUnite fichesTypesUnites[MAX_LIGNES_FICHES_TYPES_UNITES])
{
    unitesJoueurs[indiceUniteJoueur].idUnite=chercherMaxIdUnitesJoueurs(unitesJoueurs)+1;
    unitesJoueurs[indiceUniteJoueur].idFicheTypeUnite=fichesTypesUnites[indiceFicheTypeUnite].idFicheTypeUnite;
    unitesJoueurs[indiceUniteJoueur].idJoueur=idJoueur;
    char ch[10][20];
    char name[20];
    strcpy(ch[0], "jhon      ");
    strcpy(ch[1], "michael   ");
    strcpy(ch[2], "James     ");
    strcpy(ch[3], "Nathan    ");
    strcpy(ch[4], "Alexander ");
    strcpy(ch[5], "Anthony   ");
    strcpy(ch[6], "Sebastian ");
    strcpy(ch[7], "Jacob     ");
    strcpy(ch[8], "Matthew   ");
    strcpy(ch[9], "Christopher");

    int num_names=10;
    int random_index = rand() % num_names;
    strcpy(unitesJoueurs[indiceUniteJoueur].nomUnite,ch[random_index]);
    unitesJoueurs[indiceUniteJoueur].active=1;
    unitesJoueurs[indiceUniteJoueur].rang=0;
    unitesJoueurs[indiceUniteJoueur].ligne=ligne;
    unitesJoueurs[indiceUniteJoueur].colonne=colonne;
    int random_trait=rand()%MAX_ID_TRAITS;
    unitesJoueurs[indiceUniteJoueur].traits[0]=random_trait;
    random_trait=rand()%MAX_ID_TRAITS;
    unitesJoueurs[indiceUniteJoueur].traits[1]=random_trait;
    int random_trait3=rand()%10;
    random_trait=rand()%MAX_ID_TRAITS;
    if(random_trait3==0)
    {
        unitesJoueurs[indiceUniteJoueur].traits[2]=random_trait;
    }
    else
    {
        unitesJoueurs[indiceUniteJoueur].traits[2]=0;
    }
    unitesJoueurs[indiceUniteJoueur].pv=fichesTypesUnites[indiceFicheTypeUnite].pvMax;
    unitesJoueurs[indiceUniteJoueur].pvMax=fichesTypesUnites[indiceFicheTypeUnite].pvMax;
    unitesJoueurs[indiceUniteJoueur].mouvements=0;
    unitesJoueurs[indiceUniteJoueur].mouvementsMax=fichesTypesUnites[indiceFicheTypeUnite].mouvementsMax;
    unitesJoueurs[indiceUniteJoueur].niveau=fichesTypesUnites[indiceFicheTypeUnite].niveau;
    unitesJoueurs[indiceUniteJoueur].xp=0;
    unitesJoueurs[indiceUniteJoueur].xpRequise=fichesTypesUnites[indiceFicheTypeUnite].xpRequise;
    unitesJoueurs[indiceUniteJoueur].alignement=fichesTypesUnites[indiceFicheTypeUnite].alignement;
    unitesJoueurs[indiceUniteJoueur].finTour=1;
    for(int i=0; i<MAX_EFFETS_ATTAQUES_SUBITES; i++)
    {
        unitesJoueurs[indiceUniteJoueur].effetsAttaquesSubites[i].idAttaqueSpeciale=0;
        unitesJoueurs[indiceUniteJoueur].effetsAttaquesSubites[i].toursRestants=0;
    }
    appliquerTrait(indiceUniteJoueur,unitesJoueurs[indiceUniteJoueur].traits[0],unitesJoueurs);
    appliquerTrait(indiceUniteJoueur,unitesJoueurs[indiceUniteJoueur].traits[1],unitesJoueurs);
    appliquerTrait(indiceUniteJoueur,unitesJoueurs[indiceUniteJoueur].traits[2],unitesJoueurs);




}

void acheterUnites(SDL_Surface *ecran, SDL_Surface *characters[],int idJoueur, int ligneDonjon, int colonneDonjon, int nbUnitesMagasin,int nbJoueurs, int nbFichesTypesUnites, int nbVillages,UniteMagasin unitesMagasin[MAX_LIGNES_UNITESMAGASIN],UniteJoueur unitesJoueurs[MAX_LIGNES_UNITESJOUEURS],Joueur joueurs[MAX_LIGNES_JOUEURS],FicheTypeUnite fichesTypesUnites[MAX_LIGNES_FICHES_TYPES_UNITES],Village villages[MAX_LIGNES_VILLAGES],CelluleCarte carte[NB_LIGNES_CARTE][NB_COLONNES_CARTE])
{
    SDL_Surface *img_bg,*img_or,*unites_bg ;
    SDL_Color blanc = { 255, 255, 255 };
     SDL_Color rouge = { 255, 0, 0 };
    SDL_Color couleur_a_utiliser= rouge;
    img_bg= IMG_Load("assets/magasin.png");
    img_or= IMG_Load("assets/money.png");

    //SDL_FillRect(ecran, NULL, SDL_MapRGB(ecran->format, 255, 255, 255));
    SDL_Rect positionIMG,positionText,positionTitre;
    positionIMG.x=300;
    positionIMG.y=50;
    SDL_BlitSurface(img_bg, NULL, ecran, &positionIMG);
    SDL_Flip(ecran);
    SDL_Surface* texte = NULL;
    TTF_Font* font = NULL;
    font = TTF_OpenFont("assets/DejaVuSans.ttf",15);
    char titre[50];
    strcpy(titre,"recrutez une unité :");
    texte = TTF_RenderText_Blended(font,titre, couleur_a_utiliser) ;
    positionText.x=340;
    positionText.y=70;
    SDL_BlitSurface(texte, NULL, ecran, &positionText);
    unites_bg= IMG_Load("assets/unites_bg.png");
    positionIMG.x=500;
    positionIMG.y=110;
    SDL_BlitSurface(unites_bg, NULL, ecran, &positionIMG);
    SDL_Surface *acheter ,  *annuler  ;
    acheter  =IMG_Load("assets/acheter.png");
    positionIMG.x=580;
    positionIMG.y=650;
    char achat[10]="acheter",annulation[10]="annuler";
    font = TTF_OpenFont("assets/DejaVuSans.ttf",20);
    texte = TTF_RenderText_Blended(font,achat, blanc) ;
    SDL_BlitSurface(acheter, NULL, ecran, &positionIMG);
    positionIMG.x+=32;

    couleur_a_utiliser=blanc;
    SDL_BlitSurface(texte, NULL, ecran, &positionIMG);

    annuler  =IMG_Load("assets/acheter.png");
    positionIMG.x=380;
    positionIMG.y=650;
    SDL_BlitSurface(annuler, NULL, ecran, &positionIMG);
    texte = TTF_RenderText_Blended(font,annulation, blanc) ;
    positionIMG.x+=32;

    couleur_a_utiliser=rouge;
    SDL_BlitSurface(texte, NULL, ecran, &positionIMG);



    font = TTF_OpenFont("assets/DejaVuSans.ttf",15);
    SDL_Surface *character_md[7];
    character_md[0]=IMG_Load("assets/unite2-md.png");
    character_md[1]=IMG_Load("assets/unite3-md.png");
    character_md[2]=IMG_Load("assets/unite4-md.png");
    character_md[3]=IMG_Load("assets/unite5-md.png");
    character_md[4]=IMG_Load("assets/dragoon.png");
    character_md[5]=IMG_Load("assets/ranger.png");
    character_md[6]=IMG_Load("assets/thunderer.png");
    character_md[7]=IMG_Load("assets/dragonguard.png");

    SDL_Flip(ecran);


    SDL_Event event2;
    int continuer2 = 1 ;
    int check = -1 ,indiceOption = 0 ;
    while (continuer2)
    {

        SDL_WaitEvent(&event2);
        switch(event2.type)
        {
        case SDL_MOUSEBUTTONDOWN:

            if (event2.button.button == SDL_BUTTON_LEFT)
            {


                int choix_unite  =   indiceOption;

                SDL_Rect position;

                position.x =  event2.button.x;
                position.y =  event2.button.y;


                if(DoubleClickDetected() == 1)
                {


                   if(position.x>=580 && position.x<=(580+138) && position.y>=650 &&  position.y<=(650+28))
                   {
                    int indice=chercherIndiceChefUnitesJoueurs(idJoueur,unitesJoueurs);
                    int ligne=unitesJoueurs[indice].ligne;
                    int colonne=unitesJoueurs[indice].colonne;
                    int  new_ligne=-1, new_colonne=-1;
                    for(int i = ligne-2 ; i<= ligne+2 ; i++)
                    {

                        for(int  j = colonne-2 ; j<=colonne+2 ; j++)
                        {

                            if(carte[i][j].idUnite==0 && carte[i][j].idTypeTerrain==ID_TYPE_TERRAIN_CHATEAU)
                            {
                                new_ligne =  i;
                                new_colonne = j ;
                            }
                        }
                    }
                    int indice_joueur=chercherIndiceJoueur(idJoueur,nbJoueurs,joueurs);
                    int indice_unite=chercherMaxIdUnitesJoueurs(unitesJoueurs);
                    int indice_fich =chercherIndiceFicheTypeUnite(unitesMagasin[choix_unite].idFicheTypeUnite,nbFichesTypesUnites,fichesTypesUnites);
                    if(joueurs[indice_joueur].or>=fichesTypesUnites[indice_fich].prix)
                    {

                        if(chercherIndiceUniteJoueur(0,0,0,unitesJoueurs)!=-1)
                        {
                            initialiserNouvelleUniteJoueur(indice_unite,indice_fich,idJoueur,new_ligne,new_colonne,unitesJoueurs,fichesTypesUnites);

                            carte[new_ligne][new_colonne].idUnite=unitesJoueurs[indice_unite].idUnite;
                            carte[new_ligne][new_colonne].symboleJoueur=joueurs[indice_joueur].symbole;
                            miseAJourJoueurApresAchatUnite(choix_unite,indice_joueur,indice_fich,unitesJoueurs,joueurs,fichesTypesUnites);
                            //  printf("                               l'achat effectue est reussi");
                            return ;
                        }
                    }
                    else
                    {
                        char msg_error[50] =" Vous n'admet pas assez d'or ! ";
                        texte = TTF_RenderText_Blended(font,msg_error, couleur_a_utiliser) ;
                        positionText.x = 340;
                        positionText.y = 450;
                        SDL_BlitSurface(texte, NULL, ecran, &positionText);
                        SDL_Flip(ecran);
                    }
                   }
                   else{

                       if(position.x>=380 && position.x<=(380+138) && position.y>=650 &&  position.y<=(650+28))
                      {
                          return ;
                      }
                       //positionText.x =  580;
                    //positionText.y = y_depart+40+i*60;




                   }




                }
                else{
                     if(position.x>=580 && position.x<=780 && position.y>=140 && position.y<=370)
                      {
                          indiceOption =  (position.y-140)/60;
                        char msg_error[50] ="";
                        sprintf(msg_error,"%d",indiceOption);
                        texte = TTF_RenderText_Blended(font,msg_error, couleur_a_utiliser) ;
                        positionText.x = 10;
                        positionText.y = 10;
                        SDL_BlitSurface(texte, NULL, ecran, &positionText);
                        SDL_Flip(ecran);

                      }


                }




                break;



            }

            }






          if(font!= 0)
        {
            char phrase[120];


            int i, y_depart=100;
            SDL_Color couleur_a_utiliser;

            for(i=0; i<nbUnitesMagasin; i++)
            {


                if(i==indiceOption)
                {
                    couleur_a_utiliser=rouge ;

                    SDL_Surface *img =character_md[i], *img_unite =IMG_Load("assets/unitinfo.png") ,*img_bottom = IMG_Load("assets/unitinfo_bottom.png"),*img_top = IMG_Load("assets/top_border.png");
                    positionText.x =  330;
                    positionText.y = 100;
                    SDL_BlitSurface(img_unite, NULL, ecran, &positionText);
                    positionText.x =  345;
                    positionText.y = 90;
                    SDL_BlitSurface(img_top, NULL, ecran, &positionText);
                    if(unitesMagasin[i].idFicheTypeUnite==104 && unitesMagasin[i].idJoueurAutorise==1)
                    {
                        img=character_md[4];
                    }
                    if(unitesMagasin[i].idFicheTypeUnite==63 && unitesMagasin[i].idJoueurAutorise==1)
                    {
                        img=character_md[6];
                    }
                    if(unitesMagasin[i].idFicheTypeUnite==65 && unitesMagasin[i].idJoueurAutorise==1)
                    {
                        img=character_md[7];
                    }
                    if(unitesMagasin[i].idFicheTypeUnite==158 && unitesMagasin[i].idJoueurAutorise==1)
                    {
                        img=character_md[5];
                    }

                    positionText.x =  385;
                    positionText.y = 110;
                    SDL_BlitSurface(img, NULL, ecran, &positionText);


                    /* données unite  */

                    int indice_fich =chercherIndiceFicheTypeUnite(unitesMagasin[i].idFicheTypeUnite,nbFichesTypesUnites,fichesTypesUnites);
                    char  name[50];
                    strcpy(name,fichesTypesUnites[indice_fich].nom);
                    couleur_a_utiliser = blanc;
                    positionText.x =350;
                    positionText.y = 180;
                    texte = TTF_RenderText_Blended(font, name, couleur_a_utiliser) ;
                    SDL_BlitSurface(texte, NULL, ecran, &positionText);
                    strcpy(name,"Niv.");
                    char remp[50];
                    sprintf(remp,"%d",fichesTypesUnites[indice_fich].niveau);
                    strcat(name,remp);
                     positionText.x =350;
                    positionText.y = 210;
                    texte = TTF_RenderText_Blended(font, name, couleur_a_utiliser) ;
                    SDL_BlitSurface(texte, NULL, ecran, &positionText);

                    SDL_Surface *race=IMG_Load("assets/humain.png") ,*alignement =IMG_Load("assets/lawful.png");

                    positionIMG.x=400;
                    positionIMG.y=200;
                    SDL_BlitSurface(alignement, NULL, ecran, &positionIMG);
                    positionIMG.x=440;
                    positionIMG.y=200;
                    SDL_BlitSurface(race, NULL, ecran, &positionIMG);

                    strcpy(name,"");
                    strcpy(name,"PV:");
                    sprintf(remp,"%d",fichesTypesUnites[indice_fich].pvMax);
                    strcat(name,remp);
                    couleur_a_utiliser=rouge;
                    strcat(name," | ");
                    strcat(name,"XP:");
                    sprintf(remp,"%d",fichesTypesUnites[indice_fich].xpRequise);
                    strcat(name,remp);
                    texte = TTF_RenderText_Blended(font, name, couleur_a_utiliser) ;
                    positionText.x=350;
                    positionText.y=240;
                    SDL_BlitSurface(texte, NULL, ecran, &positionText);
                    couleur_a_utiliser=blanc;
                    strcpy(name,"Attaques");
                     positionText.x=350;
                    positionText.y=270;
                    texte = TTF_RenderText_Blended(font, name, couleur_a_utiliser) ;
                    SDL_BlitSurface(texte, NULL, ecran, &positionText);

                    for(int i = 0 ; i<MAX_ATTAQUES;i++)
                    {
                        if(fichesTypesUnites[indice_fich].attaques[i].degat!=0)
                        {
                            SDL_Surface *img_porte[2],*img_icon[2];
                            img_porte[0]=IMG_Load("assets/melee_attack.png");
                            img_porte[1]=IMG_Load("assets/ranged.png");
                            img_icon[0]=IMG_Load("assets/blade.png");
                            img_icon[1]=IMG_Load("assets/pierce.png");

                            if(fichesTypesUnites[indice_fich].attaques[i].porteeAttaque==0)
                            {
                                positionIMG.x=350;
                                positionIMG.y=270+(i+1)*30;
                                 SDL_BlitSurface(img_porte[fichesTypesUnites[indice_fich].attaques[i].porteeAttaque], NULL, ecran, &positionIMG);
                                 positionIMG.x=370;
                                    SDL_BlitSurface(img_icon[fichesTypesUnites[indice_fich].attaques[i].porteeAttaque], NULL, ecran, &positionIMG);

                            }
                            else{
                                positionIMG.x=350;
                                positionIMG.y=270+(i+1)*30;
                                SDL_BlitSurface(img_porte[fichesTypesUnites[indice_fich].attaques[i].porteeAttaque], NULL, ecran, &positionIMG);
                                positionIMG.x=370;
                                    SDL_BlitSurface(img_icon[fichesTypesUnites[indice_fich].attaques[i].porteeAttaque], NULL, ecran, &positionIMG);
                            }
                            strcpy(name,fichesTypesUnites[indice_fich].attaques[i].nomAttaque);

                            positionText.x=390;
                            positionText.y=270+(i+1)*30;
                            texte = TTF_RenderText_Blended(font, name, couleur_a_utiliser) ;
                            SDL_BlitSurface(texte, NULL, ecran, &positionText);


                        }
                    }





                    positionText.x =  330;
                    positionText.y = 100+330;
                    couleur_a_utiliser=rouge;
                    SDL_BlitSurface(img_bottom, NULL, ecran, &positionText);


                }
                else
                {
                    couleur_a_utiliser=blanc;
                }
                SDL_Surface *img_unite;
                texte = TTF_RenderText_Blended(font, phrase, couleur_a_utiliser) ;
                positionText.x =510;
                positionText.y = y_depart+i*50;


                img_unite=character_md[3];
                 if(unitesMagasin[i].idFicheTypeUnite==104 && unitesMagasin[i].idJoueurAutorise==1)
                {
                    img_unite=character_md[4];
                }
                if(unitesMagasin[i].idFicheTypeUnite==63 && unitesMagasin[i].idJoueurAutorise==1)
                {
                    img_unite=character_md[6];
                }
                if(unitesMagasin[i].idFicheTypeUnite==65 && unitesMagasin[i].idJoueurAutorise==1)
                {
                    img_unite=character_md[7];
                }
                if(unitesMagasin[i].idFicheTypeUnite==158 && unitesMagasin[i].idJoueurAutorise==1)
                {
                    img_unite=character_md[5];
                }

                if(unitesMagasin[i].idJoueurAutorise==1)
                {

                    positionText.y = y_depart+20+i*60;
                    SDL_BlitSurface(img_unite, NULL, ecran, &positionText);
                    int indice_fich =chercherIndiceFicheTypeUnite(unitesMagasin[i].idFicheTypeUnite,nbFichesTypesUnites,fichesTypesUnites);
                    char titre[50];
                    strcpy(titre,fichesTypesUnites[indice_fich].nom);

                    positionText.x =  580;
                    positionText.y = y_depart+40+i*60;
                    texte = TTF_RenderText_Blended(font, titre, couleur_a_utiliser) ;
                    SDL_BlitSurface(texte, NULL, ecran, &positionText);
                    char prix  [50];
                    char  remp[50];
                    sprintf(remp,"%d",fichesTypesUnites[indice_fich].prix);
                    strcpy(prix,remp);
                    positionText.x =  580;
                    positionText.y= y_depart+40+i*60+20;
                    texte = TTF_RenderText_Blended(font, prix, couleur_a_utiliser) ;
                    SDL_BlitSurface(texte, NULL, ecran, &positionText);
                }



            }
            SDL_Flip(ecran);





        }

    }




    /*
    int indice=chercherIndiceChefUnitesJoueurs(idJoueur,unitesJoueurs);
    int choix;
    int ligne=unitesJoueurs[indice].ligne;
    int colonne=unitesJoueurs[indice].colonne;
    afficherJoueurJeu(idJoueur,nbJoueurs,joueurs);
    do
    {
        afficherUnitesMagasinUnJoueur(idJoueur,nbUnitesMagasin,nbFichesTypesUnites,unitesMagasin,fichesTypesUnites);
        printf("                              ************************************************** \n");
        printf("                              **             Menu achat Unites                **\n");
        printf("                              **************************************************\n");
        printf("                              **   1- Acheter des nouvelles unites             **\n");
        printf("                              **   0- Revenir au menu precedant                **\n");
        printf("                              ************************************************** \n");
        printf("                               Entrez votre choix: ");
        scanf("%d",&choix);
        if(choix==0)
        {
            return ;
        }

        int new_ligne;
        int new_colonne;
        do
        {
            printf("                               Entrez la ligne de la case ou vous voulez placer l'unite achetee: ");
            scanf("%d",&new_ligne);
            new_ligne--;
            printf("                               Entrez la colonne de la case ou vous voulez placer l'unite achetee: ");
            scanf("%d",&new_colonne);
            new_colonne--;


        }
        while(carte[new_ligne][new_colonne].idTypeTerrain!=ID_TYPE_TERRAIN_CHATEAU || carte[new_ligne][new_colonne].idUnite==1 || ((abs(new_ligne-ligneDonjon))>2 || (abs(new_colonne-colonneDonjon))>2) );
        int num_unite;
        do
        {
            printf("                            Entrez le numero de l'unite a acherter:");
            scanf("%d",&num_unite);
        }
        while(chercherIndiceUniteMagasin(num_unite,nbUnitesMagasin,unitesMagasin)==-1 );
        int confirmation;
        printf("                               Confirmez l'achat (non=0 oui=1):     ");
        scanf("%d",&confirmation);
        if(confirmation)
        {
            int indice_joueur=chercherIndiceJoueur(idJoueur,nbJoueurs,joueurs);
            int indice_unite=chercherMaxIdUnitesJoueurs(unitesJoueurs);
            int indice_fich =chercherIndiceFicheTypeUnite(unitesMagasin[num_unite-1].idFicheTypeUnite,nbFichesTypesUnites,fichesTypesUnites);
            if(joueurs[indice_joueur].or>=fichesTypesUnites[indice_fich].prix)
            {

                if(chercherIndiceUniteJoueur(0,0,0,unitesJoueurs)!=-1)
                {
                    initialiserNouvelleUniteJoueur(indice_unite,indice_fich,idJoueur,new_ligne,new_colonne,unitesJoueurs,fichesTypesUnites);

                    carte[new_ligne][new_colonne].idUnite=unitesJoueurs[indice_unite].idUnite;
                    carte[new_ligne][new_colonne].symboleJoueur=joueurs[indice_joueur].symbole;
                    miseAJourJoueurApresAchatUnite(num_unite,indice_joueur,indice_fich,unitesJoueurs,joueurs,fichesTypesUnites);
                    printf("                               l'achat effectue est reussi");
                }
                else
                {
                    printf("                               le tableau est rempli");
                }

            }
            else
            {
                printf("                                 unite tres chere vous n'admet pas assez d'or \n\n");
            }

        }
        else
        {
            printf("                           l'achat est interrompue");
        }
    }
    while(choix!=0);
    return;
    }*/

}
int calculerCodeAttaque(int groupe_allies_joueur, int ligne_courante, int colonne_courante,int nbJoueurs, Joueur joueurs[MAX_LIGNES_JOUEURS],CelluleCarte carte[NB_LIGNES_CARTE][NB_COLONNES_CARTE] )
{
    int code=0;
    if(colonne_courante+1<NB_COLONNES_CARTE &&carte[ligne_courante][colonne_courante+1].idUnite!=0 && groupe_allies_joueur!=0 && carte[ligne_courante][colonne_courante+1].symboleJoueur=='E')
    {
        code++;
    }
    if(colonne_courante-1>0 && carte[ligne_courante][colonne_courante-1].idUnite!=0 && groupe_allies_joueur!=0   && carte[ligne_courante][colonne_courante-1].symboleJoueur=='E')
    {
        code+=10;
    }
    if(ligne_courante+1<NB_LIGNES_CARTE &&carte[ligne_courante+1][colonne_courante].idUnite!=0 && groupe_allies_joueur!=0  && carte[ligne_courante+1][colonne_courante].symboleJoueur=='E')
    {
        code+=100;
    }
    if(ligne_courante-1>0 && carte[ligne_courante-1][colonne_courante].idUnite!=0 && groupe_allies_joueur!=0  && carte[ligne_courante-1][colonne_courante].symboleJoueur=='E')
    {
        code+=1000;
    }
    return code;
}

int verifierUniteAttaquable(int codeAttaque, int ligne_courante, int colonne_courante, int ligne_ennemie, int colonne_ennemie)
{

    int droit=codeAttaque%10;
    int gauche=(codeAttaque/10)%10;
    int bas=(codeAttaque/100)%10;
    int haut=codeAttaque/1000;
    if(droit && colonne_ennemie==colonne_courante+1 && ligne_courante==ligne_ennemie)
    {
        return 1;
    }
    if(gauche && colonne_ennemie==colonne_courante-1 && ligne_courante==ligne_ennemie)
    {
        return 1;
    }
    if(bas && ligne_courante+1==ligne_ennemie && colonne_courante==colonne_ennemie)
    {
        return 1;
    }
    if(haut && ligne_courante-1==ligne_ennemie && colonne_courante==colonne_ennemie)
    {
        return 1;
    }
    return 0;
}

int determinerPossibilitesAttaque(int numJoueur, int numEnnemi,int indiceficheTypeUniteJoueur, int indiceficheTypeUniteEnnemi,FicheTypeUnite fichesTypesUnites[MAX_LIGNES_FICHES_TYPES_UNITES],int indiceDansUnitesJoueurs, int indiceEnnemiDansUnitesJoueurs,Attaque attaques[10][2], int resistances[10][2])
{

    int count=0;

    for(int i=0; i<MAX_ATTAQUES; i++)
    {

        if(fichesTypesUnites[indiceficheTypeUniteJoueur].attaques[i].degat!=0)
        {

            attaques[count][0]=fichesTypesUnites[indiceficheTypeUniteJoueur].attaques[i];
            int j=0;
            int trouve=0;
            while(j<MAX_ATTAQUES )
            {

                if(fichesTypesUnites[indiceficheTypeUniteEnnemi].attaques[j].degat!=0 && fichesTypesUnites[indiceficheTypeUniteEnnemi].attaques[j].porteeAttaque==fichesTypesUnites[indiceficheTypeUniteJoueur].attaques[i].porteeAttaque )
                {

                    attaques[count][0]=fichesTypesUnites[indiceficheTypeUniteJoueur].attaques[i];
                    attaques[count][1]=fichesTypesUnites[indiceficheTypeUniteEnnemi].attaques[j];
                    int type_attack=fichesTypesUnites[indiceficheTypeUniteJoueur].attaques[i].typeAttaque;
                    resistances[count][0]=fichesTypesUnites[indiceficheTypeUniteEnnemi].resistances[type_attack];
                    type_attack=fichesTypesUnites[indiceficheTypeUniteEnnemi].attaques[j].typeAttaque;
                    resistances[count][1]=fichesTypesUnites[indiceficheTypeUniteJoueur].resistances[type_attack];
                    count++;
                    trouve=1;
                }

                j++;
            }

            if(trouve==0)
            {

                attaques[count][1].degat=0;
                attaques[count][1].idAttaque=0;
                attaques[count][1].idAttaqueSpeciale1=0;
                attaques[count][1].idAttaqueSpeciale2=0;
                strcpy(attaques[count][1].nomAttaque," ");
                attaques[count][1].nombreAttaques=0;
                attaques[count][1].porteeAttaque=0;
                attaques[count][1].typeAttaque=0;
                int type_attack=fichesTypesUnites[indiceficheTypeUniteJoueur].attaques[i].typeAttaque;
                resistances[count][0]=fichesTypesUnites[indiceficheTypeUniteEnnemi].resistances[type_attack];
                type_attack=fichesTypesUnites[indiceficheTypeUniteEnnemi].attaques[i].typeAttaque;
                resistances[count][1]=fichesTypesUnites[indiceficheTypeUniteJoueur].resistances[type_attack];
            }




        }
    }

    return count;

}

int choisirAttaque(SDL_Surface *ecran,int numJoueur, int numEnnemi,int indiceficheTypeUniteJoueur, int indiceficheTypeUniteEnnemi,FicheTypeUnite fichesTypesUnites[MAX_LIGNES_FICHES_TYPES_UNITES],int indiceDansUnitesJoueurs, int indiceEnnemiDansUnitesJoueurs,int defenseJoueur, int defenseEnnemi,int nbrAttaques, Attaque attaques[10][2], int resistances[10][2],Attaque attaqueChoisie[2], int resistanceAuxAttaquesChoisis[2],Periode periode, int alignementJoueur, int alignementEnnemi)
{



    // affichage menu  attaque
    char name_allie[50] ;strcpy(name_allie,fichesTypesUnites[indiceficheTypeUniteJoueur].nom);
    char name_ennemie[50] ;strcpy(name_ennemie,fichesTypesUnites[indiceficheTypeUniteEnnemi].nom);
    SDL_Surface *img_bg,*img_or,*shop ,*vs;
    SDL_Color rouge = { 255, 0, 0 };
     SDL_Color blanc = { 255, 255, 255 };
    SDL_Color couleur_a_utiliser= rouge;
    img_bg= IMG_Load("assets/bg-battle.png");
    img_or= IMG_Load("assets/money.png");
    shop=IMG_Load("assets/shop.png");

    SDL_Rect positionIMG,positionText,positionTitre;
    positionIMG.x=150;
    positionIMG.y=100;
    SDL_BlitSurface(img_bg, NULL, ecran, &positionIMG);

    SDL_Surface* texte = NULL;
    TTF_Font* font = NULL;
    font = TTF_OpenFont("assets/Oswald-VariableFont_wght.ttf", 15);

    SDL_Surface *character_md[7];
    character_md[0]=IMG_Load("assets/unite2-md.png");
    character_md[1]=IMG_Load("assets/unite3-md.png");
    character_md[2]=IMG_Load("assets/unite4-md.png");
    character_md[3]=IMG_Load("assets/unite5-md.png");
    character_md[4]=IMG_Load("assets/dragoon.png");
    character_md[5]=IMG_Load("assets/ranger.png");
    character_md[6]=IMG_Load("assets/thunderer.png");
    character_md[7]=IMG_Load("assets/dragonguard.png");
    character_md[8]=IMG_Load("assets/arvith2.png");
    character_md[9]=IMG_Load("assets/mordak2.png");
    character_md[10]=IMG_Load("assets/ennemie2.png");


    //test
    int f =  indiceDansUnitesJoueurs ,  a = indiceEnnemiDansUnitesJoueurs;
    char m [50] ;
    sprintf(m,"%d",a);
      texte = TTF_RenderText_Blended(font, m, couleur_a_utiliser) ;
    positionIMG.x = 0;
    positionIMG.y=0;
    SDL_BlitSurface(texte, NULL, ecran, &positionIMG);
    SDL_Surface *img_allie ,*img_ennemie;

    //fin  test

    if(fichesTypesUnites[indiceficheTypeUniteJoueur].idFicheTypeUnite==104)
    {
        img_allie=character_md[4];
    }
    if(fichesTypesUnites[indiceficheTypeUniteJoueur].idFicheTypeUnite==63)
    {
        img_allie=character_md[6];
    }
    if(fichesTypesUnites[indiceficheTypeUniteJoueur].idFicheTypeUnite==65)
    {
        img_allie=character_md[7];
    }
    if(fichesTypesUnites[indiceficheTypeUniteJoueur].idFicheTypeUnite==185)
    {
        img_allie=character_md[5];
    }
    //choix image ennemie
    img_ennemie=character_md[10];

    if(indiceDansUnitesJoueurs==0)
    {
       img_allie=character_md[8];
    }
    if(indiceEnnemiDansUnitesJoueurs==10)
    {
        img_ennemie=character_md[9];
    }
    positionIMG.x = 0;
    positionIMG.y = 0;
    // IL FORMULE DE CALCULE DE'INDICE IS   (ligne-1)*nbperligne  + j   = current index
    SDL_Surface *border_allie , *border_ennemie;
    border_allie=IMG_Load("assets/border_allie.png");
    border_ennemie = IMG_Load("assets/border_ennemie.png");
    vs=IMG_Load("assets/vs.png");
    positionIMG.x = 220;
    positionIMG.y=160;
    SDL_BlitSurface(img_allie, NULL, ecran, &positionIMG);
    positionIMG.x = 620;
    positionIMG.y=160;
    SDL_BlitSurface(img_ennemie, NULL, ecran, &positionIMG);
     positionIMG.x = 200;
    positionIMG.y=120;
    SDL_BlitSurface(border_allie, NULL, ecran, &positionIMG);
    //position  image border ennemie
    positionIMG.x = 600;
    positionIMG.y=120;
    SDL_BlitSurface(border_ennemie, NULL, ecran, &positionIMG);
    //position  texte name allie
    texte = TTF_RenderText_Blended(font, name_allie, blanc) ;
    positionIMG.x = 210;
    positionIMG.y=250;
      SDL_BlitSurface(texte, NULL, ecran, &positionIMG);
    //position  texte name ennemie
    texte = TTF_RenderText_Blended(font, name_ennemie, blanc) ;
    positionIMG.x = 620;
    positionIMG.y=250;
    SDL_BlitSurface(texte, NULL, ecran, &positionIMG);
   //position  vs
    positionIMG.x=400;
    positionIMG.y=150;
    SDL_BlitSurface(vs, NULL, ecran, &positionIMG);
    SDL_Color noir = {0, 0, 0}; //attention ce n'est pas un Uint32



int choix = 0 , indiceOption=0;
    int continuer = 1 ;
    SDL_Event event;
    while (continuer)
    {
        SDL_WaitEvent(&event);
        switch(event.type)
        {
        case SDL_QUIT:
            continuer = 0;
            break;
        case SDL_KEYDOWN:
            switch(event.key.keysym.sym)
            {
            case SDLK_ESCAPE: // Veut arrêter le jeu

                choix = 0;
                break;
            case SDLK_KP_ENTER:
            case SDLK_RETURN: // Veut arrêter le jeu
                if(indiceOption==nbrAttaques)
                {

                   return 0 ;
                }
                else {
        attaqueChoisie[0]=attaques[indiceOption][0];
        attaqueChoisie[1]=attaques[indiceOption][1];
        resistanceAuxAttaquesChoisis[0]=resistances[indiceOption][0];
        resistanceAuxAttaquesChoisis[1]=resistances[indiceOption][1];
                        return indiceOption+1;
                }
                break;
            case SDLK_UP:
                if(indiceOption>0)
                {
                    indiceOption--;
                }
                break;
            case SDLK_DOWN:
                if(indiceOption<nbrAttaques)
                {
                    indiceOption++;
                }
                break;
            }
            break;

        }




        if(font!= 0)
        {
            char phrase[120];


            int i, y_depart=400;
            SDL_Color couleur_a_utiliser;

            for(i=0; i<=nbrAttaques; i++)
            {
                if(i==nbrAttaques){

                       if(i==indiceOption)
                    {
                        couleur_a_utiliser=rouge;
                    }
                    else
                    {
                        couleur_a_utiliser=blanc;
                    }
                    char annuler[50]="0- Annuler attack";
                    texte = TTF_RenderText_Blended(font, annuler, couleur_a_utiliser) ;
                    positionText.x =240;
                    positionText.y = y_depart+nbrAttaques*30;
                    SDL_BlitSurface(texte, NULL, ecran, &positionText);


                    break;
                }
                int  number =   i+1 ;
                sprintf(phrase,"%d",number);
                strcat(phrase,"- ");
                char ch[20]="";
                char ch2[20]="                    ";
                char m[20]="                    ";
                char ch3[20]="";
                char ch4[20]="                    ";
                strcpy(ch,attaques[i][0].nomAttaque);
                if(strlen(ch)<20)
                {
                    strncat(ch,ch2,20-strlen(ch));
                }
                strcpy(ch3,ch);
                strcat(phrase,ch3);
                int degat   =   attaques[i][0].degat;
                char espace[50]="";
                strcat(phrase,espace) ;
                char rep[50];
                sprintf(rep,"%2d",degat);
                strcat(phrase,rep);
                strcat(phrase," x ");
                degat = attaques[i][0].nombreAttaques;
                sprintf(rep,"%2d",degat);
                strcat(phrase,rep);
                strcat(phrase," (");
                degat= resistances[i][0];
                sprintf(rep,"%2d",degat);
                strcat(phrase,rep);
                strcat(phrase,"%)    <--");
                degat =  attaques[i][0].porteeAttaque;
                sprintf(rep,"%d",degat);
                strcat(phrase,rep);
                strcat(phrase," -->");
                strcpy(ch3,attaques[i][1].nomAttaque);
                degat =  attaques[i][1].degat ;
                strcat(phrase," ");
                sprintf(rep,"%2d",degat);
                strcat(phrase,rep);
                strcat(phrase," ");

                strcat(phrase," x ");
                degat= attaques[i][1].nombreAttaques;
                sprintf(rep,"%d",degat);
                strcat(phrase,rep);
                strcat(phrase," (");
                 degat= resistances[i][1];
                sprintf(rep,"%2d",degat);
                strcat(phrase,rep);
                strcat(phrase,"%)");







                if(i==indiceOption)
                {
                    couleur_a_utiliser=rouge;
                }
                else
                {
                    couleur_a_utiliser=blanc;
                }
                texte = TTF_RenderText_Blended(font, phrase, couleur_a_utiliser) ;
                positionText.x =240;
                positionText.y = y_depart+i*30;
                SDL_BlitSurface(texte, NULL, ecran, &positionText);

            }





        }
        else
        {
            //logger l'erreur   "foirage à l'ouverture du font "
        }
        SDL_Flip(ecran);
    }








    /****////




  /*  char ch[20]="";
    char ch2[20]="                    ";
    strcpy(ch,periode.moment);
    if(strlen(ch)<20)
    {
        strncat(ch,ch2,20-strlen(ch));
    }




    printf("-----------------------------------------------------------------------------------------------------------------------------------------\n");
    printf("|Periode:%s                 Bonus Joueur: %2d | Bonus Ennemie: %2d                                                       |\n",ch,periode.bonus[alignementJoueur%4],periode.bonus[alignementEnnemi%4]);
    printf("|Vous                                                          | Ennemie                                                                 |\n");
    printf("|Defense : %2d                                                  | Defense : %2d                                                            |\n",defenseJoueur,defenseJoueur);
    printf("------------------------------------------------------------------------------------------------------------------------------------------\n");

    for(int i=0; i<nbrAttaques; i++)
    {
        char ch[20]="";
        char ch2[20]="                    ";
        char m[20]="                    ";
        char ch3[20]="";
        char ch4[20]="                    ";
        strcpy(ch,attaques[i][0].nomAttaque);
        if(strlen(ch)<20)
        {
            strncat(ch,ch2,20-strlen(ch));
        }
        strcpy(ch3,attaques[i][1].nomAttaque);
        if(strlen(ch3)<20)
        {

            strncat(ch3,ch4,20-strlen(ch3));
        }

        ch[strlen(ch)]='\0';
        ch3[strlen(ch3)]='\0';
        printf("|%1d- %s %2d + (%2d%%) x %d (resistances %2d%)    <-- %d -->     %s %2d + (%2d%%) x %d (resistances %2d%)           |\n",i+1,ch,attaques[i][0].degat,periode.bonus[alignementJoueur%4],attaques[i][0].nombreAttaques,resistances[i][0],attaques[i][0].porteeAttaque,ch3,attaques[i][1].degat,periode.bonus[alignementEnnemi%4],attaques[i][1].nombreAttaques,resistances[i][1]);


    }
    printf("|0- Annuler l'attaque !                                                                                                                  |\n");
    printf("-----------------------------------------------------------------------------------------------------------------------------------------\n");
    printf("choisir un numero d'attaque ou 0 pour l'annuler : ");
   /* int choix;
    scanf("%d",&choix);*/
   /* if(choix==0)
    {
        return choix;
    }
    else
    {
        choix--;
        attaqueChoisie[0]=attaques[choix][0];
        attaqueChoisie[1]=attaques[choix][1];
        resistanceAuxAttaquesChoisis[0]=resistances[choix][0];
        resistanceAuxAttaquesChoisis[1]=resistances[choix][1];
    }
    choix++;*/
    return choix ;

}

void miseAJourJoueurApresMortUnite(int indiceUniteJoueur, int indiceJoueur,UniteJoueur unitesJoueurs[MAX_LIGNES_UNITESJOUEURS],Joueur joueurs[MAX_LIGNES_JOUEURS])
{
    if(!possedeTrait(TRAIT_FIDELE,unitesJoueurs[indiceUniteJoueur].traits))
    {
        joueurs[indiceJoueur].entretien-=unitesJoueurs[indiceUniteJoueur].niveau;
        joueurs[indiceJoueur].nombreUnites--;
        joueurs[indiceJoueur].revenu=2+joueurs[indiceJoueur].nombreVillages-maximum(0,joueurs[indiceJoueur].entretien-joueurs[indiceJoueur].nombreVillages);
    }
    else
    {
        joueurs[indiceJoueur].nombreUnites--;
    }
}

void attaque(int indiceDansUnitesJoueurs, int indiceEnnemiDansUnitesJoueurs,int indiceJoueur, int indiceJoueurEnnemi,int ligneJoueur, int colonneJoueur, int ligneEnnemi, int colonneEnnemi,CelluleCarte carte[NB_LIGNES_CARTE][NB_COLONNES_CARTE],UniteJoueur unitesJoueurs[MAX_LIGNES_UNITESJOUEURS],Joueur joueurs[MAX_LIGNES_JOUEURS],int defenseJoueur, int defenseEnnemi, Attaque attaqueChoisie[2],int resistanceAuxAttaquesChoisis[2],Periode periode, int alignementJoueur, int alignementEnnemi)
{
  int nbr_attack_allie=attaqueChoisie[0].nombreAttaques;
    int nbr_attack_ennemie=attaqueChoisie[1].nombreAttaques;
    int nbr_attack=1;
    int nbr_attack_MAX_constant=maximum(attaqueChoisie[0].nombreAttaques,attaqueChoisie[1].nombreAttaques);

    int check_death_j1 = 0, check_death_j2 = 0 ;

    printf("L'unite %d (pv:%d) du joueur Humain attaque l'unite ennemie %d (pv%d)!\n",indiceDansUnitesJoueurs+1,unitesJoueurs[indiceDansUnitesJoueurs].pv,indiceEnnemiDansUnitesJoueurs+1,unitesJoueurs[indiceEnnemiDansUnitesJoueurs].pv);
    int esquif;
    do
    {
        if(unitesJoueurs[indiceDansUnitesJoueurs].pv<=0)
        {
            unitesJoueurs[indiceDansUnitesJoueurs].idUnite=0;
            unitesJoueurs[indiceDansUnitesJoueurs].idFicheTypeUnite=0;
            unitesJoueurs[indiceDansUnitesJoueurs].idJoueur=0;
            strcpy(unitesJoueurs[indiceDansUnitesJoueurs].nomUnite," ");
            unitesJoueurs[indiceDansUnitesJoueurs].nomUnite[strlen(unitesJoueurs[indiceDansUnitesJoueurs].nomUnite)]='\0';
            unitesJoueurs[indiceDansUnitesJoueurs].active=0;
            unitesJoueurs[indiceDansUnitesJoueurs].rang=0;
            unitesJoueurs[indiceDansUnitesJoueurs].ligne=0;
            unitesJoueurs[indiceDansUnitesJoueurs].colonne=0;
            unitesJoueurs[indiceDansUnitesJoueurs].traits[0]=0;
            unitesJoueurs[indiceDansUnitesJoueurs].traits[1]=0;
            unitesJoueurs[indiceDansUnitesJoueurs].traits[2]=0;
            unitesJoueurs[indiceDansUnitesJoueurs].pv=0;
            unitesJoueurs[indiceDansUnitesJoueurs].pvMax=0;
            unitesJoueurs[indiceDansUnitesJoueurs].mouvements=0;
            unitesJoueurs[indiceDansUnitesJoueurs].mouvementsMax=0;
            unitesJoueurs[indiceDansUnitesJoueurs].xp=0;
            unitesJoueurs[indiceDansUnitesJoueurs].xpRequise=0;
            unitesJoueurs[indiceDansUnitesJoueurs].niveau=0;
            unitesJoueurs[indiceDansUnitesJoueurs].finTour=0;
            carte[ligneJoueur][colonneJoueur].idUnite=0;
            carte[ligneJoueur][colonneJoueur].symboleJoueur=" ";
            miseAJourJoueurApresMortUnite(indiceDansUnitesJoueurs,indiceJoueur,unitesJoueurs,joueurs);
            check_death_j1 = 1 ;
            break;

        }
        if(unitesJoueurs[indiceEnnemiDansUnitesJoueurs].pv<=0)
        {
            check_death_j2 = 1 ;
            unitesJoueurs[indiceEnnemiDansUnitesJoueurs].pv=0;
            unitesJoueurs[indiceEnnemiDansUnitesJoueurs].idUnite=0;
            unitesJoueurs[indiceEnnemiDansUnitesJoueurs].idJoueur=0;
            unitesJoueurs[indiceEnnemiDansUnitesJoueurs].active=0;
            unitesJoueurs[indiceEnnemiDansUnitesJoueurs].alignement=0;
            unitesJoueurs[indiceEnnemiDansUnitesJoueurs].colonne=0;
            unitesJoueurs[indiceEnnemiDansUnitesJoueurs].finTour=0;
            unitesJoueurs[indiceEnnemiDansUnitesJoueurs].idFicheTypeUnite=0;
            unitesJoueurs[indiceEnnemiDansUnitesJoueurs].ligne=0;
            unitesJoueurs[indiceEnnemiDansUnitesJoueurs].colonne=0;
            unitesJoueurs[indiceEnnemiDansUnitesJoueurs].mouvements=0;
            unitesJoueurs[indiceEnnemiDansUnitesJoueurs].mouvementsMax=0;
            unitesJoueurs[indiceEnnemiDansUnitesJoueurs].niveau=0;
            strcpy(unitesJoueurs[indiceEnnemiDansUnitesJoueurs].nomUnite,"");
            unitesJoueurs[indiceEnnemiDansUnitesJoueurs].rang=0;
            unitesJoueurs[indiceEnnemiDansUnitesJoueurs].xpRequise=0;
            unitesJoueurs[indiceEnnemiDansUnitesJoueurs].xp=0;
            carte[ligneEnnemi][colonneEnnemi].idUnite=0;
            carte[ligneEnnemi][colonneEnnemi].symboleJoueur=" ";
            miseAJourJoueurApresMortUnite(indiceEnnemiDansUnitesJoueurs,indiceJoueurEnnemi,unitesJoueurs,joueurs);
            break;


        }
        if(nbr_attack_allie!=0 )
        {
            esquif=rand()%100;
            if(esquif<=defenseEnnemi || defenseEnnemi ==0)
            {
                int damage_allie=(attaqueChoisie[0].degat*(100-resistanceAuxAttaquesChoisis[0])*(100+periode.bonus[alignementJoueur%4]))/10000;
                unitesJoueurs[indiceEnnemiDansUnitesJoueurs].pv-=damage_allie;
                printf("le jouer 1 a inflige  au joueur 2 un degat de %2d (santerestante %2d)\n",damage_allie,unitesJoueurs[indiceEnnemiDansUnitesJoueurs].pv);
                nbr_attack_allie--;

            }
            else
            {
                printf("le jouer 2 a esquife l'attaque du joueur 1 \n");

                nbr_attack_allie--;
            }
        }

        if(nbr_attack_ennemie!=0)
        {

            esquif=rand()%100;
            if(esquif<=defenseJoueur || defenseJoueur==0)
            {
                int damage_ennemie=(attaqueChoisie[1].degat*(100-resistanceAuxAttaquesChoisis[1])*(100+periode.bonus[alignementEnnemi%4]))/10000;
                unitesJoueurs[indiceDansUnitesJoueurs].pv-=damage_ennemie;
                nbr_attack_ennemie--;
                printf("le jouer 2 a inflige  au joueur 1 un degat de %2d (santerestante %2d)\n",damage_ennemie,unitesJoueurs[indiceDansUnitesJoueurs].pv);
            }
            else
            {
                printf("le jouer 1 a esquife l'attaque du joueur 2 \n");
                nbr_attack_ennemie--;
            }

        }

        printf("Attaque %d/%d \n",nbr_attack,nbr_attack_MAX_constant);
        nbr_attack++;
    }
    while(nbr_attack<nbr_attack_MAX_constant+1);
    if(check_death_j1==0)
    {
        if(unitesJoueurs[indiceDansUnitesJoueurs].pv<=0)
        {
            unitesJoueurs[indiceDansUnitesJoueurs].idUnite=0;
            unitesJoueurs[indiceDansUnitesJoueurs].idFicheTypeUnite=0;
            unitesJoueurs[indiceDansUnitesJoueurs].idJoueur=0;
            strcpy(unitesJoueurs[indiceDansUnitesJoueurs].nomUnite," ");
            unitesJoueurs[indiceDansUnitesJoueurs].nomUnite[strlen(unitesJoueurs[indiceDansUnitesJoueurs].nomUnite)]='\0';
            unitesJoueurs[indiceDansUnitesJoueurs].active=0;
            unitesJoueurs[indiceDansUnitesJoueurs].rang=0;
            unitesJoueurs[indiceDansUnitesJoueurs].ligne=0;
            unitesJoueurs[indiceDansUnitesJoueurs].colonne=0;
            unitesJoueurs[indiceDansUnitesJoueurs].traits[0]=0;
            unitesJoueurs[indiceDansUnitesJoueurs].traits[1]=0;
            unitesJoueurs[indiceDansUnitesJoueurs].traits[2]=0;
            unitesJoueurs[indiceDansUnitesJoueurs].pv=0;
            unitesJoueurs[indiceDansUnitesJoueurs].pvMax=0;
            unitesJoueurs[indiceDansUnitesJoueurs].mouvements=0;
            unitesJoueurs[indiceDansUnitesJoueurs].mouvementsMax=0;
            unitesJoueurs[indiceDansUnitesJoueurs].xp=0;
            unitesJoueurs[indiceDansUnitesJoueurs].xpRequise=0;
            unitesJoueurs[indiceDansUnitesJoueurs].niveau=0;
            unitesJoueurs[indiceDansUnitesJoueurs].finTour=0;
            carte[ligneJoueur][colonneJoueur].idUnite=0;
            carte[ligneJoueur][colonneJoueur].symboleJoueur=" ";
            miseAJourJoueurApresMortUnite(indiceDansUnitesJoueurs,indiceJoueur,unitesJoueurs,joueurs);
            check_death_j1 = 1 ;

        }

    }
    if(check_death_j2 == 0 )
    {
        if(unitesJoueurs[indiceEnnemiDansUnitesJoueurs].pv<=0)
        {
            check_death_j2 = 1 ;
            unitesJoueurs[indiceEnnemiDansUnitesJoueurs].pv=0;
            unitesJoueurs[indiceEnnemiDansUnitesJoueurs].idUnite=0;
            unitesJoueurs[indiceEnnemiDansUnitesJoueurs].idJoueur=0;
            unitesJoueurs[indiceEnnemiDansUnitesJoueurs].active=0;
            unitesJoueurs[indiceEnnemiDansUnitesJoueurs].alignement=0;
            unitesJoueurs[indiceEnnemiDansUnitesJoueurs].colonne=0;
            unitesJoueurs[indiceEnnemiDansUnitesJoueurs].finTour=0;
            unitesJoueurs[indiceEnnemiDansUnitesJoueurs].idFicheTypeUnite=0;
            unitesJoueurs[indiceEnnemiDansUnitesJoueurs].ligne=0;
            unitesJoueurs[indiceEnnemiDansUnitesJoueurs].colonne=0;
            unitesJoueurs[indiceEnnemiDansUnitesJoueurs].mouvements=0;
            unitesJoueurs[indiceEnnemiDansUnitesJoueurs].mouvementsMax=0;
            unitesJoueurs[indiceEnnemiDansUnitesJoueurs].niveau=0;
            strcpy(unitesJoueurs[indiceEnnemiDansUnitesJoueurs].nomUnite,"");
            unitesJoueurs[indiceEnnemiDansUnitesJoueurs].rang=0;
            unitesJoueurs[indiceEnnemiDansUnitesJoueurs].xpRequise=0;
            unitesJoueurs[indiceEnnemiDansUnitesJoueurs].xp=0;
            carte[ligneEnnemi][colonneEnnemi].idUnite=0;
            carte[ligneEnnemi][colonneEnnemi].symboleJoueur=" ";
            miseAJourJoueurApresMortUnite(indiceEnnemiDansUnitesJoueurs,indiceJoueurEnnemi,unitesJoueurs,joueurs);


        }

    }

}

int verifierGagnant(int nbJoueurs,Joueur joueurs[MAX_LIGNES_JOUEURS],UniteJoueur unitesJoueurs[MAX_LIGNES_UNITESJOUEURS])
{

    int indicejoueurHumain=chercherIndiceJoueurHumain(nbJoueurs,joueurs);
    int i=0;
    int humain_gagnant=1;
    int indiceunite_chef_ennemie=-1;
    int indicechef=chercherIndiceChefUnitesJoueurs(1,unitesJoueurs);

    if(indicechef==-1)
    {


        return 2;

    }

    do
    {
        if(joueurs[indicejoueurHumain].idJoueur==joueurs[i].idJoueur && joueurs[i].symbole=='J' )
        {
            if(joueurs[i].nombreUnites==0)

            {

                return 2;
            }

        }
        else
        {
            if(joueurs[i].groupe_allies!=joueurs[indicejoueurHumain].groupe_allies  )
            {


                for(int j=0; j<MAX_LIGNES_UNITESJOUEURS; j++)
                {

                    if(unitesJoueurs[j].rang==1 && unitesJoueurs[j].idJoueur==joueurs[i].idJoueur)
                    {

                        indiceunite_chef_ennemie=j;
                        humain_gagnant=0;

                    }
                }



            }
        }

        i++;
    }
    while(i<nbJoueurs);

    if(indiceunite_chef_ennemie==-1)
    {

        return 1;

    }

    return humain_gagnant;
}

int verifierFinTours(int idJoueur, UniteJoueur unitesJoueurs[MAX_LIGNES_UNITESJOUEURS])
{
    for (int i=0; i<MAX_LIGNES_UNITESJOUEURS; i++)
    {
        if(unitesJoueurs[i].finTour==0 && unitesJoueurs[i].idJoueur==idJoueur)
        {
            return 0;
        }
    }
    return 1;
}

void reinitialiserTours(int idJoueur, int nbJoueurs, Joueur joueurs[MAX_LIGNES_JOUEURS],UniteJoueur unitesJoueurs[MAX_LIGNES_UNITESJOUEURS])
{
    for(int i=0; i<MAX_LIGNES_UNITESJOUEURS; i++)
    {
        if(unitesJoueurs[i].idJoueur==idJoueur)
        {
            unitesJoueurs[i].finTour=0;
            unitesJoueurs[i].mouvements=unitesJoueurs[i].mouvementsMax;
        }
    }
    int indice=chercherIndiceJoueur(idJoueur,nbJoueurs,joueurs);
    joueurs[indice].or+=8;

}
int DoubleClickDetected (void)
{
    static Uint32 LastClickTicks;
    Uint32 CurrentClickTicks;
    /* The first time this function is called, LastClickTicks has not been initialised yet. */
    if (! LastClickTicks)
    {
        LastClickTicks = SDL_GetTicks ();
        return 0;
    }
    else
    {
        CurrentClickTicks = SDL_GetTicks ();
        /* If the period between the two clicks is smaller or equal to a pre-defined number, we report a DoubleClick event. */
        if (CurrentClickTicks - LastClickTicks <= DOUBLE_CLICK_MAX_DELAY)
        {
            /* Update LastClickTicks and signal a DoubleClick. */
            LastClickTicks = CurrentClickTicks;
            return 1;
        }
        else
        {
            /* Update LastClickTicks and signal a SingleClick. */
            LastClickTicks = CurrentClickTicks;
            return 0;
        }
    }
}
int jouerTourHumain(SDL_Surface *ecran,int idJoueur, int groupeAlliesJoueur, int nbFichesTypesUnites, int nbJoueurs,int nbUnitesMagasin, int nbVillages,FicheTypeUnite fichesTypesUnites[MAX_LIGNES_FICHES_TYPES_UNITES],Joueur joueurs[MAX_LIGNES_JOUEURS],CelluleCarte carte[NB_LIGNES_CARTE][NB_COLONNES_CARTE],UniteMagasin unitesMagasin[MAX_LIGNES_UNITESMAGASIN],Village villages[MAX_LIGNES_VILLAGES],TypeTerrain typesTerrains[NB_TYPES_TERRAINS],UniteJoueur unitesJoueurs[MAX_LIGNES_UNITESJOUEURS],Periode periode)
{
    SDL_EnableKeyRepeat(100, 100);
    SDL_Surface *img_tab[20];
    img_tab[0]=IMG_Load("assets/PLAT2.png");;
    img_tab[1]=IMG_Load("assets/CASTLE2.png");
    img_tab[2]=IMG_Load("assets/CAVERNE2.png");
    img_tab[3]=IMG_Load("assets/RECIF2.png");
    img_tab[4]=IMG_Load("assets/EAU_PROFONDE2.png");
    img_tab[5]=IMG_Load("assets/FAUX_VOILE2.png");
    img_tab[6]=IMG_Load("assets/PLAT2.png");
    img_tab[7]=IMG_Load("assets/FORET2.png");
    img_tab[8]=IMG_Load("assets/GELE2.png");
    img_tab[9]=IMG_Load("assets/CHAMPIGNONS2.png");
    img_tab[10]=IMG_Load("assets/COLLINES2.png");
    img_tab[11]=IMG_Load("assets/MONTAGNES2.png");
    img_tab[12]=IMG_Load("assets/SABLE2.png");
    img_tab[13]=IMG_Load("assets/EAU_PEU_PROFONDE2.png");
    img_tab[14]=IMG_Load("assets/MARAIS2.png");
    img_tab[15]=IMG_Load("assets/IMPARTICABLE2.png");
    img_tab[16]=IMG_Load("assets/VILLAGE2.png");
    img_tab[17]=IMG_Load("assets/DONJON2.png");
    img_tab[18]=IMG_Load("assets/marque.png");
    SDL_Surface *characters[10];
    characters[0]=IMG_Load("assets/arvith.png");
    characters[1]=IMG_Load("assets/mordak.png");
    characters[2]=IMG_Load("assets/ennemie2-mn.png");
    characters[3]=IMG_Load("assets/allie.png");
    characters[4]=IMG_Load("assets/dragoon-mn.png");
    characters[5]=IMG_Load("assets/ranger-mn.png");
    characters[6]=IMG_Load("assets/thunderer-mn.png");
    characters[7]=IMG_Load("assets/dragonguard-mn.png");
     SDL_Surface *periode_img[5] ;
                    periode_img[0]=IMG_Load("assets/periode1.png");
                    periode_img[1]=IMG_Load("assets/periode2.png");
                    periode_img[2]=IMG_Load("assets/periode3.png");
                    periode_img[3]=IMG_Load("assets/periode4.png");
                    periode_img[4]=IMG_Load("assets/periode5.png");

    afficherCarteJeu(ecran,img_tab,characters,carte,nbJoueurs,joueurs,nbVillages,villages,unitesJoueurs);
    int TAILLE_BLOC=25;
    SDL_Event event2;
    int continuer2 = 1,code = 0 ;
    int check = -1 ;
    int indice=chercherIndiceChefUnitesJoueurs(idJoueur,unitesJoueurs);
    int ligne=unitesJoueurs[indice].ligne;
    int colonne=unitesJoueurs[indice].colonne;
    while (continuer2)
    {

        SDL_WaitEvent(&event2);
        switch(event2.type)
        {

        case SDL_KEYDOWN:
            switch(event2.key.keysym.sym)
            {
            case SDLK_SPACE:
                continuer2 = 0;
                break;
            case SDLK_q:
                continuer2 = 0;
                break;





            }
            break;
        case SDL_MOUSEBUTTONDOWN:

            if (event2.button.button == SDL_BUTTON_LEFT)
            {

                SDL_Rect position;
                position.x =  event2.button.x;
                position.x/=25;
                position.x*=25;
                position.y = event2.button.y;
                position.y/=25;
                position.y*=25;
                int pos_x  = event2.button.x;
                int pos_y = event2.button.y;
               if(DoubleClickDetected()==1)
                {
                     if(pos_x>=1050 && pos_x<=1050+180 && pos_y>=635 && pos_y<=635+47)

                    {

                         continuer2 = 0;
                            break;
                    }
                }
                if(DoubleClickDetected() == 1&& check!=-1)
                {
                    effacerDeplacementsPossibles(carte);
                   if(unitesJoueurs[check].idJoueur==1) marquerDeplacementsPossibles(carte,unitesJoueurs[check].ligne,unitesJoueurs[check].colonne,unitesJoueurs[check].mouvements);
                    if(carte[position.y/25][position.x/25].marque)
                    {

                        deplacerUnite(0,check,unitesJoueurs[check].idUnite,unitesJoueurs[check].ligne,unitesJoueurs[check].colonne,position.y/25,position.x/25,nbJoueurs,nbVillages,carte,joueurs,unitesJoueurs,villages);
                        effacerDeplacementsPossibles(carte);
                        code=calculerCodeAttaque(joueurs[0].groupe_allies,unitesJoueurs[check].ligne,unitesJoueurs[check].colonne,nbJoueurs,joueurs,carte);
                        if(code!=0)
                        {

                            SDL_Event event3;
                            SDL_Surface *img_bg ;
                            SDL_Color rouge = { 255, 0, 0 };
                            SDL_Color blanc ={255 , 255 , 255};
                            SDL_Color couleur_a_utiliser= rouge;
                            img_bg= IMG_Load("assets/magasin.png");
                            SDL_Rect positionIMG,positionText,positionTitre;
                            positionIMG.x=240;
                            positionIMG.y=30;
                            SDL_BlitSurface(img_bg, NULL, ecran, &positionIMG);
                            SDL_Surface* texte = NULL;
                            TTF_Font* font = NULL;
                            font = TTF_OpenFont("assets/Oswald-VariableFont_wght.ttf", 25);
                            positionIMG.x = 0;
                            positionIMG.y = 0;
                            // IL FORMULE DE CALCULE DE'INDICE IS   (ligne-1)*nbperligne  + j   = current index
                            SDL_Surface *img[2];
                            img[0]=IMG_Load("assets/unite_magasin_allie.png");
                            img[1]=IMG_Load("assets/unite_magasin_ennemie.png");
                            positionIMG.x=390;
                            positionIMG.y=120;
                            int ligne_ennemie;
                            int colonne_ennemie;
                            printf("il ya une unite ennemie!!!!\n ");
                            int confirmation_attack=0;

                            SDL_BlitSurface(img[1], NULL, ecran, &positionIMG);
                            char question[50]="Voulez -vous attacker cette unite ennemie ? ";
                            char detecter [50] ="il ya une unite ennemie!!!!";
                            positionText.x=350;
                            positionText.y=80;
                            texte = TTF_RenderText_Blended(font,detecter, blanc) ;
                            SDL_BlitSurface(texte, NULL, ecran, &positionText);
                            texte = TTF_RenderText_Blended(font,question,blanc) ;
                            positionText.x=280;
                            positionText.y=350;
                            SDL_BlitSurface(texte, NULL, ecran, &positionText);
                            SDL_Flip(ecran);
                            int continuer3 = 1,choix=0;
                            SDL_Color noir = {0, 0, 0}; //attention ce n'est pas un Uint32



                            int indiceOption=0;
                            while (continuer3)
                            {
                                SDL_WaitEvent(&event3);
                                switch(event3.type)
                                {
                                case SDL_QUIT:
                                    continuer3 = 0;
                                    break;
                                case SDL_KEYDOWN:
                                    switch(event3.key.keysym.sym)
                                    {
                                    case SDLK_ESCAPE: // Veut arrêter le jeu

                                        continuer3 = 0;
                                        break;
                                    case SDLK_KP_ENTER:
                                    case SDLK_RETURN: // Veut arrêter le jeu
                                        if(indiceOption==0)
                                        {

                                            continuer3  = 0 ;
                                            confirmation_attack=1;

                                            break;
                                        }
                                        else if(indiceOption==1)
                                        {
                                            continuer3=0;
                                            break;
                                        }

                                        break;
                                    case SDLK_UP:
                                        if(indiceOption>0)
                                        {
                                            indiceOption--;
                                        }
                                        break;
                                    case SDLK_DOWN:
                                        if(indiceOption<1)
                                        {
                                            indiceOption++;
                                        }
                                        break;
                                    }
                                    break;

                                }




                                if(font!= 0)
                                {
                                    char* phrase[3];
                                    phrase[0]= "* Oui";
                                    phrase[1]= "* Non";


                                    int i, y_depart=400;
                                    SDL_Color couleur_a_utiliser;

                                    for(i=0; i<2; i++)
                                    {
                                        if(i==indiceOption)
                                        {
                                            couleur_a_utiliser=rouge;
                                        }
                                        else
                                        {
                                            couleur_a_utiliser=blanc;
                                        }
                                        texte = TTF_RenderText_Blended(font, phrase[i], couleur_a_utiliser) ;
                                        positionText.x = 450;
                                        positionText.y = y_depart+i*30;
                                        SDL_BlitSurface(texte, NULL, ecran, &positionText);

                                    }


                                }
                                else
                                {
                                    //logger l'erreur   "foirage à l'ouverture du font "
                                }
                                SDL_Flip(ecran);
                            }
                            if(confirmation_attack)
                            {
                                SDL_FillRect(ecran, NULL, SDL_MapRGB(ecran->format, 255, 255, 255));
                                            afficherCarteJeu(ecran,img_tab,characters,carte,nbJoueurs,joueurs,nbVillages,villages,unitesJoueurs);
                                            SDL_Flip(ecran);
                                //choix de l'unite
                                continuer3 = 1;
                                while(continuer3)
                                {
                                    SDL_WaitEvent(&event3);
                                    switch(event3.type)
                                    {
                                    case SDLK_a:
                                        continuer3=0;
                                        break ;

                                    case SDL_MOUSEBUTTONDOWN:
                                        if (event3.button.button == SDL_BUTTON_LEFT)
                                        {

                                            SDL_Rect position;
                                            position.x =  event3.button.x;
                                            position.x/=25;
                                            position.x*=25;
                                            position.y = event3.button.y;
                                            position.y/=25;
                                            position.y*=25;

                                            if(DoubleClickDetected() == 1)
                                            {

                                                SDL_Surface *Case_a_remplir = NULL;
                                                Case_a_remplir = SDL_CreateRGBSurface(SDL_HWSURFACE, TAILLE_BLOC, TAILLE_BLOC, 32, 0, 0, 0, 0);
                                                SDL_FillRect(Case_a_remplir, NULL, SDL_MapRGB(ecran->format, 255, 0, 0));
                                                SDL_SetAlpha(Case_a_remplir, SDL_SRCALPHA, 100);
                                                SDL_BlitSurface(Case_a_remplir, NULL, ecran, &position);
                                                SDL_Flip(ecran);
                                                if(carte[position.y/25][position.x/25].symboleJoueur=='E')
                                                {

                                                    int ligne_ennemie =  position.y/25;
                                                    int colonne_ennemie=position.x/25;
                                                    int ligne  = unitesJoueurs[check].ligne;
                                                    int colonne = unitesJoueurs[check].colonne;
                                                    if(verifierUniteAttaquable(code,ligne,colonne,ligne_ennemie,colonne_ennemie))
                                                    {
                                                        continuer3=0;
                                                        int id_typeterrain_ennemie=carte[ligne_ennemie][colonne_ennemie].idTypeTerrain;
                                                        int id_typeterrain_joueur=carte[ligne][colonne].idTypeTerrain;
                                                        Attaque attaques[10][2];
                                                        Attaque attaques_choisie[2];
                                                        int resistances_choisie[2];
                                                        int resistances[10][2];
                                                        int indice_ennemie=chercherIndiceUniteJoueur(carte[ligne_ennemie][colonne_ennemie].idUnite,2,1,unitesJoueurs);

                                                        int indice_fich_unite_joueur=chercherIndiceFicheTypeUnite(unitesJoueurs[check].idFicheTypeUnite,nbFichesTypesUnites,fichesTypesUnites);

                                                        int indice_fich_unite_ennemie=chercherIndiceFicheTypeUnite(unitesJoueurs[indice_ennemie].idFicheTypeUnite,nbFichesTypesUnites,fichesTypesUnites);

                                                        int nombre_attaques=determinerPossibilitesAttaque(0,1,indice_fich_unite_joueur,indice_fich_unite_ennemie,fichesTypesUnites,check,indice_ennemie,attaques,resistances);
                                                        //int defense_joueur=fichesTypesUnites[indice_fich_unite_joueur].relationsTerrains[

                                                        int relation= carte[unitesJoueurs[check].ligne][unitesJoueurs[check].colonne].idTypeTerrain;
                                                        int  defense_humain=fichesTypesUnites[indice_fich_unite_joueur].resistances[relation];
                                                        printf("defense  = %d realtion %d \n",defense_humain,relation);
                                                        int relation_ennemie= carte[unitesJoueurs[indice_ennemie].ligne][unitesJoueurs[indice_ennemie].colonne].idTypeTerrain;
                                                        int  defense_ennemie=fichesTypesUnites[indice_fich_unite_ennemie].resistances[relation_ennemie];


                                                        int choix_attack=choisirAttaque(ecran,0,1,indice_fich_unite_joueur,indice_fich_unite_ennemie,fichesTypesUnites,check,indice_ennemie,defense_humain,defense_ennemie,nombre_attaques,attaques,resistances,attaques_choisie,resistances_choisie,periode,unitesJoueurs[check].alignement,unitesJoueurs[indice_ennemie].alignement);

                                                       if(choix_attack!=0)
                                                        {

                                                    attaque(check,indice_ennemie,0,1,unitesJoueurs[check].ligne,unitesJoueurs[check].colonne,unitesJoueurs[indice_ennemie].ligne,unitesJoueurs[indice_ennemie].colonne,carte,unitesJoueurs,joueurs,defense_humain,defense_ennemie,attaques_choisie,resistances_choisie,periode,unitesJoueurs[check].alignement,unitesJoueurs[indice_ennemie].alignement);
                                                        }

                                                    }

                                                }

                                            }
                                            break;



                                    }
                                    }
                                }
                            }

                        }

                    }
                }



                /*SDL_Surface *Case_a_remplir = NULL;
                Case_a_remplir = SDL_CreateRGBSurface(SDL_HWSURFACE, TAILLE_BLOC, TAILLE_BLOC, 32, 0, 0, 0, 0);
                SDL_FillRect(Case_a_remplir, NULL, SDL_MapRGB(ecran->format, 255, 0, 0));
                SDL_SetAlpha(Case_a_remplir, SDL_SRCALPHA, 100);
                SDL_BlitSurface(Case_a_remplir, NULL, ecran, &position);*/
               afficherCarteJeu(ecran,img_tab,characters,carte,nbJoueurs,joueurs,nbVillages,villages,unitesJoueurs);
                SDL_Flip(ecran);
                if(carte[position.y/25][position.x/25].idUnite)
                {
                    effacerDeplacementsPossibles(carte);

                    int  indice=chercherIndiceUniteJoueur(carte[position.y/25][position.x/25].idUnite,1,1,unitesJoueurs);
                    check=indice;
                    if(indice==-1)
                    {
                        indice = chercherIndiceUniteJoueur(carte[position.y/25][position.x/25].idUnite,2,1,unitesJoueurs);

                    }
                    int ligne=unitesJoueurs[indice].ligne;
                    int colonne=unitesJoueurs[indice].colonne;
                    if(indice!=-1 && unitesJoueurs[indice].idJoueur!= 2) marquerDeplacementsPossibles(carte,ligne,colonne,unitesJoueurs[indice].mouvements);
                    afficherCarteJeu(ecran,img_tab,characters,carte,nbJoueurs,joueurs,nbVillages,villages,unitesJoueurs);


                    SDL_Color white = { 255, 255, 255 };
                    SDL_Color couleur_a_utiliser= white;

                    SDL_Surface* texte = NULL;
                    TTF_Font* font = NULL;
                    font = TTF_OpenFont("assets/Oswald-VariableFont_wght.ttf", 25);
                    char titre[50]="Informations unite-joueur : ";
                    char x[20];
                    sprintf(x,"%d",carte[position.y/25][position.x/25].idUnite);
                    strcat(titre,x);
                    SDL_Rect positionTexte;
                    texte = TTF_RenderText_Blended(font,titre, couleur_a_utiliser) ;
                    positionTexte.x = 970;
                    positionTexte.y = 180;
                    SDL_BlitSurface(texte, NULL, ecran, &positionTexte);
                    SDL_Surface *character_md[7];
                    character_md[0]=IMG_Load("assets/dragoon.png");
                    character_md[1]=IMG_Load("assets/ranger.png");
                    character_md[2]=IMG_Load("assets/thunderer.png");
                    character_md[3]=IMG_Load("assets/dragonguard.png");
                    character_md[4]=IMG_Load("assets/arvith2.png");
                    character_md[5]=IMG_Load("assets/mordak2.png");
                    character_md[6]=IMG_Load("assets/ennemie2.png");
                    char name[20];
                    strcpy(name,unitesJoueurs[indice].nomUnite);
                    texte = TTF_RenderText_Blended(font,name, couleur_a_utiliser) ;
                    positionTexte.x = 1060;
                    positionTexte.y = 220;
                    SDL_Surface * img;
                    SDL_BlitSurface(texte, NULL, ecran, &positionTexte);

                           if(unitesJoueurs[indice].idFicheTypeUnite==104)
                            {
                                img=character_md[0];
                            }
                            if(unitesJoueurs[indice].idFicheTypeUnite==63)
                            {
                                img=character_md[2];
                            }
                            if(unitesJoueurs[indice].idFicheTypeUnite==65)
                            {
                                img=character_md[3];
                            }
                            if(unitesJoueurs[indice].idFicheTypeUnite==185)
                            {
                                img=character_md[1];
                            }
                            if(unitesJoueurs[indice].idJoueur==1 && unitesJoueurs[indice].rang)
                            {
                                 img=character_md[4];
                            }
                             if(unitesJoueurs[indice].idJoueur==2 && unitesJoueurs[indice].rang)
                            {
                                 img=character_md[5];
                            }
                            if(unitesJoueurs[indice].idJoueur==2 && unitesJoueurs[indice].rang==0)
                            {
                                 img=character_md[6];
                            }
                    positionTexte.x = 1060;
                    positionTexte.y = 250;
                    SDL_BlitSurface(img, NULL, ecran, &positionTexte);
                    int hp =  unitesJoueurs[indice].pv;
                    int hp_max =  unitesJoueurs[indice].pvMax;
                    int niveau  =  unitesJoueurs[indice].niveau;
                    int race =  unitesJoueurs[indice].rang;

                    strcpy(x,"hp : ");
                    char  remp[20];
                    sprintf(remp,"%d",hp);
                    strcat(x,remp);
                    strcat(x,"/");
                    sprintf(remp,"%d",hp_max);
                    strcat(x,remp);
                    positionTexte.x = 1050;
                    positionTexte.y = 310;
                    texte = TTF_RenderText_Blended(font,x, couleur_a_utiliser) ;
                     SDL_BlitSurface(texte, NULL, ecran, &positionTexte);
                    strcpy(x,"niveau : ");
                    sprintf(remp,"%d",niveau);
                    strcat(x,remp);
                     positionTexte.x = 1050;
                    positionTexte.y = 350;
                    texte = TTF_RenderText_Blended(font,x, couleur_a_utiliser) ;
                     SDL_BlitSurface(texte, NULL, ecran, &positionTexte);
                      strcpy(x,"race : ");
                    sprintf(remp,"%d",race);
                    strcat(x,remp);
                     positionTexte.x = 1050;
                    positionTexte.y = 390;
                    texte = TTF_RenderText_Blended(font,x, couleur_a_utiliser) ;
                     SDL_BlitSurface(texte, NULL, ecran, &positionTexte);
                      strcpy(x,"ligne : ");
                    sprintf(remp,"%d",ligne);
                    strcat(x,remp);
                     positionTexte.x = 1050;
                    positionTexte.y = 430;
                    texte = TTF_RenderText_Blended(font,x, couleur_a_utiliser) ;
                    SDL_BlitSurface(texte, NULL, ecran, &positionTexte);
                    strcpy(x,"colonne : ");
                    sprintf(remp,"%d",colonne);
                    strcat(x,remp);
                     positionTexte.x = 1050;
                    positionTexte.y = 470;
                    texte = TTF_RenderText_Blended(font,x, couleur_a_utiliser) ;
                     SDL_BlitSurface(texte, NULL, ecran, &positionTexte);


                    positionTexte.x = 990;
                    positionTexte.y = 110;
                    SDL_BlitSurface(periode_img[periode.numOrdre-1], NULL, ecran, &positionTexte);









                    effacerDeplacementsPossibles(carte);
                    SDL_Flip(ecran);

                }


            }
            else{

                if(carte[ligne][colonne].idTypeTerrain==ID_TYPE_TERRAIN_DONJON)
                {
                    acheterUnites(ecran,characters,idJoueur,ligne,colonne,nbUnitesMagasin,nbJoueurs,nbFichesTypesUnites,nbVillages,unitesMagasin,unitesJoueurs,joueurs,fichesTypesUnites,villages,carte);
                    afficherCarteJeu(ecran,img_tab,characters,carte,nbJoueurs,joueurs,nbVillages,villages,unitesJoueurs);
                    SDL_Flip(ecran);
                }
                else
                {

                    printf("votre unite chef ne se trouve pas dans un Donjon\n ");
                }


            }
            break;



        }
    }


// code derouler jeu


    /* do
     {
         do
         {
             printf("donner le numero d'unite  a jouer ou 0 si vous voulez forcer la fin du tour ou -1 si vous  voulez acheter de nouvelles unites :");
             scanf("%d",&choix);
             if(choix!=-1 && choix!=0)
             {
                 indice=chercherIndiceUniteJoueur(choix,1,1,unitesJoueurs);
             }
         }
         while( choix <-1 || indice==-1);
         if(choix==-1)
         {
             int indice=chercherIndiceChefUnitesJoueurs(idJoueur,unitesJoueurs);
             int ligne=unitesJoueurs[indice].ligne;
             int colonne=unitesJoueurs[indice].colonne;

             if(carte[ligne][colonne].idTypeTerrain==ID_TYPE_TERRAIN_DONJON)
             {
                 acheterUnites(idJoueur,ligne,colonne,nbUnitesMagasin,nbJoueurs,nbFichesTypesUnites,nbVillages,unitesMagasin,unitesJoueurs,joueurs,fichesTypesUnites,villages,carte);
                 afficherCarteJeu(ecran,carte,nbJoueurs,joueurs,nbVillages,villages,unitesJoueurs);
             }
             else
             {

                 printf("votre unite chef ne se trouve pas dans un Donjon\n ");
             }

         }
         else
         {
             if(choix==0)
             {
                 printf("une arrete de tour forcee!!\n");
                 FinTourForce=1;
             }
             else
             {

                 marquerDeplacementsPossibles(carte,unitesJoueurs[indice].ligne,unitesJoueurs[indice].colonne,unitesJoueurs[indice].mouvements);
                 afficherCarteJeu(ecran,carte,nbJoueurs,joueurs,nbVillages,villages,unitesJoueurs);
                 printf("Confirmez le deplacement (non=0 oui=1):\n");
                 int confirmation;
                 scanf("%d",&confirmation);
                 if(confirmation)
                 {
                     int ligne,colonne;
                     do
                     {
                         printf("donner la ligne de la case a jouer (depuis les cases possibles marques de + ) : ");
                         scanf("%d",&ligne);
                         ligne--;
                         printf("\n");
                         printf("donner la colonne de la case a jouer  (deouis les cases possible marques de + ) : ");
                         scanf("%d",&colonne);
                         colonne--;
                         printf("\n");

                     }
                     while((ligne>NB_LIGNES_CARTE || colonne>NB_COLONNES_CARTE) || carte[ligne][colonne].marque==0);
                     deplacerUnite(0,indice,unitesJoueurs[indice].idUnite,unitesJoueurs[indice].ligne,unitesJoueurs[indice].colonne,ligne,colonne,nbJoueurs,nbVillages,carte,joueurs,unitesJoueurs,villages);
                     effacerDeplacementsPossibles(carte);
                     afficherCarteJeu(ecran,carte,nbJoueurs,joueurs,nbVillages,villages,unitesJoueurs);
                     int code=calculerCodeAttaque(joueurs[0].groupe_allies,unitesJoueurs[indice].ligne,unitesJoueurs[indice].colonne,nbJoueurs,joueurs,carte);
                     if(code!=0)
                     {
                         int ligne_ennemie;
                         int colonne_ennemie;
                         printf("il ya une unite ennemie!!!!\n ");
                         int confirmation_attack=0;
                         printf("Voulez -vous attacker cette unite ennemie ? ");
                         scanf("%d",&confirmation_attack);
                         if(confirmation_attack)
                         {
                             do
                             {
                                 printf("dooner la ligne de l'ennemie : ");

                                 scanf("%d",&ligne_ennemie);
                                 printf("donner la colonne de l'ennemie : ");
                                 ligne_ennemie--;
                                 scanf("%d",&colonne_ennemie);
                                 colonne_ennemie--;
                             }
                             while(carte[ligne_ennemie][colonne_ennemie].symboleJoueur!='E');
                             if(verifierUniteAttaquable(code,ligne,colonne,ligne_ennemie,colonne_ennemie))
                             {
                                 int id_typeterrain_ennemie=carte[ligne_ennemie][colonne_ennemie].idTypeTerrain;
                                 int id_typeterrain_joueur=carte[ligne][colonne].idTypeTerrain;
                                 Attaque attaques[10][2];
                                 Attaque attaques_choisie[2];
                                 int resistances_choisie[2];
                                 int resistances[10][2];
                                 int indice_ennemie=chercherIndiceUniteJoueur(carte[ligne_ennemie][colonne_ennemie].idUnite,2,1,unitesJoueurs);

                                 int indice_fich_unite_joueur=chercherIndiceFicheTypeUnite(unitesJoueurs[indice].idFicheTypeUnite,nbFichesTypesUnites,fichesTypesUnites);

                                 int indice_fich_unite_ennemie=chercherIndiceFicheTypeUnite(unitesJoueurs[indice_ennemie].idFicheTypeUnite,nbFichesTypesUnites,fichesTypesUnites);

                                 int nombre_attaques=determinerPossibilitesAttaque(0,1,indice_fich_unite_joueur,indice_fich_unite_ennemie,fichesTypesUnites,indice,indice_ennemie,attaques,resistances);
                                 //int defense_joueur=fichesTypesUnites[indice_fich_unite_joueur].relationsTerrains[

                                 int relation= carte[unitesJoueurs[indice].ligne][unitesJoueurs[indice].colonne].idTypeTerrain;
                                 int  defense_humain=fichesTypesUnites[indice_fich_unite_joueur].resistances[relation];
                                 printf("defense  = %d realtion %d \n",defense_humain,relation);
                                 int relation_ennemie= carte[unitesJoueurs[indice_ennemie].ligne][unitesJoueurs[indice_ennemie].colonne].idTypeTerrain;
                                 int  defense_ennemie=fichesTypesUnites[indice_fich_unite_ennemie].resistances[relation_ennemie];


                                 int choix_attack=choisirAttaque(0,1,indice_fich_unite_joueur,indice_fich_unite_ennemie,fichesTypesUnites,indice,indice_ennemie,defense_humain,defense_ennemie,nombre_attaques,attaques,resistances,attaques_choisie,resistances_choisie,periode,unitesJoueurs[indice].alignement,unitesJoueurs[indice_ennemie].alignement);

                                 if(choix_attack!=0)
                                 {

                                     attaque(indice,indice_ennemie,0,1,ligne,colonne,ligne_ennemie,colonne_ennemie,carte,unitesJoueurs,joueurs,defense_humain,defense_humain,attaques_choisie,resistances_choisie,periode,unitesJoueurs[indice].alignement,unitesJoueurs[indice_ennemie].alignement);
                                 }
                             }
                         }
                         else
                         {
                             printf("cette unite n'est pas attaquable");
                         }
                     }
                     unitesJoueurs[indice].finTour=1;
                     unitesJoueurs[indice].mouvements=0;
                 }
                 else
                 {
                     effacerDeplacementsPossibles(carte);
                 }
             }

         }
         if(verifierFinTours(1,unitesJoueurs)==1)
         {
             fintour= 1;


         }


     }
     while( verifierGagnant(nbJoueurs,joueurs,unitesJoueurs)==0&& FinTourForce==0 && choix!=0 && fintour==0);
    */
    effacerDeplacementsPossibles(carte);
    afficherCarteJeu(ecran,img_tab,characters,carte,nbJoueurs,joueurs,nbVillages,villages,unitesJoueurs);
    SDL_Flip(ecran);
    int codegagnant=verifierGagnant(nbJoueurs,joueurs,unitesJoueurs);

    return codegagnant;
}

int jouerTourEnnemi(int numJoueur, int groupeAlliesJoueur, int nbFichesTypesUnites, int nbJoueurs,int nbUnitesMagasin, int nbVillages,FicheTypeUnite fichesTypesUnites[MAX_LIGNES_FICHES_TYPES_UNITES],Joueur joueurs[MAX_LIGNES_JOUEURS],CelluleCarte carte[NB_LIGNES_CARTE][NB_COLONNES_CARTE],UniteMagasin unitesMagasin[MAX_LIGNES_UNITESMAGASIN],Village villages[MAX_LIGNES_VILLAGES],TypeTerrain typesTerrains[NB_TYPES_TERRAINS],UniteJoueur unitesJoueurs[MAX_LIGNES_UNITESJOUEURS],Periode periode)
{


    if(joueurs[numJoueur].nombreUnites<=3)
    {

        int indice_ennemie = chercherIndiceChefUnitesJoueurs(2,unitesJoueurs);


        acheterUnitesAuto(joueurs[numJoueur].idJoueur,unitesJoueurs[indice_ennemie].ligne,unitesJoueurs[indice_ennemie].colonne,nbUnitesMagasin,nbJoueurs,nbFichesTypesUnites,unitesMagasin,unitesJoueurs,joueurs,fichesTypesUnites,carte);

    }
    else
    {
        for(int i = 0 ; i< MAX_LIGNES_UNITESJOUEURS ; i++)
        {
            if(unitesJoueurs[i].active && unitesJoueurs[i].idJoueur ==2&& unitesJoueurs[i].mouvements!=0)
            {
                printf("deplacement de l'unite %d efectue \n",unitesJoueurs[i].idUnite);
                int codeAttaque = choixAutomatiqueUniteEnnemie(i,-1,unitesJoueurs[i].ligne,unitesJoueurs[i].colonne,nbJoueurs,carte,joueurs,unitesJoueurs);
                if(codeAttaque == -1 )
                {
                    deplacementAutomatiqueUnite(i,unitesJoueurs[i].ligne,unitesJoueurs[i].colonne,joueurs[numJoueur].groupe_allies,nbJoueurs,nbVillages,carte,joueurs,unitesJoueurs,villages);
                }
                else
                {

                    Attaque attaques[10][2];
                    Attaque attaques_choisie[2];
                    int resistances_choisie[2];
                    int resistances[10][2];
                    int indice_fich_ennemie =chercherIndiceFicheTypeUnite(unitesJoueurs[codeAttaque].idFicheTypeUnite,nbFichesTypesUnites,fichesTypesUnites);
                    int indice_fich_joueur = chercherIndiceFicheTypeUnite(unitesJoueurs[i].idFicheTypeUnite,nbFichesTypesUnites,fichesTypesUnites);

                    int nb_attack=determinerPossibilitesAttaque(1,0,indice_fich_joueur,indice_fich_ennemie,fichesTypesUnites,i,codeAttaque,attaques,resistances);

                    choisirAttaqueAutomatiquement(1,0,indice_fich_joueur,indice_fich_ennemie,fichesTypesUnites,i,codeAttaque,50,50,nb_attack,attaques,resistances,attaques_choisie,resistances_choisie,periode,unitesJoueurs[i].alignement,unitesJoueurs[codeAttaque].alignement);
                    attaque(i,codeAttaque,1,0,unitesJoueurs[i].ligne,unitesJoueurs[i].colonne,unitesJoueurs[codeAttaque].ligne,unitesJoueurs[codeAttaque].colonne,carte,unitesJoueurs,joueurs,50,50,attaques_choisie,resistances_choisie,periode,unitesJoueurs[i].alignement,unitesJoueurs[codeAttaque].alignement);
                }
            }
        }
    }
    int codegagnant=verifierGagnant(nbJoueurs,joueurs,unitesJoueurs);

    return codegagnant;

}
void acheterUnitesAuto(int idJoueur, int ligneDonjon, int colonneDonjon, int nbUnitesMagasin, int nbJoueurs, int nbFichesTypesUnites,UniteMagasin unitesMagasin[MAX_LIGNES_UNITESMAGASIN],UniteJoueur unitesJoueurs[MAX_LIGNES_UNITESJOUEURS],Joueur joueurs[MAX_LIGNES_JOUEURS],FicheTypeUnite fichesTypesUnites[MAX_LIGNES_FICHES_TYPES_UNITES],CelluleCarte carte[NB_LIGNES_CARTE][NB_COLONNES_CARTE])
{

    if(carte[ligneDonjon][colonneDonjon].idTypeTerrain==ID_TYPE_TERRAIN_DONJON)
    {

        int l ;
        int nb_unite_villain = 0 ;
        for(int i = 0 ; i<MAX_LIGNES_UNITESJOUEURS ; i++)
        {
            if(unitesJoueurs[i].active && unitesJoueurs[i].idJoueur==2 )
            {
                nb_unite_villain++;
            }
        }
        if(nb_unite_villain < 10)
        {

            int indice_joueur=chercherIndiceJoueur(idJoueur,nbJoueurs,joueurs);
            int indice_unite=chercherMaxIdUnitesJoueurs(unitesJoueurs);
            int min_value =1e9, index = -1 ;
            for(int  i = 0 ; i< nbUnitesMagasin ; i++)
            {


                int indice_fich =chercherIndiceFicheTypeUnite(unitesMagasin[i].idFicheTypeUnite,nbFichesTypesUnites,fichesTypesUnites);

                if(min_value>fichesTypesUnites[indice_fich].prix &&  unitesMagasin[i].idJoueurAutorise==2)
                {

                    min_value=minimum(min_value,fichesTypesUnites[indice_fich].prix);
                    index=indice_fich;
                }

            }

            int chef_ennemie = chercherIndiceChefUnitesJoueurs(idJoueur,unitesJoueurs);
            int chef_humain = chercherIndiceChefUnitesJoueurs(1,unitesJoueurs);
            int new_ligne  = -1, new_colonne = -1 ;
            marquerDeplacementsPossibles(carte,ligneDonjon,colonneDonjon,unitesJoueurs[chef_ennemie].mouvements);
            int calcul_dist_min=1e9;//1e9 est  10**9
            int ligne_humain_chef= unitesJoueurs[chef_humain].ligne;
            int colonne_humain_chef = unitesJoueurs[chef_humain].colonne;
            int ligne_ennemie = unitesJoueurs[chef_ennemie].ligne, colonne_ennemie = unitesJoueurs[chef_ennemie].colonne ;



            for(int i = ligne_ennemie-2 ; i<= ligne_ennemie+2 ; i++)
            {

                for(int  j = colonne_ennemie-2 ; j<=colonne_ennemie+2 ; j++)
                {

                    if(carte[i][j].idUnite==0 && carte[i][j].idTypeTerrain==ID_TYPE_TERRAIN_CHATEAU)
                    {
                        new_ligne =  i;
                        new_colonne = j ;
                    }
                }
            }
            int indice_fich = chercherIndiceFicheTypeUnite(unitesJoueurs[index].idFicheTypeUnite,nbFichesTypesUnites,fichesTypesUnites);

            if(joueurs[indice_joueur].or>=fichesTypesUnites[indice_fich].prix && indice_fich != -1)
            {


                initialiserNouvelleUniteJoueur(indice_unite,indice_fich,idJoueur,new_ligne,new_colonne,unitesJoueurs,fichesTypesUnites);
                carte[new_ligne][new_colonne].idUnite=indice_unite+1;
                carte[new_ligne][new_colonne].symboleJoueur=joueurs[indice_joueur].symbole;

                miseAJourJoueurApresAchatUnite( indice_unite,1,indice_fich,unitesJoueurs,joueurs,fichesTypesUnites);



            }

            effacerDeplacementsPossibles(carte);
        }
        else
        {

            int indice_joueur=chercherIndiceJoueur(idJoueur,nbJoueurs,joueurs);
            int indice_unite=chercherMaxIdUnitesJoueurs(unitesJoueurs);
            int max_value =0, index = -1 ;
            for(int  i = 0 ; i< nbUnitesMagasin ; i++)
            {


                int indice_fich =chercherIndiceFicheTypeUnite(unitesMagasin[i].idFicheTypeUnite,nbFichesTypesUnites,fichesTypesUnites);

                if(max_value<fichesTypesUnites[indice_fich].prix)
                {

                    max_value=maximum(max_value,fichesTypesUnites[indice_fich].prix);
                    index=indice_fich;
                }

            }

            int chef_ennemie = chercherIndiceChefUnitesJoueurs(idJoueur,unitesJoueurs);
            int chef_humain = chercherIndiceChefUnitesJoueurs(1,unitesJoueurs);
            int new_ligne  = -1, new_colonne = -1 ;
            marquerDeplacementsPossibles(carte,ligneDonjon,colonneDonjon,unitesJoueurs[chef_ennemie].mouvements);
            int calcul_dist_min=1e9;//1e9 est  10**9
            int ligne_humain_chef= unitesJoueurs[chef_humain].ligne;
            int colonne_humain_chef = unitesJoueurs[chef_humain].colonne;
            int ligne_ennemie = unitesJoueurs[chef_ennemie].ligne, colonne_ennemie = unitesJoueurs[chef_ennemie].colonne ;



            for(int i = ligne_ennemie-2 ; i<= ligne_ennemie+2 ; i++)
            {

                for(int  j = colonne_ennemie-2 ; j<=colonne_ennemie+2 ; j++)
                {

                    if(carte[i][j].idUnite==0 && carte[i][j].idTypeTerrain==ID_TYPE_TERRAIN_CHATEAU)
                    {
                        new_ligne =  i;
                        new_colonne = j ;
                    }
                }
            }
            int indice_fich = chercherIndiceFicheTypeUnite(unitesJoueurs[index].idFicheTypeUnite,nbFichesTypesUnites,fichesTypesUnites);

            if(joueurs[indice_joueur].or>=fichesTypesUnites[indice_fich].prix && indice_fich != -1)
            {

                initialiserNouvelleUniteJoueur(indice_unite,indice_fich,idJoueur,new_ligne,new_colonne,unitesJoueurs,fichesTypesUnites);
                carte[new_ligne][new_colonne].idUnite=indice_unite+1;
                carte[new_ligne][new_colonne].symboleJoueur=joueurs[indice_joueur].symbole;

                miseAJourJoueurApresAchatUnite( indice_unite,1,indice_fich,unitesJoueurs,joueurs,fichesTypesUnites);




            }
            effacerDeplacementsPossibles(carte);
        }

    }


    afficherJoueurJeu(idJoueur,nbJoueurs,joueurs);


    return;
}
void deplacementAutomatiqueUnite(int indiceUnite, int ligne_courante, int colonne_courante,int groupeAlliesJoueur, int nbJoueurs, int nbVillages,CelluleCarte carte[NB_LIGNES_CARTE][NB_COLONNES_CARTE],Joueur joueurs[MAX_LIGNES_JOUEURS],UniteJoueur unitesJoueurs[MAX_LIGNES_UNITESJOUEURS],Village villages[MAX_LIGNES_VILLAGES] )
{

    marquerDeplacementsPossibles(carte,ligne_courante,colonne_courante,unitesJoueurs[indiceUnite].mouvements);


    int health = 20033, indice_ennemie = -1 ;
    for(int i = 0 ; i<MAX_LIGNES_UNITESJOUEURS ; i++)
    {


        if( unitesJoueurs[i].active && unitesJoueurs[i].idJoueur==1)
        {
            if(unitesJoueurs[i].pv<health)
            {
                indice_ennemie = i ;
                health  =  unitesJoueurs[i].pv;
            }
        }
    }

    int nouvelle_ligne = -1, nouvelle_colonne = -1 ;
    int ligne_ennemie = unitesJoueurs[indice_ennemie].ligne, colonne_ennemie = unitesJoueurs[indice_ennemie].colonne;

    if(carte[ligne_ennemie][colonne_ennemie-1].idUnite==0&&carte[ligne_ennemie][colonne_ennemie-1].marque )
    {
        nouvelle_ligne = ligne_ennemie;
        nouvelle_colonne= colonne_ennemie-1;

    }
    if(carte[ligne_ennemie-1][colonne_ennemie].idUnite== 0 && carte[ligne_ennemie-1][colonne_ennemie].marque==1)
    {
        nouvelle_ligne = ligne_ennemie-1;
        nouvelle_colonne = colonne_ennemie;

    }

    if( carte[ligne_courante+1][colonne_ennemie].idUnite==0 &&carte[ligne_courante+1][colonne_ennemie].marque )
    {
        nouvelle_ligne = ligne_ennemie+1;
        nouvelle_colonne = colonne_ennemie;

    }
    if( carte[ligne_courante][colonne_courante+1].idUnite==0 && carte[ligne_courante][colonne_courante+1].marque )
    {

        nouvelle_ligne = ligne_ennemie;
        nouvelle_colonne = colonne_ennemie+1;

    }

    if(nouvelle_colonne!=-1 && nouvelle_colonne!=-1 && carte[nouvelle_ligne][nouvelle_colonne].marque==1&&carte[nouvelle_ligne][nouvelle_colonne].idUnite==0)
    {

        //printf("avant carte ligne courant  : %d colonne :%d idunite : %d %c %d \n",ligne_courante+1,colonne_courante+1,carte[ligne_courante][colonne_courante].idUnite,carte[ligne_courante][colonne_courante].symboleJoueur,   carte[ligne_courante][colonne_courante].marque);
        carte[nouvelle_ligne][nouvelle_colonne].idUnite=unitesJoueurs[indiceUnite].idUnite;
        carte[nouvelle_ligne][nouvelle_colonne].symboleJoueur=joueurs[1].symbole;
        carte[nouvelle_ligne][nouvelle_colonne].marque=0;
        carte[ligne_courante][colonne_courante].idUnite=0;
        carte[ligne_courante][colonne_courante].symboleJoueur=' ';
        carte[ligne_courante][colonne_courante].marque=0;
        // printf("apres carte ligne courant  : %d colonne :%d idunite : %d %c %d \n",ligne_courante+1,colonne_courante+1,carte[ligne_courante][colonne_courante].idUnite,carte[ligne_courante][colonne_courante].symboleJoueur,   carte[ligne_courante][colonne_courante].marque);




        unitesJoueurs[indiceUnite].colonne = nouvelle_colonne;
        unitesJoueurs[indiceUnite].ligne = nouvelle_ligne;
        int mouvements = unitesJoueurs[indiceUnite].mouvements;

        int mouvementsRestants = mouvements - (abs(ligne_courante - nouvelle_ligne) + abs(colonne_courante - nouvelle_colonne));
        if (mouvementsRestants < 0)
        {
            mouvementsRestants = 0;
        }
        unitesJoueurs[indiceUnite].mouvements=mouvementsRestants ;
    }
    else
    {




        int dist = 2003 ;
        for(int  i = 0 ; i< NB_LIGNES_CARTE; i++)
        {

            for(int j = 0 ; j< NB_COLONNES_CARTE ; j++)
            {

                if( carte[i][j].marque==1 && carte[i][j].idUnite==0 )
                {

                    int distance =((abs(unitesJoueurs[indice_ennemie].ligne - i )*abs(unitesJoueurs[indice_ennemie].ligne - i ))+abs(unitesJoueurs[indice_ennemie].colonne-j)*abs(unitesJoueurs[indice_ennemie].colonne-j));


                    if( distance<dist)
                    {
                        dist = distance;
                        nouvelle_colonne = j ;
                        nouvelle_ligne = i ;
                    }

                }

            }



        }
        /** si on a plusieurs unites dont leurs pv est egal a la varible health donc on doit determiner l'unite la plus proche de notre unite **/



        /*int indicevillage =-1 ;
        int village_capter = 0,ligne_village=-1, colonne_village=-1;
        for(int i = 0 ; i<NB_LIGNES_CARTE; i++)
        {
            for(int j = 0 ; j <NB_COLONNES_CARTE ; j++ )
            {
                if(carte[i][j].idUnite==0 && carte[i][j].idTypeTerrain == ID_TYPE_TERRAIN_VILLAGE && carte[i][j].marque)
                {

                    if(!village_capter)
                    {
                        int indice_village = chercherIndiceVillage(i,j,nbVillages,villages);
                        if(villages[indice_village].idJoueurProprietaire==0)
                        {
                            indicevillage=indice_village;
                            int posJ=chercherIndiceJoueur(2,nbJoueurs,joueurs);
                            villages[indice_village].idJoueurProprietaire=joueurs[posJ].idJoueur;
                            joueurs[posJ].nombreVillages++;
                            miseAJourJoueurApresOccupationVillage(posJ,joueurs);
                            village_capter = 1 ;
                        }
                        else
                        {
                            if(villages[indice_village].idJoueurProprietaire==1)
                            {
                                indicevillage=indice_village;

                                joueurs[0].nombreVillages--;
                                miseAJourJoueurApresOccupationVillage(0,joueurs);
                                int indice_ennemie = chercherIndiceJoueur(2,nbJoueurs,joueurs);
                                joueurs[indice_ennemie].nombreVillages++;
                                miseAJourJoueurApresOccupationVillage(indice_ennemie,joueurs);
                                village_capter=1;
                            }
                        }
                    }

                }

            }
            if(village_capter)
            {

                break;
            }

        }


        if(village_capter)
        {
            nouvelle_ligne =ligne_village;
            nouvelle_colonne= colonne_village;
        }
        */

        if(nouvelle_ligne!=-1 && nouvelle_colonne!=-1 && carte[nouvelle_ligne][nouvelle_colonne].idUnite==0  &&carte[nouvelle_ligne][nouvelle_colonne].marque==1)
        {

            printf("avant carte ligne courant  : %d colonne :%d idunite : %d %c %d \n",ligne_courante+1,colonne_courante+1,carte[ligne_courante][colonne_courante].idUnite,carte[ligne_courante][colonne_courante].symboleJoueur,   carte[ligne_courante][colonne_courante].marque);
            carte[nouvelle_ligne][nouvelle_colonne].idUnite=unitesJoueurs[indiceUnite].idUnite;
            carte[nouvelle_ligne][nouvelle_colonne].symboleJoueur=joueurs[1].symbole;
            carte[nouvelle_ligne][nouvelle_colonne].marque=0;
            carte[ligne_courante][colonne_courante].idUnite=0;
            carte[ligne_courante][colonne_courante].symboleJoueur=' ';
            carte[ligne_courante][colonne_courante].marque=0;
            printf("apres carte ligne courant  : %d colonne :%d idunite : %d %c %d \n",nouvelle_ligne,nouvelle_colonne,carte[ligne_courante+1][colonne_courante+1].idUnite,carte[ligne_courante+1][colonne_courante+1].symboleJoueur,   carte[ligne_courante][colonne_courante].marque);


            unitesJoueurs[indiceUnite].colonne = nouvelle_colonne;
            unitesJoueurs[indiceUnite].ligne = nouvelle_ligne;
            int mouvements = unitesJoueurs[indiceUnite].mouvements;

            int mouvementsRestants = mouvements - (abs(ligne_courante - nouvelle_ligne) + abs(colonne_courante - nouvelle_colonne));
            if (mouvementsRestants < 0)
            {
                mouvementsRestants = 0;
            }
            unitesJoueurs[indiceUnite].mouvements=mouvementsRestants ;

        }
    }



    effacerDeplacementsPossibles(carte);
}
int choisirAttaqueAutomatiquement(int numJoueur, int numEnnemi,int indiceficheTypeUniteJoueur, int indiceficheTypeUniteEnnemi,FicheTypeUnite fichesTypesUnites[MAX_LIGNES_FICHES_TYPES_UNITES],int indiceDansUnitesJoueurs, int indiceEnnemiDansUnitesJoueurs,int defenseJoueur, int defenseEnnemi, int nbrAttaques, Attaque attaques[10][2],int resistances[10][2], Attaque attaqueChoisie[2],int resistanceAuxAttaquesChoisis[2], Periode periode, int alignementJoueur,int alignementEnnemi)
{


    int choix  = 0,degat = 0 ;
    for(int i = 0 ; i<nbrAttaques; i++)
    {
        if(attaques[i][0].degat>degat)
        {
            choix = i ;
            degat = attaques[i][0].degat;
        }
    }


    attaqueChoisie[0]=attaques[choix][0];
    attaqueChoisie[1]=attaques[choix][1];
    resistanceAuxAttaquesChoisis[0]=resistances[choix][0];
    resistanceAuxAttaquesChoisis[1]=resistances[choix][1];
    return choix;

}


void deroulerJeu(SDL_Surface *ecran,int nbFichesTypesUnite, int nbJoueurs,int nbUnitesMagasin,int nbVillages,FicheTypeUnite fichesTypesUnites[MAX_LIGNES_FICHES_TYPES_UNITES],Joueur joueurs[MAX_LIGNES_JOUEURS],CelluleCarte carte[NB_LIGNES_CARTE][NB_COLONNES_CARTE],UniteMagasin unitesMagasin[MAX_LIGNES_UNITESMAGASIN],Village villages[MAX_LIGNES_VILLAGES],TypeTerrain typesTerrains[NB_TYPES_TERRAINS],Periode periodes[NB_LIGNES_PERIODES],UniteJoueur unitesJoueurs[MAX_LIGNES_UNITESJOUEURS])
{
    int tour=1;
    Tour = tour ;
    //afficherCarteJeu(ecran,carte,nbJoueurs,joueurs, nbVillages,villages,unitesJoueurs);
    int i ;
    int code_gagnant=0;

    int indice_humain=chercherIndiceJoueurHumain(nbJoueurs,joueurs);
    // jouerTourHumain(ecran,joueurs[i].idJoueur,joueurs[i].groupe_allies,nbFichesTypesUnite,nbJoueurs,nbUnitesMagasin,nbVillages,fichesTypesUnites,joueurs,carte,unitesMagasin,villages,typesTerrains,unitesJoueurs,periodes[tour%6]);
    do
    {
        for(int i=0; i<nbJoueurs; i++)
        {


            if(i==indice_humain)
            {
                printf("*****TOUR%d*****\n",tour);
                printf("*****HUMAIN*****\n");
                code_gagnant= jouerTourHumain(ecran,joueurs[i].idJoueur,joueurs[i].groupe_allies,nbFichesTypesUnite,nbJoueurs,nbUnitesMagasin,nbVillages,fichesTypesUnites,joueurs,carte,unitesMagasin,villages,typesTerrains,unitesJoueurs,periodes[tour%6]);
                verifierFinTours(joueurs[i].idJoueur,unitesJoueurs);
                reinitialiserTours(joueurs[i].idJoueur,nbJoueurs,joueurs,unitesJoueurs);

            }
            else
            {

               printf("tour skipped \n");
                int grp_allies=chercherGroupeAllies(joueurs[i].symbole,nbJoueurs,joueurs);
                printf("*****TOUR%d*****\n",tour);
                printf("*****ENNEMIE*****\n");
                jouerTourEnnemi(1,joueurs[1].groupe_allies,nbFichesTypesUnite,nbJoueurs,nbUnitesMagasin,nbVillages,fichesTypesUnites,joueurs,carte,unitesMagasin,villages,typesTerrains,unitesJoueurs,periodes[tour%6]);

                reinitialiserTours(joueurs[i].idJoueur,nbJoueurs,joueurs,unitesJoueurs);






            }
        }

        tour++;
        Tour = tour;
    }
    while( code_gagnant==0 && jouerTourHumain(ecran,joueurs[i].idJoueur,joueurs[i].groupe_allies,nbFichesTypesUnite,nbJoueurs,nbUnitesMagasin,nbVillages,fichesTypesUnites,joueurs,carte,unitesMagasin,villages,typesTerrains,unitesJoueurs,periodes[tour%6])==0  );
    SDL_Color rouge = { 255, 0, 0 };
    SDL_Color couleur_a_utiliser= rouge;
    SDL_Rect positionIMG,positionText,positionTitre;
    positionTitre.x=0;
    positionTitre.y=0;
    TTF_Font* font = NULL;
    SDL_Surface* texte = NULL;
    font = TTF_OpenFont("assets/Oswald-VariableFont_wght.ttf", 25);
    if(code_gagnant==2)
    {

             SDL_Surface *menu = IMG_Load("assets/defeat.png");
        positionTitre.x = 0;
        positionTitre.y = 0;
         SDL_BlitSurface(menu, NULL, ecran, &positionTitre);
        SDL_Flip(ecran);
               SDL_Delay(5000);

    }
    else
    {
        SDL_Surface *menu = IMG_Load("assets/victory.png");
        positionTitre.x = 0;
        positionTitre.y = 0;
         SDL_BlitSurface(menu, NULL, ecran, &positionTitre);

        SDL_Flip(ecran);
        SDL_Delay(5000);



    }



}



int choixAutomatiqueUniteEnnemie(int indice_Unite,int codeAttaque, int ligne_courante, int colonne_courante,int nbJoueurs, CelluleCarte carte[NB_LIGNES_CARTE][NB_COLONNES_CARTE],Joueur joueurs[MAX_LIGNES_JOUEURS],UniteJoueur unitesJoueurs[MAX_LIGNES_UNITESJOUEURS])
{


    codeAttaque = -1 ;
    if(carte[ligne_courante-1][colonne_courante].idUnite!=0 && carte[ligne_courante-1][colonne_courante].symboleJoueur=='J')
    {

        for(int i = 0 ; i<MAX_LIGNES_UNITESJOUEURS; i++)
        {
            if(unitesJoueurs[i].active && unitesJoueurs[i].idJoueur==1 &&unitesJoueurs[i].ligne == ligne_courante-1 && unitesJoueurs[i].colonne == colonne_courante)
            {
                return i ;
            }
        }
    }
    if(carte[ligne_courante+1][colonne_courante].idUnite!=0 && carte[ligne_courante+1][colonne_courante].symboleJoueur=='J')
    {

        for(int i = 0 ; i<MAX_LIGNES_UNITESJOUEURS; i++)
        {
            if(unitesJoueurs[i].active && unitesJoueurs[i].idJoueur==1 &&unitesJoueurs[i].ligne == ligne_courante+1 && unitesJoueurs[i].colonne == colonne_courante)
            {
                return i ;
            }
        }
    }
    if(carte[ligne_courante][colonne_courante+1].idUnite!=0 && carte[ligne_courante][colonne_courante+1].symboleJoueur=='J')
    {

        for(int i = 0 ; i<MAX_LIGNES_UNITESJOUEURS; i++)
        {
            if(unitesJoueurs[i].active && unitesJoueurs[i].idJoueur==1 &&unitesJoueurs[i].ligne == ligne_courante && unitesJoueurs[i].colonne == colonne_courante+1)
            {
                return i ;
            }
        }
    }
    if(carte[ligne_courante][colonne_courante-1].idUnite!=0 && carte[ligne_courante][colonne_courante-1].symboleJoueur=='J')
    {

        for(int i = 0 ; i<MAX_LIGNES_UNITESJOUEURS; i++)
        {
            if(unitesJoueurs[i].active && unitesJoueurs[i].idJoueur==1 &&unitesJoueurs[i].ligne == ligne_courante && unitesJoueurs[i].colonne == colonne_courante-1)
            {
                return i ;
            }
        }
    }
    return -1;






}


int main(int argc, char *argv[])
{
    /** main **/
    UniteJoueur unitesJoueurs[MAX_LIGNES_UNITESJOUEURS];
    Joueur joueurs[MAX_LIGNES_JOUEURS];
    UniteMagasin unitesMagasin[MAX_LIGNES_UNITESMAGASIN];
    Village villages[MAX_LIGNES_VILLAGES];
    TypeTerrain typesTerrains[NB_TYPES_TERRAINS];
    Periode periodes[MAX_LIGNES_PERIODES];
    CelluleCarte carte[NB_LIGNES_CARTE][ NB_COLONNES_CARTE];
    FicheTypeUnite fichesTypesUnites[MAX_LIGNES_FICHES_TYPES_UNITES];

    int nbLignesFichesTypesUnites,nbVillages,nbUnitesMagasin,nbJoueurs,nbLignesVillages;
    /** GRAPHIQUE ***/


    SDL_Surface *ecran = NULL, *menu = NULL;
    SDL_Rect positionMenu;
    SDL_Event event;
    TTF_Font* font = NULL;

    SDL_Surface* texte = NULL;
    int continuer = 1;

    SDL_Init(SDL_INIT_VIDEO);

    SDL_WM_SetIcon(IMG_Load("assets/icon.jpg"), NULL); // L'icône doit être chargée avant SDL_SetVideoMode
    ecran = SDL_SetVideoMode(1250,750, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
    SDL_WM_SetCaption("Wesnoath battle ", NULL);
    int indiceOption=0;

    menu = IMG_Load("assets/wesnoath_bg.jpg");
    positionMenu.x = 0;
    positionMenu.y = 0;
    SDL_FillRect(ecran, NULL, SDL_MapRGB(ecran->format, 0, 0, 0));

    SDL_BlitSurface(menu, NULL, ecran, &positionMenu);

    TTF_Init();
    font = TTF_OpenFont("assets/HeadingNowTrial-45Medium.ttf", 25);

    SDL_Color noir = {0, 0, 0}; //attention ce n'est pas un Uint32
    SDL_Color blanc = { 255, 255, 255 };
    SDL_Color rouge = { 255, 0, 0 };
    SDL_Rect positionText;

    int choix  = 0 ;
    while (continuer)
    {
        SDL_WaitEvent(&event);
        switch(event.type)
        {
        case SDL_QUIT:
            continuer = 0;
            break;
        case SDL_KEYDOWN:
            switch(event.key.keysym.sym)
            {
            case SDLK_ESCAPE: // Veut arrêter le jeu

                choix = 0;
                break;
            case SDLK_KP_ENTER:
            case SDLK_RETURN: // Veut arrêter le jeu
                if(indiceOption==0)
                {
                    initialiserNouveauJeu(&nbLignesFichesTypesUnites,&nbJoueurs,&nbUnitesMagasin,&nbVillages,fichesTypesUnites,joueurs,carte,unitesMagasin,villages,typesTerrains,periodes,unitesJoueurs);
                    //afficherTableauxJeu(nbLignesFichesTypesUnites,nbJoueurs,nbUnitesMagasin,nbVillages,fichesTypesUnites,joueurs,carte,unitesMagasin,villages,typesTerrains,periodes,unitesJoueurs);
                    deroulerJeu(ecran,nbLignesFichesTypesUnites,nbJoueurs,nbUnitesMagasin,nbVillages,fichesTypesUnites,joueurs,carte,unitesMagasin,villages,typesTerrains,periodes,unitesJoueurs);
                    //afficherCarteJeu(ecran,carte,nbJoueurs,joueurs,nbVillages,villages,unitesJoueurs)
                    afficherCarte(ecran,carte);
                    choix  = 1 ;
                    break;
                }
                else if(indiceOption==1)
                {
                    chargerJeuComplet(&nbLignesFichesTypesUnites,&nbJoueurs,&nbUnitesMagasin,&nbVillages,fichesTypesUnites,joueurs,carte,unitesMagasin,villages,typesTerrains,periodes,unitesJoueurs);
                    afficherTableauxJeu(nbLignesFichesTypesUnites,nbJoueurs,nbUnitesMagasin,nbVillages,fichesTypesUnites,joueurs,carte,unitesMagasin,villages,typesTerrains,periodes,unitesJoueurs);
                    deroulerJeu(ecran,nbLignesFichesTypesUnites,nbJoueurs,nbUnitesMagasin,nbVillages,fichesTypesUnites,joueurs,carte,unitesMagasin,villages,typesTerrains,periodes,unitesJoueurs);

                }
                if(indiceOption==2)
                {
                    //editeur(ecran);
                    return 0 ;
                }
                break;
            case SDLK_UP:
                if(indiceOption>0)
                {
                    indiceOption--;
                }
                break;
            case SDLK_DOWN:
                if(indiceOption<2)
                {
                    indiceOption++;
                }
                break;
            }
            break;

        }




        if(font!= 0)
        {
              SDL_Surface *menu = IMG_Load("assets/wesnoath_bg.jpg");
          positionText.x=0;
          positionText.y=0;
        SDL_BlitSurface(menu, NULL, ecran, &positionText);

            char* phrase[3];
            phrase[0]= "nouvelle partie ";
            phrase[1]= "Charger partie";
            phrase[2]= "Exit";

            int i, y_depart=400;
            SDL_Color couleur_a_utiliser;

            for(i=0; i<3; i++)
            {
                if(i==indiceOption)
                {
                    couleur_a_utiliser=rouge;
                }
                else
                {
                    couleur_a_utiliser=blanc;
                }
                SDL_Surface * border_titre =IMG_Load("assets/fin_tour.png");
                positionText.x = 570;
                positionText.y =  y_depart+i*60-5;
                SDL_BlitSurface(border_titre, NULL, ecran, &positionText);
                texte = TTF_RenderText_Blended(font, phrase[i], couleur_a_utiliser) ;
                positionText.x = 600;
                positionText.y = y_depart+i*60;
                if(i == 2)positionText.x+=50;
                SDL_BlitSurface(texte, NULL, ecran, &positionText);

            }


        }
        else
        {
            //logger l'erreur   "foirage à l'ouverture du font "
        }

        SDL_Flip(ecran);
    }
    SDL_FreeSurface(texte); //On oublie toujours pas
    TTF_CloseFont(font);
    SDL_FreeSurface(menu);



    TTF_Quit();
    SDL_Quit();

    printf("**************************************\n");
    printf("** Heroes Magic War - Character Mode**\n");
    printf("**************************************\n");
    printf("** 1 - Nouvelle partie              **\n");
    printf("** 2 - Charger partie               **\n");
    printf("** 0 - Quitter                      **\n");
    printf("**************************************\n");
    printf(" Entrez votre choix (1 ,2 ou 0) :");
    int choix_debut;
    scanf("%d",&choix_debut);
    if(choix_debut==1)
    {
        initialiserNouveauJeu(&nbLignesFichesTypesUnites,&nbJoueurs,&nbUnitesMagasin,&nbVillages,fichesTypesUnites,joueurs,carte,unitesMagasin,villages,typesTerrains,periodes,unitesJoueurs);
        afficherTableauxJeu(nbLignesFichesTypesUnites,nbJoueurs,nbUnitesMagasin,nbVillages,fichesTypesUnites,joueurs,carte,unitesMagasin,villages,typesTerrains,periodes,unitesJoueurs);
        deroulerJeu(ecran,nbLignesFichesTypesUnites,nbJoueurs,nbUnitesMagasin,nbVillages,fichesTypesUnites,joueurs,carte,unitesMagasin,villages,typesTerrains,periodes,unitesJoueurs);

    }
    if(choix_debut==2)
    {

        chargerJeuComplet(&nbLignesFichesTypesUnites,&nbJoueurs,&nbUnitesMagasin,&nbVillages,fichesTypesUnites,joueurs,carte,unitesMagasin,villages,typesTerrains,periodes,unitesJoueurs);
        afficherTableauxJeu(nbLignesFichesTypesUnites,nbJoueurs,nbUnitesMagasin,nbVillages,fichesTypesUnites,joueurs,carte,unitesMagasin,villages,typesTerrains,periodes,unitesJoueurs);
        deroulerJeu(ecran,nbLignesFichesTypesUnites,nbJoueurs,nbUnitesMagasin,nbVillages,fichesTypesUnites,joueurs,carte,unitesMagasin,villages,typesTerrains,periodes,unitesJoueurs);
    }
    if(choix_debut==0)
    {
        return 0;
    }
    srand(time(NULL));


    sauvegarderJeuComplet(nbVillages,nbJoueurs,villages,joueurs,unitesJoueurs);
    return 0;
}


