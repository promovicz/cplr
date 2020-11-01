../bin/cplr \
-P python3 \
-i Python.h \
-U _XOPEN_SOURCE \
-b 'Py_Initialize()' \
-e 'PyRun_SimpleString("print(\"hello\")")' \
-a 'Py_Finalize()' -dnp
