inline void CSH2::ADD(Sh2& s)
{
	// Rm + Rn -> Rn
	s.r_[xn00(s)] += s.r_[x0n0(s)];

	// Next instruction
	s.pc_ +=2;
	s.cycles_elapsed_ = 1;
}

void CSH2::Initialize()
{
	// Power on reset
	PowerOnReset();
	// Remaining registers are set to 0
	GBR = 0x0;
	MACH = 0x0;
	MACL = 0x0;
	PR = 0x0;
	SR = 0xF0;
	for (s32 i = 0; i<15; i++){
		R[i] = 0x0;
	}
}

inline void CSH2::ADDI(Sh2& s)
{
	// Rn + imm -> Rn 
	if (( x0nn(s)&0x80)==0)
		// #imm positive, 32bits sign extension
		s.r_[xn00(s)] += (0x000000FF & (s32)x0nn(s)); // positive
	else
		// #imm negative, 32bits sign extension
		s.r_[xn00(s)] += (0xFFFFFF00 | (s32)x0nn(s));


	s.pc_ +=2;

	s.cycles_elapsed_ = 1;
}

inline void CSH2::ADDC(Sh2& s)
{
	// Rn + Rm + T -> Rn, carry -> T

	s32 tmp0,tmp1;

	tmp1 = s.r_[xn00(s)]+s.r_[x0n0(s)];
	tmp0 = s.r_[xn00(s)];
	s.r_[xn00(s)] = tmp1 + s.sr_.get(StatusRegister::t);
	
	s32 test = (tmp0>=tmp1);
	if ((tmp0>tmp1))
		Set_T(sh2);
	else
		Clear_T(sh2);

	if (tmp1>s.r_[xn00(s)])
		Set_T(sh2);
	s.pc_ +=2;
	s.cycles_elapsed_ = 1;

}


inline uint8_t CSH2::T_Val(Sh2& s)
{
	// Returns SR T bit value
	return static_cast<uint8_t>(sh2->SR  & 0x1);
}

inline void CSH2::Set_T(Sh2& s)
{
	// SR T bit update
	sh2->SR |= 1;
}

inline void CSH2::Clear_T(Sh2& s)
{
	// T bit clear
	sh2->SR &= 0xFFFE;
}

s32 CSH2::DisplayR(uint8_t n)
{
	// Returns R register value
	return R[n];
}

inline void CSH2::ADDV(Sh2& s)
{
	// Rn + Rm -> Rn, overflow -> T
	s32 dest,src,ans;

	if (s.r_[xn00(s)]>=0) dest = 0;
	else dest = 1;
	
	if (s.r_[x0n0(s)]>=0) src = 0;
	else src = 1;

	src+=dest;
	s.r_[xn00(s)]+=s.r_[x0n0(s)];
	if (s.r_[xn00(s)]>=0) ans = 0;
	else ans = 1;

	ans+=dest;
	if (src==0 || src==2){
		if (ans==1) Set_T(sh2);
		else Clear_T(sh2);
	}
	else Clear_T(sh2);

	s.pc_+=2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::AND(Sh2& s)
{
	// Rn & Rm -> Rn
	s.r_[xn00(s)]&=s.r_[x0n0(s)];
	s.pc_+=2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::ANDI(Sh2& s)

{
	// R0 & imm -> R0
	s.r_[0]&=(0x000000FF & x0nn(s));
	s.pc_+=2;
	s.cycles_elapsed_ = 1;
}

void __fastcall CSH2::Delay_Slot(Sh2& s, const uint32_t & Addr)
{
	//Algorithm :
	// Addr read and intruction fetch
	// if the instruction is BF,BT,BRA,BSR,JMP,JSR,RTS,RTE,TRAPA,BF/S,BT/S,BRAF or BSRF then
	//		-> illegal instruction slot
	// else
	//		Slot instruction execution
	// end

	uint32_t currentInstCount = s.cycles_elapsed_; // We musn't forget the DS instruction count
	if(Addr!=0x20000202){ // Delay slot isn't detected after the Power On Reset (to prevent the "illegal instruction slot")

		sh2->currentOpcode = EmuState::pMem->ReadWord(Addr);
		//format opcodeFmt("PC=0x%X, opcode=0x%X (DS)");
		//opcodeFmt % s.pc_ % sh2->currentOpcode;
		//sh2->lastOpcodes_.push_front(opcodeFmt.str());
		//sh2->lastOpcodes_.push_front(opcode);
		//EmuState::pLog->Sh2Opcodes(opcodeFmt.str());
		
		if (sh2->IllegalSlotInstructionDetected(sh2->currentOpcode)) MessageBox(NULL,"Illegal instruction slot","Error",NULL);
		else {
			if (sh2->GetExecuteInstruction())
			{
				sh2->SetDelaySlotInstruction(true);
				while(sh2->GetDelaySlotInstruction())
				{
					Sleep(1); // Waiting until the delay slot is done
				}
				SendMessage(sh2->GetWindowHandle(),WM_UPDATE,Addr,0);		
			}

			// Delay slot instruction execution
			sh2->Execute(sh2->currentOpcode);

			s.cycles_elapsed_ += currentInstCount;
		}
	}
     
}

inline void CSH2::ANDM(Sh2& s)
{
	//(R0 + GBR) & imm -> (R0 + GBR)

	s32 temp;

	temp = EmuState::pMem->ReadByte(sh2->GBR+s.r_[0]);
	temp &= (0x000000FF & static_cast<s32>(x0nn(s)));
	EmuState::pMem->WriteByte(sh2->GBR+s.r_[0],static_cast<uint8_t>(temp));
	s.pc_ += 2;
	s.cycles_elapsed_ = 3;
}


inline void CSH2::BF(Sh2& s)
{
	// Si T = 0, disp*2 + PC -> PC
	// Si T = 1, nop

	s32 disp;

	if ((static_cast<s32>(x0nn(s)) & 0x80)==0) disp=(0x000000FF & static_cast<s32>(x0nn(s)));
	else disp=(0xFFFFFF00 | static_cast<s32>(x0nn(s)));
	if (s.sr_.get(StatusRegister::t)==0x0){
		s.pc_=s.pc_+(disp<<1)+4;
		s.cycles_elapsed_ = 3;
	}
	else{
		s.pc_+=2;
		//return false;
		s.cycles_elapsed_ = 1;
	}
}


inline void CSH2::BFS(Sh2& s)
{
	// If T=0, disp*2 + PC -> PC
	// If T=1, nop
	// Modified using SH4 manual

	s32 disp;
	// Registers save for the delay slot
	sh2->delaySlotRegisterSave_[0] = s.pc_;

	if ((x0nn(s)&0x80)==0) disp=(0x000000FF & static_cast<s32>(x0nn(s)));
	else disp=(0xFFFFFF00 | static_cast<s32>(x0nn(s)));
	if (s.sr_.get(StatusRegister::t)==0x0) {
		sh2->Delay_Slot(sh2,s.pc_+2);
		s.pc_=sh2->delaySlotRegisterSave_[0] + (disp<<1) + 4;
		s.cycles_elapsed_ = 2;
	}
	else{ 
		s.pc_+=2;
		s.cycles_elapsed_ = 1;
	}
}

inline void CSH2::BRA(Sh2& s)
{
	// disp*2 + PC -> PC
	// Modified using SH4 manual

	s32 disp;
	// PC save ofr the delay slot
	sh2->delaySlotRegisterSave_[0] = s.pc_;

	if ((xnnn(s)&0x800)==0) disp = (0x00000FFF & xnnn(s));
	else disp = (0xFFFFF000 | xnnn(s));
	Delay_Slot(sh2,s.pc_+2);
	s.pc_ = sh2->delaySlotRegisterSave_[0] + (disp<<1) +4;
	s.cycles_elapsed_ = 2;
}


inline void CSH2::BRAF(Sh2& s)
{
	// Rm + PC +4 -> PC
	// Modified using SH4 manual + correction
	// Registers save for the delay slot
	sh2->delaySlotRegisterSave_[0] = s.pc_;
	sh2->delaySlotRegisterSave_[1] = s.r_[xn00(s)];

	Delay_Slot(sh2,s.pc_+2);
	s.pc_ = sh2->delaySlotRegisterSave_[0] + sh2->delaySlotRegisterSave_[1] + 4;
	s.cycles_elapsed_ = 2;
}


inline void CSH2::BSR(Sh2& s)
{
	// PC -> PR, disp*2 + PC -> PC
	// Modified using SH4 manual + correction

	s32 disp;
	// Registers save for the delay slot
	sh2->delaySlotRegisterSave_[0] = s.pc_;

	if ((xnnn(s) & 0x800)==0) disp=(0x00000FFF & xnnn(s));
	else disp = (0xFFFFF000 | xnnn(s));
	sh2->PR = s.pc_ + 4;
	Delay_Slot(sh2,s.pc_+2);
	s.pc_ = sh2->delaySlotRegisterSave_[0] + (disp<<1) + 4;
	s.cycles_elapsed_ = 2;
}


inline void CSH2::BSRF(Sh2& s)
{
	// PC -> PR, Rm + PC -> PC
	// Modified using SH4 manual + correction
	// Registers save for the delay slot
	sh2->delaySlotRegisterSave_[0] = s.pc_;
	sh2->delaySlotRegisterSave_[1] = s.r_[xn00(s)];

	sh2->PR = s.pc_ + 4;
	Delay_Slot(sh2,s.pc_+2);
	s.pc_ = sh2->delaySlotRegisterSave_[0] + 4 + sh2->delaySlotRegisterSave_[1];
	s.cycles_elapsed_ = 2;
}


inline void CSH2::BT(Sh2& s)
{
	// If T=1, disp*2 + PC -> PC;
	// If T=0=, nop
	s32 disp;

	if ((x0nn(s) & 0x80)==0) disp=(0x000000FF & static_cast<s32>(x0nn(s)));
	else disp = (0xFFFFFF00 | static_cast<s32>(x0nn(s)));
	if (s.sr_.get(StatusRegister::t)==0x1){
		s.pc_ = s.pc_ + (disp<<1) + 4;
		s.cycles_elapsed_ = 3;
	}
	else{
		s.pc_+=2;
		s.cycles_elapsed_ = 1;
	}
}


inline void CSH2::BTS(Sh2& s)
{
	// If T=1, disp*2 + PC -> PC
	// If T=0, nop
	// Modified using SH4 manual
	s32 disp;
	// Registers save for the delay slot
	sh2->delaySlotRegisterSave_[0] = s.pc_;

	if ((x0nn(s) & 0x80)==0) disp=(0x000000FF & static_cast<s32>(x0nn(s)));
	else disp = (0xFFFFFF00 | static_cast<s32>(x0nn(s)));
	if (s.sr_.get(StatusRegister::t)==0x1) {
		sh2->Delay_Slot(sh2,s.pc_+2);
		s.pc_ = sh2->delaySlotRegisterSave_[0] + (disp<<1) +4;
		s.cycles_elapsed_ = 2;
	}
	else {
		s.pc_ += 2;
		s.cycles_elapsed_ = 1;
	}
}


inline void CSH2::CLRMAC(Sh2& s)
{
	// 0 -> MACH,MACL
	sh2->MACH = 0;
	sh2->MACL = 0;
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}


inline void CSH2::CLRT(Sh2& s)
{
	//0 -> T
	Clear_T(sh2);
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}


inline void CSH2::CMPEQ(Sh2& s)
{
	// If Rn = Rm, T=1
	if (s.r_[xn00(s)]==s.r_[x0n0(s)]) Set_T(sh2);
	else Clear_T(sh2);
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}


inline void CSH2::CMPGE(Sh2& s)
{
	// If Rn >= Rm with sign, T=1
	if (static_cast<s32>(s.r_[xn00(s)]) >= static_cast<s32>(s.r_[x0n0(s)])) Set_T(sh2);
	else Clear_T(sh2);
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::CMPGT(Sh2& s)
{
	// If Rn > Rm with sign, T=1
	if (static_cast<s32>(s.r_[xn00(s)]) > static_cast<s32>(s.r_[x0n0(s)])) Set_T(sh2);
	else Clear_T(sh2);
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::CMPHI(Sh2& s)
{
	// If Rn > Rm without sign, T=1
	if (static_cast<boost::uint32_t>(s.r_[xn00(s)]) > static_cast<boost::uint32_t>(s.r_[x0n0(s)])) Set_T(sh2);
	else Clear_T(sh2);
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::CMPHS(Sh2& s)
{
	// If Rn > Rm without sign, T=1
	if (static_cast<boost::uint32_t>(s.r_[xn00(s)]) >= static_cast<boost::uint32_t>(s.r_[x0n0(s)])) Set_T(sh2);
	else Clear_T(sh2);
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::CMPPL(Sh2& s)
{
	//If Rn > 0, T=1
	if (static_cast<s32>(s.r_[xn00(s)]) > 0) Set_T(sh2);
	else Clear_T(sh2);
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::CMPPZ(Sh2& s)
{
	//If Rn >= 0, T=1
	if (static_cast<s32>(s.r_[xn00(s)]) >= 0) Set_T(sh2);
	else Clear_T(sh2);
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::CMPSTR(Sh2& s)
{
	//If one byte of Rn = one byte of Rm then T=1
	uint32_t temp;
	s32 HH,HL,LH,LL;

	temp = s.r_[xn00(s)]^s.r_[x0n0(s)];
	HH = (temp>>12) & 0x000000FF;
	HL = (temp>>8) & 0x000000FF;
	LH = (temp>>4) & 0x000000FF;
	LL = temp & 0x000000FF;
	HH = HH && HL && LH && LL;
	if (HH == 0) Set_T(sh2);
	else Clear_T(sh2);
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::CMPIM(Sh2& s)
{
	//If R0 = imm, T=1
	s32 imm;

	if ((static_cast<s32>(x0nn(s)) & 0x80)==0) imm = (0x000000FF & static_cast<s32>(x0nn(s)));
	else imm = (0xFFFFFF00 | static_cast<s32>(x0nn(s)));
	if (s.r_[0]==imm) Set_T(sh2);
	else Clear_T(sh2);
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline uint8_t CSH2::M_Val(Sh2& s)
{
	// Returns M bit of SR
	return static_cast<uint8_t>(((sh2->SR & 0x200)>>9));
}

inline void CSH2::Clear_M(Sh2& s)
{
	// Clears M bit of SR
	sh2->SR &= 0xFDFF;
}

inline void CSH2::Set_M(Sh2& s)
{
	// Sets bit M of SR
	sh2->SR |= 0x200;
}

inline uint8_t CSH2::Q_Val(Sh2& s)
{
	// Returns Q bit of SR
	s32 temp;
	
	temp = sh2->SR & 0x100;
	if (static_cast<s32>(temp) == 0x100) return 1;
	else return 0;
}

inline void CSH2::Set_Q(Sh2& s)
{
	//Sets Q bit of SR
	if (!Q_Val(sh2))
		sh2->SR += 0x100;
}

inline void CSH2::Clear_Q(Sh2& s)
{
	// Clears Q bit of SR
	if (Q_Val(sh2))
		sh2->SR -= 0x100;
}

inline void CSH2::DIV0S(Sh2& s)
{
	// Rn MSB -> Q, Rm MSB -> M, M^Q -> T
	if ((s.r_[xn00(s)] & 0x80000000)==0) Clear_Q(sh2);
	else Set_Q(sh2);
	if ((s.r_[x0n0(s)] & 0x80000000)==0) Clear_M(sh2);
	else Set_M(sh2);
	if (M_Val(sh2)==Q_Val(sh2)) Clear_T(sh2);
	else Set_T(sh2);
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::DIV0U(Sh2& s)
{
	// 0 -> M/Q/T
	Clear_M(sh2);
	Clear_Q(sh2);
	Clear_T(sh2);
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::DIV1(Sh2& s)
{
	// Division done in one pass (Rn + Rm)
	uint32_t tmp0;
	uint8_t old_q,tmp1;

	old_q = Q_Val(sh2);
	if ((0x80000000 & s.r_[xn00(s)]) != 0) Set_Q(sh2);
	else Clear_Q(sh2);
	s.r_[xn00(s)]<<=1;
	s.r_[xn00(s)] |= s.sr_.get(StatusRegister::t);
	switch (old_q){
	case 0: switch(M_Val(sh2)){
			case 0: tmp0 = s.r_[xn00(s)];
					s.r_[xn00(s)] -=s.r_[x0n0(s)];
					tmp1 = (static_cast<uint32_t>(s.r_[xn00(s)]) > tmp0);
					switch(Q_Val(sh2)){
					case 0: if (tmp1) Set_Q(sh2);
							else Clear_Q(sh2);
							break;
					case 1: if (tmp1 == 0) Set_Q(sh2);
							else Clear_Q(sh2);
							break;
					}
					break;
			case 1: tmp0 = s.r_[xn00(s)];
					s.r_[xn00(s)] += s.r_[x0n0(s)];
					tmp1 = (static_cast<uint32_t>(s.r_[xn00(s)]) < tmp0);
					switch(Q_Val(sh2)){
					case 0: if (tmp1 == 0) Set_Q(sh2);
							else Clear_Q(sh2);
							break;
					case 1: if (tmp1) Set_Q(sh2);
							else Clear_Q(sh2);
							break;
					}
					break;
			}
			break;
	case 1: switch(M_Val(sh2)){
			case 0: tmp0 = s.r_[xn00(s)];
					s.r_[xn00(s)] += s.r_[x0n0(s)];
					tmp1 = (static_cast<uint32_t>(s.r_[xn00(s)]) < tmp0);
					switch (Q_Val(sh2)){
					case 0: if (tmp1) Set_Q(sh2);
							else Clear_Q(sh2);
							break;
					case 1: if (tmp1==0) Set_Q(sh2);
							else Clear_Q(sh2);
							break;
					}
				break;
			case 1: tmp0 = s.r_[xn00(s)];
					s.r_[xn00(s)] -= s.r_[x0n0(s)];
					tmp1 = (static_cast<uint32_t>(s.r_[xn00(s)]) > tmp0);
					switch (Q_Val(sh2)){
					case 0: if (tmp1 == 0) Set_Q(sh2);
							else Clear_Q(sh2);
							break;
					case 1: if (tmp1) Set_Q(sh2);
							else Clear_Q(sh2);
							break;
					}
				break;
			}
		break;
	}
	if (Q_Val(sh2) == M_Val(sh2)) Set_T(sh2);
	else Clear_T(sh2);

	s.pc_+=2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::DMULS(Sh2& s)
{
	// With sign, Rn * Rm -> MACH,MACL

	// Arranged using SH4 manual
	__int64 result;
	result = static_cast<__int64>(s.r_[x0n0(s)]) * static_cast<__int64>(s.r_[xn00(s)]);
	sh2->MACH = static_cast<s32>(result>>32);
	sh2->MACL = static_cast<uint32_t>(result&0x00000000FFFFFFFF);
	s.pc_ += 2;
	s.cycles_elapsed_ = 2;
}

s32 CSH2::DisplayMACH()
{
	//Returns MACH value
	return MACH;
}

s32 CSH2::DisplayMACL()
{
	//Returns MACL value
	return MACL;
}

s32 CSH2::DisplayPC()
{
	//Returns PC value
	return PC;
}

s32 CSH2::DisplayPR()
{
	//Returns PR value
	return PR;
}

s32 CSH2::DisplayGBR()
{
	//Returns GBR value
	return GBR;
}

s32 CSH2::DisplayVBR()
{
	//Returns VBR value
	return VBR;
}

s32 CSH2::DisplaySR()
{
	//Returns SR value
	return SR;
}

inline void CSH2::DMULU(Sh2& s)
{
	// Without sign, Rm * Rn -> MACH, MACL

	// MIGHT BE WRONG
	
	// Arranged using SH4 manual
	unsigned __int64 result;
	result = static_cast<unsigned __int64>(s.r_[x0n0(s)])*static_cast<unsigned __int64>(s.r_[xn00(s)]);
	sh2->MACH = static_cast<uint32_t>(result>>32);
	sh2->MACL = static_cast<uint32_t>(result&0x00000000FFFFFFFF);
	s.pc_ += 2;
	s.cycles_elapsed_ = 2;
}

inline void CSH2::DT(Sh2& s)
{
	// Rn - 1 -> Rn;
	// Si R[n] = 0, T=1
	// Sinon T=0
	s.r_[xn00(s)]--;
	if (s.r_[xn00(s)]==0) Set_T(sh2);
	else Clear_T(sh2);
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::EXTSB(Sh2& s)
{
	// Rm sign extension (byte) -> Rn
	s.r_[xn00(s)] = s.r_[x0n0(s)];
	if ((s.r_[x0n0(s)] & 0x00000080)==0) s.r_[xn00(s)] &= 0x000000FF;
	else s.r_[xn00(s)] |= 0xFFFFFF00;
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::EXTSW(Sh2& s)
{
	// Rm sign extension (word) -> Rn
	s.r_[xn00(s)] = s.r_[x0n0(s)];
	if ((s.r_[x0n0(s)] & 0x00008000)==0) s.r_[xn00(s)] &= 0x0000FFFF;
	else s.r_[xn00(s)] |= 0xFFFF0000;
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::EXTUB(Sh2& s)
{
	// Rm is 0 extended (byte) -> Rn
	s.r_[xn00(s)] = s.r_[x0n0(s)];
	s.r_[xn00(s)] &= 0x000000FF;
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::EXTUW(Sh2& s)
{
	// Rm is 0 extended (word) -> Rn
	s.r_[xn00(s)] = s.r_[x0n0(s)];
	s.r_[xn00(s)] &= 0x0000FFFF;
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::JMP(Sh2& s)
{
	// Rm -> PC
	// Arranged and fixed using SH4 manual

	// Registers save for the delay slot
	sh2->delaySlotRegisterSave_[0] = s.r_[xn00(s)];
	Delay_Slot(sh2,s.pc_ + 2);
	s.pc_ = sh2->delaySlotRegisterSave_[0]; 
	s.cycles_elapsed_ = 2;
}

inline void CSH2::JSR(Sh2& s)
{
	//PC -> PR, Rm -> PC
	// Arranged and fixed using SH4 manual
	
	// Registers save for the delay slot
	sh2->delaySlotRegisterSave_[0] = s.r_[xn00(s)];

	sh2->PR = s.pc_ + 4;
	Delay_Slot(sh2,s.pc_ + 2);
	s.pc_ = sh2->delaySlotRegisterSave_[0];
	s.cycles_elapsed_ = 2;
}

inline void CSH2::LDCSR(Sh2& s)
{
	// Rm -> SR
	sh2->SR = s.r_[xn00(s)] & 0x000003F3;
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::LDCGBR(Sh2& s)
{
	// Rm -> GBR
	sh2->GBR = s.r_[xn00(s)];
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::LDCVBR(Sh2& s)
{
	// Rm -> VBR
	sh2->VBR = s.r_[xn00(s)];
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::LDCMSR(Sh2& s)
{
	// (Rm) -> SR, Rm + 4 -> Rm
	sh2->SR = EmuState::pMem->ReadLong(s.r_[xn00(s)]) & 0x000003F3;
	s.r_[xn00(s)] += 4;
	s.pc_ += 2;
	s.cycles_elapsed_ = 3;
}

inline void CSH2::LDCMGBR(Sh2& s)
{
	// (Rm) -> GBR, Rm + 4 -> Rm
	sh2->GBR = EmuState::pMem->ReadLong(s.r_[xn00(s)]);
	s.r_[xn00(s)] += 4;
	s.pc_ += 2;
	s.cycles_elapsed_ = 3;
}

inline void CSH2::LDCMVBR(Sh2& s)
{
	// (Rm) -> VBR, Rm + 4 -> Rm
	sh2->VBR = EmuState::pMem->ReadLong(s.r_[xn00(s)]);
	s.r_[xn00(s)] += 4;
	s.pc_ += 2;
	s.cycles_elapsed_ = 3;
}

inline void CSH2::LDSMACH(Sh2& s)
{
	//Rm -> MACH
	sh2->MACH = s.r_[xn00(s)];
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::LDSMACL(Sh2& s)
{
	//Rm -> MACL
	sh2->MACL = s.r_[xn00(s)];
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::LDSPR(Sh2& s)
{
	// Rm -> PR
	sh2->PR = s.r_[xn00(s)];
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::LDSMMACH(Sh2& s)
{
	//(Rm) -> MACH, Rm + 4 -> Rm
	sh2->MACH = EmuState::pMem->ReadLong(s.r_[xn00(s)]);
	s.r_[xn00(s)] += 4;
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::LDSMMACL(Sh2& s)
{
	//(Rm) -> MACL, Rm + 4 -> Rm
	sh2->MACL = EmuState::pMem->ReadLong(s.r_[xn00(s)]);
	s.r_[xn00(s)] += 4;
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::LDSMPR(Sh2& s)
{
	//(Rm) -> PR, Rm + 4 -> Rm
	sh2->PR = EmuState::pMem->ReadLong(s.r_[xn00(s)]);
	s.r_[xn00(s)] += 4;
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::MAC(Sh2& s)
{
	// Signed operation, (Rn)*(Rm) + MAC -> MAC

	// Arranged using SH4 manual
	__int64 src_m,src_n;
	__int64 mul,mac;

	src_n = static_cast<__int64>(static_cast<s32>(EmuState::pMem->ReadLong(s.r_[xn00(s)])));
	s.r_[xn00(s)]+=4;
	src_m = static_cast<__int64>(static_cast<s32>(EmuState::pMem->ReadLong(s.r_[x0n0(s)])));
	s.r_[x0n0(s)]+=4;

	mul=src_m*src_n;
	
	mac = static_cast<uint32_t>(sh2->MACH);
	mac <<= 32;
	mac |= static_cast<uint32_t>(sh2->MACL);
	mac+=mul;

	if(S_Val(sh2)==1){
		if(mac<0xFFFF800000000000) mac=0xFFFF800000000000;
		if(mac>0x800000000000) mac=0x7FFFFFFFFFFF;
	}
	sh2->MACH = static_cast<uint32_t>(mac>>32);
	sh2->MACL = static_cast<uint32_t>(mac&0xFFFFFFFF);

	s.pc_ += 2;
	s.cycles_elapsed_ = 3;
}

inline uint8_t CSH2::S_Val(Sh2& s)
{
	// Returns S bit of SR
	s32 temp;
	
	temp = sh2->SR & 0x2;
	if (static_cast<s32>(temp) == 0x2) return 1;
	else return 0;
}

inline void CSH2::MACW(Sh2& s)
{
	// Signed operation, (Rn) * (Rm) + MAC -> MAC

	// Arranged using SH4 manual
	__int64 src_m,src_n;
	__int64 mul,mac;

	src_n=static_cast<__int64>(static_cast<s32>(EmuState::pMem->ReadWord(s.r_[xn00(s)])));
	s.r_[xn00(s)]+=2;
	src_m=static_cast<__int64>(static_cast<s32>(EmuState::pMem->ReadWord(s.r_[x0n0(s)])));
	s.r_[x0n0(s)]+=2;

	mul= src_m*src_n;
	if(S_Val(sh2)==0){
		mac = static_cast<uint32_t>(sh2->MACH);
		mac <<= 32;
		mac |= static_cast<uint32_t>(sh2->MACL);
		mac+=mul;
		sh2->MACH=static_cast<uint32_t>(mac>>32);
		sh2->MACL=static_cast<uint32_t>(mac&0xFFFFFFFF);
	}else{
		if(sh2->MACL&0x80000000) mac=static_cast<__int64>(sh2->MACL | 0xFFFFFFFF00000000);
		else mac=static_cast<__int64>(sh2->MACL & 0x00000000FFFFFFFF);
		mac+=mul;
		if(mac>0x7FFFFFFF){
			sh2->MACH|=0x00000001; sh2->MACL=0x7FFFFFFF;
		}else if(mac<0xFFFFFFFF80000000){
			sh2->MACH|=0x00000001; sh2->MACL=0x80000000;
		}else{
			sh2->MACH&=0xFFFFFFFE; sh2->MACL=static_cast<uint32_t>(mac&0x00000000FFFFFFFF);
		}
	}
	s.pc_ += 2;
	s.cycles_elapsed_ = 3;
}

inline void CSH2::MOV(Sh2& s)
{
	//Rm -> Rn
	s.r_[xn00(s)] = s.r_[x0n0(s)];
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::MOVBS(Sh2& s)
{
	//Rm -> (Rn)
	EmuState::pMem->WriteByte(s.r_[xn00(s)],static_cast<uint8_t>(s.r_[x0n0(s)]));
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::MOVWS(Sh2& s)
{
	//Rm -> (Rn)
	EmuState::pMem->WriteWord(s.r_[xn00(s)],static_cast<uint16_t>(s.r_[x0n0(s)]));
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::MOVLS(Sh2& s)
{
	//Rm -> (Rn)
	EmuState::pMem->WriteLong(s.r_[xn00(s)],s.r_[x0n0(s)]);
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::MOVBL(Sh2& s)
{
	// (Rm) -> sign extension -> Rn
	s.r_[xn00(s)] = static_cast<s32>(EmuState::pMem->ReadByte(s.r_[x0n0(s)]));
	if ((s.r_[xn00(s)] & 0x80) == 0) s.r_[xn00(s)] &= 0x000000FF;
	else s.r_[xn00(s)] |= 0xFFFFFF00;
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::MOVWL(Sh2& s)
{
	// (Rm) -> sign extension -> Rn
	s.r_[xn00(s)] = static_cast<s32>(EmuState::pMem->ReadWord(s.r_[x0n0(s)]));
	if ((s.r_[xn00(s)] & 0x8000)==0) s.r_[xn00(s)] &= 0x0000FFFF;
	else s.r_[xn00(s)] |= 0xFFFF0000;
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::MOVLL(Sh2& s)
{
	// (Rm) -> Rn
	s.r_[xn00(s)] = EmuState::pMem->ReadLong(s.r_[x0n0(s)]);
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::MOVBM(Sh2& s)
{
	// Rn - 1 -> Rn, Rm -> (Rn)
	EmuState::pMem->WriteByte(s.r_[xn00(s)] - 1,static_cast<uint8_t>(s.r_[x0n0(s)]));
	s.r_[xn00(s)] -= 1;
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::MOVWM(Sh2& s)
{
	// Rn - 2 -> Rn, Rm -> (Rn)
	EmuState::pMem->WriteWord(s.r_[xn00(s)]-2,static_cast<uint16_t>(s.r_[x0n0(s)]));
	s.r_[xn00(s)] -= 2;
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::MOVLM(Sh2& s)
{
	// Rn - 4 -> Rn, Rm -> (Rn)
	EmuState::pMem->WriteLong(s.r_[xn00(s)]-4,s.r_[x0n0(s)]);
	s.r_[xn00(s)] -= 4;
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::MOVBP(Sh2& s)
{
	// (Rm) -> sign extension -> Rn, Rm + 1 -> Rm
	s.r_[xn00(s)] = static_cast<s32>(EmuState::pMem->ReadByte(s.r_[x0n0(s)]));
	if ((s.r_[xn00(s)] & 0x80)==0) s.r_[xn00(s)] &= 0x000000FF;
	else s.r_[xn00(s)] |= 0xFFFFFF00;
	if (xn00(s)!=x0n0(s)) s.r_[x0n0(s)] += 1;
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::MOVWP(Sh2& s)
{
	// (Rm) -> sign extension -> Rn, Rm + 2 -> Rm
	s.r_[xn00(s)] = static_cast<s32>(EmuState::pMem->ReadWord(s.r_[x0n0(s)]));
	if ((s.r_[xn00(s)] & 0x8000)==0) s.r_[xn00(s)] &= 0x0000FFFF;
	else s.r_[xn00(s)] |= 0xFFFF0000;
	if (xn00(s)!=x0n0(s)) s.r_[x0n0(s)] += 2;
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::MOVLP(Sh2& s)
{
	// (Rm) -> Rn, Rm + 4 -> Rm
	s.r_[xn00(s)] = EmuState::pMem->ReadLong(s.r_[x0n0(s)]);
	if (xn00(s)!=x0n0(s)) s.r_[x0n0(s)] += 4;
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::MOVBS0(Sh2& s)
{
	// Rm -> (R0 + Rn)
	EmuState::pMem->WriteByte(s.r_[xn00(s)] + s.r_[0], static_cast<uint8_t>(s.r_[x0n0(s)]));
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::MOVWS0(Sh2& s)
{
	// Rm -> (R0 + Rn)
	EmuState::pMem->WriteWord(s.r_[xn00(s)] + s.r_[0], static_cast<uint16_t>(s.r_[x0n0(s)]));
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::MOVLS0(Sh2& s)
{
	// Rm -> (R0 + Rn)
	EmuState::pMem->WriteLong(s.r_[xn00(s)] + s.r_[0], s.r_[x0n0(s)]);
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::MOVBL0(Sh2& s)
{
	// (R0 + Rm) -> sign extension -> Rn
	s.r_[xn00(s)] = static_cast<s32>(EmuState::pMem->ReadByte(s.r_[x0n0(s)] + s.r_[0]));
	if ((s.r_[xn00(s)] & 0x80)==0) s.r_[xn00(s)] &= 0x000000FF;
	else s.r_[xn00(s)] |= 0xFFFFFF00;
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::MOVWL0(Sh2& s)
{
	// (R0 + Rm) -> sign extension -> Rn
	s.r_[xn00(s)] = static_cast<s32>(EmuState::pMem->ReadWord(s.r_[x0n0(s)]+s.r_[0]));
	if ((s.r_[xn00(s)] & 0x8000)==0) s.r_[xn00(s)] &=0x0000FFFF;
	else s.r_[xn00(s)] |= 0xFFFF0000;
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::MOVLL0(Sh2& s)
{
	// (R0 + Rm) -> Rn
	s.r_[xn00(s)] = EmuState::pMem->ReadLong(s.r_[x0n0(s)] + s.r_[0]);
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::MOVI(Sh2& s)
{
	// imm -> sign extension -> Rn
	if ((x0nn(s) & 0x80)==0) s.r_[xn00(s)] = (0x000000FF & static_cast<s32>(x0nn(s)));
	else s.r_[xn00(s)] = (0xFFFFFF00 | static_cast<s32>(x0nn(s)));
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::MOVWI(Sh2& s)
{
	//(disp * 2 + PC) -> sign extension -> Rn
	s32 disp;
	disp = (0x000000FF & static_cast<s32>(x0nn(s)));
	s.r_[xn00(s)] = static_cast<s32>(EmuState::pMem->ReadWord(s.pc_ + (disp<<1) + 4)); // + 4 added
	if ((s.r_[xn00(s)] & 0x8000)==0) s.r_[xn00(s)] &= 0x0000FFFF;
	else s.r_[xn00(s)] |= 0xFFFF0000;
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::MOVLI(Sh2& s)
{
	//(disp * 4 + PC) -> Rn
	s32 disp;

	disp = (0x000000FF & static_cast<s32>(x0nn(s)));
	s.r_[xn00(s)] = EmuState::pMem->ReadLong((s.pc_ & 0xFFFFFFFC) + (disp<<2) + 4); // + 4 added
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::MOVBLG(Sh2& s)
{
	//(disp + GBR) -> sign extension -> R0
	s32 disp;

	disp = (0x000000FF & static_cast<s32>(x0nn(s)));
	s.r_[0] = (s32)EmuState::pMem->ReadByte(sh2->GBR + disp);
	if ((s.r_[0] & 0x80)==0) s.r_[0] &= 0x000000FF;
	else s.r_[0] |= 0xFFFFFF00;
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::MOVWLG(Sh2& s)
{
	// (disp *2 + BGR) -> sign extension -> R0
	s32 disp;

	disp = (0x000000FF & static_cast<s32>(x0nn(s)));
	s.r_[0] = static_cast<s32>(EmuState::pMem->ReadWord(sh2->GBR + (disp<<1)));
	if ((s.r_[0] & 0x8000)==0) s.r_[0] &= 0x0000FFFF;
	else s.r_[0] |= 0xFFFF0000;
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::MOVLLG(Sh2& s)
{
	// (disp *4 + GBR) -> R0
	s32 disp;

	disp = (0x000000FF & static_cast<s32>(x0nn(s)));
	s.r_[0] = EmuState::pMem->ReadLong(sh2->GBR + (disp<<2));
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::MOVBSG(Sh2& s)
{
	// R0 -> (disp + GBR)
	s32 disp;

	disp = (0x000000FF & static_cast<s32>(x0nn(s)));
	EmuState::pMem->WriteByte(sh2->GBR + disp,static_cast<uint8_t>(s.r_[0]));
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::MOVWSG(Sh2& s)
{
	// R0 -> (disp *2 + GBR)
	s32 disp;

	disp = (0x000000FF & static_cast<s32>(x0nn(s)));
	EmuState::pMem->WriteWord(sh2->GBR + (disp<<1),static_cast<uint16_t>(s.r_[0]));
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::MOVLSG(Sh2& s)
{
	// R0 -> (disp *4 + GBR)
	s32 disp;

	disp = (0x000000FF & static_cast<s32>(x0nn(s)));
	EmuState::pMem->WriteLong(sh2->GBR + (disp<<2),s.r_[0]);
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::MOVBS4(Sh2& s)
{
	// R0 -> (disp + Rn)
	s32 disp;

	disp = (0x0000000F & static_cast<s32>(x00n(s)));
	EmuState::pMem->WriteByte(s.r_[x0n0(s)] + disp,static_cast<uint8_t>(s.r_[0]));
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::MOVWS4(Sh2& s)
{
	// R0 -> (disp *2 + Rn)
	s32 disp;

	disp = (0x0000000F & static_cast<s32>(x00n(s)));
	EmuState::pMem->WriteWord(s.r_[x0n0(s)] + (disp<<1),static_cast<uint16_t>(s.r_[0]));
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::MOVLS4(Sh2& s)
{
	// Rm -> (disp *4 + Rn)
	s32 disp;

	disp = (0x0000000F & static_cast<s32>(x00n(s)));
	EmuState::pMem->WriteLong(s.r_[xn00(s)] + (disp<<2),s.r_[x0n0(s)]);
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::MOVBL4(Sh2& s)
{
	// (disp + Rm)-> sign extension ->R0
	s32 disp;

	disp = (0x0000000F & static_cast<s32>(x00n(s)));
	s.r_[0] = EmuState::pMem->ReadByte(s.r_[x0n0(s)]+disp);
	if ((s.r_[0] & 0x80)==0) s.r_[0] &= 0x000000FF;
	else s.r_[0] |= 0xFFFFFF00;
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::MOVWL4(Sh2& s)
{
	// (disp *2 + Rm)-> sign extension ->R0
	s32 disp;

	disp = (0x0000000F & static_cast<s32>(x00n(s)));
	s.r_[0] = EmuState::pMem->ReadWord(s.r_[x0n0(s)]+(disp<<1));
	if ((s.r_[0] & 0x8000)==0) s.r_[0] &= 0x0000FFFF;
	else s.r_[0] |= 0xFFFF0000;
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::MOVLL4(Sh2& s)
{
	// (disp *4 +Rm) -> Rn
	s32 disp;

	disp = (0x0000000F & static_cast<s32>(x00n(s)));
	s.r_[xn00(s)] = EmuState::pMem->ReadLong(s.r_[x0n0(s)] + (disp<<2));
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::MOVA(Sh2& s)
{
	// disp *4 + PC -> R0
	s32 disp;

	disp = (0x000000FF & static_cast<s32>(x0nn(s)));
	s.r_[0] = (s.pc_ & 0xFFFFFFFC) + (disp<<2) + 4; // + 4 added
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::MOVT(Sh2& s)
{
	//T -> Rn
	s.r_[xn00(s)] = (0x00000001 & sh2->SR);
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::MULL(Sh2& s)
{
	// Rn * Rm -> MACL
	sh2->MACL = s.r_[xn00(s)]*s.r_[x0n0(s)];
	s.pc_ += 2;
	s.cycles_elapsed_ = 2; // 2 to 4
}

inline void CSH2::MULS(Sh2& s)
{
	// signed operation, Rn*Rm -> MACL
	sh2->MACL = (static_cast<s32>(static_cast<int16_t>(s.r_[xn00(s)])) * static_cast<s32>(static_cast<int16_t>(s.r_[x0n0(s)])));
	s.pc_ += 2;
	s.cycles_elapsed_ = 1; // 1 to 3
}

inline void CSH2::MULU(Sh2& s)
{
	// No sign, Rn+Rm -> MAC
	sh2->MACL = (static_cast<uint32_t>(static_cast<uint16_t>(s.r_[xn00(s)]))*static_cast<boost::uint32_t>(static_cast<uint16_t>(s.r_[x0n0(s)])));
	s.pc_ += 2;
	s.cycles_elapsed_ = 1; // 1 to 3
}

inline void CSH2::NEG(Sh2& s)
{
	//0-Rm -> Rn
	s.r_[xn00(s)] = 0 - s.r_[x0n0(s)];
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::NEGC(Sh2& s)
{
	uint32_t temp;

	temp =0 - s.r_[x0n0(s)];
	s.r_[xn00(s)] = temp - s.sr_.get(StatusRegister::t);
	if (0<temp) Set_T(sh2);
	else Clear_T(sh2);
	if (temp<static_cast<uint32_t>(s.r_[xn00(s)])) Set_T(sh2);
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::NOP(Sh2& s)
{
	// Mo operation
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::NOT(Sh2& s)
{
	// -Rm -> Rn
	s.r_[xn00(s)] = ~s.r_[x0n0(s)];
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::OR(Sh2& s)
{
	// Rn | Rm -> Rn
	s.r_[xn00(s)] |= s.r_[x0n0(s)];
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::ORI(Sh2& s)
{
	// R0 | imm -> R0
	s.r_[0] |= (0x000000FF & static_cast<s32>(x0nn(s)));
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::ORM(Sh2& s)
{
	// (R0 + GBR) | imm -> (R0 + GBR)
	s32 temp;

	temp = static_cast<s32>(EmuState::pMem->ReadByte(sh2->GBR + s.r_[0]));
	temp |= (0x000000FF & static_cast<s32>(x0nn(s)));
	EmuState::pMem->WriteByte(sh2->GBR + s.r_[0], static_cast<uint8_t>(temp));
	s.pc_ += 2;
	s.cycles_elapsed_ = 3;
}

inline void CSH2::ROTCL(Sh2& s)
{
	// T <- Rn <- T
	s32 temp;

	if ((s.r_[xn00(s)] & 0x80000000)==0) temp = 0;
	else temp = 1;
	s.r_[xn00(s)] <<= 1;
	if (s.sr_.get(StatusRegister::t)) s.r_[xn00(s)] |= 0x00000001;
	else s.r_[xn00(s)] &= 0xFFFFFFFE;
	if (temp == 1) Set_T(sh2);
	else Clear_T(sh2);
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::ROTCR(Sh2& s)
{
	// T -> Rn -> T
	s32 temp;

	if ((s.r_[xn00(s)] & 0x00000001)==0) temp=0;
	else temp = 1;
	s.r_[xn00(s)]>>=1;
	if (s.sr_.get(StatusRegister::t)) s.r_[xn00(s)] |= 0x80000000;
	else s.r_[xn00(s)] &= 0x7FFFFFFF;
	if (temp == 1) Set_T(sh2);
	else Clear_T(sh2);
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::ROTL(Sh2& s)
{
	//T <- Rn <- MSB
	if ((s.r_[xn00(s)] & 0x80000000)==0) Clear_T(sh2);
	else Set_T(sh2);
	s.r_[xn00(s)]<<=1;
	if (s.sr_.get(StatusRegister::t)) s.r_[xn00(s)] |= 0x00000001;
	else s.r_[xn00(s)] &= 0xFFFFFFFE;
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::ROTR(Sh2& s)
{
	// LSB -> Rn -> T
	if ((s.r_[xn00(s)] & 0x00000001)==0) Clear_T(sh2);
	else Set_T(sh2);
	s.r_[xn00(s)] >>=1;
	if (s.sr_.get(StatusRegister::t)) s.r_[xn00(s)] |= 0x80000000;
	else s.r_[xn00(s)] &= 0x7FFFFFFF;
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::RTE(Sh2& s)
{
	// Stack -> PC/SR
	// Modified by myself :)
	// Registers save for the delay slot
	Delay_Slot(sh2,s.pc_ + 2);
	s.pc_ = EmuState::pMem->ReadLong(s.r_[0xF]);
	s.r_[0xF] += 4;
	sh2->SR = EmuState::pMem->ReadLong(s.r_[0xF]) & 0x000003F3;
	s.r_[0xF] += 4;
	s.cycles_elapsed_ = 4;

	if(sh2->isMaster){
		if(EmuState::gInt->GetCurrentIntMaster())
		{
			// Interrupt being executed, we get back
			EmuState::pScu->ClearISTFlag(EmuState::gInt->lastMasterInterruptVector);
			// Log write
			#ifdef _LOGS
			EmuState::pLog->InterruptWrite("*** Back from interrupt ***");
			switch (EmuState::gInt->lastMasterInterruptVector)
			{
				case 0x40: // V-BLANK-IN
					EmuState::pLog->Routine("VBlank-In interrupt routine finished");
					break;
				case 0x41: // V-BLANK-OUT
					EmuState::pLog->Routine("VBlank-Out interrupt routine finished");
					break;
				case 0x42: // H-BLANK-IN
					EmuState::pLog->Routine("HBlank-In interrupt routine finished");
					break;
				case 0x43: // Timer 0
					EmuState::pLog->Routine("Timer 0 interrupt routine finished");
					break;
				case 0x44: // Timer 1
					EmuState::pLog->Routine("Timer 1 interrupt routine finished");
					break;
				case 0x45: // DSP End
					EmuState::pLog->Routine("DSP End interrupt routine finished");
					break;
				case 0x46: // Sound Request
					EmuState::pLog->Routine("Sound request interrupt routine finished");
					break;
				case 0x47: // System Manager
					EmuState::pLog->Routine("System manager interrupt routine finished");
					break;
				case 0x48: // PAD Interrupt
					EmuState::pLog->Routine("PAD interrupt routine finished");
					break;
				case 0x49: // Level-2 DMA End
					EmuState::pLog->Routine("Level 2 DMA end interrupt routine finished");
					break;
				case 0x4A: // Level-1 DMA End
					EmuState::pLog->Routine("Level 1 DMA end interrupt routine finished");
					break;
				case 0x4B: // Level-0 DMA End
					EmuState::pLog->Routine("Level 0 DMA end interrupt routine finished");
					break;
				case 0x4C: // DMA-Illegal
					EmuState::pLog->Routine("DMA illegal interrupt routine finished");
					break;
				case 0x4D: // Sprite Draw End
					EmuState::pLog->Routine("Sprite draw end interrupt routine finished");
					break;
			}

			format output("Level:0x%X");
			output % static_cast<uint32_t>(EmuState::gInt->lastMasterInterruptLevel);
			EmuState::pLog->ScuWrite(str( output ).c_str());
			EmuState::pLog->InterruptWrite(str( output ).c_str());
			#endif

			EmuState::gInt->SetCurrentIntMaster(false);
			EmuState::gInt->lastMasterInterruptVector = 0;
			EmuState::gInt->IRQM[EmuState::gInt->lastMasterInterruptLevel] = false;
			EmuState::gInt->lastMasterInterruptLevel = 0;

		}
	}else{
		if(EmuState::gInt->GetCurrentIntSlave())
		{
			// Interrupt being executed, we get back
			EmuState::gInt->SetCurrentIntSlave(false);
			EmuState::gInt->lastSlaveInterruptVector = 0;
			EmuState::gInt->IRQS[EmuState::gInt->lastSlaveInterruptLevel] = false;
			EmuState::gInt->lastSlaveInterruptLevel = 0;
		}
	}
}

inline void CSH2::RTS(Sh2& s)
{
	// PR -> PC
	// Arranged and fixed using SH4 manual
	
	Delay_Slot(sh2,s.pc_ + 2);
	s.pc_ = sh2->PR;
	s.cycles_elapsed_ = 2;
}

inline void CSH2::SETT(Sh2& s)
{
	//1 -> T
	Set_T(sh2);
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::SHAL(Sh2& s)
{
	//T <- Rn <- 0
	if ((s.r_[xn00(s)] & 0x80000000)==0) Clear_T(sh2);
	else Set_T(sh2);
	s.r_[xn00(s)] <<= 1;
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::SHAR(Sh2& s)
{
	// MSB -> Rn -> T
	s32 temp;

	if ((s.r_[xn00(s)] & 0x0000001)==0) Clear_T(sh2);
	else Set_T(sh2);
	if ((s.r_[xn00(s)] & 0x80000000)==0) temp = 0;
	else temp = 1;
	s.r_[xn00(s)] >>= 1;
	if (temp==1) s.r_[xn00(s)] |= 0x80000000;
	else s.r_[xn00(s)] &= 0x7FFFFFFF;
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::SHLL(Sh2& s)
{
	// T <- Rn <- 0
	if ((s.r_[xn00(s)] & 0x80000000)==0) Clear_T(sh2);
	else Set_T(sh2);
	s.r_[xn00(s)] <<= 1;
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::SHLL2(Sh2& s)
{
	// Rn << 2 -> Rn
	s.r_[xn00(s)] <<= 2;
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::SHLL8(Sh2& s)
{
	// Rn << 8 -> Rn
	s.r_[xn00(s)] <<= 8;
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::SHLL16(Sh2& s)
{
	// Rn << 16 -> Rn
	s.r_[xn00(s)] <<= 16;
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::SHLR(Sh2& s)
{
	// 0 -> Rn -> T
	if ((s.r_[xn00(s)] & 0x00000001)==0) Clear_T(sh2);
	else Set_T(sh2);
	s.r_[xn00(s)] >>= 1;
	s.r_[xn00(s)] &= 0x7FFFFFFF;
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::SHLR2(Sh2& s)
{
	// Rn >> 2 -> Rn
	s.r_[xn00(s)] >>= 2;
	s.r_[xn00(s)] &= 0x3FFFFFFF;
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::SHLR8(Sh2& s)
{
	// Rn >> 8 -> Rn
	s.r_[xn00(s)] >>= 8;
	s.r_[xn00(s)] &= 0x00FFFFFF;
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::SHLR16(Sh2& s)
{
	// Rn >> 16 -> Rn
	s.r_[xn00(s)] >>= 16;
	s.r_[xn00(s)] &= 0x0000FFFF;
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::SLEEP(Sh2& s)
{
	// Sleep
	// We'll see later how to implement this operation.
	// It'll involve waiting until an interrupt is fired up
	// Fixing needs some more researches on the Power Down Mode
	// Maybe a SH2 boolean to update ?
	s.cycles_elapsed_ = 3;
}

inline void CSH2::STCSR(Sh2& s)
{
	// SR -> Rn
	s.r_[xn00(s)] = sh2->SR;
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::STCGBR(Sh2& s)
{
	// GBR -> Rn
	s.r_[xn00(s)] = sh2->GBR;
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::STCVBR(Sh2& s)
{
	// VBR -> Rn
	s.r_[xn00(s)] = sh2->VBR;
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::STCMSR(Sh2& s)
{
	// Rn-4 -> Rn, SR -> (Rn)
	s.r_[xn00(s)] -= 4;
	EmuState::pMem->WriteLong(s.r_[xn00(s)],sh2->SR);
	s.pc_ += 2;
	s.cycles_elapsed_ = 2;
}

inline void CSH2::STCMGBR(Sh2& s)
{
	// Rn-4 -> Rn, GBR -> (Rn)
	s.r_[xn00(s)] -= 4;
	EmuState::pMem->WriteLong(s.r_[xn00(s)],sh2->GBR);
	s.pc_ += 2;
	s.cycles_elapsed_ = 2;
}

inline void CSH2::STCMVBR(Sh2& s)
{
	// Rn-4 -> Rn, VBR -> (Rn)
	s.r_[xn00(s)] -= 4;
	EmuState::pMem->WriteLong(s.r_[xn00(s)],sh2->VBR);
	s.pc_ += 2;
	s.cycles_elapsed_ = 2;
}

inline void CSH2::STSMACH(Sh2& s)
{
	// MACH -> Rn
	s.r_[xn00(s)] = sh2->MACH;
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::STSMACL(Sh2& s)
{
	// MACL -> Rn
	s.r_[xn00(s)] = sh2->MACL;
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::STSPR(Sh2& s)
{
	// PR -> Rn
	s.r_[xn00(s)] = sh2->PR;
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::STSMMACH(Sh2& s)
{
	// Rn - :4 -> Rn, MACH -> (Rn)
	s.r_[xn00(s)] -= 4;
	EmuState::pMem->WriteLong(s.r_[xn00(s)], sh2->MACH);
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::STSMMACL(Sh2& s)
{
	// Rn - :4 -> Rn, MACL -> (Rn)
	s.r_[xn00(s)] -= 4;
	EmuState::pMem->WriteLong(s.r_[xn00(s)], sh2->MACL);
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::STSMPR(Sh2& s)
{
	// Rn - :4 -> Rn, PR -> (Rn)
	s.r_[xn00(s)] -= 4;
	EmuState::pMem->WriteLong(s.r_[xn00(s)], sh2->PR);
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::SUB(Sh2& s)
{
	// Rn - Rm -> Rn
	s.r_[xn00(s)] -= s.r_[x0n0(s)];
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::SUBC(Sh2& s)
{
	// Rn - Rm - T -> Rn, Carry -> T
	uint32_t tmp0,tmp1;

	tmp1 = s.r_[xn00(s)] - s.r_[x0n0(s)];
	tmp0 = s.r_[xn00(s)];
	s.r_[xn00(s)] = tmp1 - s.sr_.get(StatusRegister::t);
	if (tmp0<tmp1) Set_T(sh2);
	else Clear_T(sh2);
	if (tmp1<static_cast<uint32_t>(s.r_[xn00(s)])) Set_T(sh2);
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::SUBV(Sh2& s)
{
	// Rn - Rm -> Rn, underflow -> T
	s32 dest, src, ans;

	if (static_cast<s32>(s.r_[xn00(s)])>=0) dest = 0;
	else dest = 1;
	if (static_cast<s32>(s.r_[x0n0(s)])>=0) src = 0;
	else src = 1;
	src += dest;
	s.r_[xn00(s)] -= s.r_[x0n0(s)];
	if (static_cast<s32>(s.r_[xn00(s)])>=0) ans = 0;
	else ans = 1;
	ans += dest;
	if (src==1){
		if (ans==1) Set_T(sh2);
		else Clear_T(sh2);
	}
	else Clear_T(sh2);
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::SWAPB(Sh2& s)
{
	// Rm -> bytes swap -> Rn
	uint32_t temp0,temp1;

	temp0 = s.r_[x0n0(s)] & 0xFFFF0000;
	temp1 = (s.r_[x0n0(s)] & 0x000000FF)<<8;
	s.r_[xn00(s)] = (s.r_[x0n0(s)]>>8) & 0x000000FF;
	s.r_[xn00(s)] = s.r_[xn00(s)] | temp1 | temp0;
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;

}

inline void CSH2::SWAPW(Sh2& s)
{
	// Rm -> words swap -> Rn
	uint32_t temp;
	temp = (s.r_[x0n0(s)]>>16) & 0x0000FFFF;
	s.r_[xn00(s)] = s.r_[x0n0(s)]<<16;
	s.r_[xn00(s)] |= temp;
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::TAS(Sh2& s)
{
	// If (Rn) = 0, 1 -> T, 1 -> MSB of (Rn)
	s32 temp;

	temp =static_cast<s32>(EmuState::pMem->ReadByte(s.r_[xn00(s)]));
	if (temp==0) Set_T(sh2);
	else Clear_T(sh2);
	temp |= 0x00000080;
	EmuState::pMem->WriteByte(s.r_[xn00(s)],static_cast<uint8_t>(temp));
	s.pc_ += 2;
	s.cycles_elapsed_ = 4;
}

inline void CSH2::TRAPA(Sh2& s)
{
	// PC/SR -> stack, (imm*4 + VBR) -> PC
	s32 imm;

	imm = (0x000000FF & x0nn(s));
	s.r_[15] -= 4;
	EmuState::pMem->WriteLong(s.r_[15], sh2->SR);
	s.r_[15] -= 4;
	EmuState::pMem->WriteLong(s.r_[15],s.pc_+2);
	s.pc_ = EmuState::pMem->ReadLong(sh2->VBR + (imm<<2));
	s.cycles_elapsed_ = 8;
}

inline void CSH2::TST(Sh2& s)
{
	// Rn & Rm, if result = 0, 1 -> T
	if ((s.r_[xn00(s)] & s.r_[x0n0(s)])==0) Set_T(sh2);
	else Clear_T(sh2);

	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::TSTI(Sh2& s)
{
	// R0 & imm, if result is 0, 1 -> T
	s32 temp;

	temp = s.r_[0] & (0x000000FF & static_cast<s32>(x0nn(s)));
	if (temp==0) Set_T(sh2);
	else Clear_T(sh2);
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::TSTM(Sh2& s)
{
	// (R0 + GBR) & imm, if result is 0, 1 -> T
	s32 temp;

	temp = static_cast<s32>(EmuState::pMem->ReadByte(sh2->GBR + s.r_[0]));
	temp &= (0x000000FF & static_cast<s32>(x0nn(s)));
	if (temp==0) Set_T(sh2);
	else Clear_T(sh2);
	s.pc_ += 2;
	s.cycles_elapsed_ = 3;
}

inline void CSH2::XOR(Sh2& s)
{
	// Rn^Rm -> Rn
	s.r_[xn00(s)] ^= s.r_[x0n0(s)];
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::XORI(Sh2& s)
{
	// R0 ^imm -> R0
	s.r_[0] ^= (0x000000FF & static_cast<s32>(x0nn(s)));
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}

inline void CSH2::XORM(Sh2& s)
{
	// (R0 + GBR)^imm -> (R0 + GBR)
	s32 temp;

	temp = static_cast<s32>(EmuState::pMem->ReadByte(sh2->GBR + s.r_[0]));
	temp ^= (0x000000FF & static_cast<s32>(x0nn(s)));
	EmuState::pMem->WriteByte(sh2->GBR + s.r_[0], static_cast<uint8_t>(temp));
	s.pc_ += 2;
	s.cycles_elapsed_ = 3;
}

inline void CSH2::XTRCT(Sh2& s)
{
	// Middle 32 bits of Rm and Rn -> Rn
	uint32_t temp;

	temp = (s.r_[x0n0(s)]<<16) & 0xFFFF0000;
	s.r_[xn00(s)] = (s.r_[xn00(s)]>>16) & 0x0000FFFF;
	s.r_[xn00(s)] |= temp;
	s.pc_ += 2;
	s.cycles_elapsed_ = 1;
}
