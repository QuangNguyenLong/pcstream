#ifndef PCSTREAM_BW_ESTIMATOR_H
#define PCSTREAM_BW_ESTIMATOR_H

#include <pcstream/def.h>
#include <stdlib.h>
enum
{
  PCSTREAM_BW_ESTIMATOR_HARMONIC = 1
};

typedef struct pcs_bw_estimator_t pcs_bw_estimator_t;

struct pcs_bw_estimator_t
{
  PCSTREAM_BW Ra; // (Bytes/s)

  PCSTREAM_RET(*post)
  (pcs_bw_estimator_t *,
   PCSTREAM_BW *, // Average download speed per request
                  // (Bytes/s)
   size_t         // Request count
  );
  PCSTREAM_RET(*get)
  (pcs_bw_estimator_t *, //
   PCSTREAM_BW *         // Available network bandwidth (Mbps)
  );
};

PCSTREAM_RET pcs_bw_estimator_init(pcs_bw_estimator_t *this,
                                   int type);

PCSTREAM_RET pcs_bw_estimator_destroy(pcs_bw_estimator_t *this);

PCSTREAM_RET pcs_bw_estimator_post_harmonic(pcs_bw_estimator_t *this,
                                            PCSTREAM_BW *R,
                                            size_t       M);

PCSTREAM_RET pcs_bw_estimator_get_harmonic(pcs_bw_estimator_t *this,
                                           PCSTREAM_BW *Ra);

#endif
