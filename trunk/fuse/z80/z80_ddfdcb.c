/* z80_ddfdcb.c: z80 DDCBxx and FDCBxx opcodes
   Copyright (c) 1999-2002 Philip Kendall
   
   $Id$

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

   Author contact information:

   E-mail: pak21-fuse@srcf.ucam.org
   Postal address: 15 Crescent Road, Wokingham, Berks, RG40 2DB, England

*/

/* FIXME: Contention details are unknown */

case 0x00:	/* LD B,RLC (REGISTER+dd) */
tstates += 8;
B=readbyte(tempaddr);
RLC(B);
writebyte(tempaddr, B);
break;

case 0x01:	/* LD C,RLC (REGISTER+dd) */
tstates += 8;
C=readbyte(tempaddr);
RLC(C);
writebyte(tempaddr, C);
break;

case 0x02:	/* LD D,RLC (REGISTER+dd) */
tstates += 8;
D=readbyte(tempaddr);
RLC(D);
writebyte(tempaddr, D);
break;

case 0x03:	/* LD E,RLC (REGISTER+dd) */
tstates += 8;
E=readbyte(tempaddr);
RLC(E);
writebyte(tempaddr, E);
break;

case 0x04:	/* LD H,RLC (REGISTER+dd) */
tstates += 8;
H=readbyte(tempaddr);
RLC(H);
writebyte(tempaddr, H);
break;

case 0x05:	/* LD L,RLC (REGISTER+dd) */
tstates += 8;
L=readbyte(tempaddr);
RLC(L);
writebyte(tempaddr, L);
break;

case 0x06:	/* RLC (REGISTER+dd) */
tstates += 8;
{
  BYTE bytetemp = readbyte(tempaddr);
  RLC(bytetemp);
  writebyte(tempaddr,bytetemp);
}
break;

case 0x07:	/* LD A,RLC (REGISTER+dd) */
tstates += 8;
A=readbyte(tempaddr);
RLC(A);
writebyte(tempaddr, A);
break;

case 0x08:	/* LD B,RRC (REGISTER+dd) */
tstates += 8;
B=readbyte(tempaddr);
RRC(B);
writebyte(tempaddr, B);
break;

case 0x09:	/* LD C,RRC (REGISTER+dd) */
tstates += 8;
C=readbyte(tempaddr);
RRC(C);
writebyte(tempaddr, C);
break;

case 0x0a:	/* LD D,RRC (REGISTER+dd) */
tstates += 8;
D=readbyte(tempaddr);
RRC(D);
writebyte(tempaddr, D);
break;

case 0x0b:	/* LD E,RRC (REGISTER+dd) */
tstates += 8;
E=readbyte(tempaddr);
RRC(E);
writebyte(tempaddr, E);
break;

case 0x0c:	/* LD H,RRC (REGISTER+dd) */
tstates += 8;
H=readbyte(tempaddr);
RRC(H);
writebyte(tempaddr, H);
break;

case 0x0d:	/* LD L,RRC (REGISTER+dd) */
tstates += 8;
L=readbyte(tempaddr);
RRC(L);
writebyte(tempaddr, L);
break;

case 0x0e:	/* RRC (REGISTER+dd) */
tstates += 8;
{
  BYTE bytetemp = readbyte(tempaddr);
  RRC(bytetemp);
  writebyte(tempaddr,bytetemp);
}
break;

case 0x0f:	/* LD A,RRC (REGISTER+dd) */
tstates += 8;
A=readbyte(tempaddr);
RRC(A);
writebyte(tempaddr, A);
break;

case 0x10:	/* LD B,RL (REGISTER+dd) */
tstates += 8;
B=readbyte(tempaddr);
RL(B);
writebyte(tempaddr, B);
break;

case 0x11:	/* LD C,RL (REGISTER+dd) */
tstates += 8;
C=readbyte(tempaddr);
RL(C);
writebyte(tempaddr, C);
break;

case 0x12:	/* LD D,RL (REGISTER+dd) */
tstates += 8;
D=readbyte(tempaddr);
RL(D);
writebyte(tempaddr, D);
break;

case 0x13:	/* LD E,RL (REGISTER+dd) */
tstates += 8;
E=readbyte(tempaddr);
RL(E);
writebyte(tempaddr, E);
break;

case 0x14:	/* LD H,RL (REGISTER+dd) */
tstates += 8;
H=readbyte(tempaddr);
RL(H);
writebyte(tempaddr, H);
break;

case 0x15:	/* LD L,RL (REGISTER+dd) */
tstates += 8;
L=readbyte(tempaddr);
RL(L);
writebyte(tempaddr, L);
break;

case 0x16:	/* RL (REGISTER+dd) */
tstates += 8;
{
  BYTE bytetemp = readbyte(tempaddr);
  RL(bytetemp);
  writebyte(tempaddr,bytetemp);
}
break;

case 0x17:	/* LD A,RL (REGISTER+dd) */
tstates += 8;
A=readbyte(tempaddr);
RL(A);
writebyte(tempaddr, A);
break;

case 0x18:	/* LD B,RR (REGISTER+dd) */
tstates += 8;
B=readbyte(tempaddr);
RR(B);
writebyte(tempaddr, B);
break;

case 0x19:	/* LD C,RR (REGISTER+dd) */
tstates += 8;
C=readbyte(tempaddr);
RR(C);
writebyte(tempaddr, C);
break;

case 0x1a:	/* LD D,RR (REGISTER+dd) */
tstates += 8;
D=readbyte(tempaddr);
RR(D);
writebyte(tempaddr, D);
break;

case 0x1b:	/* LD E,RR (REGISTER+dd) */
tstates += 8;
E=readbyte(tempaddr);
RR(E);
writebyte(tempaddr, E);
break;

case 0x1c:	/* LD H,RR (REGISTER+dd) */
tstates += 8;
H=readbyte(tempaddr);
RR(H);
writebyte(tempaddr, H);
break;

case 0x1d:	/* LD L,RR (REGISTER+dd) */
tstates += 8;
L=readbyte(tempaddr);
RR(L);
writebyte(tempaddr, L);
break;

case 0x1e:	/* RR (REGISTER+dd) */
tstates += 8;
{
  BYTE bytetemp = readbyte(tempaddr);
  RR(bytetemp);
  writebyte(tempaddr,bytetemp);
}
break;

case 0x1f:	/* LD A,RR (REGISTER+dd) */
tstates += 8;
A=readbyte(tempaddr);
RR(A);
writebyte(tempaddr, A);
break;

case 0x20:	/* LD B,SLA (REGISTER+dd) */
tstates += 8;
B=readbyte(tempaddr);
SLA(B);
writebyte(tempaddr, B);
break;

case 0x21:	/* LD C,SLA (REGISTER+dd) */
tstates += 8;
C=readbyte(tempaddr);
SLA(C);
writebyte(tempaddr, C);
break;

case 0x22:	/* LD D,SLA (REGISTER+dd) */
tstates += 8;
D=readbyte(tempaddr);
SLA(D);
writebyte(tempaddr, D);
break;

case 0x23:	/* LD E,SLA (REGISTER+dd) */
tstates += 8;
E=readbyte(tempaddr);
SLA(E);
writebyte(tempaddr, E);
break;

case 0x24:	/* LD H,SLA (REGISTER+dd) */
tstates += 8;
H=readbyte(tempaddr);
SLA(H);
writebyte(tempaddr, H);
break;

case 0x25:	/* LD L,SLA (REGISTER+dd) */
tstates += 8;
L=readbyte(tempaddr);
SLA(L);
writebyte(tempaddr, L);
break;

case 0x26:	/* SLA (REGISTER+dd) */
tstates += 8;
{
  BYTE bytetemp = readbyte(tempaddr);
  SLA(bytetemp);
  writebyte(tempaddr,bytetemp);
}
break;

case 0x27:	/* LD A,SLA (REGISTER+dd) */
tstates += 8;
A=readbyte(tempaddr);
SLA(A);
writebyte(tempaddr, A);
break;

case 0x28:	/* LD B,SRA (REGISTER+dd) */
tstates += 8;
B=readbyte(tempaddr);
SRA(B);
writebyte(tempaddr, B);
break;

case 0x29:	/* LD C,SRA (REGISTER+dd) */
tstates += 8;
C=readbyte(tempaddr);
SRA(C);
writebyte(tempaddr, C);
break;

case 0x2a:	/* LD D,SRA (REGISTER+dd) */
tstates += 8;
D=readbyte(tempaddr);
SRA(D);
writebyte(tempaddr, D);
break;

case 0x2b:	/* LD E,SRA (REGISTER+dd) */
tstates += 8;
E=readbyte(tempaddr);
SRA(E);
writebyte(tempaddr, E);
break;

case 0x2c:	/* LD H,SRA (REGISTER+dd) */
tstates += 8;
H=readbyte(tempaddr);
SRA(H);
writebyte(tempaddr, H);
break;

case 0x2d:	/* LD L,SRA (REGISTER+dd) */
tstates += 8;
L=readbyte(tempaddr);
SRA(L);
writebyte(tempaddr, L);
break;

case 0x2e:	/* SRA (REGISTER+dd) */
tstates += 8;
{
  BYTE bytetemp = readbyte(tempaddr);
  SRA(bytetemp);
  writebyte(tempaddr,bytetemp);
}
break;

case 0x2f:	/* LD A,SRA (REGISTER+dd) */
tstates += 8;
A=readbyte(tempaddr);
SRA(A);
writebyte(tempaddr, A);
break;

case 0x30:	/* LD B,SLL (REGISTER+dd) */
tstates += 8;
B=readbyte(tempaddr);
SLL(B);
writebyte(tempaddr, B);
break;

case 0x31:	/* LD C,SLL (REGISTER+dd) */
tstates += 8;
C=readbyte(tempaddr);
SLL(C);
writebyte(tempaddr, C);
break;

case 0x32:	/* LD D,SLL (REGISTER+dd) */
tstates += 8;
D=readbyte(tempaddr);
SLL(D);
writebyte(tempaddr, D);
break;

case 0x33:	/* LD E,SLL (REGISTER+dd) */
tstates += 8;
E=readbyte(tempaddr);
SLL(E);
writebyte(tempaddr, E);
break;

case 0x34:	/* LD H,SLL (REGISTER+dd) */
tstates += 8;
H=readbyte(tempaddr);
SLL(H);
writebyte(tempaddr, H);
break;

case 0x35:	/* LD L,SLL (REGISTER+dd) */
tstates += 8;
L=readbyte(tempaddr);
SLL(L);
writebyte(tempaddr, L);
break;

case 0x36:	/* SLL (REGISTER+dd) */
tstates += 8;
{
  BYTE bytetemp = readbyte(tempaddr);
  SLL(bytetemp);
  writebyte(tempaddr,bytetemp);
}
break;

case 0x37:	/* LD A,SLL (REGISTER+dd) */
tstates += 8;
A=readbyte(tempaddr);
SLL(A);
writebyte(tempaddr, A);
break;

case 0x38:	/* LD B,SRL (REGISTER+dd) */
tstates += 8;
B=readbyte(tempaddr);
SRL(B);
writebyte(tempaddr, B);
break;

case 0x39:	/* LD C,SRL (REGISTER+dd) */
tstates += 8;
C=readbyte(tempaddr);
SRL(C);
writebyte(tempaddr, C);
break;

case 0x3a:	/* LD D,SRL (REGISTER+dd) */
tstates += 8;
D=readbyte(tempaddr);
SRL(D);
writebyte(tempaddr, D);
break;

case 0x3b:	/* LD E,SRL (REGISTER+dd) */
tstates += 8;
E=readbyte(tempaddr);
SRL(E);
writebyte(tempaddr, E);
break;

case 0x3c:	/* LD H,SRL (REGISTER+dd) */
tstates += 8;
H=readbyte(tempaddr);
SRL(H);
writebyte(tempaddr, H);
break;

case 0x3d:	/* LD L,SRL (REGISTER+dd) */
tstates += 8;
L=readbyte(tempaddr);
SRL(L);
writebyte(tempaddr, L);
break;

case 0x3e:	/* SRL (REGISTER+dd) */
tstates += 8;
{
  BYTE bytetemp = readbyte(tempaddr);
  SRL(bytetemp);
  writebyte(tempaddr,bytetemp);
}
break;

case 0x3f:	/* LD A,SRL (REGISTER+dd) */
tstates += 8;
A=readbyte(tempaddr);
SRL(A);
writebyte(tempaddr, A);
break;

case 0x40:	/* BIT 0,(REGISTER+dd) */
case 0x41:
case 0x42:
case 0x43:
case 0x44:
case 0x45:
case 0x46:
case 0x47:
tstates += 5;
{
  BYTE bytetemp=readbyte(tempaddr);
  BIT(0,bytetemp);
}
break;

case 0x48:	/* BIT 1,(REGISTER+dd) */
case 0x49:
case 0x4a:
case 0x4b:
case 0x4c:
case 0x4d:
case 0x4e:
case 0x4f:
tstates += 5;
{
  BYTE bytetemp=readbyte(tempaddr);
  BIT(1,bytetemp);
}
break;

case 0x50:	/* BIT 2,(REGISTER+dd) */
case 0x51:
case 0x52:
case 0x53:
case 0x54:
case 0x55:
case 0x56:
case 0x57:
tstates += 5;
{
  BYTE bytetemp=readbyte(tempaddr);
  BIT(2,bytetemp);
}
break;

case 0x58:	/* BIT 3,(REGISTER+dd) */
case 0x59:
case 0x5a:
case 0x5b:
case 0x5c:
case 0x5d:
case 0x5e:
case 0x5f:
tstates += 5;
{
  BYTE bytetemp=readbyte(tempaddr);
  BIT(3,bytetemp);
}
break;

case 0x60:	/* BIT 4,(REGISTER+dd) */
case 0x61:
case 0x62:
case 0x63:
case 0x64:
case 0x65:
case 0x66:
case 0x67:
tstates += 5;
{
  BYTE bytetemp=readbyte(tempaddr);
  BIT(4,bytetemp);
}
break;

case 0x68:	/* BIT 5,(REGISTER+dd) */
case 0x69:
case 0x6a:
case 0x6b:
case 0x6c:
case 0x6d:
case 0x6e:
case 0x6f:
tstates += 5;
{
  BYTE bytetemp=readbyte(tempaddr);
  BIT(5,bytetemp);
}
break;

case 0x70:	/* BIT 6,(REGISTER+dd) */
case 0x71:
case 0x72:
case 0x73:
case 0x74:
case 0x75:
case 0x76:
case 0x77:
tstates += 5;
{
  BYTE bytetemp=readbyte(tempaddr);
  BIT(6,bytetemp);
}
break;

case 0x78:	/* BIT 7,(REGISTER+dd) */
case 0x79:
case 0x7a:
case 0x7b:
case 0x7c:
case 0x7d:
case 0x7e:
case 0x7f:
tstates += 5;
{
  BYTE bytetemp=readbyte(tempaddr);
  BIT7(bytetemp);
}
break;

case 0x80:	/* LD B,RES 0,(REGISTER+dd) */
tstates += 8;
B=readbyte(tempaddr) & 0xfe;
writebyte(tempaddr, B);
break;

case 0x81:	/* LD C,RES 0,(REGISTER+dd) */
tstates += 8;
C=readbyte(tempaddr) & 0xfe;
writebyte(tempaddr, C);
break;

case 0x82:	/* LD D,RES 0,(REGISTER+dd) */
tstates += 8;
D=readbyte(tempaddr) & 0xfe;
writebyte(tempaddr, D);
break;

case 0x83:	/* LD E,RES 0,(REGISTER+dd) */
tstates += 8;
E=readbyte(tempaddr) & 0xfe;
writebyte(tempaddr, E);
break;

case 0x84:	/* LD H,RES 0,(REGISTER+dd) */
tstates += 8;
H=readbyte(tempaddr) & 0xfe;
writebyte(tempaddr, H);
break;

case 0x85:	/* LD L,RES 0,(REGISTER+dd) */
tstates += 8;
L=readbyte(tempaddr) & 0xfe;
writebyte(tempaddr, L);
break;

case 0x86:	/* RES 0,(REGISTER+dd) */
tstates += 8;
writebyte(tempaddr, readbyte(tempaddr) & 0xfe);
break;

case 0x87:	/* LD A,RES 0,(REGISTER+dd) */
tstates += 8;
A=readbyte(tempaddr) & 0xfe;
writebyte(tempaddr, A);
break;

case 0x88:	/* LD B,RES 1,(REGISTER+dd) */
tstates += 8;
B=readbyte(tempaddr) & 0xfd;
writebyte(tempaddr, B);
break;

case 0x89:	/* LD C,RES 1,(REGISTER+dd) */
tstates += 8;
C=readbyte(tempaddr) & 0xfd;
writebyte(tempaddr, C);
break;

case 0x8a:	/* LD D,RES 1,(REGISTER+dd) */
tstates += 8;
D=readbyte(tempaddr) & 0xfd;
writebyte(tempaddr, D);
break;

case 0x8b:	/* LD E,RES 1,(REGISTER+dd) */
tstates += 8;
E=readbyte(tempaddr) & 0xfd;
writebyte(tempaddr, E);
break;

case 0x8c:	/* LD H,RES 1,(REGISTER+dd) */
tstates += 8;
H=readbyte(tempaddr) & 0xfd;
writebyte(tempaddr, H);
break;

case 0x8d:	/* LD L,RES 1,(REGISTER+dd) */
tstates += 8;
L=readbyte(tempaddr) & 0xfd;
writebyte(tempaddr, L);
break;

case 0x8e:	/* RES 1,(REGISTER+dd) */
tstates += 8;
writebyte(tempaddr, readbyte(tempaddr) & 0xfd);
break;

case 0x8f:	/* LD A,RES 1,(REGISTER+dd) */
tstates += 8;
A=readbyte(tempaddr) & 0xfd;
writebyte(tempaddr, A);
break;

case 0x90:	/* LD B,RES 2,(REGISTER+dd) */
tstates += 8;
B=readbyte(tempaddr) & 0xfb;
writebyte(tempaddr, B);
break;

case 0x91:	/* LD C,RES 2,(REGISTER+dd) */
tstates += 8;
C=readbyte(tempaddr) & 0xfb;
writebyte(tempaddr, C);
break;

case 0x92:	/* LD D,RES 2,(REGISTER+dd) */
tstates += 8;
D=readbyte(tempaddr) & 0xfb;
writebyte(tempaddr, D);
break;

case 0x93:	/* LD E,RES 2,(REGISTER+dd) */
tstates += 8;
E=readbyte(tempaddr) & 0xfb;
writebyte(tempaddr, E);
break;

case 0x94:	/* LD H,RES 2,(REGISTER+dd) */
tstates += 8;
H=readbyte(tempaddr) & 0xfb;
writebyte(tempaddr, H);
break;

case 0x95:	/* LD L,RES 2,(REGISTER+dd) */
tstates += 8;
L=readbyte(tempaddr) & 0xfb;
writebyte(tempaddr, L);
break;

case 0x96:	/* RES 2,(REGISTER+dd) */
tstates += 8;
writebyte(tempaddr, readbyte(tempaddr) & 0xfb);
break;

case 0x97:	/* LD A,RES 2,(REGISTER+dd) */
tstates += 8;
A=readbyte(tempaddr) & 0xfb;
writebyte(tempaddr, A);
break;

case 0x98:	/* LD B,RES 3,(REGISTER+dd) */
tstates += 8;
B=readbyte(tempaddr) & 0xf7;
writebyte(tempaddr, B);
break;

case 0x99:	/* LD C,RES 3,(REGISTER+dd) */
tstates += 8;
C=readbyte(tempaddr) & 0xf7;
writebyte(tempaddr, C);
break;

case 0x9a:	/* LD D,RES 3,(REGISTER+dd) */
tstates += 8;
D=readbyte(tempaddr) & 0xf7;
writebyte(tempaddr, D);
break;

case 0x9b:	/* LD E,RES 3,(REGISTER+dd) */
tstates += 8;
E=readbyte(tempaddr) & 0xf7;
writebyte(tempaddr, E);
break;

case 0x9c:	/* LD H,RES 3,(REGISTER+dd) */
tstates += 8;
H=readbyte(tempaddr) & 0xf7;
writebyte(tempaddr, H);
break;

case 0x9d:	/* LD L,RES 3,(REGISTER+dd) */
tstates += 8;L
=readbyte(tempaddr) & 0xf7;
writebyte(tempaddr, L);
break;

case 0x9e:	/* RES 3,(REGISTER+dd) */
tstates += 8;
writebyte(tempaddr, readbyte(tempaddr) & 0xf7);
break;

case 0x9f:	/* LD A,RES 3,(REGISTER+dd) */
tstates += 8;
A=readbyte(tempaddr) & 0xf7;
writebyte(tempaddr, A);
break;

case 0xa0:	/* LD B,RES 4,(REGISTER+dd) */
tstates += 8;
B=readbyte(tempaddr) & 0xef;
writebyte(tempaddr, B);
break;

case 0xa1:	/* LD C,RES 4,(REGISTER+dd) */
tstates += 8;
C=readbyte(tempaddr) & 0xef;
writebyte(tempaddr, C);
break;

case 0xa2:	/* LD D,RES 4,(REGISTER+dd) */
tstates += 8;
D=readbyte(tempaddr) & 0xef;
writebyte(tempaddr, D);
break;

case 0xa3:	/* LD E,RES 4,(REGISTER+dd) */
tstates += 8;
E=readbyte(tempaddr) & 0xef;
writebyte(tempaddr, E);
break;

case 0xa4:	/* LD H,RES 4,(REGISTER+dd) */
tstates += 8;
H=readbyte(tempaddr) & 0xef;
writebyte(tempaddr, H);
break;

case 0xa5:	/* LD L,RES 4,(REGISTER+dd) */
tstates += 8;
L=readbyte(tempaddr) & 0xef;
writebyte(tempaddr, L);
break;

case 0xa6:	/* RES 4,(REGISTER+dd) */
tstates += 8;
writebyte(tempaddr, readbyte(tempaddr) & 0xef);
break;

case 0xa7:	/* LD A,RES 4,(REGISTER+dd) */
tstates += 8;
A=readbyte(tempaddr) & 0xef;
writebyte(tempaddr, A);
break;

case 0xa8:	/* LD B,RES 5,(REGISTER+dd) */
tstates += 8;
B=readbyte(tempaddr) & 0xdf;
writebyte(tempaddr, B);
break;

case 0xa9:	/* LD C,RES 5,(REGISTER+dd) */
tstates += 8;
C=readbyte(tempaddr) & 0xdf;
writebyte(tempaddr, C);
break;

case 0xaa:	/* LD D,RES 5,(REGISTER+dd) */
tstates += 8;
D=readbyte(tempaddr) & 0xdf;
writebyte(tempaddr, D);
break;

case 0xab:	/* LD E,RES 5,(REGISTER+dd) */
tstates += 8;
E=readbyte(tempaddr) & 0xdf;
writebyte(tempaddr, E);
break;

case 0xac:	/* LD H,RES 5,(REGISTER+dd) */
tstates += 8;
H=readbyte(tempaddr) & 0xdf;
writebyte(tempaddr, H);
break;

case 0xad:	/* LD L,RES 5,(REGISTER+dd) */
tstates += 8;
L=readbyte(tempaddr) & 0xdf;
writebyte(tempaddr, L);
break;

case 0xae:	/* RES 5,(REGISTER+dd) */
tstates += 8;
writebyte(tempaddr, readbyte(tempaddr) & 0xdf);
break;

case 0xaf:	/* LD A,RES 5,(REGISTER+dd) */
tstates += 8;
A=readbyte(tempaddr) & 0xdf;
writebyte(tempaddr, A);
break;

case 0xb0:	/* LD B,RES 6,(REGISTER+dd) */
tstates += 8;
B=readbyte(tempaddr) & 0xbf;
writebyte(tempaddr, B);
break;

case 0xb1:	/* LD C,RES 6,(REGISTER+dd) */
tstates += 8;
C=readbyte(tempaddr) & 0xbf;
writebyte(tempaddr, C);
break;

case 0xb2:	/* LD D,RES 6,(REGISTER+dd) */
tstates += 8;
D=readbyte(tempaddr) & 0xbf;
writebyte(tempaddr, D);
break;

case 0xb3:	/* LD E,RES 6,(REGISTER+dd) */
tstates += 8;
E=readbyte(tempaddr) & 0xbf;
writebyte(tempaddr, E);
break;

case 0xb4:	/* LD H,RES 6,(REGISTER+dd) */
tstates += 8;
H=readbyte(tempaddr) & 0xbf;
writebyte(tempaddr, H);
break;

case 0xb5:	/* LD L,RES 6,(REGISTER+dd) */
tstates += 8;
L=readbyte(tempaddr) & 0xbf;
writebyte(tempaddr, L);
break;

case 0xb6:	/* RES 6,(REGISTER+dd) */
tstates += 8;
writebyte(tempaddr, readbyte(tempaddr) & 0xbf);
break;

case 0xb7:	/* LD A,RES 6,(REGISTER+dd) */
tstates += 8;
A=readbyte(tempaddr) & 0xbf;
writebyte(tempaddr, A);
break;

case 0xb8:	/* LD B,RES 7,(REGISTER+dd) */
tstates += 8;
B=readbyte(tempaddr) & 0x7f;
writebyte(tempaddr, B);
break;

case 0xb9:	/* LD C,RES 7,(REGISTER+dd) */
tstates += 8;
C=readbyte(tempaddr) & 0x7f;
writebyte(tempaddr, C);
break;

case 0xba:	/* LD D,RES 7,(REGISTER+dd) */
tstates += 8;
D=readbyte(tempaddr) & 0x7f;
writebyte(tempaddr, D);
break;

case 0xbb:	/* LD E,RES 7,(REGISTER+dd) */
tstates += 8;
E=readbyte(tempaddr) & 0x7f;
writebyte(tempaddr, E);
break;

case 0xbc:	/* LD H,RES 7,(REGISTER+dd) */
tstates += 8;
H=readbyte(tempaddr) & 0x7f;
writebyte(tempaddr, H);
break;

case 0xbd:	/* LD L,RES 7,(REGISTER+dd) */
tstates += 8;
L=readbyte(tempaddr) & 0x7f;
writebyte(tempaddr, L);
break;

case 0xbe:	/* RES 7,(REGISTER+dd) */
tstates += 8;
writebyte(tempaddr, readbyte(tempaddr) & 0x7f);
break;

case 0xbf:	/* LD A,RES 7,(REGISTER+dd) */
tstates += 8;
A=readbyte(tempaddr) & 0x7f;
writebyte(tempaddr, A);
break;

case 0xc0:	/* LD B,SET 0,(REGISTER+dd) */
tstates += 8;
B=readbyte(tempaddr) | 0x01;
writebyte(tempaddr, B);
break;

case 0xc1:	/* LD C,SET 0,(REGISTER+dd) */
tstates += 8;
C=readbyte(tempaddr) | 0x01;
writebyte(tempaddr, C);
break;

case 0xc2:	/* LD D,SET 0,(REGISTER+dd) */
tstates += 8;
D=readbyte(tempaddr) | 0x01;
writebyte(tempaddr, D);
break;

case 0xc3:	/* LD E,SET 0,(REGISTER+dd) */
tstates += 8;
E=readbyte(tempaddr) | 0x01;
writebyte(tempaddr, E);
break;

case 0xc4:	/* LD H,SET 0,(REGISTER+dd) */
tstates += 8;
H=readbyte(tempaddr) | 0x01;
writebyte(tempaddr, H);
break;

case 0xc5:	/* LD L,SET 0,(REGISTER+dd) */
tstates += 8;
L=readbyte(tempaddr) | 0x01;
writebyte(tempaddr, L);
break;

case 0xc6:	/* SET 0,(REGISTER+dd) */
tstates += 8;
writebyte(tempaddr, readbyte(tempaddr) | 0x01);
break;

case 0xc7:	/* LD A,SET 0,(REGISTER+dd) */
tstates += 8;
A=readbyte(tempaddr) | 0x01;
writebyte(tempaddr, A);
break;

case 0xc8:	/* LD B,SET 1,(REGISTER+dd) */
tstates += 8;
B=readbyte(tempaddr) | 0x02;
writebyte(tempaddr, B);
break;

case 0xc9:	/* LD C,SET 1,(REGISTER+dd) */
tstates += 8;
C=readbyte(tempaddr) | 0x02;
writebyte(tempaddr, C);
break;

case 0xca:	/* LD D,SET 1,(REGISTER+dd) */
tstates += 8;
D=readbyte(tempaddr) | 0x02;
writebyte(tempaddr, D);
break;

case 0xcb:	/* LD E,SET 1,(REGISTER+dd) */
tstates += 8;
E=readbyte(tempaddr) | 0x02;
writebyte(tempaddr, E);
break;

case 0xcc:	/* LD H,SET 1,(REGISTER+dd) */
tstates += 8;
H=readbyte(tempaddr) | 0x02;
writebyte(tempaddr, H);
break;

case 0xcd:	/* LD L,SET 1,(REGISTER+dd) */
tstates += 8;
L=readbyte(tempaddr) | 0x02;
writebyte(tempaddr, L);
break;

case 0xce:	/* SET 1,(REGISTER+dd) */
tstates += 8;
writebyte(tempaddr, readbyte(tempaddr) | 0x02);
break;

case 0xcf:	/* LD A,SET 1,(REGISTER+dd) */
tstates += 8;
A=readbyte(tempaddr) | 0x02;
writebyte(tempaddr, A);
break;

case 0xd0:	/* LD B,SET 2,(REGISTER+dd) */
tstates += 8;
B=readbyte(tempaddr) | 0x04;
writebyte(tempaddr, B);
break;

case 0xd1:	/* LD C,SET 2,(REGISTER+dd) */
tstates += 8;
C=readbyte(tempaddr) | 0x04;
writebyte(tempaddr, C);
break;

case 0xd2:	/* LD D,SET 2,(REGISTER+dd) */
tstates += 8;
D=readbyte(tempaddr) | 0x04;
writebyte(tempaddr, D);
break;

case 0xd3:	/* LD E,SET 2,(REGISTER+dd) */
tstates += 8;
E=readbyte(tempaddr) | 0x04;
writebyte(tempaddr, E);
break;

case 0xd4:	/* LD H,SET 2,(REGISTER+dd) */
tstates += 8;
H=readbyte(tempaddr) | 0x04;
writebyte(tempaddr, H);
break;

case 0xd5:	/* LD L,SET 2,(REGISTER+dd) */
tstates += 8;
L=readbyte(tempaddr) | 0x04;
writebyte(tempaddr, L);
break;

case 0xd6:	/* SET 2,(REGISTER+dd) */
tstates += 8;
writebyte(tempaddr, readbyte(tempaddr) | 0x04);
break;

case 0xd7:	/* LD A,SET 2,(REGISTER+dd) */
tstates += 8;
A=readbyte(tempaddr) | 0x04;
writebyte(tempaddr, A);
break;

case 0xd8:	/* LD B,SET 3,(REGISTER+dd) */
tstates += 8;
B=readbyte(tempaddr) | 0x08;
writebyte(tempaddr, B);
break;

case 0xd9:	/* LD C,SET 3,(REGISTER+dd) */
tstates += 8;
C=readbyte(tempaddr) | 0x08;
writebyte(tempaddr, C);
break;

case 0xda:	/* LD D,SET 3,(REGISTER+dd) */
tstates += 8;
D=readbyte(tempaddr) | 0x08;
writebyte(tempaddr, D);
break;

case 0xdb:	/* LD E,SET 3,(REGISTER+dd) */
tstates += 8;
E=readbyte(tempaddr) | 0x08;
writebyte(tempaddr, E);
break;

case 0xdc:	/* LD H,SET 3,(REGISTER+dd) */
tstates += 8;
H=readbyte(tempaddr) | 0x08;
writebyte(tempaddr, H);
break;

case 0xdd:	/* LD L,SET 3,(REGISTER+dd) */
tstates += 8;
L=readbyte(tempaddr) | 0x08;
writebyte(tempaddr, L);
break;

case 0xde:	/* SET 3,(REGISTER+dd) */
tstates += 8;
writebyte(tempaddr, readbyte(tempaddr) | 0x08);
break;

case 0xdf:	/* LD A,SET 3,(REGISTER+dd) */
tstates += 8;
A=readbyte(tempaddr) | 0x08;
writebyte(tempaddr, A);
break;

case 0xe0:	/* LD B,SET 4,(REGISTER+dd) */
tstates += 8;
B=readbyte(tempaddr) | 0x10;
writebyte(tempaddr, B);
break;

case 0xe1:	/* LD C,SET 4,(REGISTER+dd) */
tstates += 8;
C=readbyte(tempaddr) | 0x10;
writebyte(tempaddr, C);
break;

case 0xe2:	/* LD D,SET 4,(REGISTER+dd) */
tstates += 8;
D=readbyte(tempaddr) | 0x10;
writebyte(tempaddr, D);
break;

case 0xe3:	/* LD E,SET 4,(REGISTER+dd) */
tstates += 8;
E=readbyte(tempaddr) | 0x10;
writebyte(tempaddr, E);
break;

case 0xe4:	/* LD H,SET 4,(REGISTER+dd) */
tstates += 8;
H=readbyte(tempaddr) | 0x10;
writebyte(tempaddr, H);
break;

case 0xe5:	/* LD L,SET 4,(REGISTER+dd) */
tstates += 8;
L=readbyte(tempaddr) | 0x10;
writebyte(tempaddr, L);
break;

case 0xe6:	/* SET 4,(REGISTER+dd) */
tstates += 8;
writebyte(tempaddr, readbyte(tempaddr) | 0x10);
break;

case 0xe7:	/* LD A,SET 4,(REGISTER+dd) */
tstates += 8;
A=readbyte(tempaddr) | 0x10;
writebyte(tempaddr, A);
break;

case 0xe8:	/* LD B,SET 5,(REGISTER+dd) */
tstates += 8;
B=readbyte(tempaddr) | 0x20;
writebyte(tempaddr, B);
break;

case 0xe9:	/* LD C,SET 5,(REGISTER+dd) */
tstates += 8;
C=readbyte(tempaddr) | 0x20;
writebyte(tempaddr, C);
break;

case 0xea:	/* LD D,SET 5,(REGISTER+dd) */
tstates += 8;
D=readbyte(tempaddr) | 0x20;
writebyte(tempaddr, D);
break;

case 0xeb:	/* LD E,SET 5,(REGISTER+dd) */
tstates += 8;
E=readbyte(tempaddr) | 0x20;
writebyte(tempaddr, E);
break;

case 0xec:	/* LD H,SET 5,(REGISTER+dd) */
tstates += 8;
H=readbyte(tempaddr) | 0x20;
writebyte(tempaddr, H);
break;

case 0xed:	/* LD L,SET 5,(REGISTER+dd) */
tstates += 8;
L=readbyte(tempaddr) | 0x20;
writebyte(tempaddr, L);
break;

case 0xee:	/* SET 5,(REGISTER+dd) */
tstates += 8;
writebyte(tempaddr, readbyte(tempaddr) | 0x20);
break;

case 0xef:	/* LD A,SET 5,(REGISTER+dd) */
tstates += 8;
A=readbyte(tempaddr) | 0x20;
writebyte(tempaddr, A);
break;

case 0xf0:	/* LD B,SET 6,(REGISTER+dd) */
tstates += 8;
B=readbyte(tempaddr) | 0x40;
writebyte(tempaddr, B);
break;

case 0xf1:	/* LD C,SET 6,(REGISTER+dd) */
tstates += 8;
C=readbyte(tempaddr) | 0x40;
writebyte(tempaddr, C);
break;

case 0xf2:	/* LD D,SET 6,(REGISTER+dd) */
tstates += 8;
D=readbyte(tempaddr) | 0x40;
writebyte(tempaddr, D);
break;

case 0xf3:	/* LD E,SET 6,(REGISTER+dd) */
tstates += 8;
E=readbyte(tempaddr) | 0x40;
writebyte(tempaddr, E);
break;

case 0xf4:	/* LD H,SET 6,(REGISTER+dd) */
tstates += 8;
H=readbyte(tempaddr) | 0x40;
writebyte(tempaddr, H);
break;

case 0xf5:	/* LD L,SET 6,(REGISTER+dd) */
tstates += 8;
L=readbyte(tempaddr) | 0x40;
writebyte(tempaddr, L);
break;

case 0xf6:	/* SET 6,(REGISTER+dd) */
tstates += 8;
writebyte(tempaddr, readbyte(tempaddr) | 0x40);
break;

case 0xf7:	/* LD A,SET 6,(REGISTER+dd) */
tstates += 8;
A=readbyte(tempaddr) | 0x40;
writebyte(tempaddr, A);
break;

case 0xf8:	/* LD B,SET 7,(REGISTER+dd) */
tstates += 8;
B=readbyte(tempaddr) | 0x80;
writebyte(tempaddr, B);
break;

case 0xf9:	/* LD C,SET 7,(REGISTER+dd) */
tstates += 8;
C=readbyte(tempaddr) | 0x80;
writebyte(tempaddr, C);
break;

case 0xfa:	/* LD D,SET 7,(REGISTER+dd) */
tstates += 8;
D=readbyte(tempaddr) | 0x80;
writebyte(tempaddr, D);
break;

case 0xfb:	/* LD E,SET 7,(REGISTER+dd) */
tstates += 8;
E=readbyte(tempaddr) | 0x80;
writebyte(tempaddr, E);
break;

case 0xfc:	/* LD H,SET 7,(REGISTER+dd) */
tstates += 8;
H=readbyte(tempaddr) | 0x80;
writebyte(tempaddr, H);
break;

case 0xfd:	/* LD L,SET 7,(REGISTER+dd) */
tstates += 8;
L=readbyte(tempaddr) | 0x80;
writebyte(tempaddr, L);
break;

case 0xfe:	/* SET 7,(REGISTER+dd) */
tstates += 8;
writebyte(tempaddr, readbyte(tempaddr) | 0x80);
break;

case 0xff:	/* LD A,SET 7,(REGISTER+dd) */
tstates += 8;
A=readbyte(tempaddr) | 0x80;
writebyte(tempaddr, A);
break;
