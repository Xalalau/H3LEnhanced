#ifndef GAME_SERVER_GAMERULES_CHU3LIFECOOP_CPP
#define GAME_SERVER_GAMERULES_CHU3LIFECOOP_CPP

#include "mathlib.h"
#include "CHu3LifeCoop.h"

// Guardar as infos dos jogadores
struct playerCoopSaveRestore CoopPlyData[64];

// Nome do landmark em uso
char Hu3LandmarkName[32] = "";

// Index de um jogador
int hu3CoopPlyIndex = 1;

// Novo nome de um jogador
char hu3NetNewName[32] = "";

// Se um nome precisa ser alterado ou nao
bool hu3ChangeNetName = false;

#endif //GAME_SERVER_GAMERULES_CHU3LIFECOOP_CPP