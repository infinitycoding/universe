#ifndef _logo_h_
#define _logo_h_

static const char __logo[] = "\
           UUUUUUUU     UUUUUUUU NNNNNNNN        NNNNNNNN IIIIIIIIII           \n\
           U::::::U     U::::::U N:::::::N       N::::::N I::::::::I           \n\
           U::::::U     U::::::U N::::::::N      N::::::N I::::::::I           \n\
           UU:::::U     U:::::UU N:::::::::N     N::::::N II::::::II           \n\
            U:::::U     U:::::U  N::::::::::N    N::::::N   I::::I             \n\
            U:::::D     D:::::U  N:::::::::::N   N::::::N   I::::I             \n\
            U:::::D     D:::::U  N:::::::N::::N  N::::::N   I::::I             \n\
            U:::::D     D:::::U  N::::::N N::::N N::::::N   I::::I             \n\
            U:::::D     D:::::U  N::::::N  N::::N:::::::N   I::::I             \n\
            U:::::D     D:::::U  N::::::N   N:::::::::::N   I::::I             \n\
            U:::::D     D:::::U  N::::::N    N::::::::::N   I::::I             \n\
            U::::::U   U::::::U  N::::::N     N:::::::::N   I::::I             \n\
            U:::::::UUU:::::::U  N::::::N      N::::::::N II::::::II           \n\
             UU:::::::::::::UU   N::::::N       N:::::::N I::::::::I           \n\
               UU:::::::::UU     N::::::N        N::::::N I::::::::I           \n\
                 UUUUUUUUU       NNNNNNNN         NNNNNNN IIIIIIIIII           \n\
";

static void printlogo(void)
{
	color_t color = get_color();

	set_color(LIGHT_GREEN | BLACK << 4);

	puts(__logo);

	set_color(color);
}

#endif


