// gcc -Wall translate.c
// ./a.out inversion.txt

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define NbColons 200
#define NbLines 1000
long int HexToDec(char hex[]){
    long int base = 1;
    long int dec = 0;
    printf("test") ; 
    for (int i = strlen(hex)-1; i >= 0; i--){
        if (hex[i] >= '0' && hex[i] <= '9'){
            dec += (hex[i] - 48) * base;
            base *= 16;
        }
        else if (hex[i] >= 'a' && hex[i] <= 'f'){
            dec += (hex[i] - 87) * base;
            base *= 16;
        }
        else if (hex[i] >= 'A' && hex[i] <= 'F'){
            dec += (hex[i] - 55) * base;
            base *= 16;
        }
    }
    return dec;
}


int tagCollect(int index, char actualLine[], char allTags[NbLines][NbColons]) {
    // Fonction qui parcourt la ligne actuelle et stocke les etiquettes trouves dans le tableau "allTags"
    int tagPres=0;
    for (int i = 0; i < strlen(actualLine); i++) {
        if (actualLine[i] == ':') {
            tagPres = 1;
        }
    }
    int ind = 0;
    if (tagPres) {
        while (actualLine[ind] != ':') {
            int carVal = actualLine[ind];
            if (carVal < 48 || (carVal > 57 && carVal < 65) || (carVal > 90 && carVal < 97) || carVal > 122){
                //printf("Error : le format de l'etiquette n'est pas valide le caractere %c n'est pas accepte\n", carVal);
                return -1;
            }
            else {
                allTags[index][ind] = actualLine[ind];
                ind++;
            }
        }
        ind++;
        return 1;
    }
    return 0;
}

int tagDoubl(char allTags[NbLines][NbColons], int nbLines){
    // Fonction qui parcourt le tableaux d'etiquette et renvoie 1 si il trouve un doublon, et 0 sinon
    for (int i = 0; i < NbLines; i++){
        if (strcmp(allTags[i],"")){
            for (int j = i + 1; j < nbLines; j++){
                if (strcmp(allTags[i],allTags[j]) == 0){
                    return j;
                }
            }
        }
    }
    return 0;
}




int traduction(int index, char actualLine[], char allTags[NbLines][NbColons], int presTag, char translatedLine[NbColons]) {
    char* codeOpVals[32] = {"add", "sub", "mul", "div", "and", "or", "xor", "shl", "", "", "ldb", "ldw", "stb", "stw", "", "", "", "", "","", "jmp", "jzs", "jzc", "jcs", "jcc", "jns", "jnc", "in", "out", "rnd", "", "hlt"}; // Collection des commande du code opératoire
    long translatedLineBin[5]={0,0,0,0,0};
    int ind = 0; // Indice principal avec lequel on parcourt la ligne une fois
    int ErrorLine = 1 ;  // 1 si pas d'erreur
    if (presTag) { // On parcourt la ligne actuelle jusque la fin de l'etiquette
        while (actualLine[ind] != ':') {
            ind++;
        }
        ind++;
    }
    while (actualLine[ind] == ' '|| actualLine[ind] == '0' || actualLine[ind] == '1' || actualLine[ind] == '\t')  { // On parcourt la ligne actuelle jusqu'a ce qu'on tombe sur une commande
        ind++;
    }
    char command[10] = ""; // Chaine de caractere qui contient la commande de la ligne actuelle
    int j = 0; // Indice auxiliaire pour stocker lettre par lettre la commande actuelle
    while (actualLine[ind] != ' ' && actualLine[ind] != '\0' && actualLine[ind] != '\n') { // stockage de la commande actuelle
        command[j] = actualLine[ind];
        ind++;
        j++;
    }
    
    int i = 0; // indice qui va contenir le numero de la commande
    while (i < 32 && strcmp(codeOpVals[i], command)) { // la recherche du numero de la commande dans la Collection des commande du code opératoire
        i++;
    }
   
    if (i<32) { // Si la commande existe et est trouve on la traduit en binaire
        translatedLineBin[0] = i;
        
    } else {
        printf("Error : commande %s n'existe pas\n", command);
        ErrorLine = 0;
    }
    
    
    char parameters[3][15] = {"","",""}; // Tableau de caracteres qui contient tous les arguments de la ligne actuelle
    
    while (actualLine[ind] == ' ') {
        ind++;
    }
    
    int col = 0; // Indice pour stocker le numero de l'argument qu'on est en train de parcourir
    while (actualLine[ind] != '\n' && actualLine[ind] != '\0') { // On stocker tous les argument de la ligne actuelle
        j = 0; // Indice auxiliaire qui aide a stocker lettre par lettre un parametre qu'on est en train de parcourir
        while (actualLine[ind] != ',' && actualLine[ind] != '\n' && actualLine[ind] != '\0') {
            parameters[col][j] = actualLine[ind];
            ind++;
            j++;
        }
        if (actualLine[ind] != '\n' && actualLine[ind] != '\0') {
            ind += 2;
        }
        col++;
    }
    // "col" contient le nombre d'arguments
    j = 0; // Le numero d'argument qu'on est en train de parcourir (j<col)

  while (j < col) {
        int y; // Indice auxiliaire pour comparaison des lettres
        long int parNb = 0; // Variable auxiliaire pour recuperer la valeur decimale du numero du parametre ou de l'adrese ou pointe l'etiquette
        long int nbAux; // Variable auxiliaire qui aide a calculer la valeur immediate
        if (parameters[j][0] == '#'){
            if (j!=2) {
                printf("Error : valeur immediate doit etre passe en 2e source \n");
                ErrorLine = 0;
            }
            if (parameters[j][1] == 'h'){
                y = 2;
                char hexStr[5]="";
                while ((parameters[j][y] != '\0')) {
                    hexStr[y-2] = parameters[j][y];
                    y++;
                }
                hexStr[y] = '\0';
                parNb = HexToDec(hexStr);
                printf("%ld\n", parNb);
            }
            else {
                y = 1;
                while ((parameters[j][y] != '\0')) {
                    nbAux = (parameters[j][y] - 48) * pow(10, strlen(parameters[j])- 1 - y);    // convertir le caractere ascii en entier (0 car = 48 en decimal)
                    parNb += nbAux;
                    y++;
                }
            }
            printf(" immediate value translated : %ld\n", parNb);
            translatedLineBin[3] = 1;
            translatedLineBin[4] = parNb;
            
        }
        
        else if (parameters[j][0]=='r') { // Si l'argument est le registre, on traduit son numero en binaire
            // !!! Gerer le cas ou l'etiquette est sous forme "r..." !!!
            y = 1;
            while ((parameters[j][y]!='\0')) {
                nbAux = (parameters[j][y]-48)*pow(10, strlen(parameters[j])-1-y);    // convertir le caractere ascii en entier (0 car = 48 en decimal)
                parNb += nbAux;
                y++;
            }
            if (j!=2) {
                translatedLineBin[j+1] = parNb;
            } else {
                translatedLineBin[3] = 0;
                translatedLineBin[4] = parNb;
            }
            if (parNb < 0 || parNb > 31) {
                printf("Error : le registre %s n'existe pas\n", parameters[j]);
                ErrorLine = 0;
            } 
            
        } else { // Si l'argument est une etiquette, on cherche l'adresse ou elle pointe
            i = 0; // Indice auxiliaire qui va indiquer sur l'indice de l'etiquette dans le tableau des etiquettes
            int equal = 1; // Variable qui =0 si on a trouve l'indice de l'etiquette dans le tableau des etiquettes
            while (i < NbLines && equal) { // On cherche l'indice de l'etiquette dans le tableau des etiquettes
                y = 0;
                while (allTags[i][y]!='\0' && parameters[j][y]!='\0' && allTags[i][y]==parameters[j][y]) {
                    y++;
                }
                if (y == strlen(parameters[j]) && y == strlen(allTags[i])) {
                    equal = 0;
                    parNb = i*4; // On a trouve l'indice de l'etiquette et comme chaque instruction fait 32 bits, soit 4 octets, on multiplie l'indice par 4
                }
                i++;
            }
            if (equal) {
                printf("Error : tag %s n'existe pas\n", parameters[j]);
                ErrorLine = 0;     
            } else {
                translatedLineBin[3] = 1;
                translatedLineBin[4] = parNb;
            }
        }
        j++;
    }
    int nbAux2 = (translatedLineBin[0]<<27) + (translatedLineBin[1]<<22) + (translatedLineBin[2]<<17) + (translatedLineBin[3]<<16) + translatedLineBin[4];
    printf("Traduction : %x\n", nbAux2);

    int indxx = 0;
    while (indxx < 5) {
        printf("%ld\n", translatedLineBin[indxx]);
        indxx++;
    }
    return ErrorLine;   //tout s'est bien passé sans erreur 
}

int main(int argc, char* argv[]) {
    char allTags[NbLines][NbColons]; // La collection de tous les etiquettes contenues dans le fichier
    char line[NbColons]; // Ligne actuelle
    char* TabLines[NbLines]; // Tous les lignes du fichier
    int ErrorPres = 1;    // variable qui vaut 1 si pas d'erreur et 0 si erreur
    
    FILE* fichier_orig = fopen(argv[1],"r");
    if (fichier_orig == NULL) {
        return 1;
    }
    
    int nbLines = 0; // Indice qui aide a remplir le tableau "TabLines" et des que le fichier est parcouru va indiquer le nombre de ligne dans le fichier source
    int tagsPresence; // Variable qui indique la presence d'une nouvelle etiquette sur une ligne ("ici : ..")
    while (fgets(line, NbColons,fichier_orig)!=NULL) { // On parcourt le fichier pour trouver tous les etiquettes
        TabLines[nbLines] = strdup(line);
        tagsPresence = tagCollect(nbLines, TabLines[nbLines], allTags);
        if (tagsPresence == -1){
            printf("erreur : format de l'etiquette invalide a la ligne %d \n", nbLines + 1);
            ErrorPres = 0;
        }
        nbLines++;
    }
    // Verification que tous les etiquettes sont bien stockes
    printf("Tags : ");
    int ind = 0;
    while (ind < nbLines) {
        printf("%s\n", allTags[ind]);
        ind++;
    }
    if (tagDoubl(allTags, nbLines)){
        printf("doublon etiquette à la ligne %d", tagDoubl(allTags, nbLines));
        ErrorPres = 0;
    }

    fclose(fichier_orig);
    
    // On initialise on fichier qui va conetnir la traduction de langage machine
    FILE* fichierTranlated = NULL;
    fichierTranlated = fopen("newVersion.txt", "w");
    if (fichierTranlated == NULL) {
        return 1;
    }
    
    char translatedLine[NbColons]; // Nouvelle ligne qui est traduit
    int newI = 0; // Indice avec la quelle on va parcourir le tableau "TabLines", qui va indiquer la ligne ou on se trouve
    while (newI < nbLines) {
        if (TabLines[newI][0] != '\n'){
            tagsPresence = tagCollect(newI, TabLines[newI], allTags); // On verifie si la ligne contient une initialisation d'une etiquette
            traduction(newI, TabLines[newI], allTags, tagsPresence, translatedLine);
            //fprintf(fichierTranlated, "%s\n", TabLines[nbLines]);
            newI++;
        }
        else {
            printf("ligne %d vide \n", newI +1);
            ErrorPres = 0;
            newI++;
        }
       
    }
    fclose(fichierTranlated);
    
    return 0;
}










