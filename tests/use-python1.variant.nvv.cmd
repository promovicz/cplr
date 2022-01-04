../cplr \
-P python3 \
-i Python.h \
-U _XOPEN_SOURCE \
'Py_Initialize()' \
'PyRun_SimpleString("print(\"hello\")")' \
'Py_Finalize()' -nvv
