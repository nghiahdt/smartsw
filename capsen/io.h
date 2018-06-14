#ifndef IO_H_
#define IO_H_

#include "../stm8s/stm8s.h"

void    io_init(void);
uint8_t io_read(void);
void    io_write(uint8_t);
void    io_callback(void);

#endif //IO_H_
