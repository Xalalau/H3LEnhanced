// ---------------------------------------------------------------
// Flying Crowbar Entity. Ver 1.0 as seen in Lambda BubbleMod  
// 
// AUTHOR
//        Tyler Lund <halflife@briareos.net>
//
// LICENSE                                                            
//                                                                    
//        Permission is granted to anyone to use this  software  for  
//        any purpose on any computer system, and to redistribute it  
//        in any way, subject to the following restrictions:          
//                                                                    
//        1. The author is not responsible for the  consequences  of  
//           use of this software, no matter how awful, even if they  
//           arise from defects in it.                                
//        2. The origin of this software must not be misrepresented,  
//           either by explicit claim or by omission.                 
//        3. Altered  versions  must  be plainly marked as such, and  
//           must  not  be  misrepresented  (by  explicit  claim  or  
//           omission) as being the original software.                
//        3a. It would be nice if I got  a  copy  of  your  improved  
//            version  sent to tlund@briareos.net. 
//        4. This notice must not be removed or altered.              
//                                                                    
// ---------------------------------------------------------------

// ############ hu3lifezado ############ //
// Tiro secundario da arma banana, adaptado de:
// http://web.archive.org/web/20020717063241/http://lambda.bubblemod.org/tuts/crowbar/
// ############ //

#ifndef C_FLYING_CROWBAR
#define C_FLYING_CROWBAR

class CFlyingCrowbar : public CBaseEntity
{
public:
	DECLARE_CLASS(CFlyingCrowbar, CBaseEntity);
	DECLARE_DATADESC();

	void Spawn(void);
	void Precache(void);
	void BubbleThink(void);
	void SpinTouch(CBaseEntity *pOther);
	CBasePlayer *m_pPlayer;

private:

	EHANDLE m_hOwner;        // Original owner is stored here so we can
							 // allow the crowbar to hit the user.
};

#endif