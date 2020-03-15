Utilisation :

make clean : nettoie, supprime les dossiers build
make atom  : lance les src et les headers avec l’éditeur de texte atom
make lib   : compile la libraire
make test  : lance make lib et compile le test
make run   : lance le test
make all   : appelle clean, lib, test et run
make       : appelle all


Pour ajouter / enlever l'affichage : commentez ou de-commentez les
fonctions destructeur :

void stats() __attribute__((destructor));

void status_print() __attribute__((destructor));

dans include/my_malloc.h
