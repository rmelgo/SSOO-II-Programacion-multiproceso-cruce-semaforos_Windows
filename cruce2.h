#ifdef DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

#define PERROR(a) \
    {             \
        LPVOID lpMsgBuf;                                         \
        FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |           \
                   FORMAT_MESSAGE_FROM_SYSTEM |                  \
                   FORMAT_MESSAGE_IGNORE_INSERTS, NULL,          \
                   GetLastError(),                               \
                   MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),    \
                   (LPTSTR) &lpMsgBuf,0,NULL );                  \
        fprintf(stderr,"%s:(%d)%s\n",a,GetLastError(),lpMsgBuf); \
        LocalFree( lpMsgBuf );                                   \
    }

#define NEGRO    0
#define ROJO     1
#define VERDE    2
#define AMARILLO 3
#define AZUL     4
#define MAGENTA  5
#define CYAN     6
#define BLANCO   7

#define SEM_C1 0
#define SEM_C2 1
#define SEM_P1 2
#define SEM_P2 3

#define COCHE 0
#define PEAToN 1

struct posiciOn {int x,y;};

#ifdef DLL_EXPORTS
extern "C" DLL_API int CRUCE_inicio(int ret, int maxProcs);
extern "C" DLL_API int CRUCE_fin(void);
extern "C" DLL_API int CRUCE_gestor_inicio(void);
extern "C" DLL_API int CRUCE_pon_semAforo(int sem, int color);
extern "C" DLL_API int CRUCE_nuevo_proceso(void);
extern "C" DLL_API struct posiciOn CRUCE_inicio_coche(void);
extern "C" DLL_API struct posiciOn CRUCE_avanzar_coche(struct posiciOn sgte);
extern "C" DLL_API int CRUCE_fin_coche(void);
extern "C" DLL_API struct posiciOn CRUCE_nuevo_inicio_peatOn(void);
extern "C" DLL_API struct posiciOn CRUCE_avanzar_peatOn(struct posiciOn sgte);
extern "C" DLL_API int CRUCE_fin_peatOn(void);
extern "C" DLL_API int pausa(void);
extern "C" DLL_API int pausa_coche(void);
extern "C" DLL_API int refrescar(void);
extern "C" DLL_API void pon_error(const char *mensaje);
#endif