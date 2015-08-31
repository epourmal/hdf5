/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright by The HDF Group.                                               *
 * Copyright by the Board of Trustees of the University of Illinois.         *
 * All rights reserved.                                                      *
 *                                                                           *
 * This file is part of HDF5.  The full HDF5 copyright notice, including     *
 * terms governing use, modification, and redistribution, is contained in    *
 * the files COPYING and Copyright.html.  COPYING can be found at the root   *
 * of the source code distribution tree; Copyright.html can be found at the  *
 * root level of an installed copy of the electronic HDF5 document set and   *
 * is linked from the top-level documents page.  It can also be found at     *
 * http://hdfgroup.org/HDF5/doc/Copyright.html.  If you do not have          *
 * access to either file, you may request a copy from help@hdfgroup.org.     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* Programmer:  Jerome Soumagne <jsoumagne@hdfgroup.org>
 *              Wednesday, Sep 24, 2014
 */

#include "h5test.h"

const char *FILENAME[] = {
    "query",
    NULL
};

static hid_t
test_query_create(void)
{
    int min = 17;
    int max = 22;
    double value1 = 21.2f;
    int value2 = 25;
    hid_t q1 = H5I_BADID, q2 = H5I_BADID, q3 = H5I_BADID, q4 = H5I_BADID;
    hid_t q5 = H5I_BADID, q6 = H5I_BADID, q7 = H5I_BADID;

    /* Create and combine a bunch of queries
     * Query is: ((17 < x < 22) && (x != 21.2)) || (x == 25)
     */
    if ((q1 = H5Qcreate(H5Q_TYPE_DATA_ELEM, H5Q_MATCH_GREATER_THAN,
            H5T_NATIVE_INT, &min)) < 0) FAIL_STACK_ERROR;
    if ((q2 = H5Qcreate(H5Q_TYPE_DATA_ELEM, H5Q_MATCH_LESS_THAN, H5T_NATIVE_INT,
            &max)) < 0) FAIL_STACK_ERROR;
    if ((q3 = H5Qcombine(q1, H5Q_COMBINE_AND, q2)) < 0) FAIL_STACK_ERROR;
    if ((q4 = H5Qcreate(H5Q_TYPE_DATA_ELEM, H5Q_MATCH_NOT_EQUAL,
            H5T_NATIVE_DOUBLE, &value1)) < 0) FAIL_STACK_ERROR;
    if ((q5 = H5Qcombine(q3, H5Q_COMBINE_AND, q4)) < 0) FAIL_STACK_ERROR;
    if ((q6 = H5Qcreate(H5Q_TYPE_DATA_ELEM, H5Q_MATCH_EQUAL, H5T_NATIVE_INT,
            &value2)) < 0) FAIL_STACK_ERROR;
    if ((q7 = H5Qcombine(q5, H5Q_COMBINE_OR, q6)) < 0) FAIL_STACK_ERROR;

    return q7;

error:
    H5E_BEGIN_TRY {
        H5Qclose(q1);
        H5Qclose(q2);
        H5Qclose(q3);
        H5Qclose(q4);
        H5Qclose(q5);
        H5Qclose(q6);
        H5Qclose(q7);
    } H5E_END_TRY;
    return -1;
}

static herr_t
test_query_close(hid_t query)
{
    H5Q_combine_op_t op_type;

    H5Qget_combine_op(query, &op_type);
    if (op_type == H5Q_SINGLETON) {
        if (H5Qclose(query) < 0) FAIL_STACK_ERROR;
    } else {
        hid_t sub_query1, sub_query2;

        if (H5Qget_components(query, &sub_query1, &sub_query2) < 0)
            FAIL_STACK_ERROR;

        if (test_query_close(sub_query1) < 0) FAIL_STACK_ERROR;
        if (test_query_close(sub_query2) < 0) FAIL_STACK_ERROR;
    }

    return 0;

error:
    H5E_BEGIN_TRY {
        /* Nothing */
    } H5E_END_TRY;
    return -1;
}

static herr_t
test_query_apply_elem(hid_t query, hbool_t *result, hid_t type_id, const void *value)
{
    H5Q_combine_op_t op_type;

    H5Qget_combine_op(query, &op_type);
    if (op_type == H5Q_SINGLETON) {
        H5Q_type_t query_type;

        if (H5Qget_type(query, &query_type) < 0) FAIL_STACK_ERROR;

        /* Query type should be H5Q_TYPE_DATA_ELEM */
        if (query_type != H5Q_TYPE_DATA_ELEM) FAIL_STACK_ERROR;
        if (H5Qapply_singleton(query, result, type_id, value) < 0) FAIL_STACK_ERROR;
    } else {
        hbool_t sub_result1, sub_result2;
        hid_t sub_query1_id, sub_query2_id;

        if (H5Qget_components(query, &sub_query1_id, &sub_query2_id) < 0)
            FAIL_STACK_ERROR;
        if (test_query_apply_elem(sub_query1_id, &sub_result1, type_id, value) < 0)
            FAIL_STACK_ERROR;
        if (test_query_apply_elem(sub_query2_id, &sub_result2, type_id, value) < 0)
            FAIL_STACK_ERROR;

        *result = (op_type == H5Q_COMBINE_AND) ? sub_result1 && sub_result2 :
                sub_result1 || sub_result2;

        if (H5Qclose(sub_query1_id) < 0) FAIL_STACK_ERROR;
        if (H5Qclose(sub_query2_id) < 0) FAIL_STACK_ERROR;
    }

    return 0;

error:
    H5E_BEGIN_TRY {
        /* Nothing */
    } H5E_END_TRY;
    return -1;
}

static herr_t
test_query_apply(hid_t query)
{
    int data_elem1 = 15, data_elem2 = 20, data_elem3 = 25;
    double data_elem4 = 21.2f;
    float data_elem5 = 17.2f;
    double data_elem6 = 18.0f;
    double data_elem7 = 2.4f;
    double data_elem8 = 25.0f;
    hbool_t result = 0;

    if (test_query_apply_elem(query, &result, H5T_NATIVE_INT, &data_elem1) < 0)
        FAIL_STACK_ERROR;
    if (!result) {
//        printf("Data element (%d) does not match query\n", data_elem1);
    } else {
        FAIL_STACK_ERROR;
    }

    if (test_query_apply_elem(query, &result, H5T_NATIVE_INT, &data_elem2) < 0)
        FAIL_STACK_ERROR;
    if (result) {
//        printf("Data element (%d) matches query\n", data_elem2);
    } else {
        FAIL_STACK_ERROR;
    }

    if (test_query_apply_elem(query, &result, H5T_NATIVE_INT, &data_elem3) < 0)
        FAIL_STACK_ERROR;
    if (result) {
//        printf("Data element (%d) matches query\n", data_elem3);
    } else {
        FAIL_STACK_ERROR;
    }

    if (test_query_apply_elem(query, &result, H5T_NATIVE_DOUBLE, &data_elem4) < 0)
        FAIL_STACK_ERROR;
    if (!result) {
//        printf("Data element (%lf) does not match query\n", data_elem4);
    } else {
        FAIL_STACK_ERROR;
    }

    if (test_query_apply_elem(query, &result, H5T_NATIVE_FLOAT, &data_elem5) < 0)
        FAIL_STACK_ERROR;
    if (result) {
//        printf("Data element (%f) matches query\n", data_elem5);
    } else {
        FAIL_STACK_ERROR;
    }

    if (test_query_apply_elem(query, &result, H5T_NATIVE_DOUBLE, &data_elem6) < 0)
        FAIL_STACK_ERROR;
    if (result) {
//        printf("Data element (%f) matches query\n", data_elem6);
    } else {
        FAIL_STACK_ERROR;
    }

    if (test_query_apply_elem(query, &result, H5T_NATIVE_DOUBLE, &data_elem7) < 0)
        FAIL_STACK_ERROR;
    if (!result) {
//        printf("Data element (%f) does not match query\n", data_elem7);
    } else {
        FAIL_STACK_ERROR;
    }

    if (test_query_apply_elem(query, &result, H5T_NATIVE_DOUBLE, &data_elem8) < 0)
        FAIL_STACK_ERROR;
    if (result) {
//        printf("Data element (%f) matches query\n", data_elem8);
    } else {
        FAIL_STACK_ERROR;
    }

    return 0;

error:
    H5E_BEGIN_TRY {
        /* Nothing */
    } H5E_END_TRY;
    return -1;
}

static herr_t
test_query_encode(hid_t query)
{
    hid_t dec_query = H5I_BADID;
    char *buf = NULL;
    size_t nalloc = 0;

    if (H5Qencode(query, NULL, &nalloc) < 0) FAIL_STACK_ERROR;
    buf = (char *) malloc(nalloc);
    if (H5Qencode(query, buf, &nalloc) < 0) FAIL_STACK_ERROR;
    if ((dec_query = H5Qdecode(buf)) < 0) FAIL_STACK_ERROR;

    /* Check/apply decoded query */
    if (test_query_apply(dec_query) < 0) FAIL_STACK_ERROR;

    if (H5Qclose(dec_query) < 0) FAIL_STACK_ERROR;
    HDfree(buf);

    return 0;

error:
    H5E_BEGIN_TRY {
        if (dec_query != H5I_BADID) H5Qclose(dec_query);
        HDfree(buf);
    } H5E_END_TRY;
    return -1;
}

int
main(int argc, char **argv)
{
    char filename[1024]; /* file name */
    hid_t query = H5I_BADID, fapl = H5I_BADID;

    /* Reset library */
    h5_reset();
    fapl = h5_fileaccess();

    h5_fixname(FILENAME[0], fapl, filename, sizeof(filename));
    /* Check that no object is left open */
    H5Pset_fclose_degree(fapl, H5F_CLOSE_SEMI);

    TESTING("query create and combine");

    if ((query = test_query_create()) < 0) FAIL_STACK_ERROR;

    PASSED();

    TESTING("query apply");

    if (test_query_apply(query) < 0) FAIL_STACK_ERROR;

    PASSED();

    TESTING("query encode/decode");

    if (test_query_encode(query) < 0) FAIL_STACK_ERROR;


    PASSED();

    TESTING("query close");

    if (test_query_close(query) < 0) FAIL_STACK_ERROR;

    PASSED();

    /* Verify symbol table messages are cached */
    if(h5_verify_cached_stabs(FILENAME, fapl) < 0) TEST_ERROR

    puts("All query tests passed.");
    h5_cleanup(FILENAME, fapl);

    return 0;

error:
    puts("*** TESTS FAILED ***");
    H5E_BEGIN_TRY {
        test_query_close(query);
    } H5E_END_TRY;

    return 1;
}