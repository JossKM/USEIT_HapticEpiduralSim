#pragma once

// quality-of-life debugging preprocessor macros, because i'm lazy - joss
#define HAPTIC_DEBUG

#ifdef HAPTIC_DEBUG
#define PRINT(x) std::cout << x;
#define PRINTLN(x) std::cout << x << std::endl;
#define PRINTLNWAIT(x, time) std::cout << x << std::endl; Sleep(time);
#else
#define PRINTLN(x) ; 
#define PRINTWAIT(x, time) ;
#endif