#ifndef PCA9533_H
#define PCA9533_H

#include "driver/i2c_master.h"

extern i2c_master_dev_handle_t PCA9533_handle;

void PCA9533_init(void);


#endif // PCA9533_H
