#if !defined (_FRAMEBUFFER_H_)
#define _FRAMEBUFFER_H_

int framebuffer_init ();

void framebuffer_deinit ();

typedef int (*modeCallback) ( int x, int y, int bpp, void* callbackInfo );

int framebuffer_enum_modes ( modeCallback callbackFunction, 
                             void* callbackInfo );

unsigned char* framebuffer_get_buffer ();

int framebuffer_get_buffer_size ();

void framebuffer_release_buffer ( unsigned char* aBuffer );

void framebuffer_flip_buffer (); 

int framebuffer_cursor_init ();
int framebuffer_cursor_set ( int aX, int aY );
int framebuffer_cursor_enable ( int anEnable );

#endif // _FRAMEBUFFER_H_
