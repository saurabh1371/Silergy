 /***************************************************************************
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * DESCRIPTION: CE init
 *
 * Copyright 2017, Silergy Corp. All rights reserved.
 *
 * $Id: ceM11a02e.c 25019 2020-05-07 23:59:54Z rvandewa $
 ****************************************************************************/

#include <stdlib.h>
#include "options.h"
#include "ce.h"
#include "delay.h"
#include "gpio.h"
#include "Defines.h"

const char fw_ver_ce_code[] =
"ceM11a02f"  /* Device Driver name */
" v"         /* ver string */
"3.00"       /* Major_ver.Minor_ver */
"\r\n"       /* CR + LF */
;


// ce_ram structure place in CEDATA section, with R/W type.
#if __ARMCC_VERSION
ce_data_t ce_data __attribute__((at(CE_RAM_LOCATOR))); 
#elif __GNUC__
ce_data_t __attribute__((section (".ce_data"))) ce_data;
#else
#error undefined CE data area.
// Portable, but the CE data is not visible in the debugger.
//#define ce_data (*(volatile ce_data_t *)CE_RAM_LOCATOR)
#endif

uint32_t ce_ver[CE_NAME_SZ]; // Little endian copy of CE RAM


// in SY7-series flash address is a maximum of 16 bits, 
// CELCTN[2:0] represents upper 3 bits of CE program in flash address.  
const int NumCeCode=1457;	// The number of words in the 'CeCode' array.
const unsigned int CeCode[] =
{
 0xf1f0ecff, 0x205b8f50, 0x74398c5b, 0xedff205f,
 0x8f51743b, 0x8c5fecff, 0x205b8f50, 0x74398c5b,
 0xedff205f, 0x8f51743b, 0x8c5fecff, 0x205b8f50,
 0x74398c5b, 0xedff205f, 0x8f51743b, 0x8c5fecff,
 0x205b8f50, 0x74398c5b, 0xedff205f, 0x8f51743b,
 0x8c5fecff, 0x205b8f50, 0x74398c5b, 0xedff205f,
 0x8f51743b, 0x8c5fecff, 0x205b8f50, 0x74398c5b,
 0xedff205f, 0x8f51743b, 0x8c5f2f60, 0x8f50f05b,
 0xb5502050, 0xf1f1d070, 0x8f70f1f0, 0x00598059,
 0xf039205b, 0x74617461, 0x855a305b, 0x1f60865a,
 0x2f608f50, 0xf05fb550, 0x2050f1f1, 0xd0718f71,
 0xf1f0005d, 0x805df03b, 0x205f7461, 0x7461855e,
 0x305f1f60, 0x865e2082, 0xf1f1847b, 0xf1f02068,
 0x0f608768, 0x20688483, 0xb4682620, 0x8f50f050,
 0x20646866, 0x8f502067, 0xd061f050, 0x2c618f50,
 0x30508150, 0x30508650, 0x20500069, 0x876a8f69,
 0xb769206a, 0x006d8f50, 0x876df06d, 0x206cd06d,
 0x8f513051, 0x86512051, 0x816d2450, 0x006b8f6e,
 0x00bf8f50, 0x2c50afbf, 0x00be8fbe, 0x8482b4be,
 0x2082f1f1, 0x007bf1f0, 0x8f502150, 0xf1f18f7c,
 0xf1f0206e, 0x006a7467, 0x8867f1f1, 0x207c127c,
 0x057c087c, 0xf1f08f50, 0xcd504d50, 0xf1f18f4c,
 0x204c8f4e, 0x8f5bf1f0, 0x2560f1f1, 0x8f4d8f5c,
 0xf1f0f054, 0x2054f1f1, 0x854d854e, 0x208d8fad,
 0xf1f02040, 0xf1f18fae, 0xf1f06300, 0x8f502150,
 0x8f50f1f1, 0xf0cb33cc, 0xf1f08651, 0xf1f123cc,
 0xf1f08551, 0x20501051, 0x8f512051, 0xf1f18fce,
 0x10cdf1f0, 0x8f51f1f1, 0xf0cbf1f0, 0x35518651,
 0x25518551, 0x2051f1f1, 0x00cc8fcc, 0x20ce8fcd,
 0xf0cbf1f0, 0x3f60f1f1, 0x85cbf1f0, 0x2f60f1f1,
 0x86cbf1f0, 0xc010f1f1, 0x40cef1f0, 0x8f50c056,
 0x4050f1f1, 0x8f43f1f0, 0x8f53f1f1, 0x2643f1f0,
 0x003e193e, 0x8f3e2012, 0x8f51c051, 0x403ef1f1,
 0x00438f43, 0xc0865043, 0x0043008e, 0x8f43c086,
 0x40438f8e, 0x2043008f, 0x00910094, 0x8f8dc087,
 0x408d8f8d, 0xc0884043, 0x8f8f2090, 0x8f91c089,
 0x40438f90, 0x20938f94, 0x20928f93, 0xc08a4043,
 0x8f92208d, 0xf1f0103d, 0xf1f18f8d, 0x2b8df1f0,
 0x003d8f3d, 0xf1f1208d, 0x8f43c086, 0x50430043,
 0x00958f43, 0xc0864043, 0x8f952043, 0x00960098,
 0x009bf1f0, 0x8f40f1f1, 0xc087f1f0, 0x40408f40,
 0xf1f1c088, 0x40438f96, 0x20978f98, 0xc0894043,
 0x8f97209a, 0x8f9b2099, 0x8f9ac08a, 0x40438f99,
 0xc08bf1f0, 0x4053f1f1, 0x8fbd20bd, 0x10be8fbd,
 0x2bbd00be, 0x8fbe20b1, 0x8f48209c, 0x8faff1f0,
 0x21038f50, 0x8f532650, 0x00621962, 0x8f622015,
 0x8f51c051, 0x40620050, 0x8f52f1f1, 0x20acf1f0,
 0x8f5020c8, 0xf1f18fac, 0xf1f020c6, 0x8fc820c7,
 0x8fc620c3, 0x8f51c013, 0x40528f52, 0xc0574052,
 0x8fc320c3, 0x10c68f52, 0x34520852, 0x00518fc7,
 0x20c701c6, 0x16c612c8, 0x06c8f1f1, 0x04ac06ac,
 0xf1f01650, 0x8f501250, 0x06501850, 0x8f50c013,
 0x40538f53, 0xc0574053, 0x8f532f20, 0x8f522e52,
 0xf0522050, 0x68538f50, 0x20500050, 0x8f510051,
 0x8f520052, 0x8f530053, 0xf1f18fb0, 0xf1f0f017,
 0xf1f120b0, 0xf1f06850, 0x69516a52, 0x6b538f50,
 0xc0164050, 0xf1f18f43, 0x8fb120b1, 0xf1f0103f,
 0xf1f18fb1, 0x2bb1f1f0, 0x003f8f3f, 0xf1f1c086,
 0x50430043, 0x009d8f43, 0xc0864043, 0x8f9d2043,
 0x009e00a0, 0x00a38f9c, 0xc087409c, 0x8f9cc088,
 0x40438f9e, 0x209f8fa0, 0xc0894043, 0x8f9f20a2,
 0x8fa320a1, 0x8fa2c08a, 0x40438fa1, 0x209c10b2,
 0x8f9c2b9c, 0x00b28fb2, 0xf1f02064, 0xf1f18f47,
 0x20b38fba, 0xf1f02002, 0x8f502150, 0x10658f52,
 0x2b520065, 0x8f65f1f1, 0x20abf1f0, 0x8f5020c2,
 0xf1f18fab, 0xf1f020c0, 0x8fc220c1, 0x8fc020a7,
 0x8f51c011, 0x40528f52, 0xc0554052, 0x8fa720a7,
 0x10c08f52, 0x24520652, 0x19520051, 0x8fc120c1,
 0x12c004c0, 0x16c003c2, 0x16c2f1f1, 0x15abf1f0,
 0x08508f50, 0x03500650, 0x18500a50, 0xf1f18f43,
 0xf1f08f64, 0xf1f1c086, 0x50430043, 0x00a48f43,
 0xc0864043, 0x8fa42043, 0x00a500a7, 0x00aa8fb3,
 0xc08740b3, 0x8fb3c088, 0x40438fa5, 0x20a68fa7,
 0xc0894043, 0x8fa620a9, 0x8faa20a8, 0x8fa9c08a,
 0x40438fa8, 0xc04d4049, 0x004f8f4f, 0xc04e504f,
 0x00491949, 0x0a8d0aad, 0x8f49c05b, 0x4058005d,
 0x8f5dc05c, 0x505d0058, 0x19580a8d, 0x0aad8f58,
 0xc04d40b4, 0x00b58fb5, 0xc04e50b5, 0x00b419b4,
 0xf1f00a40, 0xf1f10aae, 0x8fb4c04d, 0x404a0050,
 0x8f50c04e, 0x5050004a, 0x194a0a9c, 0x0aaf8f4a,
 0xc05b4059, 0x005e8f5e, 0xc05c505e, 0x00591959,
 0x0a9c0aaf, 0x8f59c04d, 0x40b600b7, 0x8fb7c04e,
 0x50b700b6, 0x19b60ab1, 0x0a488fb6, 0xc04d404b,
 0x00518f51, 0xc04e5051, 0x004b194b, 0x0ab30aba,
 0x8f4bc05b, 0x405a005f, 0x8f5fc05c, 0x505f005a,
 0x195a0ab3, 0x0aba8f5a, 0xc04d40b8, 0x00b98fb9,
 0xc04e50b9, 0x00b819b8, 0xf1f00a64, 0xf1f10a47,
 0x8fb822b3, 0x107d107d, 0x007e007e, 0xf1f08f50,
 0xf1f1107e, 0x8f7ef1f0, 0xc0224d82, 0x8f51c051,
 0x40508f36, 0x2e36f1f1, 0x007f8f7f, 0x8b7dbb7f,
 0xf1f02264, 0xf1f11080, 0x10800081, 0x0081f1f0,
 0x8f50f1f1, 0x10818f81, 0xf1f0c022, 0x4d828f51,
 0xc0514050, 0x8f372e37, 0xf1f10082, 0x8f828b80,
 0xbb82225a, 0x10831083, 0x00840084, 0x8f431084,
 0x8f84f1f0, 0xc0224d82, 0xf1f18f44, 0xc0444043,
 0x8fbb2ebb, 0x00858f85, 0x8b83bb85, 0xf1f0bf52,
 0xf0672c60, 0x00528552, 0x28208f50, 0x2c50af50,
 0x2064d024, 0xb0382038, 0x0f608f38, 0xd0502052,
 0x0f608052, 0x20528f80, 0x2180af51, 0x8f502150,
 0xaf528f50, 0x2150af53, 0x26208f50, 0xf0502051,
 0x68526953, 0x8f502f22, 0x1050f1f1, 0x8f72f1f0,
 0x2f208f52, 0x25528f52, 0xf0522f22, 0xf1f16872,
 0xefff8c72, 0xf1f02380, 0xd3c53f60, 0x8f50be50,
 0x2f226950, 0xeeff8c50, 0xf1f1ceb3, 0x448df1f0,
 0x00aa17aa, 0x8faace64, 0xf1f144b1, 0xf1f000ab,
 0x17ab8fab, 0xce36f1f1, 0x448df1f0, 0x00ac17ac,
 0x85acce37, 0xf1f144b1, 0xf1f000ad, 0x17ad85ad,
 0x2f208f52, 0x2e528f52, 0xf0522040, 0xf1f168bd,
 0xf1f08f53, 0x2f53af50, 0x20531050, 0x8f51cf51,
 0x40518f52, 0x20510150, 0x8f51c050, 0x40510070,
 0x00288f50, 0x2e50af70, 0x0052106f, 0x00718f50,
 0x2750af71, 0x006f8f6f, 0xf1f12fb1, 0xaf4320b1,
 0x10438f44, 0xcf444044, 0x8f452044, 0x01438f44,
 0xc0434044, 0xf1f00073, 0x002c8f50, 0x2e50af73,
 0xf1f10045, 0xf1f01072, 0x00748f50, 0x2750af74,
 0x00728f72, 0xce644364, 0x00a817a8, 0x8fa8f1f1,
 0xce4b4449, 0x00521752, 0x8f52ceb8, 0x44b60053,
 0x17538f53, 0x2fb4af43, 0x20b41043, 0x8f44cf44,
 0x40448f45, 0x20440143, 0x8f44c043, 0x40440061,
 0xf1f00028, 0xf1f18f43, 0x2e43af61, 0x00451060,
 0x00628f43, 0x2743af62, 0x00608f60, 0x2fb6af43,
 0x20b61043, 0x8f44cf44, 0x40448f45, 0x20440143,
 0x8f44c043, 0x40440064, 0xf1f0002c, 0xf1f18f43,
 0x2e43af64, 0x00451063, 0x00658f43, 0x2743af65,
 0x00638f63, 0xceb843b8, 0x00661766, 0x8f66cebb,
 0x448d0075, 0x17758f75, 0xcebc449c, 0x00781778,
 0x8f78f1f0, 0x20630f60, 0x8f638423, 0xb46360aa,
 0x702900af, 0x8f502e50, 0xafaf00ae, 0x8fae8485,
 0xb4ae60ac, 0x702a00b3, 0x8f502e50, 0xafb300b2,
 0x8fb28488, 0xb4b220ab, 0x002d00b1, 0x8f502e50,
 0xafb100b0, 0x8fb08486, 0xb4b020ad, 0x002e00b5,
 0x8f502e50, 0xafb500b4, 0x8fb48489, 0xb4b42f20,
 0x8f522e52, 0x8f52f052, 0x2063d026, 0x206f00b7,
 0x8f502b50, 0xafb700b6, 0x8fb6f1f1, 0x84bff1f0,
 0xb4b620b7, 0xf1f184c0, 0xf1f0206f, 0xf1f100c3,
 0x8f432b43, 0xafc300c4, 0x8fc4c08c, 0x40c48f44,
 0x63448f44, 0x21448f44, 0x20c46844, 0x8ec5bec4,
 0x20c38ec6, 0x20bf68c5, 0xf1f08f8a, 0xf1f120c0,
 0x68c6f1f0, 0x8f8b2072, 0x00b98f50, 0x2b50afb9,
 0x00b88fb8, 0xf1f184c1, 0xf1f0b4b8, 0x20b9f1f1,
 0x84c2f1f0, 0x2072f1f1, 0x00c78f43, 0x2b43afc7,
 0x00c88fc8, 0xc08c40c8, 0x8f446344, 0x8f442144,
 0x8f4420c8, 0x68448ec9, 0xbec820c7, 0x8eca20c1,
 0x68c9f1f0, 0x8f8cf1f1, 0x20c268ca, 0xf1f08f8d,
 0x20a800bb, 0x8f502e50, 0xafbb00ba, 0x8fba848e,
 0xb4baf1f1, 0x6052f1f0, 0x7029f1f1, 0x00548f43,
 0x2e43af54, 0x00568f56, 0xf1f08493, 0xf1f1b456,
 0x2053f1f0, 0x002df1f1, 0x00558f43, 0x2e43af55,
 0x00578f57, 0xf1f08494, 0xf1f1b457, 0x20600069,
 0x8f432b43, 0xaf690068, 0x8f68f1f0, 0x8495f1f1,
 0xb4682069, 0xf1f08496, 0xf1f12063, 0x006b8f43,
 0x2b43af6b, 0x006a8f6a, 0xf1f08497, 0xf1f1b46a,
 0x206bf1f0, 0x8498f1f1, 0x2066006d, 0x8f432e43,
 0xaf6d006c, 0x8f6cf1f0, 0x8499f1f1, 0xb46c2067,
 0x006f8f43, 0x2e43af6f, 0x006e8f6e, 0xf1f0849a,
 0xf1f1b46e, 0x6075f1f0, 0x702af1f1, 0x00768f43,
 0x2e43af76, 0x00778f77, 0xf1f084a5, 0xf1f1b477,
 0x2078f1f0, 0x002ef1f1, 0x00798f43, 0x2e43af79,
 0x007a8f7a, 0xf1f084a6, 0xf1f1b47a, 0xf1f02085,
 0x8f50d025, 0xb1502086, 0x8f51d025, 0xb1512050,
 0x00518f53, 0xf0202085, 0x00866b53, 0xf1f18fb0,
 0xf1f02085, 0x6b508f50, 0x30508f51, 0x20507151,
 0xf1f171b0, 0xf1f08484, 0x20888f50, 0xd025b150,
 0x20898f51, 0xd025b151, 0x20500051, 0x8f532088,
 0x00896b53, 0xf1f18fb0, 0xf1f02088, 0x6b508f50,
 0x30508f51, 0x20507151, 0xf1f171b0, 0xf1f08487,
 0x20938f50, 0xd025b150, 0x20948f51, 0xd025b151,
 0x20500051, 0x8f53f020, 0x20930094, 0x6b53f1f1,
 0x8fb0f1f0, 0x20936b50, 0x8f503050, 0x8f512050,
 0x7151f1f1, 0x71b0f1f0, 0x849220a5, 0x8f50d025,
 0xb15020a6, 0x8f51d025, 0xb1512050, 0x00518f53,
 0xf1f18fb0, 0xf1f020a5, 0x6b508f50, 0x30508f51,
 0x20507151, 0xf1f171b0, 0xf1f084a4, 0x25208f51,
 0xf0512084, 0x68588450, 0x2087685c, 0x8451f020,
 0x20506350, 0x6d508f50, 0x20516351, 0x6d518f51,
 0xc0214050, 0x8439003a, 0x8f502650, 0xa43a6c39,
 0x8439c021, 0x4051843b, 0x003c8f51, 0x2651a43c,
 0x6c3b843b, 0x20fdf1f1, 0x00748f43, 0x2d43af74,
 0x00738f73, 0x0d73f1f0, 0x8491f1f1, 0xb473f1f0,
 0x20911018, 0x8f50c019, 0x48508fc4, 0x28c48481,
 0xc01b40c4, 0x001a8f51, 0xc05140c4, 0x08610860,
 0x8f512851, 0x8f512f22, 0xeeff8c50, 0x20fc8490,
 0xffffffff
};

int NumCeData=465;	// The number of words in the 'CeData' array.
const unsigned int CeData[]=
{
 0x00000000, 0x00000000, 0x00000000, 0x00000000,
 0x00000000, 0xffffffff, 0xffffffff, 0xffffffff,
 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
 0x00004000, 0x00004000, 0x00000000, 0x00004000,
 0x00004000, 0x00000000, 0x00004000, 0x00000001,
 0x00000000, 0x0000339d, 0x00000000, 0x00000000,
 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
 0x00316c29, 0x0000286a, 0x0000192d, 0x00001000,
 0x01a48ffa, 0x000004dd, 0x00000143, 0x00000000,
 0x00000000, 0x00000000, 0x00000000, 0x00000000,
 0x00000000, 0x00000000, 0x00000000, 0x00000000,
 0x00000000, 0x00000000, 0x00000000, 0x00000000,
 0x00000000, 0x00000000, 0x00000000, 0x00000000,
 0x00000000, 0x00000000, 0x00000000, 0x00000000,
 0x00000000, 0x00000000, 0x00000000, 0x00000000,
 0x00000000, 0xffffffff, 0xffffffff, 0xffffffff,
 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
 0x43454d31, 0x31613032, 0x66000000, 0x00000000,
 0xffffffff, 0x00004000, 0x00004000, 0x00004000,
 0x00000000, 0x00000000, 0x00000000, 0x40000000,
 0x00000000, 0x00000000, 0x00000000, 0x40000000,
 0x00008000, 0x40000000, 0x00000000, 0x00000000,
 0x00000000, 0x00000000, 0x00000000, 0x00000000,
 0x00000000, 0x00000000, 0x00000000, 0x03700000,
 0x09ffffff, 0x00000000, 0x00000000, 0x00000000,
 0x00000000, 0x00000000, 0x00000000, 0x00000000,
 0x00000000, 0xffffffff, 0xffffffff, 0xffffffff,
 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
 0x00000000, 0x00000000, 0x00000000, 0x00000000,
 0x00000000, 0x00000000, 0x00000000, 0x00000000,
 0x00000000, 0x00000000, 0x00000000, 0x00000000,
 0x00000000, 0x00000000, 0x00000000, 0x00000000,
 0x00000000, 0x00000000, 0x00000000, 0x00000000,
 0x00000000, 0x00000000, 0x00000000, 0x00000000,
 0x00000000, 0x00000000, 0x00000000, 0x00000000,
 0x00000000, 0x00000000, 0x00000000, 0x00000000,
 0x00000000, 0x00000000, 0x00000000, 0x00000000,
 0x00000000, 0x00000000, 0x00000000, 0x00000000,
 0x00000000, 0x00000000, 0x00000000, 0x00000000,
 0x00000000, 0x00000000, 0x00000000, 0x00000000,
 0x00000000, 0x00000000, 0x00000000, 0x00000000,
 0x00000000, 0x00000000, 0x00000000, 0x00000000,
 0x00000000, 0x00000000, 0x00000000, 0x00000000,
 0x00000000, 0x00000000, 0x00000000, 0x00000000,
 0x00000000, 0x00000000, 0x00000000, 0x00000000,
 0x00000000, 0x00000000, 0x00000000, 0x00000000,
 0x00000000, 0xffffffff, 0xffffffff, 0xffffffff,
 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
 0x00000000, 0xffffffff, 0xffffffff, 0xffffffff,
 0x00000000, 0xffffffff, 0xffffffff, 0xffffffff,
 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
 0xffffffff, 0xffffffff, 0xffffffff, 0x00000000,
 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
 0xffffffff, 0x00008000, 0x40000000, 0x00000000,
 0x00000000, 0x00000000, 0x00000000, 0x00000000,
 0x00000000, 0x00000000, 0x00000000, 0x00000000,
 0x00000000, 0x00000000, 0x00000000, 0x00000000,
 0x00000000, 0x00000000, 0x00000000, 0x00000000,
 0x00000000, 0x00000000, 0x00000000, 0x00000000,
 0x00000000, 0x00000000, 0x00000000, 0x00000000,
 0x00000000, 0x00000000, 0x00000000, 0x00000000,
 0x00000000, 0x00000000, 0x00000000, 0x00000000,
 0x00000000, 0x00000000, 0x00000000, 0x00000000,
 0x00000000, 0x00000000, 0x00000000, 0x00000000,
 0x00000000, 0x00000000, 0x00000000, 0x00000000,
 0x00000000, 0x00000000, 0x00000000, 0x00000000,
 0x00000000, 0x00000000, 0x00000000, 0x00000000,
 0x00000000, 0x00000000, 0x00000000, 0x00000000,
 0x00000000, 0x00000000, 0x00000000, 0x00000000,
 0x00000000, 0x00000000, 0x00000000, 0x00000000,
 0x00000000, 0x00000000, 0xffffdea2, 0x00003414,
 0x000010a5, 0xfffffdab, 0x00000057, 0x000045e0,
 0x00006523, 0x00000000, 0x00000000, 0x00000000,
 0x00000000, 0x00000000, 0x00000000, 0x00000000,
 0x00000000, 0x00000000, 0x00000000, 0x00000000,
 0x00000000, 0x00000000, 0x00000000, 0x00000000,
 0x00000000, 0x00000000, 0x00000000, 0x00000000,
 0x00000000, 0x00000000, 0x00000000, 0x00000000,
 0x00000000, 0x00000000, 0x00000000, 0x00000000,
 0x00000000, 0x00000000, 0x00000000, 0x00000000,
 0x00000000, 0x00000000, 0x00000000, 0x00000000,
 0x00000000, 0x00000000, 0x00000000, 0x00000000,
 0x00000000, 0x00000000, 0x00000000, 0x00000000,
 0x00000000, 0x00000000, 0x00000000, 0x00000000,
 0x00000000, 0x00000000, 0x00000000, 0x00000000,
 0x00000000, 0x00000000, 0x00000000, 0x00000000,
 0x00000000, 0x00000000, 0x00000000, 0x00000000,
 0x00000000, 0x00000000, 0x00000000, 0x00000000,
 0x00000000, 0x00000000, 0x00000000, 0x00000000,
 0x00000000
};


// initialize CE
void cem11a02f_init(void)
{
    // Shut down the analog front-end, in case it was up.
    //  Initialize CE , ADC registers :
    CE->CNTL_b.ce_en = 0; // Disable CE
    // Add code for a delay for 2 sample times here.
    delay(DELAY_US(488)); // 2/4096 S 
    ADC->CNTL = 0;  // Set ADC clocks off.
	
	  if(NM_CT_Detected==1)//RAJIV
	  ADC->CNTL_b.adc_vls  = 1; // Update adc_vls //RAJIV
	
    // Add code for a delay for 1 sample time here. 
    delay(DELAY_US(245)); // 1/4096 S 
    CE->INT_CNTL = 0;  // Disable and clear all CE interrupts.

    // Set up the CE.
    CE->PLS_CNTL_b.pls_interval = 0x68; // Set pulse interval 
		
		if(NM_CT_Detected==0)//RAJIV
		{
			CE->CNTL_b.sum_samps = 0x1000; // Set the sum samples==>VLS=0 //RAJIV
			CE->PLS_CNTL_b.pls_wid_max = 0x66; // Set the maximum pulse width //RAJIV
		}
		else //RAJIV
		{
			CE->CNTL_b.sum_samps = 0x144; // Set the sum samples==>VLS=1 //RAJIV
			CE->PLS_CNTL_b.pls_wid_max = 0x13; // Set the maximum pulse width //RAJIV
		}
	
    CE->CNTL_b.chop_en = 3; // Enable default chop mode
    CE->CNTL_b.fchop_en = 0; // Enable fast chop 
    CE->CNTL_b.prechop_en = 1; // Enable preamp's chop 
    CE->CNTL_b.bgchop_en = 1; // Enable bandgap's chop 
    CE->CNTL_b.equ = EQUATION; // Set the metering equation (for CE code) 
    CE->CNTL_b.invert_pls = 0; // Do not invert pulses (0: high = off) 

    // Set up the ADCs. 
    ADC->VREF_CNTL_b.adcs_en = 1; // ADCS (Staring) clock enable        
    ADC->VREF_CNTL_b.adcm_en = 1; // ADCM (Multiplexed) clock enable      
    ADC->VREF_CNTL_b.pre_en = 1; // Preamp enabled. 
    ADC->VREF_CNTL_b.fir_str = 0; // Enable fir-length stretch for slot 0. 
    ADC->VREF_CNTL_b.vref_en = 1; // Enable the voltage reference.
    ADC->CNTL_b.adcs_bias = 0x03; // Set the bias voltage of the staring ADC(s) 
    ADC->CNTL_b.adcm_bias = 0x05; // Set the bias voltage of the multiplexed ADC 
    ADC->CNTL_b.low_bias = 0; // Set the bias range 
    ADC->SLOT_CNTL_b.slot0_sel = 0x01; // The ADC1 input for slot 0. 
    ADC->SLOT_CNTL_b.slot1_sel = 0x02; // The ADC1 input for slot 1. 
    ADC->FIR_LEN_b.fir0_len = 0xB; // The FIR length for slot 0. 
    ADC->FIR_LEN_b.fir1_len = 0xB; // The FIR length for slot 1. 
    RMT->CNTL = 0; // No remote for this CE

    // Load the CE's data and code. 
    ce_load_code(&CeCode[0]); // Also sets up pulse outputs.
    ce_load_ram(&CeData[0], NumCeData);

		

    // Permit external temperature compensation, external pulse.
    ce_data.ceconfig |= CE_EXT_TEMP | EXT_PULSE;
    // Disable zero-crossing interrupt, internal creep. 
    ce_data.ceconfig &= ~(EDGE_INT | INT_CREEP);

		if(NM_CT_Detected==1)//RAJIV
		ce_data.ceconfig |=BIT29;//==>VLS=1//RAJIV
    // Enable needed CE interrupts; Occurs elsewhere in code. 
}

void cem11a02f_enable(void)
{
    CE->CNTL_b.ce_en = 1; // Enable CE (The CE does not run until the ADC clock starts) 	
    ADC->CNTL_b.adcs_spd = 1; // Set staring ADC clock speed 
    ADC->CNTL_b.adcm_spd = 1; // Set multiplexed ADC clock speed
    // Start the analog front end: 
    ADC->CNTL_b.mux_div = 2; // Set the number of ADC slots per mux. frame 
    // Note that the vls bit affects the CPU system clock, including the peripheral clocks. 
    // Peripherals should be set-up using the final clock rate.
}

// CE code and LSBs (i.e. the constants used to convert CE data to 
// useful units.), and other items needed by the MPU code.
const ce_t cem11a02f=
{
    // Code
    cem11a02f_init,
    cem11a02f_enable, 
    // LSBs
    4096,               // CE_FS, from CE's Ref. Manual's Table
    4096,               // CE_SUM_SAMP, from table
    1.09435E-05,        // CE_CVSQ, from table
    1.05753E-05,        // CE_CISQ, from table
    4.37742E-05,        // CE_CINSQ, from table
    1.07592E-05,        // CE_CP, from table
    6.03097E-09,        // CE_CV, from table
    6.56692E-10,        // CE_QUANT_W_LSB, from table
    3.12525E-13,        // CE_QUANT_I_LSB, from table
    6.67941E-10,        // CE_QUANT_V_LSB, from table
    // Other calculation quantities for the CE, from the reference manual.
    (1.0/2048.0),       // CE_H_PER_L, from insqres_x equation
    7.03687e13,         // CE_WRATE_POWER_OF_TWO, from WRATE equation
    64,                 // CE_PULSE_SLOW_FACTOR, 2^6, from WRATE equation
    16,                 // CE_PULSE_FAST_FACTOR, 2^4, from WRATE equ.
    4096,               // CE_NACC, Typical, assumed by CE ref. manual.
    6.0,                // CE_X, from WRATE equation
    2,                  // CE_PLL_CNT, 1st accum. interval with stable data
    1,                  // I1_SCALED_BY_CE, i.e. CE adjusts units for I1
    0x4000,             // UNITY_GAIN, needed by autocal to scale gains.
    1,                  // Does VAR0SUM exist?
    1,                  // Does VAR1SUM exist?
    (const int32_t *)CeCode, // Locate the CE's code and data.
    &NumCeCode,
    (const int32_t *)CeData,
    &NumCeData,
    (0x50),             // CE_NAME_IDX, index of the CE's name
    2.96696e-9          // creep_thr_lsb
};


/***************************************************************************   
* Description:  
*       ce_load_ram().
* Parameters:
*   Input:
*         Starting address and length of starting CE data.
*   Output: 
*         Clears and loads CE RAM
* Return Status: 
*         None
***************************************************************************/
void ce_load_ram(const unsigned int * ce_data_ptr, int len_in)
{
    int len;
    int index = 0; 
    uint32_t *ce_ram_ptr;        // Destination ptr

    // Clear CE RAM Data
    //len = 256;	// 1 KB CE RAM
    //ce_ram_ptr = (uint32_t *) CE_RAM; // Destination pointer
    //while(len > 0)
    //{
    //    *ce_ram_ptr++ = 0x00 ;
    //    --len;
    //}

    // Move CE Data image from flash to start of CERAM. 
    // CE data ends at higher address.
    len = len_in; // The number of words in the 'CeData' array

    ce_ram_ptr = (uint32_t *) CE_RAM; // Destination pointer
    while(len > 0)
    {
        *ce_ram_ptr++ =  *ce_data_ptr++;
        --len;
    }

    ce_ram_ptr = (uint32_t *) CE_RAM; // CE RAM base
    while(index < CE_NAME_SZ)
    { // Make copy of CE name, CE will use CE_RAM & overwrite it when it starts 
        #if __ARMCC_VERSION
        // CE name is in big-endian, change it to little-endian with __rev()
        ce_ver[index++] = __rev(*(ce_ram_ptr + ce_ptr->name_idx));
        #elif __GNUC__
        // CE name is in big-endian, change it to little-endian with 
        // GCC's intrinsic
        ce_ver[index++] = __builtin_bswap32(*(ce_ram_ptr + ce_ptr->name_idx));
        #else
        #error undefined byte-swap intrinsic
        #endif
        ce_ram_ptr++;
    }
}


/***************************************************************************   
* Description:  
*       Returns string pointer to the CE name.
* Parameters:
*   Input:
*         None.
*   Output: 
*         Pointer to the CE's file name, a zero-terminated C string
*         in the static CE data image kept in code space to initialize the CE.
* Return Status: 
*         None
***************************************************************************/
const char *ce_name(void)
{
    return (const char *)ce_ver;
} // ce_name


/***************************************************************************   
* Description:  
*       Copies the default CE calibration data from the CE image (in flash) 
*       to a data buffer (in RAM).
*
* Parameters:
*   Input:
*       *dest_ptr - Address of a data buffer to hold default CE calibration data.
*       *src_ptr - Pointer to the CE default calibration data (in flash).
*       length - The length is a count in 32-bit words (Calibration Data).
*   Output: 
*         The CE's default calibration is copied to the data buffer.
* Return Status: 
*         None
***************************************************************************/
void ce_get_cal_default(int32_t *dest_ptr, const int32_t *src_ptr, 
        int32_t length)
{
    // Be sure not to overwrite the destination data or get garbage.
    length = lmin(length,CE_DEFAULT_CNT);

    while(length > 0) 
    {
        *dest_ptr++ =  *src_ptr++;
        --length;
    }
} // ce_get_cal_default


/***************************************************************************   
* Description:  
*       Copies the CE's active calibration data (in CERAM) to data buffer.
*       Note that if the CE is not running, the data might not be valid.
*
* Parameters:
*   Input:
*       *dest_ptr - Address of a data buffer to hold default CE calibration data.
*       (CE_PARM_BASE  is the src_ptr pointer to the CE calibration data in CERAM).
*       length - The length is a count in 32-bit words (Calibration Data).
*   Output: 
*         The CE's current calibration is copied to the address.
* Return Status: 
*         None
***************************************************************************/
void ce_get_cal(int32_t *dest_ptr, int32_t length)
{
    int32_t *src_ptr = CE_PARM_BASE; // CE Parameter Base address(in CE RAM) 
	
    // Be sure not to overwrite the destination data or get garbage.
    length = lmin(length,CE_DEFAULT_CNT);

    // Get the CE data from the CE RAM.
    for(; 0 < length; --length) 
    {
        *dest_ptr++ = *src_ptr++; // copy to Data RAM to CE RAM
    }
} // ce_get_cal


/***************************************************************************   
* Description:  
*       Copies calibration data to the CE's active calibration data(in CERAM).
*       Note that if the CE is not running, the data might not be right.
*
* Parameters:
*   Input:
*       *src_ptr - Address of a data buffer that holds CE calibration data.
        (CE_PARM_BASE  is the dst_ptr pointer to the CE calibration data in CERAM).
*       length - The length is a count in 32-bit words (Calibration Data).
*   Output: 
*         The source is copied to the CE's current calibration.
* Return Status: 
*         None
***************************************************************************/
void ce_put_cal(int32_t *src_ptr, int32_t length)
{
    int32_t *dest_ptr = CE_PARM_BASE; // CE Parameter Base address(in CE RAM) 
	
    // Be sure not to overwrite the destination data or get garbage.
    length = lmin(length, CE_DEFAULT_CNT);

    // Put the new calibration data into the CE RAM.
    for(; 0 < length; --length) /* copy to CE RAM from Data RAM*/
    {
        *dest_ptr++ = *src_ptr++;
    }
} // ce_put_cal


/***************************************************************************
 * Copyright (C) 2017 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp. 
 ***************************************************************************/
 
