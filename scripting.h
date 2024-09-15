#ifndef SCRIPTING_H
#define SCRIPTING_H

extern void scripting_Init();
extern void scripting_Quit();

extern int scripting_GetFloatSetting(char *name, float *f);
extern int scripting_GetIntegerSetting(char *name, int *i);
extern int scripting_GetIntegerResult(int *i);
extern void scripting_GetFloatArray(char *name, float *f, int n);
extern int scripting_GetStringSetting(char *name, char **s);
extern int scripting_GetStringResult(char **s);
extern int scripting_CopyStringResult(char *s, int len);

extern void scripting_SetFloatSetting(char *name, float f);
extern void scripting_RunFile(char *name);
extern void scripting_Run(char *command);
extern void scripting_RunFormat(char *format, ...);
extern void scripting_RunGC();

#endif
