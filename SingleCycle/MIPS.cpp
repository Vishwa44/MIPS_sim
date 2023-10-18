#include<iostream>
#include<string>
#include<vector>
#include<bitset>
#include<fstream>

using namespace std;

#define ADDU (1)
#define SUBU (3)
#define AND (4)
#define OR  (5)
#define NOR (7)

#define MemSize (65536)


class RF
{
  public:
    bitset<32> ReadData1, ReadData2; 
    RF()
    { 
      Registers.resize(32);  
      Registers[0] = bitset<32> (0);  
    }

    void ReadWrite(bitset<5> RdReg1, bitset<5> RdReg2, bitset<5> WrtReg, bitset<32> WrtData, bitset<1> WrtEnable)
    {   
      /**
       * @brief Reads or writes data from/to the Register.
       *
       * This function is used to read or write data from/to the register, depending on the value of WrtEnable.
       * Put the read results to the ReadData1 and ReadData2.
       */
      // TODO: implement!
      if (WrtEnable == 0)
      {
        ReadData1 = Registers[RdReg1.to_ulong()];
        ReadData2 = Registers[RdReg2.to_ulong()];
      }
      else if (WrtEnable == 1)
      {
        Registers[WrtReg.to_ulong()] = WrtData;
        cout<<"Written register: "<<Registers[WrtReg.to_ulong()]<<endl;
      }

    }

    void OutputRF()
    {
      ofstream rfout;
      rfout.open("RFresult.txt",std::ios_base::app);
      if (rfout.is_open())
      {
        rfout<<"A state of RF:"<<endl;
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


class ALU
{
  public:
    bitset<32> ALUresult;
    bitset<32> ALUOperation (bitset<3> ALUOP, bitset<32> oprand1, bitset<32> oprand2)
    {   
      /**
       * @brief Implement the ALU operation here.
       *
       * ALU operation depends on the ALUOP, which are definded as ADDU, SUBU, etc. 
       */
      // TODO: implement!
      switch(ALUOP.to_ulong())
      {
        case ADDU:
        cout<<"Rtype ADDU"<<endl;
        return bitset<32> (oprand1.to_ulong() + oprand2.to_ulong());

        case SUBU:
        cout<<"Rtype SUBU"<<endl;
        return bitset<32> (oprand1.to_ulong() - oprand2.to_ulong());

        case AND:
        cout<<"Rtype AND"<<endl;
        return bitset<32> (oprand1 & oprand2);

        case OR:
        cout<<"Rtype OR"<<endl;
        return bitset<32> (oprand1 | oprand2);

        case NOR:
        cout<<"Rtype NOR"<<endl;
        return bitset<32> (~(oprand1 | oprand2));
        
      }

      // return bitset<32> (0);
    }            
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

    bitset<32> ReadMemory (bitset<32> ReadAddress) 
    {    
      // TODO: implement!
      /**
       * @brief Read Instruction Memory (IMem).
       *
       * Read the byte at the ReadAddress and the following three byte,
       * and return the read result. 
       */
        string totalbit = "";
        for (int i = ReadAddress.to_ulong(); i < ReadAddress.to_ulong()+4; i++)
        {
            totalbit = totalbit + IMem[i].to_string();
        }
        return bitset<32> (totalbit);
      // return Instruction;     
    }     

  private:
    vector<bitset<8>> IMem;

};

class DataMem    
{
  public:
    bitset<32> readdata;  
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
    bitset<32> MemoryAccess (bitset<32> Address, bitset<32> WriteData, bitset<1> readmem, bitset<1> writemem) 
    {    
      if (readmem == 1)
      {
        
        string totalbit = "";
        for (int i = Address.to_ulong(); i < Address.to_ulong()+4; i++)
        {
            totalbit = totalbit + DMem[i].to_string();
        }
        // cout<<"size ="<<totalbit.data();
        return (bitset<32>)totalbit;
      }
      // return readdata; 

      if (writemem == 1)
      {
        string writeDataStr = WriteData.to_string();
        int writeAddress = Address.to_ulong();
        for (int i=0; i<writeDataStr.length(); i=i+8) 
        {
          string dataByte = writeDataStr.substr(i, 8);
          cout<<"index of writedata "<<i<<endl;
          DMem[writeAddress] = bitset<8> (dataByte);
          cout<<"writing to dmem: "<<dataByte<<" at "<<writeAddress<<endl;
          writeAddress ++;
        }
        return WriteData;


      }
      // return bitset<32> (0);

    }   

    void OutputDataMem()
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



int main()
{
  RF myRF;
  ALU myALU;
  INSMem myInsMem;
  DataMem myDataMem;
  bitset<32> programCounter;
  while (1)  // TODO: implement!
  {
    int temp = programCounter.to_ulong();
    cout<<"counter: "<< programCounter<<endl;
    bitset<32> instruction = myInsMem.ReadMemory(programCounter);
    cout<<"The instruction: "<<instruction<<endl;
    if (instruction == bitset<32> (4294967295))
    {
      cout<<"HALT"<<endl;
      break;
    }
    string instructionTemp = instruction.to_string();
    if (instructionTemp.substr(0,6) == "000000")
    {
      cout<<"This is R-type"<<endl;
      bitset<5> rs (instructionTemp.substr(6,5));
      bitset<5> rt (instructionTemp.substr(11,5));
      bitset<5> rd (instructionTemp.substr(16,5));
      myRF.ReadWrite(rs, rt, bitset<5> (0), bitset<32> (0), bitset<1> (0));
      cout<<"Register at rs: " <<myRF.ReadData1<<endl;
      cout<<"Register at rt: " <<myRF.ReadData2<<endl; 
      bitset<32> calcData = myALU.ALUOperation(bitset<3>(instructionTemp.substr(29,3)), myRF.ReadData1, myRF.ReadData2);
      myRF.ReadWrite(0, 0, rd, calcData, 1);
      cout<<"Wrote to rd: "<<rd<<endl;

      temp = temp+4;
      programCounter = bitset<32> (temp);
    }
    else if(instructionTemp.substr(0,6) == "000010") 
    {
      cout<<"This is Jump J-type"<<endl;
      temp = temp+4;
      bitset<32> jumptemp = bitset<32> (temp);
      string JumpAddr = jumptemp.to_string().substr(0,4) + instructionTemp.substr(6,26) + "00";
      programCounter = bitset<32> (JumpAddr);

    }
    else
    {
      cout<<"This is I-type"<<endl;
      // decode I type
      // enum ITypeOP {100011, 101011, 001001, 000100}; //lw, sw, addiu, beq 
      if (instructionTemp.substr(0,6) == "100011")
      {
        cout<<"Load word"<<endl;
        bitset<5> rs (instructionTemp.substr(6,5));
        bitset<5> rt (instructionTemp.substr(11,5)); 
        myRF.ReadWrite(rs, rt, bitset<5> (0), bitset<32> (0), bitset<1> (0));
        cout<<"Register at rs: " <<myRF.ReadData1<<endl;
        cout<<"Register at rt: " <<myRF.ReadData2<<endl;
        cout<<"immediate: "<<instructionTemp.substr(16,16)<<endl;

        bitset<32> signeximm (instructionTemp.substr(16,16)); 
        cout<<"signeximm: "<<signeximm<<endl;

        cout<<"rt: "<<rt<<endl;
        cout<<"rs: "<<rs<<endl;
        int fromdatamem =  signeximm.to_ulong() +  myRF.ReadData1.to_ulong();
        cout<<"Target datamem: "<<fromdatamem<<endl;
        bitset<32> readdata = myDataMem.MemoryAccess(bitset<32> (fromdatamem), bitset<32> (0), bitset<1> (1), bitset<1> (0));
        cout<<"loaded from datamem: "<<readdata<<endl;
        myRF.ReadWrite(bitset<5> (0), bitset<5> (0), rt, readdata, bitset<1> (1));

        temp = temp+4;
        programCounter = bitset<32> (temp);
      }
      else if (instructionTemp.substr(0,6) == "101011")
      {
        cout<<"Store word"<<endl;
        bitset<5> rs (instructionTemp.substr(6,5));
        bitset<5> rt (instructionTemp.substr(11,5));
        bitset<32> signeximm (instructionTemp.substr(16,16)); 
        myRF.ReadWrite(rs, rt, 0, 0, 0);
        cout<<"Register at rs: " <<myRF.ReadData1<<endl;
        cout<<"Register at rt: " <<myRF.ReadData2<<endl;
        int todatamem =  signeximm.to_ulong() +  myRF.ReadData1.to_ulong();
        cout<<"Target datamem: "<<todatamem<<endl;
        // now write to main memory 
        bitset<32> writtendata = myDataMem.MemoryAccess(bitset<32> (todatamem), bitset<32> (myRF.ReadData2), bitset<1> (0), bitset<1> (1));

        temp = temp+4;
        programCounter = bitset<32> (temp);
      }
      else if (instructionTemp.substr(0,6) == "001001")
      {
        cout<<"Add imm unsigned"<<endl;
        bitset<5> rs (instructionTemp.substr(6,5));
        bitset<5> rt (instructionTemp.substr(11,5));
        bitset<32> signeximm (instructionTemp.substr(16,16)); 
        // r[rt]=r[rs] + signeximm
        myRF.ReadWrite(rs, rt, 0, 0, 0);
        cout<<"Register at rs: " <<myRF.ReadData1<<endl;
        // cout<<"Register at rt: " <<myRF.ReadData2<<endl; 
        bitset<32> calcData = myALU.ALUOperation(bitset<3>(instructionTemp.substr(3,3)), myRF.ReadData1, signeximm);
        myRF.ReadWrite(0, 0, rt, calcData, 1);
        cout<<"Wrote to rt: "<<rt<<endl;

        temp = temp+4;
        programCounter = bitset<32> (temp);
      }
      else
      {
        cout<<"Beq"<<endl;
        bitset<5> rs (instructionTemp.substr(6,5));
        bitset<5> rt (instructionTemp.substr(11,5));
        string immediate = instructionTemp.substr(16,16);
        myRF.ReadWrite(rs, rt, 0, 0, 0);
        if (myRF.ReadData1 == myRF.ReadData2)
        {
          string branchAddress = string(14, immediate[0]) + immediate + "00";
          int branchTo = (bitset<32> (branchAddress)).to_ulong();
          temp = temp + 4 + branchTo;
          programCounter = bitset<32> (temp); 
        }
        else 
        {
          temp = temp+4;
          programCounter = bitset<32> (temp);  
        }
        
      }
    }

  
    /**** You don't need to modify the following lines. ****/
    myRF.OutputRF(); // dump RF;    
  }
  myDataMem.OutputDataMem(); // dump data mem

  return 0;
}
a
