/*
 * Copyright � 1999 NCSA
 *                  All rights reserved.
 *
 * Programmer:  Robb Matzke <matzke@llnl.gov>
 *              Monday, July 26, 1999
 */
#ifndef _H5FDpublic_H
#define _H5FDpublic_H

/* Types of allocation requests */
typedef enum H5FD_mem_t {
    H5FD_MEM_NOLIST	= -1,			/*must be negative*/
    H5FD_MEM_DEFAULT	= 0,			/*must be zero*/
    H5FD_MEM_SUPER,
    H5FD_MEM_BTREE,
    H5FD_MEM_DRAW,
    H5FD_MEM_META,
    H5FD_MEM_GROUP,
    H5FD_MEM_GHEAP,
    H5FD_MEM_LHEAP,
    H5FD_MEM_OHDR,

    H5FD_MEM_NTYPES				/*must be last*/
} H5FD_mem_t;
/*
 * A free-list map which maps all types of allocation requests to a single
 * free list.  This is useful for drivers that don't really care about
 * keeping different requests segregated in the underlying file and which
 * want to make most efficient reuse of freed memory.  The use of the
 * H5FD_MEM_SUPER free list is arbitrary.
 */
#define H5FD_FLMAP_SINGLE {						      \
    H5FD_MEM_SUPER,			/*default*/			      \
    H5FD_MEM_SUPER,			/*super*/			      \
    H5FD_MEM_SUPER,			/*btree*/			      \
    H5FD_MEM_SUPER,			/*draw*/			      \
    H5FD_MEM_SUPER,			/*meta*/			      \
    H5FD_MEM_SUPER,			/*group*/			      \
    H5FD_MEM_SUPER,			/*gheap*/			      \
    H5FD_MEM_SUPER,			/*lheap*/			      \
    H5FD_MEM_SUPER			/*ohdr*/			      \
}

/*
 * A free-list map which segregates requests into `raw' or `meta' data
 * pools.
 */
#define H5FD_FLMAP_DICHOTOMY {						      \
    H5FD_MEM_META,			/*default*/			      \
    H5FD_MEM_META,			/*super*/			      \
    H5FD_MEM_META,			/*btree*/			      \
    H5FD_MEM_DRAW,			/*draw*/			      \
    H5FD_MEM_META,			/*meta*/			      \
    H5FD_MEM_META,			/*group*/			      \
    H5FD_MEM_META,			/*gheap*/			      \
    H5FD_MEM_META,			/*lheap*/			      \
    H5FD_MEM_META			/*ohdr*/			      \
}

/*
 * The default free list map which causes each request type to use it's own
 * free-list.
 */
#define H5FD_FLMAP_DEFAULT {						      \
    H5FD_MEM_DEFAULT,			/*default*/			      \
    H5FD_MEM_DEFAULT,			/*super*/			      \
    H5FD_MEM_DEFAULT,			/*btree*/			      \
    H5FD_MEM_DEFAULT,			/*draw*/			      \
    H5FD_MEM_DEFAULT,			/*meta*/			      \
    H5FD_MEM_DEFAULT,			/*group*/			      \
    H5FD_MEM_DEFAULT,			/*gheap*/			      \
    H5FD_MEM_DEFAULT,			/*lheap*/			      \
    H5FD_MEM_DEFAULT			/*ohdr*/			      \
}

	

/* Forward declaration */
typedef struct H5FD_t H5FD_t;

/* Class information for each file driver */
typedef struct H5FD_class_t {
    const char *name;
    haddr_t maxaddr;
    size_t fapl_size;
    void *(*fapl_copy)(const void *fapl);
    herr_t (*fapl_free)(void *fapl);
    size_t dxpl_size;
    void *(*dxpl_copy)(const void *dxpl);
    herr_t (*dxpl_free)(void *dxpl);
    H5FD_t *(*open)(const char *name, unsigned flags, hid_t fapl,
		    haddr_t maxaddr);
    herr_t (*close)(H5FD_t *file);
    int (*cmp)(const H5FD_t *f1, const H5FD_t *f2);
    haddr_t (*alloc)(H5FD_t *file, H5FD_mem_t type, hsize_t size);
    herr_t (*free)(H5FD_t *file, H5FD_mem_t type, haddr_t addr, hsize_t size);
    haddr_t (*get_eoa)(H5FD_t *file);
    herr_t (*set_eoa)(H5FD_t *file, haddr_t addr);
    haddr_t (*get_eof)(H5FD_t *file);
    herr_t (*read)(H5FD_t *file, hid_t dxpl, haddr_t addr, hsize_t size,
		   void *buffer);
    herr_t (*write)(H5FD_t *file, hid_t dxpl, haddr_t addr, hsize_t size,
		    const void *buffer);
    herr_t (*flush)(H5FD_t *file);
    H5FD_mem_t fl_map[H5FD_MEM_NTYPES];
} H5FD_class_t;

/* A free list is a singly-linked list of address/size pairs. */
typedef struct H5FD_free_t {
    haddr_t		addr;
    hsize_t		size;
    struct H5FD_free_t	*next;
} H5FD_free_t;

/*
 * The main datatype for each driver. Public fields common to all drivers are
 * declared here and the driver appends private fields in memory.
 */
struct H5FD_t {
    hid_t		driver_id;	/*driver ID for this file	*/
    const H5FD_class_t	*cls;		/*constant class info		*/
    haddr_t		maxaddr;	/*for this file, overrides class*/
    H5FD_free_t		*fl[H5FD_MEM_NTYPES];/*freelist per allocation type*/
};

#ifdef __cplusplus
extern "C" {
#endif

/* Function prototypes */
hid_t H5FDregister(const H5FD_class_t *cls);
herr_t H5FDunregister(hid_t driver_id);
H5FD_t *H5FDopen(const char *name, unsigned flags, hid_t fapl_id,
		 haddr_t maxaddr);
herr_t H5FDclose(H5FD_t *file);
int H5FDcmp(const H5FD_t *f1, const H5FD_t *f2);
haddr_t H5FDalloc(H5FD_t *file, H5FD_mem_t type, hsize_t size);
herr_t H5FDfree(H5FD_t *file, H5FD_mem_t type, haddr_t addr, hsize_t size);
haddr_t H5FDrealloc(H5FD_t *file, H5FD_mem_t type, haddr_t addr,
		    hsize_t old_size, hsize_t new_size);
haddr_t H5FDget_eoa(H5FD_t *file);
herr_t H5FDset_eoa(H5FD_t *file, haddr_t eof);
haddr_t H5FDget_eof(H5FD_t *file);
herr_t H5FDread(H5FD_t *file, hid_t dxpl_id, haddr_t addr, hsize_t size,
		void *buf/*out*/);
herr_t H5FDwrite(H5FD_t *file, hid_t dxpl_id, haddr_t addr, hsize_t size,
		 const void *buf);
herr_t H5FDflush(H5FD_t *file);

#ifdef __cplusplus
}
#endif
#endif
