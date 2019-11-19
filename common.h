#pragma once
#ifndef COMMON_H_
#define COMMON_H_

/*----------------------------
	定数
----------------------------*/
#define SCREEN_WIDTH	(1280)
#define SCREEN_HEIGHT	(720)

/*----------------------------
	マクロ定義
----------------------------*/
//セーフリリースマクロ
#define SAFE_RELEASE(p) if(p!=NULL){ p->Release(); p=NULL;}



#endif