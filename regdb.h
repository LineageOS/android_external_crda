#include <linux/types.h>

/*
 * WARNING: This file needs to be kept in sync with
 *  - the parser (dbparse.py)
 *  - the generator code (db2bin.py)
 */

/* spells "RGDB" */
#define REGDB_MAGIC	0x52474442

/*
 * Only supported version now, start at arbitrary number
 * to have some more magic. We still consider this to be
 * "Version 1" of the file.
 */
#define REGDB_VERSION	19

/*
 * The signature at the end of the file is an RSA-signed
 * SHA-1 hash of the file.
 */

struct regdb_file_header {
	/* must be REGDB_MAGIC */
	__be32	magic;
	/* must be REGDB_VERSION */
	__be32	version;
	/*
	 * Pointer (offset) into file where country list starts
	 * and number of countries. The country list is sorted
	 * alphabetically to allow binary searching (should it
	 * become really huge.)
	 */
	__be32	reg_country_ptr;
	__be32	reg_country_num;
	/* length (in bytes) of the signature at the end of the file */
	__be32	signature_length;
};

struct regdb_file_freq_range {
	__be32	start_freq,
		end_freq,
		max_bandwidth;
};

/*
 * Values of zero mean "not applicable", i.e. the regulatory
 * does not limit a certain value.
 */
struct regdb_file_power_rule {
	/* antenna gain is in mBi (100 * dBi) */
	__be32	max_antenna_gain;
	/* this is in mBm (100 * dBm) */
	__be32	max_eirp;
};

#define EDGE_POWER_SHIFT	11

enum reg_rule_flags {
	RRF_NO_OFDM		= 1<<0,
	RRF_NO_CCK		= 1<<1,
	RRF_NO_INDOOR		= 1<<2,
	RRF_NO_OUTDOOR		= 1<<3,
	RRF_DFS			= 1<<4,
	RRF_PTP_ONLY		= 1<<5,
	RRF_PTMP_ONLY		= 1<<6,
	RRF_PASSIVE_SCAN	= 1<<7,
	RRF_NO_IBSS		= 1<<8,
	RRF_NO_HT20		= 1<<9,
	RRF_NO_HT40		= 1<<10,
	RRF_EDGE_POWER_MASK	= 3<<EDGE_POWER_SHIFT,
};

/* TODO: flesh out what these mean! */
#define EDGE_POWER_NO	0
#define EDGE_POWER_1	1
#define EDGE_POWER_2	2
#define EDGE_POWER_3	3

struct regdb_file_reg_rule {
	/* pointers (offsets) into the file */
	__be32	freq_range_ptr,
		power_rule_ptr;
	/* rule flags */
	__be32 flags;
};

struct regdb_file_reg_rules_collection {
	__be32	reg_rule_num;
	/* pointers (offsets) into the file */
	__be32	reg_rule_ptrs[];
};

struct regdb_file_reg_country {
	__u8	alpha2[2];
	__u8	PAD[2];
	/* pointer (offset) into the file */
	__be32	reg_collection_ptr;
};


/*
 * Verify that no unexpected padding is added to structures
 * for some reason.
 */

#define ERROR_ON(cond) \
	((void)sizeof(char[1 - 2*!!(cond)]))

#define CHECK_STRUCT(name, size) \
	ERROR_ON(sizeof(struct name) != size)

static inline void check_db_binary_structs(void)
{
	CHECK_STRUCT(regdb_file_header, 20);
	CHECK_STRUCT(regdb_file_freq_range, 12);
	CHECK_STRUCT(regdb_file_power_rule, 8);
	CHECK_STRUCT(regdb_file_reg_rule, 12);
	CHECK_STRUCT(regdb_file_reg_rules_collection, 4);
	CHECK_STRUCT(regdb_file_reg_country, 8);
}
