#undef TARGET_OS_CPP_BUILTINS
#define TARGET_OS_CPP_BUILTINS()      \
	do {                                \
		builtin_define_std ("universe");      \
		builtin_define_std ("unix");      \
		builtin_assert ("system=universe");   \
		builtin_assert ("system=unix");   \
    } while(0);

