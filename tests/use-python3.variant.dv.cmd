../bin/cplr \
-P python3 \
-i Python.h \
-U _XOPEN_SOURCE \
-b 'Py_Initialize()' \
-e 'for(int i = 1; i < argc; i++) PyRun_SimpleString(argv[i])' \
-a 'Py_Finalize()' -dv -- \
'import os' \
'os.system("echo this actually works")' \
'os.system("echo oh yes absolutely it does")'
