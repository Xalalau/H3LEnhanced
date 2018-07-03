// ##############################
// HU3-LIFE VARIAVEIS DO COOP
// ##############################

#ifndef GAME_SERVER_GAMERULES_CHU3LIFECOOP_CPP
#define GAME_SERVER_GAMERULES_CHU3LIFECOOP_CPP

#include "mathlib.h"
#include "CHu3LifeCoop.h"

// Guardar as infos dos jogadores
struct playerCoopSaveRestore CoopPlyData[64];

// Nome do landmark em uso
char hu3LandmarkName[32] = "";

// Index de um jogador
int hu3CoopPlyIndex = 1;
// Novo nome de um jogador
char hu3NetNewName[32] = "";
// Se um nome precisa ser alterado ou nao
bool hu3ChangeNetName = false;

// Liga o processamento de trigger_changelevel no think do modo coop
bool hu3ChangelevelCheck = false;

// Pequeno atraso no inicio de func_tracktrains (segundos)
int hu3TracktrainDelay = 13;

#endif //GAME_SERVER_GAMERULES_CHU3LIFECOOP_CPP