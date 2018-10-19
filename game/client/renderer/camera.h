//========= Copyright © 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

// Camera.h  --  defines and such for a 3rd person camera
// NOTE: must include quakedef.h first

#ifndef _CAMERA_H_
#define _CAMERA_H_

// pitch, yaw, dist
extern Vector cam_ofs;
// Using third person camera
extern int cam_thirdperson;

void CAM_Init( void );
void CAM_ClearStates( void );
void CAM_StartMouseMove(void);
void CAM_EndMouseMove(void);

// ############ hu3lifezado ############ //
// Variavel usada para remover o sprite do crosshair
static wrect_t nullrc;
// Variavel que controla os modos de camera. 0 = primeira pessoa; 1, 2 e 3 = terceiras pessoas
extern int hu3_cam_valor;
// Variavel para dizer se a camera deve seguir o jogador por tras nos modos 2 e 3 da terceira pessoa
extern bool hu3_cam_seguir_ply;
// ############ //

#endif		// _CAMERA_H_
