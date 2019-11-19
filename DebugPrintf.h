/*--------------------------------------------------------

	DebugPrintf.hを作って使おう
	作成日31年4月24日
	製作者五十嵐凌

--------------------------------------------------------*/

//========================================================

#ifndef _DEBUG_PRINTF_H_
#define _DEBUG_PRINTF_H_

#include <windows.h>
#include <stdio.h>


//デバッグ用のprintf...VSの出力ウィンドウに出力される
inline void DebugPrintf(const char* pFormat, ...) 
{
//こんな風に書くとReleaseビルド時はビルドされない範囲を指定できる
#if defined(_DEBUG) || defined(DEBUG)
	va_list argp;
	char buf[256];
	va_start(argp, pFormat);
	vsprintf_s(buf, 256, pFormat, argp);
	va_end(argp);
	OutputDebugStringA(buf);
#endif //_DEBUG || DEBUG
}

#endif // _DEBUG_PRINTF_H_
