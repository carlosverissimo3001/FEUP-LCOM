PROG=proj

SRCS = proj.c kbc_int.c sprite.c timer_int.c vcard.c utils.c kbc_verification.c kbc_io.c covid_escape.c 

# __LCOM_OPTIMIZED__ disables most of the LCF's overhead (and functionality!)
CPPFLAGS += -pedantic -D __LCOM_OPTIMIZED__

DPADD += ${LIBLCF}
LDADD += -llcf

.include <minix.lcom.mk>
