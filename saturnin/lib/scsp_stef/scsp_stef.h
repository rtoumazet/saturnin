#pragma once
#ifndef _SCSP_STEF_H
#define _SCSP_STEF_H

#define _CDECL   __cdecl
#define FASTCALL __fastcall
//#define FASTCALL
#define DECL_FASTCALL(type, name) type FASTCALL name
//#define INLINE                    __inline
#define INLINE static __inline

extern DECL_FASTCALL(unsigned char, scsp_r_b(unsigned long));
extern DECL_FASTCALL(unsigned short, scsp_r_w(unsigned long));
extern DECL_FASTCALL(unsigned long, scsp_r_d(unsigned long));
extern DECL_FASTCALL(void, scsp_w_b(unsigned long, unsigned char));
extern DECL_FASTCALL(void, scsp_w_w(unsigned long, unsigned short));
extern DECL_FASTCALL(void, scsp_w_d(unsigned long, unsigned long));

extern void scsp_init(unsigned char* scsp_ram, void* sint_hand, void* mint_hand);
extern void scsp_shutdown(void);
extern void scsp_reset(void);

extern char* scsp_debug_ilist(int, char*);
extern char* scsp_debug_slist_on(void);

extern void          scsp_midi_in_send(unsigned char data);
extern void          scsp_midi_out_send(unsigned char data);
extern unsigned char scsp_midi_in_read(void);
extern unsigned char scsp_midi_out_read(void);
extern void          scsp_update(long* bufL, long* bufR, unsigned long len);
extern void          scsp_update_timer(unsigned long len);

#endif
