#ifndef REGISTERS_H
#define REGISTERS_H

#include <stdint.h>
#include <cstdint>

enum RegisterFlags {
  FLAGS_ZERO = (1 << 7),
  FLAGS_NEGATIVE = (1 << 6),
  FLAGS_HALFCARRY = (1 << 5),
  FLAGS_CARRY = (1 << 4)
};

class Registers {
  public:
    struct {
      union {
        struct {
          unsigned char f;
          unsigned char a;
        };
        unsigned short fa;
      };
    };

    	struct {
		union {
			struct {
				unsigned char c;
				unsigned char b;
			};
			unsigned short bc;
		};
	};
	
	struct {
		union {
			struct {
				unsigned char e;
				unsigned char d;
			};
			unsigned short de;
		};
	};
	
	struct {
		union {
			struct {
				unsigned char l;
				unsigned char h;
			};
			unsigned short hl;
		};
	};
	
	unsigned short sp;
	unsigned short pc;

  bool IsFlagSet(uint8_t flag);
  void SetFlag(uint8_t flag, bool state);
};

#endif