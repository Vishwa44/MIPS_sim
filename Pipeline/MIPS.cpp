#include<iostream>
#include<string>
#include<vector>
#include<bitset>
#include<fstream>
using namespace std;

#define MemSize 1000 // memory size, in reality, the memory size should be 2^32, but for this lab csa23, for the space resaon, we keep it as this large number, but the memory is still 32-bit addressable.

struct IFStruct {
    bitset<32>  PC;
    bool        nop;  
};

struct IDStruct {
    bitset<32>  Instr;
    bool        nop;  
};

struct EXStruct {
    bitset<32>  Read_data1;
    bitset<32>  Read_data2;
    bitset<16>  Imm;
    bitset<5>   Rs;
    bitset<5>   Rt;
    bitset<5>   Wrt_reg_addr;
    bool        is_I_type;
    bool        rd_mem;
    bool        wrt_mem; 
    bool        alu_op;     //1 for addu, lw, sw, 0 for subu 
    bool        wrt_enable;
    bool        nop;  
};

struct MEMStruct {
    bitset<32>  ALUresult;
    bitset<32>  Store_data;
    bitset<5>   Rs;
    bitset<5>   Rt;    
    bitset<5>   Wrt_reg_addr;
    bool        rd_mem;
    bool        wrt_mem; 
    bool        wrt_enable;    
    bool        nop;    
};

struct WBStruct {
    bitset<32>  Wrt_data;
    bitset<5>   Rs;
    bitset<5>   Rt;     
    bitset<5>   Wrt_reg_addr;
    bool        wrt_enable;
    bool        nop;     
};

struct stateStruct {
    IFStruct    IF;
    IDStruct    ID;
    EXStruct    EX;
    MEMStruct   MEM;
    WBStruct    WB;
};

class RF
{
    public: 
        bitset<32> Reg_data;
     	RF()
    	{ 
			Registers.resize(32);  
			Registers[0] = bitset<32> (0);  
        }
	
        bitset<32> readRF(bitset<5> Reg_addr)
        {   
            Reg_data = Registers[Reg_addr.to_ulong()];
            return Reg_data;
        }
    
        void writeRF(bitset<5> Reg_addr, bitset<32> Wrt_reg_data)
        {
            Registers[Reg_addr.to_ulong()] = Wrt_reg_data;
        }
		 
		void outputRF()
		{
			ofstream rfout;
			rfout.open("RFresult.txt",std::ios_base::app);
			if (rfout.is_open())
			{
				rfout<<"State of RF:\t"<<endl;
				for (int j = 0; j<32; j++)
				{        
					rfout << Registers[j]<<endl;
				}
			}
			else cout<<"Unable to open file";
			rfout.close();               
		} 
			
	private:
		vector<bitset<32> >Registers;	
};

class INSMem
{
	public:
        bitset<32> Instruction;
        INSMem()
        {       
			IMem.resize(MemSize); 
            ifstream imem;
			string line;
			int i=0;
			imem.open("imem.txt");
			if (imem.is_open())
			{
				while (getline(imem,line))
				{      
					IMem[i] = bitset<8>(line);
					i++;
				}                    
			}
            else cout<<"Unable to open file";
			imem.close();                     
		}
                  
		bitset<32> readInstr(bitset<32> ReadAddress) 
		{    
			string insmem;
			insmem.append(IMem[ReadAddress.to_ulong()].to_string());
			insmem.append(IMem[ReadAddress.to_ulong()+1].to_string());
			insmem.append(IMem[ReadAddress.to_ulong()+2].to_string());
			insmem.append(IMem[ReadAddress.to_ulong()+3].to_string());
			Instruction = bitset<32>(insmem);		//read instruction memory
			return Instruction;     
		}     
      
    private:
        vector<bitset<8> > IMem;     
};
      
class DataMem    
{
    public:
        bitset<32> ReadData;  
        DataMem()
        {
            DMem.resize(MemSize); 
            ifstream dmem;
            string line;
            int i=0;
            dmem.open("dmem.txt");
            if (dmem.is_open())
            {
                while (getline(dmem,line))
                {      
                    DMem[i] = bitset<8>(line);
                    i++;
                }
            }
            else cout<<"Unable to open file";
                dmem.close();          
        }
		
        bitset<32> readDataMem(bitset<32> Address)
        {	
			string datamem;
            datamem.append(DMem[Address.to_ulong()].to_string());
            datamem.append(DMem[Address.to_ulong()+1].to_string());
            datamem.append(DMem[Address.to_ulong()+2].to_string());
            datamem.append(DMem[Address.to_ulong()+3].to_string());
            ReadData = bitset<32>(datamem);		//read data memory
            return ReadData;               
		}
            
        void writeDataMem(bitset<32> Address, bitset<32> WriteData)            
        {
            DMem[Address.to_ulong()] = bitset<8>(WriteData.to_string().substr(0,8));
            DMem[Address.to_ulong()+1] = bitset<8>(WriteData.to_string().substr(8,8));
            DMem[Address.to_ulong()+2] = bitset<8>(WriteData.to_string().substr(16,8));
            DMem[Address.to_ulong()+3] = bitset<8>(WriteData.to_string().substr(24,8));  
        }   
                     
        void outputDataMem()
        {
            ofstream dmemout;
            dmemout.open("dmemresult.txt");
            if (dmemout.is_open())
            {
                for (int j = 0; j< 1000; j++)
                {     
                    dmemout << DMem[j]<<endl;
                }
                     
            }
            else cout<<"Unable to open file";
            dmemout.close();               
        }             
      
    private:
		vector<bitset<8> > DMem;      
};  


void printState(stateStruct state, int cycle)
{
    ofstream printstate;
    printstate.open("stateresult.txt", std::ios_base::app);
    if (printstate.is_open())
    {
        printstate<<"State after executing cycle:\t"<<cycle<<endl; 
        
        printstate<<"IF.PC:\t"<<state.IF.PC.to_ulong()<<endl;        
        printstate<<"IF.nop:\t"<<state.IF.nop<<endl; 
        
        printstate<<"ID.Instr:\t"<<state.ID.Instr<<endl; 
        printstate<<"ID.nop:\t"<<state.ID.nop<<endl;
        
        printstate<<"EX.Read_data1:\t"<<state.EX.Read_data1<<endl;
        printstate<<"EX.Read_data2:\t"<<state.EX.Read_data2<<endl;
        printstate<<"EX.Imm:\t"<<state.EX.Imm<<endl; 
        printstate<<"EX.Rs:\t"<<state.EX.Rs<<endl;
        printstate<<"EX.Rt:\t"<<state.EX.Rt<<endl;
        printstate<<"EX.Wrt_reg_addr:\t"<<state.EX.Wrt_reg_addr<<endl;
        printstate<<"EX.is_I_type:\t"<<state.EX.is_I_type<<endl; 
        printstate<<"EX.rd_mem:\t"<<state.EX.rd_mem<<endl;
        printstate<<"EX.wrt_mem:\t"<<state.EX.wrt_mem<<endl;        
        printstate<<"EX.alu_op:\t"<<state.EX.alu_op<<endl;
        printstate<<"EX.wrt_enable:\t"<<state.EX.wrt_enable<<endl;
        printstate<<"EX.nop:\t"<<state.EX.nop<<endl;        

        printstate<<"MEM.ALUresult:\t"<<state.MEM.ALUresult<<endl;
        printstate<<"MEM.Store_data:\t"<<state.MEM.Store_data<<endl; 
        printstate<<"MEM.Rs:\t"<<state.MEM.Rs<<endl;
        printstate<<"MEM.Rt:\t"<<state.MEM.Rt<<endl;   
        printstate<<"MEM.Wrt_reg_addr:\t"<<state.MEM.Wrt_reg_addr<<endl;              
        printstate<<"MEM.rd_mem:\t"<<state.MEM.rd_mem<<endl;
        printstate<<"MEM.wrt_mem:\t"<<state.MEM.wrt_mem<<endl; 
        printstate<<"MEM.wrt_enable:\t"<<state.MEM.wrt_enable<<endl;         
        printstate<<"MEM.nop:\t"<<state.MEM.nop<<endl;        

        printstate<<"WB.Wrt_data:\t"<<state.WB.Wrt_data<<endl;
        printstate<<"WB.Rs:\t"<<state.WB.Rs<<endl;
        printstate<<"WB.Rt:\t"<<state.WB.Rt<<endl;        
        printstate<<"WB.Wrt_reg_addr:\t"<<state.WB.Wrt_reg_addr<<endl;
        printstate<<"WB.wrt_enable:\t"<<state.WB.wrt_enable<<endl;        
        printstate<<"WB.nop:\t"<<state.WB.nop<<endl; 
    }
    else cout<<"Unable to open file";
    printstate.close();
}

void IDEXupdate(stateStruct &state, RF &myRF, bitset<32> instructTemp, bool &BNE_Flag, stateStruct &oldstate)
{

string instructTempSTR = instructTemp.to_string();
cout<<"In IDEXupdate: "<<instructTempSTR<<endl;
bitset<5> rs  (instructTempSTR.substr(6,5));
state.EX.Rs = rs;
bitset<5> rt (instructTempSTR.substr(11,5));
state.EX.Rt = rt;
state.EX.Read_data1 = myRF.readRF(rs);

state.EX.Read_data2 = myRF.readRF(rt);

bitset<5> rd (instructTempSTR.substr(16,5));
bitset<16> imm (instructTempSTR.substr(16,16));
state.EX.Imm = imm;
string opcode = instructTempSTR.substr(0,6);
if (opcode != "000000")
{
    state.EX.is_I_type = 1;
     if (opcode == "100011")
     {
        // load word
        state.EX.rd_mem = 1;
        state.EX.wrt_mem = 0;
        state.EX.alu_op = 1;
        state.EX.wrt_enable = 1;
        state.EX.Wrt_reg_addr = rt;
     }
     else if (opcode == "101011")
     {
        // store word
        state.EX.rd_mem = 0;
        state.EX.wrt_mem = 1;
        state.EX.alu_op = 1;    
        state.EX.wrt_enable = 0;

     }
    else if (opcode == "000101")
     {
        // branch on not equal
        cout<<"BNE"<<endl;
        if (state.EX.Read_data1 != state.EX.Read_data2)
        {
            cout<<"Branch Not Equal"<<endl;
            BNE_Flag = 1;
            string branchAddress = string(14, imm.to_string()[0]) + imm.to_string() + "00";
            int branchTo = (bitset<32> (branchAddress)).to_ulong();
            int temp = oldstate.IF.PC.to_ullong() +  branchTo;
            cout<<"Branching to PC: "<<temp<<endl;
            oldstate.IF.PC = bitset<32> (temp);
        }
        state.EX.rd_mem = 0;
        state.EX.wrt_mem = 0;
        state.EX.wrt_enable = 0;
        

     }
}
else
{
    state.EX.is_I_type = 0;
    state.EX.rd_mem = 0;
    state.EX.wrt_mem = 0;
    string func = instructTempSTR.substr(29,3);
    if (func == "011")
    {
        // subu
        state.EX.alu_op = 0;
        state.EX.Wrt_reg_addr = rd;
        state.EX.wrt_enable = 1;
    }
    else if (func == "001")
    {
        // addu
        state.EX.alu_op = 1;
        state.EX.Wrt_reg_addr = rd;
        state.EX.wrt_enable = 1;
    }
}
state.EX.nop = 0;
cout<<"in IDEXupdate: "<<instructTemp<<endl;
}


void EXMEMupdate (stateStruct &state, stateStruct &newState, int cycle) 
{
    cout<<"In EXMEMupdate"<<endl;
    newState.MEM.Rs = state.EX.Rs;
    newState.MEM.Rt = state.EX.Rt;
    newState.MEM.rd_mem = state.EX.rd_mem;
    newState.MEM.wrt_mem = state.EX.wrt_mem;
    newState.MEM.wrt_enable = state.EX.wrt_enable;
    newState.MEM.Wrt_reg_addr = state.EX.Wrt_reg_addr;

    // check for ADD-ADD dependency

    if (newState.MEM.Rs == state.MEM.Wrt_reg_addr && newState.MEM.Rs != bitset<5> (0))
     {
        cout<<"FORWARDING: newState.MEM.Rs == state.MEM.Wrt_reg_addr"<<endl;
        cout<<newState.MEM.Rs<<endl;
        cout<<state.MEM.Wrt_reg_addr<<endl;
        state.EX.Read_data1 = state.MEM.ALUresult;}
    if  (newState.MEM.Rt == state.MEM.Wrt_reg_addr && newState.MEM.Rt != bitset<5> (0))
     {  cout<<newState.MEM.Rt<<endl;
        cout<<state.MEM.Wrt_reg_addr<<endl;
        cout<<"FORWARDING: newState.MEM.Rt == state.MEM.Wrt_reg_addr"<<endl;
        state.EX.Read_data2 = state.MEM.ALUresult;
    }

    // Check for LW-ADD dependency

    if (newState.MEM.Rs == state.WB.Wrt_reg_addr && newState.MEM.Rs != bitset<5> (0))
        {
            cout<<newState.MEM.Rs<<endl;
            cout<<state.WB.Wrt_reg_addr<<endl;
            cout<<"FORWARDING: newState.MEM.Rs == state.WB.Wrt_reg_addr"<<endl;
            state.EX.Read_data1 = state.WB.Wrt_data;
        }
    if (newState.MEM.Rt == state.WB.Wrt_reg_addr && newState.MEM.Rt != bitset<5> (0))
        {
            cout<<newState.MEM.Rt<<endl;
            cout<<state.WB.Wrt_reg_addr<<endl;
            cout<<"FORWARDING: newState.MEM.Rt == state.WB.Wrt_reg_addr"<<endl;
            state.EX.Read_data2 = state.WB.Wrt_data;
        }


    if (state.EX.is_I_type) 
    {
        // check for ADD-LW dependency
        string imm = state.EX.Imm.to_string().substr(); 
        bitset<32> extendedImm = bitset<32> (string(16, imm[0]) + imm);
        newState.MEM.ALUresult = bitset<32> (extendedImm.to_ullong() + state.EX.Read_data1.to_ullong());

        if (state.EX.wrt_mem) 
        {
            cout<<" EXMEMupdate in state.EX.wrt_mem"<<endl;
            cout<<state.EX.Read_data2<<endl;
            newState.MEM.Store_data = state.EX.Read_data2;
        }
        
    }
    else 
    {
        if (state.EX.alu_op == 1)
        {
            newState.MEM.ALUresult = bitset<32> (state.EX.Read_data1.to_ullong() + state.EX.Read_data2.to_ullong());
        }
        else if (state.EX.alu_op == 0) 
        {
            newState.MEM.ALUresult = bitset<32> (state.EX.Read_data1.to_ullong() - state.EX.Read_data2.to_ullong());
        }
    }
    newState.MEM.nop = 0;
}

void MEMWBupdate (stateStruct &state, stateStruct &newState, DataMem &myDataMem) 
{
    cout<<"In MEMWBupdate"<<endl;
    newState.WB.Rs = state.MEM.Rs;
    newState.WB.Rt = state.MEM.Rt;
    newState.WB.wrt_enable = state.MEM.wrt_enable;
    newState.WB.Wrt_reg_addr = state.MEM.Wrt_reg_addr;
    newState.WB.Wrt_data = state.MEM.ALUresult;
    newState.WB.nop = 0;
    if (state.MEM.rd_mem == 1)
    {
        cout<<"In state.MEM.rd_mem == 1"<<endl;
        cout<<state.MEM.ALUresult<<endl;
        newState.WB.Wrt_data = myDataMem.readDataMem(state.MEM.ALUresult);

    }
    if (state.MEM.wrt_mem == 1)
    {
        cout<<"in state.MEM.wrt_mem == 1"<<endl;
        cout<<state.MEM.ALUresult<<endl;
        cout<<state.MEM.Store_data<<endl;
        myDataMem.writeDataMem(state.MEM.ALUresult, state.MEM.Store_data);
    }

}

void WBexecutie (stateStruct &state, stateStruct &newState, RF &myRF)
{
    cout<<"In WBexecutie"<<endl;

    if (state.WB.wrt_enable == 1)
    {
        cout<<"writing to "<<state.WB.Wrt_reg_addr<<endl;
        cout<<"writing "<<state.WB.Wrt_data<<endl;
        myRF.writeRF(state.WB.Wrt_reg_addr, state.WB.Wrt_data);
    }
} 
void checkconflict (stateStruct &newState, bool &Stall)
{
    cout<<"Checking for stall req"<<endl;
    if (newState.EX.rd_mem == 1 && newState.EX.is_I_type == 1)
    {
        string instructTempSTR = newState.ID.Instr.to_string();
        bitset<5> rs  (instructTempSTR.substr(6,5));
        bitset<5> rt (instructTempSTR.substr(11,5));
        if (newState.EX.Rt == rs or newState.EX.Rt == rt)
        {
            Stall = 1;
            newState.IF.nop = 1;
            newState.ID.nop = 1;
            cout<<"STALL needed"<<endl;
        }

    }
}

int main() 
{
    
    RF myRF;
    INSMem myInsMem;
    DataMem myDataMem;
    stateStruct state;
    int cycle = 0;
    state.IF.PC = bitset<32> (0);
    state.IF.nop =	0;
    state.ID.nop=	0;
    state.EX.nop=	1;
    state.MEM.nop=	1;
    state.WB.nop=	1;
    bool HALT = 0;
    bool BNE_Flag = 0;
    bool Stall = 0;
    while (1) {
        cout<<"STALL: "<<Stall<<endl;
        stateStruct newState;
        newState.EX.wrt_enable = 0;
         cout<<"in loop cycle:"<<cycle<<endl;
        /* --------------------- WB stage --------------------- */
        if (state.WB.nop == 0) 
        {WBexecutie(state, newState, myRF);
        if (state.IF.nop == 1 && state.IF.nop == 1 && state.EX.nop == 1 && state.MEM.nop == 1 && HALT == 1)
        {
            newState.WB.nop = 1;
        }}


        /* --------------------- MEM stage --------------------- */
        if (state.MEM.nop == 0) 
        {MEMWBupdate(state, newState, myDataMem);
        if (state.IF.nop == 1 && state.IF.nop == 1 && state.EX.nop == 1 && HALT == 1)
        {
            newState.MEM.nop = 1;
        }}


        /* --------------------- EX stage --------------------- */
        if (state.EX.nop == 0) 
        {EXMEMupdate(state, newState, cycle);
        if (state.IF.nop == 1 && state.IF.nop == 1 && HALT == 1)
        {
            newState.EX.nop = 1;
        }}
          

        /* --------------------- ID stage --------------------- */
        if (state.ID.nop == 0) 
        {IDEXupdate(newState, myRF, state.ID.Instr, BNE_Flag, state);}
        
        /* --------------------- IF stage --------------------- */
        if (state.IF.nop==0) 
        {newState.ID.Instr = myInsMem.readInstr(state.IF.PC);
        newState.ID.nop = 0;
        cout<<"in IF: "<<newState.ID.Instr<<endl; 
        checkconflict(newState, Stall);

        if (newState.ID.Instr == bitset<32> (4294967295))
        {
            newState.IF.nop = 1;
            newState.ID.nop = 1;
            HALT = 1;
        }
        else
        {
            if (BNE_Flag == 0)
            {state.IF.PC = bitset<32> (state.IF.PC.to_ulong() + 4);}
            else
            {BNE_Flag = 0;}
            newState.IF.PC = state.IF.PC;
            cout<<"the PC: "<<newState.IF.PC.to_ullong()<<endl;
        }       
        }
        else
        {
            if (Stall == 1)
            {
                cout<<"Stall updated"<<endl; 
                newState.IF.nop = 0;
                Stall = 0;
                newState.IF.PC = state.IF.PC;
                newState.ID.Instr = state.ID.Instr;
                newState.ID.nop = 0;
            }
        }

        // if (Stall == 1)
        // {
        //     cout<<"Stall updated"<<endl;
        //     newState.IF.nop = 0;
        //     newState.ID.nop = 0;
        //     Stall = 0 ;}
        // else
        if (state.IF.nop && state.ID.nop && state.EX.nop && state.MEM.nop && state.WB.nop)
            break;
        
        printState(newState, cycle); //print states after executing cycle 0, cycle 1, cycle 2 ... 
       
        state = newState; /*** The end of the cycle and updates the current state with the values calculated in this cycle. csa23 ***/ 
        cycle++;
    }
    
    myRF.outputRF(); // dump RF;	
	myDataMem.outputDataMem(); // dump data mem 
	
	return 0;
}
