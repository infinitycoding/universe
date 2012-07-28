#ifndef _logo_h_
#define _logo_h_

static const char __logo[] = "\
            UUUUUUUU     UUUUUUUUNNNNNNNN        NNNNNNNNIIIIIIIIII            \n\
            U::::::U     U::::::UN:::::::N       N::::::NI::::::::I            \n\
            U::::::U     U::::::UN::::::::N      N::::::NI::::::::I            \n\
            UU:::::U     U:::::UUN:::::::::N     N::::::NII::::::II            \n\
             U:::::U     U:::::U N::::::::::N    N::::::N  I::::I              \n\
             U:::::D     D:::::U N:::::::::::N   N::::::N  I::::I              \n\
             U:::::D     D:::::U N:::::::N::::N  N::::::N  I::::I              \n\
             U:::::D     D:::::U N::::::N N::::N N::::::N  I::::I              \n\
             U:::::D     D:::::U N::::::N  N::::N:::::::N  I::::I              \n\
             U:::::D     D:::::U N::::::N   N:::::::::::N  I::::I              \n\
             U:::::D     D:::::U N::::::N    N::::::::::N  I::::I              \n\
             U::::::U   U::::::U N::::::N     N:::::::::N  I::::I              \n\
             U:::::::UUU:::::::U N::::::N      N::::::::NII::::::II            \n\
              UU:::::::::::::UU  N::::::N       N:::::::NI::::::::I            \n\
                UU:::::::::UU    N::::::N        N::::::NI::::::::I            \n\
                  UUUUUUUUU      NNNNNNNN         NNNNNNNIIIIIIIIII            \n\
";

static void printlogo(void)
{
	color_t color = get_color();
	
	set_color(LIGHT_MAGENTA | BLACK << 4);
	
	puts(__logo);
	
	set_color(color);
}

#endif
