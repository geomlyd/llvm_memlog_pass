#include "llvm/Pass.h"
#include "llvm/Module.h"
#include "llvm/Support/raw_ostream.h"
#include <assert.h>
#include "llvm/Type.h"
#include "llvm/DerivedTypes.h"
#include "llvm/Constants.h"
#include "llvm/GlobalVariable.h"
#include "llvm/IRBuilder.h"
#include <string.h>
#include <stdio.h>

using namespace llvm;

namespace {
	struct MemlogPass : public ModulePass {
	static char ID;
	MemlogPass() : ModulePass(ID) {}

	virtual bool runOnModule(Module &M) {
		if(M.begin() == M.end())//empty file
			return false;
		if(!M.getTypeByName("struct._IO_FILE"))
			StructType::create(M.getContext(), "struct._IO_FILE");		
		
		//declare prototype for "fopen"
		Function* fOpenFunc = M.getFunction("fopen");
		if (!fOpenFunc){
			Type *argTypes_fOpen[] = {Type::getInt8PtrTy(M.getContext()), 
				Type::getInt8PtrTy(M.getContext())};
			Type *retType_fOpen = 
				M.getTypeByName("struct._IO_FILE")->getPointerTo();
			FunctionType *fOpenFuncType = FunctionType::get(retType_fOpen, 
				ArrayRef<Type*>(argTypes_fOpen, 2), false);			
			fOpenFunc = Function::Create(fOpenFuncType, GlobalValue::ExternalLinkage,
				"fopen", &M);
			fOpenFunc->setCallingConv(CallingConv::C);
		}		
				
		//declare prototype for "fclose"
		Function *fCloseFunc = M.getFunction("fclose");
		if (!fCloseFunc){
			Type *argTypes_fClose[] = {
				M.getTypeByName("struct._IO_FILE")->getPointerTo()};
			Type *retType_fClose = Type::getInt32Ty(M.getContext());
			FunctionType *fCloseFuncType = FunctionType::get(retType_fClose, 
				ArrayRef<Type*>(argTypes_fClose, 1), false);
			fCloseFunc = Function::Create(fCloseFuncType, GlobalValue::ExternalLinkage,
				"fclose", &M);
			fCloseFunc->setCallingConv(CallingConv::C);
		}		
				
		//declare prototype for "malloc"
		Function *mallocFunc = M.getFunction("malloc");
		if (!mallocFunc){		
			Type *argTypes_malloc[] = {Type::getInt32Ty(M.getContext())};
			Type *retType_malloc = Type::getInt8PtrTy(M.getContext());
			FunctionType *mallocFuncType = FunctionType::get(retType_malloc, 
				ArrayRef<Type*>(argTypes_malloc, 1), false);
			mallocFunc = Function::Create(mallocFuncType, GlobalValue::ExternalLinkage,
				"malloc", &M);
			mallocFunc->setCallingConv(CallingConv::C);
		}
		
		//declare prototype for "free"
		Function *freeFunc = M.getFunction("free");
		if (!freeFunc){		
			Type *argTypes_free[] = {Type::getInt8PtrTy(M.getContext())};
			Type *retType_free = Type::getVoidTy(M.getContext());
			FunctionType *freeFuncType = FunctionType::get(retType_free, 
				ArrayRef<Type*>(argTypes_free, 1), false);
			freeFunc = Function::Create(freeFuncType, GlobalValue::ExternalLinkage,
				"free", &M);
			freeFunc->setCallingConv(CallingConv::C);
		}		
		
		//declare prototype for "fprintf"
		Function *fprintfFunc = M.getFunction("fprintf");
		if(!fprintfFunc){
			Type *argTypes_fprintf[] = {M.getTypeByName("struct._IO_FILE")->getPointerTo(), 
				Type::getInt8Ty(M.getContext())->getPointerTo()};
			Type *retType_fprintf = Type::getInt32Ty(M.getContext());
			FunctionType *fprintfFuncType = FunctionType::get(retType_fprintf, 
				ArrayRef<Type*>(argTypes_fprintf, 2), true);
			fprintfFunc = Function::Create(fprintfFuncType, GlobalValue::ExternalLinkage,
				"fprintf", &M);
			fprintfFunc->setCallingConv(CallingConv::C);
		}

		IRBuilder<> builder(M.getContext());	
		GlobalVariable* memlogFilePointer = NULL;
		Value *mallocPrintString = NULL, *freePrintString = NULL;
		LoadInst *loadedFp = NULL;
		
		if(M.getFunction("main") != NULL && M.begin() != M.end()){
			BasicBlock *firstBlock = M.getFunction("main")->begin();
			builder.SetInsertPoint(firstBlock, firstBlock->begin());
			
			//create strings necessary for fopen
			Value *outputFileString = builder.CreateGlobalStringPtr(
					"memlog.txt", "$fileNameStr");
			Value *fileModeString = 
				builder.CreateGlobalStringPtr("w", "$fileModeStr");			
			
			//create the global file pointer variable
			memlogFilePointer = new GlobalVariable(M, 
			M.getTypeByName("struct._IO_FILE")->getPointerTo(),
			/*isConstant=*/false, GlobalValue::ExternalLinkage,
			/*Initializer=*/NULL, "$memlogFilePointer");
			memlogFilePointer->setAlignment(4);	
			memlogFilePointer->setInitializer(ConstantPointerNull::get
				(M.getTypeByName("struct._IO_FILE")->getPointerTo()));	
				
			//create the call to fopen
			std::vector<Value*> fOpenArgs; 
			fOpenArgs.push_back(outputFileString);
			fOpenArgs.push_back(fileModeString);
			CallInst *fOpenCall = builder.CreateCall(fOpenFunc, fOpenArgs, "callfopen");
			builder.CreateStore(fOpenCall, memlogFilePointer, false);			
		}
		else if(M.begin() != M.end()){
			BasicBlock *firstBlock = M.getFunctionList().begin()->begin();
			builder.SetInsertPoint(firstBlock, firstBlock->begin());
			memlogFilePointer = new GlobalVariable(M, 
			M.getTypeByName("struct._IO_FILE")->getPointerTo(),
			/*isConstant=*/false, GlobalValue::ExternalLinkage,
			/*Initializer=*/NULL, "$memlogFilePointer");
			memlogFilePointer->setAlignment(4);			
		}
		//create the necessary strings
		mallocPrintString =
			builder.CreateGlobalStringPtr("Malloc %d bytes at loc %p\n",
				"$mallocPrintStr");
		freePrintString =
			builder.CreateGlobalStringPtr("Free'd memory at loc %p\n",
				"$freePrintStr");		
		
		for(Module::iterator F = M.begin(); F != M.end(); ++F){
			for(Function::iterator b = F->begin(); b != F->end(); ++b){
				BasicBlock *currBlock = b;
				for(BasicBlock::iterator i = currBlock->begin(); i != currBlock->end(); ++i){
					Instruction *currInstr = i;
					//exit point of main
					if(F->getName() == "main" && i->getOpcode() == Instruction::Ret){
						builder.SetInsertPoint(currBlock, currInstr);
						//before each return statement, insert a call to fclose
						loadedFp = builder.CreateLoad(memlogFilePointer, 
							"$filePtrLoaded");
						std::vector<Value*> fCloseArgs; 
						fCloseArgs.push_back(loadedFp);								
						CallInst *fCloseCall = builder.CreateCall(fCloseFunc, 
							fCloseArgs, "callfclose");
					}
					else if(i->getOpcode() == Instruction::Call){
						CallInst *currCall = (CallInst *)currInstr;
						Function *currFunc = currCall->getCalledFunction();
						if(currFunc == mallocFunc){
							//safe insertion, malloc call can't be a terminator
							builder.SetInsertPoint(currBlock, currInstr->getNextNode());
							//create a call to fprintf
							Value *mallocSize = currCall->getArgOperand(0);
							loadedFp = builder.CreateLoad(memlogFilePointer, 
								"$filePtrLoaded");
							std::vector<Value*> fprintfArgs;
							fprintfArgs.push_back(loadedFp);
							fprintfArgs.push_back(mallocPrintString);
							fprintfArgs.push_back(mallocSize);
							fprintfArgs.push_back(currCall); //malloc result (loc.)
							CallInst *fprintfCall = builder.CreateCall(fprintfFunc,
								fprintfArgs, "callfprintf");
						}
						else if(currFunc == freeFunc){
							//safe insertion, free call can't be a terminator
							builder.SetInsertPoint(currBlock, currInstr->getNextNode());
							//create a call to fprintf
							loadedFp = builder.CreateLoad(memlogFilePointer, 
								"$filePtrLoaded");							
							std::vector<Value*> fprintfArgs;
							fprintfArgs.push_back(loadedFp);
							fprintfArgs.push_back(freePrintString);
							//free location
							fprintfArgs.push_back(currCall->getArgOperand(0));
							CallInst *fprintfCall = builder.CreateCall(fprintfFunc,
								fprintfArgs, "callfprintf");							
						}
					}
				}
			}	
		}

		return true;
	}
	};
}

char MemlogPass::ID = 0;
static RegisterPass<MemlogPass> X("memlog", "Memory Log Pass", false, false);