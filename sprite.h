#pragma once
#ifndef SPRITE_H
#define SPRITE_H

enum ZERO_POSITION {
	CENTER,
	BOTTOM,
};

//表示
//(座標.x,座標.y,ID)
void Sprite_Draw(int textureId,float dx, float dy);

//回転
//(座標.x,座標.y,ID,回転の中心.x,回転の中心.y,回転の中心角)
void Sprite_Draw(int textureId,float dx, float dy, float center_x, float center_y, float angle);

//拡大
//(座標.x,座標.y,ID,拡大する倍率.x,拡大する倍率.y,中心.x,中心.y)
void Sprite_Draw(int textureId,float dx, float dy, float scale_x, float scale_y,float center_x, float center_y);

//拡大・回転
//(座標.x,座標.y,ID,拡大する倍率.x,拡大する倍率.y,中心.x,中心.y,回転の中心角)
void Sprite_Draw(int textureId,float dx, float dy, float scale_x, float scale_y, float center_x, float center_y, float angle);

//拡大・回転・反転
//(座標.x,座標.y,ID,拡大する倍率.x,拡大する倍率.y,中心.x,中心.y,回転の中心角)
void Sprite_Mirror_Draw(int textureId,float dx, float dy, float scale_x, float scale_y, float center_x, float center_y, float angle);


//カット
//(座標.x,座標.y,ID,カットするテクスチャ座標.x,カットするテクスチャ座標.y,カットするテクスチャの幅,カットするテクスチャの高さ)
void Sprite_Draw(int textureId,float dx, float dy,int cut_x,int cut_y,int cut_w,int cut_h);

//カット・回転
//(座標.x,座標.y,ID,カットするテクスチャ座標.x,カットするテクスチャ座標.y,カットするテクスチャの幅,カットするテクスチャの高さ,回転の中心.x,回転の中心.y,回転の中心角)
void Sprite_Draw(int textureId,float dx, float dy, int cut_x, int cut_y, int cut_w, int cut_h,float center_x,float center_y, float angle);

//カット・拡大
//(座標.x,座標.y,ID,カットするテクスチャ座標.x,カットするテクスチャ座標.y,カットするテクスチャの幅,カットするテクスチャの高さ,拡大する倍率.x,拡大する倍率.y)
void Sprite_Draw(int textureId,float dx, float dy, int cut_x, int cut_y, int cut_w, int cut_h, float scale_x, float scale_y, float center_x, float center_y);

//カット・拡大・回転
//(座標.x,座標.y,ID,カットするテクスチャ座標.x,カットするテクスチャ座標.y,カットするテクスチャの幅,カットするテクスチャの高さ,拡大する倍率.x,拡大する倍率.y,中心.x,中心.y,回転の中心角)
void Sprite_Draw(int textureId,float dx, float dy, int cut_x, int cut_y, int cut_w, int cut_h, float scale_x, float scale_y, float center_x, float center_y,float angle);

//カット・拡大・回転・反転
//(座標.x,座標.y,ID,カットするテクスチャ座標.x,カットするテクスチャ座標.y,カットするテクスチャの幅,カットするテクスチャの高さ,拡大する倍率.x,拡大する倍率.y,中心.x,中心.y,回転の中心角)
void Sprite_Mirror_Draw(int textureId,float dx, float dy, int cut_x, int cut_y, int cut_w, int cut_h, float scale_x, float scale_y, float center_x, float center_y, float angle);

//色セット
void Sprite_SetColor(D3DCOLOR Color);

#endif // !SPRITE_H