#ifndef GAME_SERVER_GAMERULES_CHU3FLIFECOOP_H
#define GAME_SERVER_GAMERULES_CHU3FLIFECOOP_H

// ############ hu3lifezado ############ //
// [MODO COOP]
//Guardar as informacoes dos players
struct playerCoopWeapons {
	char name[30];
	int currentammo;
	char type1[20];
	int amountammo1;
	char type2[20];
	int amountammo2;
};
struct playerCoopSaveRestore {
	Vector relPos;
	Vector v_angle;
	Vector velocity;
	Vector angles;
	Vector punchangle;
	bool used; // Isso eh importante para lidar com jogadores que estejam com nome repetido
	int weapons; // HUD
	int team;
	float armorvalue;
	bool bInDuck;
	int deadflag;
	int fixangle;
	float health;
	float frags;
	float flFallVelocity;
	char *pName;
	struct playerCoopWeapons keepweapons[64];
};
extern struct playerCoopSaveRestore CoopPlyData[64];
// Nome do landmark em uso
extern char Hu3LandmarkName[32];
// Nome a ser atribuido ao jogador (caso necessario)
extern char hu3NetName[32];
// Index do jogador em CoopPlyData
extern bool hu3ChangeNetName;
// ############ //

#endif //GAME_SERVER_GAMERULES_CHU3FLIFECOOP_H
