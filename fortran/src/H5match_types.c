/****p* Program/H5match_types
 *
 * NAME
 *  Executable: H5match_types
 *
 * FILE
 *  fortran/src/H5match_types.c
 *
 * PURPOSE
 *  C Program to match C types to Fortran types.
 *  Creates the files H5f90i_gen.h for the C code and
 *  H5fortran_types.F90 for the Fortran code.
 *
 * COPYRIGHT
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright by The HDF Group.                                               *
 * Copyright by the Board of Trustees of the University of Illinois.         *
 * All rights reserved.                                                      *
 *                                                                           *
 * This file is part of HDF5.  The full HDF5 copyright notice, including     *
 * terms governing use, modification, and redistribution, is contained in    *
 * the COPYING file, which can be found at the root of the source code       *
 * distribution tree, or in https://www.hdfgroup.org/licenses.               *
 * If you do not have access to either file, you may request a copy from     *
 * help@hdfgroup.org.                                                        *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 ******
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "H5public.h"
/* Include H5Ipublic.h for hid_t type */
#include "H5Ipublic.h"

/* Definitions of which fortran type sizes exist */
#include "H5fort_type_defines.h"

/* File pointers for files */
FILE *c_header;
FILE *fort_header;

#define CFILE "H5f90i_gen.h"
#define FFILE "H5fortran_types.F90"

/* Prototypes for the write routines */
void writeTypedef(const char *c_typedef, const char *c_type, int size);
void writeTypedefDefault(const char *c_typedef, int size);
void writeToFiles(const char *c_typedef, const char *fortran_type, const char *c_type, int kind);
void writeToCFileOnly(const char *c_typedef, const char *fortran_type, const char *c_type, int size);
void writeToFilesChr(const char *c_typedef, const char *fortran_type, const char *c_type, int size,
                     const char *kind);

static void
initCfile(void)
{
    fprintf(c_header, "/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\n\
 * Copyright by The HDF Group.                                               *\n\
 * Copyright by the Board of Trustees of the University of Illinois.         *\n\
 * All rights reserved.                                                      *\n\
 *                                                                           *\n\
 * This file is part of HDF5.  The full HDF5 copyright notice, including     *\n\
 * terms governing use, modification, and redistribution, is contained in    *\n\
 * the COPYING file, which can be found at the root of the source code       *\n\
 * distribution tree, or in https://www.hdfgroup.org/licenses.               *\n\
 * If you do not have access to either file, you may request a copy from     *\n\
 * help@hdfgroup.org.                                                        *\n\
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */\n\
\n\n\
#ifndef H5f90i_gen_H\n\
#define H5f90i_gen_H\n\
\n\
/* This file is automatically generated by H5match_types.c at build time. */\n\
\n\
#include \"H5public.h\"\n\n");
}

static void
initFfile(void)
{
    fprintf(fort_header, "! * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * \n\
!   Copyright by The HDF Group.                                               *\n\
!   Copyright by the Board of Trustees of the University of Illinois.         *\n\
!   All rights reserved.                                                      *\n\
!                                                                             *\n\
!   This file is part of HDF5.  The full HDF5 copyright notice, including     *\n\
!   terms governing use, modification, and redistribution, is contained in    *\n\
!   the COPYING file, which can be found at the root of the source code       *\n\
!   distribution tree, or in https://www.hdfgroup.org/licenses.               *\n\
!   If you do not have access to either file, you may request a copy from     *\n\
!   help@hdfgroup.org.                                                        *\n\
! * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\n\
!\n!\n\
! This file is automatically generated by H5match_types.c and contains HDF5 Fortran90 type definitions.\n!\n\
       MODULE H5FORTRAN_TYPES\n\
         USE ISO_C_BINDING\n\
         !\n\
         !  HDF5 integers\n\
         !\n");
}

static void
endCfile(void)
{
    fprintf(c_header, "\n#endif /* _H5f90i_gen_H */\n");
}
static void
endFfile(void)
{
    fprintf(fort_header, "\n        INTEGER(SIZE_T), PARAMETER :: OBJECT_NAMELEN_DEFAULT_F = -1\n\n");
    fprintf(fort_header, "        END MODULE H5FORTRAN_TYPES\n");
}

/* Define a c_int_x type in the C header */
void
writeTypedef(const char *c_typedef, const char *c_type, int size)
{
    fprintf(c_header, "#define c_%s_%u %s\n", c_typedef, size, c_type);
}

/* Call this function if there is no matching C type for sizes > 1 */
void
writeTypedefDefault(const char *c_typedef, int size)
{
    assert(size % 2 == 0);
    fprintf(c_header, "typedef struct {c_%s_%u a; c_%s_%u b;} c_%s_%u;\n", c_typedef, size / 2, c_typedef,
            size / 2, c_typedef, size);
}

/* Create matching Fortran and C types by writing to both files */
void
writeToFiles(const char *c_typedef, const char *fortran_type, const char *c_type, int kind)
{
    fprintf(fort_header, "        INTEGER, PARAMETER :: %s = %u\n", fortran_type, kind);
    fprintf(c_header, "typedef c_%s_%d %s;\n", c_typedef, kind, c_type);
}
void
writeToFilesChr(const char *c_typedef, const char *fortran_type, const char *c_type, int size,
                const char *kind)
{
    fprintf(fort_header, "        INTEGER, PARAMETER :: %s = %s\n", fortran_type, kind);
    fprintf(c_header, "typedef c_%s_%d %s;\n", c_typedef, size, c_type);
}
int
main(void)
{
    int  i;
    char chrA[64], chrB[64];

    int  IntKinds[]         = H5_FORTRAN_INTEGER_KINDS;
    int  IntKinds_SizeOf[]  = H5_FORTRAN_INTEGER_KINDS_SIZEOF;
    int  RealKinds[]        = H5_FORTRAN_REAL_KINDS;
    int  RealKinds_SizeOf[] = H5_FORTRAN_REAL_KINDS_SIZEOF;
    char Real_C_TYPES[10][32];

    int FORTRAN_NUM_INTEGER_KINDS = H5_FORTRAN_NUM_INTEGER_KINDS;
    int H5_FORTRAN_NUM_REAL_KINDS;
#if H5_FORTRAN_HAVE_C_LONG_DOUBLE != 0
    int found_long_double = 0;
#endif

    /* Open target files */
    c_header    = fopen(CFILE, "w");
    fort_header = fopen(FFILE, "w");

    /* Write copyright, boilerplate to both files */
    initCfile();
    initFfile();

    /* (a) define c_int_x */

    FORTRAN_NUM_INTEGER_KINDS = (int)(sizeof(IntKinds) / sizeof(IntKinds[0]));
    H5_FORTRAN_NUM_REAL_KINDS = (int)(sizeof(RealKinds) / sizeof(RealKinds[0]));

    fprintf(fort_header, "        INTEGER, PARAMETER :: H5_FORTRAN_NUM_INTEGER_KINDS = %d\n",
            FORTRAN_NUM_INTEGER_KINDS);

    for (i = 0; i < FORTRAN_NUM_INTEGER_KINDS; i++) {
        if (sizeof(long long) == IntKinds_SizeOf[i])
            writeTypedef("int", "long long", IntKinds[i]);
        else if (sizeof(long) == IntKinds[i])
            writeTypedef("int", "long", IntKinds[i]);
        else if (sizeof(int) == IntKinds_SizeOf[i])
            writeTypedef("int", "int", IntKinds[i]);
        else if (sizeof(short) == IntKinds_SizeOf[i])
            writeTypedef("int", "short", IntKinds[i]);
        else if (IntKinds_SizeOf[i] == 1) {
            writeTypedef("int", "char", IntKinds[i]);
            /* Actually, char is not necessarily one byte.
             * But if char isn't, then nothing is, so this
             * is as close as we can get. */
        }
        else {
            writeTypedefDefault("int", IntKinds[i]);
        }
        if (sizeof(size_t) == IntKinds_SizeOf[i])
            writeTypedef("size_t", "size_t", IntKinds[i]);
        if (sizeof(time_t) == IntKinds_SizeOf[i])
            writeTypedef("time_t", "time_t", IntKinds[i]);
        if (sizeof(hsize_t) == IntKinds_SizeOf[i])
            writeTypedef("hsize_t", "hsize_t", IntKinds[i]);
    }

    /* (b) Define c_float_x */

    for (i = 0; i < H5_FORTRAN_NUM_REAL_KINDS; i++) {
        if (sizeof(float) == RealKinds_SizeOf[i]) {
            writeTypedef("float", "float", RealKinds[i]);
            strcpy(Real_C_TYPES[i], "C_FLOAT");
        }
        else if (sizeof(double) == RealKinds_SizeOf[i]) {
            writeTypedef("float", "double", RealKinds[i]);
            strcpy(Real_C_TYPES[i], "C_DOUBLE");
        }
#if H5_FORTRAN_HAVE_C_LONG_DOUBLE != 0
        else if (sizeof(long double) == RealKinds_SizeOf[i] && found_long_double == 0) {
            writeTypedef("float", "long double", RealKinds[i]);
            strcpy(Real_C_TYPES[i], "C_LONG_DOUBLE");
            found_long_double = 1;
        }
#ifdef H5_HAVE_FLOAT128
        /* Don't select a higher precision than Fortran can support */
        else if (sizeof(__float128) == RealKinds_SizeOf[i] && found_long_double == 1 &&
                 H5_PAC_FC_MAX_REAL_PRECISION > 28) {
            writeTypedef("float", "__float128", RealKinds[i]);
            strcpy(Real_C_TYPES[i], "C_FLOAT128");
        }
#else
        else if (sizeof(long double) == RealKinds_SizeOf[i] && found_long_double == 1 &&
                 H5_PAC_FC_MAX_REAL_PRECISION > 28) {
            writeTypedef("float", "long double", RealKinds[i]);
            strcpy(Real_C_TYPES[i], "C_FLOAT128");
        }
#endif
#else /* There is no C_LONG_DOUBLE intrinsic */
#ifdef H5_HAVE_FLOAT128
        /* Don't select a higher precision than Fortran can support */
        else if (sizeof(__float128) == RealKinds_SizeOf[i]) {
            writeTypedef("float", "__float128", RealKinds[i]);
            strcpy(Real_C_TYPES[i], "C_FLOAT128");
        }
#else
        else if (sizeof(long double) == RealKinds_SizeOf[i]) {
            writeTypedef("float", "long double", RealKinds[i]);
            strcpy(Real_C_TYPES[i], "C_FLOAT128");
        }
#endif
#endif
        else {
            printf("\n                      **** HDF5 WARNING ****\n");
            printf(
                "Fortran REAL(KIND=%d) is %d Bytes, but no corresponding C float type exists of that size\n",
                RealKinds[i], RealKinds_SizeOf[i]);
            printf("     !!! Fortran interfaces will not be generated for REAL(KIND=%d) !!!\n\n",
                   RealKinds[i]);

            RealKinds_SizeOf[i] = -1;
            RealKinds[i]        = -1;
        }
    }

    /* Now begin defining fortran types. */
    fprintf(c_header, "\n");

    /* haddr_t */
    for (i = 0; i < FORTRAN_NUM_INTEGER_KINDS; i++) {
        if (IntKinds_SizeOf[i] == H5_SIZEOF_HADDR_T) {
            writeToFiles("int", "HADDR_T", "haddr_t_f", IntKinds[i]);
            break;
        }
        if (i == (FORTRAN_NUM_INTEGER_KINDS - 1))
            /* Error: couldn't find a size for haddr_t */
            return -1;
    }

    /* hsize_t */
    for (i = 0; i < FORTRAN_NUM_INTEGER_KINDS; i++) {
        if (IntKinds_SizeOf[i] == H5_SIZEOF_HSIZE_T) {
            writeToFiles("hsize_t", "HSIZE_T", "hsize_t_f", IntKinds[i]);
            break;
        }
        if (i == (FORTRAN_NUM_INTEGER_KINDS - 1))
            /* Error: couldn't find a size for hsize_t */
            return -1;
    }

    /* hssize_t */
    for (i = 0; i < FORTRAN_NUM_INTEGER_KINDS; i++) {
        if (IntKinds_SizeOf[i] == H5_SIZEOF_HSSIZE_T) {
            writeToFiles("int", "HSSIZE_T", "hssize_t_f", IntKinds[i]);
            break;
        }
        if (i == (FORTRAN_NUM_INTEGER_KINDS - 1))
            /* Error: couldn't find a size for hssize_t */
            return -1;
    }

    /* off_t */
    for (i = 0; i < FORTRAN_NUM_INTEGER_KINDS; i++) {
        if (IntKinds_SizeOf[i] == H5_SIZEOF_OFF_T) {
            writeToFiles("int", "OFF_T", "off_t_f", IntKinds[i]);
            break;
        }
        if (i == (FORTRAN_NUM_INTEGER_KINDS - 1))
            /* Error: couldn't find a size for off_t */
            return -1;
    }

    /* size_t */
    for (i = 0; i < FORTRAN_NUM_INTEGER_KINDS; i++) {
        if (IntKinds_SizeOf[i] == H5_SIZEOF_SIZE_T) {
            writeToFiles("size_t", "SIZE_T", "size_t_f", IntKinds[i]);
            break;
        }
        if (i == (FORTRAN_NUM_INTEGER_KINDS - 1))
            /* Error: couldn't find a size for size_t */
            return -1;
    }

    /* time_t */
    for (i = 0; i < FORTRAN_NUM_INTEGER_KINDS; i++) {
        if (IntKinds_SizeOf[i] == H5_SIZEOF_TIME_T) {
            writeToFiles("time_t", "TIME_T", "time_t_f", IntKinds[i]);
            break;
        }
        if (i == (FORTRAN_NUM_INTEGER_KINDS - 1))
            /* Error: couldn't find a size for time_t */
            return -1;
    }

    /* int */
    writeToFiles("int", "Fortran_INTEGER", "int_f", H5_FORTRAN_NATIVE_INTEGER_KIND);

    /* int_1, int_2, int_4, int_8 */

    /* Defined different KINDs of integers */

    fprintf(fort_header, "        INTEGER, DIMENSION(1:%d), PARAMETER :: Fortran_INTEGER_AVAIL_KINDS = (/",
            FORTRAN_NUM_INTEGER_KINDS);

    for (i = 0; i < FORTRAN_NUM_INTEGER_KINDS; i++) {
        fprintf(fort_header, "%d", (int)IntKinds[i]);
        if (i == FORTRAN_NUM_INTEGER_KINDS - 1) {
            fprintf(fort_header, "/)\n");
        }
        else {
            fprintf(fort_header, ",");
        }
    }

    /* real_4, real_8, real_16 */

    /* Defined different KINDs of reals:                          */
    /* if the REAL kind is not available then we assign           */
    /* it a value of the next larger one, but if the next         */
    /* higher one is not available we assigned it the next lowest */

    for (i = 0; i < H5_FORTRAN_NUM_REAL_KINDS; i++) {
        if (RealKinds[i] > 0) {
            snprintf(chrA, sizeof(chrA), "Fortran_REAL_%s", Real_C_TYPES[i]);
            snprintf(chrB, sizeof(chrB), "real_%s_f", Real_C_TYPES[i]);
            writeToFiles("float", chrA, chrB, RealKinds[i]);
        }
    }

    /* hid_t */
    for (i = 0; i < FORTRAN_NUM_INTEGER_KINDS; i++) {
        if (IntKinds_SizeOf[i] == H5_SIZEOF_HID_T) {
            writeToFiles("int", "HID_T", "hid_t_f", IntKinds[i]);
            break;
        }
        if (i == (FORTRAN_NUM_INTEGER_KINDS - 1))
            /* Error: couldn't find a size for hid_t */
            return -1;
    }
    /* real_f */
#if H5_FORTRAN_HAVE_C_LONG_DOUBLE != 0
    if (H5_FORTRAN_NATIVE_REAL_SIZEOF == sizeof(long double)) {
        writeToFilesChr("float", "Fortran_REAL", "real_f", H5_FORTRAN_NATIVE_REAL_KIND, "C_LONG_DOUBLE");
    }
    else
#endif
        if (H5_FORTRAN_NATIVE_REAL_SIZEOF == sizeof(double)) {
        writeToFilesChr("float", "Fortran_REAL", "real_f", H5_FORTRAN_NATIVE_REAL_KIND, "C_DOUBLE");
    }
    else if (H5_FORTRAN_NATIVE_REAL_SIZEOF == sizeof(float))
        writeToFilesChr("float", "Fortran_REAL", "real_f", H5_FORTRAN_NATIVE_REAL_KIND, "C_FLOAT");
    else {
        /* No exact match, choose the next highest */
        if (H5_FORTRAN_NATIVE_REAL_SIZEOF > sizeof(long double))
            writeToFilesChr("float", "Fortran_REAL", "real_f", H5_FORTRAN_NATIVE_REAL_KIND, "C_LONG_DOUBLE");
        else if (H5_FORTRAN_NATIVE_REAL_SIZEOF > sizeof(double))
            writeToFilesChr("float", "Fortran_REAL", "real_f", H5_FORTRAN_NATIVE_REAL_KIND, "C_DOUBLE");
        else if (H5_FORTRAN_NATIVE_REAL_SIZEOF > sizeof(float))
            writeToFilesChr("float", "Fortran_REAL", "real_f", H5_FORTRAN_NATIVE_REAL_KIND, "C_FLOAT");
        else {
            /* Error: couldn't find a size for real_f */
            printf("Error: couldn't find a size for real_f \n");
            return -1;
        }
    }

    /* double_f */
#if H5_FORTRAN_HAVE_C_LONG_DOUBLE != 0
    if (H5_FORTRAN_NATIVE_DOUBLE_SIZEOF == sizeof(long double)) {
        writeToFilesChr("float", "Fortran_DOUBLE", "double_f", H5_FORTRAN_NATIVE_DOUBLE_KIND,
                        "C_LONG_DOUBLE");
    }
    else
#endif
        if (H5_FORTRAN_NATIVE_DOUBLE_SIZEOF == sizeof(double)) {
        writeToFilesChr("float", "Fortran_DOUBLE", "double_f", H5_FORTRAN_NATIVE_DOUBLE_KIND, "C_DOUBLE");
    }
    else if (H5_FORTRAN_NATIVE_DOUBLE_SIZEOF == sizeof(float))
        writeToFilesChr("float", "Fortran_DOUBLE", "double_f", H5_FORTRAN_NATIVE_DOUBLE_KIND, "C_FLOAT");
#ifdef H5_HAVE_FLOAT128
    /* Don't select a higher precision than Fortran can support */
    else if (sizeof(__float128) == H5_FORTRAN_NATIVE_DOUBLE_SIZEOF && H5_PAC_FC_MAX_REAL_PRECISION > 28) {
        writeToFilesChr("float", "Fortran_DOUBLE", "double_f", H5_FORTRAN_NATIVE_DOUBLE_KIND,
                        "Fortran_REAL_C_FLOAT128");
    }
#else
    else if (sizeof(long double) == H5_FORTRAN_NATIVE_DOUBLE_SIZEOF && H5_PAC_FC_MAX_REAL_PRECISION > 28) {
        writeToFilesChr("float", "Fortran_DOUBLE", "double_f", H5_FORTRAN_NATIVE_DOUBLE_KIND,
                        "Fortran_REAL_C_FLOAT128");
    }
#endif
    else {
        /* Error: couldn't find a size for double_f */
        printf("Error: couldn't find a size for double_f \n");
        return -1;
    }

    /* Need the buffer size for the fortran derived type 'hdset_reg_ref_t_f03'
     * in order to be interoperable with C's structure, the C buffer size
     * H5R_DSET_REG_REF_BUF_SIZE is (sizeof(haddr_t)+4)
     */

    fprintf(fort_header, "        INTEGER, PARAMETER :: H5R_DSET_REG_REF_BUF_SIZE_F = %u\n",
            H5_SIZEOF_HADDR_T + 4);

    /* Close files */
    endCfile();
    endFfile();
    fclose(c_header);
    fclose(fort_header);

    return 0;
}
