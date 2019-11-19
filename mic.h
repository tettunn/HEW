#pragma once
#ifndef MIC_H_
#define MIC_H_

void Mic_Init();
void Mic_UnInit();
void Mic_Update();

short Mic_GetData(void);

#endif // !MIC_H_
