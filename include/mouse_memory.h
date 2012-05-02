#ifndef MOUSE_MEMORY_H_INCLUDED
#define MOUSE_MEMORY_H_INCLUDED

int a4_mem_erase_block(a4_device *dev, unsigned short page);
int a4_mem_mov_block(a4_device *dev, unsigned short addr);
int a4_mem_mov_word(a4_device *dev, unsigned short addr);
int a4_mem_write_word(a4_device *dev, unsigned short offset, unsigned short word);
unsigned short a4_mem_read_word(a4_device *dev, unsigned short addr);
int a4_mem_read_block(a4_device *dev, unsigned short addr, unsigned short word_sz, void *buf,void (*progress)(int));
int a4_mem_write_block(a4_device *dev, unsigned short addr, unsigned short words_sz, void *inbuf, void (*progress)(int));

#endif // MOUSE_MEMORY_H_INCLUDED
