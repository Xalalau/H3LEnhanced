#ifndef GAME_SERVER_GAMERULES_CHU3FLIFECOOP_H
#define GAME_SERVER_GAMERULES_CHU3FLIFECOOP_H

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
	bool used; // Isso eh importante para lidar com jogadores que estejam com nome repetido e evitar carregamentos ruins de itens
	bool changinglevel; // Serve para o hack de acerto das municoes
	int weapons; // HUD
	int team;
	float armorvalue;
	bool bInDuck;
	bool flashlight;
	int deadflag;
	int fixangle;
	float health;
	float frags;
	float flFallVelocity;
	char *pName;
	struct playerCoopWeapons keepweapons[64];
};

// Guardar as infos dos jogadores
extern struct playerCoopSaveRestore CoopPlyData[64];

// Nome do landmark em uso
extern char Hu3LandmarkName[32];

// Index de um jogador
extern int hu3CoopPlyIndex;

// Novo nome de um jogador
extern char hu3NetNewName[32];

// Se um nome precisa ser alterado ou nao
extern bool hu3ChangeNetName;

#endif //GAME_SERVER_GAMERULES_CHU3FLIFECOOP_H
