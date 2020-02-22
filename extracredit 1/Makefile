default: M-M.c
	gcc -O0 -o m-m M-M.c

sparse: M-M-sparse.c
	gcc -O0 -o sparse M-M-sparse.c

noopt: M-M_none.c
	gcc -O0 -o m-m_no M-M_none.c

reg-sparse: sparse_reg.c
	gcc -O0 -o reg-sparse sparse_reg.c