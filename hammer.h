#ifndef _HAMMER_H_
#define _HAMMER_H_
#include<d3dx9.h>

void Hammer_Init(void);
void Hammer_Uninit(void);
void Hammer_Update(void);
void Hammer_Draw(void);

D3DXVECTOR3 HamPosition_Get(void);

#endif // !_HAMMER_H_