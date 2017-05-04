#ifndef GAME_SERVER_GAMERULES_CHU3LIFECOOP_CPP
#define GAME_SERVER_GAMERULES_CHU3LIFECOOP_CPP

#include "mathlib.h"
#include "CHu3LifeCoop.h"

// MOVER PARA DENTRO DE ALGUMA CLASSE!! NAO COMPILA NO LINUX!! PROBLEMA DE CONSTRUCTOR E INICIALIZACAO!!

// Guardar a info dos jogadores
struct playerCoopSaveRestore CoopPlyData[64];
// Guardar o nome do landmark que esta sendo utilizado
char Hu3LandmarkName[32] = "";
// Nome a ser atribuido ao jogador (caso necessario)
char hu3NetName[32] = "";
// Uso essa variavel para reduzir as checagens
bool hu3ChangeNetName = false;

#endif //GAME_SERVER_GAMERULES_CHU3LIFECOOP_CPP
