#ifndef PCSTREAM_BW_ESTIMATOR_H
#define PCSTREAM_BW_ESTIMATOR_H

#include <pcstream/def.h>
#include <stdlib.h>

typedef struct pcs_bw_estimator_t pcs_bw_estimator_t;

/**
 * @struct pcs_bw_estimator_t
 * @brief Structure representing a bandwidth estimator.
 * @details This structure stores the estimated bandwidth (`Ra`),
 * and provides methods that estimate and retrieve the estimated
 * bandwidth. The estimation algorithm used depend on the specific
 * estimator chosen.
 *
 * To use this structure, follow these steps:
 *
 * 1. Initialize the estimator using `pcs_bw_estimator_init`.
 * 2. Estimate bandwidth using the `post` method.
 * 3. Retrieve the estimated bandwidth using the `get` method.
 * 4. Destroy the estimator using `pcs_bw_estimator_destroy` when
 * done.
 *
 * Example usage:
 *
 * @code
 *    PCSTREAM_BW Ra = 0;
 *    PCSTREAM_BW dl_speeds[] = {123, 456, 789};
 *    pcs_bw_estimator_t bwes = {0};
 *
 *    pcs_bw_estimator_init(&bwes, PCSTREAM_BW_ESTIMATOR_HARMONIC);
 *
 *    bwes.post(&bwes, dl_speeds, sizeof(dl_speeds));
 *
 *    bwes.get(&bwes, &Ra);
 *
 *    // Use the estimated bandwidth (Ra)
 *
 *    pcs_bw_estimator_destroy(&bwes);
 * @endcode
 */

struct pcs_bw_estimator_t
{
  /**
   * @brief Estimated bandwidth.
   * @details This field stores the estimated bandwidth in bytes per
   * second.
   */
  PCSTREAM_BW dls_es; // (Bytes/s)

  /**
   * @brief Method to estimate current network bandwidth.
   * @details This method is responsible for processing the
   * provided download speeds and updating the estimated bandwidth
   * based on the estimator type.
   */
  PCSTREAM_RET(*post)
  (pcs_bw_estimator_t *,
   PCSTREAM_BW *, // Average download speed per request
                  // (Bytes/s)
   size_t         // Request count
  );

  /**
   * @brief Getter for the estimated bandwidth `Ra`.
   * @details This method retrieves the current bandwidth
   * estimation.
   */
  PCSTREAM_RET(*get)
  (pcs_bw_estimator_t *, //
   PCSTREAM_BW *         // Available network bandwidth (Mbps)
  );
};

/**
 * @brief Initializer of `pcs_bw_estimator_t`.
 * @details This function initializes a pcs_bw_estimator_t instance
 * by setting its `Ra` field and assigning appropriate function
 * pointers for the selected estimator type.
 *
 * @param self Pointer to the `pcs_bw_estimator_t` instance.
 * @param type Type of bandwidth estimator to use.
 * @return `PCSTREAM_RET_SUCCESS` on successful initialization.
 */
PCSTREAM_EXPORT PCSTREAM_RET
pcs_bw_estimator_init(pcs_bw_estimator_t *self, int type);

/**
 * @brief Destroyer of `pcs_bw_estimator_t`.
 * @details This function resets the `pcs_bw_estimator_t` instance
 * and clears its fields.
 *
 * @param self Pointer to the `pcs_bw_estimator_t` instance.
 * @return `PCSTREAM_RET_SUCCESS` on successful destruction.
 */
PCSTREAM_EXPORT PCSTREAM_RET
             pcs_bw_estimator_destroy(pcs_bw_estimator_t *self);

/**
 * @brief Estimate network bandwidth using harmonic
 * mean.
 * @details This function calculates the harmonic mean of the
 * recorded download speeds in `R` and updates the `Ra` field of the
 * estimator.
 *
 * @param self Pointer to the pcs_bw_estimator_t instance.
 * @param dls_arr Array of recorded download speeds.
 * @param dls_count The number of recorded download speeds.
 * @return PCSTREAM_RET_SUCCESS on successful processing,
 *         PCSTREAM_RET_FAIL on failure.
 */
PCSTREAM_RET pcs_bw_estimator_post_harmonic(pcs_bw_estimator_t *self,
                                            PCSTREAM_BW *dls_arr,
                                            size_t       dls_count);

/**
 * @brief Get the estimateid bandwidth using the harmonic mean.
 * @details This function retrieves the current estimated bandwidth
 * from the `Ra` field of the estimator.
 *
 * @param self Pointer to the pcs_bw_estimator_t instance.
 * @param Ra Pointer to store the estimated bandwidth.
 * @return PCSTREAM_RET_SUCCESS on successful retrieval,
 *         PCSTREAM_RET_FAIL if fail.
 */
PCSTREAM_RET pcs_bw_estimator_get_harmonic(pcs_bw_estimator_t *self,
                                           PCSTREAM_BW *dls_es);

#endif // PCSTREAM_BW_ESTIMATOR_H
