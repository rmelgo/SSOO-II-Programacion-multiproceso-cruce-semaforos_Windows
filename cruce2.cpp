#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "cruce2.h"

DWORD WINAPI ciclo_semaforico(LPVOID parametro);
DWORD WINAPI coche(LPVOID parametro);
DWORD WINAPI peaton(LPVOID parametro);
void n_pausas(int n, HINSTANCE manejador_dll);
void poner_semaforo(int tipo_semaforo, int valor, HINSTANCE manejador_dll);

struct tipo_informacion_matriz {
	int clave;
	int lista_avisos[3];
	int num_remplazos;
};

struct datos {
	HINSTANCE manejador_dll;
	HANDLE manejador_semaforo_C1;
	HANDLE manejador_semaforo_C2;
	HANDLE manejador_semaforo_P1;
	HANDLE manejador_semaforo_P2;
	HANDLE manejador_semaforo_avance;
	HANDLE manejador_cruce1;
	HANDLE manejador_cruce2;
	HANDLE manejador_cruce4;
	CRITICAL_SECTION sc1; 
	CRITICAL_SECTION control_procesos_activos;
	CRITICAL_SECTION control_estado_cruce1; 
	CRITICAL_SECTION control_estado_cruce2;
	CRITICAL_SECTION control_estado_cruce4;
	LONG estado_cruce1;
	LONG estado_cruce2;
	LONG estado_cruce4;
	struct tipo_informacion_matriz matriz_posiciones[75][75];
	DWORD hilos[50];
	LONG procesos_activos; 
};

int main(int argc, char** argv) {
    int (*CRUCE_inicio)(int, int);
    int (*CRUCE_nuevo_proceso)(void);
    struct datos parametro;
	DWORD error;
	int n_max_procesos = -1;
	int uso_cpu = -1;
    int parametro_hilo_ciclo_semaforico;
    int hijo;
    DWORD hilo_coche, hilo_peaton;
	
	if (argc != 3) { 
        printf("Error. Numero de parametros incorrecto.\n");
        exit(1); 
    }    
    else {
        if (atoi (argv[2]) < 0) { 
            printf("Error. El segundo parametro debe ser un numero entero mayor o igual a cero.\n");
            exit(2);
        }
        else {
            n_max_procesos = atoi(argv[1]);
            uso_cpu = atoi(argv[2]);           
        }
    } 
	
	HINSTANCE manejador_dll = LoadLibrary("cruce2.dll"); 
	
	if (manejador_dll == NULL) {
        exit(3);
    }
	
	CRUCE_inicio = (int (*)(int, int)) GetProcAddress(manejador_dll, "CRUCE_inicio"); 
	if (CRUCE_inicio == NULL) {
        exit(4);
    }
       
    HANDLE cruce1 = CreateSemaphore(NULL, 1, 1, "Cruce1");
    if (cruce1 == NULL) {
    	exit(5);
	}
    
    HANDLE manejador_cruce1 = OpenSemaphore(SEMAPHORE_ALL_ACCESS, 0, "Cruce1");
    if (manejador_cruce1 == NULL) {
    	exit(6);
	}
    
    HANDLE cruce2 = CreateSemaphore(NULL, 1, 1, "Cruce2");
    if (cruce2 == NULL) {
    	exit(5);
	}
    
    HANDLE manejador_cruce2 = OpenSemaphore(SEMAPHORE_ALL_ACCESS, 0, "Cruce2");
    if (manejador_cruce1 == NULL) {
    	exit(6);
	}
    
    HANDLE cruce4 = CreateSemaphore(NULL, 1, 1, "Cruce4");
    if (cruce4 == NULL) {
    	exit(5);
	}
    
    HANDLE manejador_cruce4 = OpenSemaphore(SEMAPHORE_ALL_ACCESS, 0, "Cruce4");
    if (manejador_cruce4 == NULL) {
    	exit(6);
	}
    
    HANDLE semaforo_C1 = CreateSemaphore(NULL, 0, 1, "Semaforo_C1");
    if (semaforo_C1 == NULL) {
    	exit(5);
	}
    
    HANDLE manejador_semaforo_C1 = OpenSemaphore(SEMAPHORE_ALL_ACCESS, 0, "Semaforo_C1");
    if (manejador_semaforo_C1 == NULL) {
    	exit(6);
	}
    
    HANDLE semaforo_C2 = CreateSemaphore(NULL, 0, 1, "Semaforo_C2");
    if (semaforo_C2 == NULL) {
    	exit(5);
	}
    
    HANDLE manejador_semaforo_C2 = OpenSemaphore(SEMAPHORE_ALL_ACCESS, 0, "Semaforo_C2");
    if (manejador_semaforo_C2 == NULL) {
    	exit(6);
	}
    
    HANDLE semaforo_P1 = CreateSemaphore(NULL, 0, 1, "Semaforo_P1");
    if (semaforo_P1 == NULL) {
    	exit(5);
	}
    
    HANDLE manejador_semaforo_P1 = OpenSemaphore(SEMAPHORE_ALL_ACCESS, 0, "Semaforo_P1");
    if (manejador_semaforo_P1 == NULL) {
    	exit(6);
	}
    
    HANDLE semaforo_P2 = CreateSemaphore(NULL, 0, 1, "Semaforo_P2");
    if (semaforo_P2 == NULL) {
    	exit(5);
	}
    
    HANDLE manejador_semaforo_P2 = OpenSemaphore(SEMAPHORE_ALL_ACCESS, 0, "Semaforo_P2");
    if (manejador_semaforo_P2 == NULL) {
    	exit(6);
	}
    
    HANDLE semaforo_avance = CreateSemaphore(NULL, 1, 1, "Semaforo_avance");
    if (semaforo_avance == NULL) {
    	exit(5);
	}
    
    HANDLE manejador_semaforo_avance = OpenSemaphore(SEMAPHORE_ALL_ACCESS, 0, "Semaforo_avance");
    if (manejador_semaforo_avance == NULL) {
    	exit(6);
	}
    
    InitializeCriticalSection(&parametro.sc1); 
    InitializeCriticalSection(&parametro.control_estado_cruce1); 
    InitializeCriticalSection(&parametro.control_estado_cruce2); 
    InitializeCriticalSection(&parametro.control_estado_cruce4); 
    InitializeCriticalSection(&parametro.control_procesos_activos); 
    
    if (CRUCE_inicio(uso_cpu, n_max_procesos) == -1) {
    	exit(7);
	}
    
    
    parametro.manejador_dll = manejador_dll;
    parametro.manejador_semaforo_C1 = manejador_semaforo_C1;
    parametro.manejador_semaforo_C2 = manejador_semaforo_C2;
    parametro.manejador_semaforo_P1 = manejador_semaforo_P1;
    parametro.manejador_semaforo_P2 = manejador_semaforo_P2;
    parametro.manejador_semaforo_avance = manejador_semaforo_avance;
    parametro.manejador_cruce1 = manejador_cruce1;
    parametro.manejador_cruce2 = manejador_cruce2;
    parametro.manejador_cruce4 = manejador_cruce4;
    parametro.procesos_activos = 1;
    
    parametro.procesos_activos = InterlockedIncrement(&parametro.procesos_activos);
    
    HANDLE hilo = CreateThread(NULL, 0, ciclo_semaforico, &parametro, 0, NULL);
    if (hilo == NULL) { 
        exit(8);
    }        
    
    HANDLE manejador_hilos[n_max_procesos-2];
    for (int i = 0; i < 75; i++) {
    	for (int j = 0; j < 75; j++) {
    		parametro.matriz_posiciones[i][j].clave = 0;
    		for (int k = 0; k < 3; k++) {
    			parametro.matriz_posiciones[i][j].lista_avisos[k] = 0;
			}
			parametro.matriz_posiciones[i][j].num_remplazos = 0;
		}
	}
	
	parametro.estado_cruce1 = 0;
	parametro.estado_cruce2 = 0;
	parametro.estado_cruce4 = 0;
     
    for(;;) {
    	if (n_max_procesos == parametro.procesos_activos) { 
    		if (WaitForMultipleObjects(n_max_procesos-2, manejador_hilos, 0, INFINITE) == WAIT_FAILED) { 
    		    exit(10);
			}			
		}
		else {
		    CRUCE_nuevo_proceso = (int (*)(void)) GetProcAddress(manejador_dll, "CRUCE_nuevo_proceso"); 
	        if (CRUCE_nuevo_proceso == NULL) {
                exit(4);
            }
            hijo = CRUCE_nuevo_proceso();
            if (hijo == -1) {
            	exit(7);
			}
			EnterCriticalSection(&parametro.control_procesos_activos); 
            parametro.procesos_activos = InterlockedIncrement(&parametro.procesos_activos);
	        LeaveCriticalSection(&parametro.control_procesos_activos);		
			if (hijo == COCHE) {
				EnterCriticalSection(&parametro.control_procesos_activos); 
				manejador_hilos[parametro.procesos_activos - 3] = CreateThread(NULL, 0, coche, &parametro, 0, &parametro.hilos[parametro.procesos_activos]); 
				LeaveCriticalSection(&parametro.control_procesos_activos);
                if (coche == NULL) { 
                   exit(8);               
                }                    
			}
			else if (hijo == PEAToN) {
				EnterCriticalSection(&parametro.control_procesos_activos); 
				manejador_hilos[parametro.procesos_activos - 3] = CreateThread(NULL, 0, peaton, &parametro, 0, &parametro.hilos[parametro.procesos_activos]); 
				LeaveCriticalSection(&parametro.control_procesos_activos);
                if (peaton == NULL) { 
                   exit(8);
                }                
			}			
		}		
	}
	
	DeleteCriticalSection(&parametro.sc1);
	DeleteCriticalSection(&parametro.control_estado_cruce1);
	DeleteCriticalSection(&parametro.control_estado_cruce2);
	DeleteCriticalSection(&parametro.control_estado_cruce4);
	DeleteCriticalSection(&parametro.control_procesos_activos);
    	
	printf("\n");
	return 0;
}

DWORD WINAPI ciclo_semaforico(LPVOID parametro) { //el manejador_dll ya es un puntero de por si que se le tiene que pasar a GetProcAddress	
    struct datos *p;
    p = (struct datos *) parametro;

    int (*CRUCE_gestor_inicio)(void);
	CRUCE_gestor_inicio = (int (*)(void)) GetProcAddress((HINSTANCE) p->manejador_dll, "CRUCE_gestor_inicio"); 
	if (CRUCE_gestor_inicio == NULL) {
        exit(4);
    }
 
    if (CRUCE_gestor_inicio() == -1) {
    	exit(7);
	}
  
   	for (;;) {
	    //Fase 1
	    poner_semaforo(SEM_C1, VERDE, p->manejador_dll);
	    poner_semaforo(SEM_P2, VERDE, p->manejador_dll);
	    poner_semaforo(SEM_C2, ROJO, p->manejador_dll);
	    poner_semaforo(SEM_P1, ROJO, p->manejador_dll);
	    if (ReleaseSemaphore(p->manejador_semaforo_C1, 1, NULL) == FALSE) {
	    	exit(9);
		}
	    if (ReleaseSemaphore(p->manejador_semaforo_P2, 1, NULL) == FALSE) {
	    	exit(9);
		}
	    n_pausas(6, (HINSTANCE) p->manejador_dll);
	    if (WaitForSingleObject(p->manejador_semaforo_C1, INFINITE) == WAIT_FAILED) {
	    	exit(10);
		}
	    if (WaitForSingleObject(p->manejador_semaforo_P2, INFINITE) == WAIT_FAILED) {
	    	exit(10);
		}
        poner_semaforo(SEM_C1, AMARILLO, p->manejador_dll); 
	    poner_semaforo(SEM_P2, ROJO, p->manejador_dll);
	    n_pausas(2, (HINSTANCE) p->manejador_dll);
	    if (WaitForSingleObject(p->manejador_cruce4, INFINITE) == WAIT_FAILED) {
	    	exit(10);
		}
	    if (ReleaseSemaphore(p->manejador_cruce4, 1, NULL) == FALSE) {
	    	exit(9);
		}
	    WaitForSingleObject(p->manejador_cruce2, INFINITE);
	    if (ReleaseSemaphore(p->manejador_cruce2, 1, NULL) == FALSE) {
	    	exit(9);
		}
		poner_semaforo(SEM_C1, ROJO, p->manejador_dll);	
	    
	    //Fase 2
	    poner_semaforo(SEM_C2, VERDE, p->manejador_dll);  //C2 -> Verde
	    if (ReleaseSemaphore(p->manejador_semaforo_C2, 1, NULL) == FALSE) {
	    	exit(9);
		}
	    n_pausas(9, (HINSTANCE) p->manejador_dll);
	    if (WaitForSingleObject(p->manejador_semaforo_C2, INFINITE) == WAIT_FAILED) {
	    	exit(10);
		}
	    poner_semaforo(SEM_C2, AMARILLO, p->manejador_dll);	//C2 -> Amarillo
	    n_pausas(2, (HINSTANCE) p->manejador_dll);
	    if (WaitForSingleObject(p->manejador_cruce4, INFINITE) == WAIT_FAILED) {
	    	exit(10);
		}
	    if (ReleaseSemaphore(p->manejador_cruce4, 1, NULL) == FALSE) {
	    	exit(9);
		}
	    poner_semaforo(SEM_C2, ROJO, p->manejador_dll);	//C2 -> Rojo
	    
	    //Fase 3
	    poner_semaforo(SEM_P1, VERDE, p->manejador_dll);	//P1 -> Verde
	    if (ReleaseSemaphore(p->manejador_semaforo_P1, 1, NULL) == FALSE) {
	    	exit(9);
		}
	    n_pausas(12, (HINSTANCE) p->manejador_dll);
	    if (WaitForSingleObject(p->manejador_semaforo_P1, INFINITE) == WAIT_FAILED) {
	    	exit(10);
		}
	    poner_semaforo(SEM_P1, ROJO, p->manejador_dll);	//P1 -> Rojo
	    poner_semaforo(SEM_C1, AMARILLO, p->manejador_dll);	//C1 -> Amarillo
	    n_pausas(2, (HINSTANCE) p->manejador_dll);
	    if (WaitForSingleObject(p->manejador_cruce1, INFINITE) == WAIT_FAILED) {
	    	exit(10);
		}
	    if (ReleaseSemaphore(p->manejador_cruce1, 1, NULL) == FALSE) {
	    	exit(9);
		}
	}
	return 0;
}

DWORD WINAPI coche(LPVOID parametro) {	
    struct datos *p;
    p = (struct datos *) parametro;
    
	struct posiciOn pos, pos_actual1, pos_actual2, pos_actual3, pos_actual4, pos_actual5;
	pos_actual1.x = -1;
    pos_actual1.y = -1;
    pos_actual2.x = -1;
    pos_actual2.y = -1;
    pos_actual3.x = -1;
    pos_actual3.y = -1;
    pos_actual4.x = -1;
    pos_actual4.y = -1;
    pos_actual5.x = -1;
    pos_actual5.y = -1;
    int esperar_posicion_siguiente;
    int posicion_conflictiva = 0;
    int entre_en_el_cruce3 = 0, sali_del_cruce3 = 0;
    int entre_en_el_cruce4 = 0, sali_del_cruce4 = 0;
	struct posiciOn (*CRUCE_inicio_coche)(void);
	struct posiciOn (*CRUCE_avanzar_coche)(struct posiciOn);
	int (*pausa_coche)(void);
	int (*CRUCE_fin_coche)(void);
	CRUCE_inicio_coche = (struct posiciOn (*)(void)) GetProcAddress((HINSTANCE) p->manejador_dll, "CRUCE_inicio_coche"); 
	if (CRUCE_inicio_coche == NULL) {
    	exit(4);
    }
    
    CRUCE_avanzar_coche = (struct posiciOn (*)(struct posiciOn)) GetProcAddress((HINSTANCE) p->manejador_dll, "CRUCE_avanzar_coche"); 
	if (CRUCE_avanzar_coche == NULL) {
    	exit(4);
    }
    
    pausa_coche = (int (*)(void)) GetProcAddress((HINSTANCE) p->manejador_dll, "pausa_coche"); 
	if (pausa_coche == NULL) {
    	exit(4);
    }
    
    CRUCE_fin_coche = (int (*)(void)) GetProcAddress((HINSTANCE) p->manejador_dll, "CRUCE_fin_coche"); 
	if (CRUCE_fin_coche == NULL) {
    	exit(4);
    }
    
    pos = CRUCE_inicio_coche();
    MSG mensaje;
    PeekMessage(&mensaje, NULL, WM_USER, WM_USER, PM_NOREMOVE);
	
	while (pos.y >= 0) {
		pausa_coche();
		if (pos.y == 6) { 
            if (WaitForSingleObject(p->manejador_semaforo_C1, INFINITE) == WAIT_FAILED) { 
                exit(10);
			}
            posicion_conflictiva = 1;
        }
        if (pos.x == 13) { 
            if (WaitForSingleObject(p->manejador_semaforo_C2, INFINITE) == WAIT_FAILED) { 
                exit(10);
			}
            posicion_conflictiva = 2;
        }
        
        if (pos.x >= 25 && pos.y >= 6 && pos.y <= 15 && entre_en_el_cruce3 == 0) { 
            EnterCriticalSection(&p->sc1); 
            entre_en_el_cruce3 = 1;
        }
        if (pos.y == 16 && sali_del_cruce3 == 0) {                              
            LeaveCriticalSection(&p->sc1);
            sali_del_cruce3 = 1;
        }
        
        if (pos.x >= 13 && pos.y >= 6 && entre_en_el_cruce4 == 0) {                              
            if (p->estado_cruce4 == 0) {
            	if (WaitForSingleObject(p->manejador_cruce4, INFINITE) == WAIT_FAILED) {
            		exit(10);
				}
			}
			EnterCriticalSection(&p->control_estado_cruce4); 
            p->estado_cruce4 = InterlockedIncrement(&p->estado_cruce4);
	        LeaveCriticalSection(&p->control_estado_cruce4);
			
            entre_en_el_cruce4 = 1;                                                           
        }
                                     
        if (esperar_posicion_siguiente == 1) {
        	if (GetMessage(&mensaje, NULL, 1500, 2500) == FALSE) {
        		exit(12);
			}
        	esperar_posicion_siguiente = 0;
		}
        
        if (WaitForSingleObject(p->manejador_semaforo_avance, INFINITE) == WAIT_FAILED) {
            exit(10);
		}
        
        if (p->matriz_posiciones[pos.x+3][pos.y].clave != 0) {
        	p->matriz_posiciones[pos.x+3][pos.y].clave = GetCurrentThreadId();
            esperar_posicion_siguiente = 1;	       	
		}
		else {
		    p->matriz_posiciones[pos.x+3][pos.y].clave = GetCurrentThreadId();
		    if (p->matriz_posiciones[pos_actual5.x+3][pos_actual5.y].clave != GetCurrentThreadId() && p->matriz_posiciones[pos_actual5.x+3][pos_actual5.y].clave != 0) {
		        if (!PostThreadMessage(p->matriz_posiciones[pos_actual5.x+3][pos_actual5.y].clave, 2000, 0, 0)) {
                    exit(11);
				}
			}
		    p->matriz_posiciones[pos_actual5.x+3][pos_actual5.y].clave = 0;
		
		    pos_actual5 = pos_actual4;
            pos_actual4 = pos_actual3;
            pos_actual3 = pos_actual2;               
            pos_actual2 = pos_actual1;                                                                                                                                                           
            pos_actual1 = pos;
		    pos = CRUCE_avanzar_coche(pos);
        }
        
        if (posicion_conflictiva == 1) { 
            if (ReleaseSemaphore(p->manejador_semaforo_C1, 1, NULL) == FALSE) {
            	exit(9);
			} 
			posicion_conflictiva = 0;      
        }
        if (posicion_conflictiva == 2) { 
            if (ReleaseSemaphore(p->manejador_semaforo_C2, 1, NULL) == FALSE) {
            	exit(9);
			}
            posicion_conflictiva = 0;
        }
                        
        if (pos.y >= 0) {
            if (ReleaseSemaphore(p->manejador_semaforo_avance, 1, NULL) == FALSE) {
            	exit(9);
			}
        }  	
	}
	
	p->matriz_posiciones[pos_actual1.x+3][pos_actual1.y].clave = 0;
	p->matriz_posiciones[pos_actual2.x+3][pos_actual2.y].clave = 0;
	p->matriz_posiciones[pos_actual3.x+3][pos_actual3.y].clave = 0;
	p->matriz_posiciones[pos_actual4.x+3][pos_actual4.y].clave = 0;
	if (p->matriz_posiciones[pos_actual5.x+3][pos_actual5.y].clave != GetCurrentThreadId()) {
		if (!PostThreadMessage(p->matriz_posiciones[pos_actual5.x+3][pos_actual5.y].clave, 2000, 0, 0)) {
			exit(11);
		}
	}
	p->matriz_posiciones[pos_actual5.x+3][pos_actual5.y].clave = 0;
	
	
	EnterCriticalSection(&p->control_estado_cruce4); 
    p->estado_cruce4 = InterlockedDecrement(&p->estado_cruce4);
	LeaveCriticalSection(&p->control_estado_cruce4);
    if (p->estado_cruce4 == 0) {
    	if (ReleaseSemaphore(p->manejador_cruce4, 1, NULL) == FALSE) {
    		exit(9);
		}
	}
			
	if (CRUCE_fin_coche() == -1) {
		exit(7);
	}
	
	EnterCriticalSection(&p->control_procesos_activos); 
	p->procesos_activos = InterlockedDecrement(&p->procesos_activos);
	LeaveCriticalSection(&p->control_procesos_activos);
	
	if (ReleaseSemaphore(p->manejador_semaforo_avance, 1, NULL) == FALSE) {
		exit(9);
	}
	ExitThread(0);
	return 0;
}

DWORD WINAPI peaton(LPVOID parametro) {	
    struct datos *p;
    p = (struct datos *) parametro;
    
    struct posiciOn pos, pos_actual;
	pos_actual.x = -1;
    pos_actual.y = -1;

    int entre_en_el_cruce1 = 0, sali_del_cruce1 = 0;
    int entre_en_el_cruce2 = 0, sali_del_cruce2 = 0;
	int esperar_posicion_siguiente;
	int posicion_conflictiva = 0;
	struct posiciOn (*CRUCE_nuevo_inicio_peatOn)(void);
	struct posiciOn (*CRUCE_avanzar_peatOn)(struct posiciOn);
	
	int (*CRUCE_fin_peatOn)(void);
	CRUCE_nuevo_inicio_peatOn = (struct posiciOn (*)(void)) GetProcAddress((HINSTANCE) p->manejador_dll, "CRUCE_nuevo_inicio_peatOn"); 
	if (CRUCE_nuevo_inicio_peatOn == NULL) {
    	exit(4);
    }
    
    int (*pausa)(void) = (int (*)(void)) GetProcAddress((HINSTANCE) p->manejador_dll, "pausa"); 
	if (pausa == NULL) {
    	exit(4);
    }
    
    CRUCE_avanzar_peatOn = (struct posiciOn (*)(struct posiciOn)) GetProcAddress((HINSTANCE) p->manejador_dll, "CRUCE_avanzar_peatOn"); 
	if (CRUCE_avanzar_peatOn == NULL) {
    	exit(4);
    }
    
    CRUCE_fin_peatOn = (int (*)(void)) GetProcAddress((HINSTANCE) p->manejador_dll, "CRUCE_fin_peatOn"); 
	if (CRUCE_fin_peatOn == NULL) {
    	exit(4);
    }
    
    pos = CRUCE_nuevo_inicio_peatOn();
    
	MSG mensaje;
    PeekMessage(&mensaje, NULL, WM_USER, WM_USER, PM_NOREMOVE);
	
	while (pos.y >= 0) {
		pausa();
		if (pos.y == 11 && pos.x < 30) { 
            if (WaitForSingleObject(p->manejador_semaforo_P2, INFINITE) == WAIT_FAILED) {
            	exit(10);
			}
            posicion_conflictiva = 4;
        }
        else {
	        if (pos.x == 30) {
	            if (WaitForSingleObject(p->manejador_semaforo_P1, INFINITE) == WAIT_FAILED) {
	            	exit(10);
				}
	        	posicion_conflictiva = 3;
			}
		}
		
		if (esperar_posicion_siguiente == 1) {
        	if (GetMessage(&mensaje, NULL, 1500, 2500) == FALSE) {
        		exit(12);
			}
        	esperar_posicion_siguiente = 0;
		}
		
		if (WaitForSingleObject(p->manejador_semaforo_avance, INFINITE) == WAIT_FAILED) {
			exit(10);
		}
		
		if (p->matriz_posiciones[pos.x+3][pos.y].clave != 0) {
        	p->matriz_posiciones[pos.x+3][pos.y].clave = GetCurrentThreadId();
        	p->matriz_posiciones[pos.x+3][pos.y].lista_avisos[p->matriz_posiciones[pos.x+3][pos.y].num_remplazos] = GetCurrentThreadId();
        	p->matriz_posiciones[pos.x+3][pos.y].num_remplazos = p->matriz_posiciones[pos.x+3][pos.y].num_remplazos + 1;
            esperar_posicion_siguiente = 1;	       	
		}
		else {
		    p->matriz_posiciones[pos.x+3][pos.y].clave = GetCurrentThreadId();
		    if (p->matriz_posiciones[pos_actual.x+3][pos_actual.y].clave != GetCurrentThreadId() && p->matriz_posiciones[pos_actual.x+3][pos_actual.y].clave != 0) {
                for (int i = 0; i < p->matriz_posiciones[pos_actual.x+3][pos_actual.y].num_remplazos; i++) {								
		            if (!PostThreadMessage(p->matriz_posiciones[pos_actual.x+3][pos_actual.y].lista_avisos[i], 2000, 0, 0)) {
                        exit(11);
				    }
				}
			}
		    p->matriz_posiciones[pos_actual.x+3][pos_actual.y].clave = 0;
		    for (int i = 0; i < 3; i++) {
		    	 p->matriz_posiciones[pos_actual.x+3][pos_actual.y].lista_avisos[i] = 0;
			}
			p->matriz_posiciones[pos_actual.x+3][pos_actual.y].num_remplazos = 0;
			
			if (pos.y == 11 && pos.x >= 22 && pos.x <= 26 && entre_en_el_cruce2 == 0) {                                
                if (p->estado_cruce2 == 0) {
            	    if (WaitForSingleObject(p->manejador_cruce2, INFINITE) == WAIT_FAILED) {
            	    	exit(10);
					}
			    }
			    EnterCriticalSection(&p->control_estado_cruce2); 
			    p->estado_cruce2 = InterlockedIncrement(&p->estado_cruce2);
	            LeaveCriticalSection(&p->control_estado_cruce2);
			    
                entre_en_el_cruce2 = 1;   
            }
            if (pos.y == 6 && pos.x < 30 && sali_del_cruce2 == 0) {  
				EnterCriticalSection(&p->control_estado_cruce2); 
			    p->estado_cruce2 = InterlockedDecrement(&p->estado_cruce2);
	            LeaveCriticalSection(&p->control_estado_cruce2);                             
                if (p->estado_cruce2 == 0) {
    	            if (ReleaseSemaphore(p->manejador_cruce2, 1, NULL) == FALSE) {
    	            	exit(9);
					}
	            } 
				sali_del_cruce2 = 1;                              
            }
            if (pos.x == 30 && pos.y >= 13 && pos.y <= 15 && entre_en_el_cruce1 == 0) { 
                if (p->estado_cruce1 == 0) {
            	    if (WaitForSingleObject(p->manejador_cruce1, INFINITE) == WAIT_FAILED) {
            	    	exit(10);
					}
			    }
			    EnterCriticalSection(&p->control_estado_cruce1); 
			    p->estado_cruce1 = InterlockedIncrement(&p->estado_cruce1);
	            LeaveCriticalSection(&p->control_estado_cruce1);
	            
                entre_en_el_cruce1 = 1;  
            }
            if (pos.x == 41 && sali_del_cruce1 == 0) {
				EnterCriticalSection(&p->control_estado_cruce1); 
			    p->estado_cruce1 = InterlockedDecrement(&p->estado_cruce1);
	            LeaveCriticalSection(&p->control_estado_cruce1);
                if (p->estado_cruce1 == 0) {
    	            if (ReleaseSemaphore(p->manejador_cruce1, 1, NULL) == FALSE) {
    	            	exit(9);
					}
	            } 
				sali_del_cruce1 = 1; 
            } 	
		                                                                                                                                                        
            pos_actual = pos;
		    pos = CRUCE_avanzar_peatOn(pos);
        }
		
		if (posicion_conflictiva == 4) { 
            if (ReleaseSemaphore(p->manejador_semaforo_P2, 1, NULL) == FALSE) {
            	exit(9);
			}
        	posicion_conflictiva = 0;
		}		
		if (posicion_conflictiva == 3) { 
	        if (ReleaseSemaphore(p->manejador_semaforo_P1, 1, NULL) == FALSE) {
	        	exit(9);
			}
	        posicion_conflictiva = 0;
		}
				 
		if (pos.y >= 0) {
            if (ReleaseSemaphore(p->manejador_semaforo_avance, 1, NULL) == FALSE) {
            	exit(9);
			}
        }  		 			
	}
	
	if (p->matriz_posiciones[pos_actual.x+3][pos_actual.y].clave != GetCurrentThreadId()) {
		for (int i = 0; i < p->matriz_posiciones[pos_actual.x+3][pos_actual.y].num_remplazos; i++) {								
		    if (!PostThreadMessage(p->matriz_posiciones[pos_actual.x+3][pos_actual.y].lista_avisos[i], 2000, 0, 0)) {
                exit(11);
			}
		}
	}
	p->matriz_posiciones[pos_actual.x+3][pos_actual.y].clave = 0;
	for (int i = 0; i < 3; i++) {
		p->matriz_posiciones[pos_actual.x+3][pos_actual.y].lista_avisos[i] = 0;
	}
	p->matriz_posiciones[pos_actual.x+3][pos_actual.y].num_remplazos = 0;
	 	
	if (CRUCE_fin_peatOn() == -1) {
		exit(7);
	}
	
	EnterCriticalSection(&p->control_procesos_activos); 
	p->procesos_activos = InterlockedDecrement(&p->procesos_activos);
	LeaveCriticalSection(&p->control_procesos_activos);
	
	if (ReleaseSemaphore(p->manejador_semaforo_avance, 1, NULL) == FALSE) {
		exit(9);
	}
	 
	ExitThread(0);
	return 0;
}

void n_pausas(int n, HINSTANCE manejador_dll) {
    int i;

    int (*pausa)(void);
    pausa = (int (*)(void)) GetProcAddress(manejador_dll, "pausa");
    if (pausa == NULL) {
        exit(4);
    }
    
    for (i = 0; i < n; i++) pausa();
}

void poner_semaforo(int tipo_semaforo, int valor, HINSTANCE manejador_dll) {
	int (*CRUCE_pon_semAforo)(int, int);
	CRUCE_pon_semAforo = (int (*)(int, int)) GetProcAddress((HINSTANCE) manejador_dll, "CRUCE_pon_semAforo"); 
	if (CRUCE_pon_semAforo == NULL) {
        exit(4);
    }
	
	if (CRUCE_pon_semAforo(tipo_semaforo, valor) == -1) {
	    exit(7);
	}
}
