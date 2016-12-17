#include <sigma0/sigma0.h>
#include <c4/arch/interrupts.h>
#include <stdint.h>
#include <stdbool.h>

static inline uint8_t c4_inbyte( unsigned port ){
	int ret;

	DO_SYSCALL( SYSCALL_IOPORT, SYSCALL_IO_INPUT, port, 0, 0, ret );

	return ret;
}

static inline void c4_outbyte( unsigned port, uint8_t value ){
	int ret;

	DO_SYSCALL( SYSCALL_IOPORT, SYSCALL_IO_OUTPUT, port, value, 0, ret );

	return;
}

int c4_msg_send( message_t *buffer, unsigned to ){
	int ret = 0;

	DO_SYSCALL( SYSCALL_SEND, buffer, to, 0, 0, ret );

	return ret;
}

void _start( void *data ){
	uintptr_t display = (uintptr_t)data;
	int ret;

	message_t msg;

	while ( true ){
		unsigned from = MESSAGE_INTERRUPT_MASK | INTERRUPT_KEYBOARD;

		DO_SYSCALL( SYSCALL_RECIEVE, &msg, from, 0, 0, ret );

		unsigned scancode = c4_inbyte( 0x60 );
		bool     key_up   = !!(scancode & 0x80);

		scancode &= ~0x80;

		msg.type = 0xbeef;
		msg.data[0] = scancode;
		msg.data[1] = key_up;

		c4_msg_send( &msg, display );
	}
}
